#include"..\stdafx.h"

#include"Level0Process.h"
#include"..\Global.h"
#include"..\AuxiliaryFunction.h"

#include"..\gdal\include\gdal_priv.h"
#include<Windows.h>
#pragma comment(lib,"gdal_i.lib")

//BIL转换为BSQ文件
long Level0Process::Level0Proc_RawToBSQ(const char* pathRawBIL, const char* pathBSQ)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALAllRegister();
	GDALDatasetH m_dataset = GDALOpen(pathRawBIL, GA_ReadOnly);
	int srcxsize = GDALGetRasterXSize(m_dataset);
	int srcysize = GDALGetRasterYSize(m_dataset);
	int srcbands = GDALGetRasterCount(m_dataset);
	GDALClose(m_dataset);

	int dstxsize = srcxsize;
	int dstysize = srcysize;
	int dstbands = srcbands;

	//开始  
	//获得文件句柄  
	HANDLE hFile = CreateFile(
		pathRawBIL,   //文件名  
		GENERIC_READ | GENERIC_WRITE, //对文件进行读写操作  
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,  //打开已存在文件  
		FILE_ATTRIBUTE_NORMAL,
		0);
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran = SysInfo.dwAllocationGranularity;
	//返回值size_high,size_low分别表示文件大小的高32位/低32位  
	DWORD size_high;
	__int64 qwFileSize = GetFileSize(hFile, &size_high);
	qwFileSize |= (((__int64)size_high) << 32);
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	CloseHandle(hFile);


	//65536*1000/513/648/2=98 //也就是说读了197行 那么偏转为
	DWORD FrameSize = srcbands*srcxsize * 2;
	DWORD RealGetLines = 50;				//一次读取50帧的设计
	DWORD TotalLines = 0;				//总读取的帧数
	DWORD BlockSize = 10000 * dwGran;

	DWORD RealBlockSize = FrameSize*RealGetLines;
	DWORD LeftBytesPre = 0;
	DWORD LeftBytes = 0;						//剩余的字节数

												//unsigned char *imgData	=new unsigned char[RealBlockSize];
	unsigned char *leftFirst = new unsigned char[FrameSize];
	unsigned char *leftFrame = new unsigned char[FrameSize];
	unsigned char *imgDst = new unsigned char[srcysize*srcxsize * 2];
	unsigned char *imgDstCut = new unsigned char[dstysize*dstxsize * 2];
	errno_t err;
	FILE* fBSQ = NULL;
	err=fopen_s(&fBSQ,pathBSQ, "wb");
	if (err != 0)
		exit(-1);	//错误代码和日志

	for (int i = 0; i<srcbands; ++i)
	{
		printf("\rprocess band :%d", i + 1);
		DWORD blockSize = BlockSize;
		__int64 tmpFileSize = qwFileSize;
		__int64 qwOffset = 0;
		TotalLines = 0;
		while (tmpFileSize>0)
		{
			if (tmpFileSize<blockSize)
				blockSize = (DWORD)tmpFileSize;
			LPBYTE lpbMapAddress = (LPBYTE)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, (DWORD)(qwOffset >> 32), (DWORD)(qwOffset & 0xFFFFFFFF), blockSize);
			//实际读取的行数
			RealGetLines = (LeftBytesPre + blockSize) / FrameSize;
			LeftBytes = (LeftBytesPre + blockSize) - RealGetLines*FrameSize;

			//第一帧影像
			//上一帧的后一段数据加上本帧的前一段数据
			for (int j = 0; j<(int)LeftBytesPre; ++j)
				leftFirst[j] = leftFrame[j];
			for (int j = LeftBytesPre; j<(int)FrameSize; ++j)
				leftFirst[j] = *(lpbMapAddress + j - LeftBytesPre);
			//剩下的影像数据
			for (int j = 0; j<(int)LeftBytes; ++j)
				leftFrame[j] = *(lpbMapAddress + RealGetLines*FrameSize - LeftBytesPre + j);

			//复制数据给目标影像
			memcpy(imgDst + TotalLines*srcxsize * 2, leftFirst + i*srcxsize * 2, srcxsize * 2);
			for (int j = 1; j<(int)RealGetLines; ++j)
				memcpy(imgDst + (j + TotalLines)*srcxsize * 2, lpbMapAddress + FrameSize - LeftBytesPre + i*srcxsize * 2 + (j - 1)*FrameSize, srcxsize * 2);

			UnmapViewOfFile(lpbMapAddress);
			tmpFileSize -= blockSize;
			qwOffset += blockSize;
			TotalLines += RealGetLines;
			LeftBytesPre = LeftBytes;
		}
		for (int j = 0; j<srcysize; ++j)
			memcpy(imgDstCut + j*dstxsize * 2, imgDst + j*srcxsize * 2, dstxsize * 2);
		fwrite(imgDstCut, 2, dstxsize*dstysize, fBSQ);
	}
	CloseHandle(hFileMap);
	fclose(fBSQ);

	//写头文件
	char drive[_MAX_DRIVE]; char dir[_MAX_DIR]; char filename[_MAX_FNAME]; char ext[_MAX_EXT];
	char path[_MAX_PATH];
	_splitpath_s(pathBSQ, drive, dir, filename, ext);
	_makepath_s(path, drive, dir, filename, "hdr");
	ENVIHeader mENVIHeader;
	memset(&mENVIHeader, 0, sizeof(ENVIHeader));
	mENVIHeader.datatype = 12;
	mENVIHeader.imgWidth = dstxsize;
	mENVIHeader.imgHeight = dstysize;
	mENVIHeader.imgBands = dstbands;
	mENVIHeader.interleave = "BSQ";
	WriteENVIHeader(path, mENVIHeader);

	delete[]imgDstCut;
	delete[]imgDst;
	delete[]leftFirst;
	delete[]leftFrame;

	printf("\n");
	return 0;
}


//获取非均匀性校正参数
void Level0Process::GetNonuniformParameters(const char* pathCalibFile, const char* pathDarkFile, float* params)
{
	//首先获取定标影像，然后获取影像
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径

	GDALDatasetH m_datasetH = GDALOpen(pathCalibFile, GA_ReadOnly);
	GDALDatasetH m_datasetD = GDALOpen(pathDarkFile, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetH);
	int ysize = GDALGetRasterYSize(m_datasetH);
	int bands = GDALGetRasterCount(m_datasetH);

	int xsize_d = GDALGetRasterXSize(m_datasetD);
	int ysize_d = GDALGetRasterYSize(m_datasetD);
	int bands_d = GDALGetRasterCount(m_datasetD);

	//获取数据
	//param=new float[xsize*ysize];
	unsigned short* darkBuffer = new unsigned short[xsize_d*ysize_d];
	unsigned short* imgBuffer = new unsigned short[xsize*ysize];

	//for (int i=0;i<bands;++i)
	GDALRasterIO(GDALGetRasterBand(m_datasetH, 2), GF_Read, 0, 0, xsize, ysize, imgBuffer, xsize, ysize, GDT_UInt16, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetD, 1), GF_Read, 0, 0, xsize_d, ysize_d, darkBuffer, xsize_d, ysize_d, GDT_UInt16, 0, 0);

	//获取行均值
	unsigned short * tmpLine = new unsigned short[xsize];
	//每一行求取其校正系数
	for (int i = 0; i<ysize; ++i)
	{
		//减去暗电流
		for (int j = 0; j<xsize; ++j)
			tmpLine[j] = imgBuffer[i*xsize + j] - darkBuffer[i*xsize + j];

		//两次求均值
		double dtmp = 0;
		for (int j = 0; j < xsize; ++j)
			dtmp += double(tmpLine[j]) / double(xsize);
		for (int j = 0; j<xsize; ++j)
			params[i*xsize + j] = dtmp / double(tmpLine[j]);
	}
	delete[]tmpLine;
	delete[]imgBuffer;
	delete[]darkBuffer;

	GDALClose(m_datasetH);
	GDALClose(m_datasetD);
}
//非均匀性校正文件
long Level0Process::Level0Proc_Nonuniform(const char* pathBSQ, const char* pathNonuniform, const char* pathCalibFile/* = 0*/, const char* pathDarkFile /*= 0*/)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALAllRegister();
	GDALDatasetH m_dataset = GDALOpen(pathBSQ, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int bands = GDALGetRasterCount(m_dataset);

	unsigned short* imgSrc = new unsigned short[xsize*ysize];
	float* imgDst = new float[xsize*ysize];

	GDALDatasetH m_datasetDark = GDALOpen(pathDarkFile, GA_ReadOnly);
	int darkxsize = GDALGetRasterXSize(m_datasetDark);
	int darkysize = GDALGetRasterYSize(m_datasetDark);
	int bandsdark = GDALGetRasterCount(m_datasetDark);


	printf("\r获取校正参数");
	float *param = new float[xsize*bands];
	memset(param, 0, sizeof(float)*xsize*bands);
	if (pathCalibFile != NULL)
		GetNonuniformParameters(pathCalibFile, pathDarkFile, param);
	else
	{
		//暗电流比影像DN值还大  表示搞不清楚对于全谱段数据暗电流似乎有问题
		unsigned short* dataDark=new unsigned short[darkxsize*darkysize];
		GDALRasterIO(GDALGetRasterBand(m_datasetDark,1),GF_Read,0,0,darkxsize,darkysize,dataDark,darkxsize,darkysize,GDT_UInt16,0,0);
		//获取校正系数
		for (int k = 0; k<bands; ++k)
		{
			GDALRasterIO(GDALGetRasterBand(m_dataset, k + 1), GF_Read, 0, 0, xsize, ysize, imgSrc, xsize, ysize, GDT_UInt16, 0, 0);
			float LineMeanNum = 0.0f;

			for (int i=0;i<xsize;++i)
			 	for (int j=0;j<ysize;++j)
			 		imgSrc[j*xsize+i]=imgSrc[j*xsize+i]-dataDark[k*darkxsize+i];

			//列均值
			for (int i = 0; i<xsize; ++i)
				for (int j = 0; j<ysize; ++j)
					param[k*xsize + i] += float(imgSrc[j*xsize + i]) / ysize;
			for (int i = 0; i<xsize; ++i)
				LineMeanNum += param[k*xsize + i] / xsize;
			for (int i = 0; i<xsize; ++i)
				param[k*xsize + i] = LineMeanNum / param[k*xsize + i];
		}
		GDALClose(m_datasetDark);
		delete[]dataDark;
	}

	char** 	papszOptions = NULL;
	papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "bsq");
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("ENVI"), pathNonuniform, xsize, ysize, bands, GDT_UInt16, papszOptions);

	printf("\r               ");
	for (int i = 0; i<bands; ++i)
	{
		printf("\r处理波段%d", i + 1);
		GDALRasterIO(GDALGetRasterBand(m_dataset, i + 1), GF_Read, 0, 0, xsize, ysize, imgSrc, xsize, ysize, GDT_UInt16, 0, 0);
		for (int j = 0; j<xsize; ++j)
			for (int k = 0; k<ysize; ++k)
				imgDst[k*xsize + j] = (float)(imgSrc[k*xsize + j])*param[i*xsize + j];
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, imgDst, xsize, ysize, GDT_Float32, 0, 0);
		GDALFlushCache(m_datasetDst);
	}
	printf("\n");

	GDALClose(m_dataset);
	GDALClose(m_datasetDst);

	delete[]param;
	delete[]imgSrc;
	delete[]imgDst;

	return 0;
}


//除去两侧辅助数据和无意义波段，得到纯影像数据信息并转换为TIF格式
long Level0Process::Level0Proc_GetUsefulData(const char* pathBSQ, const char* pathUseful, int leftunuse, int rightunuse, vector<int> usefulBands)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALAllRegister();
	GDALDatasetH m_dataset = GDALOpen(pathBSQ, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int bands = GDALGetRasterCount(m_dataset);

	//参数异常
	if (xsize - leftunuse - rightunuse < 0)
		exit(-1);

	unsigned short* imgBuffer	 = NULL;
	unsigned short* imgBufferDst = NULL;
	try
	{
		imgBuffer	 = new unsigned short[xsize*ysize];
		imgBufferDst = new unsigned short[(xsize - leftunuse - rightunuse)*ysize];
	}
	catch (bad_alloc)
	{
		printf("memory allocate err\n");
		exit(-1);
	}
	int totalBands = 0;
	for (size_t i = 0; i < usefulBands.size(); ++i)
		if (usefulBands[i]>0 && usefulBands[i] <= bands)
			totalBands++;
	//输入参数异常
	if (totalBands == 0)
		exit(-1);

	char** 	papszOptions = NULL;
	papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "bsq");//BSQ格式存储
	GDALDatasetH m_datasetdst = GDALCreate(GDALGetDriverByName("GTiff"), pathUseful, xsize - leftunuse - rightunuse, ysize, totalBands,GDT_UInt16, papszOptions);
	totalBands = 0;
	for (size_t i = 0; i < usefulBands.size(); i++)
	{
		if (usefulBands[i] > 0 && usefulBands[i] <= bands)
		{
			totalBands++;
			GDALRasterIO(GDALGetRasterBand(m_dataset, usefulBands[i]), GF_Read, 0, 0, xsize, ysize, imgBuffer, xsize, ysize, GDT_UInt16, 0, 0);
			for (size_t j = 0; j < ysize; j++)
				memcpy(imgBufferDst + j*(xsize - leftunuse - rightunuse), imgBuffer + j*xsize + leftunuse, sizeof(unsigned short)*(xsize - leftunuse - rightunuse));
			GDALRasterIO(GDALGetRasterBand(m_dataset,totalBands), GF_Read, 0, 0, xsize - leftunuse - rightunuse, ysize, imgBuffer, xsize - leftunuse - rightunuse, ysize, GDT_UInt16, 0, 0);
		}
	}
	
	if (imgBuffer)
		delete[]imgBuffer;
	if (imgBufferDst)
		delete[]imgBufferDst;

	imgBuffer = NULL;
	imgBufferDst = NULL;
	GDALClose(m_dataset);
	GDALClose(m_datasetdst);
	return 0;
}


//从原始数据中解算得到EVENT数据文件，这个函数是以全谱段数据为例，对于不同传感器有不同的数据格式需要重载此函数
long Level0Process::Level0Proc_ExtractEvent(const char* pathRawBIL, const char* pathEvent,const int nEventOffset)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALAllRegister();
	GDALDatasetH m_dataset = GDALOpen(pathRawBIL, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int bands = GDALGetRasterCount(m_dataset);
	GDALClose(m_dataset);

	int lError = 0;
	int i = 0;
	__int64 nOffset = 0;
	int nWidths = xsize, nLines = ysize, nBands = bands, nHeadOffset = 0;
	__int64 nFrameSize = 0;
	FILE *fD0 = NULL, *fEvent = NULL;
	errno_t err = 0;
	unsigned short lGPStime1 = 0, ltempGPStime1 = 0;
	unsigned short lGPStime2 = 0, ltempGPStime2 = 0;
	unsigned short nMicrosecond = 0, ntempMicrosecond = 0;
	unsigned long ltempGPStime = 0, lGPStime = 0;
	nFrameSize = xsize*bands*sizeof(unsigned short);

	//打开0级数据
	err = fopen_s(&fD0, pathRawBIL, "rb");
	if (err != 0)
	{
		lError = 1;
		goto ErrEnd;
	}
	err = fopen_s(&fEvent, pathEvent, "w");
	if (err != 0)
	{
		lError = 1;
		goto ErrEnd;
	}
	nOffset = nEventOffset;

	_fseeki64(fD0, nOffset, SEEK_SET);
	fread_s(&ltempGPStime1, sizeof(unsigned short), sizeof(unsigned short), 1, fD0);
	fread_s(&ltempGPStime2, sizeof(unsigned short), sizeof(unsigned short), 1, fD0);
	fread_s(&nMicrosecond, sizeof(unsigned short), sizeof(unsigned short), 1, fD0);
	lGPStime = ltempGPStime1 * 65535 + ltempGPStime2;
	for (i = 0; i < nLines; i++)
	{
		_fseeki64(fD0, nOffset, SEEK_SET);
		fread_s(&ltempGPStime1, sizeof(unsigned short), sizeof(unsigned short), 1, fD0);
		fread_s(&ltempGPStime2, sizeof(unsigned short), sizeof(unsigned short), 1, fD0);
		fread_s(&ntempMicrosecond, sizeof(unsigned short), sizeof(unsigned short), 1, fD0);
		ltempGPStime = ltempGPStime1 * 65535 + ltempGPStime2;
		if (lGPStime<ltempGPStime)
		{
			//判断是否出现GPS周内秒缺失
			if (nMicrosecond<ntempMicrosecond)
			{
				lGPStime = ltempGPStime + 1;
			}
			else
			{
				lGPStime = ltempGPStime;
			}
		}
		else
		{
			lGPStime = ltempGPStime;
			nMicrosecond = ntempMicrosecond;
		}
		nOffset += nFrameSize;
		fprintf_s(fEvent, "%d	%lf\n", i + 1, lGPStime + (double)ntempMicrosecond / 10000.0);
	}

ErrEnd:
	if (fD0)
	{
		fclose(fD0);
	}
	if (fEvent)
	{
		fclose(fEvent);
	}
	return lError;
}


//对全谱段短波数据Event信息进行修正
long QPDLevel0Process::Level0Proc_ModifySWIREvent(const char* pathEvent, const char* pathExEvent, float fTime)
{
	long IError = 0;
	FILE *fEvent1 = NULL, *fEvent2 = NULL;
	errno_t err = 0;

	//打开SBET文件、EVENT文件、POS文件
	err = fopen_s(&fEvent1, pathEvent, "r");		//二进制
	if (err)
	{
		IError = 1;
	}
	err = fopen_s(&fEvent2, pathExEvent, "w");		//二进制
	if (err)
	{
		IError = 1;
	}
	int ss = 0;
	double t = 0, t1 = 0, t2 = 0, t3 = 0;
	t2 = fTime;
	int i = 1;

	//NND 这个函数看了半天才看懂，+1秒
	do
	{
		fscanf_s(fEvent1, "%d	%lf", &ss, &t);
		t1 = t - int(t);
		if (t1<t3)
		{
			t2 += 1;
		}
		else
		{

		}
		t3 = t1;
		fprintf(fEvent2, "%d	%lf\n", i, t2 + t1);
		i++;
	} while (!feof(fEvent1));
	fclose(fEvent1);
	fclose(fEvent2);
	return 0;
}



