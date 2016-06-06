#include"DCTTrans.h"
#include"..\Global.h"
#include<math.h>
#include<omp.h>
#include "..\gdal/include/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

//1维离散DCT变换
void DCT1D(float* dataIn, int length, float* DCTData)
{
	for (int i = 0; i < length; ++i)
	{
		DCTData[i] = 0;
		for (int j = 0; j < length; ++j)
		{
			DCTData[i] += dataIn[j] * cos(PI*(2 * j + 1)*i / 2 / length);
		}
		if (i == 0)
			DCTData[i] = sqrt(1.0f / float(length))*DCTData[i];
		else
			DCTData[i] = sqrt(2.0f / float(length))*DCTData[i];
	}
}
void IDCT1D(float* DCTData, int length, float* IDCTData)
{
	for (int i = 0; i < length; ++i)
	{
		IDCTData[i] = 0;
		for (int j = 0; j < length; ++j)
		{
			if (j == 0)
				IDCTData[i] += sqrt(1.0f / float(length))*DCTData[j] * cos(float(PI*(i + 0.5)*j)  / float(length));
			if (j != 0)
				IDCTData[i] += sqrt(2.0f / float(length))*DCTData[j] * cos(float(PI*(i + 0.5)*j)  / float(length));
		}
	}
}

//2维离散DCT变换
void DCT2D(float* dataIn, int xsize, int ysize, float* DCTData)
{
	if (xsize != ysize)
	{
		printf("input matrix is not a square！\n");
	}
	memset(DCTData, 0, sizeof(float)*xsize*ysize);
	float *param = new float[xsize*xsize];
	for (int i = 0; i < xsize; ++i)
		for (int j = 0; j < xsize; ++j)
			param[j*xsize + i] = cos(float(float((i + 0.5)*PI*j) / float(xsize)));
#pragma omp parallel for
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			//内部循环
			float tmp1, tmp2;
			if (i == 0)
				tmp1 = sqrt(float(1) / float(xsize));
			else
				tmp1 = sqrt(float(2) / float(xsize));
			if (j == 0)
				tmp2 = sqrt(float(1) / float(xsize));
			else
				tmp2 = sqrt(float(2) / float(xsize));

			double tmp = 0;
			for (int m = 0; m < xsize; ++m)
			{
				for (int n = 0; n < ysize; ++n)
				{
					tmp += float(dataIn[n*xsize + m]) *param[i*xsize+m]*param[j*xsize+n];
				}
			}
			DCTData[j*xsize + i] = tmp*tmp1*tmp2;
		}
	}
	delete[]param;
	param = NULL;
}
void IDCT2D(float* DCTData, int xsize, int ysize, float* IDCTData)
{
	if (xsize != ysize)
	{
		printf("input matrix is not a square！\n");
	}
	memset(IDCTData, 0, sizeof(float)*xsize*ysize);
	float *param = new float[xsize*xsize];
	for (int i = 0; i < xsize; ++i)
		for (int j = 0; j < xsize; ++j)
			param[j*xsize + i] = cos(float(float((i + 0.5)*PI*j) / float(xsize)));
#pragma omp parallel for
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			float tmp = 0;
			for (int m = 0; m < xsize; ++m)
			{
				for (int n = 0; n < ysize; ++n)
				{
					float tmp1, tmp2;
					if(m==0)
						tmp1 = sqrt(float(1) / float(xsize));
					else
						tmp1 = sqrt(float(2) / float(xsize));
					if(n==0)
						tmp2 = sqrt(float(1) / float(ysize));
					else
						tmp2 = sqrt(float(2) / float(ysize));
					tmp += tmp1*tmp2*float(DCTData[n*xsize + m]) *param[m*xsize + i] * param[n*xsize + j];
				}
			}
			IDCTData[j*xsize + i] = tmp;
		}
	}
	delete[]param;
	param = NULL;
}

//3维离散DCT变换
void DCT3D(float* dataIn, int xsize, int ysize, int bands, float* DCTData)
{
	if (xsize != ysize)
	{
		printf("input matrix is not a square！\n");
	}

	float* dataTemp = new float[bands];
	float* dataTempDCT = new float[bands];
	for (int i = 0; i < bands; ++i)
		DCT2D(dataIn+i*xsize*ysize, xsize, ysize, DCTData+i*xsize*ysize);
	for (int i = 0; i < xsize*ysize; ++i)
	{
		for (int j = 0; j < bands; ++j)
			dataTemp[j] = DCTData[j*xsize*ysize + i];
		DCT1D(dataTemp, bands, dataTempDCT);
		for (int j = 0; j < bands; ++j)
			DCTData[j*xsize*ysize + i] = dataTempDCT[j];
	}
	delete[]dataTemp;
	delete[]dataTempDCT;
}
void IDCT3D(float* DCTData, int xsize, int ysize, int bands, float* IDCTData)
{
	if (xsize != ysize)
	{
		printf("input matrix is not a square！\n");
	}
	float* dataTemp = new float[bands];
	float* dataTempDCT = new float[bands];
	float* dataIDCT = new float[xsize*ysize];
	for (int i = 0; i < xsize*ysize; ++i)
	{
		for (int j = 0; j < bands; ++j)
			dataTemp[j] = DCTData[j*xsize*ysize + i];
		IDCT1D(dataTemp, bands, dataTempDCT);
		for (int j = 0; j < bands; ++j)
			IDCTData[j*xsize*ysize + i] = dataTempDCT[j];
	}

	for (int i = 0; i < bands; ++i)
	{
		IDCT2D(IDCTData+i*xsize*ysize,xsize,ysize, dataIDCT);
		memcpy(IDCTData + i*xsize*ysize, dataIDCT, sizeof(float)*xsize*ysize);
	}
	delete[]dataTemp;
	delete[]dataTempDCT;
	delete[]dataIDCT;
}	 

//图像离散DCT变换
//某一个波段
void DCT2D(const char* pathImgIn, const char* pathOut, int bandIdx)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen(pathImgIn, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);
	if (bandIdx > bands)
		return;
	float* data = new float[xsize*ysize];
	float* dataDCT = new float[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_datasetin, bandIdx), GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, GDT_Float32, 0, 0);
	DCT2D(data, xsize, ysize, dataDCT);
	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetout, 1), GF_Write, 0, 0, xsize, ysize, dataDCT, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetout);
	GDALClose(m_datasetin);
	delete[]data;
	delete[]dataDCT;
}
void IDCT2D(const char* pathImgIn, const char* pathOut, int bandIdx)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen(pathImgIn, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);
	if (bandIdx > bands)
		return;
	float* dataDCT = new float[xsize*ysize];
	float* dataIDCT = new float[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_datasetin, bandIdx), GF_Read, 0, 0, xsize, ysize, dataDCT, xsize, ysize, GDT_Float32, 0, 0);
	IDCT2D(dataDCT, xsize, ysize, dataIDCT);
	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetout, 1), GF_Write, 0, 0, xsize, ysize, dataIDCT, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetout);
	GDALClose(m_datasetin);
	delete[]dataDCT;
	delete[]dataIDCT;
}
//所有波段
void DCT3D(const char* pathImgIn, const char* pathOut)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen(pathImgIn, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);

	//空间申请应该先try一下，算球了以后再说
	float* data = new float[xsize*ysize*bands];
	float* dataDCT = new float[xsize*ysize*bands];
	for (int i = 0; i < bands; ++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetin, i+1), GF_Read, 0, 0, xsize, ysize, data + i*xsize*ysize,  xsize, ysize, GDT_Float32, 0, 0);
	DCT3D(data, xsize, ysize, bands, dataDCT);
	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize, ysize, bands, GDT_Float32, NULL);
	for (int i = 0; i < bands; ++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetout, i + 1), GF_Write, 0, 0, xsize, ysize, dataDCT + i*xsize*ysize, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetin);
	GDALClose(m_datasetout);
	delete[]data;
	delete[]dataDCT;
}
void IDCT3D(const char* pathImgIn, const char* pathOut)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen(pathImgIn, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);

	//空间申请应该先try一下，算球了以后再说
	float* dataDCT = new float[xsize*ysize*bands];
	float* dataIDCT = new float[xsize*ysize*bands];
	for (int i = 0; i < bands; ++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetin, i + 1), GF_Read, 0, 0, xsize, ysize, dataDCT+i*xsize*ysize, xsize, ysize, GDT_Float32, 0, 0);
	IDCT3D(dataDCT, xsize, ysize, bands, dataIDCT);
	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize, ysize, bands, GDT_Byte, NULL);
	for (int i = 0; i < bands; ++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetout, i + 1), GF_Write, 0, 0, xsize, ysize, dataIDCT + i*xsize*ysize, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetin);
	GDALClose(m_datasetout);
	delete[]dataDCT;
	delete[]dataIDCT;
}

//图像DCT滤波
void DCTFliter2D(const char* pathImgIn, const char* pathOut, int bandIdx, float thresthod)
{
	DCT2D(pathImgIn, "~temp.tif", bandIdx);
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen("~temp.tif", GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);
	if (bandIdx > bands)
		return;
	float* dataDCT = new float[xsize*ysize];
	float* dataIDCT = new float[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_datasetin, 1), GF_Read, 0, 0, xsize, ysize, dataDCT, xsize, ysize, GDT_Float32, 0, 0);
	for (int i = 0; i < xsize*ysize; ++i)
	{
		if (abs(dataDCT[i]) > thresthod)
			dataDCT[i] = 0;
	}
	IDCT2D(dataDCT, xsize, ysize, dataIDCT);
	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetout, 1), GF_Write, 0, 0, xsize, ysize, dataIDCT, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetout);
	GDALClose(m_datasetin);
	delete[]dataDCT;
	delete[]dataIDCT;

	remove("~temp.tif");
}
void DCTFliter3D(const char* pathImgIn, const char* pathOut, float thresthod)
{
	DCT3D(pathImgIn, "~temp.tif");
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen("~temp.tif", GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);

	//内存申请会不会爆掉，不管了!!
	float* dataDCT = new float[xsize*ysize*bands];
	float* dataIDCT = new float[xsize*ysize*bands];
	for (int i = 0; i < bands; ++i)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetin, i+1), GF_Read, 0, 0, xsize, ysize, dataDCT+i*xsize*ysize, xsize, ysize, GDT_Float32, 0, 0);
	}

	for (int i = 0; i < xsize*ysize; ++i)
	{
		if (abs(dataDCT[i]) < thresthod)
			dataDCT[i] = 0;
	}
	IDCT3D(dataDCT, xsize, ysize, bands, dataIDCT);
	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathOut, xsize, ysize, bands, GDT_Float32, NULL);
	for (int i = 0; i < bands; ++i)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetout, i+1), GF_Write, 0, 0, xsize, ysize, dataIDCT + i*xsize*ysize, xsize, ysize, GDT_Float32, 0, 0);
	}
	GDALClose(m_datasetin);
	GDALClose(m_datasetout);
	delete[]dataDCT;
	delete[]dataIDCT;
}