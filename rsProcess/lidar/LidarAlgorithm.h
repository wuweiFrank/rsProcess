#pragma once
#include"LidarDefs.h"
#include"..\Global.h"

//感觉算法都是面向过程的，似乎不太好封装，或者说封装起来没什么意义

//根据Lidar数据创建三角网、这里有一个问题，似乎应该根据部分点进行创建，也就是传说中的LOD模型
//但是我现在还没理解LOD模型怎么实现，但是分块创建似乎是可行的

//通过视点位置和视点方向获取观察矩阵
long LidarGetRange(LASSet &m_lasDataset, THREEDPOINT viewPnt, THREEDPOINT viewDrection, Rect &viewRect);

//根据视点和方向创建三角网
long LidarTinCreate(LASSet &m_lasDataset, THREEDPOINT viewPnt, THREEDPOINT viewDrection);