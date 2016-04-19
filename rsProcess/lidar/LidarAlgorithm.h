#pragma once
#include"LidarDefs.h"
#include"..\Global.h"


//感觉算法都是面向过程的，似乎不太好封装，或者说封装起来没什么意义

//根据Lidar数据创建三角网、这里有一个问题，似乎应该根据部分点进行创建，也就是传说中的LOD模型
long LidarGetFrustumPlanes(float** g_frustumPlanes);

//判断点是否在截体中
bool LidarPointInFrustum(float** g_frustumPlanes,THREEDPOINT pnt);

//通过视点位置和视点方向获取观察矩阵
long LidarGetRange(LASSet m_lasDataset, vector<int> &m_rectIdx);

//根据视点和方向创建三角网
long LidarTinCreate(LASSet &m_lasDataset,THREEDPOINT centerView, vector<int> &m_rectIdx);