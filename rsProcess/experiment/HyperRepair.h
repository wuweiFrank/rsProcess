#pragma once
#include"..\Global.h"
#include"..\gdal\include\gdal_priv.h"
#include<vector>
using namespace std;

#pragma comment(lib,"gdal_i.lib");
/*
	这个部分是实验性代码，代码并没有经过优化，代码结构也可能不合理，为了实现某个功能随意添加的代码
*/

//获取分割影像各个块，并得到各个块和数据缺失边
//每个分割的边缘像素
struct Edge_Pixels
{
	int m_segType;
	vector<CPOINT> m_edge_pixels;
};

//获取影像边缘交点信息
void get_segment_edge(const char* img_path, vector<Edge_Pixels> &edgeinter);

//获取影像分割与数据缺失块交点信息
void get_segment_intersection(const char* img_path, vector<Edge_Pixels> &intersecEdge,int begLine,int endLine);

//边缘像素采样拟合（这里拟合方法的选取是个大麻烦）
void get_fit_points(vector<Edge_Pixels> edgeinter , Edge_Pixels preIntersectEdge,vector<CPOINT> &fitpoints,int xsize,int ysize);

//将控制点进行分解得到
void split_points(vector<CPOINT> fitpoints, vector<CPOINT> &splitPoint1, vector<CPOINT> &splitPoint2);

//找到拟合后的具体对象在在对应边的位置，或多个像素拟合的交点
void get_fit_interesction(vector<vector<CPOINT>> &fitpoints, vector<CPOINT> &fitIntersect,vector<CPOINT> &edgeEntersect, int begLine, int endLine);

//对于交点进行处理和整理，得到较好的结果
void process_edge_fit_intersection(vector<CPOINT> &fitIntersect, vector<CPOINT> &edgeEntersect, vector<Edge_Pixels> &intersecEdge, int begLine, int endLine);

//首先对非均匀现象要进行简单的校正
void correct_non_nonhomogeneity(const char* pathImgIn, const char* pathImgOut);

//设置掩膜区域
void set_mask_region(const char* pathImgIn, const char* pathImgOut, CPOINT leftup, CPOINT rightdown);

//实验处理函数
void experiment_process();