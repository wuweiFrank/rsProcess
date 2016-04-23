#pragma once
#include"LidarDefs.h"
#include"..\Global.h"

//根据视点和方向创建三角网
long LidarTinCreateLocal(LASSet &m_lasDataset);

//创建全局三角网
GDALTriangulation* LidarTinCreateGlobal(LASSet &m_lasDataset);