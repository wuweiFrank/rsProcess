#pragma once
#include"..\Global.h"
#include"..\GeoPOSProcess.h"
//二级数据产品生产基类,基类为虚机类；
//所有派生类必须重载数据产品生产函数
class Level2Process
{
public:
	//===============================================================================================================================================================
	/*
	功能：根据IGM和EO元素对影像进行校正获取无DEM条件下影像的校正
	参数：1.char *pFile 校正前影像
			2.char *pCFile 校正后影像
			3.char *pEOFile 由POS结算得到的EO元素
			4.float fGSDX, float fGSDY 地面分辨率
			5.double fElevation 给定平均高程
			6. int nEOOffset EO的偏移量
			7.float fFov, float fIFov 总视场和瞬时视场
			8.float fFocalLen 传感器焦距
			9.bool bIGM 是否生成IGM数据
			10.char *pIGMFile IGM文件路径
			11.bool bInverse=false 是否反转
	*/
	virtual long Level2Proc_Product2A(const char *pFile, const char *pCFile, const char *pEOFile, float fGSDX, float fGSDY, double fElevation, int nEOOffset,
		float fFov, float fIFov, float fFocalLen, bool bIGM, const char *pIGMFile,bool bInverse=false)=0;
	//与2A级数据产品生产函数相比2B级数据产品生产函数主要增加的接口为DEM输入路径，若输入路径为NULL，则二者相同，如果存在DEM则利用DEM进行解算
	virtual long Level2Proc_Product2B(const char *pFile, const char *pCFile, const char *pEOFile, float fGSDX, float fGSDY, double fElevation, int nEOOffset,
		float fFov, float fIFov, float fFocalLen, const char* pDEMFile, bool bIGM, const char *pIGMFile, bool bInverse = false)=0;
	//================================================================================================================================================================
	/*
	功能：计算所有影像地物像元（中间影像体）的所有大地坐标，并将计算得到的大地坐标保存到三维点数组中
	计算影像中的最大最小点，并将坐标系有大地坐标转换为UTM坐标，因为最终需要的是UTM坐标
	参数：nLines		影像高度
	nImgWidth		影像地物像元（中间影像体）
	nInterval		影像EO文件行与影像行对应关系
	pGoundPt		三维点数组
	pEOElement	EO文件体
	fFov			总视场角度
	fFocalLen		焦距
	minPnt：		影像内最小大地坐标点
	maxPnt：		影像内最大大地坐标点
	*/
	//根据EO元素计算IGM文件
	virtual long Level2Proc_CalculateIGMFile(int nLines, int nImgWidth, int nInterval, THREEDPOINT *pGoundPt, EO *pEOElement, float fFov, float fFocalLen, DPOINT &minPt, DPOINT &maxPt, double dB, double dL, bool bInverse);

	//根据EO元素和DEM元素计算IGM文件
	virtual long Level2Proc_CalculateIGMFileDEM(int nLines, int nImgWidth, int nInterval, THREEDPOINT *pGoundPt, EO *pEOElement, float fFov, float fFocalLen, DPOINT &minPt, DPOINT &maxPt, double dB, double dL, const char* pathDEM, bool bInverse);
	/*
	const char* pIGMFile:	文件输出路径
	THREEDPOINT *pGoundPnt: IGM文件
	int nImgWidth, int nHeight IGM文件宽度和高度
	IGM文件在这里我直接用二进制文件写进去了读的时候也直接读就好了
	*/
	virtual long Level2Proc_SaveIGMFile(const char* pIGMFile, THREEDPOINT *pGoundPnt, int nImgWidth, int nImgHeight);

	//读取IGM文件
	virtual long Level2Proc_ReadIGMFile(const char* pIGMFile, THREEDPOINT *pGoundPnt, int &nImgWidth, int &nImgHeight);

	//读取DEM文件
	/*
	参数：const char *pDEMFile DEM文件路径
	int &nSamples, int &nLines DEM数据行列数
	DPOINT rangePnt[]      DEM区域范围
	double dGeoTransform[] DEM仿射变换系数
	int &nZone,int projType	投影带和投影类型 1为WGS84经纬度 2为UTM投影，0为其他投影
	double *pDEMPt DEM数据
	*/
	virtual long Level2Proc_ReadDEMFile(const char *pDEMFile, int &nSamples, int &nLines, DPOINT rangePnt[], double dGeoTransform[], int &nZone, int &projType, double *&pDEMPt);
};

//全谱段数据处理派生类，重载A级产品生产，B级产品生产函数
class QPDLevel2Process : public Level2Process
{
public:
	long Level2Proc_Product2A(const char *pFile, const char *pCFile, const char *pEOFile, float fGSDX, float fGSDY, double fElevation, int nEOOffset,
		float fFov, float fIFov, float fFocalLen, bool bIGM, const char *pIGMFile, bool bInverse = false);


	//与2A级数据产品生产函数相比2B级数据产品生产函数主要增加的接口为DEM输入路径，若输入路径为NULL，则二者相同，如果存在DEM则利用DEM进行解算
	long Level2Proc_Product2B(const char *pFile, const char *pCFile, const char *pEOFile, float fGSDX, float fGSDY, double fElevation, int nEOOffset,
		float fFov, float fIFov, float fFocalLen, const char* pDEMFile, bool bIGM, const char *pIGMFile, bool bInverse = false);

};