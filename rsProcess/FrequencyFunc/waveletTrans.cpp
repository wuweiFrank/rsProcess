#include "waveletTrans.h"
//============================================================================
// Name        : waveletTrans.cpp
// Author      : ww
// Version     : 1.0  
// Copyright   : ww
// Description : 影像的小波变换及其逆变换
//============================================================================
//  [4/9/2014 wuwei just you]
// 增加对影像所有波段的小波变换和小波逆变换处理
// 增加对影像所有波段的影像低通和高通滤波
// [3/23/2015 wuwei just you]
/********************************************************************************
*函数描述：	LPFliter，HPFliter	：完成图像的小波变换							*	
*函数参数：	char* pathSrc       ：输入影像路径     								*
*			char* pathDst1		：输出影像路径									*
*			int band			：处理影像的第几个波段							*
********************************************************************************/
//基于小波变换的高通滤波
void HPFliter(char* pathSrc,char* pathDst,int band)
{
	//三层小波变换
	WaveletTransTri(pathSrc,"~tempH.tif","~tempV.tif",band);	

	//获取数据滤波处理
	GDALAllRegister();
	GDALDatasetH m_dataset;
	m_dataset=GDALOpen("~tempV.tif",GA_Update);
	int width=GDALGetRasterXSize(m_dataset);
	int heigh=GDALGetRasterYSize(m_dataset);
	//const int layer=8;
	float* m_data=new float[width*heigh];
	GDALRasterBandH m_band=GDALGetRasterBand(m_dataset,1);
	GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_data,width,heigh,GDT_Float32,0,0);

	int iWidth=width>>3;
	int iHeigh=heigh>>3;

	for (int i=0;i<iWidth;i++)
		for (int j=0;j<iHeigh;j++)
			m_data[j*width+i]=0;
	GDALRasterIO(GDALGetRasterBand(m_dataset,1),GF_Write,0,0,width,heigh,m_data,width,heigh,GDT_Float32,0,0);
	GDALClose(m_dataset);
	delete[]m_data;

	//小波反变换
	WaveletInvTrans(pathDst,"~tempH.tif","~tempV.tif",3,1);
	std::remove("~tempH.tif");
	std::remove("~tempV.tif");
}
void HPFliter(char* pathSrc,char* pathDst)
{
	//三层小波变换
	WaveletTransTri(pathSrc,"~tempH.tif","~tempV.tif");	

	//获取数据滤波处理
	GDALAllRegister();
	GDALDatasetH m_dataset;
	m_dataset=GDALOpen("~tempV.tif",GA_Update);
	int width=GDALGetRasterXSize(m_dataset);
	int heigh=GDALGetRasterYSize(m_dataset);
	int bands=GDALGetRasterCount(m_dataset);
	//const int layer=8;
	float* m_data=new float[width*heigh];
	for(int k=0;k<bands;k++)
	{

		GDALRasterBandH m_band=GDALGetRasterBand(m_dataset,k+1);
		GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_data,width,heigh,GDT_Float32,0,0);

		int iWidth=width>>3;
		int iHeigh=heigh>>3;

		for (int i=0;i<iWidth;i++)
			for (int j=0;j<iHeigh;j++)
				m_data[j*width+i]=0;
		GDALRasterIO(GDALGetRasterBand(m_dataset,k+1),GF_Write,0,0,width,heigh,m_data,width,heigh,GDT_Float32,0,0);
	}
	
	GDALClose(m_dataset);
	delete[]m_data;

	//小波反变换
	WaveletInvTrans(pathDst,"~tempH.tif","~tempV.tif",3);
	std::remove("~tempH.tif");
	std::remove("~tempV.tif");
}

//基于小波变换的低通滤波
void LPFliter(char* pathSrc,char* pathDst,int band)
{
	//三层小波变换
	WaveletTransOnce(pathSrc,"~tempH.tif","~tempV.tif",band);	

	//获取数据滤波处理
	GDALAllRegister();
	GDALDatasetH m_dataset1,m_dataset2;
	m_dataset1=GDALOpen("~tempV.tif",GA_Update);
	m_dataset2=GDALOpen("~tempH.tif",GA_Update);
	int width1=GDALGetRasterXSize(m_dataset1);
	int heigh1=GDALGetRasterYSize(m_dataset1);
	int width2=GDALGetRasterXSize(m_dataset2);
	int heigh2=GDALGetRasterYSize(m_dataset2);

	//const int layer=8;
	float* m_data1=new float[width1*heigh1];
	float* m_data2=new float[width2*heigh2];
	GDALRasterBandH m_band=GDALGetRasterBand(m_dataset1,1);
	GDALRasterIO(m_band,GF_Read,0,0,width1,heigh1,m_data1,width1,heigh1,GDT_Float32,0,0);

	int iWidth1=width1>>1;
	int iHeigh1=heigh1>>1;
	int iWidth2=width2>>1;
	int iHeigh2=heigh2;
	for (int i=width1-iWidth1;i<width1;i++)
		for (int j=0;j<heigh1;j++)
			m_data1[j*width1+i]=0;
	for (int i=0;i<width1;i++)
		for (int j=heigh1-iHeigh1;j<heigh1;j++)
			m_data1[j*width1+i]=0;
	for (int i=iWidth2;i<width2;i++)
		for(int j=0;j<iHeigh2;j++)
			m_data2[j*width1+i]=0;

	m_band=GDALGetRasterBand(m_dataset1,1);
	GDALRasterIO(m_band,GF_Write,0,0,width1,heigh1,m_data1,width1,heigh1,GDT_Float32,0,0);
	m_band=GDALGetRasterBand(m_dataset2,1);
	GDALRasterIO(m_band,GF_Write,0,0,width2,heigh2,m_data2,width2,heigh2,GDT_Float32,0,0);
	GDALClose(m_dataset1);
	GDALClose(m_dataset2);
	delete[]m_data1;
	delete[]m_data2;

	//小波反变换
	WaveletInvTrans(pathDst,"~tempH.tif","~tempV.tif",1);
	std::remove("~tempH.tif");
	std::remove("~tempV.tif");
}
void LPFliter(char* pathSrc,char* pathDst)
{
	//三层小波变换
	WaveletTransTri(pathSrc,"~tempH.tif","~tempV.tif");	

	//获取数据滤波处理
	GDALAllRegister();
	GDALDatasetH m_dataset1,m_dataset2;
	m_dataset1=GDALOpen("~tempV.tif",GA_Update);
	m_dataset2=GDALOpen("~tempH.tif",GA_Update);
	int width1=GDALGetRasterXSize(m_dataset1);
	int heigh1=GDALGetRasterYSize(m_dataset1);
	int width2=GDALGetRasterXSize(m_dataset2);
	int heigh2=GDALGetRasterYSize(m_dataset2);
	int bands =GDALGetRasterCount(m_dataset1);

	//const int layer=8;
	float* m_data1=new float[width1*heigh1];
	float* m_data2=new float[width2*heigh2];

	for(int i=0;i<bands;i++)
	{
		GDALRasterBandH m_band=GDALGetRasterBand(m_dataset1,i+1);
		GDALRasterIO(m_band,GF_Read,0,0,width1,heigh1,m_data1,width1,heigh1,GDT_Float32,0,0);

		int iWidth1=width1>>1;
		int iHeigh1=heigh1>>1;
		int iWidth2=width2>>1;
		int iHeigh2=heigh2;
		for (int i=width1-iWidth1;i<width1;i++)
			for (int j=0;j<heigh1;j++)
				m_data1[j*width1+i]=0;
		for (int i=0;i<width1;i++)
			for (int j=heigh1-iHeigh1;j<heigh1;j++)
				m_data1[j*width1+i]=0;
		for (int i=iWidth2;i<width2;i++)
			for(int j=0;j<iHeigh2;j++)
				m_data2[j*width1+i]=0;

		m_band=GDALGetRasterBand(m_dataset1,1);
		GDALRasterIO(m_band,GF_Write,0,0,width1,heigh1,m_data1,width1,heigh1,GDT_Float32,0,0);
		m_band=GDALGetRasterBand(m_dataset2,1);
		GDALRasterIO(m_band,GF_Write,0,0,width2,heigh2,m_data2,width2,heigh2,GDT_Float32,0,0);	
	}

	GDALClose(m_dataset1);
	GDALClose(m_dataset2);
	delete[]m_data1;
	delete[]m_data2;

	//小波反变换
	WaveletInvTrans(pathDst,"~tempH.tif","~tempV.tif",1);
	std::remove("~tempH.tif");
	std::remove("~tempV.tif");
}

/********************************************************************************
*函数描述：Hori_Transform完成一次图像水平方向的小波变换						    *	
*函数参数：float  **spOriginData：二维指针，指向原始的图像数据					*
*		   float  **spTransData0：小波变换系数，存放一次水平变换后的小波系数	*
*		   int   nHeight		  ：图像属性参数，数值为原始图像的高			*
*		   int	 nWidth_H	      ：图像属性参数，数值为原始图像宽度值的一半	*
*		   float fRadius		  ：小波变换因子，在调用时候已指定数值为1		*
********************************************************************************/
void Hori_Transform(float ** spOriginData, float ** spTransData0, int nHeight, int nWidth_H, float fRadius)
{
	int Trans_W,				//图像扫描线控制：横坐标
		Trans_H,				//图像扫描线控制：纵坐标
		Trans_M,				//图像矩阵的横坐标
		Trans_N;				//图像矩阵的纵坐标
	int iWidth = nWidth_H * 2;	//原始图像的宽度值
	float  Trans_Coeff0;		//小波变换系数
	float  Trans_Coeff1;
	//本模块完成变换系数的赋值采样
	for(Trans_H = 0; Trans_H < nHeight; Trans_H ++)            
	{
		for(Trans_N = 0; Trans_N < nWidth_H; Trans_N ++)           
		{
			Trans_W = Trans_N *2;
			if (fRadius == 2)
			{
				spTransData0[Trans_H][Trans_N] = (spOriginData[Trans_H][Trans_W]);
				spTransData0[Trans_H][nWidth_H+Trans_N] = (spOriginData[Trans_H][Trans_W+1]);
			}
			else
			{
				spTransData0[Trans_H][Trans_N] = (spOriginData[Trans_H][Trans_W]-128);				//even
				spTransData0[Trans_H][nWidth_H+Trans_N] = (spOriginData[Trans_H][Trans_W+1]-128);	//odd
			}
		}
	}
	//通过图像的差分，完成小波变换
	for(Trans_H=0; Trans_H<nHeight; Trans_H++)
	{
		for(Trans_N=0; Trans_N<nWidth_H-1; Trans_N++)
		{
			//奇偶数值和的一半
			Trans_Coeff1 = ((spTransData0[Trans_H][Trans_N]+spTransData0[Trans_H][Trans_N+1])/2);	
			//逻辑非操作后数值加1
			Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;	
			//系数预测
			spTransData0[Trans_H][nWidth_H+Trans_N] = spTransData0[Trans_H][nWidth_H+Trans_N]+Trans_Coeff1;	
		}
		//完成一个偶系数的边界处理
		Trans_Coeff1 = ((spTransData0[Trans_H][nWidth_H-1]+spTransData0[Trans_H][nWidth_H-2])/2);
		Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;
		spTransData0[Trans_H][iWidth-1] = spTransData0[Trans_H][iWidth-1]+Trans_Coeff1;
		//完成一个奇系数的边界处理
		Trans_Coeff0 = ((spTransData0[Trans_H][nWidth_H]+spTransData0[Trans_H][nWidth_H+1])/4);
		spTransData0[Trans_H][0] = spTransData0[Trans_H][0]+Trans_Coeff0;
		//提升，整数到整数的变换
		for(Trans_N=1; Trans_N<nWidth_H; Trans_N++)
		{
			Trans_Coeff0 = ((spTransData0[Trans_H][nWidth_H+Trans_N]+spTransData0[Trans_H][nWidth_H+Trans_N-1])/4);
			spTransData0[Trans_H][Trans_N] = spTransData0[Trans_H][Trans_N]+Trans_Coeff0;
		}

	}
}

//转换为一维指针
void Hori_Transform(float *  spOriginData, float *  spTransData0, int nHeight, int nWidth_H, float fRadius)
{
	//检查输入
	if (spOriginData==NULL||spTransData0==NULL)
		return ;
	if (nHeight<=0||nWidth_H<=0)
		return ;
	int nWidth=2*nWidth_H;

	float** m_spOriginData;
	float** m_spTransData0;

	//指针空间的申请
	m_spOriginData=new float*[nHeight];
	m_spTransData0=new float*[nHeight];
	for (int i=0;i<nWidth;i++)
	{
		m_spOriginData[i]=new float[nWidth];
		m_spTransData0[i]=new float[nWidth];

		memcpy(m_spOriginData[i],spOriginData+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData0[i],m_spTransData0+i*nWidth,sizeof(float)*nWidth);
	}
	//调用二维指针函数
	Hori_Transform(m_spOriginData,m_spTransData0,nHeight,nWidth_H,fRadius);

	for (int i=0;i<nHeight;i++)
		memcpy(m_spTransData0+i*nWidth,m_spTransData0[i],sizeof(float)*nWidth);

	//指针空间的释放
	for (int i=0;i<nHeight;i++)
	{
		delete[]m_spTransData0[i];
		delete[]m_spOriginData[i];
	}
	delete[]m_spTransData0;
	delete[]m_spOriginData;
}


/********************************************************************************
*函数描述：	Vert_Transform完成一次图像竖直方向的小波变换						*	
*函数参数：	float  **spOriginData：二维指针，指向原始的图像数据					*
*			float  **spTransData1：小波变换系数，存放一次竖直变换后的小波系数	*
*			int   nHeight_H		：图像属性参数，数值为原始图像高度值的一半		*
*			int	  nWidth		：图像属性参数，数值为原始图像宽度				*
*			float fRadius		：小波变换因子，在调用时候已指定数值为1		*
********************************************************************************/
void Vert_Transform(float ** spOriginData, float ** spTransData1, int nHeight_H, int nWidth, float fRadius)
{
	int Trans_W,				//图像扫描线控制：横坐标
		Trans_H,				//图像扫描线控制：纵坐标
		Trans_M,				//图像矩阵的横坐标
		Trans_N;				//图像矩阵的纵坐标
	int iHeight = nHeight_H * 2;//原始图像的宽度值
	float  Trans_Coeff0;			//小波变换系数
	float  Trans_Coeff1;
	//本模块完成变换系数的赋值采样
	for(Trans_W = 0; Trans_W < nWidth; Trans_W ++)            
	{
		for(Trans_M = 0; Trans_M < nHeight_H; Trans_M ++)           
		{
			Trans_H = Trans_M * 2;
			if (fRadius == 2)
			{
				spTransData1[Trans_M][Trans_W] = (spOriginData[Trans_H][Trans_W]);
				spTransData1[nHeight_H + Trans_M][Trans_W] = (spOriginData[Trans_H+1][Trans_W]);
			}
			else
			{
				spTransData1[Trans_M][Trans_W] = (spOriginData[Trans_H][Trans_W]-128);		//even
				spTransData1[nHeight_H + Trans_M][Trans_W] = (spOriginData[Trans_H+1][Trans_W]-128);	//odd
			}
		}
	}
	//通过图像的差分，完成小波变换
	for(Trans_W=0; Trans_W<nWidth; Trans_W++)
	{
		for(Trans_M=0; Trans_M<nHeight_H-1; Trans_M++)
		{
			//奇偶数值和的一半
			Trans_Coeff1 = ((spTransData1[Trans_M][Trans_W]+spTransData1[Trans_M+1][Trans_W])/2);	
			//逻辑非操作后数值加1
			Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;	
			//系数预测
			spTransData1[nHeight_H + Trans_M][Trans_W] = spTransData1[nHeight_H + Trans_M][Trans_W]+Trans_Coeff1;
		}
		//完成一个偶系数的边界处理
		Trans_Coeff1 = ((spTransData1[nHeight_H-1][Trans_W]+spTransData1[nHeight_H-2][Trans_W])/2);
		Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;
		spTransData1[iHeight-1][Trans_W] = spTransData1[iHeight-1][Trans_W]+Trans_Coeff1;
		//完成一个奇系数的边界处理
		Trans_Coeff0 = ((spTransData1[nHeight_H][Trans_W]+spTransData1[nHeight_H+1][Trans_W])/4);
		spTransData1[0][Trans_W] = spTransData1[0][Trans_W]+Trans_Coeff0;
		//提升，整数到整数的变换
		for(Trans_M=1; Trans_M<nHeight_H; Trans_M++)
		{
			Trans_Coeff0 = ((spTransData1[nHeight_H + Trans_M][Trans_W]+spTransData1[nHeight_H + Trans_M -1][Trans_W])/4);
			spTransData1[Trans_M][Trans_W] = spTransData1[Trans_M][Trans_W]+Trans_Coeff0;
		}

	}
}

//转换为一维指针
void Vert_Transform(float *  spOriginData, float *  spTransData1, int nHeight_H, int nWidth, float fRadius)
{
	//检查输入
	if (spOriginData==NULL||spTransData1==NULL)
		return ;
	if (nHeight_H<=0||nWidth<=0)
		return ;
	int nHeight=2*nHeight_H;

	float** m_spOriginData;
	float** m_spTransData1;

	//指针空间的申请
	m_spOriginData=new float*[nHeight];
	m_spTransData1=new float*[nHeight];
	for (int i=0;i<nWidth;i++)
	{
		m_spOriginData[i]=new float[nWidth];
		m_spTransData1[i]=new float[nWidth];

		memcpy(m_spOriginData[i],spOriginData+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData1[i],m_spTransData1+i*nWidth,sizeof(float)*nWidth);
	}
	//调用二维指针函数
	Vert_Transform(m_spOriginData,m_spTransData1,nHeight_H,nWidth,fRadius);

	for (int i=0;i<nHeight;i++)
		memcpy(m_spTransData1+i*nWidth,m_spTransData1[i],sizeof(float)*nWidth);

	//指针空间的释放
	for (int i=0;i<nHeight;i++)
	{
		delete[]m_spTransData1[i];
		delete[]m_spOriginData[i];
	}
	delete[]m_spTransData1;
	delete[]m_spOriginData;
}


/********************************************************************************
*函数描述：	DWT_Once完成一次图像的小波变换										*
*函数参数：	float  **spOriginData：二维指针，指向原始的图像数据					*
*			float  **spTransData0：小波变换系数，存放一次水平变换后的小波系数	*
*			float  **spTransData1：小波变换系数，存放一次竖直变换后的小波系数	*
*			int   nHeight		：图像属性参数，数值为原始图像的高度值			*
*			int	  nHeight_H		：图像属性参数，数值为原始图像高度值的一半		*
*			int   nWidth		：图像属性参数，数值为原始图像的宽度值			*
*			int	  nWidth_H		：图像属性参数，数值为原始图像宽度值的一半		*
*			int   layer			：小波变换的层数，数值为1层						*
*			float fRadius		：小波变换因子，在调用时候已指定数值为1.414		*
********************************************************************************/
void DWT_Once(float ** spOriginData, float ** spTransData0, float ** spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	int Trans_W,				//图像扫描线控制：横坐标
		Trans_H,				//图像扫描线控制：纵坐标
		Trans_M,				//图像矩阵的横坐标
		Trans_N;				//图像矩阵的纵坐标
	float  Trans_Coeff0;		//小波变换系数
	float  Trans_Coeff1;
	fRadius=1.414f;				//变换滤波系数
	//本模块完成变换系数的赋值采样
	//行变换,第一次（layer=1时）时nHeight即为原始图像的高度值
	for(Trans_H=0; Trans_H<nHeight; Trans_H++)            
	{
		if(layer == 1)
			//layer=1时，nWidth_H为原始图像宽度值的一半
			for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)          
			{
				Trans_W=Trans_N<<1;
				if (fRadius==2)
				{
					spTransData0[Trans_H][Trans_N] = (spOriginData[Trans_H][Trans_W]);
					spTransData0[Trans_H][nWidth_H+Trans_N] = (spOriginData[Trans_H][Trans_W+1]);
				}
				else
				{
					spTransData0[Trans_H][Trans_N] = (spOriginData[Trans_H][Trans_W]-128);		
					spTransData0[Trans_H][nWidth_H+Trans_N] = (spOriginData[Trans_H][Trans_W+1]-128);	
				}
			}
			//若变换层数大于1,则仅采样低频的小波系数
			if(layer > 1)
				for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)
				{
					Trans_W=Trans_N*2;
					spTransData0[Trans_H][Trans_N] = spTransData1[Trans_H][Trans_W];
					spTransData0[Trans_H][nWidth_H+Trans_N] = spTransData1[Trans_H][Trans_W+1];
				}
	}
	for(Trans_H=0; Trans_H<nHeight; Trans_H++)
	{
		for(Trans_N=0; Trans_N<nWidth_H-1; Trans_N++)
		{
			//奇偶数值和的一半
			Trans_Coeff1 = ((spTransData0[Trans_H][Trans_N]+spTransData0[Trans_H][Trans_N+1])/2);	
			//逻辑非操作后数值加1
			Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;	
			//系数预测
			spTransData0[Trans_H][nWidth_H+Trans_N] = spTransData0[Trans_H][nWidth_H+Trans_N]+Trans_Coeff1;	
		}
		//完成一个偶系数的边界处理
		Trans_Coeff1 = ((spTransData0[Trans_H][nWidth_H-1]+spTransData0[Trans_H][nWidth_H-2])/2);
		Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;
		spTransData0[Trans_H][nWidth-1] = spTransData0[Trans_H][nWidth-1]+Trans_Coeff1;
		//完成一个奇系数的边界处理
		Trans_Coeff0 = ((spTransData0[Trans_H][nWidth_H]+spTransData0[Trans_H][nWidth_H+1])/4);
		spTransData0[Trans_H][0] = spTransData0[Trans_H][0]+Trans_Coeff0;
		//提升，整数到整数的变换
		for(Trans_N=1; Trans_N<nWidth_H; Trans_N++)
		{
			Trans_Coeff0 = ((spTransData0[Trans_H][nWidth_H+Trans_N]+spTransData0[Trans_H][nWidth_H+Trans_N-1])/4);
			spTransData0[Trans_H][Trans_N] = spTransData0[Trans_H][Trans_N]+Trans_Coeff0;
		}

	}//水平方向的变换结束
	//竖直方向的变换开始，数据源未水平变换后的小波系数
	for(Trans_M=0; Trans_M<nHeight; Trans_M++)
	{
		for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)
		{
			spTransData0[Trans_M][Trans_N]*=fRadius;
			spTransData0[Trans_M][Trans_N+nWidth_H]/=fRadius;
		}
	}
	//行提升后的数据在spTransData0中，spTransData0中的数据自然奇偶有序
	for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)
	{
		//列变换
		for(Trans_M=0; Trans_M<nHeight_H; Trans_M++)
		{
			Trans_H =Trans_M<<1;
			//频带LL部分
			spTransData1[Trans_M][Trans_N] = spTransData0[Trans_H][Trans_N];
			//频带HL部分
			spTransData1[nHeight_H+Trans_M][Trans_N] = spTransData0[Trans_H+1][Trans_N];
			//频带LH部分
			spTransData1[Trans_M][nWidth_H+Trans_N] = spTransData0[Trans_H][nWidth_H+Trans_N];	
			//频带HH部分
			spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N] = spTransData0[Trans_H+1][nWidth_H+Trans_N];
		}
		//第一次提升奇数坐标系数
		for(Trans_M=0; Trans_M<nHeight_H-1; Trans_M++)
		{
			//竖直方向的变换 
			Trans_Coeff1 = ((spTransData1[Trans_M][Trans_N]+spTransData1[Trans_M+1][Trans_N])/2);
			Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;
			spTransData1[nHeight_H+Trans_M][Trans_N] = spTransData1[nHeight_H+Trans_M][Trans_N]+Trans_Coeff1;
			Trans_Coeff1 = ((spTransData1[Trans_M][nWidth_H+Trans_N]+spTransData1[Trans_M+1][nWidth_H+Trans_N])/2);
			Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;
			spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N] = spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N]+Trans_Coeff1;
		}
		Trans_Coeff1 = ((spTransData1[nHeight_H-1][Trans_N]+spTransData1[nHeight_H-2][Trans_N])/2);
		Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;
		spTransData1[nHeight-1][Trans_N] = spTransData1[nHeight-1][Trans_N]+Trans_Coeff1;
		Trans_Coeff1 = ((spTransData1[nHeight_H-1][nWidth_H+Trans_N]+spTransData1[nHeight_H-2][nWidth_H+Trans_N])/2);
		Trans_Coeff1=-Trans_Coeff1/*~Trans_Coeff1+1*/;
		//边界处理
		spTransData1[nHeight-1][nWidth_H+Trans_N] = spTransData1[nHeight-1][nWidth_H+Trans_N]+Trans_Coeff1;

		Trans_Coeff0 = ((spTransData1[nHeight_H][Trans_N]+spTransData1[nHeight_H+1][Trans_N])/4);
		spTransData1[0][Trans_N] = spTransData1[0][Trans_N]+Trans_Coeff0;
		Trans_Coeff0 = ((spTransData1[nHeight_H][nWidth_H+Trans_N]+spTransData1[nHeight_H+1][nWidth_H+Trans_N])/4);
		//边界处理
		spTransData1[0][nWidth_H+Trans_N] = spTransData1[0][nWidth_H+Trans_N]+Trans_Coeff0;
		//第一次提升偶数坐标系数
		for(Trans_M=1; Trans_M<nHeight_H; Trans_M++)
		{
			Trans_Coeff0 = ((spTransData1[nHeight_H+Trans_M][Trans_N]+spTransData1[nHeight_H+Trans_M-1][Trans_N])/4);
			spTransData1[Trans_M][Trans_N] = spTransData1[Trans_M][Trans_N]+Trans_Coeff0;
			Trans_Coeff0 = ((spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N]+spTransData1[nHeight_H+Trans_M-1][nWidth_H+Trans_N])/4);
			spTransData1[Trans_M][nWidth_H+Trans_N] = spTransData1[Trans_M][nWidth_H+Trans_N]+Trans_Coeff0;
		}
	}
	//存放小波系数，LL频带的系数进行幅值增强处理，其它高频频带的系数则削弱其幅值
	for(Trans_N=0; Trans_N<nWidth; Trans_N++)
	{
		for(Trans_M=0; Trans_M<nHeight_H; Trans_M++)
		{
			spTransData1[Trans_M][Trans_N]*=fRadius;
			spTransData1[Trans_M+nHeight_H][Trans_N]/=fRadius;
		}
	}
}

//转换为一维指针
void DWT_Once(float *  spOriginData, float *  spTransData0, float *  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	//检查输入
	if (spOriginData==NULL||spTransData0==NULL||spTransData1==NULL)
		return ;
	if (nHeight<=0||nWidth<=0||nHeight_H<=0||nWidth_H<=0)
		return ;

	float** m_spOriginData;
	float** m_spTransData0;
	float** m_spTransData1;

	//指针空间的申请
	m_spOriginData=new float*[nHeight];
	m_spTransData0=new float*[nHeight];
	m_spTransData1=new float*[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		m_spOriginData[i]=new float [nWidth];
		m_spTransData0[i]=new float [nWidth];
		m_spTransData1[i]=new float [nWidth];

		memcpy(m_spOriginData[i],spOriginData+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData0[i],spTransData0+i*nWidth,sizeof(float)*nWidth)  ;
		memcpy(m_spTransData1[i],spTransData1+i*nWidth,sizeof(float)*nWidth)  ;
	}

	//调用二维指针函数
	DWT_Once(m_spOriginData,m_spTransData0,m_spTransData1,nHeight,nHeight_H,nWidth,nWidth_H,layer,fRadius);

	for (int i=0;i<nHeight;i++)
	{
		memcpy(spTransData0+i*nWidth,m_spTransData0[i],sizeof(float)*nWidth);
		memcpy(spTransData1+i*nWidth,m_spTransData1[i],sizeof(float)*nWidth);
	}

	//指针空间的释放
	for (int i=0;i<nHeight;i++)
	{
		delete[]m_spOriginData[i];
		delete[]m_spTransData0[i];
		delete[]m_spTransData1[i];
	}
	delete[]m_spOriginData;
	delete[]m_spTransData0;
	delete[]m_spTransData1;
}


/********************************************************************************
*函数描述：	DWT_TwoLayers完成两次图像的小波变换									*	
*函数参数：	float  **spOriginData：二维指针，指向原始的图像数据					*
*			float  **spTransData0：小波变换系数，存放一次水平变换后的小波系数	*
*			float  **spTransData1：小波变换系数，存放一次竖直变换后的小波系数	*
*			int   nHeight		：图像属性参数，数值为原始图像的高度值			*
*			int	  nHeight_H		：图像属性参数，数值为原始图像高度值的一半		*
*			int   nWidth		：图像属性参数，数值为原始图像的宽度值			*
*			int	  nWidth_H		：图像属性参数，数值为原始图像宽度值的一半		*
*			int   layer			：小波变换的层数，数值为2层						*
*			float fRadius		：小波变换因子，在调用时候已指定数值为1.414		*
********************************************************************************/
void DWT_TwoLayers(float ** spOriginData, float ** spTransData0, float ** spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	int i;
	float  **pData, **pTran0, **pTran1;
	//图像的属性参数
	int iWidth, iHeight, iWidth_H, iHeight_H;
	float fr = fRadius;
	//获得数据空间的指针
	pData = spOriginData;
	pTran0 = spTransData0;
	pTran1 = spTransData1;
	//图像属性参数赋值
	iWidth = nWidth;	iWidth_H = nWidth_H;
	iHeight = nHeight;	iHeight_H = nHeight_H;
	//利用循环完成两次小波变换
	for(i=1; i<=layer; i++)
	{
		DWT_Once(pData,pTran0,pTran1,iHeight,iHeight_H,iWidth,iWidth_H,i,fr);
		iHeight=iHeight/2;		iWidth=iWidth/2;
		iHeight_H=iHeight/2;	iWidth_H=iWidth/2;
	}
}

//转换为一维指针
void DWT_TwoLayers(float *  spOriginData, float *  spTransData0, float *  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	//检查输入
	if (spOriginData==NULL||spTransData0==NULL||spTransData1==NULL)
		return ;
	if (nHeight<=0||nWidth<=0||nHeight_H<=0||nWidth_H<=0)
		return ;

	float** m_spOriginData;
	float** m_spTransData0;
	float** m_spTransData1;

	//指针空间的申请
	m_spOriginData=new float*[nHeight];
	m_spTransData0=new float*[nHeight];
	m_spTransData1=new float*[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		m_spOriginData[i]=new float[nWidth];
		m_spTransData0[i]=new float[nWidth];
		m_spTransData1[i]=new float[nWidth];

		memcpy(m_spOriginData[i],spOriginData+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData0[i],spTransData0+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData1[i],spTransData1+i*nWidth,sizeof(float)*nWidth);
	}

	//调用二维指针函数
	DWT_TwoLayers(m_spOriginData,m_spTransData0,m_spTransData1,nHeight,nHeight_H,nWidth,nWidth_H,layer,fRadius);

	for (int i=0;i<nHeight;i++)
	{
		memcpy(spTransData0+i*nWidth,m_spTransData0[i],sizeof(float)*nWidth);
		memcpy(spTransData1+i*nWidth,m_spTransData1[i],sizeof(float)*nWidth);
	}

	//指针空间的释放
	for (int i=0;i<nHeight;i++)
	{
		delete[]m_spOriginData[i];
		delete[]m_spTransData0[i];
		delete[]m_spTransData1[i];
	}
	delete[]m_spOriginData;
	delete[]m_spTransData0;
	delete[]m_spTransData1;
}


/********************************************************************************
*函数描述：	DWT_TriLayers完成三次图像的小波变换									*	
*函数参数：	float  **spOriginData：二维指针，指向原始的图像数据					*
*			float  **spTransData0：小波变换系数，存放一次水平变换后的小波系数	*
*			float  **spTransData1：小波变换系数，存放一次竖直变换后的小波系数	*
*			int   nHeight		：图像属性参数，数值为原始图像的高度值			*
*			int	  nHeight_H		：图像属性参数，数值为原始图像高度值的一半		*
*			int   nWidth		：图像属性参数，数值为原始图像的宽度值			*
*			int	  nWidth_H		：图像属性参数，数值为原始图像宽度值的一半		*
*			int   layer			：小波变换的层数,数值为3层						*
*			float fRadius		：小波变换因子，在调用时候已指定数值为1.414		*
********************************************************************************/
void DWT_TriLayers(float ** spOriginData, float ** spTransData0, float ** spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	int i;
	float  **pData, **pTran0, **pTran1;
	//图像的属性参数
	int iWidth, iHeight, iWidth_H, iHeight_H;
	float fr = fRadius;
	//获得数据空间的指针
	pData = spOriginData;
	pTran0 = spTransData0;
	pTran1 = spTransData1;
	//图像属性参数赋值
	iWidth = nWidth;	iWidth_H = nWidth_H;
	iHeight = nHeight;	iHeight_H = nHeight_H;
	//利用循环完成两次小波变换
	for(i=1; i<=layer; i++)
	{
		DWT_Once(pData,pTran0,pTran1,iHeight,iHeight_H,iWidth,iWidth_H,i,fr);
		iHeight=iHeight/2;		iWidth=iWidth/2;
		iHeight_H=iHeight/2;	iWidth_H=iWidth/2;
	}
}

//转换为一维指针
void DWT_TriLayers(float *  spOriginData, float *  spTransData0, float *  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	//检查输入
	if (spOriginData==NULL||spTransData0==NULL||spTransData1==NULL)
		return ;
	if (nHeight<=0||nWidth<=0||nHeight_H<=0||nWidth_H<=0)
		return ;

	float** m_spOriginData;
	float** m_spTransData0;
	float** m_spTransData1;

	//指针空间的申请
	m_spOriginData=new float*[nHeight];
	m_spTransData0=new float*[nHeight];
	m_spTransData1=new float*[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		m_spOriginData[i]=new float[nWidth];
		m_spTransData0[i]=new float[nWidth];
		m_spTransData1[i]=new float[nWidth];

		memcpy(m_spOriginData[i],spOriginData+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData0[i],spTransData0+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData1[i],spTransData1+i*nWidth,sizeof(float)*nWidth);
	}

	//调用二维指针函数
	DWT_TriLayers(m_spOriginData,m_spTransData0,m_spTransData1,nHeight,nHeight_H,nWidth,nWidth_H,layer,fRadius);

	for (int i=0;i<nHeight;i++)
	{
		memcpy(spTransData0+i*nWidth,m_spTransData0[i],sizeof(float)*nWidth);
		memcpy(spTransData1+i*nWidth,m_spTransData1[i],sizeof(float)*nWidth);
	}

	//指针空间的释放
	for (int i=0;i<nHeight;i++)
	{
		delete[]m_spOriginData[i];
		delete[]m_spTransData0[i];
		delete[]m_spTransData1[i];
	}
	delete[]m_spOriginData;
	delete[]m_spTransData0;
	delete[]m_spTransData1;
}


/********************************************************************************
*函数描述：	DWTi_Once完成一次图像小波变换的逆变换								*	
*函数参数：	float  **spData		：二维指针，其数据空间存放小波逆变换后的图像数据*
*			float  **spTransData0：小波变换系数，存放一次水平变换后的小波系数	*
*			float  **spTransData1：小波变换系数，存放一次竖直变换后的小波系数	*
*			int   nHeight		：图像属性参数，数值为原始图像的高度值			*
*			int	  nHeight_H		：图像属性参数，数值为原始图像高度值的一半		*
*			int   nWidth		：图像属性参数，数值为原始图像的宽度值			*
*			int	  nWidth_H		：图像属性参数，数值为原始图像宽度值的一半		*
*			int   layer			：小波变换的层数,数值为3层						*
*			float fRadius		：小波变换因子，在调用时候已指定数值为1.414		*
********************************************************************************/
void DWTi_Once(float  **spData, float  **spTransData0, float  **spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	int Trans_W,				//图像扫描线控制：横坐标
		Trans_H,				//图像扫描线控制：纵坐标
		Trans_M,				//图像矩阵的横坐标
		Trans_N;				//图像矩阵的纵坐标

	float  WvltCoeff0;			//小波变换系数
	float  WvltCoeff1;

	for(Trans_N=0; Trans_N<nWidth; Trans_N++)
	{
		for(Trans_M=0; Trans_M<nHeight_H; Trans_M++)
		{
			spTransData1[Trans_M][Trans_N]/=fRadius;
			spTransData1[Trans_M+nHeight_H][Trans_N]*=fRadius;
		}
	}
	//逆变换是一个逆过程，所以现从竖直方向开始
	//竖直方向的逆变换（此时自然奇偶有序排列），Trans_N为图像矩阵的纵坐标
	for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)
	{
		//偶数坐标的小波系数逆变换时的边界处理
		WvltCoeff0 = ((spTransData1[nHeight_H][Trans_N]+spTransData1[nHeight_H+1][Trans_N])/4);
		spTransData1[0][Trans_N] = spTransData1[0][Trans_N]-WvltCoeff0;
		WvltCoeff0 = ((spTransData1[nHeight_H][nWidth_H+Trans_N]+spTransData1[nHeight_H+1][nWidth_H+Trans_N])/4);
		spTransData1[0][nWidth_H+Trans_N] = spTransData1[0][nWidth_H+Trans_N]-WvltCoeff0;
		//竖直方向逆变换的第二次偶数坐标小波系数的逆变换
		for(Trans_M=1; Trans_M<nHeight_H; Trans_M++)
		{
			WvltCoeff0 = ((spTransData1[nHeight_H+Trans_M][Trans_N]+spTransData1[nHeight_H+Trans_M-1][Trans_N])/4);
			spTransData1[Trans_M][Trans_N] = spTransData1[Trans_M][Trans_N]-WvltCoeff0;
			WvltCoeff0 = ((spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N]+spTransData1[nHeight_H+Trans_M-1][nWidth_H+Trans_N])/4);
			spTransData1[Trans_M][nWidth_H+Trans_N] = spTransData1[Trans_M][nWidth_H+Trans_N]-WvltCoeff0;
		}
		//第二次奇数坐标小波系数的逆变换
		for(Trans_M=0; Trans_M<nHeight_H-1; Trans_M++)
		{
			WvltCoeff1 = ((spTransData1[Trans_M][Trans_N]+spTransData1[Trans_M+1][Trans_N])/2);
			WvltCoeff1=-WvltCoeff1/*~WvltCoeff1+1*/;
			spTransData1[nHeight_H+Trans_M][Trans_N] = spTransData1[nHeight_H+Trans_M][Trans_N]-WvltCoeff1;
			WvltCoeff1 = ((spTransData1[Trans_M][nWidth_H+Trans_N]+spTransData1[Trans_M+1][nWidth_H+Trans_N])/2);
			WvltCoeff1=-WvltCoeff1/*~WvltCoeff1+1*/;
			spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N] = spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N]-WvltCoeff1;
		}
		//奇数坐标小波系数逆变换时的边界处理
		WvltCoeff1 = ((spTransData1[nHeight_H-1][Trans_N]+spTransData1[nHeight_H-2][Trans_N])/2);
		WvltCoeff1=-WvltCoeff1/*~WvltCoeff1+1*/;
		spTransData1[nHeight-1][Trans_N] = spTransData1[nHeight-1][Trans_N]-WvltCoeff1;
		WvltCoeff1 = ((spTransData1[nHeight_H-1][nWidth_H+Trans_N]+spTransData1[nHeight_H-2][nWidth_H+Trans_N])/2);
		WvltCoeff1=-WvltCoeff1/*~WvltCoeff1+1*/;
		spTransData1[nHeight-1][nWidth_H+Trans_N] = spTransData1[nHeight-1][nWidth_H+Trans_N]-WvltCoeff1;
		
		//本模块完成变换系数的赋值采样的逆操作
		for(Trans_M=0; Trans_M<nHeight_H; Trans_M++)
		{
			Trans_H =Trans_M<<1;
			spTransData0[Trans_H][Trans_N] = spTransData1[Trans_M][Trans_N];
			spTransData0[Trans_H+1][Trans_N] = spTransData1[nHeight_H+Trans_M][Trans_N];
			spTransData0[Trans_H][nWidth_H+Trans_N] = spTransData1[Trans_M][nWidth_H+Trans_N];
			spTransData0[Trans_H+1][nWidth_H+Trans_N]= spTransData1[nHeight_H+Trans_M][nWidth_H+Trans_N];
		}
	}

	//去除小波变换中频带系数的滤波影像
	for(Trans_M=0; Trans_M<nHeight; Trans_M++)
	{
		for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)
		{
			spTransData0[Trans_M][Trans_N]/=fRadius;
			spTransData0[Trans_M][Trans_N+nWidth_H]*=fRadius;
		}
	}
	//下面进行水平方向的逆变换
	//水平方向的逆变换
	for(Trans_H=0; Trans_H<nHeight; Trans_H++)
	{
		//偶数坐标小波系数逆变换时的边界处理
		WvltCoeff0 = ((spTransData0[Trans_H][nWidth_H]+spTransData0[Trans_H][nWidth_H+1])/4);
		spTransData0[Trans_H][0] = spTransData0[Trans_H][0]-WvltCoeff0;
		//第二次偶数坐标小波系数的逆变换
		for(Trans_N=1; Trans_N<nWidth_H; Trans_N++)
		{
			WvltCoeff0 = ((spTransData0[Trans_H][nWidth_H+Trans_N]+spTransData0[Trans_H][nWidth_H+Trans_N-1])/4);
			spTransData0[Trans_H][Trans_N] = spTransData0[Trans_H][Trans_N]-WvltCoeff0;
		}
		//第二次奇数坐标小波系数的逆变换
		for(Trans_N=0; Trans_N<nWidth_H-1; Trans_N++)
		{
			WvltCoeff1 = ((spTransData0[Trans_H][Trans_N]+spTransData0[Trans_H][Trans_N+1])/2);
			WvltCoeff1=-WvltCoeff1/*~WvltCoeff1+1*/;
			spTransData0[Trans_H][nWidth_H+Trans_N] = spTransData0[Trans_H][nWidth_H+Trans_N]-WvltCoeff1;
		}
		//奇数坐标小波系数逆变换时的边界处理
		WvltCoeff1 = ((spTransData0[Trans_H][nWidth_H-1]+spTransData0[Trans_H][nWidth_H-2])/2);
		WvltCoeff1=-WvltCoeff1/*~WvltCoeff1+1*/;
		spTransData0[Trans_H][nWidth-1] = spTransData0[Trans_H][nWidth-1]-WvltCoeff1;

		if(layer > 1)
		{
			for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)
			{
				Trans_W =Trans_N<<1;
				spTransData1[Trans_H][Trans_W] = spTransData0[Trans_H][Trans_N];
				spTransData1[Trans_H][Trans_W+1] = spTransData0[Trans_H][nWidth_H+Trans_N];
			}
		}
		if(layer == 1)
		{
			for(Trans_N=0; Trans_N<nWidth_H; Trans_N++)
			{
				Trans_W =Trans_N<<1;
				if(fRadius!=2)
				{
					spTransData0[Trans_H][Trans_N]=spTransData0[Trans_H][Trans_N]+128;
					spTransData0[Trans_H][nWidth_H+Trans_N]=spTransData0[Trans_H][nWidth_H+Trans_N]+128;
				}
				////if(spTransData0[Trans_H][Trans_N]>255) 
				////	spTransData0[Trans_H][Trans_N]=255;
				////if(spTransData0[Trans_H][Trans_N]<0)   
				////	spTransData0[Trans_H][Trans_N]=0;
				////if(spTransData0[Trans_H][nWidth_H+Trans_N]>255) 
				////	spTransData0[Trans_H][nWidth_H+Trans_N]=255;
				////if(spTransData0[Trans_H][nWidth_H+Trans_N]<0)   
				////	spTransData0[Trans_H][nWidth_H+Trans_N]=0;
				//spData[Trans_H][Trans_W] = (un char)spTransData0[Trans_H][Trans_N];
				spData[Trans_H][Trans_W] = spTransData0[Trans_H][Trans_N];
				//spData[Trans_H][Trans_W+1] = (un char)spTransData0[Trans_H][nWidth_H+Trans_N];
				spData[Trans_H][Trans_W+1] =spTransData0[Trans_H][nWidth_H+Trans_N];
			}
		}

	}

}

//将二维指针转换为一维指针
void DWTi_Once(float*   spData, float*   spTransData0, float*  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius)
{
	//检查输入
	if (spData==NULL||spTransData0==NULL||spTransData1==NULL)
		return ;
	if (nHeight<=0||nWidth<=0||nHeight_H<=0||nWidth_H<=0)
		return ;

	float** m_spData;
	float** m_spTransData0;
	float** m_spTransData1;

	//指针空间的申请
	m_spData=new float*[nHeight];
	m_spTransData0=new float*[nHeight];
	m_spTransData1=new float*[nHeight];
	for (int i=0;i<nWidth;i++)
	{
		m_spData[i]=new float [nWidth];
		m_spTransData0[i]=new float[nWidth];
		m_spTransData1[i]=new float[nWidth];

		memcpy(m_spData[i],spData+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData0[i],spTransData0+i*nWidth,sizeof(float)*nWidth);
		memcpy(m_spTransData1[i],spTransData1+i*nWidth,sizeof(float)*nWidth);
	}

	//调用二维指针函数
	DWTi_Once(m_spData,m_spTransData0,m_spTransData1,nHeight,nHeight_H,nWidth,nWidth_H,layer,fRadius);

	for (int i=0;i<nWidth;i++)
	{
		memcpy(spTransData0+i*nWidth,m_spTransData0[i],sizeof(float)*nWidth);
		memcpy(spTransData1+i*nWidth,m_spTransData1[i],sizeof(float)*nWidth);
		memcpy(spData+i*nWidth,m_spData[i],sizeof(float)*nWidth);
	}

	//指针空间的释放
	for (int i=0;i<nHeight;i++)
	{
		delete[]m_spData[i];
		delete[]m_spTransData0[i];
		delete[]m_spTransData1[i];
	}
	delete[]m_spData;
	delete[]m_spTransData0;
	delete[]m_spTransData1;
}


//////////////////////////////////////////////////////////////////////////
/********************************************************************************
*函数描述：	WaveletTransOnce,WaveletTransTwice,WaveletTransTri完成图像的小波变换*	
*函数参数：	char* pathSrc       ：输入小波变换影像的路径     					*
*			char* pathDst1		：水平方向小波变换系数输出路径					*
*			char* pathDst2		：竖直方向小波变换系数输出路径					*
*			int band			：处理影像的第几个波段							*
********************************************************************************/
void WaveletTransOnce(char* pathSrc,char* pathDst1,char* pathDst2,int band)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst1,m_datasetdst2;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);
	if (band>bands)
		return ;
	
	float* m_Indata =new float[width*heigh];
	float* m_Outdata1=new float[width*heigh];
	float* m_Outdata2=new float[width*heigh];

	GDALRasterBandH m_band;
	//读数据
	m_band=GDALGetRasterBand(m_datasetsrc,band);
	GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata,width,heigh,GDT_Float32,0,0);
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	DWT_Once(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,1,1.414f);
	//写数据
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst1=GDALCreate(GDALGetDriverByName("GTiff"),pathDst1,width,heigh,1,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst1,proj);
	m_band=GDALGetRasterBand(m_datasetdst1,1);
	GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata1,width,heigh,GDT_Float32,0,0);

	//写数据
	m_datasetdst2=GDALCreate(GDALGetDriverByName("GTiff"),pathDst2,width,heigh,1,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst2,proj);
	m_band=GDALGetRasterBand(m_datasetdst2,1);
	GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata2,width,heigh,GDT_Float32,0,0);


	GDALClose(m_datasetdst1);
	GDALClose(m_datasetdst2);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata1;
	delete[]m_Outdata2;
}
void WaveletTransOnce(char* pathSrc,char* pathDst1,char* pathDst2)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst1,m_datasetdst2;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);

	float* m_Indata =new float[width*heigh];
	float* m_Outdata1=new float[width*heigh];
	float* m_Outdata2=new float[width*heigh];

	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst1=GDALCreate(GDALGetDriverByName("GTiff"),pathDst1,width,heigh,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst1,proj);
	m_datasetdst2=GDALCreate(GDALGetDriverByName("GTiff"),pathDst2,width,heigh,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst2,proj);

	GDALRasterBandH m_band;
	for (int i=0;i<bands;i++)
	{
		m_band=GDALGetRasterBand(m_datasetsrc,i+1);
		GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata,width,heigh,GDT_Float32,0,0);
		DWT_Once(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,1,1.414f);

		m_band=GDALGetRasterBand(m_datasetdst1,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata1,width,heigh,GDT_Float32,0,0);

		m_band=GDALGetRasterBand(m_datasetdst2,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata2,width,heigh,GDT_Float32,0,0);
	}

	GDALClose(m_datasetdst1);
	GDALClose(m_datasetdst2);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata1;
	delete[]m_Outdata2;
}

//二次小波变换
void WaveletTransTwice(char* pathSrc,char* pathDst1,char* pathDst2,int band)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst1,m_datasetdst2;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);
	float* m_Indata =new float[width*heigh];
	if (band>bands)
		return ;

	float* m_Outdata1=new float[width*heigh];
	float* m_Outdata2=new float[width*heigh];

	GDALRasterBandH m_band;
	//读数据
	m_band=GDALGetRasterBand(m_datasetsrc,band);
	GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata,width,heigh,GDT_Float32,0,0);
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	//DWT_Once(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,1,1.414);
	DWT_TwoLayers(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,2,1.414f);
	//写数据
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst1=GDALCreate(GDALGetDriverByName("GTiff"),pathDst1,width,heigh,1,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst1,proj);
	m_band=GDALGetRasterBand(m_datasetdst1,1);
	GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata1,width,heigh,GDT_Float32,0,0);

	//写数据
	m_datasetdst2=GDALCreate(GDALGetDriverByName("GTiff"),pathDst2,width,heigh,1,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst2,proj);
	m_band=GDALGetRasterBand(m_datasetdst2,1);
	GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata2,width,heigh,GDT_Float32,0,0);


	GDALClose(m_datasetdst1);
	GDALClose(m_datasetdst2);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata1;
	delete[]m_Outdata2;

}
void WaveletTransTwice(char* pathSrc,char* pathDst1,char* pathDst2)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst1,m_datasetdst2;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);

	float* m_Indata =new float[width*heigh];
	float* m_Outdata1=new float[width*heigh];
	float* m_Outdata2=new float[width*heigh];

	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst1=GDALCreate(GDALGetDriverByName("GTiff"),pathDst1,width,heigh,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst1,proj);
	m_datasetdst2=GDALCreate(GDALGetDriverByName("GTiff"),pathDst2,width,heigh,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst2,proj);

	GDALRasterBandH m_band;
	for (int i=0;i<bands;i++)
	{
		m_band=GDALGetRasterBand(m_datasetsrc,i+1);
		GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata,width,heigh,GDT_Float32,0,0);
		DWT_TwoLayers(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,2,1.414f);

		m_band=GDALGetRasterBand(m_datasetdst1,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata1,width,heigh,GDT_Float32,0,0);

		m_band=GDALGetRasterBand(m_datasetdst2,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata2,width,heigh,GDT_Float32,0,0);
	}

	GDALClose(m_datasetdst1);
	GDALClose(m_datasetdst2);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata1;
	delete[]m_Outdata2;
}

//三次小波变换
void WaveletTransTri(char* pathSrc,char* pathDst1,char* pathDst2,int band)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst1,m_datasetdst2;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);
	if (band>bands)
		return ;

	float* m_Indata =new float[width*heigh];
	float* m_Outdata1=new float[width*heigh];
	float* m_Outdata2=new float[width*heigh];

	GDALRasterBandH m_band;
	//读数据
	m_band=GDALGetRasterBand(m_datasetsrc,band);
	GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata,width,heigh,GDT_Float32,0,0);
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	//DWT_Once(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,1,1.414);
	//DWT_TwoLayers(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,2,1.414);
	DWT_TriLayers(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,3,1.414f);
	
	//写数据
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst1=GDALCreate(GDALGetDriverByName("GTiff"),pathDst1,width,heigh,1,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst1,proj);
	m_band=GDALGetRasterBand(m_datasetdst1,1);
	GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata1,width,heigh,GDT_Float32,0,0);

	//写数据
	m_datasetdst2=GDALCreate(GDALGetDriverByName("GTiff"),pathDst2,width,heigh,1,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst2,proj);
	m_band=GDALGetRasterBand(m_datasetdst2,1);
	GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata2,width,heigh,GDT_Float32,0,0);


	GDALClose(m_datasetdst1);
	GDALClose(m_datasetdst2);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata1;
	delete[]m_Outdata2;
}
void WaveletTransTri(char* pathSrc,char* pathDst1,char* pathDst2)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst1,m_datasetdst2;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);

	float* m_Indata =new float[width*heigh];
	float* m_Outdata1=new float[width*heigh];
	float* m_Outdata2=new float[width*heigh];

	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst1=GDALCreate(GDALGetDriverByName("GTiff"),pathDst1,width,heigh,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst1,proj);
	m_datasetdst2=GDALCreate(GDALGetDriverByName("GTiff"),pathDst2,width,heigh,bands,GDT_Float32,papszOptions);
	GDALSetProjection(m_datasetdst2,proj);

	GDALRasterBandH m_band;
	for (int i=0;i<bands;i++)
	{
		m_band=GDALGetRasterBand(m_datasetsrc,i+1);
		GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata,width,heigh,GDT_Float32,0,0);
		DWT_TriLayers(m_Indata,m_Outdata1,m_Outdata2,heigh,heigh/2,width,width/2,3,1.414f);

		m_band=GDALGetRasterBand(m_datasetdst1,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata1,width,heigh,GDT_Float32,0,0);

		m_band=GDALGetRasterBand(m_datasetdst2,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata2,width,heigh,GDT_Float32,0,0);
	}

	GDALClose(m_datasetdst1);
	GDALClose(m_datasetdst2);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata1;
	delete[]m_Outdata2;
}

/********************************************************************************
*函数描述：	WaveletInvTrans完成一次图像变换的逆变换								*	
*函数参数：	char* pathDst		：输出逆变换后影像路径							*
*			char* pathSrc1		：水平方向上小波变换系数影像路径				*
*			char* pathSrc2		：竖直方向上小波变换系数影像路径				*
*			int layer			：小波逆变换的层数（与小波正变换层数相同复原）	*
*			int band			：小波变换处理的波段信息程序运行时指定为1		*
********************************************************************************/
//小波逆变换
void WaveletInvTrans(char* pathDst,char* pathSrc1,char* pathSrc2,int layer,int band/*=1*/)
{
	GDALAllRegister();
	GDALDatasetH m_datasetdst,m_datasetsrc1,m_datasetsrc2;

	m_datasetsrc1=GDALOpen(pathSrc1,GA_ReadOnly);
	m_datasetsrc2=GDALOpen(pathSrc2,GA_ReadOnly);
	if(m_datasetsrc1==NULL||m_datasetsrc2==NULL)
		return;

	//获取影像数据
	int width=GDALGetRasterXSize(m_datasetsrc1);
	int heigh=GDALGetRasterYSize(m_datasetsrc2);
	int width_h=width/2;
	int heigh_h=heigh/2;
	int iHeigh =/*heigh/4*/heigh/int(pow((double)2,layer-1));
	int iWidth=/*width/4*/width/int(pow((double)2,layer-1));
	int iHeigh_h=/*iHeigh/2*/heigh_h/int(pow((double)2,layer-1));
	int iWidth_h=/*iWidth/2*/width_h/int(pow((double)2,layer-1));

	//空间申请
	float** m_datasrc1=new float*[heigh];
	float** m_datasrc2=new float*[heigh];
	float** m_datadst=new float*[heigh];
	float* datasrc1=new float[heigh*width];
	float* datasrc2=new float[heigh*width];
	float* datadst =new float[heigh*width];

	if (m_datasrc1==NULL||m_datasrc2==NULL||m_datadst==NULL)
		return;
	for (int i=0;i<heigh;i++)
	{
		m_datasrc1[i]=new float[width];
		m_datasrc2[i]=new float[width];
		m_datadst[i] =new float[width];
	}

	GDALRasterBandH m_bandH;
	m_bandH=GDALGetRasterBand(m_datasetsrc1,band);
	GDALRasterIO(m_bandH,GF_Read,0,0,width,heigh,datasrc1,width,heigh,GDT_Float32,0,0);
	m_bandH=GDALGetRasterBand(m_datasetsrc2,band);
	GDALRasterIO(m_bandH,GF_Read,0,0,width,heigh,datasrc2,width,heigh,GDT_Float32,0,0);

	for (int i=0;i<heigh;i++)
	{
		memcpy(m_datasrc1[i],datasrc1+i*width,sizeof(float)*width);
		memcpy(m_datasrc2[i],datasrc2+i*width,sizeof(float)*width);
	}

	//创建数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,1,GDT_Float32,papszOptions);
	if(m_datasetdst==NULL)
		return;

	for(int i=layer;i>=1;i--)
	{
		DWTi_Once(m_datadst,m_datasrc1,m_datasrc2,iHeigh,iHeigh_h,iWidth,iWidth_h,i,1.414f/*2.0f*//*1.731*/);
		iHeigh*=2;		  iWidth*=2;
		iHeigh_h=iHeigh_h*2;iWidth_h=iWidth_h*2;
	}

	for (int i=0;i<heigh;i++)
		memcpy(datadst+i*width,m_datadst[i],sizeof(float)*width);

	m_bandH=GDALGetRasterBand(m_datasetdst,band);
	GDALRasterIO(m_bandH,GF_Write,0,0,width,heigh,datadst,width,heigh,GDT_Float32,0,0);
	//删除指针

	GDALClose(m_datasetdst );
	GDALClose(m_datasetsrc1);
	GDALClose(m_datasetsrc2);

	for (int i=0;i<heigh;i++)
	{
		delete[]m_datadst[i] ;
		delete[]m_datasrc1[i];
		delete[]m_datasrc2[i];
	}
	delete[]m_datadst ;
	delete[]m_datasrc1;
	delete[]m_datasrc2;

	delete[]datadst;
	delete[]datasrc1;
	delete[]datasrc2;
}
void WaveletInvTrans(char* pathDst,char* pathSrc1,char* pathSrc2,int layer)
{
	GDALAllRegister();
	GDALDatasetH m_datasetdst,m_datasetsrc1,m_datasetsrc2;

	m_datasetsrc1=GDALOpen(pathSrc1,GA_ReadOnly);
	m_datasetsrc2=GDALOpen(pathSrc2,GA_ReadOnly);
	if(m_datasetsrc1==NULL||m_datasetsrc2==NULL)
		return;

	//获取影像数据
	int width=GDALGetRasterXSize(m_datasetsrc1);
	int heigh=GDALGetRasterYSize(m_datasetsrc2);
	int bands1=GDALGetRasterCount(m_datasetsrc1);
	int bands2=GDALGetRasterCount(m_datasetsrc2);
	if(bands2!=bands1)
		return ;

	int width_h=width/2;
	int heigh_h=heigh/2;
	int iHeigh =/*heigh/4*/heigh/int(pow((double)2,layer-1));
	int iWidth=/*width/4*/width/int(pow((double)2,layer-1));
	int iHeigh_h=/*iHeigh/2*/heigh_h/int(pow((double)2,layer-1));
	int iWidth_h=/*iWidth/2*/width_h/int(pow((double)2,layer-1));

	//空间申请
	float** m_datasrc1=new float*[heigh];
	float** m_datasrc2=new float*[heigh];
	float** m_datadst=new float*[heigh];
	float* datasrc1=new float[heigh*width];
	float* datasrc2=new float[heigh*width];
	float* datadst =new float[heigh*width];

	if (m_datasrc1==NULL||m_datasrc2==NULL||m_datadst==NULL)
		return;
	for (int i=0;i<heigh;i++)
	{
		m_datasrc1[i]=new float[width];
		m_datasrc2[i]=new float[width];
		m_datadst[i] =new float[width];
	}

	//创建数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,bands1,GDT_Float32,papszOptions);
	if(m_datasetdst==NULL)
		return;

	GDALRasterBandH m_bandH;

	for (int i=0;i<bands1;i++)
	{
		m_bandH=GDALGetRasterBand(m_datasetsrc1,i+1);
		GDALRasterIO(m_bandH,GF_Read,0,0,width,heigh,datasrc1,width,heigh,GDT_Float32,0,0);
		m_bandH=GDALGetRasterBand(m_datasetsrc2,i+1);
		GDALRasterIO(m_bandH,GF_Read,0,0,width,heigh,datasrc2,width,heigh,GDT_Float32,0,0);

		for (int j=0;j<heigh;j++)
		{
			memcpy(m_datasrc1[j],datasrc1+j*width,sizeof(float)*width);
			memcpy(m_datasrc2[j],datasrc2+j*width,sizeof(float)*width);
		}

		for(int j=layer;j>=1;j--)
		{
			DWTi_Once(m_datadst,m_datasrc1,m_datasrc2,iHeigh,iHeigh_h,iWidth,iWidth_h,j,1.414f/*2.0f*//*1.731*/);
			iHeigh*=2;		  iWidth*=2;
			iHeigh_h=iHeigh_h*2;iWidth_h=iWidth_h*2;
		}

		for (int j=0;j<heigh;j++)
			memcpy(datadst+j*width,m_datadst[j],sizeof(float)*width);

		m_bandH=GDALGetRasterBand(m_datasetdst,i+1);
		GDALRasterIO(m_bandH,GF_Write,0,0,width,heigh,datadst,width,heigh,GDT_Float32,0,0);

		for(int j=layer;j>=1;j--)
		{
			iHeigh/=2;		  iWidth/=2;
			iHeigh_h=iHeigh_h/2;iWidth_h=iWidth_h/2;
		}
	}

	GDALClose(m_datasetdst );
	GDALClose(m_datasetsrc1);
	GDALClose(m_datasetsrc2);

	for (int i=0;i<heigh;i++)
	{
		delete[]m_datadst[i] ;
		delete[]m_datasrc1[i];
		delete[]m_datasrc2[i];
	}
	delete[]m_datadst ;
	delete[]m_datasrc1;
	delete[]m_datasrc2;

	delete[]datadst;
	delete[]datasrc1;
	delete[]datasrc2;

}
