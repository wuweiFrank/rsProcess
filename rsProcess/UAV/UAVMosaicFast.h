#pragma once
#include"..\OPENCVTools.h"
#include<string>
using namespace std;

//无人机影像数据的快速拼接处理
struct adfAffineTrans
{
	double m_affineTransParameters[6];
};

class UAVMosaicFast
{
private:
	//根据影像获取仿射变换系数 这样的校正是以最后一张影像为基准的
	long UAVMosaicFast_AffineTrans(vector<string> pszImages);

	//获取影像拼接的范围
	long UAVMosaicFast_GetMosaicRange(vector<string> pszImages,int& mosaic_width,int &mosaic_height);

	//影像色调调整第一张影像为需要调整的影像，第二张影像为基准影像
	long UAVMosaicFast_HistroMatch(unsigned char* imgBuffer1, unsigned char* imgBuffer2, int xsize1, int ysize1, int xsize2, int ysize2 );

	//通过仿射变换向数据域中填入数据
	long UAVMosaicFast_AffineTrans(adfAffineTrans& affineTransParam, unsigned char* imgBuffer, int xsize, int ysize, unsigned char* imgMosaic,int mosaicx,int mosaicy);

public:
	//影像镶嵌
	long UAVMosaicFast_Mosaic(vector<string> pszImages,const char* pathDst);

private:
	vector<adfAffineTrans> m_affineTransParameters;
};