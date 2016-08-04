#include"HyperSpectralRepair.h"
#include"..\AuxiliaryFunction.h"
#include"..\HyperSpectral\fusionfunc.h"
#include"HyperRepair.h"
#include"..\HyperSpectral\HyperSpectural.h"
void HyperRepairFunc::RepairFunc_SpectralRun(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end)
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
	if (lossbands<int(0.5*bands))
		RepairFunc_RepairPartialSpectral(dataImg, dataMsk, xsize, ysize, bands);
	else
		//RepairFunc_RepairTotal(dataImg, dataMsk, xsize, ysize, bands);
		return;

	//数据写回
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathReDst, xsize, ysize, bands, GDT_Int32, NULL);
	for (int i = 0; i < bands; ++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, dataImg + i*xsize*ysize, xsize, ysize, GDT_Int32, 0, 0);
	GDALClose(m_datasetDst);

	delete[]dataImg;
	delete[]dataMsk;

}

void HyperRepairFunc::RepairFunc_SpatialRun(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathDeImg, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMaskImg, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetImg);
	int regionsize = 7;

	//数据申请
	float* imgData = new float[xsize*ysize];
	float* mskData = new float[xsize*ysize];
	float* tmpMskData = new float[xsize*ysize];

	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathReDst, xsize, ysize, bands, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, mskData, xsize, ysize, GDT_Float32, 0, 0);
	clock_t m_time_st = clock();
	for (int i = 1; i <= bands; ++i)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetImg, i), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
		if (i >= st&&i <= end)
		{
			memcpy(tmpMskData, mskData, sizeof(float)*xsize*ysize);
			while (!RepairFunc_ExemplarCheckTerminal(tmpMskData, xsize, ysize))
			{
				CPOINT pos;
				RepairFunc_FirstPosition(imgData, tmpMskData, xsize, ysize, pos);
				RepairFunc_ExemplarPos(imgData, mskData, tmpMskData, xsize, ysize, pos);
				//printf("repair the position (%d,%d)\n", pos.x, pos.y);
			}
		}
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	}
	clock_t m_time_end = clock();
	printf("time consuming :%lf s\n", double(m_time_end - m_time_st) / CLOCKS_PER_SEC);
	GDALClose(m_datasetDst);
	//数据输出
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);
	delete[]imgData;
	delete[]mskData;
	delete[]tmpMskData;

}

void HyperRepairFunc::RepairFunc_SpatialSpectralRun(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathDeImg, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMaskImg, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetImg);
	int regionsize = 7;

	//数据申请
	float* imgData = new float[xsize*ysize];
	float* mskData = new float[xsize*ysize];
	float* tmpMskData = new float[xsize*ysize];
	vector<CPOINT> repair_point_list;
	vector<CPOINT> degard_point_list;
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathReDst, xsize, ysize, bands, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, mskData, xsize, ysize, GDT_Float32, 0, 0);
	clock_t m_time_st = clock();

	GDALRasterIO(GDALGetRasterBand(m_datasetImg, 2), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	memcpy(tmpMskData, mskData, sizeof(float)*xsize*ysize);
	while (!RepairFunc_ExemplarCheckTerminal(tmpMskData, xsize, ysize))
	{
		CPOINT pos,repairPos;
		RepairFunc_FirstPosition(imgData, tmpMskData, xsize, ysize, pos);
		RepairFunc_GetBestMacthPos(imgData, mskData, tmpMskData, xsize, ysize, pos, repairPos);
		repair_point_list.push_back(repairPos);
		degard_point_list.push_back(pos);
	}

	for (int i = 1; i <= bands; ++i)
	{
		if (i<st || i>end)
		{
			GDALRasterIO(GDALGetRasterBand(m_datasetImg, i), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
			GDALRasterIO(GDALGetRasterBand(m_datasetDst, i), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
		}
		else
		{
			GDALRasterIO(GDALGetRasterBand(m_datasetImg, i), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
			for (int j = 0; j < repair_point_list.size(); ++j)
			{
				imgData[degard_point_list[j].y*xsize + degard_point_list[j].x] = imgData[repair_point_list[j].y*xsize + repair_point_list[j].x];
			}
			GDALRasterIO(GDALGetRasterBand(m_datasetDst, i), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
		}
	}
	clock_t m_time_end = clock();
	printf("time consuming :%lf s\n", double(m_time_end - m_time_st) / CLOCKS_PER_SEC);
	GDALClose(m_datasetDst);
	//数据输出
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);
	delete[]imgData; imgData = NULL;
	delete[]mskData; mskData = NULL;
	delete[]tmpMskData; tmpMskData = NULL;
}

void HyperRepairFunc::RepairFunc_EdgePriorityRun(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathDeImg, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMaskImg, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetImg);
	int regionsize = 7;

	//数据申请
	float* imgData = new float[xsize*ysize];
	float* mskData = new float[xsize*ysize];
	float* tmpMskData = new float[xsize*ysize];

	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathReDst, xsize, ysize, bands, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, mskData, xsize, ysize, GDT_Float32, 0, 0);
	clock_t m_time_st = clock();
	for (int i = 1; i <= bands; ++i)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetImg, i), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
		if (i >= st&&i <= end)
		{
			vector<CPOINT> edge_list;
			memcpy(tmpMskData, mskData, sizeof(float)*xsize*ysize);
			RepairFunc_GetRepairEdge(tmpMskData, xsize, ysize, edge_list);
			int num = 0;
			while (!edge_list.empty())
			{
				RepairFunc_RepairEdge(imgData, mskData, tmpMskData, edge_list, xsize, ysize);
				RepairFunc_GetRepairEdge(tmpMskData, xsize, ysize, edge_list);
				printf("repair loop %d\n",++num);
			}
		}

		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	}
	clock_t m_time_end = clock();
	printf("time consuming :%lf s\n", double(m_time_end - m_time_st) / CLOCKS_PER_SEC);
	GDALClose(m_datasetDst);
	//数据输出
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);
	delete[]imgData;
	delete[]mskData;
	delete[]tmpMskData;
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

void HyperRepairFunc::RepairFunc_GenerateTexture(const char* pathImg, int rangesize, const char* pathOut)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImg, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_datasetImg);
	int ysize = GDALGetRasterYSize(m_datasetImg);
	float* dataImg = new float[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_datasetImg, 1), GF_Read, 0, 0, xsize, ysize, dataImg, xsize, ysize, GDT_Float32, 0, 0);

	GDALDatasetH m_datasetOut = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize - rangesize, ysize - rangesize, rangesize*rangesize, GDT_Float32, 0);

	float* dataOut = new float[(xsize - rangesize)*(ysize - rangesize)];
	int bandidx = 1;
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
			GDALRasterIO(GDALGetRasterBand(m_datasetOut, bandidx), GF_Write, 0, 0, xsize - rangesize, ysize - rangesize, dataOut, xsize - rangesize, ysize - rangesize, GDT_Float32, 0, 0);
			bandidx++;
		}
	}
	GDALClose(m_datasetImg);
	GDALClose(m_datasetOut);
	delete[]dataImg;
	delete[]dataOut;
}

void HyperRepairFunc::RepairFunc_RepairPartialSpectral(int* dataImg, int* dataMask, int xsize, int ysize, int bands)
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
				RepairFunc_RepairSpectral(dataImg, i, j, xsize, ysize, matchPos, matchWeight);
			}
		}
	}
	printf("\n");
}

void HyperRepairFunc::RepairFunc_TextureElement(const char* pathEle, int rangesize, int num, const char* pathDist)
{
	int tmpsize = sqrt(double(num)) + 1;
	float* data = new float[rangesize*rangesize*num];
	char* pc = new char[256];//足够长
	strcpy(pc, pathEle);
	data = get_endmenber_spectralf(pc, rangesize*rangesize, num);

	//数据转换
	float* img = new float[tmpsize*tmpsize*rangesize*rangesize];
	memset(img, 0, sizeof(float)*tmpsize*tmpsize*rangesize*rangesize);
	for (int i = 0; i < tmpsize; ++i)
	{
		for (int j = 0; j < tmpsize; ++j)
		{
			if (j*tmpsize + i < num)
			{
				for (int m = 0; m < rangesize; ++m)
				{
					for (int n = 0; n < rangesize; ++n)
					{
						img[(j*rangesize + n)*rangesize + i*rangesize + m] = data[(m*rangesize + n)*num + j*tmpsize + i];
					}
				}
			}
			else
				break;
		}
	}
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_dataset = GDALCreate(GDALGetDriverByName("GTiff"), pathDist, tmpsize*rangesize, tmpsize*rangesize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_dataset, 1), GF_Write, 0, 0, tmpsize*rangesize, tmpsize*rangesize, img, tmpsize*rangesize, tmpsize*rangesize, GDT_Float32, 0, 0);
	GDALClose(m_dataset);
	delete[]data;
	delete[]img;
}

void HyperRepairFunc::RepairFunc_ImgDecompose(const char* pathImg, const char* pathHyper, const char* pathLow, float threshold)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImg, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_datasetImg);
	int ysize = GDALGetRasterYSize(m_datasetImg);
	int bands = GDALGetRasterCount(m_datasetImg);

	float* imgData = new float[xsize*ysize];
	float* dtHData = new float[xsize*ysize];
	float* dtLData = new float[xsize*ysize];
	//设置输出影像数据
	GDALDatasetH m_dstHDataset = GDALCreate(GDALGetDriverByName("GTiff"), pathHyper, xsize, ysize, bands, GDT_Float32, NULL);
	GDALDatasetH m_dstLDataset = GDALCreate(GDALGetDriverByName("GTiff"), pathLow, xsize, ysize, bands, GDT_Float32, NULL);

	clock_t time_st = clock();
	for (int i = 0; i < bands; ++i)
	{
		printf("process band %d\r", i + 1);
		
		GDALRasterIO(GDALGetRasterBand(m_datasetImg, i + 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
		DCT2D(imgData, xsize, ysize, dtLData);
		for (int j = 0; j < xsize*ysize; ++j)
			dtHData[j] = imgData[j] - dtLData[j];
		GDALRasterIO(GDALGetRasterBand(m_dstHDataset, i + 1), GF_Write, 0, 0, xsize, ysize, dtHData, xsize, ysize, GDT_Float32, 0, 0);
		GDALRasterIO(GDALGetRasterBand(m_dstLDataset, i + 1), GF_Write, 0, 0, xsize, ysize, dtLData, xsize, ysize, GDT_Float32, 0, 0);
	}
	GDALClose(m_datasetImg);
	GDALClose(m_dstHDataset);
	GDALClose(m_dstLDataset);
	clock_t time_end = clock();
	printf("\nprocess time:%lf s", double(time_end - time_st) / double(CLOCKS_PER_SEC));
	delete[]imgData;
	delete[]dtHData;
	delete[]dtLData;
}

void HyperRepairFunc::RepairFunc_PCATrans(const char* pathImg, const char* pathPCA, const char* pathParam)
{
	//首先获取影像信息
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImg, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_datasetImg);
	int ysize = GDALGetRasterYSize(m_datasetImg);
	int bands = GDALGetRasterCount(m_datasetImg);
	GDALClose(m_datasetImg);
	PCATrans((char*)pathImg, (char*)pathPCA, NULL, bands, (char*)pathParam);
}

void HyperRepairFunc::RepairFunc_PCAIntensity(const char* pathImgPCA, const char* InitialMask, const char* pathPCAIntensity)
{
	//首先获取掩膜区域的区域范围
	vector<CPOINT> edge_list;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetMask = GDALOpen(InitialMask, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_datasetMask);
	int ysize = GDALGetRasterYSize(m_datasetMask);

	float* imgData = new float[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_datasetMask, 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	RepairFunc_GetRepairEdge(imgData, xsize, ysize, edge_list);
	GDALClose(m_datasetMask);

	//获取边界的最大最小值
	int minx = 99999, miny = 99999, maxx = -99999, maxy = -99999;
	for (int i = 0; i < edge_list.size(); ++i)
	{
		minx = min(edge_list[i].x, minx);
		maxx = max(edge_list[i].x, maxx);
		miny = min(edge_list[i].y, miny);
		maxy = max(edge_list[i].y, maxy);
	}
	int sizex = (maxx - minx) / 2, sizey = (maxy - miny) / 2;
	int minxex, minyex, maxxex, maxyex;
	if (minx > sizex)
		minxex = minx - sizex;
	if (miny > sizey)
		minyex = miny - sizey;
	if (xsize - maxx > sizex)
		maxxex = maxx + sizex;
	if (ysize - maxy > sizey)
		maxyex = maxy + sizey;

	//然后统计待修复区域和正确区域的均值
	GDALDatasetH m_datasetSrc = GDALOpen(pathImgPCA, GA_ReadOnly);
	int bands = GDALGetRasterCount(m_datasetSrc);
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathPCAIntensity, xsize, ysize, bands, GDT_Float32, NULL);

	for (int i = 0; i < bands; ++i)
	{
		double meanRight = 0, meanRepair = 0;
		int numRigth = 0, numRepair = 0;

		printf("process bands %d\n", i + 1);
		GDALRasterIO(GDALGetRasterBand(m_datasetSrc, i + 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
		for (int i = minxex; i < maxxex; ++i)
		{
			for (int j = minyex; j < maxyex; ++j)
			{
				if (i >= minx&&i<=maxx&&j>=miny&&j <= maxy)
				{
					meanRepair += (imgData[j*xsize+i]);
					numRepair++;
				}
				else
				{
					meanRight += (imgData[j*xsize + i]);
					numRigth++;
				}
			}
		}
		//色调调整
		double ratio = 0;
		if (meanRepair != 0 && meanRight != 0)
		{
			//if((meanRight>0&&meanRight>0)||(meanRepair<0&&meanRight<0))
				ratio = (meanRight / double(numRigth)) / (meanRepair / double(numRepair));
			//else
			//	ratio = (meanRight / double(numRigth)) - (meanRepair / double(numRepair));

		}
		else
		{
			GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
			continue;
		}

		for (int i = minxex; i < maxxex; ++i)
		{
			for (int j = minyex; j < maxyex; ++j)
			{
				if (i >= minx&&i<=maxx&&j>=miny&&j <= maxy)
				{
					//if ((meanRight>0 && meanRight>0) || (meanRepair<0 && meanRight<0))
						imgData[j*xsize + i]*= ratio;
					//else
					//	imgData[j*xsize + i] += ratio;
				}
			}
		}
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	}
	GDALClose(m_datasetDst);
	GDALClose(m_datasetSrc);
	delete[]imgData; imgData = NULL;

}

//==============================================================================//
void HyperRepairFunc::RepairFunc_SetLossBands(int st, int end)
{
	m_HyperRepairFunc_st = st;
	m_HyperRepairFunc_end = end;
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

void HyperRepairFunc::RepairFunc_RepairSpectral(int* dataImg, int posx, int posy, int xsize, int ysize, vector<int>matchPos, vector<float> matchWeight)
{
	dataImg[posy*xsize + ysize] = 0;
	double *tmp = new double[m_HyperRepairFunc_end - m_HyperRepairFunc_st + 1];
	memset(tmp, 0, sizeof(double)*(m_HyperRepairFunc_end - m_HyperRepairFunc_st + 1));
	for (int i = 0; i < matchPos.size(); ++i)
	{
		int tmpPosx = matchPos[i] % xsize;
		int tmpPosy = matchPos[i] / xsize;
		for (int j = m_HyperRepairFunc_st; j < m_HyperRepairFunc_end+1; ++j)
			tmp[j]+= double(dataImg[tmpPosy*xsize + tmpPosx + j*xsize*ysize]) * double(matchWeight[i]);
	}
	for (int j = m_HyperRepairFunc_st; j < m_HyperRepairFunc_end + 1; ++j)
		dataImg[posy*xsize + posx + j*xsize*ysize] = tmp[j];
	delete[]tmp;
}

//==============================================================================//
void HyperRepairFunc::RepairFunc_FirstPosition(float* imgData, float* maskRepair, int xsize, int ysize, CPOINT &firstPos)
{
	//首先获取待修复区域的边界像素
	vector<CPOINT> m_edge_list;
	for (int i = 1; i < xsize - 1; ++i)
	{
		for (int j = 1; j < ysize - 1; ++j)
		{
			if (maskRepair[j*xsize + i] == -1 && (maskRepair[j*xsize + i + 1] != -1 || maskRepair[j*xsize + i - 1] != -1
				|| maskRepair[(j + 1)*xsize + i] != -1 || maskRepair[(j + 1)*xsize + i - 1] != -1 || maskRepair[(j + 1)*xsize + i + 1] != -1
				|| maskRepair[(j - 1)*xsize + i] != -1 || maskRepair[(j - 1)*xsize + i - 1] != -1 || maskRepair[(j - 1)*xsize + i - 1] != -1))
			{
				CPOINT tmpPnt;
				tmpPnt.x = i; tmpPnt.y = j;
				m_edge_list.push_back(tmpPnt);
			}
		}
	}

	//遍历边界像素获取最需要修复的图像的位置
	//有两个标准，周围包含最多像素值或者标准差，或者纹理丰度的评价指标

	//置信度权重
	float* confidenceWeight = new float[m_edge_list.size()];
	//纹理丰度权重
	float* textureWeight = new float[m_edge_list.size()];

	//计算置信度权重

	for (int i = 0; i < m_edge_list.size(); ++i)
	{
		int num = 0;
		int x = m_edge_list[i].x;
		int y = m_edge_list[i].y;
		//8邻域
		if (maskRepair[(y - 1)*xsize + x] != -1)
			num++;
		if (maskRepair[(y +1)*xsize + x] != -1)
			num++;
		if (maskRepair[(y - 1)*xsize + x-1] != -1)
			num++;
		if (maskRepair[(y)*xsize + x-1] != -1)
			num++;
		if (maskRepair[(y + 1)*xsize + x-1] != -1)
			num++;
		if (maskRepair[(y - 1)*xsize + x+1] != -1)
			num++;		
		if (maskRepair[(y)*xsize + x+1] != -1)
			num++;		
		if (maskRepair[(y + 1)*xsize + x+1] != -1)
			num++;
		confidenceWeight[i] = float(num) / 8.0f;
	}


	//计算纹理丰度权重（纹理丰度权的计算采用一定大小窗口内非待修复位置标准差计算）
	const int windsize = 10;	//自定义窗口大小
	vector<float> imgWin;
	for (int i = 0; i < m_edge_list.size(); ++i)
	{
		int x = m_edge_list[i].x;
		int y = m_edge_list[i].y;
		for (int i = x - windsize; i < x + windsize; ++i)
		{
			for (int j = y - windsize; j < y + windsize; ++j)
			{
				if (maskRepair[j*xsize + i] != -1)
					imgWin.push_back(imgData[j*xsize + i]);
			}
		}
		double sum = std::accumulate(std::begin(imgWin), std::end(imgWin), 0.0);
		double mean = sum / imgWin.size(); //均值  
		double accum = 0.0;
		std::for_each(std::begin(imgWin), std::end(imgWin), [&](const double d) {
			accum += (d - mean)*(d - mean);
		});
		double stdev = sqrt(accum / (imgWin.size() - 1)); //标准差 
		textureWeight[i] = stdev;
		imgWin.clear();
	}

	//求最优先位置的下标
	int idx = 0; double max = -1;
	for (int i = 0; i < m_edge_list.size(); ++i)
	{
		if (max < confidenceWeight[i] * textureWeight[i])
		{
			idx = i;
			max = confidenceWeight[i] * textureWeight[i];
		}
	}
	firstPos.x = m_edge_list[idx].x;
	firstPos.y = m_edge_list[idx].y;
	delete[]confidenceWeight;
	delete[]textureWeight;
}

void HyperRepairFunc::RepairFunc_ExemplarPos(float* imgData, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos)
{
	//首先找到包含需要处理位置区域且包含最少待修复点的掩膜区
	int windsize = 7;

	//修复
	//首先根据待修复点在修复区域的哪个角点判断中心像素应该的坐标
	CPOINT tmpCenterPos;
	tmpCenterPos.x = firstPos.x ;
	tmpCenterPos.y = firstPos.y ;

	//然后进行模版匹配校正
	float *regionmask = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	memset(regionmask, 0, sizeof(float) * (2 * windsize + 1)*(2 * windsize + 1));

	float* data1 = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	float* data2 = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	int num = 0;
	//获取待修复区域模板
	for (int i = tmpCenterPos.x -windsize; i <= tmpCenterPos.x + windsize; ++i)
	{
		for (int j = tmpCenterPos.y - windsize; j <= tmpCenterPos.y + windsize; ++j)
		{
			if (maskRepair[j * xsize + i] != -1)
			{
				data1[num] = imgData[j * xsize + i];
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize)] =1;
				num++;
			}
			else
			{
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize)] = 0;
			}
		}
	}
	//获取匹配区域模板像素值 由于要遍历整个影像所以效率会比较低
	double maxrel = 999999999; 
	int maxidx = 0, maxidy = 0;
	for (int i = 0; i <xsize - 2 * windsize - 1; ++i)
	{
		for (int j = 0; j <ysize - 2 * windsize - 1; ++j)
		{
			bool is = false;
			num = 0;
			for (int m = 0; m < (2 * windsize + 1); m++)
			{
				for (int n = 0; n < (2 * windsize + 1); n++)
				{
					if (maskRepair[(j + n)*xsize + i + m] == -1)
						is = true;
					else
					{
						if (regionmask[n*(2 * windsize + 1) + m] == 1)
						{
							data2[num] = imgData[(j + n)*xsize + i + m];
							num++;
						}
					}

				}
			}//内层循环

			//必须不包含任何待修复区域中的点以免出现重复纹理效应
			if (!is)
			{
				//空间值的相似性
				float tmp = GetSSD(data1, data2, num);
				if (tmp<maxrel)
				{
					maxrel = tmp;
					maxidx = i; maxidy = j;
				}
			}

		}
	}//遍历整个影像的循环

	//进行修复
	maskRepair[firstPos.y*xsize + firstPos.x] = 1;
	imgData[firstPos.y* xsize + firstPos.x] = imgData[(windsize + maxidy)* xsize + windsize + maxidx];
	printf("best match pos (%d,%d)\r", maxidx, maxidy);
	//内存清空
	delete[]regionmask;
	delete[]data1;
	delete[]data2;
}

void HyperRepairFunc::RepairFunc_ExemplarPosSpectral(float* imgData, float*imgDataPrv1, float* imgDataPrv2, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos)
{
	int windsize = 4;
	CPOINT tmpCenterPos;
	tmpCenterPos.x = firstPos.x /*+ windsize-1*/;
	tmpCenterPos.y = firstPos.y/* + windsize-1*/;
	float *regionmask = new float[(2 * windsize + 1)*(2 * windsize + 1)*3];
	memset(regionmask, 0, sizeof(float) * (2 * windsize + 1)*(2 * windsize + 1)*3);
	float* data1 = new float[(2 * windsize + 1)*(2 * windsize + 1)*3];
	float* data2 = new float[(2 * windsize + 1)*(2 * windsize + 1)*3];
	int num = 0;

	//获取待修复区前两个波段是否存在
	if (imgDataPrv1 == NULL)
	{
		memset(regionmask, 0, sizeof(float)*(2 * windsize + 1)*(2 * windsize + 1));
	}
	else
	{
		memset(regionmask, 1, sizeof(float)*(2 * windsize + 1)*(2 * windsize + 1));
		for (int i = tmpCenterPos.x - windsize; i <= tmpCenterPos.x + windsize; ++i)
		{
			for (int j = tmpCenterPos.y - windsize; j <= tmpCenterPos.y + windsize; ++j)
			{
				data1[num] = imgDataPrv1[j * xsize + i];
			}
		}
	}
	if (imgDataPrv2 == NULL)
	{
		memset(regionmask + (2 * windsize + 1)*(2 * windsize + 1), 0, sizeof(float)*(2 * windsize + 1)*(2 * windsize + 1));
	}
	else 
	{
		memset(regionmask + (2 * windsize + 1)*(2 * windsize + 1), 1, sizeof(float)*(2 * windsize + 1)*(2 * windsize + 1));
		for (int i = tmpCenterPos.x - windsize; i <= tmpCenterPos.x + windsize; ++i)
		{
			for (int j = tmpCenterPos.y - windsize; j <= tmpCenterPos.y + windsize; ++j)
			{
				data1[num] = imgDataPrv2[j * xsize + i];
			}
		}
	}

	//结合待修复区域前两个波段进行处理
	for (int i = tmpCenterPos.x - windsize; i <= tmpCenterPos.x + windsize; ++i)
	{
		for (int j = tmpCenterPos.y - windsize; j <= tmpCenterPos.y + windsize; ++j)
		{

			if (maskRepair[j * xsize + i] != -1)
			{
				data1[num] = imgData[j * xsize + i];
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize) + 2 * (2 * windsize + 1)*(2 * windsize + 1)] = 1;
				num++;
			}
			else
			{
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize) + 2 * (2 * windsize + 1)*(2 * windsize + 1)] = 0;
			}
		}
	}

	//获取匹配区域模板像素值 由于要遍历整个影像所以效率会比较低
	double maxrel = 999999999;
	int maxidx = 0, maxidy = 0;
	for (int i = 0; i <xsize - 2 * windsize - 1; ++i)
	{
		for (int j = 0; j <ysize - 2 * windsize - 1; ++j)
		{
			bool is = false;
			num = 0;
			
			for (int m = 0; m < (2 * windsize + 1); m++)
			{
				for (int n = 0; n < (2 * windsize + 1); n++)
				{
					if (imgDataPrv1 != NULL)
					{
						data2[num] = imgDataPrv1[(j + n)*xsize + i + m];
						num++;
					}
					if(imgDataPrv2 != NULL)
					{
						data2[num] = imgDataPrv2[(j + n)*xsize + i + m];
						num++;
					}
					if (maskInitial[(j + n)*xsize + i + m] == -1)
						is = true;
					else
					{
						if (regionmask[n*(2 * windsize + 1) + m] == 1)
						{
							data2[num] = imgData[(j + n)*xsize + i + m];
							num++;
						}
					}

				}
			}//内层循环

			 //必须不包含任何待修复区域中的点以免出现重复纹理效应
			if (!is)
			{
				//空间值的相似性
				float tmp = GetSSD(data1, data2, num);
				if (tmp<maxrel)
				{
					maxrel = tmp;
					maxidx = i; maxidy = j;
				}
			}

		}
	}//遍历整个影像的循环
	 //进行修复
	maskRepair[firstPos.y*xsize + firstPos.x] = 1;
	imgData[firstPos.y* xsize + firstPos.x] = imgData[(windsize + maxidy)* xsize + windsize + maxidx];
	printf("best match pos (%d,%d)\r", maxidx, maxidy);
	//内存清空
	delete[]regionmask;
	delete[]data1;
	delete[]data2;
}

void HyperRepairFunc::RepairFunc_GetBestMacthPos(float* imgData, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos, CPOINT &bestPos)
{
	//首先找到包含需要处理位置区域且包含最少待修复点的掩膜区
	int windsize = 13;
	int exwindsize = 20;

	CPOINT tmpCenterPos;
	tmpCenterPos.x = firstPos.x;
	tmpCenterPos.y = firstPos.y;

	//然后进行模版匹配校正	//在这里不仅要考虑模板还要考虑Hellinger距离
	float *regionmask = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	memset(regionmask, 0, sizeof(float) * (2 * windsize + 1)*(2 * windsize + 1));

	float* data1 = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	float* data2 = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	int num = 0;
	//获取待修复区域模板
	for (int i = tmpCenterPos.x - windsize; i <= tmpCenterPos.x + windsize; ++i)
	{
		for (int j = tmpCenterPos.y - windsize; j <= tmpCenterPos.y + windsize; ++j)
		{
			if (maskRepair[j * xsize + i] != -1)
			{
				data1[num] = imgData[j * xsize + i];
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize)] = 1;
				num++;
			}
			else
			{
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize)] = 0;
			}
		}
	}

	//获取匹配区域模板像素值 由于要遍历整个影像所以效率会比较低
	double maxrel = 999999999;
	int maxidx = 0, maxidy = 0;
	for (int i = 0; i <xsize - 2 * windsize - 1; ++i)
	{
		for (int j = 0; j <ysize - 2 * windsize - 1; ++j)
		{
			bool is = false;
			num = 0;
			for (int m = 0; m < (2 * windsize + 1); m++)
			{
				for (int n = 0; n < (2 * windsize + 1); n++)
				{
					if (maskInitial[(j + n)*xsize + i + m] == -1)
						is = true;
					else
					{
						if (regionmask[n*(2 * windsize + 1) + m] == 1)
						{
							data2[num] = imgData[(j + n)*xsize + i + m];
							num++;
						}
					}

				}
			}//内层循环

			 //必须不包含任何待修复区域中的点以免出现重复纹理效应
			if (!is)
			{
				//空间值的相似性
				float tmp = GetSSD(data1, data2, num)/*GetCoefficient(data1,data2,num)*/;
				if (tmp<maxrel)
				{
					maxrel = tmp;
					maxidx = i; maxidy = j;
				}
			}

		}
	}//遍历整个影像的循环

	 //进行修复
	maskRepair[firstPos.y*xsize + firstPos.x] = 1;
	bestPos.x = maxidx;
	bestPos.y = maxidy;
	imgData[firstPos.y*xsize + firstPos.x] = imgData[maxidy*xsize + maxidx];
	printf("best match pos (%d,%d)\r", maxidx, maxidy);
	//内存清空
	delete[]regionmask;
	delete[]data1;
	delete[]data2;
}

//通过Hellinger距离计算得到的最佳匹配点
void HyperRepairFunc::RepairFunc_GetBestMacthPosWithHellinger(float* imgData, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos, CPOINT &bestPos)
{
	//首先找到包含需要处理位置区域且包含最少待修复点的掩膜区
	int windsize   = 9;		//计算SSD的窗口打啊小
	int exwindsize = 15;	//计算Hellinger距离的扩展窗口的大小

	//修复
	//首先根据待修复点在修复区域的哪个角点判断中心像素应该的坐标
	CPOINT tmpCenterPos;
	tmpCenterPos.x = firstPos.x ;
	tmpCenterPos.y = firstPos.y ;

	//然后进行模版匹配校正	//在这里不仅要考虑模板还要考虑Hellinger距离
	float *regionmask = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	memset(regionmask, 0, sizeof(float) * (2 * windsize + 1)*(2 * windsize + 1));
	float* data1 = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	float* data2 = new float[(2 * windsize + 1)*(2 * windsize + 1)];
	float* dataHellinger1 = new float[(2 * exwindsize + 1)*(2 * exwindsize + 1)];
	float* dataHellinger2 = new float[(2 * exwindsize + 1)*(2 * exwindsize + 1)];

	int num = 0;
	//获取待修复区域模板
	for (int i = tmpCenterPos.x - windsize; i <= tmpCenterPos.x + windsize; ++i)
	{
		for (int j = tmpCenterPos.y - windsize; j <= tmpCenterPos.y + windsize; ++j)
		{
			if (maskRepair[j * xsize + i] != -1)
			{
				data1[num] = imgData[j * xsize + i];
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize)] = 1;
				num++;
			}
			else
			{
				regionmask[(j - (tmpCenterPos.y - windsize))*(2 * windsize + 1) + i - (tmpCenterPos.x - windsize)] = 0;
			}
		}
	}
	//获取待修复区域扩展模板大小
	int numex = 0;
	for (int i = tmpCenterPos.x - exwindsize; i <= tmpCenterPos.x + exwindsize; ++i)
	{
		for (int j = tmpCenterPos.y - exwindsize; j <= tmpCenterPos.y + exwindsize; ++j)
		{
			if (maskRepair[j * xsize + i] != -1)
			{
				dataHellinger1[numex] = imgData[j * xsize + i];
				numex++;
			}
		}
	}

	//在整个影像上进行匹配
	//获取匹配区域模板像素值 由于要遍历整个影像所以效率会比较低
	double maxrel = 999999999;
	int maxidx = 0, maxidy = 0;
	for (int i = 0; i <xsize - 2 * exwindsize - 1; ++i)
	{
		for (int j = 0; j <ysize - 2 * exwindsize - 1; ++j)
		{
			bool is = false;
			num = 0;
			int tmpex = 0;
			for (int m = 0; m < (2 * windsize + 1); m++)
			{
				for (int n = 0; n < (2 * windsize + 1); n++)
				{
					if (maskInitial[(j + n)*xsize + i + m] == -1)
						is = true;
					else
					{
						if (regionmask[n*(2 * windsize + 1) + m] == 1)
						{
							data2[num] = imgData[(j + n)*xsize + i + m];
							num++;
						}
					}

				}
			}//内层循环获取SSD匹配

			for (int m = 0; m < (2 * exwindsize + 1); m++)
			{
				for (int n = 0; n < (2 * exwindsize + 1); n++)
				{
					if (maskInitial[(j + n)*xsize + i + m] == -1)
						is = true;
					else
					{
						dataHellinger2[n*(2*exwindsize+1)+m] = imgData[(j + n)*xsize + i + m];
					}

				}
			}//内层循环获取SSD匹配


			 //必须不包含任何待修复区域中的点以免出现重复纹理效应
			if (!is)
			{
				//空间值的相似性
				float tmp = GetSSD(data1, data2, num)*GetHellingerDistance(dataHellinger1, numex, dataHellinger2, (2 * exwindsize + 1)*(2 * exwindsize + 1));
				if (tmp<maxrel)
				{
					maxrel = tmp;
					maxidx = i; maxidy = j;
				}
			}

		}
	}//遍历整个影像的循环

	 //进行修复
	maskRepair[firstPos.y*xsize + firstPos.x] = 1;
	bestPos.x = maxidx;
	bestPos.y = maxidy;
	imgData[firstPos.y*xsize + firstPos.x] = imgData[maxidy*xsize + maxidx];
	printf("best match pos (%d,%d)\r", maxidx, maxidy);
	//内存清空
	delete[]regionmask;
	delete[]data1;
	delete[]data2;
	delete[]dataHellinger1;
	delete[]dataHellinger2;
}

bool HyperRepairFunc::RepairFunc_ExemplarCheckTerminal(float* maskRepair, int xsize, int ysize)
{
	for (int i = 0; i < xsize*ysize; ++i)
	{
		if (maskRepair[i] == -1)
			return false;
	}
	return true;
}

void HyperRepairFunc::RepairFunc_GetRepairEdge(float* maskRepair, int xsize, int ysize, vector<CPOINT> &edge_list)
{
	for (int i = 1; i < xsize - 1; ++i)
	{
		for (int j = 1; j < ysize - 1; ++j)
		{
			if (maskRepair[j*xsize + i] == -1 && (maskRepair[j*xsize + i + 1] != -1 || maskRepair[j*xsize + i - 1] != -1
				|| maskRepair[(j + 1)*xsize + i] != -1 || maskRepair[(j + 1)*xsize + i - 1] != -1 || maskRepair[(j + 1)*xsize + i + 1] != -1
				|| maskRepair[(j - 1)*xsize + i] != -1 || maskRepair[(j - 1)*xsize + i - 1] != -1 || maskRepair[(j - 1)*xsize + i - 1] != -1))
			{
				CPOINT tmpPnt;
				tmpPnt.x = i; tmpPnt.y = j;
				edge_list.push_back(tmpPnt);
			}
		}
	}
}

void HyperRepairFunc::RepairFunc_RepairEdge(float* imgData, float* maskInitial, float* maskRepair, vector<CPOINT> &edge_list, int xsize, int ysize)
{
	//对所有边界像素进行修复
	for (int i = 0; i < edge_list.size(); ++i)
	{
		//RepairFunc_ExemplarPos(imgData, maskInitial, maskRepair, xsize, ysize, edge_list[i]);
		CPOINT pnts;
		RepairFunc_GetBestMacthPosWithHellinger(imgData, maskInitial, maskRepair, xsize, ysize, edge_list[i], pnts);
	}
	edge_list.clear();
}

void HyperRepairFunc::RepairFunc_IterativeRefine(float* imgData, float* maskInitial, int xsize, int ysize)
{
	//迭代精化的策略为计算
	//首先获取待修复图像的边界，然后根据图像边界进行修复，依次调整
	float* tmpMask = new float[xsize*ysize];
	memcpy(tmpMask, maskInitial, sizeof(float)*xsize*ysize);
	vector<CPOINT> edge_list;
	RepairFunc_GetRepairEdge(tmpMask,xsize,ysize,edge_list);


}

//我认为还是应该先进行分解，将图像分解为问题影像和强度影像然后进行修复
//如果是投影的话应该先投影，然后强度调整，然后主成份进行分解修复，然后次要成分进行修复，然后进行光谱匹配修复
//这样才是一个完整的修复流程
void HyperRepairFuncTest()
{
	char* pathImg = "D:\\my_doc\\2015.10.20数据\\hyper\\hypertest";
	char* pathSimu = "D:\\my_doc\\2015.10.20数据\\hyper\\hypertestSimulateMore";
	char* pathRepair = "D:\\my_doc\\2015.10.20数据\\hyper\\repairSpatialMore.tif";

	char* pathPCA = "D:\\my_doc\\2015.10.20数据\\hyper\\PCASpatialMore.tif";
	char* pathPCAIntentity= "D:\\my_doc\\2015.10.20数据\\hyper\\PCASpatialIntity.tif";
	char* pathMatrix = "D:\\my_doc\\2015.10.20数据\\hyper\\PCAMatrix.txt";

	char* pathMaskImg = "D:\\my_doc\\2015.10.20数据\\hyper\\msk.bmp";
	char* pathMaskDst = "D:\\my_doc\\2015.10.20数据\\hyper\\mskAllNarrow.bmp";
	CPOINT pnt1, pnt2;
	pnt1.x = 164; pnt1.y = 267;
	pnt2.x = 245; pnt2.y = 287;
	HyperRepairFunc m_Repair;
	//set_mask_region(pathMaskImg, pathMaskDst,pnt1,pnt2);
	//m_Repair.RepairFunc_SimuliDegradImg(pathImg, pathSimu, 1, 201, pnt1, pnt2);
	//m_Repair.RepairFunc_SpectralRun(pathSimu, pathMaskImg, pathRepair, 2, 90);
	//m_Repair.RepairFunc_PCATrans(pathSimu, pathPCA, pathMatrix);
	//m_Repair.RepairFunc_PCAIntensity(pathPCA, pathMaskImg, pathPCAIntentity);
	m_Repair.RepairFunc_EdgePriorityRun(pathSimu, pathMaskImg, pathRepair, 2, 2);
	//char* pathHyper = "D:\\my_doc\\2015.10.20数据\\hyper\\hyper.tif";
	//char* pathLow = "D:\\my_doc\\2015.10.20数据\\hyper\\low.tif";
	//m_Repair.RepairFunc_ImgDecompose(pathSimu, pathHyper, pathLow, 0);
}