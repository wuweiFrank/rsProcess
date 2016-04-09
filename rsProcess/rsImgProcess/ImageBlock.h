#pragma once
#pragma comment(lib,"gdal_i.lib")
#include "..\gdal\include\gdal_priv.h"

//分块大小为512*512
//设置分块大小为2的倍数主要目的在于通过移位运算加快分块速度
//影像分块并不通过读入影像进行分块，而是通过分块标签获取每一块在影像中的位置，以及每一块缓冲区的位置

//影像分块标签
struct  BlockLabel 
{
	//每一块左上角点影像的坐标
	int m_xIndex;
	int m_yIndex;
	
	//影像快的宽度和高度
	int m_BlockWidth;
	int m_BlockHeight;

	//影像块边界位置
	int m_edgeV[2];		//上下边界
	int m_edgeH[2];		//左右边界

	void operator =(BlockLabel block)
	{
		m_xIndex=block.m_xIndex;
		m_yIndex=block.m_yIndex;
		m_BlockHeight=block.m_BlockHeight;
		m_BlockWidth=block.m_BlockWidth;
		m_edgeH[0]=block.m_edgeH[0];m_edgeH[1]=block.m_edgeH[1];
		m_edgeV[0]=block.m_edgeV[0];m_edgeV[1]=block.m_edgeV[1];
	}
};

//影像分块数目
int  GetImageXBlockNum(GDALDatasetH dataset);	//获取影像x轴方向分块数目

int  GetImageYBlockNum(GDALDatasetH dataset);	//获取影像y轴方向分块数目

/*
	获取影像分块信息
	参数：
	dataset				：影像数据集
	blocks				：分块后获取的影像块
	blockXNum blockYNum	：影像块的数目
	buffer				：缓冲区的大小（不能超过影像块的大小）
*/
long  GetImageBlock(GDALDatasetH dataset,BlockLabel* blocks,int blockXNum,int blockYNum,int buffer);

/*
	读写分块影像数据
	参数：
	dataset				：影像数据集
	block				：待读取数据的影像块
	imgBuffer			：读取的影像数据
	band				：读取数据的波段
*/
//float型数据影像
long  ReadBlockImageData(GDALDatasetH dataset, BlockLabel block,float* imgBuffer,int band=1); 
long  WriteBlockImageData(GDALDatasetH dataset,BlockLabel block,float* imgBuffer,int band=1);

//int型数据影像
long  ReadBlockImageData(GDALDatasetH dataset, BlockLabel block,int* imgBuffer,int band=1);
long  WriteBlockImageData(GDALDatasetH dataset,BlockLabel block,int* imgBuffer,int band=1);

//BYTE型数据影像
long  ReadBlockImageData(GDALDatasetH dataset, BlockLabel block,unsigned char* imgBuffer,int band=1);
long  WriteBlockImageData(GDALDatasetH dataset,BlockLabel block,unsigned char* imgBuffer,int band=1);