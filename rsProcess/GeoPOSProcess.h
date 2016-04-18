#pragma once
#include"Global.h"
#include"AuxiliaryFunction.h"

#include<vector>
using namespace std;

//POS和EO数据处理文件，文件中包括POS和EO数据的处理

//EO和POS数据结构的定义
typedef struct stExteriorOrientation
{
	//三个线元素
	double m_dX;
	double m_dY;
	double m_dZ;

	//三个角元素
	double m_phia;
	double m_omega;
	double m_kappa;

	//旋转矩阵
	double m_dRMatrix[9];
}EO;
typedef struct stRelativeOrientation
{
	//三个线元素
	double m_Bx;
	double m_By;
	double m_Bz;
	//三个角元素
	double m_phia;
	double m_omega;
	double m_kappa;

	//旋转矩阵
	double m_dRMatrix[9];
}REO;
typedef struct stPositOrientationSys
{
	//经纬度和高度
	double m_latitude;
	double m_longitude;
	double m_height;

	//侧滚、俯仰和偏航
	double m_roll;
	double m_pitch;
	double m_yaw;
}POS;

class GeoPOSProcess
{
	//构造和析构函数
public:
	GeoPOSProcess() {
		m_geo_POS = NULL;
		m_geo_EO = NULL;
	}
	~GeoPOSProcess() {
		if (m_geo_EO != NULL)
			delete[]m_geo_EO;
		if (m_geo_POS != NULL)
			delete[]m_geo_POS;
	}

	//纯虚函数在派生的时候必须重载
	//读取POS数据，纯虚函数，所有POS数据处理类必须重载此函数
	/*
		const char *pPOSFile:POS数据路径
		long nLines			:POS数据行数
		double &dB, double &dL, double &dH 中心经纬度和高度
		int nbeginLine		:POS数据头行数
	*/
	virtual long GeoPOSProc_ReadPartPOS(const char *pPOSFile, long nLines, double &dB, double &dL, double &dH, int nbeginLine) = 0;
	
	//从SBET文件中解算出POS数据
	virtual long GeoPOSProc_ExtractSBET(const char* pathSBET, const char* pathEvent, const char* pathPOS, float fOffsetGPS)=0;

	//由POS数据解算EO元素
	virtual long GeoPOSProc_ExtractEO(const char *pPOSFile, int nLines, const  char *pEOFile, THREEDPOINT THETA, float *fSetupAngle, int nbeginLine);

	/*
		功能：读取解算得到的EO数据
		参数：1.const char* pathEO EO元素路径
		2.double &dB 测区平均纬度
		3.double &dL 测区平均经度
		4.double &dH 测区平均高度
	*/
	virtual long GeoPOSProc_ReadEOFile(const char* pathEO, double &dB, double &dL, double &dH);

//private:
	//根据POS数据计算航带所在象限
	//EMMatrix 为地心坐标系到成图坐标系的转换
	long GeoPOSProc_EOQuadrant(int nLines, double EMMatrix[], THREEDPOINT &XYZPoint);
	//根据单个POS计算航带所在象限
	long GeoPOSProc_EOQuadrant(POS curPOS, double EMMatrix[], THREEDPOINT &XYZPoint);

	//根据POS获取EO元素和旋转矩阵并输出到文件中批量处理
	long GeoPOSProc_EOMatrixTurn(int nCurLine, THREEDPOINT &XYZPoint, int nQuadNum, double EMMatrix[], THREEDPOINT ANGLETHETA, THREEDPOINT POSISTTHETA, FILE *fEO);
	//根据单个POS数据获取EO元素
	long GeoPOSProc_EOMatrixTurn(POS pdfPOS, THREEDPOINT &XYZPoint, THREEDPOINT THETA, int nQuadNum, double EMMatrix[], EO &pdfEO);

public:
	//POS数据和EO数据
	POS* m_geo_POS;
	EO*  m_geo_EO;

	//测区或航带中心经纬度和飞行的平均高度
	double m_geo_dB;
	double m_geo_dL;
	double m_geo_dH;
	int    m_nPOSLines;	//POS数据行数
};

//全谱段数据处理
class QPDGeoPOSProcess : public GeoPOSProcess
{
public:
	//从SBET文件中解算出POS数据
	long GeoPOSProc_ExtractSBET(const char* pathSBET, const char* pathEvent, const char* pathPOS, float fOffsetGPS);

	//获取POS数据
	long GeoPOSProc_ReadPartPOS(const char *pPOSFile, long nLines, double &dB, double &dL, double &dH, int nbeginLine);

	//获取EO元素的行数
	long GeoPOSProc_GetEOLines(const char *pEoFile, int &nEOLines);
};