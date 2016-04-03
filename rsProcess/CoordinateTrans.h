#pragma once
/*  Created on : 2016 - 04 - 01
* Author : wuwei
*		description：影像数据产品生产坐标转换类
*/
#include "stdafx.h"
#include "CoordinateTrans.h"
#include "Global.h"
#include "matrixOperation.h"
#include "tsmUTM.h"
//#include"GeoPOSProcess.h"
#include <iostream>
#include<fstream>

using namespace std;
#include "gdal/include/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

/*
坐标转换类结构定义：
功能：  1.地心坐标到经纬度之间的转换，WGS84椭球
		2.根据EO计算每个影像店在成图坐标系下的坐标
		3.由成图坐标系下的坐标计算UTM坐标系下的坐标
		4.计算原始影像上每个像元位置对应到校正影像上的位置
		5.已知转换前每个像素位置对应转换后像素的位置 根据转换对应计算转换后每个像素的值
*/
//WGS84 地心坐标系和经纬度的转换
class CoordinateTrans
{
public:

	long BLHToXYZ(double dB, double dL, double dH, THREEDPOINT &XYZPnt);

	long XYZToBLHS(double dB, double dL, double dH, THREEDPOINT *pGoundPnt, int PxelNum);

	long XYZToBLH(THREEDPOINT XYZPnt, double &dB, double &dL, double &dH);

	//virtual long CoordinateImgToGeoMap(int nImgHeight, int nImgWidth, THREEDPOINT *pGoundPt, EO *pdEOData, float fFov, float fFocalLen);
	//virtual long CoordinateGeoMapToUTM(double dB, double dL, double dH, THREEDPOINT *pGoundPnt, int nImgHeight, int nImgWidth, DPOINT &minPnt, DPOINT &maxPnt);
	//virtual long CoordinateSrcToCorrectMap(THREEDPOINT *pGoundPnts, DPOINT* pfMapPositions, float fGSD, DPOINT minPnt, DPOINT maxPnt, int nImgHeight, int nImgWidth);
};
