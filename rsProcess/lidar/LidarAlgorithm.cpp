#include <windows.h>
#include"LidarAlgorithm.h"
#include"..\AuxiliaryFunction.h"
#include "glut.h"
#pragma comment(lib,"glut.lib")

//获取视图投影平面
long LidarGetFrustumPlanes(float** g_frustumPlanes)
{
	float p[16];	// projection matrix
	float mv[16];	// model-view matrix
	float mvp[16];	// model-view-projection matrix
	float t;

	glGetFloatv(GL_PROJECTION_MATRIX, p);
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);

	//
	// Concatenate the projection matrix and the model-view matrix to produce
	// a combined model-view-projection matrix.
	//
	mvp[0] = mv[0] * p[0] + mv[1] * p[4] + mv[2] * p[8] + mv[3] * p[12];
	mvp[1] = mv[0] * p[1] + mv[1] * p[5] + mv[2] * p[9] + mv[3] * p[13];
	mvp[2] = mv[0] * p[2] + mv[1] * p[6] + mv[2] * p[10] + mv[3] * p[14];
	mvp[3] = mv[0] * p[3] + mv[1] * p[7] + mv[2] * p[11] + mv[3] * p[15];

	mvp[4] = mv[4] * p[0] + mv[5] * p[4] + mv[6] * p[8] + mv[7] * p[12];
	mvp[5] = mv[4] * p[1] + mv[5] * p[5] + mv[6] * p[9] + mv[7] * p[13];
	mvp[6] = mv[4] * p[2] + mv[5] * p[6] + mv[6] * p[10] + mv[7] * p[14];
	mvp[7] = mv[4] * p[3] + mv[5] * p[7] + mv[6] * p[11] + mv[7] * p[15];

	mvp[8] = mv[8] * p[0] + mv[9] * p[4] + mv[10] * p[8] + mv[11] * p[12];
	mvp[9] = mv[8] * p[1] + mv[9] * p[5] + mv[10] * p[9] + mv[11] * p[13];
	mvp[10] = mv[8] * p[2] + mv[9] * p[6] + mv[10] * p[10] + mv[11] * p[14];
	mvp[11] = mv[8] * p[3] + mv[9] * p[7] + mv[10] * p[11] + mv[11] * p[15];

	mvp[12] = mv[12] * p[0] + mv[13] * p[4] + mv[14] * p[8] + mv[15] * p[12];
	mvp[13] = mv[12] * p[1] + mv[13] * p[5] + mv[14] * p[9] + mv[15] * p[13];
	mvp[14] = mv[12] * p[2] + mv[13] * p[6] + mv[14] * p[10] + mv[15] * p[14];
	mvp[15] = mv[12] * p[3] + mv[13] * p[7] + mv[14] * p[11] + mv[15] * p[15];



	//
	// Extract the frustum's right clipping plane and normalize it.
	//
	g_frustumPlanes[0][0] = mvp[3] - mvp[0];
	g_frustumPlanes[0][1] = mvp[7] - mvp[4];
	g_frustumPlanes[0][2] = mvp[11] - mvp[8];
	g_frustumPlanes[0][3] = mvp[15] - mvp[12];

	t = (float)sqrt(g_frustumPlanes[0][0] * g_frustumPlanes[0][0] +
		g_frustumPlanes[0][1] * g_frustumPlanes[0][1] +
		g_frustumPlanes[0][2] * g_frustumPlanes[0][2]);

	g_frustumPlanes[0][0] /= t;
	g_frustumPlanes[0][1] /= t;
	g_frustumPlanes[0][2] /= t;
	g_frustumPlanes[0][3] /= t;

	//
	// Extract the frustum's left clipping plane and normalize it.
	//
	g_frustumPlanes[1][0] = mvp[3] + mvp[0];
	g_frustumPlanes[1][1] = mvp[7] + mvp[4];
	g_frustumPlanes[1][2] = mvp[11] + mvp[8];
	g_frustumPlanes[1][3] = mvp[15] + mvp[12];

	t = (float)sqrt(g_frustumPlanes[1][0] * g_frustumPlanes[1][0] +
		g_frustumPlanes[1][1] * g_frustumPlanes[1][1] +
		g_frustumPlanes[1][2] * g_frustumPlanes[1][2]);

	g_frustumPlanes[1][0] /= t;
	g_frustumPlanes[1][1] /= t;
	g_frustumPlanes[1][2] /= t;
	g_frustumPlanes[1][3] /= t;



	//
	// Extract the frustum's bottom clipping plane and normalize it.
	//
	g_frustumPlanes[2][0] = mvp[3] + mvp[1];
	g_frustumPlanes[2][1] = mvp[7] + mvp[5];
	g_frustumPlanes[2][2] = mvp[11] + mvp[9];
	g_frustumPlanes[2][3] = mvp[15] + mvp[13];

	t = (float)sqrt(g_frustumPlanes[2][0] * g_frustumPlanes[2][0] +
		g_frustumPlanes[2][1] * g_frustumPlanes[2][1] +
		g_frustumPlanes[2][2] * g_frustumPlanes[2][2]);

	g_frustumPlanes[2][0] /= t;
	g_frustumPlanes[2][1] /= t;
	g_frustumPlanes[2][2] /= t;
	g_frustumPlanes[2][3] /= t;
	//
	// Extract the frustum's top clipping plane and normalize it.
	//
	g_frustumPlanes[3][0] = mvp[3] - mvp[1];
	g_frustumPlanes[3][1] = mvp[7] - mvp[5];
	g_frustumPlanes[3][2] = mvp[11] - mvp[9];
	g_frustumPlanes[3][3] = mvp[15] - mvp[13];

	t = (float)sqrt(g_frustumPlanes[3][0] * g_frustumPlanes[3][0] +
		g_frustumPlanes[3][1] * g_frustumPlanes[3][1] +
		g_frustumPlanes[3][2] * g_frustumPlanes[3][2]);

	g_frustumPlanes[3][0] /= t;
	g_frustumPlanes[3][1] /= t;
	g_frustumPlanes[3][2] /= t;
	g_frustumPlanes[3][3] /= t;

	//
	// Extract the frustum's far clipping plane and normalize it.
	//
	g_frustumPlanes[4][0] = mvp[3] - mvp[2];
	g_frustumPlanes[4][1] = mvp[7] - mvp[6];
	g_frustumPlanes[4][2] = mvp[11] - mvp[10];
	g_frustumPlanes[4][3] = mvp[15] - mvp[14];

	t = (float)sqrt(g_frustumPlanes[4][0] * g_frustumPlanes[4][0] +
		g_frustumPlanes[4][1] * g_frustumPlanes[4][1] +
		g_frustumPlanes[4][2] * g_frustumPlanes[4][2]);

	g_frustumPlanes[4][0] /= t;
	g_frustumPlanes[4][1] /= t;
	g_frustumPlanes[4][2] /= t;
	g_frustumPlanes[4][3] /= t;
	//
	// Extract the frustum's near clipping plane and normalize it.
	//
	g_frustumPlanes[5][0] = mvp[3] + mvp[2];
	g_frustumPlanes[5][1] = mvp[7] + mvp[6];
	g_frustumPlanes[5][2] = mvp[11] + mvp[10];
	g_frustumPlanes[5][3] = mvp[15] + mvp[14];

	t = (float)sqrt(g_frustumPlanes[5][0] * g_frustumPlanes[5][0] +
		g_frustumPlanes[5][1] * g_frustumPlanes[5][1] +
		g_frustumPlanes[5][2] * g_frustumPlanes[5][2]);

	g_frustumPlanes[5][0] /= t;
	g_frustumPlanes[5][1] /= t;
	g_frustumPlanes[5][2] /= t;
	g_frustumPlanes[5][3] /= t;

	return 0;
}

//判断点是否在视图截体中
bool LidarPointInFrustum(float** g_frustumPlanes, THREEDPOINT pnt)
{
	for (int i = 0; i < 6; ++i)
	{
		if (g_frustumPlanes[i][0] * pnt.dX +
			g_frustumPlanes[i][1] * pnt.dY +
			g_frustumPlanes[i][2] * pnt.dZ +
			g_frustumPlanes[i][3] <= 0)
			return false;
	}
	return true;
}

//判断哪个矩形区域在视图截体中
long LidarGetRange(LASSet m_lasDataset, vector<int> &m_rectIdx)
{
	float** g_frustumPlanes;
	g_frustumPlanes = new float*[6];
	for (size_t i = 0; i < 6; i++)
		g_frustumPlanes[i] = new float[4];

	LidarGetFrustumPlanes(g_frustumPlanes);
	for (int i = 0; i < m_lasDataset.m_numRectangles; ++i)
	{
		if (LidarPointInFrustum(g_frustumPlanes, m_lasDataset.m_lasRectangles[i].m_rectCenter))
			m_rectIdx.push_back(i);
	}

	for (size_t i = 0; i < 6; i++)
		delete[]g_frustumPlanes[i];
	delete[]g_frustumPlanes;
	return 0;
}

//创建三角网
long LidarTinCreate(LASSet &m_lasDataset, THREEDPOINT centerView, vector<int> &m_rectIdx)
{
	//计算距离获取层次细节
	float* dis = NULL;
	try
	{
		dis = new float[m_rectIdx.size()];
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}
	for (int i = 0; i < m_rectIdx.size(); ++i)
		dis[i] = GetDisofPoints(m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_rectCenter, centerView);
	
	//获取数据点的范围
	double minWidth = MAX_NUM, maxWidth = MIN_NUM, minHeight = MAX_NUM, maxHeight = MIN_NUM;
	for (int i = 0; i < m_rectIdx.size(); ++i)
	{
		minWidth = min(m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_Rectangle.min[0], minWidth);
		maxWidth = max(m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_Rectangle.max[0], maxWidth);

		minHeight = min(m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_Rectangle.min[1], minHeight);
		maxHeight = max(m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_Rectangle.max[0], maxHeight);
	}
	double diagonal = min(maxWidth - minWidth, maxHeight - minHeight);
	float minElement = MAX_NUM;
	for (int i = 0; i < m_rectIdx.size(); ++i)
		minElement = min(minElement, dis[i]);
	for (int i = 0; i < m_rectIdx.size(); ++i)
		dis[i] = (dis[i]- minElement)/ (minElement+LIMIT); //防止为0值，虽然可能性不大

	for (int i = 0; i < m_rectIdx.size(); ++i)
		dis[i] +=0.5f+int(minElement/ diagonal);	   //四舍五入

	//计算顶点个数
	int totalVertix = 0;
	for (size_t i = 0; i < m_rectIdx.size(); i++)
		totalVertix += m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_lasPoints_numbers / int(dis[i] + 1);

	double *padX = NULL;
	double *padY = NULL;
	try
	{
		padX = new double[totalVertix];
		padY = new double[totalVertix];
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}

	totalVertix = 0;
	for (size_t i = 0; i < m_rectIdx.size(); i++)
	{
		for (size_t j = 0; j < m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_lasPoints_numbers; j+= int(dis[i] + 1))
		{
			padX[totalVertix] = m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_lasPoints[j].m_vec3d.x;
			padY[totalVertix] = m_lasDataset.m_lasRectangles[m_rectIdx[i]].m_lasPoints[j].m_vec3d.y;
			totalVertix++;
		}
	}
	m_lasDataset.m_lasTriangle = GDALTriangulationCreateDelaunay(totalVertix, padX, padY);

	//然后对点进行采样 根据距离进行采样
	if (dis != NULL)
		delete[]dis;
	if (padX != NULL)
		delete[]padX;
	if (padX != NULL)
		delete[]padX;
	padX = padY = NULL;
	dis = NULL;
}

