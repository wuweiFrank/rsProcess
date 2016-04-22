#include <windows.h>
#include<omp.h>
#include"LidarAlgorithm.h"
#include"..\AuxiliaryFunction.h"


//创建三角网,这里有一个问题，每次视角的移动都需要重新构建三角网，这样会不会比较慢,暂时还没想到解决办法
long LidarTinCreate(LASSet &m_lasDataset)
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
		//然后对点进行采样 根据距离进行采样
		if (padX != NULL)
			delete[]padX;
		if (padY != NULL)
			delete[]padY;
		padX = padY = NULL;
	}
	//对块与块之间的数据建立一个三角网然后删除比较长的边
	return 0;
}

