#include"HyperSpectralRepair.h"
#include"..\AuxiliaryFunction.h"
#include"..\HyperSpectral\fusionfunc.h"
#include"HyperRepair.h"
void HyperRepairFunc::RepairFunc_Run(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathDeImg, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMaskImg, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetImg);
	int ysize = GDALGetRasterYSize(m_datasetImg);
	int bands = GDALGetRasterCount(m_datasetImg);
	int lossbands = end - st + 1;
	RepairFunc_SetLossBands(st, end);

	int* dataImg = NULL;
	int* dataMsk = NULL;
	try
	{
		dataImg= new int[xsize*ysize*bands];
		dataMsk= new int[xsize*ysize];
	}
	catch (bad_alloc e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}
	for (int i = 0; i < bands; ++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetImg, i + 1), GF_Read, 0, 0, xsize, ysize, dataImg+i*xsize*ysize, xsize, ysize, GDT_Int32, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, dataMsk, xsize, ysize, GDT_Int32, 0, 0);
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);


	//缺失波段必须在百分之70以下，否则效果不好。可能百分比需要进行调整?
	if (lossbands<int(0.3*bands))
		RepairFunc_RepairPartial(dataImg, dataMsk,xsize,ysize,bands);
	else
		RepairFunc_RepairTotal(dataImg, dataMsk, xsize, ysize, bands);

	//数据写回
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathReDst, xsize, ysize, bands, GDT_Int32, NULL);
	for (int i = 0; i < bands; ++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, dataImg + i*xsize*ysize, xsize, ysize, GDT_Int32, 0, 0);
	GDALClose(m_datasetDst);

	delete[]dataImg;
	delete[]dataMsk;

}

void HyperRepairFunc::RepairFunc_SimuliDegradImg(const char* pathImg, const char* pathDeImg, int st, int end, CPOINT lup, CPOINT rbottom)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_dataset = GDALOpen(pathImg, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int bands = GDALGetRasterCount(m_dataset);
	char **papszOptions = NULL;
	papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BSQ");
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("ENVI"), pathDeImg, xsize, ysize, bands, GDT_Int32, papszOptions);
	int* pData = new int[xsize*ysize];

	for (int i = 0; i < bands; ++i)
	{
		GDALRasterIO(GDALGetRasterBand(m_dataset, i + 1), GF_Read, 0, 0, xsize, ysize, pData, xsize, ysize, GDT_Int32, 0, 0);
		if (i >= st - 1 && i <= end - 1)
		{
			for (int i = lup.x; i < rbottom.x; ++i)
			{
				for (int j = lup.y; j < rbottom.y; j++)
				{
					pData[j*xsize + i] = -1;
				}
			}
		}
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, pData, xsize, ysize, GDT_Int32, 0, 0);
	}
	GDALClose(m_datasetDst);
	GDALClose(m_dataset);
	delete[]pData;
}

void HyperRepairFunc::RepairFunc_SetLossBands(int st, int end)
{
	m_HyperRepairFunc_st = st;
	m_HyperRepairFunc_end = end;
}

void HyperRepairFunc::RepairFunc_RepairPartial(int* dataImg, int* dataMask,int xsize,int ysize,int bands)
{
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			if (dataMask[j*xsize + i] == -1)
			{
				printf("process position %d %d\r", i, j);
				vector<int>matchPos;
				vector<float>matchWeight;
				RepairFunc_MatchLoss(dataImg, dataMask, i, j, xsize, ysize, bands, matchPos, matchWeight);
				RepairFunc_Repair(dataImg, i, j,xsize,ysize, matchPos, matchWeight);
			}
		}
	}
	printf("\n");
}

void HyperRepairFunc::RepairFunc_RepairTotal(int* dataImg, int* dataMask, int xsize, int ysize, int bands)
{

}

void HyperRepairFunc::RepairFunc_MatchLoss(int* dataImg, int* dataMask, int posx, int posy, int xsize, int ysize, int bands, vector<int>&matchPos, vector<float> &matchWeight)
{
	int loss = m_HyperRepairFunc_end - m_HyperRepairFunc_st + 1;
	int numband = bands - loss;
	float* data1 = new float[numband];
	float* data2 = new float[numband];

	//获取缺失数据波段其他正确波段像素
	int total = 0;
	for (int i = 0; i < bands;++i)
	{
		if (i >= (m_HyperRepairFunc_st - 1) && i <= (m_HyperRepairFunc_end - 1))
			continue;
		else
		{
			data2[total] =(float) dataImg[posy*xsize + posx + i*xsize*ysize];
			total++;
		}
	}

	//计算相关系数
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			total = 0;
			if (dataMask[j*xsize + i] == -1)
				continue;
			for (int k = 0; k < bands; ++k)
			{
				if (k >= (m_HyperRepairFunc_st - 1) && k <= (m_HyperRepairFunc_end - 1))
					continue;
				else
				{
					data1[total] = (float)dataImg[j*xsize + i + k*xsize*ysize];
					total++;
				}
			}
			//float coff = GetCoefficient(data1, data2, numband);
			//if (coff > 0.98)
			//{
			//	matchPos.push_back(j*xsize + i);
			//}
			float dis = GetSSD(data1, data2, numband);
			if (dis <20* numband)
			{
				matchPos.push_back(j*xsize + i);
			}

		}
	}

	//计算权重
	float totalweigh = 0;
	for (int i = 0; i < matchPos.size(); ++i)
	{
		int tmpPosx = matchPos[i] % xsize;
		int tmpPosy = matchPos[i] / xsize;

		//距离反比加权
		float weigh = 1.0f / float(abs(posx - tmpPosx) + abs(posy - tmpPosy));
		matchWeight.push_back(weigh);
		totalweigh += weigh;
	}

	for (int i = 0; i < matchWeight.size(); ++i)
	{
		matchWeight[i] /= totalweigh;
	}

	delete[]data1;
	delete[]data2;
}

void HyperRepairFunc::RepairFunc_Repair(int* dataImg, int posx, int posy, int xsize, int ysize, vector<int>matchPos, vector<float> matchWeight)
{
	dataImg[posy*xsize + ysize] = 0;
	double *tmp = new double[m_HyperRepairFunc_end - m_HyperRepairFunc_st + 1];
	memset(tmp, 0, sizeof(double)*(m_HyperRepairFunc_end - m_HyperRepairFunc_st + 1));
	for (int i = 0; i < matchPos.size(); ++i)
	{
		int tmpPosx = matchPos[i] % xsize;
		int tmpPosy = matchPos[i] / xsize;
		for (int j = m_HyperRepairFunc_st - 1; j < m_HyperRepairFunc_end - 1; ++j)
			tmp[j]+= double(dataImg[tmpPosy*xsize + tmpPosx + j*xsize*ysize]) * double(matchWeight[i]);
	}
	for (int j = m_HyperRepairFunc_st - 1; j < m_HyperRepairFunc_end - 1; ++j)
		dataImg[posy*xsize + posx + j*xsize*ysize] = tmp[j];
	delete[]tmp;
}

void HyperRepairFunc::RepairFunc_GenerateTexture(const char* pathImg, int rangesize, const char* pathOut)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImg, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_datasetImg);
	int ysize = GDALGetRasterYSize(m_datasetImg);
	float* dataImg = new float[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_datasetImg, 1), GF_Read, 0, 0, xsize, ysize, dataImg, xsize, ysize, GDT_Float32, 0, 0);

	GDALDatasetH m_datasetOut = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize - rangesize, ysize - rangesize, rangesize*rangesize,GDT_Float32,0);

	float* dataOut = new float[(xsize - rangesize)*(ysize - rangesize)];
	int bandidx = 1;;
	for (int i = 0; i < rangesize; ++i)
	{
		for (int j = 0; j < rangesize; ++j)
		{
			//构建图像
			for (int m = 0; m < xsize - rangesize; ++m)
			{
				for (int n = 0; n < ysize - rangesize; ++n)
				{
					dataOut[n*(xsize - rangesize) + m] = dataImg[(j + n)*xsize + i + m];
				}
			}
			//输出
			GDALRasterIO(GDALGetRasterBand(m_datasetOut, bandidx), GF_Write, 0, 0, xsize - rangesize, ysize - rangesize, dataOut, xsize-rangesize, ysize- rangesize, GDT_Float32, 0, 0);
			bandidx++;
		}
	}
	GDALClose(m_datasetImg);
	GDALClose(m_datasetOut);
	delete[]dataImg;
	delete[]dataOut;
}

void HyperRepairFunc::RepairFunc_TextureElement(const char* pathEle, int rangesize, int num, const char* pathDist)
{
	float* data = new float[rangesize*rangesize*num];
	char* pc = new char[256];//足够长
	strcpy(pc, pathEle);
	data = get_endmenber_spectralf(pc,  rangesize*rangesize, num);

	//数据转换
	float* img = new float[rangesize*rangesize*num];
	for (int i = 0; i < num; ++i)
	{
		for (int m = 0; m < rangesize; ++m)
		{
			for (int n = 0; n < rangesize; ++n)
			{
				img[(m*rangesize*num+n+i*rangesize)] = data[(m*rangesize+n)*num+i];
			}
		}
	}
	GDALAllRegister();
	GDALDatasetH m_dataset = GDALCreate(GDALGetDriverByName("GTiff"), pathDist, rangesize, rangesize*num, 1, GDT_Float32,NULL);
	GDALRasterIO(GDALGetRasterBand(m_dataset, 1), GF_Write, 0, 0, rangesize, rangesize*num, img, rangesize, rangesize*num, GDT_Float32, 0, 0);
	GDALClose(m_dataset);
	delete[]data;
	delete[]img;
}

void HyperRepairFuncTest()
{
	char* pathImg = "D:\\my_doc\\2015.10.20数据\\hyper\\hypertest";
	char* pathSimu = "D:\\my_doc\\2015.10.20数据\\hyper\\hypertestSimulateAllNarrow";
	char* pathRepair = "D:\\my_doc\\2015.10.20数据\\hyper\\repair.tif";
	char* pathMaskImg = "D:\\my_doc\\2015.10.20数据\\hyper\\mask.bmp";
	char* pathMaskDst = "D:\\my_doc\\2015.10.20数据\\hyper\\mskAllNarrow.bmp";
	CPOINT pnt1, pnt2;
	pnt1.x = 164; pnt1.y = 267;
	pnt2.x = 245; pnt2.y = 287;
	HyperRepairFunc m_Repair;
	set_mask_region(pathMaskImg, pathMaskDst,pnt1,pnt2);
	//m_Repair.RepairFunc_SimuliDegradImg(pathImg, pathSimu, 1, 201, pnt1, pnt2);
	//m_Repair.RepairFunc_Run(pathSimu, pathMaskDst, pathRepair, 1, 20);
}