#include"stdafx.h"
#include"AuxiliaryFunction.h"
#include<fstream>
#include<iostream>  
#include<io.h>  
#include <fstream>
using namespace std;
//写ENVI头文件到文件中
void WriteENVIHeader(const char* pathHeader, ENVIHeader mImgHeader)
{
	FILE* fpHead = NULL;
	errno_t err = fopen_s(&fpHead, pathHeader, "w");
	if (err != 0)
		exit(-1);

	if (!fpHead)
		exit(-1);

	fprintf(fpHead, "%s\n%s\n%s\n", "ENVI", "description = {", "File Imported into ENVI.}");
	fprintf(fpHead, "samples = %d\n", mImgHeader.imgWidth);					//写入行像元数
	fprintf(fpHead, "lines = %d\n", mImgHeader.imgHeight);				    //写入行数
	fprintf(fpHead, "bands = %d\n", mImgHeader.imgBands);					//写入波段数
	fprintf(fpHead, "file type = %s\n", "ENVI Standard");				    //写入标示
	fprintf(fpHead, "data type = %d\n", mImgHeader.datatype);
	fprintf(fpHead, "interleave = %s\n", mImgHeader.interleave.c_str());
	fprintf(fpHead, "sensor type = %s\n", "Unknown");
	fprintf(fpHead, "byte order = %d\n", mImgHeader.byteorder);
	fprintf(fpHead, "wavelength units = %s\n", "Unknown");

	//投影和坐标系统
	if (mImgHeader.bimgGeoProjection)
	{
		fprintf(fpHead, "map info = { %s,", mImgHeader.mapInfo.projection.c_str());
		fprintf(fpHead, "%lf,", mImgHeader.mapInfo.adfGeotransform[1]);
		fprintf(fpHead, "%lf,", mImgHeader.mapInfo.adfGeotransform[5]);
		fprintf(fpHead, "%lf,", mImgHeader.mapInfo.adfGeotransform[0]);
		fprintf(fpHead, "%lf,", mImgHeader.mapInfo.adfGeotransform[3]);
		fprintf(fpHead, "%lf,", mImgHeader.mapInfo.adfGeotransform[2]);
		fprintf(fpHead, "%lf,", mImgHeader.mapInfo.adfGeotransform[4]);
		fprintf(fpHead, "%d,", mImgHeader.mapInfo.zone);
		fprintf(fpHead, "%s\n", mImgHeader.mapInfo.directions.c_str());
		fprintf(fpHead, "%s\n", mImgHeader.coordianteSys.c_str());
	}

	//波长
	if (mImgHeader.bimgWaveLength)
	{
		fprintf(fpHead, "wavelength = {");
		for (int i = 0; i < mImgHeader.imgWaveLength.size(); ++i)
		{
			for (int j = 0; j + i < mImgHeader.imgWaveLength.size(); ++j)
				fprintf(fpHead, "%lf", mImgHeader.imgWaveLength[j + i]);
			fprintf(fpHead, "\n");
		}
		fprintf(fpHead, "}\n");
	}

	//半波宽
	if (mImgHeader.bimgFWHM)
	{
		fprintf(fpHead, "fwhm = {");
		for (int i = 0; i < mImgHeader.imgFWHM.size(); ++i)
		{
			for (int j = 0; j + i < mImgHeader.imgFWHM.size(); ++j)
				fprintf(fpHead, "%lf", mImgHeader.imgFWHM[j + i]);
			fprintf(fpHead, "\n");
		}
		fprintf(fpHead, "}\n");
	}

	//波段名
	if (mImgHeader.bimgBandNames)
	{
		fprintf(fpHead, "band names = {");
		for (int i = 0; i < mImgHeader.imgBandNnames.size(); ++i)
		{
			for (int j = 0; j + i < mImgHeader.imgBandNnames.size(); ++j)
				fprintf(fpHead, "%s", mImgHeader.imgBandNnames[j + i].c_str());
			fprintf(fpHead, "\n");
		}
		fprintf(fpHead, "}\n");
	}

	fclose(fpHead);		//关闭文件
}

//获取均值和标准差
void GetAveDev(unsigned char *pBuffer, int nSamples, int nLines, int nBand, float &fAverage, float &fDeviate)
{
	double dSum = 0, dMul = 0;
	__int64 nCount = 0;
	int i = 0, j = 0;
	int nOffset = 0;
	fAverage = 0;
	fDeviate = 0;
	int nPix = nLines*nSamples;
	for (i = 0; i<nPix; i++)
	{
		nOffset = nBand*nPix + i;
		if (pBuffer[nOffset] != 0)
		{
			dSum += (float)pBuffer[nOffset];
			dMul += (float)(pBuffer[nOffset] * pBuffer[nOffset]);
			nCount++;
		}
	}
	if (nCount == 0)
	{
		return ;
	}
	fAverage = (float)dSum / nCount;
	dMul /= nCount;
	dMul -= fAverage*fAverage;
	fDeviate = (float)sqrt(dMul);
	return ;
}
void GetAveDev(unsigned short *pBuffer, int nSamples, int nLines, int nBand, float &fAverage, float &fDeviate)
{
	double dSum = 0, dMul = 0;
	__int64 nCount = 0;
	int i = 0, j = 0;
	int nOffset = 0;
	fAverage = 0;
	fDeviate = 0;
	int nPix = nLines*nSamples;
	for (i = 0; i<nPix; i++)
	{
		nOffset = nBand*nPix + i;
		if (pBuffer[nOffset] != 0)
		{
			dSum += pBuffer[nOffset];
			dMul += pBuffer[nOffset] * pBuffer[nOffset];
			nCount++;
		}
	}
	if (nCount == 0)
	{
		return;
	}
	fAverage = dSum / nCount;
	dMul /= nCount;
	dMul -= fAverage*fAverage;
	fDeviate = (float)sqrt(dMul);
	return;
}
void GetAveDev(float *pBuffer, int nSamples, int nLines, int nBand, float &fAverage, float &fDeviate)
{
	double dSum = 0, dMul = 0;
	__int64 nCount = 0;
	int i = 0, j = 0;
	int nOffset = 0;
	fAverage = 0;
	fDeviate = 0;
	int nPix = nLines*nSamples;
	for (i = 0; i<nPix; i++)
	{
		nOffset = nBand*nPix + i;
		if (pBuffer[nOffset] != 0)
		{
			dSum += pBuffer[nOffset];
			dMul += pBuffer[nOffset] * pBuffer[nOffset];
			nCount++;
		}
	}
	if (nCount == 0)
	{
		return;
	}
	fAverage = dSum / nCount;
	dMul /= nCount;
	dMul -= fAverage*fAverage;
	fDeviate = (float)sqrt(dMul);
	return;
}

void NormalizeData(float* pBuffer, int length, float* pNormalBuffer)
{
	float* tmpData = new float[length];
	memcpy(tmpData, pBuffer, sizeof(float)*length);
	float fAvg, fDev;
	GetAveDev(tmpData, 1, length, 0, fAvg, fDev);
	if (fabs(fDev)< 0.0000001)
	{
		for (int i = 0; i < length; ++i)
			pNormalBuffer[i] = pBuffer[i] - fAvg;
	}
	else
	{
		for (int i = 0; i < length; ++i)
			pNormalBuffer[i] =( pBuffer[i] - fAvg)/fDev;
	}
	delete[]tmpData;
}
void NormalizeData(double* pBuffer, int length, double* pNormalBuffer)
{
	float* tmpData = new float[length];
	memcpy(tmpData, pBuffer, sizeof(float)*length);
	float fAvg, fDev;
	GetAveDev(tmpData, 1, length, 0, fAvg, fDev);
	if (fabs(fDev)< 0.0000001)
	{
		for (int i = 0; i < length; ++i)
			pNormalBuffer[i] = pBuffer[i] - fAvg;
	}
	else
	{
		for (int i = 0; i < length; ++i)
			pNormalBuffer[i] = (pBuffer[i] - fAvg) / fDev;
	}
	delete[]tmpData;
}
float GetCoefficient(float* data1, float* data2, int num)
{
	float avg1, avg2;
	float dev1, dev2;
	GetAveDev(data1, 1, 5, 0, avg1, dev1);
	GetAveDev(data2, 1, 5, 0, avg2, dev2);

	float tmp = 0;
	for (int i = 0; i < 5; ++i)
	{
		tmp += (float(data1[i]) - avg1)*(float(data2[i]) - avg2);
	}
	return tmp / 5.0f / dev1 / dev2;
}
float GetSSD(float* data1, float* data2, int num)
{
	float tmp = 0;
	for (int i = 0; i < num; ++i)
	{
		tmp += (data1[i] - data2[i])*(data1[i] - data2[i]);
	}
	return sqrt(tmp);
}
float GetHellingerDistance(float* distribution1, int num1, float* distribution2, int num2)
{
	//首先获取最大最小值
	int max1 = -999999, min1 = 999999, max2 = -999999, min2 = 999999;
	for (int i = 0; i < num1; ++i)
	{
		max1 = max(max1, int(distribution1[i]));
		min1 = min(min1, int(distribution1[i]));
	}
	for (int i = 0; i < num2; ++i)
	{
		max2 = max(max2, int(distribution2[i]));
		min2 = min(min2, int(distribution2[i]));
	}

	//获取概率直方图
	float *hist1 = new float[max1 - min1 + 1];
	float *hist2 = new float[max2 - min2 + 1];
	memset(hist1, 0, sizeof(float)*(max1 - min1 + 1));
	memset(hist2, 0, sizeof(float)*(max2 - min2 + 1));

	for (int i = 0; i < num1; ++i)
		hist1[int(distribution1[i]) - min1] += 1.0/float(num1);
	for (int i = 0; i < num2; ++i)
		hist2[int(distribution2[i]) - min2] += 1.0/float(num2);

	int min = max(min1, min2);
	int max = min(max1, max2);
	float tmp = 0;
	for (int i = min; i < max; ++i)
		tmp += sqrt(hist1[i-min1] * hist2[i-min2]);
	delete[]hist1; hist1 = NULL;
	delete[]hist2; hist2 = NULL;
	return sqrt(1.0 - tmp);

}

//对数据进行采样
void GetImgSample(unsigned char *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, unsigned char *pRegBuffer)
{
	int i = 0, j = 0;			//行列循环变量
	DPOINT originPnt;			//影像左上点
	originPnt.dX = minPt.dX;
	originPnt.dY = maxPt.dY;

	float *fDGrey = NULL;			//灰度存储数组
	float *fDItem = NULL;			//权值存储数组
	try
	{
		fDGrey = new float[nReSamples*nReLines];
	}
	catch (bad_alloc)
	{
		exit(-1);
	}
	try
	{
		fDItem = new float[nReSamples*nReLines];
	}
	catch (bad_alloc)
	{
		exit(-1);
	}
	memset(fDGrey, 0, nReSamples*nReLines*sizeof(float));	//初始化化
	memset(fDItem, 0, nReSamples*nReLines*sizeof(float));	//初始化化

	for (i = 0; i<nLines; i++)
	{
#pragma omp parallel for
		for (j = 0; j<nSamples; j++)
		{
			unsigned char fDN = 0;
			float fTempGrey[4];						//定义权值数组	
			int nC = 0, nY = 0;						//坐标取整变量
			double fDX = 0, fDY = 0;				//坐标取余数变量
			DPOINT presentPnt;
			presentPnt.dX = 0;
			presentPnt.dY = 0;

			long lPixOffset = i*nSamples + j;
			//对于原始影像(i,j)坐标为(x,y)，对应新图像位置为
			presentPnt.dX = fabs(pGoundPt[lPixOffset].dX - originPnt.dX) / fGSDX;
			presentPnt.dY = fabs(pGoundPt[lPixOffset].dY - originPnt.dY) / fGSDY;
			//由于presentPnt不是整数，进行取整
			nC = (int)presentPnt.dX;
			nY = (int)presentPnt.dY;
			//由于presentPnt不是整数，进行取余
			fDX = presentPnt.dX - nC;
			fDY = presentPnt.dY - nY;
			fDN = pImgBuffer[lPixOffset];		//获取当前点的原始DN值

			fTempGrey[0] = (float)(1 - fDX)*(1 - fDY)*fDN;
			fTempGrey[1] = (float)fDX*(1 - fDY)*fDN;
			fTempGrey[2] = (float)(1 - fDX)*fDY*fDN;
			fTempGrey[3] = (float)fDX*fDY*fDN;

			if (nC >= 0 && nC<nReSamples && nY >= 0 && nY<nReLines)
			{
				long lOffset = 0;
				lOffset = nY*nReSamples + nC;
				fDGrey[lOffset] += fTempGrey[0];
				fDItem[lOffset] += (1 - fDX)*(1 - fDY);						//左上点
				if (nC < nReSamples - 1)	//未处于右边界
				{
					fDGrey[lOffset + 1] += (float)fTempGrey[1];
					fDItem[lOffset + 1] += (float)fDX*(1 - fDY);					//右上点
				}
				if (nY < nReLines - 1)	//未处于下边界
				{
					fDGrey[lOffset + nReSamples] += (float)fTempGrey[2];
					fDItem[lOffset + nReSamples] += (float)(1 - fDX)*fDY;		//左下点
				}
				if (nC<nReSamples - 1 && nY<nReLines - 1)
				{
					fDGrey[lOffset + nReSamples + 1] += fTempGrey[3];
					fDItem[lOffset + nReSamples + 1] += fDX*fDY;			//右下点
				}
			}
		}
	}
	for (i = 0; i<nReLines; i++)
	{
#pragma omp parallel for
		for (j = 0; j<nReSamples; j++)
		{
			long lOffset = i*nReSamples + j;
			if (fDItem[lOffset] != 0)
			{
				pRegBuffer[lOffset] = unsigned char(fDGrey[lOffset] / fDItem[lOffset]);
			}
			else	//修复黑点
			{
				if (i>0 && i<nReLines - 1 && j>0 && j<nReSamples - 1)	//不处于边界位置
				{
					float fSumValues = 0;
					int nCount = 0;

					if (fDItem[lOffset - nReSamples - 1] != 0)	//左上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples - 1] / fDItem[lOffset - nReSamples - 1];
					}
					if (fDItem[lOffset - nReSamples] != 0)		//上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples] / fDItem[lOffset - nReSamples];
					}
					if (fDItem[lOffset - nReSamples + 1] != 0)	//右上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples + 1] / fDItem[lOffset - nReSamples + 1];
					}
					if (fDItem[lOffset - 1] != 0)					//左
					{
						nCount++;
						fSumValues += fDGrey[lOffset - 1] / fDItem[lOffset - 1];
					}
					if (fDItem[lOffset + 1] != 0)					//右
					{
						nCount++;
						fSumValues += fDGrey[lOffset + 1] / fDItem[lOffset + 1];
					}
					if (fDItem[lOffset + nReSamples - 1] != 0)	//左下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples - 1] / fDItem[lOffset + nReSamples - 1];
					}
					if (fDItem[lOffset + nReSamples] != 0)		//下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples] / fDItem[lOffset + nReSamples];
					}
					if (fDItem[lOffset + nReSamples + 1] != 0)	//右下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples + 1] / fDItem[lOffset + nReSamples + 1];
					}
					if (nCount >= 5)	//如果周围有五个以上不是黑点就进行均值处理
					{
						pRegBuffer[lOffset] = unsigned char(fSumValues / nCount);
					}
				}
			}
		}
	}
	if (fDGrey)
	{
		delete[]fDGrey;
		fDGrey = NULL;
	}
	if (fDItem)
	{
		delete[]fDItem;
		fDGrey = NULL;
	}
}
void GetImgSample(unsigned short *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, unsigned short *pRegBuffer)
{
	int i = 0, j = 0;			//行列循环变量
	DPOINT originPnt;			//影像左上点
	originPnt.dX = minPt.dX;
	originPnt.dY = maxPt.dY;

	float *fDGrey = NULL;			//灰度存储数组
	float *fDItem = NULL;			//权值存储数组
	try
	{
		fDGrey = new float[nReSamples*nReLines];
	}
	catch (bad_alloc)
	{
		exit(-1);
	}
	try
	{
		fDItem = new float[nReSamples*nReLines];
	}
	catch (bad_alloc )
	{
		exit(-1);
	}
	memset(fDGrey, 0, nReSamples*nReLines*sizeof(float));	//初始化化
	memset(fDItem, 0, nReSamples*nReLines*sizeof(float));	//初始化化

	for (i = 0; i<nLines; i++)
	{
#pragma omp parallel for
		for (j = 0; j<nSamples; j++)
		{
			unsigned short fDN = 0;
			float fTempGrey[4];						//定义权值数组	
			int nC = 0, nY = 0;						//坐标取整变量
			double fDX = 0, fDY = 0;				//坐标取余数变量
			DPOINT presentPnt;
			presentPnt.dX = 0;
			presentPnt.dY = 0;

			long lPixOffset = i*nSamples + j;
			//对于原始影像(i,j)坐标为(x,y)，对应新图像位置为
			presentPnt.dX = fabs(pGoundPt[lPixOffset].dX - originPnt.dX) / fGSDX;
			presentPnt.dY = fabs(pGoundPt[lPixOffset].dY - originPnt.dY) / fGSDY;
			//由于presentPnt不是整数，进行取整
			nC = (int)presentPnt.dX;
			nY = (int)presentPnt.dY;
			//由于presentPnt不是整数，进行取余
			fDX = presentPnt.dX - nC;
			fDY = presentPnt.dY - nY;
			fDN = pImgBuffer[lPixOffset];		//获取当前点的原始DN值

			fTempGrey[0] = (float)(1 - fDX)*(1 - fDY)*fDN;
			fTempGrey[1] = (float)fDX*(1 - fDY)*fDN;
			fTempGrey[2] = (float)(1 - fDX)*fDY*fDN;
			fTempGrey[3] = (float)fDX*fDY*fDN;

			if (nC >= 0 && nC<nReSamples && nY >= 0 && nY<nReLines)
			{
				long lOffset = 0;
				lOffset = nY*nReSamples + nC;
				fDGrey[lOffset] += fTempGrey[0];
				fDItem[lOffset] += (1 - fDX)*(1 - fDY);						//左上点
				if (nC < nReSamples - 1)	//未处于右边界
				{
					fDGrey[lOffset + 1] += (float)fTempGrey[1];
					fDItem[lOffset + 1] += (float)fDX*(1 - fDY);					//右上点
				}
				if (nY < nReLines - 1)	//未处于下边界
				{
					fDGrey[lOffset + nReSamples] += (float)fTempGrey[2];
					fDItem[lOffset + nReSamples] += (float)(1 - fDX)*fDY;		//左下点
				}
				if (nC<nReSamples - 1 && nY<nReLines - 1)
				{
					fDGrey[lOffset + nReSamples + 1] += fTempGrey[3];
					fDItem[lOffset + nReSamples + 1] += fDX*fDY;			//右下点
				}
			}
		}
	}
	for (i = 0; i<nReLines; i++)
	{
#pragma omp parallel for
		for (j = 0; j<nReSamples; j++)
		{
			long lOffset = i*nReSamples + j;
			if (fDItem[lOffset] != 0)
			{
				pRegBuffer[lOffset] = unsigned short(fDGrey[lOffset] / fDItem[lOffset]);
			}
			else	//修复黑点
			{
				if (i>0 && i<nReLines - 1 && j>0 && j<nReSamples - 1)	//不处于边界位置
				{
					float fSumValues = 0;
					int nCount = 0;

					if (fDItem[lOffset - nReSamples - 1] != 0)	//左上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples - 1] / fDItem[lOffset - nReSamples - 1];
					}
					if (fDItem[lOffset - nReSamples] != 0)		//上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples] / fDItem[lOffset - nReSamples];
					}
					if (fDItem[lOffset - nReSamples + 1] != 0)	//右上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples + 1] / fDItem[lOffset - nReSamples + 1];
					}
					if (fDItem[lOffset - 1] != 0)					//左
					{
						nCount++;
						fSumValues += fDGrey[lOffset - 1] / fDItem[lOffset - 1];
					}
					if (fDItem[lOffset + 1] != 0)					//右
					{
						nCount++;
						fSumValues += fDGrey[lOffset + 1] / fDItem[lOffset + 1];
					}
					if (fDItem[lOffset + nReSamples - 1] != 0)	//左下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples - 1] / fDItem[lOffset + nReSamples - 1];
					}
					if (fDItem[lOffset + nReSamples] != 0)		//下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples] / fDItem[lOffset + nReSamples];
					}
					if (fDItem[lOffset + nReSamples + 1] != 0)	//右下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples + 1] / fDItem[lOffset + nReSamples + 1];
					}
					if (nCount >= 5)	//如果周围有五个以上不是黑点就进行均值处理
					{
						pRegBuffer[lOffset] = unsigned short(fSumValues / nCount);
					}
				}
			}
		}
	}
	if (fDGrey)
	{
		delete[]fDGrey;
		fDGrey = NULL;
	}
	if (fDItem)
	{
		delete[]fDItem;
		fDGrey = NULL;
	}
}
void GetImgSample(float *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, float *pRegBuffer)
{
	int i = 0, j = 0;			//行列循环变量
	DPOINT originPnt;			//影像左上点
	originPnt.dX = minPt.dX;
	originPnt.dY = maxPt.dY;

	float *fDGrey = NULL;			//灰度存储数组
	float *fDItem = NULL;			//权值存储数组
	try
	{
		fDGrey = new float[nReSamples*nReLines];
	}
	catch (bad_alloc )
	{
		exit(-1);
	}
	try
	{
		fDItem = new float[nReSamples*nReLines];
	}
	catch (bad_alloc )
	{
		exit(-1);
	}
	memset(fDGrey, 0, nReSamples*nReLines*sizeof(float));	//初始化化
	memset(fDItem, 0, nReSamples*nReLines*sizeof(float));	//初始化化

	for (i = 0; i<nLines; i++)
	{
#pragma omp parallel for
		for (j = 0; j<nSamples; j++)
		{
			float fDN = 0;
			float fTempGrey[4];						//定义权值数组	
			int nC = 0, nY = 0;						//坐标取整变量
			double fDX = 0, fDY = 0;				//坐标取余数变量
			DPOINT presentPnt;
			presentPnt.dX = 0;
			presentPnt.dY = 0;

			long lPixOffset = i*nSamples + j;
			//对于原始影像(i,j)坐标为(x,y)，对应新图像位置为
			presentPnt.dX = fabs(pGoundPt[lPixOffset].dX - originPnt.dX) / fGSDX;
			presentPnt.dY = fabs(pGoundPt[lPixOffset].dY - originPnt.dY) / fGSDY;
			//由于presentPnt不是整数，进行取整
			nC = (int)presentPnt.dX;
			nY = (int)presentPnt.dY;
			//由于presentPnt不是整数，进行取余
			fDX = presentPnt.dX - nC;
			fDY = presentPnt.dY - nY;
			fDN = pImgBuffer[lPixOffset];		//获取当前点的原始DN值

			fTempGrey[0] = (float)(1 - fDX)*(1 - fDY)*fDN;
			fTempGrey[1] = (float)fDX*(1 - fDY)*fDN;
			fTempGrey[2] = (float)(1 - fDX)*fDY*fDN;
			fTempGrey[3] = (float)fDX*fDY*fDN;

			if (nC >= 0 && nC<nReSamples && nY >= 0 && nY<nReLines)
			{
				long lOffset = 0;
				lOffset = nY*nReSamples + nC;
				fDGrey[lOffset] += fTempGrey[0];
				fDItem[lOffset] += (1 - fDX)*(1 - fDY);						//左上点
				if (nC < nReSamples - 1)	//未处于右边界
				{
					fDGrey[lOffset + 1] += fTempGrey[1];
					fDItem[lOffset + 1] += fDX*(1 - fDY);					//右上点
				}
				if (nY < nReLines - 1)	//未处于下边界
				{
					fDGrey[lOffset + nReSamples] += fTempGrey[2];
					fDItem[lOffset + nReSamples] += (1 - fDX)*fDY;		//左下点
				}
				if (nC<nReSamples - 1 && nY<nReLines - 1)
				{
					fDGrey[lOffset + nReSamples + 1] += (float)fTempGrey[3];
					fDItem[lOffset + nReSamples + 1] += (float)fDX*fDY;			//右下点
				}
			}
		}
	}
	for (i = 0; i<nReLines; i++)
	{
#pragma omp parallel for
		for (j = 0; j<nReSamples; j++)
		{
			long lOffset = i*nReSamples + j;
			if (fDItem[lOffset] != 0)
			{
				pRegBuffer[lOffset] = float(fDGrey[lOffset] / fDItem[lOffset]);
			}
			else	//修复黑点
			{
				if (i>0 && i<nReLines - 1 && j>0 && j<nReSamples - 1)	//不处于边界位置
				{
					float fSumValues = 0;
					int nCount = 0;

					if (fDItem[lOffset - nReSamples - 1] != 0)	//左上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples - 1] / fDItem[lOffset - nReSamples - 1];
					}
					if (fDItem[lOffset - nReSamples] != 0)		//上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples] / fDItem[lOffset - nReSamples];
					}
					if (fDItem[lOffset - nReSamples + 1] != 0)	//右上
					{
						nCount++;
						fSumValues += fDGrey[lOffset - nReSamples + 1] / fDItem[lOffset - nReSamples + 1];
					}
					if (fDItem[lOffset - 1] != 0)					//左
					{
						nCount++;
						fSumValues += fDGrey[lOffset - 1] / fDItem[lOffset - 1];
					}
					if (fDItem[lOffset + 1] != 0)					//右
					{
						nCount++;
						fSumValues += fDGrey[lOffset + 1] / fDItem[lOffset + 1];
					}
					if (fDItem[lOffset + nReSamples - 1] != 0)	//左下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples - 1] / fDItem[lOffset + nReSamples - 1];
					}
					if (fDItem[lOffset + nReSamples] != 0)		//下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples] / fDItem[lOffset + nReSamples];
					}
					if (fDItem[lOffset + nReSamples + 1] != 0)	//右下
					{
						nCount++;
						fSumValues += fDGrey[lOffset + nReSamples + 1] / fDItem[lOffset + nReSamples + 1];
					}
					if (nCount >= 5)	//如果周围有五个以上不是黑点就进行均值处理
					{
						pRegBuffer[lOffset] = float(fSumValues / nCount);
					}
				}
			}
		}
	}
	if (fDGrey)
	{
		delete[]fDGrey;
		fDGrey = NULL;
	}
	if (fDItem)
	{
		delete[]fDItem;
		fDGrey = NULL;
	}
}
void GetImgSample(float *pImgBuffer, DPOINT *pPositions, int nImgWidth, int nLines, int nReImgWidth, int nReLines, float *pRegBuffer)
{
	float *fDGrey = NULL, *fDItem = NULL;
	try 
	{
		fDGrey = new float[nReImgWidth * nReLines];
		fDItem = new float[nReImgWidth * nReLines];
		memset(fDGrey, 0, nReImgWidth * nReLines*sizeof(float));
		memset(fDItem, 0, nReImgWidth * nReLines*sizeof(float));
	}
	catch(bad_alloc)
	{
		printf("allocate memory error\n");
		exit(-1);
	}

	for (long ns = 0; ns<nReImgWidth*nReLines; ns++)
		pRegBuffer[ns] = 0.0;

	int i = 0, j = 0;
	int nC = 0, nY = 0;
	double fDX = 0, fDY = 0;

	unsigned long nOffset = 0;

	for (i = 0; i<nLines; i++)
	{
		for (j = 0; j<nImgWidth; j++)
		{
			nC = (int)pPositions[i*nImgWidth + j].dX;
			nY = (int)pPositions[i*nImgWidth + j].dY;       //dddd

			fDX = pPositions[i*nImgWidth + j].dX - nC;
			fDY = pPositions[i*nImgWidth + j].dY - nY;

			float fDN = pImgBuffer[i*nImgWidth + j];

			float fTempGrey[4];
			memset(fTempGrey, 0, 4 * sizeof(float));
			fTempGrey[0] = float((1 - fDX)*(1 - fDY)*fDN);
			fTempGrey[1] = float(fDX*(1 - fDY)*fDN);
			fTempGrey[2] = float((1 - fDX)*fDY*fDN);
			fTempGrey[3] = float(fDX*fDY*fDN);

			if (nC >= 0 && nC<nReImgWidth && nY >= 0 && nY<nReLines)
			{
				nOffset = nY*nReImgWidth + nC;
				fDGrey[nOffset] += fTempGrey[0];
				fDItem[nOffset] += float((1 - fDX)*(1 - fDY));
				if (nC < nReImgWidth - 1)
				{
					fDGrey[nOffset + 1] += fTempGrey[1];
					fDItem[nOffset + 1] += float(fDX*(1 - fDY));
				}
				if (nY < nReLines - 1)
				{
					fDGrey[nOffset + nReImgWidth] += fTempGrey[2];
					fDItem[nOffset + nReImgWidth] += float((1 - fDX)*fDY);
				}
				if (nC<nReImgWidth - 1 && nY<nReLines - 1)
				{
					fDGrey[nOffset + nReImgWidth + 1] += fTempGrey[3];
					fDItem[nOffset + nReImgWidth + 1] += float(fDX*fDY);
				}
			}
		}
	}

	float fSumValues = 0;
	float fSumDItems = 0;
	int   nCount = 0;
	int sss = 0;

	for (i = 0; i<nReLines; i++)
	{
		for (j = 0; j<nReImgWidth; j++)
		{
			nOffset = i*nReImgWidth + j;
			if (fDItem[nOffset] != 0)
			{
				pRegBuffer[nOffset] = fDGrey[nOffset] / fDItem[nOffset];
			}
			else
			{
				if (i>0 && i<nReLines - 1 && j>0 && j<nReImgWidth - 1)
				{
					fSumValues = 0;
					fSumDItems = 0;
					nCount = 0;

					if (fDItem[nOffset - nReImgWidth - 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - nReImgWidth - 1] / fDItem[nOffset - nReImgWidth - 1];//
						fSumDItems += fDItem[nOffset - nReImgWidth - 1];
					}
					if (fDItem[nOffset - nReImgWidth] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - nReImgWidth] / fDItem[nOffset - nReImgWidth];
						fSumDItems += fDItem[nOffset - nReImgWidth];
					}
					if (fDItem[nOffset - nReImgWidth + 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - nReImgWidth + 1] / fDItem[nOffset - nReImgWidth + 1];
						fSumDItems += fDItem[nOffset - nReImgWidth + 1];
					}
					if (fDItem[nOffset - 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - 1] / fDItem[nOffset - 1];
						fSumDItems += fDItem[nOffset - 1];
					}
					if (fDItem[nOffset + 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + 1] / fDItem[nOffset + 1];
						fSumDItems += fDItem[nOffset + 1];
					}
					if (fDItem[nOffset + nReImgWidth - 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + nReImgWidth - 1] / fDItem[nOffset + nReImgWidth - 1];
						fSumDItems += fDItem[nOffset + nReImgWidth - 1];
					}
					if (fDItem[nOffset + nReImgWidth] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + nReImgWidth] / fDItem[nOffset + nReImgWidth];
						fSumDItems += fDItem[nOffset + nReImgWidth];
					}
					if (fDItem[nOffset + nReImgWidth + 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + nReImgWidth + 1] / fDItem[nOffset + nReImgWidth + 1];
						fSumDItems += fDItem[nOffset + nReImgWidth + 1];
					}

					if (nCount >= 1)
					{
						//	if( fSumDItems>0.5 )
						pRegBuffer[nOffset] = fSumValues / nCount;
						sss++;
					}

				}
			}
		}
	}

	if (fDGrey != NULL)
		delete[]fDGrey;
	if (fDItem != NULL)
		delete[]fDItem;
	fDGrey = NULL;
	fDItem = NULL;
}
void GetImgSample(unsigned char *pImgBuffer, DPOINT *pPositions, int nImgWidth, int nLines, int nReImgWidth, int nReLines, unsigned char *pRegBuffer)
{
	float *fDGrey = NULL, *fDItem = NULL;
	try
	{
		fDGrey = new float[nReImgWidth * nReLines];
		fDItem = new float[nReImgWidth * nReLines];
		memset(fDGrey, 0, nReImgWidth * nReLines*sizeof(float));
		memset(fDItem, 0, nReImgWidth * nReLines*sizeof(float));
	}
	catch (bad_alloc)
	{
		printf("allocate memory error\n");
		exit(-1);
	}

	for (long ns = 0; ns<nReImgWidth*nReLines; ns++)
		pRegBuffer[ns] = 0;

	int i = 0, j = 0;
	int nC = 0, nY = 0;
	double fDX = 0, fDY = 0;

	unsigned long nOffset = 0;

	for (i = 0; i<nLines; i++)
	{
		for (j = 0; j<nImgWidth; j++)
		{
			nC = (int)pPositions[i*nImgWidth + j].dX;
			nY = (int)pPositions[i*nImgWidth + j].dY;       //dddd

			fDX = pPositions[i*nImgWidth + j].dX - nC;
			fDY = pPositions[i*nImgWidth + j].dY - nY;

			float fDN = pImgBuffer[i*nImgWidth + j];

			float fTempGrey[4];
			memset(fTempGrey, 0, 4 * sizeof(float));
			fTempGrey[0] = float((1 - fDX)*(1 - fDY)*fDN);
			fTempGrey[1] = float(fDX*(1 - fDY)*fDN);
			fTempGrey[2] = float((1 - fDX)*fDY*fDN);
			fTempGrey[3] = float(fDX*fDY*fDN);

			if (nC >= 0 && nC<nReImgWidth && nY >= 0 && nY<nReLines)
			{
				nOffset = nY*nReImgWidth + nC;
				fDGrey[nOffset] += fTempGrey[0];
				fDItem[nOffset] += float((1 - fDX)*(1 - fDY));
				if (nC < nReImgWidth - 1)
				{
					fDGrey[nOffset + 1] += fTempGrey[1];
					fDItem[nOffset + 1] += float(fDX*(1 - fDY));
				}
				if (nY < nReLines - 1)
				{
					fDGrey[nOffset + nReImgWidth] += fTempGrey[2];
					fDItem[nOffset + nReImgWidth] += float((1 - fDX)*fDY);
				}
				if (nC<nReImgWidth - 1 && nY<nReLines - 1)
				{
					fDGrey[nOffset + nReImgWidth + 1] += fTempGrey[3];
					fDItem[nOffset + nReImgWidth + 1] += float(fDX*fDY);
				}
			}
		}
	}

	float fSumValues = 0;
	float fSumDItems = 0;
	int   nCount = 0;
	int sss = 0;

	for (i = 0; i<nReLines; i++)
	{
		for (j = 0; j<nReImgWidth; j++)
		{
			nOffset = i*nReImgWidth + j;
			if (fDItem[nOffset] != 0)
			{
				pRegBuffer[nOffset] = fDGrey[nOffset] / fDItem[nOffset];
			}
			else
			{
				if (i>0 && i<nReLines - 1 && j>0 && j<nReImgWidth - 1)
				{
					fSumValues = 0;
					fSumDItems = 0;
					nCount = 0;

					if (fDItem[nOffset - nReImgWidth - 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - nReImgWidth - 1] / fDItem[nOffset - nReImgWidth - 1];//
						fSumDItems += fDItem[nOffset - nReImgWidth - 1];
					}
					if (fDItem[nOffset - nReImgWidth] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - nReImgWidth] / fDItem[nOffset - nReImgWidth];
						fSumDItems += fDItem[nOffset - nReImgWidth];
					}
					if (fDItem[nOffset - nReImgWidth + 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - nReImgWidth + 1] / fDItem[nOffset - nReImgWidth + 1];
						fSumDItems += fDItem[nOffset - nReImgWidth + 1];
					}
					if (fDItem[nOffset - 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset - 1] / fDItem[nOffset - 1];
						fSumDItems += fDItem[nOffset - 1];
					}
					if (fDItem[nOffset + 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + 1] / fDItem[nOffset + 1];
						fSumDItems += fDItem[nOffset + 1];
					}
					if (fDItem[nOffset + nReImgWidth - 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + nReImgWidth - 1] / fDItem[nOffset + nReImgWidth - 1];
						fSumDItems += fDItem[nOffset + nReImgWidth - 1];
					}
					if (fDItem[nOffset + nReImgWidth] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + nReImgWidth] / fDItem[nOffset + nReImgWidth];
						fSumDItems += fDItem[nOffset + nReImgWidth];
					}
					if (fDItem[nOffset + nReImgWidth + 1] != 0)
					{
						nCount++;
						fSumValues += fDGrey[nOffset + nReImgWidth + 1] / fDItem[nOffset + nReImgWidth + 1];
						fSumDItems += fDItem[nOffset + nReImgWidth + 1];
					}

					if (nCount >= 1)
					{
						//	if( fSumDItems>0.5 )
						pRegBuffer[nOffset] = fSumValues / nCount;
						sss++;
					}

				}
			}
		}
	}

	if (fDGrey != NULL)
		delete[]fDGrey;
	if (fDItem != NULL)
		delete[]fDItem;
	fDGrey = NULL;
	fDItem = NULL;
}

//获取直方图匹配后的直方图
//影像2的直方图为基准
void GetImgHistroMatch(double* img1, double *img2, int xsize1, int ysize1, int xsize2, int ysize2, int minPix, int maxPix, int* histroMap)
{
	int *ihistro1 = NULL;
	double* fhistro1 = NULL;
	int *ihistro2 = NULL;
	double* fhistro2 = NULL;
	try
	{
		ihistro1 = new int[maxPix-minPix];
		fhistro1 = new double[maxPix - minPix];
		ihistro2 = new int[maxPix - minPix];
		fhistro2 = new double[maxPix - minPix];

		memset(ihistro1, 0, sizeof(int)*(maxPix - minPix));
		memset(ihistro2, 0, sizeof(int)*(maxPix - minPix));
		memset(fhistro1, 0, sizeof(double)*(maxPix - minPix));
		memset(fhistro2, 0, sizeof(double)*(maxPix - minPix));
	}
	catch (bad_alloc)
	{
		printf("allocate memory error\n");
		exit(-1);
	}

	for (int i = 0; i<xsize1*ysize1; ++i)
		ihistro1[int(img1[i])-minPix]++;
	for (int i = 0; i<xsize2*ysize2; ++i)
		ihistro2[int(img2[i]) - minPix]++;
	for (int i = minPix; i<maxPix; ++i)
	{
		fhistro1[i] = double(ihistro1[i]) / xsize1 / ysize1;
		fhistro2[i] = double(ihistro2[i]) / xsize2 / ysize2;
	}
	//累积直方图
	for (int i = 1; i<maxPix; ++i)
	{
		fhistro1[i] = fhistro1[i - 1] + fhistro1[i];
		fhistro2[i] = fhistro2[i - 1] + fhistro2[i];
	}



	//直方图匹配
	double m_diffA, m_diffB;  int k = 0;
	for (int i = 0; i < maxPix - minPix; i++)
	{
		m_diffB = 1;
		for (int j = k; j < maxPix - minPix; j++)
		{
			m_diffA = abs(fhistro1[i] - fhistro2[j]);
			if (m_diffA - m_diffB < 1.0E-5)
			{
				m_diffB = m_diffA;
				k = j;
			}
			else
			{
				k = j - 1;
				break;
			}
		}
		if (k == maxPix - minPix - 1)
		{
			for (int l = i; l < maxPix - minPix; l++)
				histroMap[l] = k;
			break;
		}
		histroMap[i] = k;
	}

	//清理内存
	delete[]ihistro1;
	delete[]ihistro2;
	delete[]fhistro1;
	delete[]fhistro2;
}
void GetImgHistroMatch(unsigned char* img1, unsigned char*img2, int xsize1, int ysize1, int xsize2, int ysize2, int minPix, int maxPix, int* histroMap)
{
	int *ihistro1 = NULL;
	double* fhistro1 = NULL;
	int *ihistro2 = NULL;
	double* fhistro2 = NULL;
	try
	{
		ihistro1 = new int[maxPix - minPix];
		fhistro1 = new double[maxPix - minPix];
		ihistro2 = new int[maxPix - minPix];
		fhistro2 = new double[maxPix - minPix];

		memset(ihistro1, 0, sizeof(int)*(maxPix - minPix));
		memset(ihistro2, 0, sizeof(int)*(maxPix - minPix));
		memset(fhistro1, 0, sizeof(double)*(maxPix - minPix));
		memset(fhistro2, 0, sizeof(double)*(maxPix - minPix));
	}
	catch (bad_alloc)
	{
		printf("allocate memory error\n");
		exit(-1);
	}

	for (int i = 0; i<xsize1*ysize1; ++i)
		ihistro1[int(img1[i]) - minPix]++;
	for (int i = 0; i<xsize2*ysize2; ++i)
		ihistro2[int(img2[i]) - minPix]++;
	for (int i = minPix; i<maxPix; ++i)
	{
		fhistro1[i] = double(ihistro1[i]) / xsize1 / ysize1;
		fhistro2[i] = double(ihistro2[i]) / xsize2 / ysize2;
	}
	//累积直方图
	for (int i = 1; i<maxPix; ++i)
	{
		fhistro1[i] = fhistro1[i - 1] + fhistro1[i];
		fhistro2[i] = fhistro2[i - 1] + fhistro2[i];
	}



	//直方图匹配
	double m_diffA, m_diffB;  int k = 0;
	for (int i = 0; i < maxPix - minPix; i++)
	{
		m_diffB = 1;
		for (int j = k; j < maxPix - minPix; j++)
		{
			m_diffA = abs(fhistro1[i] - fhistro2[j]);
			if (m_diffA - m_diffB < 1.0E-5)
			{
				m_diffB = m_diffA;
				k = j;
			}
			else
			{
				k = j - 1;
				break;
			}
		}
		if (k == maxPix - minPix - 1)
		{
			for (int l = i; l < maxPix - minPix; l++)
				histroMap[l] = k;
			break;
		}
		histroMap[i] = k;
	}

	//清理内存
	delete[]ihistro1;
	delete[]ihistro2;
	delete[]fhistro1;
	delete[]fhistro2;
}
//两点之间的距离
double GetDisofPoints(THREEDPOINT pnt1, THREEDPOINT pnt2)
{
	return sqrt((pnt1.dX - pnt2.dX)*(pnt1.dX - pnt2.dX) + (pnt1.dY - pnt2.dY)*(pnt1.dY - pnt2.dY) + (pnt1.dZ - pnt2.dZ)*(pnt1.dZ - pnt2.dZ));
}
double GetDisofPoints(CPOINT pnt1, CPOINT pnt2)
{
	return sqrt(double(pnt1.x - pnt2.x)*double(pnt1.x - pnt2.x) + double(pnt1.y - pnt2.y)*double(pnt1.y - pnt2.y));
}
//获取影像
void GetImageList(const char* pathList, vector<string> &pszImage)
{
	fstream fin;
	fin.open(pathList, ios_base::in);
	if (!fin.is_open())
	{
		exit(-1);
	}
	do
	{
		string str;
		char tmp[1024];
		fin.getline(tmp, 1024);
		str = tmp;
		if (str != "")
			pszImage.push_back(str);
	} while (!fin.eof());
	fin.close();
}
void SetFileList(const char* pathDir, const char* pathList)
{
	ofstream ofs;
	ofs.open(pathList);
	if (!ofs.is_open())
		return ;
	_finddata_t file;
	long lf;
	char szTmp[300];
	sprintf(szTmp, "%s\\*.*", pathDir);
	//输入文件夹路径  
	if ((lf = _findfirst(szTmp, &file)) == -1)
		printf("not found\n");
	else
	{
		//输出文件名  
		if (file.name != "."&&file.name != "..")
			ofs << pathDir << "\\" << file.name;
		while (_findnext(lf, &file) == 0)
		{
			ofs << endl << pathDir << "\\" << file.name;
		}
	}
	_findclose(lf);
}
double MaxAvg(double data1, double data2, double data3)
{
	double m = data1;
	if (data1>data2)
		m = data2;
	if (m>data3)
		m = data3;
	if (m == data1)
		return (data2 + data3) / 2;
	else if (m == data2)
		return (data1 + data3) / 2;
	else
		return (data1 + data2) / 2;
}
double MinAvg(double data1, double data2, double data3)
{
	double m = data1;
	if (data1<data2)
		m = data2;
	if (m<data3)
		m = data3;
	if (m == data1)
		return (data2 + data3) / 2;
	else if (m == data2)
		return (data1 + data3) / 2;
	else
		return (data1 + data2) / 2;
}