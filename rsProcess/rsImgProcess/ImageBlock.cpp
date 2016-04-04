//============================================================================
// Name        : ImageBlock.cpp
// Author      : ww
// Version     : 1.0
// Copyright   : ww
// Description : 影像分块读写操作
// Rely On	   :
//1.GDAL库文件 基于GDAL的影像分块读写操作
//============================================================================
//Version 1.1
/*
1.添加int型数据 char型数据的影像分块
Copyright   : ww
[8/3/2014 wuwei just you]
*/
//Version 1.1.1
/*
1.添加测试函数
Copyright   : ww
[2/7/2015 wuwei just you]
*/
//============================================================================
#include "ImageBlock.h"
//影像分块操作不能提高影像处理的效率，但是对影像进行分块可以方便的进行多线程操作
//通过多个线程对影像进行操作充分利用多核特点提高影像处理的效率

//获取影像块的数目
int  GetImageXBlockNum(GDALDatasetH dataset)
{
	int width=GDALGetRasterXSize(dataset);
	int xNum;
	if (width%512==0)
		xNum=(width>>9);
	else
		xNum=(width>>9)+1;
	return xNum;
}
int  GetImageYBlockNum(GDALDatasetH dataset)
{
	int height=GDALGetRasterYSize(dataset);
	int yNum;
	if(height%512==0)
		yNum=(height>>9);
	else
		yNum=(height>>9)+1;
	return yNum;
}

//获取影像分块
long GetImageBlock(GDALDatasetH dataset,BlockLabel* blocks,int blockXNum,int blockYNum,int buffer)
{

	if(dataset==NULL||blocks==NULL||blockXNum<1||buffer>=512)
		return -1;

	int width =GDALGetRasterXSize(dataset);
	int height=GDALGetRasterYSize(dataset);

	if (width<512)
	{
		for(int i=0;i<blockXNum*blockYNum;i++)
		{
			blocks[i].m_BlockWidth=width;
			blocks[i].m_xIndex    =0;
			blocks[i].m_edgeH[0]  =0;		//左边界
			blocks[i].m_edgeH[1]  =width;	//右边界
		}
	}
	else
	{
		for (int i=0;i<(blockXNum-1);i++)
		{
			for (int j=0;j<blockYNum;j++)
			{
				blocks[i+j*blockXNum].m_BlockWidth=512;
				blocks[i+j*blockXNum].m_xIndex    =i<<9;
			}
		}
		for (int j=0;j<blockYNum;j++)
		{
			blocks[blockXNum-1+j*blockXNum].m_BlockWidth=width-((blockXNum-1)<<9);
			blocks[blockXNum-1+j*blockXNum].m_xIndex		=(blockXNum-1)<<9;
		}

		//缓冲区边界分析
		for (int j=0;j<blockYNum;j++)
		{
			blocks[0+j*blockXNum].m_edgeH[0]=0;
			blocks[0+j*blockXNum].m_edgeH[1]=blocks[0+j*blockXNum].m_BlockWidth+buffer;
		}
		for (int i=1;i<blockXNum-1;i++)
		{
			for (int j=0;j<blockYNum;j++)
			{
				blocks[i+j*blockXNum].m_edgeH[0]=(i<<9)-buffer;
				blocks[i+j*blockXNum].m_edgeH[1]=(i<<9)+buffer+blocks[i+j*blockXNum].m_BlockWidth;
			}
		}
		for (int j=0;j<blockYNum;j++)
		{
			blocks[blockXNum-1+j*blockXNum].m_edgeH[0]=((blockXNum-1)<<9)-buffer;
			blocks[blockXNum-1+j*blockXNum].m_edgeH[1]=width;
		}
	}
	if (height<512)
	{
		for(int i=0;i<blockXNum*blockYNum;i++)
		{
			blocks[i].m_BlockHeight=height;
			blocks[i].m_yIndex     =0;
			blocks[i].m_edgeV[0]   =0;
			blocks[i].m_edgeV[1]   =height;
		}
	}
	else
	{
		for (int i=0;i<blockXNum;i++)
		{
			for (int j=0;j<blockYNum-1;j++)
			{
				blocks[i+j*blockXNum].m_BlockHeight=512;
				blocks[i+j*blockXNum].m_yIndex    =j<<9;
			}
		}
		for (int i=0;i<blockXNum;i++)
		{
			blocks[i+(blockYNum-1)*blockXNum].m_BlockHeight=height-((blockYNum-1)<<9);
			blocks[i+(blockYNum-1)*blockXNum].m_yIndex     =(blockYNum-1)<<9;
		}

		//缓冲区边界分析
		for (int i=0;i<blockXNum;i++)
		{
			blocks[i].m_edgeV[0]=0;
			blocks[i].m_edgeV[1]=blocks[i].m_BlockHeight+buffer;
		}
		for (int j=1;j<blockYNum-1;j++)
		{
			for (int i=0;i<blockXNum;i++)
			{
				blocks[i+j*blockXNum].m_edgeV[0]=(j<<9)-buffer;
				blocks[i+j*blockXNum].m_edgeV[1]=(j<<9)+buffer+blocks[i+j*blockXNum].m_BlockHeight;
			}
		}

		for (int i=0;i<blockXNum;i++)
		{
			blocks[i+(blockYNum-1)*blockXNum].m_edgeV[0]=((blockYNum-1)<<9)-buffer;
			blocks[i+(blockYNum-1)*blockXNum].m_edgeV[1]=height;
		}
	}
	return 0;

}


//读取分块影像
long ReadBlockImageData(GDALDatasetH dataset, BlockLabel block,float* imgBuffer,int band/*=1*/)
{
	//检查输入
	if(dataset==NULL||imgBuffer==NULL)
		return -1;
	if (block.m_BlockWidth==0||block.m_BlockHeight==0)
		return -2;

	int readWidth =block.m_edgeH[1]-block.m_edgeH[0];
	int readHeight=block.m_edgeV[1]-block.m_edgeV[0];

	//获取影像块数据
	GDALRasterIO(GDALGetRasterBand(dataset,band),GF_Read,block.m_edgeH[0],block.m_edgeV[0],readWidth,readHeight,imgBuffer,readWidth,readHeight,GDT_Float32,0,0);

	//获取数据成功返回
	return 0;
}
//写分块影像
long WriteBlockImageData(GDALDatasetH dataset,BlockLabel block,float* imgBuffer,int band/* =1 */)
{
	//检查输入
	if(dataset==NULL||imgBuffer==NULL)
		return -1;

	//内存空间的申请
	float* img=NULL;
	img=new float[block.m_BlockWidth*block.m_BlockHeight];
	if(img==NULL)
		return -2;

	//获取数据集
	int blockWidth=block.m_edgeH[1]-block.m_edgeH[0];
	for (int i=block.m_yIndex-block.m_edgeV[0],j=0;j<block.m_BlockHeight;j++,i++)
		memcpy(img+j*block.m_BlockWidth,imgBuffer+i*blockWidth+block.m_xIndex-block.m_edgeH[0],sizeof(float)*block.m_BlockWidth);

	//写入影像数据集中
	GDALRasterIO(GDALGetRasterBand(dataset,band),GF_Write,block.m_xIndex,block.m_yIndex,block.m_BlockWidth,block.m_BlockHeight,
														img,block.m_BlockWidth,block.m_BlockHeight,GDT_Float32,0,0);
	delete[]img;img=NULL;
	return 0;
}

//读取分块影像
long ReadBlockImageData(GDALDatasetH dataset, BlockLabel block,int* imgBuffer,int band/*=1*/)
{
	//检查输入
	if(dataset==NULL||imgBuffer==NULL)
		return -1;
	if (block.m_BlockWidth==0||block.m_BlockHeight==0)
		return -2;

	int readWidth =block.m_edgeH[1]-block.m_edgeH[0];
	int readHeight=block.m_edgeV[1]-block.m_edgeV[0];

	//获取影像块数据
	GDALRasterIO(GDALGetRasterBand(dataset,band),GF_Read,block.m_edgeH[0],block.m_edgeV[0],readWidth,readHeight,imgBuffer,readWidth,readHeight,GDT_Int32,0,0);

	//获取数据成功返回
	return 0;
}
//写分块影像
long WriteBlockImageData(GDALDatasetH dataset,BlockLabel block,int* imgBuffer,int band/* =1 */)
{
	//检查输入
	if(dataset==NULL||imgBuffer==NULL)
		return -1;

	//内存空间的申请
	int* img=NULL;
	img=new int[block.m_BlockWidth*block.m_BlockHeight];
	if(img==NULL)
		return -2;

	//获取数据集
	int blockWidth=block.m_edgeH[1]-block.m_edgeH[0];
	for (int i=block.m_yIndex-block.m_edgeV[0],j=0;j<block.m_BlockHeight;j++,i++)
		memcpy(img+j*block.m_BlockWidth,imgBuffer+i*blockWidth+block.m_xIndex-block.m_edgeH[0],sizeof(int)*block.m_BlockWidth);

	//写入影像数据集中
	GDALRasterIO(GDALGetRasterBand(dataset,band),GF_Write,block.m_xIndex,block.m_yIndex,block.m_BlockWidth,block.m_BlockHeight,
		img,block.m_BlockWidth,block.m_BlockHeight,GDT_Int32,0,0);
	delete[]img;img=NULL;
	return 0;
}

//读取分块影像
long ReadBlockImageData(GDALDatasetH dataset, BlockLabel block,unsigned char* imgBuffer,int band/*=1*/)
{
	//检查输入
	if(dataset==NULL||imgBuffer==NULL)
		return -1;
	if (block.m_BlockWidth==0||block.m_BlockHeight==0)
		return -2;

	int readWidth =block.m_edgeH[1]-block.m_edgeH[0];
	int readHeight=block.m_edgeV[1]-block.m_edgeV[0];

	//获取影像块数据
	GDALRasterIO(GDALGetRasterBand(dataset,band),GF_Read,block.m_edgeH[0],block.m_edgeV[0],readWidth,readHeight,imgBuffer,readWidth,readHeight,GDT_Byte,0,0);

	//获取数据成功返回
	return 0;
}
//写分块影像
long WriteBlockImageData(GDALDatasetH dataset,BlockLabel block,unsigned char* imgBuffer,int band/* =1 */)
{
	//检查输入
	if(dataset==NULL||imgBuffer==NULL)
		return -1;

	//内存空间的申请
	unsigned char* img=NULL;
	img=new unsigned char[block.m_BlockWidth*block.m_BlockHeight];
	if(img==NULL)
		return -2;

	//获取数据集
	int blockWidth=block.m_edgeH[1]-block.m_edgeH[0];
	for (int i=block.m_yIndex-block.m_edgeV[0],j=0;j<block.m_BlockHeight;j++,i++)
		memcpy(img+j*block.m_BlockWidth,imgBuffer+i*blockWidth+block.m_xIndex-block.m_edgeH[0],sizeof(unsigned char)*block.m_BlockWidth);

	//写入影像数据集中
	GDALRasterIO(GDALGetRasterBand(dataset,band),GF_Write,block.m_xIndex,block.m_yIndex,block.m_BlockWidth,block.m_BlockHeight,
		img,block.m_BlockWidth,block.m_BlockHeight,GDT_Byte,0,0);
	delete[]img;img=NULL;
	return 0;
}
