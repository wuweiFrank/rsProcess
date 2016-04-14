#pragma once
#include"..\Global.h"
#include"..\gdal\include\gdal_priv.h"
#include<vector>
using namespace std;

#pragma comment(lib,"gdal_i.lib");


//获取分割影像各个块，并得到各个块和数据缺失边
//每个分割的边缘像素
struct Edge_Pixels
{
	int m_segType;
	vector<CPOINT> m_edge_pixels;
};
//每个分割与数据缺失点交点数据
struct Edge_Intersection
{
	int m_segType;
	vector<CPOINT> m_edge_intersction;
};

//获取影像边缘交点信息
void get_segment_edge(const char* img_path, vector<Edge_Pixels> &edgeinter);

//获取影像分割与数据缺失块交点信息
void get_segment_intersection(const char* img_path, vector<Edge_Pixels> &edgeinter,int begLine,int endLine);