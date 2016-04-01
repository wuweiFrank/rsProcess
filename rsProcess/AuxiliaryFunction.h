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

//对数据进行采样
/*
	在几何校正后校正前数据和校正后数据并不是完全对应
	通过采样方法得到没有数据位置像素值
*/
//对float类型的数据和unsigned short类型的数据进行处理
void GetImgSample(unsigned short *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, unsigned short *pRegBuffer);
void GetImgSample(float *pImgBuffer, DPOINT &minPt, DPOINT &maxPt, THREEDPOINT *pGoundPt, float fGSDX, float fGSDY, int nSamples, int nLines, int nReSamples, int nReLines, float *pRegBuffer);