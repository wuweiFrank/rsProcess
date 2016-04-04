#pragma once

#ifndef _SPFLITER_
#define _SPFLITER_

//GDAL封装
#include <vector>
#include <algorithm>

#include"..\Global.h"
#include "..\gdal/include/gdal_priv.h"
#include "..\ThreadPool.h"
#include "ImageBlock.h"

using namespace std;
#pragma comment(lib,"gdal_i.lib")

//#define PI 3.1415926534
//影像空域滤波
class  spFliter
{
public:

	//构造函数和析构函数初始化
	spFliter(){
		m_is_core=false;
		m_fliter_core=NULL;
	}
   ~spFliter(){
		if (m_is_core&&m_fliter_core!=NULL)
		{
			for (int i=0;i<m_core_ysize;i++)
			{
				delete[]m_fliter_core[i];
			}
			delete[]m_fliter_core;
		}
	}

	//设置滤波核
	void spFliterCore(float** core,int xsize,int ysize);
	void spFliterCore(float*  core,int xsize,int ysize);

	//进行滤波操作
	//GDAL封装
	void spitalFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset);
	void spitalFliter(char* pathSrc,char* pathDst);

	//拉普拉斯算子进行滤波
	void LaplaceFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset);
	void LaplaceFliter(char* pathSrc,char* pathDst);

	//sobel算子进行滤波
	void SobelFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset);
	void SobelFliter(char* pathSrc,char* pathDst);

	//均值滤波
	void MeanFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset);
	void MeanFliter(char* pathSrc,char* pathDst);

	//0值滤波
	void ZeroFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset);
	void ZeroFliter(char* pathSrc,char* pathDst);


	//中值滤波
	void MedFliter(float*imgBufferIn,int width,int heigh,int xpos,int ypos,
							int xoffset,int yoffset,int xfsize,int yfsize,float* imgBufferOut);
	void MedFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset);
	void MedFliter(char* pathSrc,char* pathDst);


	//滤波操作核
	void spFliterOperation(float*imgBufferIn,int width,int heigh,int xpos,int ypos,int xoffset,int yoffset,float* imgBufferOut);
private:

	//滤波操作核
	float __fastcall spFliterOperationCore(float*imgBufferIn,int xpos,int ypos);

	//成员变量
	bool m_is_core;				//滤波核是否存在
	int m_core_xsize;			//滤波核x尺寸大小
	int m_core_ysize;			//滤波核y尺寸大小
	int m_width;				//影像的width
	int m_heigh;				//影像的heigh
	float **m_fliter_core;		//影像滤波核
};


//设置高斯滤波核函数
long  SetGaussCore(float **kernel,float sigma, int xsize=3,int ysize=3);
long  SetGaussCore(float *kernel, float sigma, int xsize=3,int ysize=3);


//影像进行高斯滤波
long  GaussFliter(float *srcImg,float *dstImg,int xImg,int yImg,float ** kernel,int xkernel=3,int ykernel=3);

//GDAL封装影像高斯滤波
long  GaussFliter(char* pathSrc,char* pathDst,float** kernel, int xkernel=3, int ykernel=3);


//进行高斯差分
long  GaussDifference(float* srcImg1,float* srcImg2,float* dstImg,int xImg,int yImg);

//GDAL封装的高斯差分
long  GaussDifference(char* pathsrc1,char* pathsrc2,char* pathdst);


//  [2/6/2015 wuwei just you]
//TODO：影像分块进行滤波，滤波后影像分块写入文件中
//		增加多线程处理，提高运行效率
//影像分块滤波
long  ImageSpaitalBlockFliter(char* pathsrc,char* pathdst,float* fliterCore,int xcoresize,int ycoresize);

//多线程影像分块滤波
long  ThreadImageSpaitalFliter(char* pathsrc,char* pathdst,float* fliterCore,int xcoresize,int ycoresize);


//  [4/11/2015 wuwei just you]
//影像形态学滤波，形态学滤波本质上就是最大值或最小值滤波
//设置合适的滤波窗口和滤波核中心点位置
class  MorphologyFliter
{
	//构造函数和析构函数初始化
	MorphologyFliter(){
		m_is_core=false;
		m_fliter_core=NULL;
	}
	~MorphologyFliter(){
		if (m_is_core&&m_fliter_core!=NULL)
		{
			for (int i=0;i<m_core_ysize;i++)
			{
				delete[]m_fliter_core[i];
			}
			delete[]m_fliter_core;
		}
	}

	//设置形态学滤波核
	void MorphologyCore(float** core,int xsize,int ysize,int centerx,int centery);
	void MorphologyCore(float*  core,int xsize,int ysize,int centerx,int centery);

	//形态学腐蚀
	void MorphologyCorrode(float* dataIn,int sizex,int sizey,int bands,float* dataOut);

	//形态学膨胀
	void MorphologyDilation(float* dataIn,int sizex,int sizey,int bands,float* dataOut);

	//GDAL封装的形态学腐蚀与膨胀
	//形态学腐蚀
	void MorphologyCorrode(char* pathIn,char* pathOut);

	//形态学膨胀
	void MorphologyDilation(char* pathIn,char* pathOut);

private:
	//成员变量
	bool m_is_core;				//滤波核是否存在
	int m_core_xsize;			//滤波核x尺寸大小
	int m_core_ysize;			//滤波核y尺寸大小
	int m_width;				//影像的width
	int m_heigh;				//影像的heigh
	int m_centerx;				//滤波核中心x位置
	int m_centery;				//滤波核中心y位置
	float **m_fliter_core;		//影像滤波核
};

#endif
//  [4/9/2014 wuwei just you]
//	version 1.0影像的空间域滤波和高斯滤波



