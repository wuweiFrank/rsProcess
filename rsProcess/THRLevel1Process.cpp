#include"stdafx.h"
#include"Level1Process.h"
#include<io.h>
#include<fstream>
using namespace std;

//===========================================================================================================================================
//接口函数
long THRLevel1Process::Level1Proc_SpectarlCalibTIR(const char* hyperModtran, const char* bandInfo, const char* fileImg,
	const char* plank_line_coeff, float bin, float begWaveLength, float endWaveLength,
	const char* pathShift)
{
	if (endWaveLength<begWaveLength)
		return -1;
	if (bin <= 0)
		return -1;

	//获取影像信息
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALAllRegister();
	GDALDataset* m_dataset = (GDALDataset*)GDALOpen(fileImg, GA_ReadOnly);
	if (m_dataset == NULL)
		return -1;

	int bandsNumber = 256;
	int imgSamples, imgLines;
	imgSamples = m_dataset->GetRasterXSize();
	imgLines = m_dataset->GetRasterYSize();
	bandsNumber = m_dataset->GetRasterCount();
	int imgSelLine = 300;	//减小数据量只读取300行

	//数据空间计算
	int n_bin = int(((endWaveLength - begWaveLength) / bin) + 1);
	long lError = 0;

	FILE* fModtrans = NULL;
	FILE* fBandInfo = NULL;

	//所有数据空间的申请
	float* nWLens = NULL;
	float *hyperSpectral = NULL;
	float *band_waveLength = NULL;
	float *band_FWHM = NULL;
	float **img_data = NULL;
	float **optParam = NULL;
	float *plank_coef = NULL;
	try
	{
		nWLens = new float[n_bin];				  //MODTRAN 模拟光谱每个波段的波长
		for (int i = 0; i<n_bin; i++)
			nWLens[i] = i*bin + begWaveLength;
		hyperSpectral = new float[n_bin];		  //MODTRAN 模拟光谱的辐亮度
		band_waveLength = new float[bandsNumber];//影像实验室定标的中心波长
		band_FWHM = new float[bandsNumber];	  //影像实验室定标的半波宽
		img_data = new float*[bandsNumber];	  //影像数据空间
		for (int i = 0; i<bandsNumber; i++)
			img_data[i] = new float[imgSamples*imgSelLine];
		optParam = new float*[imgSamples];	   //解算得到的光谱定标参数（每一列有一个定标参数）
		for (int i = 0; i<imgSamples; i++)
			optParam[i] = new float[2];
		plank_coef = new float[2 * bandsNumber];	   //光谱辐射校正普朗克校正系数

	}
	catch (bad_alloc* e)
	{
		lError = -1;
		goto Err_End;
	}

	int iterator_param1 = 0;
	char szTemp[1024];

	//读取MODTRAN模拟的反射率和辐亮度
	if (fopen_s(&fModtrans, hyperModtran, "r")!=0)
	{
		lError = -1;
		goto Err_End;
	}
	//跳过11行
	for (int i = 0; i < 11; i++)
		fgets(szTemp, 1024, fModtrans);
		//ifs.getline(szTemp, 1024);

	//读取数据
	float dataHyper[16];
	for (int i = 0; i<n_bin; i++)
	{
		fscanf_s(fModtrans, "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f", &dataHyper[0], &dataHyper[1], &dataHyper[2], &dataHyper[3], &dataHyper[4],
			&dataHyper[5], &dataHyper[6], &dataHyper[7], &dataHyper[8], &dataHyper[9],
			&dataHyper[10], &dataHyper[11], &dataHyper[12], &dataHyper[13], &dataHyper[14],
			&dataHyper[15]);
		hyperSpectral[i] = dataHyper[8] * 1000.0f;
	}
	fclose(fModtrans); fModtrans = NULL;


	//读取中心波长信息和半波宽
	iterator_param1 = 0;
	if (fopen_s(&fBandInfo, bandInfo, "r") != 0)
	{
		lError = -1;
		goto Err_End;
	}
	do
	{
		fscanf_s(fBandInfo, "%f%f", &band_waveLength[iterator_param1], &band_FWHM[iterator_param1]);
		iterator_param1++;
	} while (!feof(fBandInfo) && iterator_param1<bandsNumber);
	fclose(fBandInfo); fBandInfo = NULL;

	if (iterator_param1 != bandsNumber)
	{
		lError = -1;
		goto Err_End;
	}

	//读取普朗克线性化系数
	//跳过第一行
	FILE *fPlank = NULL;
	if (fopen_s(&fPlank, plank_line_coeff,"r")!=0)
	{
		lError = -1;
		goto Err_End;
	}
	fgets(szTemp, 1024, fPlank);
	for (int i = 0; i<bandsNumber; i++)
	{
		fscanf_s(fPlank, "%f%f", &plank_coef[2 * i + 0], &plank_coef[2 * i + 1]);
	}
	fclose(fPlank); fPlank = NULL;

	//大气吸收峰
	float wlen_min = 11.65;		//自己定义的水汽吸收峰
	float wlen_max = 11.825;

	int index_min, index_max;
	int sel_number_band;		//大气吸收峰的波段数目
	for (int i = 0; i<bandsNumber; i++)
	{
		if (band_waveLength[i] <= wlen_max)
			index_max = i;
		if (band_waveLength[i] <= wlen_min)
			index_min = i;
	}
	index_min++;

	//读取影像数据
	for (int i = 0; i<bandsNumber; i++)
		GDALRasterIO(GDALGetRasterBand(m_dataset, i + 1), GF_Read, 0, imgLines - imgSelLine, imgSamples,
			imgSelLine, img_data[i], imgSamples, imgSelLine, GDT_Float32, 0, 0);
	GDALClose((GDALDatasetH)m_dataset);

	//转换为辐亮度
	for (int i = 0; i<bandsNumber; i++)
	{
		for (int j = 0; j<imgSamples*imgSelLine; j++)
			img_data[i][j] = img_data[i][j] * plank_coef[2 * i + 1] + plank_coef[2 * i + 0];
	}

	//针对每一列进行光谱定标
	//每一列的光谱定标数据	
	int totalproc = 0;
#pragma omp parallel for
	for (int j = 0; j<imgSamples; j++)
	{
		totalproc++;
		float *spectral_slice = new float[bandsNumber];  //用做处理的列均值光谱
		memset(spectral_slice, 0, sizeof(float)*bandsNumber);

		printf("\rprocess sample %d", totalproc);
		for (int i = 0; i<bandsNumber; i++)
		{
			for (int k = 0; k<imgSelLine; k++)
				spectral_slice[i] += img_data[i][k*imgSamples + j];
		}
		for (int i = 0; i<bandsNumber; i++)
			spectral_slice[i] /= imgSelLine;

		Level1Proc_Powell(optParam[j], hyperSpectral, nWLens, n_bin, band_waveLength, band_FWHM, spectral_slice, bandsNumber, index_min, index_max);
		delete[]spectral_slice;
	}
	printf("\n");

	FILE* fShift = NULL;
	fopen_s(&fShift, pathShift, "w");
	for (int i = 0; i < imgSamples; i++)
		fprintf(fShift, "%lf  %lf\n", optParam[i][0], optParam[i][1]);
	fclose(fShift); fShift = NULL;

	//清除指针并退出
Err_End:
	if (nWLens != NULL)
		delete[]nWLens;
	if (hyperSpectral != NULL)
		delete[]hyperSpectral;
	if (band_waveLength != NULL)
		delete[]band_waveLength;
	if (band_FWHM != NULL)
		delete[]band_FWHM;
	if (img_data != NULL)
	{
		for (int i = 0; i<bandsNumber; i++)
			delete[]img_data[i];
		delete[]img_data;
	}

	if (optParam != NULL)
	{
		for (int i = 0; i<imgSamples; i++)
			delete[]optParam[i];
		delete[]optParam;
	}
	if (plank_coef != NULL)
		delete[]plank_coef;
	return lError;
}
long THRLevel1Process::Level1Proc_SpectarlCalibTIRSample(const char* hyperModtran, const char* bandInfo, const char* fileImg,
	const char* plank_line_coeff, float bin, float begWaveLength,
	float endWaveLength, int sampleNum, const char* pathShift)
{
	if (endWaveLength<begWaveLength)
		return -1;
	if (bin <= 0)
		return -1;

	//获取影像信息
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALAllRegister();
	GDALDataset* m_dataset = (GDALDataset*)GDALOpen(fileImg, GA_ReadOnly);
	if (m_dataset == NULL)
		return -1;

	int bandsNumber = 256;
	int imgSamples, imgLines;
	imgSamples = m_dataset->GetRasterXSize();
	imgLines = m_dataset->GetRasterYSize();
	bandsNumber = m_dataset->GetRasterCount();
	int imgSelLine = 300;	//减小数据量只读取300行

							//数据空间计算
	int n_bin = int(((endWaveLength - begWaveLength) / bin) + 1);
	long lError = 0;
	int sample_interval = int(imgSamples / sampleNum);			//采样间隔

	FILE* fModtrans = NULL;
	FILE* fBandInfo = NULL;

	//所有数据空间的申请
	float *nWLens = NULL;
	float *hyperSpectral = NULL;
	float *band_waveLength = NULL;
	float *band_FWHM = NULL;
	float **img_data = NULL;
	float **optParam = NULL;
	float *plank_coef = NULL;
	try
	{
		nWLens = new float[n_bin];					//MODTRAN 模拟光谱每个波段的波长
		for (int i = 0; i<n_bin; i++)
			nWLens[i] = i*bin + begWaveLength;
		hyperSpectral = new float[n_bin];				//MODTRAN 模拟光谱的辐亮度
		band_waveLength = new float[bandsNumber];		//影像实验室定标的中心波长
		band_FWHM = new float[bandsNumber];			//影像实验室定标的半波宽
		img_data = new float*[bandsNumber];			//影像数据空间
		for (int i = 0; i<bandsNumber; i++)
			img_data[i] = new float[imgSamples*imgSelLine];
		optParam = new float*[sampleNum];			//解算得到的光谱定标参数（每一列有一个定标参数）
		for (int i = 0; i<sampleNum; i++)
			optParam[i] = new float[2];
		plank_coef = new float[2 * bandsNumber];		//光谱辐射校正普朗克校正系数

	}
	catch (bad_alloc* e)
	{
		lError = -1;
		goto Err_End;
	}

	int iterator_param1 = 0;
	char szTemp[1024];

	//读取MODTRAN模拟的反射率和辐亮度
	//读取MODTRAN模拟的反射率和辐亮度
	if (fopen_s(&fModtrans, hyperModtran, "r") != 0)
	{
		lError = -1;
		goto Err_End;
	}
	//跳过11行
	for (int i = 0; i < 11; i++)
		fgets(szTemp, 1024, fModtrans);
	//ifs.getline(szTemp, 1024);

	//读取数据
	float dataHyper[16];
	for (int i = 0; i<n_bin; i++)
	{
		fscanf_s(fModtrans, "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f", &dataHyper[0], &dataHyper[1], &dataHyper[2], &dataHyper[3], &dataHyper[4],
			&dataHyper[5], &dataHyper[6], &dataHyper[7], &dataHyper[8], &dataHyper[9],
			&dataHyper[10], &dataHyper[11], &dataHyper[12], &dataHyper[13], &dataHyper[14],
			&dataHyper[15]);
		hyperSpectral[i] = dataHyper[8] * 1000.0f;
	}
	fclose(fModtrans); fModtrans = NULL;


	//读取中心波长信息和半波宽
	iterator_param1 = 0;
	if (fopen_s(&fBandInfo, bandInfo, "r") != 0)
	{
		lError = -1;
		goto Err_End;
	}
	do
	{
		fscanf_s(fBandInfo, "%f%f", &band_waveLength[iterator_param1], &band_FWHM[iterator_param1]);
		iterator_param1++;
	} while (!feof(fBandInfo) && iterator_param1<bandsNumber);
	fclose(fBandInfo); fBandInfo = NULL;

	if (iterator_param1 != bandsNumber)
	{
		lError = -1;
		goto Err_End;
	}

	//读取普朗克线性化系数
	//跳过第一行
	FILE *fPlank = NULL;
	if (fopen_s(&fPlank, plank_line_coeff, "r") != 0)
	{
		lError = -1;
		goto Err_End;
	}
	fgets(szTemp, 1024, fPlank);
	for (int i = 0; i<bandsNumber; i++)
	{
		fscanf_s(fPlank, "%f%f", &plank_coef[2 * i + 0], &plank_coef[2 * i + 1]);
	}
	fclose(fPlank); fPlank = NULL;

	//大气吸收峰
	float wlen_min = 11.65;		//自己定义的水汽吸收峰
	float wlen_max = 11.825;

	int index_min, index_max;
	int sel_number_band;		//大气吸收峰的波段数目
	for (int i = 0; i<bandsNumber; i++)
	{
		if (band_waveLength[i] <= wlen_max)
			index_max = i;
		if (band_waveLength[i] <= wlen_min)
			index_min = i;
	}
	index_min++;

	//读取影像数据
	for (int i = 0; i<bandsNumber; i++)
		GDALRasterIO(GDALGetRasterBand(m_dataset, i + 1), GF_Read, 0, imgLines - imgSelLine, imgSamples,
			imgSelLine, img_data[i], imgSamples, imgSelLine, GDT_Float32, 0, 0);
	GDALClose((GDALDatasetH)m_dataset);

	//转换为辐亮度
	for (int i = 0; i<bandsNumber; i++)
	{
		for (int j = 0; j<imgSamples*imgSelLine; j++)
			img_data[i][j] = img_data[i][j] * plank_coef[2 * i + 1] + plank_coef[2 * i + 0];
	}

	//针对每一列进行光谱定标
	//每一列的光谱定标数据	
	int totalproc = 0;
#pragma omp parallel for
	for (int j = 0; j<sampleNum; j++)
	{
		totalproc++;
		float *spectral_slice = new float[bandsNumber];  //用做处理的列均值光谱
		memset(spectral_slice, 0, sizeof(float)*bandsNumber);

		printf("\rprocess sample %d", totalproc);
		for (int i = 0; i<bandsNumber; i++)
		{
			for (int k = 0; k<imgSelLine; k++)
				spectral_slice[i] += img_data[i][k*imgSamples + j*sample_interval];
		}
		for (int i = 0; i<bandsNumber; i++)
			spectral_slice[i] /= imgSelLine;

		Level1Proc_Powell(optParam[j], hyperSpectral, nWLens, n_bin, band_waveLength, band_FWHM, spectral_slice, bandsNumber, index_min, index_max);
		delete[]spectral_slice;
	}
	printf("\n");


	FILE* fShift = NULL;
	fopen_s(&fShift, pathShift, "w");
	for (int i = 0; i < imgSamples; i++)
		fprintf(fShift, "%d  %lf  %lf\n", i*sample_interval,optParam[i][0], optParam[i][1]);
	fclose(fShift); fShift = NULL;

	//清除指针并退出
Err_End:
	if (optParam != NULL)
	{
		for (int i = 0; i<sampleNum; i++)
			delete[]optParam[i];
		delete[]optParam;
	}
	if (nWLens != NULL)
		delete[]nWLens;
	if (hyperSpectral != NULL)
		delete[]hyperSpectral;
	if (band_waveLength != NULL)
		delete[]band_waveLength;
	if (band_FWHM != NULL)
		delete[]band_FWHM;
	if (img_data != NULL)
	{
		for (int i = 0; i<bandsNumber; i++)
			delete[]img_data[i];
		delete[]img_data;
	}
	if (plank_coef != NULL)
		delete[]plank_coef;
	return lError;
}

//=============================================================================================================================================
//内部接口
long THRLevel1Process::Level1Proc_MODTRAN_Batch(const char* tap_name, const char* modtranDir, char* SURREF1, float V11, float V21)
{
	if (tap_name == NULL)
		return -1;

	//Card1：21个参数
	char MODTRAN = 'T';
	char SPEED = 's';
	char BINARY = ' ';
	char LYMOLC = ' ';
	char MODEL = '2';
	char T_BEST = ' ';
	char *ModelT = "2";
	char ITYPE[10]; sprintf_s(ITYPE, "%4s", "2");
	char IEMSCT[10]; sprintf_s(IEMSCT, "%5s", "2");
	char IMULT[10]; sprintf_s(IMULT, "%5s", "1");
	char M1[10]; sprintf_s(M1, "%5s", ModelT);
	char M2[10]; sprintf_s(M2, "%5s", ModelT);
	char M3[10]; sprintf_s(M3, "%5s", ModelT);
	char M4[10]; sprintf_s(M4, "%5s", ModelT);
	char M5[10]; sprintf_s(M5, "%5s", ModelT);
	char M6[10]; sprintf_s(M6, "%5s", ModelT);
	char MDEF[10]; sprintf_s(MDEF, "%5s", "0");
	char I_RD2C[10]; sprintf_s(I_RD2C, "%5s", "0");
	char CKPRNT = ' ';
	char NOPRNT[10]; sprintf_s(NOPRNT, "%4s", "0");
	char TPTEMP[20]; sprintf_s(TPTEMP, "% 8s", "300.000");
	char SURREF[20]; sprintf_s(SURREF, "% 7s", "0.000");

	//Card1A：22个参数
	char DIS = 't';
	char DISAZM = 't';
	char DISALB = ' ';
	char NSTR[10]; sprintf_s(NSTR, "%3s", "8");
	char SFWM[10]; sprintf_s(SFWM, "%4s", "0");
	char CO2MX[20]; sprintf_s(CO2MX, "%10s", "390.00000");
	char H2OSTR[20]; sprintf_s(H2OSTR, "%10s", "1.00000");
	char O3STR[20]; sprintf_s(O3STR, "%10s", "1.00000");
	char C_PROF = ' ';
	char LSUNFL = 'f';
	char* LBMNAM = " t";
	char* LFLTNM = " f";
	char* H2OAER = " t";
	char* CDTDIR = "  ";
	char SLEVEL = ' ';
	char SOLCON[20]; sprintf_s(SOLCON, "%9s", "0");
	char CDASTM = ' ';
	char ASTMC[20]; sprintf_s(ASTMC, "%9s", " ");
	char ASTMX[20]; sprintf_s(ASTMX, "%10s", "0");
	char ASTMO[20]; sprintf_s(ASTMO, "%10s", " ");
	char AERRH[20]; sprintf_s(AERRH, "%10s", " ");
	char NSSALB[20]; sprintf_s(NSSALB, "%10s", "0");

	//card1A2：1个参数
	char* BMNAME = "p1_2009";

	//card2：14个参数
	char* APLUS = "  ";
	char IHAZE[10]; sprintf_s(IHAZE, "%3s", "4");
	char CNOVAM = ' ';
	char ISEASN[10]; sprintf_s(ISEASN, "%4s", "0");
	char ARUSS[10]; sprintf_s(ARUSS, "%3s", " ");
	char IVULCN[10]; sprintf_s(IVULCN, "%2s", "0");
	char ICSTL[10]; sprintf_s(ICSTL, "%5s", "0");
	char ICLD[10]; sprintf_s(ICLD, "%5s", "0");
	char IVSA[10]; sprintf_s(IVSA, "%5s", "0");
	char VIS[20]; sprintf_s(VIS, "%10s", "30.00000");
	char WSS[20]; sprintf_s(WSS, "%10s", "0.00000");
	char WHH[20]; sprintf_s(WHH, "%10s", "0.00000");
	char RAINRT[20]; sprintf_s(RAINRT, "%10s", "0.00000");
	char GNDALT[20]; sprintf_s(GNDALT, "%10s", "0.00300");

	//card3：9个参数（默认2c选项）
	char H1ALT[20]; sprintf_s(H1ALT, "%10s", "2.003");
	char H2ALT[20]; sprintf_s(H2ALT, "%10s", "0.003");
	char OBSZEN[20]; sprintf_s(OBSZEN, "%10s", "0.000");
	char HRANGE[20]; sprintf_s(HRANGE, "%10s", "2.000");
	char BETA[20];  sprintf_s(BETA, "%10s", "0.000");
	char RAD_E[20];  sprintf_s(RAD_E, "%10s", "0.000");
	char LENN[10];  sprintf_s(LENN, "%5s", "0");
	char BCKZEN[30]; sprintf_s(BCKZEN, "%15s", "0.000");
	char CKRANG[20]; sprintf_s(CKRANG, "%10s", "0.000");

	//card3A1：4个参数
	char IPARM[10]; sprintf_s(IPARM, "%5s", "1");
	char IPH[10]; sprintf_s(IPH, "%5s", "2");
	char IDAY[10]; sprintf_s(IDAY, "%5s", "157");
	char ISOURC[10]; sprintf_s(ISOURC, "%5s", "0");

	//card3A2：8个参数
	char PARM1[20]; sprintf_s(PARM1, "%10s", "29.93642");
	char PARM2[20]; sprintf_s(PARM2, "%10s", "237.64168");
	char PARM3[20]; sprintf_s(PARM3, "%10s", "0.00000");
	char PARM4[20]; sprintf_s(PARM4, "%10s", "0.00000");

	char GMTIME[20]; sprintf_s(GMTIME, "%10s", "8.00000");
	char TRUEAZ[20]; sprintf_s(TRUEAZ, "%10s", "0.00000");
	char ANGLEM[20]; sprintf_s(ANGLEM, "%10s", "0.00000");
	char G[20]; sprintf_s(G, "%10s", "0.00000");

	//card4：16个参数
	char V1[20]; sprintf_s(V1, "%10s", "0.320");
	char V2[20]; sprintf_s(V2, "%10s", "2.600");
	char DV[20]; sprintf_s(DV, "%10s", "0.001");
	char FWHM[20]; sprintf_s(FWHM, "%10s", "0.002");
	char YFLAG = 't';
	char XFLAG = 'm';
	char DLIMIT[20]; sprintf_s(DLIMIT, "%8s", " ");
	char FLAGS11 = 'm';
	char FLAGS22 = '2';
	char FLAGS33 = 'a';
	char FLAGS44 = 'a';
	char FLAGS55 = ' ';
	char FLAGS66 = ' ';
	char FLAGS77 = ' ';
	char MLFLX[20]; sprintf_s(MLFLX, "%10s", " ");
	char VRFRAC[20]; sprintf_s(VRFRAC, "%10s", " ");

	//card5：1个参数
	char IRPT[10]; sprintf_s(IRPT, "%5s", "0");

	//重新设置参数
	sprintf_s(SURREF, "%7s", SURREF1);
	sprintf_s(V1, "%10.3f", V11);
	sprintf_s(V2, "%10.3f", V21);

	char modExeIn1[256];
	sprintf_s(modExeIn1, "%s\\%s", modtranDir, tap_name);
	ofstream ofs(modExeIn1);
	if (!ofs.is_open())
		return -1;

	//card1 参数输出
	ofs << MODTRAN << SPEED << BINARY << LYMOLC << MODEL << T_BEST << ITYPE << IEMSCT <<
		IMULT << M1 << M2 << M3 << M4 << M5 << M6 << MDEF <<
		I_RD2C << CKPRNT << NOPRNT << TPTEMP << SURREF << "     " << "!card1" << endl;
	//card1A 参数输出
	ofs << DIS << DISAZM << DISALB << NSTR << SFWM << CO2MX << H2OSTR << O3STR <<
		C_PROF << LSUNFL << LBMNAM << LFLTNM << H2OAER << CDTDIR << SLEVEL <<
		SOLCON << CDASTM << ASTMC << ASTMX << ASTMO << AERRH << NSSALB << "     " << "!card1A" << endl;
	//card1A2 参数输出
	ofs << BMNAME << endl;
	//card2 参数输出
	ofs << APLUS << IHAZE << CNOVAM << ISEASN << ARUSS << IVULCN << ICSTL << ICLD <<
		IVSA << VIS << WSS << WHH << RAINRT << GNDALT << "     " << "!card2" << endl;
	//card3 参数输出
	ofs << H1ALT << H2ALT << OBSZEN << HRANGE << BETA << RAD_E << LENN << BCKZEN << CKRANG << "     " << "!card3" << endl;
	//card3A1 参数输出
	ofs << IPARM << IPH << IDAY << ISOURC << "     " << "!card3A1" << endl;
	//card3A2 参数输出
	ofs << PARM1 << PARM2 << PARM3 << PARM4 << GMTIME << TRUEAZ << ANGLEM << G << "     " << "!card3A2" << endl;
	//card4  参数输出
	ofs << V1 << V2 << DV << FWHM << YFLAG << XFLAG << DLIMIT << FLAGS11 << FLAGS22 << FLAGS33 << FLAGS44 <<
		FLAGS55 << FLAGS66 << FLAGS77 << MLFLX << VRFRAC << "     " << "!card4" << endl;
	//card5 参数输出
	ofs << IRPT << "     " << "!card5" << endl;
	ofs.close();

	char modExeIn2[256];
	sprintf_s(modExeIn2, "%s\\%s", modtranDir, "mod5root.in");
	ofs.open(modExeIn2);
	if (!ofs.is_open())
		return -1;
	ofs << modExeIn1 << endl;
	ofs.close();

	//调用MODTRAN生成模拟光谱
	printf("execute MODTRAN simulate the spectral\n");
	char cmdLine[1024];
	sprintf_s(cmdLine, "%s\\%s", modtranDir, "Mod53qwin.exe");
	system(cmdLine);
	return 0;
}
long THRLevel1Process::Level1Proc_Simulate(float *hyperRadiance, float* hyperWaveLength, float *center_wave_Length, int hyperNum,
	float *band_FWHM, int bandNum, float shift_lamda, float delta_shift_lamda, float* simuluate_radiance)
{
	//判断输入是否有误
	if (hyperRadiance == NULL || hyperWaveLength == NULL ||
		center_wave_Length == NULL || band_FWHM == NULL ||
		simuluate_radiance == NULL)
		return -1;

	//获取模拟光谱
	memset(simuluate_radiance, 0, sizeof(float)*bandNum);
	float* rsp = new float[hyperNum];
	for (int i = 0; i<bandNum; i++)
	{
		for (int j = 0; j<hyperNum; j++)
			rsp[j] = exp(-pow((hyperWaveLength[j] - center_wave_Length[i] - shift_lamda / 1000.0f), 2) / (pow(band_FWHM[i] + delta_shift_lamda / 1000, 2) / 2.7735));

		for (int j = 0; j<hyperNum; j++)
		{
			if (rsp[j]<0.0001)
				rsp[j] = 0;
		}
		float tmp1 = 0, tmp2 = 0;
		for (int j = 0; j<hyperNum; j++)
		{
			tmp1 += rsp[j] * hyperRadiance[j];
			tmp2 += rsp[j];
		}
		simuluate_radiance[i] = tmp1 / tmp2*1000.0f;
	}
	delete[]rsp;

	return 0;
}
long  THRLevel1Process::Level1Proc_NODD(float* radiance, int index_min, int index_max, float* radiance_NODD)
{
	//判断数据输入是否正确
	if (radiance == NULL || radiance_NODD == NULL)
		return -1;

	//数据空间申请
	float *tmpRadiacneNODD1 = new float[index_max - index_min + 1];
	float *tmpRadiacneNODD2 = new float[index_max - index_min];

	//1.辐亮度光谱取负对数
	for (int i = index_min; i <= index_max; i++)
		tmpRadiacneNODD1[i - index_min] = -log(radiance[i]);

	//2.取负对数后求导，得到微分光谱（一阶差分）
	for (int i = index_min; i <= index_max - 1; i++)
		tmpRadiacneNODD2[i - index_min] = tmpRadiacneNODD1[i - index_min + 1] - tmpRadiacneNODD1[i - index_min];

	//3.对一阶微分光谱进行正态变换
	float tmpmean = 0, tmpstdvar = 0;
	for (int i = 0; i<index_max - index_min; i++)
		tmpmean += tmpRadiacneNODD2[i];
	tmpmean /= (index_max - index_min);
	for (int i = 0; i<index_max - index_min; i++)
		tmpstdvar += (tmpRadiacneNODD2[i] - tmpmean)*(tmpRadiacneNODD2[i] - tmpmean);
	tmpstdvar /= (index_max - index_min);
	tmpstdvar = sqrt(tmpstdvar);
	for (int i = 0; i<(index_max - index_min); i++)
		radiance_NODD[i] = (tmpRadiacneNODD2[i] - tmpmean) / tmpstdvar;

	return 0;
}
float THRLevel1Process::Level1Proc_MeritFunc(float* simulateRadiance, float* realRadiance, int index_min, int index_max)
{
	//检测输入是否正确
	if (simulateRadiance == NULL || realRadiance == NULL)
		return -1;

	float* simulate_radiance_NODD = new float[index_max - index_min];
	float* real_radiance_NODD = new float[index_max - index_min];

	long lError = 0;
	lError = Level1Proc_NODD(simulateRadiance, index_min, index_max, simulate_radiance_NODD);
	if (lError != 0)
		goto ErrEnd;
	lError = Level1Proc_NODD(realRadiance, index_min, index_max, real_radiance_NODD);
	if (lError != 0)
		goto ErrEnd;

	float gamma = 0.5;
	float tmpSD = 0, tmpSA = 0;
	for (int i = 0; i<index_max - index_min; i++)
		tmpSD += (simulate_radiance_NODD[i] - real_radiance_NODD[i])*(simulate_radiance_NODD[i] - real_radiance_NODD[i]);
	tmpSD /= (index_max - index_min);

	float tmp1 = 0, tmp2 = 0, tmp3 = 0;
	for (int j = 0; j<index_max - index_min; j++)
	{
		tmp1 += simulate_radiance_NODD[j] * real_radiance_NODD[j];
		tmp2 += simulate_radiance_NODD[j] * simulate_radiance_NODD[j];
		tmp3 += real_radiance_NODD[j] * real_radiance_NODD[j];
	}

	tmpSA = PI / 2 * acos(tmp1 / sqrt(tmp2*tmp3));
	delete[]simulate_radiance_NODD;
	delete[]real_radiance_NODD;
	return (1 - gamma)*tmpSD + gamma*tmpSA;

ErrEnd:
	delete[]simulate_radiance_NODD;
	delete[]real_radiance_NODD;
	return -1.0f;
}
long  THRLevel1Process::Level1Proc_Powell(float* optParam, float* hyperSpectral, float *hyperWavelength, int hyperNum,
	float* bandCenterLength, float* bandFWHM, float* realSpectral, int realNum,
	int index_min, int index_max)
{
	//检查数据输入
	if (optParam == NULL || hyperSpectral == NULL ||
		hyperWavelength == NULL || bandCenterLength == NULL ||
		bandFWHM == NULL || realSpectral == NULL)
		return -1;

	//设置初始值为0 初始方向 误差限 迭代次数
	optParam[0] = optParam[1] = 0;
	float iniDirect1[2], iniDirect2[2], iniDirect3[2];
	iniDirect1[0] = 1; iniDirect1[1] = 0;
	iniDirect2[0] = 0; iniDirect2[1] = 1;

	float errThres = 0.0001;
	float merit = 0;

	float lamdaStep = 10.0f;		//步长
	float belta = 0.1;		    //收缩因子
	float acclerFactor = 1.0f;	//加速因子

	int   iteratorNum = 0;
	long  lError = 0;
	//开始进行迭代求解
	float *simulate_radiance = new float[realNum];
	//第一次求解得到模拟光谱和误差
	lError = Level1Proc_Simulate(hyperSpectral, hyperWavelength, bandCenterLength, hyperNum, bandFWHM, realNum, optParam[0], optParam[1], simulate_radiance);

	////测试模拟光谱的求解是否正确：输出到文件中绘图
	//ofstream ofs("temp.txt");
	//for (int i=0;i<realNum;i++)
	//	ofs<<bandCenterLength[i]<<" "<<simulate_radiance[i]<<endl;
	//ofs.close();

	if (lError != 0)
	{
		delete[]simulate_radiance;
		return lError;
	}

	merit = Level1Proc_MeritFunc(simulate_radiance, realSpectral, index_min, index_max);

	//在这里采用模式搜索方法求解
	float merit_pre = 0;
	float tmpParam1[2], tmpParam2[2], preParam[2], tmpParam[2];
	float merit1, merit2;
	bool  isLastSmaller = false;
	float lamdaStepPre = lamdaStep;
	merit_pre = merit;

	//初始化
	tmpParam[0] = optParam[0];
	tmpParam[1] = optParam[1];
	do
	{
		iteratorNum++;
		preParam[0] = optParam[0];
		preParam[1] = optParam[1];
		isLastSmaller = false;

		//x方向正向
		tmpParam1[0] = tmpParam[0] + lamdaStep*iniDirect1[0];
		tmpParam1[1] = tmpParam[1] + lamdaStep*iniDirect1[1];
		lError = Level1Proc_Simulate(hyperSpectral, hyperWavelength, bandCenterLength, hyperNum, bandFWHM, realNum, tmpParam1[0], tmpParam1[1], simulate_radiance);
		merit1 = Level1Proc_MeritFunc(simulate_radiance, realSpectral, index_min, index_max);
		//x方向负向
		tmpParam2[0] = tmpParam[0] - lamdaStep*iniDirect1[0];
		tmpParam2[1] = tmpParam[1] - lamdaStep*iniDirect1[1];
		lError = Level1Proc_Simulate(hyperSpectral, hyperWavelength, bandCenterLength, hyperNum, bandFWHM, realNum, tmpParam2[0], tmpParam2[1], simulate_radiance);
		merit2 = Level1Proc_MeritFunc(simulate_radiance, realSpectral, index_min, index_max);
		if (merit1<merit_pre)
		{
			tmpParam[0] = tmpParam1[0];
			tmpParam[1] = tmpParam1[1];
			merit_pre = merit1;
			isLastSmaller = true;
		}
		else if (merit2<merit_pre)
		{
			tmpParam[0] = tmpParam2[0];
			tmpParam[1] = tmpParam2[1];
			merit_pre = merit2;
			isLastSmaller = true;
		}

		//y方向正向
		tmpParam1[0] = tmpParam[0] + lamdaStep*iniDirect2[0];
		tmpParam1[1] = tmpParam[1] + lamdaStep*iniDirect2[1];
		lError = Level1Proc_Simulate(hyperSpectral, hyperWavelength, bandCenterLength, hyperNum, bandFWHM, realNum, tmpParam1[0], tmpParam1[1], simulate_radiance);
		merit1 = Level1Proc_MeritFunc(simulate_radiance, realSpectral, index_min, index_max);
		//y方向负向
		tmpParam2[0] = tmpParam[0] - lamdaStep*iniDirect2[0];
		tmpParam2[1] = tmpParam[1] - lamdaStep*iniDirect2[1];
		lError = Level1Proc_Simulate(hyperSpectral, hyperWavelength, bandCenterLength, hyperNum, bandFWHM, realNum, tmpParam2[0], tmpParam2[1], simulate_radiance);
		merit2 = Level1Proc_MeritFunc(simulate_radiance, realSpectral, index_min, index_max);
		if (merit1<merit_pre)
		{
			tmpParam[0] = tmpParam1[0];
			tmpParam[1] = tmpParam1[1];
			merit_pre = merit1;
			isLastSmaller = true;
		}
		else if (merit2<merit_pre)
		{
			tmpParam[0] = tmpParam2[0];
			tmpParam[1] = tmpParam2[1];
			merit_pre = merit2;
			isLastSmaller = true;
		}


		if (isLastSmaller)
		{
			optParam[0] = tmpParam[0];
			optParam[1] = tmpParam[1];

			//下一个参考点
			tmpParam[0] = 2 * optParam[0] - preParam[0];
			tmpParam[1] = 2 * optParam[1] - preParam[1];
			continue;
		}
		else
		{
			if (tmpParam[0] == preParam[0] && tmpParam[1] == preParam[1])
			{
				if (lamdaStep>errThres)
				{
					lamdaStepPre = lamdaStep;
					lamdaStep = lamdaStepPre*belta;
				}
				else
					break;
			}
			else
			{
				tmpParam[0] = preParam[0];
				tmpParam[1] = preParam[1];
			}
		}
	} while (true);

	//清除数据并退出
	delete[]simulate_radiance;
	return lError;
}
