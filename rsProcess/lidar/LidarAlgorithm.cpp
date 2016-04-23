#include <windows.h>
#include<omp.h>
#include"LidarAlgorithm.h"
#include"..\AuxiliaryFunction.h"

//创建三角网,这里有一个问题，每次视角的移动都需要重新构建三角网，这样会不会比较慢,暂时还没想到解决办法
long LidarTinCreateLocal(LASSet &m_lasDataset)
{
#pragma omp parallel for
	for (int i = 0; i < m_lasDataset.m_numRectangles; ++i)
	{
		double *padX = NULL;
		double *padY = NULL;
		try
		{
			padX = new double[m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers];
			padY = new double[m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers];
		}
		catch (bad_alloc &e)
		{
			printf("%s\n", e.what());
			exit(-1);
		}

		for (size_t j = 0; j < m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers; j ++)
		{
			padX[j] = m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.x;
			padY[j] = m_lasDataset.m_lasRectangles[i].m_lasPoints[j].m_vec3d.y;
		}
		m_lasDataset.m_lasRectangles[i].m_lasTriangle=GDALTriangulationCreateDelaunay(m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers, padX, padY);
		if (padX != NULL)
			delete[]padX;
		if (padY != NULL)
			delete[]padY;
		padX = padY = NULL;
	}
	//对块与块之间的数据建立一个三角网然后删除比较长的边
	return 0;
}

//创建全局三角网
GDALTriangulation* LidarTinCreateGlobal(LASSet &m_lasDataset)
{
	//获取总点数
	int totalPoints = 0;
	for (size_t i = 0; i < m_lasDataset.m_numRectangles; ++i)
		totalPoints+=m_lasDataset.m_lasRectangles[i].m_lasPoints_numbers;

	//数据点
	double *padX = NULL;
	double *padY = NULL;
	try
	{
		padX = new double[totalPoints];
		padY = new double[totalPoints];
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}

	for (int i = 0; i < totalPoints; ++i)
	{
		int idxrect = m_lasDataset.m_LASPointID[i].rectangle_idx;
		int idxpnts = m_lasDataset.m_LASPointID[i].point_idx_inRect;

		padX[i] = m_lasDataset.m_lasRectangles[idxrect].m_lasPoints[idxpnts].m_vec3d.x;
		padY[i] = m_lasDataset.m_lasRectangles[idxrect].m_lasPoints[idxpnts].m_vec3d.y;
	}
	GDALTriangulation* m_lasTriangle = NULL;
	m_lasTriangle = GDALTriangulationCreateDelaunay(totalPoints, padX, padY);
	if (padX != NULL)
		delete[]padX;
	if (padY != NULL)
		delete[]padY;
	padX = padY = NULL;
	return m_lasTriangle;
}