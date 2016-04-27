#pragma once
#include"Geometry.h"
#include"LidarDefs.h"
#include"..\Global.h"

//lidar点云滤波处理
class LidarFliterBase
{
public:
	//lidar点云滤波参数
	long LidarFliterParameter(double fliterWidth, double fliterHeight) {
		m_FliterWidth = fliterWidth;
		m_FliterHeight = fliterHeight;
	}

	//点云滤波处理过程
	virtual long LidarFliterProcess(LASSet &lasDataset)=0;

	//点云滤波单元
	virtual long LidarFliter(LASSet &lasDataset) = 0;

public:
	//滤波窗口宽度和滤波窗口高度
	double m_FliterWidth;
	double m_FliterHeight;
};

//点云区域均值滤波
class LidarMeanFliter : public LidarFliterBase
{
public:
	//点云滤波处理过程
	long LidarFliterProcess(LASSet &lasDataset) { return 0; }

	//点云滤波单元
	long LidarFliter(LASSet &lasDataset) { return 0; }
};

//点云平面滤波
class LidarPlaneFliter : public LidarFliterBase
{
public:
	//点云滤波处理过程
	long LidarFliterProcess(LASSet &lasDataset) { return 0; }

	//点云滤波单元
	long LidarFliter(LASSet &lasDataset) { return 0; }
};

//点云迭代地面点滤波滤波消除地面物体得到地面点
class LidarGroundFliter : public LidarFliterBase
{
public:
	//点云滤波处理过程
	long LidarFliterProcess(LASSet &lasDataset) { return 0; }

	//点云滤波单元
	long LidarFliter(LASSet &lasDataset) { return 0; }
};