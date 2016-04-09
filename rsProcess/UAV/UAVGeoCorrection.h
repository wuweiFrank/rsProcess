#pragma once

#include"..\GeoPOSProcess.h"
#include"..\AerialProduct\Level2Process.h"

//无人机影像POS数据处理
class UAVPOSProcess : public GeoPOSProcess
{
public:
	//1.读取影像数据
	long GeoPOSProc_ReadPartPOS(const char *pPOSFile, long nLines, double &dB, double &dL, double &dH, int nbeginLine);

	//2.无人机影像一般不需要解算SBET文件，无人机POS数据可以直接获取
	long GeoPOSProc_ExtractSBET(const char* pathSBET, const char* pathEvent, const char* pathPOS, float fOffsetGPS) { return 0; }

	//3.由单个POS数据解算EO元素，无人机不考虑安置角和安置矢量
	long GeoPOSProc_ExtractEO(POS m_perpos, EO &m_pereo);
};

//UAV的几何校正和推扫式的影像几何校正有比较大的差异
//UAV的一景影像只有1个POS信息
//1.在校正过程中应该将地面点设置为已知点
//2.由POS数据解算出EO元素
//3.通过EO元素进行校正
class UAVGeoCorrection : public Level2Process
{
public:
	//1.对影像集进行校正
	long UAVGeoCorrection_GeoCorrect(const char* pathSrcDir, const char* pathDstDir, const char* pathPOS, const char* pathDEM,
																			int IMGbegline, int POSbegline, int lines,double fLen , double fGSD, double AvgHeight);

	//2.对单个影像进行校正
	long UAVGeoCorrection_GeoCorrect(const char* pathSrc, const char* pathDst, EO  m_preeo, double dL, double dB, double fLen, double fGSD, double AvgHeight, char* pathDEM = NULL);

	//3.计算每个点的地理坐标
	long UAVGeoCorrection_GeoPntsProb(double dB, double dL, double dH, EO pEO, double fLen, int width, int height, THREEDPOINT *pGoundPnt, DPOINT* pMapImgPnt, int pntNum);

	//4.精确计算每个点的坐标然后进行校正
	long UAVGeoCorrection_GeoPntsAccu(double dB, double dL, double dH, EO pEO, double fLen, int width, int height, THREEDPOINT *pGoundPnt,const char* pathDEM);
};