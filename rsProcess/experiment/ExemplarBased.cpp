#include"ExemplarBased.h"
#include"..\gdal\include\gdal_priv.h"
#include"..\AuxiliaryFunction.h"
using namespace std;
#pragma comment(lib,"gdal_i.lib")

void ExemplarBased::ExemplarBased_Inpaint(const char* pathImg, const char* pathMsk, const char* pathDst)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImg, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMsk, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetMsk);
	int regionsize = 5;

	//数据申请
	float* imgData = new float[xsize*ysize];
	float* mskData = new float[xsize*ysize];
	float* tmpMskData = new float[xsize*ysize];

	GDALRasterIO(GDALGetRasterBand(m_datasetImg, 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, mskData, xsize, ysize, GDT_Float32, 0, 0);

	vector<CPOINT> edges;
	ExemplarBased_UpdateEdge(mskData, xsize, ysize, edges);
	memcpy(tmpMskData, mskData, sizeof(float)*xsize*ysize);
	while (edges.size() != 0)
	{
		CPOINT pos;
		ExemplarBased_GetPriority(imgData, xsize, ysize, edges, pos);
		ExemplarBased_PriorityInpaint(pos, regionsize, imgData, mskData, xsize, ysize);
		ExemplarBased_UpdateMask(imgData, tmpMskData, xsize, ysize);
		ExemplarBased_UpdateEdge(tmpMskData, xsize, ysize, edges);
	}
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetDst, 1), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetDst);
	//数据输出
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);
	delete[]imgData;
	delete[]mskData;
	delete[]tmpMskData;
}
void ExemplarBased::ExemplarBased_InpaintTexture(const char* pathImg, const char* pathMsk, const char* pathTexture, const char* pathDst)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImg, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMsk, GA_ReadOnly);
	GDALDatasetH m_datasetTxt = GDALOpen(pathTexture, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetMsk);
	int regionsize = 5;

	//数据申请
	float* imgData = new float[xsize*ysize];
	float* mskData = new float[xsize*ysize];
	float* txtData = new float[xsize*ysize];
	float* tmpMskData = new float[xsize*ysize];

	GDALRasterIO(GDALGetRasterBand(m_datasetImg, 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, mskData, xsize, ysize, GDT_Float32, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetTxt, 1), GF_Read, 0, 0, xsize, ysize, txtData, xsize, ysize, GDT_Float32, 0, 0);
	ExemplarBased_Init(imgData, mskData, xsize, ysize);
	vector<CPOINT> edges;
	ExemplarBased_UpdateEdge(mskData, xsize, ysize, edges);
	memcpy(tmpMskData, mskData, sizeof(float)*xsize*ysize);
	while (edges.size() != 0)
	{
		CPOINT pos;
		ExemplarBased_GetPriority(imgData, xsize, ysize, edges, pos);
		printf("first pos: %d  %d\n", pos.x, pos.y);
		ExemplarBased_ProprityInpaintTexture(pos, regionsize, imgData, tmpMskData, mskData,txtData, xsize, ysize);

		//ExemplarBased_UpdateMask(imgData, tmpMskData, xsize, ysize);
		ExemplarBased_UpdateEdge(tmpMskData, xsize, ysize, edges);
	}

	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetDst, 1), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetDst);

	//数据输出
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);
	delete[]imgData;
	delete[]mskData;
	delete[]tmpMskData;
}
void ExemplarBased::ExemplarBased_InpaintLess(const char* pathImg, const char* pathMsk, const char* pathDst)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImg, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMsk, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetMsk);
	int regionsize = 13;

	//数据申请
	float* imgData = new float[xsize*ysize];
	float* mskData = new float[xsize*ysize];
	float* tmpMskData = new float[xsize*ysize];

	GDALRasterIO(GDALGetRasterBand(m_datasetImg, 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, mskData, xsize, ysize, GDT_Float32, 0, 0);

	vector<CPOINT> edges;
	ExemplarBased_UpdateEdge(mskData, xsize, ysize, edges);
	memcpy(tmpMskData, mskData, sizeof(float)*xsize*ysize);
	while (edges.size() != 0)
	{
		CPOINT pos;
		ExemplarBased_GetPriority(imgData, xsize, ysize, edges, pos);
		ExemplarBased_PriorityInpaintLess(pos, regionsize, imgData, mskData, xsize, ysize);
		ExemplarBased_UpdateMask(imgData, tmpMskData, xsize, ysize);
		ExemplarBased_UpdateEdge(tmpMskData, xsize, ysize, edges);
	}
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetDst, 1), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetDst);
	//数据输出
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);
	delete[]imgData;
	delete[]mskData;
	delete[]tmpMskData;
}
void ExemplarBased::ExemplarBased_UpdateEdge(float* mskData, int xsize, int ysize, vector<CPOINT> &edge)
{
	edge.clear();
	for (int i = 1; i < xsize-1; ++i)
	{
		for (int j = 1; j < ysize-1; ++j)
		{
			if (mskData[j*xsize + i] == -1 && (mskData[j*xsize + i + 1] != -1 || mskData[j*xsize + i - 1] != -1
				|| mskData[(j + 1)*xsize + i] != -1 || mskData[(j + 1)*xsize + i - 1] != -1 || mskData[(j + 1)*xsize + i + 1] != -1
				|| mskData[(j - 1)*xsize + i] != -1 || mskData[(j - 1)*xsize + i - 1] != -1 || mskData[(j - 1)*xsize + i - 1] != -1))
			{
				CPOINT tmpPnt;
				tmpPnt.x = i; tmpPnt.y = j;
				edge.push_back(tmpPnt);
			}
		}
	}
}

void ExemplarBased::ExemplarBased_UpdateMask(float* imgData, float *mskData, int xsize, int ysize)
{
	for (int i = 1; i < xsize - 1; ++i)
	{
		for (int j = 1; j < ysize - 1; ++j)
		{
			if (imgData[j*xsize+i]==-1)
				mskData[j*xsize + i] = -1;
			else
				mskData[j*xsize + i] = 1;
		}
	}
}

void ExemplarBased::ExemplarBased_GetPriority(float* imgData, int xsize, int ysize, vector<CPOINT> edge, CPOINT &pos)
{
	//计算每一个边界点的优先系数
	double* priority = new double[edge.size()];
	int regionsize = 11*11;
	for (int i = 0; i < edge.size(); ++i)
	{
		int t1 = edge[i].x;
		int t2 = edge[i].y;
		double total = 0;
		for (int m = t1 - 5; m <= t1 + 5; m++)
		{
			for (int n = t2 - 5; n <= t2+5; n++)
			{
				if(imgData[n*xsize+m]!=-1)
					total++;
			}
		}
		double Cp = total / double(regionsize);
		double *np = new double[2];
		double *ip = new double[2];
		ExemplarBased_Np(edge, edge[i], imgData, xsize, ysize, np);
		ExemplarBased_Ip(imgData, edge[i], xsize, ysize, ip);
		double Dp = abs(np[0]*ip[0]+np[1]*ip[1]);
		priority[i] = Cp*Dp;
		delete[]np;
		delete[]ip;
	}
	double max_num = priority[0];
	int idx = 0;
	for (int i = 0; i < edge.size();++i)
	{
		if (max_num < priority[i])
		{
			idx = i;
			max_num = priority[i];
		}
	}

	pos.x = edge[idx].x;
	pos.y = edge[idx].y;

	delete[]priority;
}

void ExemplarBased::ExemplarBased_PriorityInpaint(CPOINT pos, int regionSize, float* imgData, float* mskData, int xsize, int ysize)
{
	float *regionmask=new float[(2* regionSize +1)*(2* regionSize + 1)];
	memset(regionmask, 0, sizeof(float) * (2 * regionSize + 1)*(2 * regionSize + 1));
	int num = 0,lastnum;
	float raio = 0.2;
	CPOINT tmpPos;
	tmpPos.x = pos.x; tmpPos.y = pos.y;
	num = 0;
	for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
	{
		for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
		{
			if (imgData[j * xsize + i] != -1)
			{
				regionmask[(j - (tmpPos.y - regionSize))*(2 * regionSize + 1) + i - (tmpPos.x - regionSize)] = 1;
				num++;
			}
			else
				regionmask[(j - (tmpPos.y - regionSize))*(2 * regionSize + 1) + i - (tmpPos.x - regionSize)] = 0;
		}
	}

	float* data1 = new float[num];
	float* data2 = new float[num];

	num = 0;
	for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
	{
		for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
		{
			if (imgData[j * xsize + i] != -1)
			{
				data1[num] = imgData[j * xsize + i];
				num++;
			}
		}
	}

	double maxrel = 999999999; int maxidx = 0, maxidy=0;
	for (int i =0; i <xsize-2* regionSize -1; ++i)
	{
		for (int j = 0; j <ysize - 2 * regionSize - 1; ++j)
		{
			bool is = false;
			num = 0;
			for (int m = 0; m < (2 * regionSize + 1); m++)
			{
				for (int n = 0; n < (2 * regionSize + 1); n++)
				{
					if (mskData[(j + n)*xsize + i+m] == -1)
						is = true;
					else
					{
						if (regionmask[n*(2 * regionSize + 1) + m] == 1)
						{
							data2[num] = imgData[(j + n)*xsize + i+m];
							num++;
						}
					}
				}
			}//内层循环
			if (!is)
			{
				//空间值的相似性
				float tmp= GetSSD(data1, data2, num);
				if (tmp<maxrel)
				{
					maxrel = tmp;
					maxidx = i; maxidy = j;
				}
			}

		}
	}//遍历整个影像的循环
	printf("best match:%d  %d\n", maxidx, maxidy);

	//修补
	//for (int i =0; i <2*regionSize+1; ++i)
	//{
	//	for (int j = 0; j <2* regionSize+1; ++j)
	//	{
	//		if (imgData[(j + tmpPos.y- regionSize)* xsize + i + tmpPos.x- regionSize] == -1)
	//		{
	//			imgData[(j + tmpPos.y- regionSize)* xsize + i + tmpPos.x-regionSize] = imgData[(j + maxidy)* xsize + i + maxidx];
	//		}
	//	}
	//}
	//只修复中心像素
	imgData[(regionSize + tmpPos.y - regionSize)* xsize + regionSize + tmpPos.x - regionSize] = imgData[(regionSize + maxidy)* xsize + regionSize + maxidx];
	delete[]regionmask;
	delete[]data1;
	delete[]data2;
}
void ExemplarBased::ExemplarBased_PriorityInpaintLess(CPOINT pos, int regionSize, float* imgData, float* mskData, int xsize, int ysize)
{
	float *regionmask = new float[(2 * regionSize + 1)*(2 * regionSize + 1)];
	memset(regionmask, 0, sizeof(float) * (2 * regionSize + 1)*(2 * regionSize + 1));
	int num = 0, lastnum;
	float raio = 0.1;
	CPOINT tmpPos;
	tmpPos.x = pos.x; tmpPos.y = pos.y;

	for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
	{
		for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
		{
			if (imgData[j * xsize + i] != -1)
				num++;
		}
	}
	lastnum = num;
	int iter = 0;
	do
	{
		lastnum = num;
		num = 0;
		for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
		{
			for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
			{
				if (imgData[j * xsize + i] != -1)
					num++;
			}
		}
		if (float(num) / float((2 * regionSize + 1)*(2 * regionSize + 1)) > 1 - raio)
			break;
		else if (iter % 2 == 0)
		{
			tmpPos.x += 1;
			num = 0;
			for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
			{
				for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
				{
					if (imgData[j * xsize + i] != -1)
						num++;
				}
			}
			if (num <= lastnum)
				tmpPos.x -= 2;
			else
				lastnum = num;
		}
		else if (iter % 2 == 1)
		{
 			tmpPos.y += 1;
			num = 0;
			for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
			{
				for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
				{
					if (imgData[j * xsize + i] != -1)
						num++;
				}
			}
			if (num <= lastnum)
				tmpPos.y -= 2;
			else
				lastnum = num;
		}
		iter = (iter + 1) % 2;
		
	} while (true);
	num = 0;
	for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
	{
		for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
		{
			if (imgData[j * xsize + i] != -1)
			{
				regionmask[(j - (tmpPos.y - regionSize))*(2 * regionSize + 1) + i - (tmpPos.x - regionSize)] = 1;
				num++;
			}
			else
				regionmask[(j - (tmpPos.y - regionSize))*(2 * regionSize + 1) + i - (tmpPos.x - regionSize)] = 0;
		}
	}

	float* data1 = new float[num];
	float* data2 = new float[num];

	num = 0;
	for (int i = tmpPos.x - regionSize; i <= tmpPos.x + regionSize; ++i)
	{
		for (int j = tmpPos.y - regionSize; j <= tmpPos.y + regionSize; ++j)
		{
			if (imgData[j * xsize + i] != -1)
			{
				data1[num] = imgData[j * xsize + i];
				num++;
			}
		}
	}

	double maxrel = 999999999; int maxidx = 0, maxidy = 0;
	for (int i = 0; i <xsize - 2 * regionSize - 1; ++i)
	{
		for (int j = 0; j <ysize - 2 * regionSize - 1; ++j)
		{
			bool is = false;
			num = 0;
			for (int m = 0; m < (2 * regionSize + 1); m++)
			{
				for (int n = 0; n < (2 * regionSize + 1); n++)
				{
					if (mskData[(j + n)*xsize + i + m] == -1)
						is = true;
					else
					{
						if (regionmask[n*(2 * regionSize + 1) + m] == 1)
						{
							data2[num] = imgData[(j + n)*xsize + i + m];
							num++;
						}
					}
				}
			}//内层循环
			if (!is)
			{
				//空间值的相似性
				float tmp = GetSSD(data1, data2, num);
				if (tmp<maxrel)
				{
					maxrel = tmp;
					maxidx = i; maxidy = j;
				}
			}

		}
	}//遍历整个影像的循环
	printf("best match:%d  %d\n", maxidx, maxidy);

	//修补
	for (int i = 0; i <2 * regionSize + 1; ++i)
	{
		for (int j = 0; j <2 * regionSize + 1; ++j)
		{
			if (imgData[(j + tmpPos.y - regionSize)* xsize + i + tmpPos.x - regionSize] == -1)
			{
				imgData[(j + tmpPos.y - regionSize)* xsize + i + tmpPos.x - regionSize] = imgData[(j + maxidy)* xsize + i + maxidx];
			}
		}
	}


	delete[]regionmask;
	delete[]data1;
	delete[]data2;
}
void ExemplarBased::ExemplarBased_ProprityInpaintTexture(CPOINT pos, int regionSize, float* imgData, float *tmpMsk,float* mskData, float* txtData, int xsize, int ysize)
{
	float *regionmask = new float[(2 * regionSize + 1)*(2 * regionSize + 1)];
	memset(regionmask, 0, sizeof(float) * (2 * regionSize + 1)*(2 * regionSize + 1));
	int num = 0;
	for (int i = pos.x - regionSize; i <= pos.x + regionSize; ++i)
	{
		for (int j = pos.y - regionSize; j <= pos.y + regionSize; ++j)
		{
			if (imgData[j * xsize + i] != -1)
			{
				regionmask[(j - (pos.y - regionSize))*(2 * regionSize + 1) + i - (pos.x - regionSize)] = 1;
				num++;
			}
			else
				regionmask[(j - (pos.y - regionSize))*(2 * regionSize + 1) + i - (pos.x - regionSize)] = 0;
		}
	}
	//强度影像
	float* data1 = new float[num];
	float* data2 = new float[num];
	//纹理影像
	float* data3 = new float[num];
	float* data4 = new float[num];

	num = 0;
	for (int i = pos.x - regionSize; i <= pos.x + regionSize; ++i)
	{
		for (int j = pos.y - regionSize; j <= pos.y + regionSize; ++j)
		{
			if (imgData[j * xsize + i] != -1)
			{
				data1[num] = imgData[j * xsize + i];
				data3[num] = txtData[j * xsize + i];
				num++;
			}
		}
	}

	double maxrel = 999999999; int maxidx = 0, maxidy = 0;
	for (int i = 0; i <xsize - 2 * regionSize - 1; ++i)
	{
		for (int j = 0; j <ysize - 2 * regionSize - 1; ++j)
		{
			bool is = false;
			num = 0;
			for (int m = 0; m < (2 * regionSize + 1); m++)
			{
				for (int n = 0; n < (2 * regionSize + 1); n++)
				{
					if (mskData[(j + n)*xsize + i + m] == -1)
						is = true;
					else
					{
						if (regionmask[n*(2 * regionSize + 1) + m] == 1)
						{
							data2[num] = imgData[(j + n)*xsize + i + m];
							data4[num] = txtData[(j + n)*xsize + i + m];
							num++;
						}
					}
				}
			}//内层循环
			if (!is)
			{
				//空间值的相似性
				float tmp1 = GetSSD(data1, data2, num);
				float tmp2 = GetSSD(data3, data4, num);
				float tmp = 0.7*tmp1 + 0.3*tmp2;
				if (tmp<maxrel)
				{
					maxrel = tmp;
					maxidx = i; maxidy = j;
				}
			}
		}
	}//遍历整个影像的循环
	printf("best match:%d  %d\n", maxidx, maxidy);

	//修补
	for (int i = 0; i <2 * regionSize + 1; ++i)
	{
		for (int j = 0; j <2 * regionSize + 1; ++j)
		{
			if (tmpMsk[(j + pos.y - regionSize)* xsize + i + pos.x - regionSize] == -1)
			{
				imgData[(j + pos.y - regionSize)* xsize + i + pos.x - regionSize] = imgData[(j + maxidy)* xsize + i + maxidx];
				tmpMsk[(j + pos.y - regionSize)* xsize + i + pos.x - regionSize] = 1;
			}
		}
	}

	delete[]regionmask;
	delete[]data1;
	delete[]data2;
}

void ExemplarBased::ExemplarBased_Np(vector<CPOINT> edge, CPOINT pos, float* imgData, int xisze, int ysize, double*np)
{
	CPOINT posLeft, posRight;
	int x = pos.x;
	int y = pos.y;
	bool left = false, right = false;
	if (imgData[y*xisze + x - 1] == -1)
	{
		posLeft.x = x - 1;
		posLeft.y = y;
		left = true;
	}
	else if (imgData[(y - 1)*xisze + x - 1] == -1)
	{
		posLeft.x = x - 1;
		posLeft.y = y-1;
		left = true;
	}
	else if (imgData[(y + 1)*xisze + x - 1] == -1)
	{
		posLeft.x = x - 1;
		posLeft.y = y - 1;
		left = true;
	}
	else if (imgData[(y - 1)*xisze + x] == -1)
	{
		posLeft.x = x;
		posLeft.y = y - 1;
		left = true;
	}
	else if (imgData[(y + 1)*xisze + x ] == -1)
	{
		posLeft.x = x;
		posLeft.y = y - 1;
		left = true;
	}

	/*---------*/
	if (imgData[y*xisze + x + 1] == -1)
	{
		posRight.x = x - 1;
		posRight.y = y;
		right = true;
	}
	else if (imgData[(y - 1)*xisze + x + 1] == -1)
	{
		posRight.x = x - 1;
		posRight.y = y - 1;
		right = true;
	}
	else if (imgData[(y + 1)*xisze + x + 1] == -1)
	{
		posRight.x = x - 1;
		posRight.y = y - 1;
		right = true;
	}
	else if (imgData[(y - 1)*xisze + x] == -1)
	{
		posRight.x = x;
		posRight.y = y - 1;
		right = true;
	}
	else if (imgData[(y + 1)*xisze + x] == -1)
	{
		posRight.x = x;
		posRight.y = y - 1;
		right = true;
	}

	//然后判断
	if (!left&&!right)
	{
		np[0] = 100;
		np[1] = 100;
	}
	else if(left&&!right)
	{
		np[0] = -double(pos.y - posLeft.y)/sqrt(double((pos.x - posLeft.x)*(pos.x - posLeft.x)) + double((pos.y - posLeft.y)*(pos.y - posLeft.y)));
		np[1] = -double(pos.x - posLeft.x)/sqrt(double((pos.x - posLeft.x)*(pos.x - posLeft.x)) + double((pos.y - posLeft.y)*(pos.y - posLeft.y)));
	}
	else if (!left&&right)
	{
		np[0] = -double(pos.y - posRight.y) / sqrt(double((pos.x - posRight.x)*(pos.x - posRight.x)) + double((pos.y - posRight.y)*(pos.y - posRight.y)));
		np[1] = -double(pos.x - posRight.x) / sqrt(double((pos.x - posRight.x)*(pos.x - posRight.x)) + double((pos.y - posRight.y)*(pos.y - posRight.y)));
	}
	else
	{
		if (posLeft.x == posRight.x&&posLeft.y == posRight.y)
		{
			np[0] = 1;
			np[1] = 1;
		}
		else
		{
			np[0] = -double(posLeft.y - posRight.y) / sqrt(double((posLeft.x - posRight.x)*(posLeft.x - posRight.x)) + double((posLeft.y - posRight.y)*(posLeft.y - posRight.y)));
			np[1] = -double(posLeft.x - posRight.x) / sqrt(double((posLeft.x - posRight.x)*(posLeft.x - posRight.x)) + double((posLeft.y - posRight.y)*(posLeft.y - posRight.y)));
		}
	}
}

void ExemplarBased::ExemplarBased_Ip(float* imgData, CPOINT pos, int xsize, int ysize, double*ip)
{
	int x = pos.x, y = pos.y;
	double laplacex = 0.5*(MinAvg(MaxAvg(imgData[x + 2 + y*xsize], imgData[x + 1 + y*xsize], imgData[x + y*xsize]),
		max(imgData[x + 1 + (y + 1)*xsize], imgData[x + 1 + y*xsize]),
		max(imgData[x + 1 + (y - 1)*xsize], imgData[x + 1 + y*xsize]) - MinAvg(MaxAvg(imgData[x - 2 + y*xsize], imgData[x - 1 + y*xsize], imgData[x + y*xsize]), max(imgData[x - 1 + (y + 1)*xsize], imgData[x - 1 + y*xsize]), max(imgData[x - 1 + (y - 1)*xsize], imgData[x - 1 + y*xsize]))));
	double laplacey = 0.5*(MinAvg(MaxAvg(imgData[x + (y + 2)*xsize], imgData[x + (y + 1)*xsize], imgData[x + y*xsize]),
		max(imgData[x + 1 + (y + 1)*xsize], imgData[x + (y + 1)*xsize]),
		max(imgData[x - 1 + (y + 1)*xsize], imgData[x + 1 + y*xsize]) - MinAvg(MaxAvg(imgData[x + (y - 2)*xsize], imgData[x + (y - 1)*xsize], imgData[x + y*xsize]), max(imgData[x + 1 + (y - 1)*xsize], imgData[x - 1 + y*xsize]), max(imgData[x - 1 + (y - 1)*xsize], imgData[x - 1 + y*xsize]))));

	ip[0] = laplacex;
	ip[1] = laplacey;
}

void ExemplarBased::ExemplarBased_BestInpaint(float* data1, float* data2, int xsize, int ysize)
{


}

void ExemplarBased::ExemplarBased_Init(float* data, float *maskData, int xsize, int ysize)
{
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			if (maskData[j*xsize + i] == -1)
			{
				CPOINT pnt[8];
				int nerb = 0, iter = 0;
				//找到四邻域方向的像素值
				int tempxleft = i, tempxright = i, tempyup = j, tempydown = j;
				bool left = false, right = false, up = false, down = false;
				do
				{
					if (tempxleft > 0)
						tempxleft--;
					if (tempxright < xsize - 1)
						tempxright++;
					if (tempyup > 0)
						tempyup--;
					if (tempydown < ysize - 1)
						tempydown++;

					if (maskData[tempxleft + j*xsize] != -1 && !left)
					{
						pnt[nerb].x = tempxleft; pnt[nerb].y = j;
						left = true;
						nerb++;
						iter++;
					}
					if (maskData[tempxright + j*xsize] != -1 && !right)
					{
						pnt[nerb].x = tempxright; pnt[nerb].y = j;
						right = true;
						nerb++;
						iter++;
					}
					if (maskData[i + tempyup*xsize] != -1 && !up)
					{
						pnt[nerb].x = i; pnt[nerb].y = tempyup;
						up = true;
						nerb++;
						iter++;
					}
					if (maskData[i + tempydown*xsize] != -1 && !down)
					{
						pnt[nerb].x = i; pnt[nerb].y = tempydown;
						down = true;
						nerb++;
						iter++;
					}
					if (tempxleft == 0)
					{
						iter++;
						left = true;
					}
					if (tempxright == xsize - 1)
					{
						iter++;
						right = true;
					}
					if (tempyup == 0)
					{
						iter++;
						up = true;
					}
					if (tempydown == ysize - 1)
					{
						down = true;
						iter++;
					}
				} while (iter<4);

				//四个方向考虑距离的插值
				double totaldis = 0;
				for (int iterator = 0; iterator < nerb; ++iterator)
				{
					CPOINT tpnt; tpnt.x = i; tpnt.y = j;
					totaldis += 1.0 / GetDisofPoints(tpnt, pnt[iterator]);
				}
				float datavalue = 0;
				for (int iterator = 0; iterator < nerb; ++iterator)
				{
					CPOINT tpnt; tpnt.x = i; tpnt.y = j;
					double dis = 1.0 / GetDisofPoints(tpnt, pnt[iterator]);
					datavalue += dis / totaldis*data[pnt[iterator].x + pnt[iterator].y*xsize];
				}
				data[j*xsize + i] = datavalue;
			}
		}
	}
}