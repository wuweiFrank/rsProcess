#include "fusionfunc.h"
#include <fstream>
#include "..\gdal\include\gdal_priv.h"
#include "..\matrixOperation.h"	//矩阵处理函数
#include"..\Global.h"
#include "..\rsImgProcess\SampleFunc.h"			//采样函数
using namespace std;

#pragma comment(lib,"gdal_i.lib")
//对于影像进行最小二乘解混（不加约束）
void endmember_unmix_LSE(char* pathEnd,char* pathImg,char* pathRed,int endnumbers)
{
	GDALAllRegister();
	CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "NO" );	//中文路径

	GDALDatasetH m_dataset;
	m_dataset=GDALOpen(pathImg,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset);
	int ysize=GDALGetRasterYSize(m_dataset);
	int bands=GDALGetRasterCount(m_dataset);

	ofstream ofs;
	ofs.open("test.txt");

	if (endnumbers>bands)
	{
		//无法求解
		printf("端元数大于波段个数，无法进行最小二乘求解！\n");
		return;
	}

	float *imgData=new float[xsize*ysize*bands];
	float *endData=new float[bands*endnumbers];

	//获取端元光谱
	endData=get_endmenber_spectralf(pathEnd,bands,endnumbers);

	double *dendData=new double[bands*endnumbers];
	//端元数据按比例缩小
	for (int i=0;i<endnumbers*bands;i++)
		dendData[i]=endData[i]/*/10.0f*/;

	//获取影像数据
	for (int i=0;i<bands;i++)
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,imgData+i*xsize*ysize,xsize,ysize,GDT_Float32,0,0);
	GDALClose(m_dataset);

	//最小二乘解算输出
	double* data =new double[bands];
	double* datamuti=new double[endnumbers];

	double* res  =new double[endnumbers];
	double* endTrans=new double[endnumbers*bands];
	double* endMuti =new double[endnumbers*endnumbers];
	double* endInv  =new double[endnumbers*endnumbers];
	float* resImgae=new float[endnumbers*xsize*ysize];


	//将结果输出为影像算了......
	for (int i=0;i<xsize*ysize;i++)
	{
		for(int j=0;j<bands;j++)
			data[j]=imgData[j*xsize*ysize+i];

		//矩阵运算
		MatrixTrans(dendData,bands,endnumbers,endTrans);
		MatrixMuti(endTrans,endnumbers,bands,endnumbers,dendData,endMuti);
		MatrixInverse(endMuti,endnumbers,endInv);
		MatrixMuti(endTrans,endnumbers,bands,1,data,datamuti);
		MatrixMuti(endInv,endnumbers,endnumbers,1,datamuti,res);

		//输出求解结果
		printf("\r最小二乘计算丰度处理像素：%d/%d",xsize*ysize,i+1);
		
		//for(int j=0;j<endnumbers;j++)
		//	ofs<<res[j]<<"  ";
		//ofs<<endl;

		//结果输出为影像
		for (int j=0;j<endnumbers;j++)
			resImgae[j*xsize*ysize+i]=res[j];

	}
	printf("\n finished...\n");

	GDALDatasetH m_datasetout;
	m_datasetout=GDALCreate(GDALGetDriverByName("GTiff"),pathRed,xsize,ysize,endnumbers,GDT_Float32,NULL);
	for (int i=0;i<endnumbers;i++)
		GDALRasterIO(GDALGetRasterBand(m_datasetout,i+1),GF_Write,0,0,xsize,ysize,resImgae+i*xsize*ysize,xsize,ysize,GDT_Float32,0,0);
	GDALClose(m_datasetout);

	//清除数据
	delete[]imgData;imgData=NULL;
	delete[]endData;endData=NULL;

	delete[]data;   data=NULL;
	delete[]res;	res=NULL;
	delete[]endTrans;endTrans=NULL;
	delete[]endMuti;endMuti=NULL;
	delete[]endInv; endInv=NULL;
	delete[]resImgae;resImgae=NULL;
	delete[]dendData;dendData=NULL;
	delete[]datamuti;datamuti=NULL;

	return ;
}

//对解混丰度进行高斯点扩散函数处理
//得到修正后高光谱的丰度
void endmember_sample(char* pathRed,char* pathSample)
{
	GDALAllRegister();
	GDALDatasetH m_dataset;
	m_dataset=GDALOpen(pathRed,GA_ReadOnly);
	
	//空间响应为高斯函数
	CSampleFunc samplefunc;
	samplefunc.gauss_down_sample(pathRed,3,3,pathSample);

	return ;
}

//计算求解的残差 其中最后一个波段为总残差
void endmember_residual(char* pathimg,char* pathend,char* pathred,char* pathres,int endnumbers)
{
	GDALAllRegister();
	GDALDatasetH m_dataset   =GDALOpen(pathred,GA_ReadOnly);
	GDALDatasetH m_datasetimg=GDALOpen(pathimg,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset);
	int ysize=GDALGetRasterYSize(m_dataset);
	int bands=GDALGetRasterCount(m_dataset);

	float* datared=new float[xsize*ysize*bands];
	float* dataimg=new float[xsize*ysize*bands];
	float* dataend=new float[bands*endnumbers];
	float* dataout=new float[(bands+1)*xsize*ysize];
	memset(dataout,0,sizeof(float)*xsize*ysize*(bands+1));

	for(int i=0;i<bands;i++)
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,datared+i*xsize*ysize,xsize,ysize,GDT_Float32,0,0);
	for(int i=0;i<bands;i++)
		GDALRasterIO(GDALGetRasterBand(m_datasetimg,i+1),GF_Read,0,0,xsize,ysize,dataimg+i*xsize*ysize,xsize,ysize,GDT_Float32,0,0);
	dataend=get_endmenber_spectralf(pathend,bands,endnumbers);
	GDALClose(m_dataset);
	GDALClose(m_datasetimg);

	for (int i=0;i<xsize*ysize;i++)
	{
		for (int j=0;j<bands;j++)
			for(int k=0;k<endnumbers;k++)
				dataout[j*xsize*ysize+i]+=datared[k*xsize*ysize+i]*dataend[j*endnumbers+k];

		//最后一个波段为总的残差
		for (int j=0;j<bands;j++)
			dataout[(bands)*xsize*ysize+i]+=dataout[j*xsize*ysize+i]-dataimg[j*xsize*ysize+i];
	}
	
	GDALDatasetH m_datasetout=GDALCreate(GDALGetDriverByName("GTiff"),pathres,xsize,ysize,bands+1,GDT_Float32,NULL);
	for(int i=0;i<bands+1;i++)
		GDALRasterIO(GDALGetRasterBand(m_datasetout,i+1),GF_Write,0,0,xsize,ysize,dataout+i*xsize*ysize,xsize,ysize,GDT_Float32,0,0);
	GDALClose(m_datasetout);

	delete[]dataend;
	delete[]dataimg;
	delete[]dataout;
	delete[]datared;
}

//修正端元光谱
void endmember_modify(char* pathred,char* pathimg,char* pathmodify)
{
	//通过丰度 原始影像和原始端元获取新端元
	GDALAllRegister();
	GDALDatasetH m_datared=GDALOpen(pathred,GA_ReadOnly);
	GDALDatasetH m_dataimg=GDALOpen(pathimg,GA_ReadOnly);

	int xsizered=GDALGetRasterXSize(m_datared);
	int ysizered=GDALGetRasterYSize(m_datared);
	int bandsred=GDALGetRasterCount(m_datared);

	int xsizeimg=GDALGetRasterXSize(m_dataimg);
	int ysizeimg=GDALGetRasterYSize(m_dataimg);
	int bandsimg=GDALGetRasterCount(m_dataimg);

	//获取数据
	double *datared=new double[xsizered*ysizered*bandsred];
	double *dataimg=new double[xsizeimg*ysizeimg*bandsimg];
	for(int i=0;i<bandsimg;i++)
		GDALRasterIO(GDALGetRasterBand(m_dataimg,i+1),GF_Read,0,0,xsizeimg,ysizeimg,dataimg+i*xsizeimg*ysizeimg,xsizeimg,ysizeimg,GDT_Float64,0,0);
	for(int i=0;i<bandsred;i++)
		GDALRasterIO(GDALGetRasterBand(m_datared,i+1),GF_Read,0,0,xsizered,ysizered,datared+i*xsizered*ysizered,xsizered,ysizered,GDT_Float64,0,0);

	//矩阵运算
	double *dataend=new double[bandsred*bandsimg];
	memset(dataend,0,sizeof(double)*bandsred*bandsimg);

	double *dataredt=new double[xsizered*ysizered*bandsred];
	double *dataredm=new double[bandsred*bandsred];
	double *dataredI=new double[bandsred*bandsred];
	double *dataimgm=new double[bandsimg*bandsred];

	//计算中
	printf("计算修正后端元光谱\n");
	MatrixTrans(datared,bandsred,xsizered*ysizered,dataredt);
	MatrixMuti(datared,bandsred,xsizered*ysizered,bandsred,dataredt,dataredm);
	MatrixMuti(dataimg,bandsimg,xsizered*ysizered,bandsred,dataredt,dataimgm);
	MatrixInverse(dataredm,bandsred,dataredI);
	MatrixMuti(dataimgm,bandsimg,bandsred,bandsred,dataredI,dataend);
	printf("finished...\n");

	//端元导出
	export_endmenber_spectral(pathmodify,dataend,bandsimg,bandsred);

	//清理内存空间
	delete[]dataend;
	delete[]dataredt;
	delete[]dataredm;
	delete[]dataredI;
	delete[]dataimgm;

	delete[]datared;
	delete[]dataimg;
}

//根据丰度信息和修正后的端元信息进行影像重建
void endmember_recovery_image(char* pathred,char* pathmodend,char* pathout,int hyspectral)
{
	GDALAllRegister();
	GDALDatasetH m_datasetred=GDALOpen(pathred,GA_ReadOnly);

	int xsize=GDALGetRasterXSize(m_datasetred);
	int ysize=GDALGetRasterYSize(m_datasetred);
	int bands=GDALGetRasterCount(m_datasetred);

	//影像数据空间的申请
	float* datared=new float[xsize*ysize*bands];
	float* datamodend=new float[bands*hyspectral];
	float* datarecov =new float[xsize*ysize*hyspectral];
	memset(datarecov,0,sizeof(float)*xsize*ysize*hyspectral);

	for(int i=0;i<bands;i++)
		GDALRasterIO(GDALGetRasterBand(m_datasetred,i+1),GF_Read,0,0,xsize,ysize,datared+i*xsize*ysize,xsize,ysize,GDT_Float32,0,0);
	datamodend=get_endmenber_spectralf(pathmodend,hyspectral,bands);
	GDALClose(m_datasetred);


	//求解每一个像素
	for (int i=0;i<xsize*ysize;i++)
	{
		printf("\r原始图像恢复像素：%d/%d",xsize*ysize,i+1);
		for (int j=0;j<hyspectral;j++)
		{
			for (int k=0;k<bands;k++)
			{
				datarecov[j*xsize*ysize+i]+=datared[k*xsize*ysize+i]*datamodend[j*bands+k];
			}
		}
	}
	printf("\n");

	//结果输出
	GDALDatasetH m_dataset=GDALCreate(GDALGetDriverByName("GTiff"),pathout,xsize,ysize,hyspectral,GDT_Float32,NULL);
	for (int i=0;i<hyspectral;i++)
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Write,0,0,xsize,ysize,datarecov+i*xsize*ysize,xsize,ysize,GDT_Float32,0,0);
	GDALClose(m_dataset);

	//清除数据
	delete[]datarecov;
	delete[]datared;
	delete[]datamodend;
}

//计算影像光谱角残差和绝对值差
void endmember_residual_spAngle(char* pathrecovery,char* pathOri,char* pathresidual)
{
	residual_spAngle(pathrecovery,pathOri,pathresidual);
}