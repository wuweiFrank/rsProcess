#pragma once
#include"LidarDefs.h"
#include"..\Global.h"

//创建三角网局部
long LidarTinCreateLocal(LASSet &m_lasDataset);

//创建全局三角网
GDALTriangulation* LidarTinCreateGlobal(LASSet &m_lasDataset);

//创建规则格网局部

