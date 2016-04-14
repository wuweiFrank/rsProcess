#include"HyperRepair.h"
#include<map>

//获取分割块的边缘像素信息
void get_segment_edge(const char* img_path, vector<Edge_Pixels> &edgeinter)
{
	//首先获取分割块
	int seg_number = 0;
	
	//使用GDAL打开影像并获取数据
	GDALDatasetH m_dataset = GDALOpen(img_path, GA_ReadOnly);
	
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int* imgData = NULL;

	//数据空间的申请和数据读取
	try
	{
		imgData = new int[xsize*ysize];
		GDALRasterIO(GDALGetRasterBand(m_dataset, 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Int32, 0, 0);
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}

	//获取分割块数
	vector<int> seg;
	for (size_t i = 0; i < xsize*ysize; ++i)
	{
		bool isIn = false;
		for (size_t j = 0; j < seg.size(); ++j)
		{
			if (imgData[i] == seg[j]&& imgData[i] == seg[j]!=0)
				isIn = true;
		}
		if (!isIn)
		{
			Edge_Pixels m_pixels;
			m_pixels.m_segType = imgData[i];
			edgeinter.push_back(m_pixels);
			seg.push_back(imgData[i]);
		}
	}
	edgeinter.resize(seg.size());

	//获取每一个分割块的边界像素
	for (size_t i = 1; i < xsize-1; i++)
	{
		for (size_t j = 1; j < ysize-1; j++)
		{
			//获取块
			int idx = 0;
			for (int k = 0; k < seg.size(); ++k)
			{
				if (imgData[j*xsize + i] == seg[k])
					idx = k;
			}

			//判断是不是边界8邻域
			if (imgData[j*xsize + i] != imgData[j*xsize + i - 1] ||
				imgData[j*xsize + i] != imgData[j*xsize + i + 1] ||
				imgData[(j - 1)*xsize + i] != imgData[j*xsize + i] ||
				imgData[(j + 1)*xsize + i] != imgData[j*xsize + i] ||
				imgData[(j + 1)*xsize + i] != imgData[j*xsize + i + 1] ||
				imgData[(j + 1)*xsize + i] != imgData[j*xsize + i - 1] ||
				imgData[(j - 1)*xsize + i] != imgData[j*xsize + i + 1] ||
				imgData[(j - 1)*xsize + i] != imgData[j*xsize - i])
			{
				CPOINT pnt; pnt.x = i; pnt.y = j;
				edgeinter[idx].m_edge_pixels.push_back(pnt);
			}
		}
	}
}

void get_segment_intersection(const char* img_path, vector<Edge_Pixels> &edgeinter, int begLine, int endLine)
{

}