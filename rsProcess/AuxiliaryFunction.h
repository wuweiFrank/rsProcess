#pragma once
#include"Global.h"

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