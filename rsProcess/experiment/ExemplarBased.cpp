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
		ExemplarBased_PriorityInpaint(pos, regionsize, imgData, mskData, xsize, ysize);
		GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, xsize, ysize, 1, GDT_Float32, NULL);
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, 1), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
		GDALClose(m_datasetDst);

		ExemplarBased_UpdateMask(imgData, tmpMskData, xsize, ysize);
		ExemplarBased_UpdateEdge(tmpMskData, xsize, ysize, edges);
	}

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
	int regionsize = 27*27;
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

	float* data1 = new float[num];
	float* data2 = new float[num];

	num = 0;
	for (int i = pos.x - regionSize; i <= pos.x + regionSize; ++i)
	{
		for (int j = pos.y - regionSize; j <= pos.y + regionSize; ++j)
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
	for (int i =0; i <2*regionSize+1; ++i)
	{
		for (int j = 0; j <2* regionSize+1; ++j)
		{
			if (imgData[(j + pos.y- regionSize)* xsize + i + pos.x- regionSize] == -1)
			{
				imgData[(j + pos.y- regionSize)* xsize + i + pos.x-regionSize] = imgData[(j + maxidy)* xsize + i + maxidx];
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
	double Ix = imgData[pos.y*xsize + pos.x + 1] - imgData[pos.y*xsize + pos.x - 1];
	double Iy = imgData[(pos.y+1)*xsize + pos.x] - imgData[(pos.y-1)*xsize + pos.x - 1];

	ip[0] = Ix;
	ip[1] = Iy;
}