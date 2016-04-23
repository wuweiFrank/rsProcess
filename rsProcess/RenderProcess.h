#pragma once
#include"lidar\LidarDefs.h"
//²âÊÔÊı¾İäÖÈ¾Èı½ÇÍø
long LidarTriangleRenderLocal(LASSet* lasDataset);

long LidarTriangleRenderGlobal(LASSet* lasDataset, GDALTriangulation*  m_triangle);