#pragma once
/*
	影像重采样函数，函数可以对影像进行resize操作
	影像重采样包括最近邻插值，双线性插值，双三次卷积插值
	采用多线程处理，以期提高处理效率，在处理过程中线程切换比较啊耗时
	处理小影像时不适合使用多线程处理，在处理较大影像过程中使用多线程
	可以提高处理效率
*/
#ifndef _IMAGE_SAMPLE_
#define _IMAGE_SAMPLE_


#include "..\gdal\include\ogrsf_frmts.h"
#include "..\gdal\include\gdal_priv.h"
#include "..\ThreadPool.h"

#pragma comment(lib,"gdal_i.lib")

// 全局：线程锁
static CRITICAL_SECTION cs_read_sample;	//读锁
static CRITICAL_SECTION cs_write_sample;//写锁

//多线程结构体
struct  SAMPLE_STUECT
{
	GDALDatasetH m_dataset_In;
	GDALDatasetH m_dataset_Out;
	int bandIndex;
};


/************************************************************************/
/*								线程函数    			                */
/************************************************************************/

//多线程运算
//对每个波段都加入线程池进行处理 多线程 加速运算
DWORD  WINAPI NearestThreadFunc(LPVOID vsampleSt);
DWORD  WINAPI BilinerThreadFunc(LPVOID vsampleSt);
DWORD  WINAPI CubicThreadFunc(LPVOID vsampleSt);

/************************************************************************/
/*								采样函数    			                */
/************************************************************************/
class  CSampleFunc
{
public:
	CSampleFunc(void);
	~CSampleFunc(void);

	//多线程重采样
	/*
		参数：char* pathIn		：输入影像路径
			  int xsampleszie	：采样后影像大小x
			  int ysamplesize	：采样后影像大小y
			  char* pathOut		：输出结果影像路径

	*/
	long nearest_sample_thread(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut);
	long biliner_sample_thread(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut);
	long cubic_sample_thread(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut);

	//直接根据影像路径进行重采样 单线程操作
	/*
		参数：char* pathIn		：输入影像路径
			  int xsampleszie	：采样后影像大小x
			  int ysamplesize	：采样后影像大小y
			  char* pathOut		：输出结果影像路径

	*/
	long nearest_sample(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut);
	long biliner_sample(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut);
	long cubic_sample(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut);
	long gauss_down_sample(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut);
	
	//对影像数据进行重采样
	/*
		参数：int in_xsize			输入影像x的大小
			  int in_ysize			输出影像y的大小
			  float* imgBufferIn	输入影像数据单波段
			  int sample_xsize		采样后影像的大小x
			  int sample_ysize		采样后影像的大小y
			  float* imgBufferOut	采样后影像数据输出
	*/
	long gauss_down_sample(int in_xsize,int in_ysize,float* imgBufferIn,int sample_xsize,int sample_ysize,float* imgBufferOut);
	long nearest_sample(int in_xsize,int in_ysize,float* imgBufferIn,int out_xsize,int out_ysize,float* imgBufferOut);
	long biliner_sample(int in_xsize,int in_ysize,float* imgBufferIn,int out_xsize,int out_ysize,float* imgBufferOut);
	long cubic_sample(int in_xsize,int in_ysize,float* imgBufferIn,int out_xsize,int out_ysize,float* imgBufferOut);


	//重采样函数 根据对应点的对应坐标进行重采样
	/*
		参数：float* imgBuffer		：输入影像路径
			  int xsize				：x方向输入影像大小
			  int ysize				：y方向输入影像大小
			  float* xpos			：对应采样后的位置x
			  float* ypos			：对应采样后的位置y
			  int out_xsize			：输出影像x大小
			  int out_ysize			：输出影像y大小
			  float* imgBufferOut	：输出影像数据
	*/
	long nearest_sample(float* imgBuffer,int xsize,int ysize,float* xpos,float* ypos,int out_xsize,int out_ysize,float* imgBufferOut);
	long biliner_sample(float* imgBuffer,int xsize,int ysize,float* xpos,float* ypos,int out_xsize,int out_ysize,float* imgBufferOut);
	long cubic_sample(float* imgBuffer,int xsize,int ysize,float* xpos,float* ypos,int out_xsize,int out_ysize,float* imgBufferOut);

	//重采样函数的原子操作
	/*
		参数：float *imgBuffer	：输入影像数据
			  int xsize			：输入影像x的大小
			  int ysize			：输入影像y的大小
			  float xpos		：输入采样点的位置x
			  float ypos		：输入采样点的位置y
	*/
	float _stdcall nearest_sample(float *imgBuffer,int xsize,int ysize,float xpos,float ypos);
	float _stdcall biliner_sample(float *imgBuffer,int xsize,int ysize,float xpos,float ypos);
	float _stdcall cubic_sample(float *imgBuffer,int xsize,int ysize,float xpos,float ypos);

private:

	//拟合采样函数
	float __fastcall s_function(float x);

	CThreadPool m_threadpool;
};

#endif

//  [2/6/2015 wuwei just you]
//TODO：增加三维离散点的插值算法