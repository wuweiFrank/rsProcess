#pragma once
#include"Global.h"

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

//写ENVI头文件格式
/*
	const char* pathHeader:头文件输出路径
	ENVIHeader mImgHeader :头文件信息
*/
void WriteENVIHeader(const char* pathHeader, ENVIHeader mImgHeader);

//获取数据均值和标准差
/*
	unsigned char *pBuffer：数据
	int nSamples, int nLines, int nBand：数据行列和波段数
	float &fAverage, float &fDeviate：数据均值和标准差
*/
void GetAveDev(unsigned char *pBuffer, int nSamples, int nLines, int nBand, float &fAverage, float &fDeviate);
void GetAveDev(unsigned short *pBuffer, int nSamples, int nLines, int nBand, float &fAverage, float &fDeviate);
void GetAveDev(float *pBuffer, int nSamples, int nLines, int nBand, float &fAverage, float &fDeviate);

//数据归一化
void NormalizeData(float* pBuffer,int length,float* pNormalBuffer);
void NormalizeData(double* pBuffer, int length, double* pNormalBuffer);
//获取相关系数
float GetCoefficient(float* data1, float* data2, int num);
float GetSSD(float* data1, float* data2, int num);
//对数据进行采样
/*
	在几何校正后校正前数据和校正后数据并不是完全对应
	通过采样方法得到没有数据位置像素值
*/
//对float类型的数据和unsigned short类型的数据进行处理
void GetImgSample(unsigned char *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, unsigned char *pRegBuffer);
void GetImgSample(unsigned short *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, unsigned short *pRegBuffer);
void GetImgSample(float *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, float *pRegBuffer);

void GetImgSample(float *pImgBuffer, DPOINT *pPositions, int nImgWidth, int nLines, int nReImgWidth, int nReLines, float *pRegBuffer);//pPositions为校正前影像为止对应校正后影像的影像位置
void GetImgSample(unsigned char *pImgBuffer, DPOINT *pPositions, int nImgWidth, int nLines, int nReImgWidth, int nReLines, unsigned char *pRegBuffer);//pPositions为校正前影像为止对应校正后影像的影像位置


//获取两幅影像匹配直方图
//以img2为标准
void GetImgHistroMatch(double* img1, double *img2, int xsize1, int ysize1, int xsize2, int ysize2, int minPix, int maxPix, int* histroMap);
void GetImgHistroMatch(unsigned char* img1, unsigned char*img2, int xsize1, int ysize1, int xsize2, int ysize2, int minPix, int maxPix, int* histroMap);

//获取两点之间的距离
double GetDisofPoints(THREEDPOINT pnt1, THREEDPOINT pnt2);
double GetDisofPoints(CPOINT pnt1, CPOINT pnt2);

//获取影像列表
void GetImageList(const char* pathList, vector<string> &pszImage);

//三个数中最大两个数的平均值
double MaxAvg(double data1, double data2, double data3);
//三个数中最小两个数的平均值
double MinAvg(double data1, double data2, double data3);

