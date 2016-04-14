#include"HyperRepair.h"
#include<map>

//获取分割块的边缘像素信息
void get_segment_edge(const char* img_path, vector<Edge_Pixels> &edgeinter)
{
	//使用GDAL打开影像并获取数据
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
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
		if (imgData[i] == 255 || imgData[i] == 0)
			continue;
		for (size_t j = 0; j < seg.size(); ++j)
		{
			if (imgData[i] == seg[j] || imgData[i] == 0 || imgData[i]  == 255)
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
			if (imgData[j*xsize+i] == 0 || imgData[j*xsize + i] == 255)
				continue;
			//获取块
			int idx = 0;
			for (int k = 0; k < seg.size(); ++k)
			{
				if (imgData[j*xsize + i] == seg[k])
					idx = k;
			}
			
			//判断是不是边界8邻域
			if (imgData[j*xsize + i] != imgData[j*xsize + i - 1]	   ||
				imgData[j*xsize + i] != imgData[j*xsize + i + 1]	   ||
				imgData[(j - 1)*xsize + i] != imgData[j*xsize + i]	   ||
				imgData[(j + 1)*xsize + i] != imgData[j*xsize + i]	   ||
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
	if (imgData != NULL)
		delete[]imgData;
	imgData = NULL;
	GDALClose(m_dataset);
}

//获取交点信息
void get_segment_intersection(const char* img_path, vector<Edge_Pixels> &intersecEdge, int begLine, int endLine)
{
	//使用GDAL打开影像并获取数据
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
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
	//获取分割块数
	vector<int> seg;
	for (size_t i = 0; i < xsize*ysize; ++i)
	{
		bool isIn = false;
		if (imgData[i] == 255 || imgData[i] == 0)
			continue;
		for (size_t j = 0; j < seg.size(); ++j)
		{
			if (imgData[i] == seg[j] || imgData[i] == 0 || imgData[i] == 255)
				isIn = true;
		}
		if (!isIn)
		{
			Edge_Pixels m_pixels;
			m_pixels.m_segType = imgData[i];
			intersecEdge.push_back(m_pixels);
			seg.push_back(imgData[i]);
		}
	}

	//获取边界数据
	for (size_t i = 0; i < ysize; i++)
	{
		if (imgData[i*xsize + begLine] == 255)
		{
			for (size_t j = 0; j < intersecEdge.size(); j++)
			{
				CPOINT pnt;

				if (i != ysize - 1)
				{
					pnt.x = begLine; pnt.y = (i + 1);
					if (intersecEdge[j].m_segType == imgData[(i + 1)*xsize + begLine])
						intersecEdge[j].m_edge_pixels.push_back(pnt);
				}
				if (i != 0)
				{
					pnt.x = begLine; pnt.y = (i - 1);
					if (intersecEdge[j].m_segType == imgData[(i - 1)*xsize + begLine])
						intersecEdge[j].m_edge_pixels.push_back(pnt);
				}
			}
		}
		if (imgData[i*xsize + endLine] == 255)
		{
			for (size_t j = 0; j < intersecEdge.size(); j++)
			{
				CPOINT pnt; 
				if (i != ysize - 1)
				{
					pnt.x = endLine; pnt.y = (i + 1);
					if (intersecEdge[j].m_segType == imgData[(i + 1)*xsize + endLine])
						intersecEdge[j].m_edge_pixels.push_back(pnt);
				}
				if (i != 0)
				{
					pnt.x = endLine; pnt.y = (i - 1);
					if (intersecEdge[j].m_segType == imgData[(i - 1)*xsize + endLine])
						intersecEdge[j].m_edge_pixels.push_back(pnt);
				}
			}
		}
	}
	
	if (imgData != NULL)
		delete[]imgData;
	imgData = NULL;
	GDALClose(m_dataset);
}





//实验处理
void experiment_process()
{
	vector<Edge_Pixels>  edgeinter;
	vector<Edge_Pixels> intersecEdge;
	int begLine = 417;
	int endLine = 520;
	char* pathimg = "D:\\watershed.bmp";
	get_segment_edge(pathimg, edgeinter);
	get_segment_intersection(pathimg, intersecEdge, begLine, endLine);

	//将求解结果输出
	for (int i = 0; i < intersecEdge.size(); ++i)
	{
		printf("class type : %d\n", intersecEdge[i].m_segType);
		for (size_t j = 0; j < intersecEdge[i].m_edge_pixels.size(); j++)
		{
			printf("position of edge : %d  %d\n", intersecEdge[i].m_edge_pixels[j].x, intersecEdge[i].m_edge_pixels[j].y);
		}
	}
}