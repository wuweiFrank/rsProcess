#include"HyperRepair.h"
#include"..\AuxiliaryFunction.h"
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
		printf("process pixel %d\r", i + 1);
		bool isIn = false;
		if (imgData[i] == -1)
			continue;
		for (size_t j = 0; j < seg.size(); ++j)
		{
			if (imgData[i] == seg[j] ||imgData[i]  == -1)
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

//	//获取每一个分割块的边界像素 i++)
//	{
//		for (size_t j = 1; j < ysize-1; j++)
//		{
//			printf("process cols %d\r", i + 1);
//			if (imgData[j*xsize+i] == 0 || imgData[j*xsize + i] == 255)
//				continue;
//			//获取块
//			int idx = 0;
//			for (int k = 0; k < seg.size(); ++k)
//			{
//				if (imgData[j*xsize + i] == seg[k])
//					idx = k;
//			}
//			
//			//判断是不是边界8邻域
//			if (imgData[j*xsize + i] != imgData[j*xsize + i - 1]	   ||
//				imgData[j*xsize + i] != imgData[j*xsize + i + 1]	   ||
//				imgData[(j - 1)*xsize + i] != imgData[j*xsize + i]	   ||
//				imgData[(j + 1)*xsize + i] != imgData[j*xsize + i]	   ||
//				imgData[(j + 1)*xsize + i] != imgData[j*xsize + i + 1] ||
//				imgData[(j + 1)*xsize + i] != imgData[j*xsize + i - 1] ||
//				imgData[(j - 1)*xsize + i] != imgData[j*xsize + i + 1] ||
//				imgData[(j - 1)*xsize + i] != imgData[j*xsize - i])
//			{
////				CPOINT pnt; pnt.x = i; pnt.y = j;
////				edgeinter[idx].m_edge_pixels.push_back(pnt);
//			}
//		}
//	}
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

//获取拟合点，这里获取拟合点比较麻烦，与边界有多少个交点就需要获取多少个拟合点列
void get_fit_points(vector<Edge_Pixels> edgeinter, Edge_Pixels preIntersectEdge, vector<CPOINT> &fitpoints, int xsize, int ysize)
{
	//获取边界点的最大最小值
	//首先判断边界点是否为背景点，若为背景点，则不参与
	bool isbeg = false, isend = false;
	for (int i = 0; i < preIntersectEdge.m_edge_pixels.size(); ++i)
	{
		if (preIntersectEdge.m_edge_pixels[i].y == 1)
			isbeg = true;
		if (preIntersectEdge.m_edge_pixels[i].y == ysize - 2)
			isend = true;
	}
	if (isbeg&&isend)
	{
		printf("background \n");
		return;
	}

	//获取拟合点是哪一个类
	int idx = 0;
	for (int i = 0;i< edgeinter.size();++i)
	{
		if (preIntersectEdge.m_segType == edgeinter[i].m_segType)
			idx = i;
	}

	int maxpos = edgeinter[idx].m_edge_pixels[0].x;
	int minpos = edgeinter[idx].m_edge_pixels[0].x;
	for (size_t i = 0; i < edgeinter[idx].m_edge_pixels.size(); i++)
	{
		maxpos = max(maxpos, edgeinter[idx].m_edge_pixels[0].x);
		minpos = min(minpos, edgeinter[idx].m_edge_pixels[0].x);
	}

	int interval = (maxpos - minpos) / 5;
	for (size_t i = 0; i < edgeinter[idx].m_edge_pixels.size(); i++)
	{
		//反正只去三个点 就不用循环了 麻烦
		int pos1 = minpos + interval;
		int pos2 = minpos + 2 * interval;
		int pos3 = minpos + 3 * interval;

		if (edgeinter[idx].m_edge_pixels[i].x == pos1)
			fitpoints.push_back(edgeinter[idx].m_edge_pixels[i]);
		if (edgeinter[idx].m_edge_pixels[i].x == pos2)
			fitpoints.push_back(edgeinter[idx].m_edge_pixels[i]);
		if (edgeinter[idx].m_edge_pixels[i].x == pos3)
			fitpoints.push_back(edgeinter[idx].m_edge_pixels[i]);
	}

}

//分解得到点，这个分解很容易，直接通过大小可以分解出来
void split_points(vector<CPOINT> fitpoints, vector<CPOINT> &splitPoint1, vector<CPOINT> &splitPoint2)
{
	//首先按照x坐标的大小顺序排列
	for (size_t i = 0; i < fitpoints.size(); i++)
	{
		for (size_t j = i; j < fitpoints.size(); j++)
		{
			//交换
			if (fitpoints[i].x < fitpoints[j].x)
			{
				CPOINT pnt;
				pnt.x = fitpoints[i].x;
				pnt.y = fitpoints[i].y;

				fitpoints[i].x = fitpoints[j].x;
				fitpoints[i].y = fitpoints[j].y;
			}
		}
	}

	//获取
	for (size_t i = 0; i < fitpoints.size(); i+=2)
	{
		if (fitpoints[2 * i + 0].y>fitpoints[2 * i + 1].y)
		{
			splitPoint1.push_back(fitpoints[2 * i + 0]);
			splitPoint2.push_back(fitpoints[2 * i + 1]);
		}
		else
		{
			splitPoint1.push_back(fitpoints[2 * i + 1]);
			splitPoint2.push_back(fitpoints[2 * i + 0]);
		}
	}

}

//对非均匀性进行简单的校正
void correct_non_nonhomogeneity(const char* pathImgIn, const char* pathImgOut)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_dataset = GDALOpen(pathImgIn, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int bands = GDALGetRasterCount(m_dataset);
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathImgOut, xsize, ysize, bands, GDT_Byte, NULL);

	unsigned char* pData = new unsigned char[xsize*ysize];
	double* mean_line = new double[xsize];
	double* correct_param1 = new double[xsize];
	double* correct_param2 = new double[xsize];

	for (int i = 0; i < bands; ++i)
	{
		GDALRasterIO(GDALGetRasterBand(m_dataset, i + 1), GF_Read, 0, 0, xsize, ysize, pData, xsize, ysize, GDT_Byte, 0, 0);
		double mean_data = 0;
		for (int j = 0; j < xsize*ysize; ++j)
			mean_data += double(pData[j]) / xsize / ysize;

		for (int j = 0; j < xsize; ++j)
		{
			mean_line[j] = 0;
			for (int k = 0; k < ysize; k++)
			{
				mean_line[j] += double(pData[k*xsize + j]) / ysize;
			}
		}
		for (int k = 0; k < xsize-1; k++)
		{
			correct_param1[k]=mean_line[k] /mean_line[k+1];
		}
		correct_param1[xsize - 1] = 1;
		for (int j = 0; j < xsize; ++j)
		{
			correct_param2[j] = 1;
			if (j < xsize / 2)
			{
				for (int k = j; k < xsize/2; ++k)
				{
					correct_param2[j] = correct_param2[j]/correct_param1[k];
				}
			}
			else
			{
				for (int k = xsize / 2; k < j; ++k)
				{
					correct_param2[j] = correct_param2[j] * correct_param1[k];
				}
			}
		}

		for (int j = 0; j < xsize; ++j)
		{
			for (int k = 0; k < ysize; k++)
			{
				pData[k*xsize + j] = (unsigned char)(double(pData[k*xsize + j])*correct_param2[j]);
			}
		}
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, pData, xsize, ysize, GDT_Byte, 0, 0);
	}
	GDALClose(m_dataset);
	GDALClose(m_datasetDst);
	delete[]pData;
	delete[]mean_line;
	delete[]correct_param1;
	delete[]correct_param2;
}

void set_mask_region(const char* pathImgIn, const char* pathImgOut, CPOINT leftup, CPOINT rightdown)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_dataset = GDALOpen(pathImgIn, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int bands = GDALGetRasterCount(m_dataset);
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathImgOut, xsize, ysize, bands, GDT_Int32, NULL);
	int* pData = new int[xsize*ysize];
	for (int i = 0; i < bands; ++i)
	{
		GDALRasterIO(GDALGetRasterBand(m_dataset, i + 1), GF_Read, 0, 0, xsize, ysize, pData, xsize, ysize, GDT_Int32, 0, 0);
		for (int i = leftup.x; i < rightdown.x; ++i)
		{
			for (int j = leftup.y; j < rightdown.y; j++)
			{
				pData[j*xsize + i] = -1;
			}
		}
		GDALRasterIO(GDALGetRasterBand(m_datasetDst, i + 1), GF_Write, 0, 0, xsize, ysize, pData, xsize, ysize, GDT_Int32, 0, 0);
	}
	GDALClose(m_dataset);
	GDALClose(m_datasetDst);
	delete[]pData;
}

void set_mask_region(const char* pathImg1, const char* pathMsk, const char* pathDst)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_dataset    = GDALOpen(pathImg1, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathMsk, GA_ReadOnly);
	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	int bands = GDALGetRasterCount(m_dataset);
	int* pData1   = new int[xsize*ysize];
	int* pDataMsk = new int[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_dataset, 1), GF_Read, 0, 0, xsize, ysize, pData1, xsize, ysize, GDT_Int32, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, pDataMsk, xsize, ysize, GDT_Int32, 0, 0);

	for (int i = 0; i < xsize*ysize; ++i)
	{
		if (pDataMsk[i] == -1)
			pData1[i] = -1;
	}
	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, xsize, ysize,1,GDT_Int32,NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetDst, 1), GF_Write, 0, 0, xsize, ysize, pData1, xsize, ysize, GDT_Int32, 0, 0);

	GDALClose(m_dataset);
	GDALClose(m_datasetMsk);
	GDALClose(m_datasetDst);
	delete[]pData1;
	delete[]pDataMsk;
}

//获取边界区域
void get_mask_edge_region(float* imgData, vector<CPOINT> &outline, int xsize, int ysize)
{
	for (int i = 5; i < xsize-5; ++i)
	{
		for (int j = 5; j < ysize-5; ++j)
		{
			if ((imgData[j*xsize + i] == -1) && (
				imgData[j*xsize + i + 1] != -1 ||
				imgData[j*xsize + i - 1] != -1 ||
				imgData[(j - 1)*xsize + i - 1] != -1 ||
				imgData[(j - 1)*xsize + i] != -1 ||
				imgData[(j - 1)*xsize + i + 1] != -1 ||
				imgData[(j + 1)*xsize + i - 1] != -1 ||
				imgData[(j + 1)*xsize + i] != -1 ||
				imgData[(j + 1)*xsize + i + 1] != -1))
			{
				CPOINT pnt;
				pnt.x = i; pnt.y = j;
				outline.push_back(pnt);
			}
		}
	}

}
void repair_outline(float* imgData, vector<CPOINT> &outline, int xsize, int ysize)
{
	float* tmp = new float[xsize*ysize];
	memcpy(tmp, imgData, sizeof(float)*xsize*ysize);
	for (int i = 0; i < outline.size(); ++i)
	{
		vector<CPOINT> pnt1;
		int x = outline[i].x;
		int y = outline[i].y;

		if (imgData[(y - 1)*xsize + x-1] != -1)
		{
			CPOINT tp; tp.x = x-1; tp.y = y - 1;
			pnt1.push_back(tp);
		}
		if (imgData[(y - 1)*xsize + x] != -1)
		{
			CPOINT tp; tp.x = x; tp.y = y - 1;
			pnt1.push_back(tp);
		}
		if (imgData[(y - 1)*xsize + x+1] != -1)
		{
			CPOINT tp; tp.x = x+1; tp.y = y - 1;
			pnt1.push_back(tp);
		}
		if (imgData[(y )*xsize + x-1] != -1)
		{
			CPOINT tp; tp.x = x-1; tp.y = y;
			pnt1.push_back(tp);
		}
		if (imgData[(y)*xsize + x+1] != -1)
		{
			CPOINT tp; tp.x = x+1; tp.y = y;
			pnt1.push_back(tp);
		}
		if (imgData[(y + 1)*xsize + x-1] != -1)
		{
			CPOINT tp; tp.x = x-1; tp.y = y + 1;
			pnt1.push_back(tp);
		}
		if (imgData[(y + 1)*xsize + x] != -1)
		{
			CPOINT tp; tp.x = x; tp.y = y + 1;
			pnt1.push_back(tp);
		}
		if (imgData[(y + 1)*xsize + x+1] != -1)
		{
			CPOINT tp; tp.x = x+1; tp.y = y + 1;
			pnt1.push_back(tp);
		}

		float grad = 0;
		vector<float> tmpf1;
		vector<float> tmpf2;
		float minf = 99999; int mini = -1;
		for (int j = 0; j < pnt1.size(); ++j)
		{
			int x1, y1;
			x1 = pnt1[j].x - x + pnt1[j].x;
			y1 = pnt1[j].y - y + pnt1[j].y;
			grad += abs(imgData[y1*xsize + x1] - imgData[pnt1[j].y*xsize + pnt1[j].x]);
			if (abs(imgData[y1*xsize + x1] - imgData[pnt1[j].y*xsize + pnt1[j].x])<minf)
			{
				minf = abs(imgData[y1*xsize + x1] - imgData[pnt1[j].y*xsize + pnt1[j].x]);
				mini = j;
			}
		}
		for (int j = 0; j < pnt1.size(); ++j)
		{
			int x1, y1;
			x1 = pnt1[j].x - x + pnt1[j].x;
			y1 = pnt1[j].y - y + pnt1[j].y;
			float tt = abs(imgData[y1*xsize + x1] - imgData[pnt1[j].y*xsize + pnt1[j].x]);
			if(tt==0)
				tmpf1.push_back(grad / 0.001);
			else if(j== mini)
				tmpf1.push_back(grad / tt*1.2);
			else
				tmpf1.push_back(grad / tt);
		}

		grad = 0;
		for (int j = 0; j < pnt1.size(); ++j)
			grad += tmpf1[j];
		for (int j = 0; j < pnt1.size(); ++j)
		{
			
			if (grad == 0)
				tmpf2.push_back(1.0f/ pnt1.size());
			else
				tmpf2.push_back(tmpf1[j] / grad);
		}

		float temp = 0;
		for (int j = 0; j < pnt1.size(); ++j)
		{
			int x1, y1;
			x1 = pnt1[j].x - x + pnt1[j].x;
			y1 = pnt1[j].y - y + pnt1[j].y;
			float tt = imgData[pnt1[j].y*xsize + pnt1[j].x];
			temp += tt*tmpf2[j];
		}
		tmp[y*xsize + x] = temp;
	}
	memcpy( imgData, tmp, sizeof(float)*xsize*ysize);
	delete[]tmp;
	outline.clear();
}
void repair(const char* pathImg, const char* pathDst)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_dataset = GDALOpen(pathImg, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_dataset);
	int ysize = GDALGetRasterYSize(m_dataset);
	float* imgData = NULL;
	imgData = new float[xsize*ysize];
	GDALRasterIO(GDALGetRasterBand(m_dataset, 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_dataset);
	vector<CPOINT> outline;
	get_mask_edge_region(imgData, outline, xsize, ysize);

	do
	{
		repair_outline(imgData, outline, xsize, ysize);
		get_mask_edge_region(imgData, outline, xsize, ysize);
	} while (!outline.empty());

	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetDst, 1), GF_Write, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALClose(m_datasetDst);
	delete[]imgData;
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