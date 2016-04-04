#include "SampleFunc.h"
#include <cmath>
#include <memory>
#include <io.h>
#include <exception> 

#include "spFliter.h"
#include "..\matrixOperation.h"

using namespace std;

//多线程处理每个波段
CSampleFunc::CSampleFunc(void)
{
	m_threadpool.Create();
	//电脑核数为两个 尽可能提高效率减小线程切换的开销
	m_threadpool.SetPoolSize(4);
}

CSampleFunc::~CSampleFunc(void)
{
	m_threadpool.Destroy();
}

/*----------------------三次卷积采样函数------------------*/
float __fastcall CSampleFunc::s_function(float x)
{
	if(fabs(x)<=1)
		return 1-2*fabs(x)*fabs(x)+fabs(x)*fabs(x)*fabs(x);
	else if(fabs(x)>1&&fabs(x)<=2)
		return 4-8*fabs(x)+5*fabs(x)*fabs(x)-fabs(x)*fabs(x)*fabs(x);
	else
		return 0.0f;
}

/*------------------------线程函数------------------------*/
DWORD WINAPI NearestThreadFunc(LPVOID vsampleSt)
{
	UserPoolData* poolData = (UserPoolData*)vsampleSt;
	SAMPLE_STUECT* sampleSt=(SAMPLE_STUECT*)poolData->pData;

	CSampleFunc sample;

	GDALDatasetH datasatin =sampleSt->m_dataset_In;
	GDALDatasetH datasatOut=sampleSt->m_dataset_Out;
	int bandindex=sampleSt->bandIndex;



	int xsize=GDALGetRasterXSize(datasatin);
	int ysize=GDALGetRasterYSize(datasatin);
	int xsample=GDALGetRasterXSize(datasatOut);
	int ysample=GDALGetRasterYSize(datasatOut);

	float *datain =new float[xsize*ysize];
	float *dataout=new float[xsample*ysample];

	//EnterCriticalSection(&cs_read_sample);
	GDALRasterIO(GDALGetRasterBand(datasatin,bandindex),GF_Read,0,0,xsize,ysize,datain,xsize,ysize,GDT_Float32,0,0);
	//LeaveCriticalSection(&cs_read_sample);

	sample.nearest_sample(xsize,ysize,datain,xsample,ysample,dataout);

	//EnterCriticalSection(&cs_write_sample);
	GDALRasterIO(GDALGetRasterBand(datasatOut,bandindex),GF_Write,0,0,xsample,ysample,dataout,xsample,ysample,GDT_Float32,0,0);
	//EnterCriticalSection(&cs_write_sample);

	if(datain!=NULL)
		delete[]datain;datain=NULL;
	if (dataout!=NULL)
		delete[]dataout;dataout=NULL;

	return 0;
}
DWORD WINAPI BilinerThreadFunc(LPVOID vsampleSt)
{
	UserPoolData* poolData = (UserPoolData*)vsampleSt;
	SAMPLE_STUECT* sampleSt=(SAMPLE_STUECT*)poolData->pData;
	CSampleFunc sample;

	GDALDatasetH datasatin =sampleSt->m_dataset_In;
	GDALDatasetH datasatOut=sampleSt->m_dataset_Out;
	int bandindex=sampleSt->bandIndex;

	int xsize=GDALGetRasterXSize(datasatin);
	int ysize=GDALGetRasterYSize(datasatin);
	int xsample=GDALGetRasterXSize(datasatOut);
	int ysample=GDALGetRasterYSize(datasatOut);

	float *datain =new float[xsize*ysize];
	float *dataout=new float[xsample*ysample];
	GDALRasterIO(GDALGetRasterBand(datasatin,bandindex),GF_Read,0,0,xsize,ysize,datain,xsize,ysize,GDT_Float32,0,0);
	sample.biliner_sample(xsize,ysize,datain,xsample,ysample,dataout);
	GDALRasterIO(GDALGetRasterBand(datasatOut,bandindex),GF_Write,0,0,xsample,ysample,dataout,xsample,ysample,GDT_Float32,0,0);

	delete[]datain;datain=NULL;
	delete[]dataout;dataout=NULL;

	return 0;
}
DWORD WINAPI CubicThreadFunc(LPVOID vsampleSt)
{
	UserPoolData* poolData = (UserPoolData*)vsampleSt;
	SAMPLE_STUECT* sampleSt=(SAMPLE_STUECT*)poolData->pData;
	CSampleFunc sample;

	GDALDatasetH datasatin =sampleSt->m_dataset_In;
	GDALDatasetH datasatOut=sampleSt->m_dataset_Out;
	int bandindex=sampleSt->bandIndex;

	int xsize=GDALGetRasterXSize(datasatin);
	int ysize=GDALGetRasterYSize(datasatin);
	int xsample=GDALGetRasterXSize(datasatOut);
	int ysample=GDALGetRasterYSize(datasatOut);

	float *datain =new float[xsize*ysize];
	float *dataout=new float[xsample*ysample];
	GDALRasterIO(GDALGetRasterBand(datasatin,bandindex),GF_Read,0,0,xsize,ysize,datain,xsize,ysize,GDT_Float32,0,0);
	sample.cubic_sample(xsize,ysize,datain,xsample,ysample,dataout);
	GDALRasterIO(GDALGetRasterBand(datasatOut,bandindex),GF_Write,0,0,xsample,ysample,dataout,xsample,ysample,GDT_Float32,0,0);

	delete[]datain;datain=NULL;
	delete[]dataout;dataout=NULL;

	return 0;
}

/*-------------------------------采样原子操作----------------------------------*/
float _stdcall CSampleFunc::nearest_sample(float *imgBuffer,int xsize,int ysize,float xpos,float ypos)
{
	//检查输入
	if (imgBuffer==NULL)
		return -1000;

	//获取四个角点的位置
	int xupInt,xdownInt;
	int yupInt,ydownInt;
	int x;int y;		//实际插值的位置
	xupInt=ceilf(xpos);xdownInt=floorf(xpos);
	yupInt=ceilf(ypos);ydownInt=floorf(ypos);

	if(float(xupInt)-xpos<xpos-float(xdownInt))
		x=xupInt;
	else
		x=xdownInt;
	if(float(yupInt)-ypos<ypos-float(ydownInt))
		y=yupInt;
	else
		y=ydownInt;

	if(x>=xsize||x<0||y>=ysize||y<0)
		return 0.0f;
	else
		return imgBuffer[y*xsize+x];
}

float _stdcall CSampleFunc::biliner_sample(float *imgBuffer,int xsize,int ysize,float xpos,float ypos)
{
	//检查输入
	if (imgBuffer==NULL)
		return -1000;

	//获取四个角点的位置
	int xupInt,xdownInt;
	int yupInt,ydownInt;

	//...
	xpos+=0.0001;
	ypos+=0.0001;

	xupInt=ceilf(xpos);xdownInt=floorf(xpos);
	yupInt=ceilf(ypos);ydownInt=floorf(ypos);

	double data1[2],data2[4],data3[2];
	double mutidata12[2],sampledata=0;

	data1[0]=float(xupInt)-xpos;
	data1[1]=xpos-float(xdownInt);

	data3[0]=float(yupInt)-ypos;
	data3[1]=ypos-float(ydownInt);

// 	if (data1[0]==0)
// 		data1[0]=0.0001f;
// 	else if (data1[1]==0)
// 		data1[1]=0.0001f;
// 	if (data3[0]==0)
// 		data3[0]=0.001;
// 	if (data3[1]==0)
// 		data3[1]=0.0001;

	//四个角点数据
	if(xdownInt<0||xupInt>=xsize||ydownInt<0||yupInt>=ysize)
		memset(data2,0,sizeof(float)*4);
	else
	{
		data2[0]=imgBuffer[ydownInt*xsize+xdownInt];	//左上
		data2[1]=imgBuffer[ydownInt*xsize+xupInt];		//右上
		data2[2]=imgBuffer[yupInt*xsize+xdownInt];		//左下	
		data2[3]=imgBuffer[yupInt*xsize+xupInt];		//右下
	}

	MatrixMuti(data1,1,2,2,data2,mutidata12);
	MatrixMuti(mutidata12,1,2,1,data3,&sampledata);
	if (fabs(sampledata)<=data2[0]/1000)
		return 0.0f;
	else
		return sampledata;
}

float _stdcall CSampleFunc::cubic_sample(float *imgBuffer,int xsize,int ysize,float xpos,float ypos)
{
	//检查输入
	if (imgBuffer==NULL)
		return -1000;

	//获取四个角点的位置
	int xupInt,xdownInt;float xfloat;
	int yupInt,ydownInt;float yfloat;

	xpos+=0.0001;
	ypos+=0.0001;

	xupInt=ceilf(xpos);xdownInt=floorf(xpos);
	yupInt=ceilf(ypos);ydownInt=floorf(ypos);
	xfloat=xpos-float(xdownInt);
	yfloat=ypos-float(ydownInt);

	//数据
	double data1[4],data2[16],data3[4];
	double muti12[4],sampledata;

	//采样比例
	data1[0]=s_function(xfloat+1);
	data1[1]=s_function(xfloat+0);
	data1[2]=s_function(1-xfloat);
	data1[3]=s_function(2-xfloat);

	data3[0]=s_function(yfloat+1);
	data3[1]=s_function(yfloat+0);
	data3[2]=s_function(1-yfloat);
	data3[3]=s_function(2-yfloat);

	//数据段
	/*
	　  ┏ f(xdownInt-1, ydownInt-1)　f(xdownInt-1, ydownInt+0)　f(xdownInt-1, ydownInt+1)　f(xdownInt-1, ydownInt+2) ┓
    [B]=┃ f(xdownInt+0, ydownInt-1)　f(xdownInt+0, ydownInt+0)　f(xdownInt+0, ydownInt+1)　f(xdownInt+0, ydownInt+2) ┃
		┃ f(xdownInt+1, ydownInt-1)　f(xdownInt+1, ydownInt+0)　f(xdownInt+1, ydownInt+1)　f(xdownInt+1, ydownInt+2) ┃
		┗ f(xdownInt+2, ydownInt-1)　f(xdownInt+2, ydownInt+0)　f(xdownInt+2, ydownInt+1)　f(xdownInt+2, ydownInt+2) ┛
	*/

	//第一行
	if(xdownInt>0||ydownInt>=0||xdownInt<xsize-1||ydownInt-1>=ysize)
		data2[0]=0.0f;
	else 
		data2[0]=imgBuffer[(ydownInt-1)*xsize+xdownInt-1];

	if(xdownInt-1<0||ydownInt+0<0||xdownInt-1>=xsize||ydownInt+0>=ysize)
		data2[1]=0.0f;
	else 
		data2[1]=imgBuffer[(ydownInt+0)*xsize+xdownInt-1];

	if(xdownInt-1<0||ydownInt+1<0||xdownInt-1>=xsize||ydownInt+1>=ysize)
		data2[2]=0.0f;
	else 
		data2[2]=imgBuffer[(ydownInt+1)*xsize+xdownInt-1];

	if(xdownInt-1<0||ydownInt+2<0||xdownInt-1>=xsize||ydownInt+2>=ysize)
		data2[3]=0.0f;
	else 
		data2[3]=imgBuffer[(ydownInt+2)*xsize+xdownInt-1];

	//第二行
	if(xdownInt+0<0||ydownInt-1<0||xdownInt+0>=xsize||ydownInt-1>=ysize)
		data2[4]=0.0f;
	else 
		data2[4]=imgBuffer[(ydownInt-1)*xsize+xdownInt+0];

	if(xdownInt+0<0||ydownInt+0<0||xdownInt+0>=xsize||ydownInt+0>=ysize)
		data2[5]=0.0f;
	else 
		data2[5]=imgBuffer[(ydownInt+0)*xsize+xdownInt+0];

	if(xdownInt+0<0||ydownInt+1<0||xdownInt+0>=xsize||ydownInt+1>=ysize)
		data2[6]=0.0f;
	else 
		data2[6]=imgBuffer[(ydownInt+1)*xsize+xdownInt+0];

	if(xdownInt+01<0||ydownInt+2<0||xdownInt+0>=xsize||ydownInt+2>=ysize)
		data2[7]=0.0f;
	else 
		data2[7]=imgBuffer[(ydownInt+2)*xsize+xdownInt+0];

	// 第三行
	if(xdownInt+1<0||ydownInt-1<0||xdownInt+1>=xsize||ydownInt-1>=ysize)
		data2[8]=0.0f;
	else 
		data2[8]=imgBuffer[(ydownInt-1)*xsize+xdownInt+1];
	if(xdownInt+1<0||ydownInt+0<0||xdownInt+1>=xsize||ydownInt+0>=ysize)
		data2[9]=0.0f;
	else 
		data2[9]=imgBuffer[(ydownInt+0)*xsize+xdownInt+1];
	if(xdownInt+1<0||ydownInt+1<0||xdownInt+1>=xsize||ydownInt+1>=ysize)
		data2[10]=0.0f;
	else 
		data2[10]=imgBuffer[(ydownInt+1)*xsize+xdownInt+1];
	if(xdownInt+1<0||ydownInt+2<0||xdownInt+1>=xsize||ydownInt+2>=ysize)
		data2[11]=0.0f;
	else 
		data2[11]=imgBuffer[(ydownInt+2)*xsize+xdownInt+1];

	//第四行
	if(xdownInt+2<0||ydownInt-1<0||xdownInt+2>=xsize||ydownInt-1>=ysize)
		data2[12]=0.0f;
	else 
		data2[12]=imgBuffer[(ydownInt-1)*xsize+xdownInt+2];
	if(xdownInt+2<0||ydownInt+0<0||xdownInt+2>=xsize||ydownInt+0>=ysize)
		data2[13]=0.0f;
	else 
		data2[13]=imgBuffer[(ydownInt+0)*xsize+xdownInt+2];
	if(xdownInt+2<0||ydownInt+1<0||xdownInt+2>=xsize||ydownInt+1>=ysize)
		data2[14]=0.0f;
	else 
		data2[14]=imgBuffer[(ydownInt+1)*xsize+xdownInt+2];
	if(xdownInt+2<0||ydownInt+2<0||xdownInt+2>=xsize||ydownInt+2>=ysize)
		data2[15]=0.0f;
	else 
		data2[15]=imgBuffer[(ydownInt+2)*xsize+xdownInt+2];

	MatrixMuti(data1,1,4,4,data2,muti12);
	MatrixMuti(muti12,1,4,1,data3,&sampledata);

	return sampledata;
}

/*-------------------------------对数据采样----------------------------------*/
long CSampleFunc::nearest_sample(int in_xsize,int in_ysize,float* imgBufferIn,
							     int out_xsize,int out_ysize,float* imgBufferOut)
{
	//检查输入
	if (imgBufferIn==NULL||imgBufferOut==NULL)
		return -1;

	//计算采样比
	float xsamplerate=float(in_xsize)/float(out_xsize);
	float ysamplerate=float(in_ysize)/float(out_ysize);

	//进行重采样操作
	for (int i=0;i<out_xsize;i++)
	{
		//fprintf(stdout,"\r处理列%d/%d...",out_xsize,i);
		for (int j=0;j<out_ysize;j++)
		{
			imgBufferOut[j*out_xsize+i]=nearest_sample(imgBufferIn,in_xsize,in_ysize,float(i)*xsamplerate,float(j)*ysamplerate);
		}
	}

	return 0;
}

long CSampleFunc::biliner_sample(int in_xsize,int in_ysize,float* imgBufferIn,
								 int out_xsize,int out_ysize,float* imgBufferOut)
{
	//检查输入
	if (imgBufferIn==NULL||imgBufferOut==NULL)
		return -1;

	//计算采样比
	float xsamplerate=float(in_xsize)/float(out_xsize);
	float ysamplerate=float(in_ysize)/float(out_ysize);

	//进行重采样操作
	for (int j=0;j<out_ysize;j++)
	{
		//fprintf(stdout,"\r处理行%d/%d...",out_ysize,j);

		for (int i=0;i<out_xsize;i++)
		{
			imgBufferOut[j*out_xsize+i]=biliner_sample(imgBufferIn,in_xsize,in_ysize,float(i)*xsamplerate,float(j)*ysamplerate);
		}
	}

	return 0;
}

long CSampleFunc::cubic_sample(int in_xsize,int in_ysize,float* imgBufferIn,
							   int out_xsize,int out_ysize,float* imgBufferOut)
{
	//检查输入
	if (imgBufferIn==NULL||imgBufferOut==NULL)
		return -1;

	//计算采样比
	float xsamplerate=float(in_xsize)/float(out_xsize);
	float ysamplerate=float(in_ysize)/float(out_ysize);

	//进行重采样操作
	for (int i=0;i<out_xsize;i++)
	{
		//fprintf(stdout,"\r处理列%d/%d...",out_xsize,i);

		for (int j=0;j<out_ysize;j++)
		{
			imgBufferOut[j*out_xsize+i]=cubic_sample(imgBufferIn,in_xsize,in_ysize,float(i)*xsamplerate,float(j)*ysamplerate);
		}
		
	}

	return 0;
}

/*-----------------------------根据点的位置进行采样---------------------------*/
long CSampleFunc::nearest_sample(float* imgBuffer,int xsize,int ysize,float* xpos,float* ypos,
								int out_xsize,int out_ysize,float* imgBufferOut)
{
	if (imgBuffer==NULL||imgBufferOut==NULL)
		return -1;
	for (int i=0;i<out_xsize;i++)
	{
		for (int j=0;j<out_ysize;j++)
		{
			float tempdata=nearest_sample(imgBuffer,xsize,ysize,xpos[j*out_xsize+i],ypos[j*out_xsize+i]);
			imgBufferOut[j*out_xsize+i]=tempdata;
		}
	}
	return 0;
}

long CSampleFunc::biliner_sample(float* imgBuffer,int xsize,int ysize,float* xpos,float* ypos,
								int out_xsize,int out_ysize,float* imgBufferOut)
{
	if (imgBuffer==NULL||imgBufferOut==NULL)
		return -1;
	for (int i=0;i<out_xsize;i++)
	{
		for (int j=0;j<out_ysize;j++)
		{
			float tempdata=biliner_sample(imgBuffer,xsize,ysize,xpos[j*out_xsize+i],ypos[j*out_xsize+i]);
			imgBufferOut[j*out_xsize+i]=tempdata;
		}
	}
	return 0;
}

long CSampleFunc::cubic_sample(float* imgBuffer,int xsize,int ysize,float* xpos,float* ypos,
								int out_xsize,int out_ysize,float* imgBufferOut)
{
	if (imgBuffer==NULL||imgBufferOut==NULL)
		return -1;
	for (int i=0;i<out_xsize;i++)
	{
		for (int j=0;j<out_ysize;j++)
		{
			float tempdata=cubic_sample(imgBuffer,xsize,ysize,xpos[j*out_xsize+i],ypos[j*out_xsize+i]);
			imgBufferOut[j*out_xsize+i]=tempdata;
		}
	}
	return 0;
}


/*------------------------------影像进行高斯降采样-----------------------------*/
//sample_xsize 表示在x方向上1个像素表示原始象元x方向个数
//sample_xsize 表示在y方向上1个像素表示原始象元y方向个数
long CSampleFunc::gauss_down_sample(int in_xsize,int in_ysize,float* imgBufferIn,
									int sample_xsize,int sample_ysize,float* imgBufferOut)
{
	int out_xize=in_xsize/sample_xsize;
	int out_yize=in_ysize/sample_ysize;

	//获取高斯核
	float *gauss_kernel=new float[sample_xsize*sample_ysize];
	SetGaussCore(gauss_kernel,1.0f,sample_xsize,sample_ysize);

	//将高斯核归一化
	float total=0;
	for (int i=0;i<sample_xsize*sample_ysize;i++)
		total+=gauss_kernel[i];
	for (int i=0;i<sample_xsize*sample_ysize;i++)
		gauss_kernel[i]/=total;

	memset(imgBufferOut,0,sizeof(float)*out_xize*out_yize);

	for (int i=0,l=0;i<in_xsize-sample_xsize;i+=sample_xsize,l++)
	{
		for (int j=0,m=0;j<in_ysize-sample_ysize;j+=sample_ysize,m++)
		{
			//内层卷积运算
			total=0;
			for (int p=0;p<sample_ysize;p++)
			{
				for(int q=0;q<sample_xsize;q++)
				{
					total+=imgBufferIn[(j+p)*in_xsize+q+i]*gauss_kernel[p*sample_xsize+q];
				}
			}
			imgBufferOut[m*out_xize+l]=total;
		}
	}

	return 0;
}

/*---------------------------------影像进行采样--------------------------------*/
long CSampleFunc::nearest_sample(char* pathIn,int xsamplesize,int ysamplesize,char* pathOut)
{
	if(pathIn==NULL||pathOut==NULL)
		return -1;

	//读取数据
	GDALAllRegister();
	GDALDatasetH m_dataset_in=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset_in);
	int ysize=GDALGetRasterYSize(m_dataset_in);
	int bands=GDALGetRasterCount(m_dataset_in);
	double adftrans_in[6],adftrans_out[6];
	GDALGetGeoTransform(m_dataset_in,adftrans_in);
	float *datain =new float[xsize*ysize];
	float *dataout=new float[xsamplesize*ysamplesize];
	const char* pszWKT=GDALGetProjectionRef(m_dataset_in);
	//创建输出数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataset_out=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsamplesize,ysamplesize,bands,GDT_Float32,papszOptions);
	for (int i=0;i<bands;i++)
	{
		fprintf(stdout,"\r处理波段%d/%d...\n",bands,i);

		GDALRasterIO(GDALGetRasterBand(m_dataset_in,i+1),GF_Read,0,0,xsize,ysize,datain,xsize,ysize,GDT_Float32,0,0);
		nearest_sample(xsize,ysize,datain,xsamplesize,ysamplesize,dataout);
		GDALRasterIO(GDALGetRasterBand(m_dataset_out,i+1),GF_Write,0,0,xsamplesize,ysamplesize,dataout,xsamplesize,ysamplesize,GDT_Float32,0,0);
	}
	fflush(stdout);

	adftrans_out[0]=adftrans_in[0];
	adftrans_out[1]=adftrans_in[1]*float(xsize)/float(xsamplesize);
	adftrans_out[2]=adftrans_in[2];
	adftrans_out[3]=adftrans_in[3];
	adftrans_out[4]=adftrans_in[4];
	adftrans_out[5]=adftrans_in[5]*float(ysize)/float(ysamplesize);
	GDALSetGeoTransform(m_dataset_out,adftrans_out);
	GDALSetProjection(m_dataset_out,pszWKT);
	GDALClose(m_dataset_out);
	GDALClose(m_dataset_in);
	delete[]datain;
	delete[]dataout;
	return 0;
}

long CSampleFunc::biliner_sample(char* pathIn,int xsamplesize,int ysamplesize,char* pathOut)
{
	if(pathIn==NULL||pathOut==NULL)
		return -1;

	//读取数据
	GDALAllRegister();
	GDALDatasetH m_dataset_in=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset_in);
	int ysize=GDALGetRasterYSize(m_dataset_in);
	int bands=GDALGetRasterCount(m_dataset_in);
	double adftrans_in[6],adftrans_out[6];
	GDALGetGeoTransform(m_dataset_in,adftrans_in);
	float *datain =new float[xsize*ysize];
	float *dataout=new float[xsamplesize*ysamplesize];
	const char* pszWKT=GDALGetProjectionRef(m_dataset_in);
	//创建输出数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataset_out=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsamplesize,ysamplesize,bands,GDT_Float32,papszOptions);
	for (int i=0;i<bands;i++)
	{
		fprintf(stdout,"\r处理波段%d/%d...\n",bands,i);

		GDALRasterIO(GDALGetRasterBand(m_dataset_in,i+1),GF_Read,0,0,xsize,ysize,datain,xsize,ysize,GDT_Float32,0,0);
		biliner_sample(xsize,ysize,datain,xsamplesize,ysamplesize,dataout);
		GDALRasterIO(GDALGetRasterBand(m_dataset_out,i+1),GF_Write,0,0,xsamplesize,ysamplesize,dataout,xsamplesize,ysamplesize,GDT_Float32,0,0);
	}
	fflush(stdout);
	GDALClose(m_dataset_in);

	adftrans_out[0]=adftrans_in[0];
	adftrans_out[1]=adftrans_in[1]*float(xsize)/float(xsamplesize);
	adftrans_out[2]=adftrans_in[2];
	adftrans_out[3]=adftrans_in[3];
	adftrans_out[4]=adftrans_in[4];
	adftrans_out[5]=adftrans_in[5]*float(ysize)/float(ysamplesize);
	GDALSetGeoTransform(m_dataset_out,adftrans_out);
	GDALSetProjection(m_dataset_out,pszWKT);

	delete[]datain;
	delete[]dataout;

	GDALClose(m_dataset_out);

	return 0;
}

long CSampleFunc::cubic_sample(char* pathIn,int xsamplesize,int ysamplesize,char* pathOut)
{
	if(pathIn==NULL||pathOut==NULL)
		return -1;

	//读取数据
	GDALAllRegister();
	GDALDatasetH m_dataset_in=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset_in);
	int ysize=GDALGetRasterYSize(m_dataset_in);
	int bands=GDALGetRasterCount(m_dataset_in);
	double adftrans_in[6],adftrans_out[6];
	GDALGetGeoTransform(m_dataset_in,adftrans_in);
	float *datain =new float[xsize*ysize];
	float *dataout=new float[xsamplesize*ysamplesize];
	const char* pszWKT=GDALGetProjectionRef(m_dataset_in);

	//创建输出数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataset_out=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsamplesize,ysamplesize,bands,GDT_Float32,papszOptions);
	for (int i=0;i<bands;i++)
	{
		fprintf(stdout,"\r处理波段%d/%d...\n",bands,i);

		GDALRasterIO(GDALGetRasterBand(m_dataset_in,i+1),GF_Read,0,0,xsize,ysize,datain,xsize,ysize,GDT_Float32,0,0);
		cubic_sample(xsize,ysize,datain,xsamplesize,ysamplesize,dataout);
		GDALRasterIO(GDALGetRasterBand(m_dataset_out,i+1),GF_Write,0,0,xsamplesize,ysamplesize,dataout,xsamplesize,ysamplesize,GDT_Float32,0,0);
	}
	fflush(stdout);

	adftrans_out[0]=adftrans_in[0];
	adftrans_out[1]=adftrans_in[1]*float(xsize)/float(xsamplesize);
	adftrans_out[2]=adftrans_in[2];
	adftrans_out[3]=adftrans_in[3];
	adftrans_out[4]=adftrans_in[4];
	adftrans_out[5]=adftrans_in[5]*float(ysize)/float(ysamplesize);
	GDALSetGeoTransform(m_dataset_out,adftrans_out);
	GDALSetProjection(m_dataset_out,pszWKT);

	GDALClose(m_dataset_in);
	GDALClose(m_dataset_out);
	delete[]datain;
	delete[]dataout;

	return 0;
}



/*--------------------------------高斯降采样---------------------------------*/
//xsampleszie 表示在x方向上1个像素表示原始象元x方向个数
//ysamplesize 表示在y方向上1个像素表示原始象元y方向个数
long CSampleFunc::gauss_down_sample(char* pathIn,int xsampleszie,int ysamplesize,char* pathOut)
{
	GDALAllRegister();

	GDALDatasetH m_dataset=GDALOpen(pathIn,GA_ReadOnly);
	int in_xsize=GDALGetRasterXSize(m_dataset);
	int in_ysize=GDALGetRasterYSize(m_dataset);
	int in_bands=GDALGetRasterCount(m_dataset);

	int out_xsize=in_xsize/xsampleszie;
	int out_ysize=in_ysize/ysamplesize;
	double adftrans_in[6],adftrans_out[6];
	GDALGetGeoTransform(m_dataset,adftrans_in);
	float *datain =new float[in_xsize*in_ysize];
	float *dataout=new float[out_xsize*out_ysize];
	const char* pszWKT=GDALGetProjectionRef(m_dataset);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_datasetOut=GDALCreate(GDALGetDriverByName("HFA"),pathOut,out_xsize,out_ysize,in_bands,GDT_Float32,papszOptions);
	for (int i=0;i<in_bands;i++)
	{
		printf("正在处理波段%d...\n",i+1);
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,in_xsize,in_ysize,datain,in_xsize,in_ysize,GDT_Float32,0,0);
		gauss_down_sample(in_xsize,in_ysize,datain,xsampleszie,ysamplesize,dataout);
		GDALRasterIO(GDALGetRasterBand(m_datasetOut,i+1),GF_Write,0,0,out_xsize,out_ysize,dataout,out_xsize,out_ysize,GDT_Float32,0,0);
	}

	adftrans_out[0]=adftrans_in[0];
	adftrans_out[1]=adftrans_in[1]*float(in_xsize)/float(out_xsize);
	adftrans_out[2]=adftrans_in[2];
	adftrans_out[3]=adftrans_in[3];
	adftrans_out[4]=adftrans_in[4];
	adftrans_out[5]=adftrans_in[5]*float(in_ysize)/float(out_ysize);
	GDALSetGeoTransform(m_datasetOut,adftrans_out);
	GDALSetProjection(m_datasetOut,pszWKT);

	GDALClose(m_datasetOut);
	GDALClose(m_dataset);

	return 0;
}


/*------------------------------多线程影像重采样-------------------------------*/
long CSampleFunc::nearest_sample_thread(char* pathIn,int xsamplesize,int ysamplesize,char* pathOut)
{
	if(pathIn==NULL||pathOut==NULL)
		return -1;

	//读取数据
	GDALAllRegister();
	GDALDatasetH m_dataset_in=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset_in);
	int ysize=GDALGetRasterYSize(m_dataset_in);
	int bands=GDALGetRasterCount(m_dataset_in);
	double adftrans_in[6],adftrans_out[6];
	GDALGetGeoTransform(m_dataset_in,adftrans_in);
	
	//内存测试
	try
	{
		float *testmemory=NULL;
		if((testmemory=new float[20*max(xsize,xsamplesize)*max(ysize,ysamplesize)])==NULL)
		{
			printf("Memory limited!\n");
			return -1;
		}
		if(testmemory!=NULL)
		{
			delete []testmemory;
			testmemory=NULL;
		}
	}
	catch(int)
	{
		exit(0);
	}
	//创建输出数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataset_out=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsamplesize,ysamplesize,bands,GDT_Float32,papszOptions);
	
	GDALDatasetH testdataset=m_dataset_out;
	int testx=GDALGetRasterXSize(m_dataset_out);
	
	
	SAMPLE_STUECT* samplest=new SAMPLE_STUECT[bands];
// 	InitializeCriticalSection(&cs_read_sample);
// 	InitializeCriticalSection(&cs_write_sample);
	for (int i=0;i<bands;i++)
	{
		fprintf(stdout,"\r处理波段%d/%d...\n",bands,i);
		
		samplest[i].m_dataset_In=m_dataset_in;
		samplest[i].m_dataset_Out=m_dataset_out;
		samplest[i].bandIndex=i+1;
		m_threadpool.Run(NearestThreadFunc,(LPVOID)&samplest[i],Low);

	}
	fflush(stdout);

	do 
	{
		Sleep(1);
	} while (m_threadpool.GetWorkingThreadCount()!=0);

// 	DeleteCriticalSection(&cs_read_sample);
// 	DeleteCriticalSection(&cs_write_sample);

//	m_threadpool.Destroy();
	printf("nearest resample finished!\n");

	adftrans_out[0]=adftrans_in[0];
	adftrans_out[1]=adftrans_in[1]*float(xsize)/float(xsamplesize);
	adftrans_out[2]=adftrans_in[2];
	adftrans_out[3]=adftrans_in[3];
	adftrans_out[4]=adftrans_in[4];
	adftrans_out[5]=adftrans_in[5]*float(ysize)/float(ysamplesize);
	GDALSetGeoTransform(m_dataset_out,adftrans_out);

	GDALClose(m_dataset_out);
	GDALClose(m_dataset_in);
	delete[]samplest;

	return 0;
}

long CSampleFunc::biliner_sample_thread(char* pathIn,int xsamplesize,int ysamplesize,char* pathOut)
{
	if(pathIn==NULL||pathOut==NULL)
		return -1;

	//读取数据
	GDALAllRegister();
	GDALDatasetH m_dataset_in=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset_in);
	int ysize=GDALGetRasterYSize(m_dataset_in);
	int bands=GDALGetRasterCount(m_dataset_in);
	double adftrans_in[6],adftrans_out[6];
	GDALGetGeoTransform(m_dataset_in,adftrans_in);
	const char* pszWKT=GDALGetProjectionRef(m_dataset_in);

	//内存测试
	try
	{
		float *testmemory=NULL;
		if((testmemory=new float[20*max(xsize,xsamplesize)*max(ysize,ysamplesize)])==NULL)
		{
			printf("Memory limited!\n");
			return -1;
		}
		if(testmemory!=NULL)
		{
			delete []testmemory;
			testmemory=NULL;
		}

	}
	catch(int)
	{
		exit(0);
	}
	//创建输出数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataset_out=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsamplesize,ysamplesize,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_dataset_out,pszWKT);

	SAMPLE_STUECT* samplest=new SAMPLE_STUECT[bands];
	for (int i=0;i<bands;i++)
	{
		fprintf(stdout,"\r处理波段%d/%d...\n",bands,i);
		samplest[i].m_dataset_In=m_dataset_in;
		samplest[i].m_dataset_Out=m_dataset_out;
		samplest[i].bandIndex=i+1;
		m_threadpool.Run(BilinerThreadFunc,(LPVOID)&samplest[i],Low);

	}
	fflush(stdout);

	do 
	{
		Sleep(1);
	} while (m_threadpool.GetWorkingThreadCount()!=0);
	printf("biliner resample finished!\n");


	adftrans_out[0]=adftrans_in[0];
	adftrans_out[1]=adftrans_in[1]*float(xsize)/float(xsamplesize);
	adftrans_out[2]=adftrans_in[2];
	adftrans_out[3]=adftrans_in[3];
	adftrans_out[4]=adftrans_in[4];
	adftrans_out[5]=adftrans_in[5]*float(ysize)/float(ysamplesize);
	GDALSetGeoTransform(m_dataset_out,adftrans_out);

	GDALClose(m_dataset_in);
	GDALClose(m_dataset_out);
	delete[]samplest;

	return 0;
}

long CSampleFunc::cubic_sample_thread(char* pathIn,int xsamplesize,int ysamplesize,char* pathOut)
{
	if(pathIn==NULL||pathOut==NULL)
		return -1;

	//读取数据
	GDALAllRegister();
	GDALDatasetH m_dataset_in=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset_in);
	int ysize=GDALGetRasterYSize(m_dataset_in);
	int bands=GDALGetRasterCount(m_dataset_in);
	double adftrans_in[6],adftrans_out[6];
	GDALGetGeoTransform(m_dataset_in,adftrans_in);
	const char* pszWKT=GDALGetProjectionRef(m_dataset_in);

	//内存测试
	try
	{
		float *testmemory=NULL;
		if((testmemory=new float[20*max(xsize,xsamplesize)*max(ysize,ysamplesize)])==NULL)
		{
			printf("Memory limited!\n");
			return -1;
		}
		if(testmemory!=NULL)
		{
			delete []testmemory;
			testmemory=NULL;
		}
	}
	catch(int)
	{
		exit(0);
	}
	//创建输出数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataset_out=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsamplesize,ysamplesize,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_dataset_out,pszWKT);

	SAMPLE_STUECT* samplest=new SAMPLE_STUECT[bands];
	for (int i=0;i<bands;i++)
	{
		fprintf(stdout,"\r处理波段%d/%d...\n",bands,i);
		samplest[i].m_dataset_In=m_dataset_in;
		samplest[i].m_dataset_Out=m_dataset_out;
		samplest[i].bandIndex=i+1;
		m_threadpool.Run(CubicThreadFunc,(LPVOID)&samplest[i],Low);

	}
	fflush(stdout);

	do 
	{
		Sleep(1);
	} while (m_threadpool.GetWorkingThreadCount()!=0);
	printf("Cubic resample finished!\n");
	adftrans_out[0]=adftrans_in[0];
	adftrans_out[1]=adftrans_in[1]*float(xsize)/float(xsamplesize);
	adftrans_out[2]=adftrans_in[2];
	adftrans_out[3]=adftrans_in[3];
	adftrans_out[4]=adftrans_in[4];
	adftrans_out[5]=adftrans_in[5]*float(ysize)/float(ysamplesize);
	GDALSetGeoTransform(m_dataset_out,adftrans_out);

	GDALClose(m_dataset_in);
	GDALClose(m_dataset_out);
	delete[]samplest;

	return 0;
}