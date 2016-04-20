#pragma once
#include"..\OPENCVTools.h"
#include"UAVGeoCorrection.h"
#include<vector>

using namespace std;
using namespace cv;

/*框标坐标系结构体 4个角点*/
struct CornerCoordi4
{
	double xPixel[4];			//框标像素坐标x
	double yPixel[4];			//框标像素坐标y

	double xPic[4];				//框标像片坐标x
	double yPic[4];				//框标像片坐标y
};
/*框标坐标系结构体 8个角点*/
struct CornerCoordi8
{
	double xPixel[8];			//框标像素坐标x
	double yPixel[8];			//框标像素坐标y

	double xPic[8];				//框标像片坐标x
	double yPic[8];				//框标像片坐标y
};


/*还是想做一套这样的工具出来用于摄影测量，OPENCV的工具毕竟用于SFM的，用于摄影测量不太适用*/
class UAVPhotogrammetryTools
{
public:
	//1.空间前方交会 根据外方位元素和匹配点解算空间点三维坐标
	long UAVPhotogrammetryTools_UAVFesction(vector<Point2f> pnt1,EO imgEO1, vector<Point2f> pnt2, EO imgEO2,vector<Point3d> &points);

	//2.空间后方交会，根据控制点和影像点解算影像外方位元素
	long UAVPhotogrammetryTools_UAVResction(vector<Point2f> pnt1, vector<Point3d> pnt2,EO &eoElement);

	//3.空间相对定向，根据匹配点进行空间相对定向
	long UAVPhotogrammetryTools_ROrientation(vector<Point2f> pnt1, vector<Point2f> pnt2, REO &reoRElementL, REO &reoRElementR);

	//4.绝对定向
	long UAVPhotogrammetryTools_AOrientation(vector<Point2f> pntModel1, vector<Point2f> pntModel2, REO& REOl, REO& REOr,vector<Point3d> pntGeo, EO &eoAElement,double &lamda);

	//6.设置相机内参数
	long UAVPhotogrammetryTools_SetParam(double len,double px0,double py0,double tk[3],double tp[3],double alpha,double belta);

	//7.像素坐标到影像坐标的转换系数
	long UAVPhotogrammetryTools_PixelToImgTrans(CornerCoordi4 m_coordiTrans4);		//四个框标的框标坐标系的转换
	long UAVPhotogrammetryTools_PixelToImgTrans(CornerCoordi8 m_coordiTrans8);		//八个框标的框标坐标系的转换

	//8.径向和切向畸变校正
	long UAVPhotogrammetryTools_DistortionCorrection(vector<Point2f> &pntCamera);

	//TODO:5.光束法平差
	long UAVPhotogrammetryTools_BundlerAdj();

	//8.将测试函数声明为友元函数方便获取
	friend void PhotogrammetryToolsTest();

private:
	//1.相机内定向
	long UAVPhotogrammetryTools_IOrientation(vector<Point2f> &pointImg);

	//摄影测量必须要求标定相机，所以相机内参应该给定
	double m_PixelToImgTrans[6];				//从像素坐标到影像坐标的转换
	double m_fLen;								//相机焦距 m为单位
	double m_px, m_py;							//主轴和相片的对应位置
	double m_k[3], m_p[3], m_alpha, m_belta;	//径向和切向畸变
	bool  isInternal;
};

void PhotogrammetryToolsTest();