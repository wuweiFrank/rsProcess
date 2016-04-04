#include "fusionfunc.h"
#include "..\rsImgProcess\SampleFunc.h"
#include "..\gdal/include/gdal_priv.h"
#include "HyperSpectural.h"
#include "..\FrequencyFunc\waveletTrans.h" 
#pragma comment(lib,"gdal_i.lib")
//根据获取波段范围的分组情况
void wavelet_get_spectral_group(float* hywavelen,int hynum,float* mswavelen,int msnum,int *group,bool range)
{
	memset(group,0,sizeof(int)*hynum);
	//无输入波段波长的情况下
	if(hywavelen==NULL&&mswavelen==NULL)
	{
		//直接对应求均值 按照波段平均划分
		int hmMap=hynum/msnum;
		for (int i=0;i<msnum;i++)
			for (int j=0;j<hmMap;j++)
				group[i*hmMap+j]=i;
		group[hynum-1]=msnum-1;
	}

	//有输入波长的情况下  首先获取多光谱波段对应的高光谱波段 然后进行处理
	if(hywavelen!=NULL&&mswavelen!=NULL)	
	{
		//对高光谱计算各个波段对应的多光谱波段
		if(range==true)		//若输入的是光谱范围
		{
			for (int i=0;i<hynum;i++)
			{
				for(int j=0;j<msnum;j++)
				{
					if(hywavelen[j]<mswavelen[2*j+1])
					{
						group[j]=j;
						break;
					}
				}
			}
		}
		else
		{
			//获取最近的光谱
			for(int i=0;i<hynum;i++)//直接输入的是光谱
			{
				int indexm=0;
				float mindata=1000;;
				for (int j=0;j<msnum;j++)
				{
					if(mindata>abs(hywavelen[i]-mswavelen[j]))
					{
						mindata=abs(hywavelen[i]-mswavelen[j]);
						indexm=j;
					}
				}
				group[i]=indexm;
			}
		}//对高光谱数据分组结束
	}
}

//影像重采样
void wavelet_image_sample(char* pathInHy,char* pathInMs,char* pathOut)
{
	printf("高光谱图像重采样\n");
	GDALAllRegister();
	GDALDatasetH m_datasetHy=GDALOpen(pathInHy,GA_ReadOnly);
	GDALDatasetH m_datasetMs=GDALOpen(pathInMs,GA_ReadOnly);

	int xsizems=GDALGetRasterXSize(m_datasetMs);
	int ysizems=GDALGetRasterYSize(m_datasetMs);

	int xsizehy=GDALGetRasterXSize(m_datasetHy);
	int ysizehy=GDALGetRasterYSize(m_datasetHy);

	CSampleFunc samplefunc;
	samplefunc.cubic_sample(pathInHy,xsizems,ysizems,pathOut);
}

//对高光谱影像的每一个波段进行小波变换
void wavelet_to_image(char* pathHy,char* pathHyW1,char* pathHyW2,char* pathMs,char* pathMsW1,char* pathMsW2)
{
	WaveletTransTri(pathHy,pathHyW1,pathHyW2);
	WaveletTransTri(pathMs,pathMsW1,pathMsW2);
}

//通过给定的对应波段进行小波变换融合
void wavelet_fusion(char* pathHyW1,char* pathHyW2,char* pathMsW2,char* pathRecov,int* group)
{
	GDALAllRegister();
	GDALDatasetH m_datasethyw1=GDALOpen(pathHyW1,GA_ReadOnly);
	GDALDatasetH m_datasethyw2=GDALOpen(pathHyW2,GA_Update);
	GDALDatasetH m_datasetmsw1=GDALOpen(pathMsW2,GA_ReadOnly);

	int xsize=GDALGetRasterXSize(m_datasethyw1);
	int ysize=GDALGetRasterYSize(m_datasethyw1);
	int band2=GDALGetRasterCount(m_datasethyw1);
	int band1=GDALGetRasterCount(m_datasetmsw1);

	float* datahy=new float[xsize*ysize];
	float* datams=new float[xsize*ysize];

	for (int i=0;i<band1;i++)
	{
		printf("\r融合波段%d/%d",band1,i+1);
		GDALRasterIO(GDALGetRasterBand(m_datasethyw2,i+1),GF_Read,0,0,xsize,ysize,datahy,xsize,ysize,GDT_Float32,0,0);
		GDALRasterIO(GDALGetRasterBand(m_datasetmsw1,group[i]+1),GF_Read,0,0,xsize,ysize,datams,xsize,ysize,GDT_Float32,0,0);
		
		for (int j=0;j<xsize;j++)
			for(int k=0;k<ysize;k++)
				if(j>xsize/8||i>ysize/8)
					datahy[k*xsize+j]=datams[k*xsize+j];
		GDALRasterIO(GDALGetRasterBand(m_datasethyw2,i+1),GF_Write,0,0,xsize,ysize,datahy,xsize,ysize,GDT_Float32,0,0);
	}
	printf("\n");

	GDALClose(m_datasethyw1);
	GDALClose(m_datasethyw2);
	GDALClose(m_datasetmsw1);
	delete[]datahy;
	delete[]datams;

	printf("小波逆变换\n");
	WaveletInvTrans(pathRecov,pathHyW1,pathHyW2,3);
}