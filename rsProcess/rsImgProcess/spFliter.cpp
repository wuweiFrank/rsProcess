#include "spFliter.h"

//============================================================================
// Name        : spFliter.cpp
// Author      : ww
// Version     : 1.0  
// Copyright   : ww
// Description : 影像空间域滤波
//============================================================================
//  [4/9/2014 wuwei just you]


/*
	Version     : 2.0  	
	1.增加影像分块滤波函数
	2.影像分块后多线程处理滤波
	3.增加测试函数测试所有代码
	[2/7/2015 wuwei just you]
*/



/*
	Version     : 2.1  	
	1.增加影像形态学处理
		1.1设置影像形态学滤波核
		1.2影像形态学腐蚀操作
		1.3影像形态学膨胀操作
		1.4影像开、闭运算
    [4/11/2015 wuwei just you]
*/

/********************************************************************************
*函数描述：spFliterCore设置影像滤波核大小和滤波核内容						    *	
*函数参数：float  **core：二维指针，设置的滤波核								*
*		   int   xsize		  ：滤波核的x大小									*
*		   int	 ysize	      ：滤波核的y大小									*
********************************************************************************/
void spFliter::spFliterCore(float** core,int xsize,int ysize)
{
	if (core==NULL)
		return;

	m_core_xsize=xsize;
	m_core_ysize=ysize;

	m_fliter_core=new float *[ysize];
	for (int i=0;i<ysize;i++)
	{
		m_fliter_core[i]=new float [xsize];
		memcpy(m_fliter_core[i],core[i],sizeof(float)*xsize);
	}
	m_is_core=true;
}

//一维指针描述
void spFliter::spFliterCore(float* core,int xsize,int ysize)
{
	if (core==NULL)
		return;

	m_core_xsize=xsize;
	m_core_ysize=ysize;

	m_fliter_core=new float *[ysize];
	for (int i=0;i<ysize;i++)
	{
		m_fliter_core[i]=new float [xsize];
		memcpy(m_fliter_core[i],core+i*xsize,sizeof(float)*xsize);
	}
	m_is_core=true;
}


/********************************************************************************
*函数描述：spitalFliter		  : GDAL封装的影像空域滤波						    *	
*函数参数：char *pathSrc	  ：输入影像路径									*
*		   char *pathDst	  : 输出影像路径									*
*		   int   xpos		  ：模板处理位置x									*
*		   int	 ypos	      ：模板处理位置y									*
*		   int	 xoffset	  ：影像x方向的偏移									*
*		   int	 yoffset	  ：影像y方向的偏移									*
********************************************************************************/
void spFliter::spitalFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset)
{
	//GDAL封装的影像空域滤波
	GDALAllRegister();
	GDALDatasetH m_src_dataset, m_dst_dataset;

	//获取数据
	m_src_dataset=GDALOpen(pathSrc,GA_ReadOnly);
	int width=GDALGetRasterXSize(m_src_dataset);
	int heigh=GDALGetRasterYSize(m_src_dataset);
	int bands=GDALGetRasterCount(m_src_dataset);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_dst_dataset=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,bands,GDT_Float32,papszOptions);
	float *m_datasrc=new float[width*heigh];
	float *m_datadst=new float[width*heigh];

	for (int i=0;i<bands;i++)
	{
		GDALRasterBandH m_band;
		m_band=GDALGetRasterBand(m_src_dataset,i+1);
		GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_datasrc,width,heigh,GDT_Float32,0,0);
		memcpy(m_datadst,m_datasrc,sizeof(float)*width*heigh);
		spFliterOperation(m_datasrc,width,heigh,xpos,ypos,xoffset,yoffset,m_datadst);
		m_band=GDALGetRasterBand(m_dst_dataset,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_datadst,width,heigh,GDT_Float32,0,0);
	}

	const char* proj=GDALGetProjectionRef(m_src_dataset);
	GDALSetProjection(m_dst_dataset,proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_src_dataset,adfGeoTrans);
	GDALSetGeoTransform(m_dst_dataset,adfGeoTrans);

	GDALClose(m_src_dataset);
	GDALClose(m_dst_dataset);
	delete[]m_datadst;
	delete[]m_datasrc;

}
void spFliter::spitalFliter(char* pathSrc,char* pathDst)
{
	//GDAL封装的影像空域滤波
	GDALAllRegister();
	GDALDatasetH m_src_dataset, m_dst_dataset;

	//获取数据
	m_src_dataset=GDALOpen(pathSrc,GA_ReadOnly);
	int width=GDALGetRasterXSize(m_src_dataset);
	int heigh=GDALGetRasterYSize(m_src_dataset);
	int bands=GDALGetRasterCount(m_src_dataset);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_dst_dataset=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,bands,GDT_Float32,papszOptions);
	float *m_datasrc=new float[width*heigh];
	float *m_datadst=new float[width*heigh];

	for (int i=0;i<bands;i++)
	{
		GDALRasterBandH m_band;
		m_band=GDALGetRasterBand(m_src_dataset,i+1);
		GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_datasrc,width,heigh,GDT_Float32,0,0);
		memcpy(m_datadst,m_datasrc,sizeof(float)*width*heigh);
		spFliterOperation(m_datasrc,width,heigh,0,0,width,heigh,m_datadst);
		m_band=GDALGetRasterBand(m_dst_dataset,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_datadst,width,heigh,GDT_Float32,0,0);
	}

	const char* proj=GDALGetProjectionRef(m_src_dataset);
	GDALSetProjection(m_dst_dataset,proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_src_dataset,adfGeoTrans);
	GDALSetGeoTransform(m_dst_dataset,adfGeoTrans);

	GDALClose(m_src_dataset);
	GDALClose(m_dst_dataset);
	delete[]m_datadst;
	delete[]m_datasrc;
}

//具体使用滤波核滤波
//Laplace算子滤波
void spFliter::LaplaceFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=-1.0f;m_fliter_core[0][1]=-1.0f;m_fliter_core[0][2]=-1.0f;
	m_fliter_core[1][0]=-1.0f;m_fliter_core[1][1]=8.0f; m_fliter_core[1][2]=-1.0f;
	m_fliter_core[2][0]=-1.0f;m_fliter_core[2][1]=-1.0f;m_fliter_core[2][2]=-1.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst,xpos,ypos,xoffset,yoffset);

}
void spFliter::LaplaceFliter(char* pathSrc,char* pathDst)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=-1.0f;m_fliter_core[0][1]=-1.0f;m_fliter_core[0][2]=-1.0f;
	m_fliter_core[1][0]=-1.0f;m_fliter_core[1][1]=8.0f; m_fliter_core[1][2]=-1.0f;
	m_fliter_core[2][0]=-1.0f;m_fliter_core[2][1]=-1.0f;m_fliter_core[2][2]=-1.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst);
}

//sobel算子滤波
void spFliter::SobelFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=1.0f;m_fliter_core[0][1]=0.0f;m_fliter_core[0][2]=-1.0f;
	m_fliter_core[1][0]=2.0f;m_fliter_core[1][1]=0.0f; m_fliter_core[1][2]=-2.0f;
	m_fliter_core[2][0]=1.0f;m_fliter_core[2][1]=0.0f;m_fliter_core[2][2]=-1.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst,xpos,ypos,xoffset,yoffset);
}
void spFliter::SobelFliter(char* pathSrc,char* pathDst)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=1.0f;m_fliter_core[0][1]=0.0f;m_fliter_core[0][2]=-1.0f;
	m_fliter_core[1][0]=2.0f;m_fliter_core[1][1]=0.0f; m_fliter_core[1][2]=-2.0f;
	m_fliter_core[2][0]=1.0f;m_fliter_core[2][1]=0.0f;m_fliter_core[2][2]=-1.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst);
}

//均值滤波
void spFliter::MeanFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=1.0f/9.0f;m_fliter_core[0][1]=1.0f/9.0f;m_fliter_core[0][2]=1.0f/9.0f;
	m_fliter_core[1][0]=1.0f/9.0f;m_fliter_core[1][1]=1.0f/9.0f;m_fliter_core[1][2]=1.0f/9.0f;
	m_fliter_core[2][0]=1.0f/9.0f;m_fliter_core[2][1]=1.0f/9.0f;m_fliter_core[2][2]=1.0f/9.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst,xpos,ypos,xoffset,yoffset);
}
void spFliter::MeanFliter(char* pathSrc,char* pathDst)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=1.0f/9.0f;m_fliter_core[0][1]=1.0f/9.0f;m_fliter_core[0][2]=1.0f/9.0f;
	m_fliter_core[1][0]=1.0f/9.0f;m_fliter_core[1][1]=1.0f/9.0f;m_fliter_core[1][2]=1.0f/9.0f;
	m_fliter_core[2][0]=1.0f/9.0f;m_fliter_core[2][1]=1.0f/9.0f;m_fliter_core[2][2]=1.0f/9.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst);
}

//0值滤波
void spFliter::ZeroFliter(char* pathSrc,char* pathDst,int xpos,int ypos,int xoffset,int yoffset)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=0.0f;m_fliter_core[0][1]=0.0f;m_fliter_core[0][2]=0.0f;
	m_fliter_core[1][0]=0.0f;m_fliter_core[1][1]=0.0f;m_fliter_core[1][2]=0.0f;
	m_fliter_core[2][0]=0.0f;m_fliter_core[2][1]=0.0f;m_fliter_core[2][2]=0.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst,xpos,ypos,xoffset,yoffset);
}
void spFliter::ZeroFliter(char* pathSrc,char* pathDst)
{
	m_is_core=true;
	m_fliter_core=new float *[3];
	for (int i=0;i<3;i++)
		m_fliter_core[i]=new float [3];
	m_fliter_core[0][0]=0.0f;m_fliter_core[0][1]=0.0f;m_fliter_core[0][2]=0.0f;
	m_fliter_core[1][0]=0.0f;m_fliter_core[1][1]=0.0f;m_fliter_core[1][2]=0.0f;
	m_fliter_core[2][0]=0.0f;m_fliter_core[2][1]=0.0f;m_fliter_core[2][2]=0.0f;
	m_core_xsize=m_core_ysize=3;
	spitalFliter(pathSrc,pathDst);
}

/********************************************************************************
*函数描述：MedFliter		  ：中值滤波函数								    *	
*函数参数：float  *imgBufferIn：输入影像的数据空间								*
*		   int   width		  ：影像的宽度										*
*		   int	 heigh	      ：影像的高度										*
*		   int	 xpos	      ：滤波影像的起始x位置								*
*		   int	 ypos	      ：滤波影像的起始y位置								*
*		   int	 xoffset	  ：影像x方向的偏移									*
*		   int	 yoffset	  ：影像y方向的偏移									*
*		   float  *imgBufferOut：输出影像的数据空间								*
********************************************************************************/
float getMedValue(float* imgTemp,int width,int height)							//获取数组的中值
{
	int i,j;// 循环变量
	float bTemp;
	int iFilterLen=width*height;

	// 用冒泡法对数组进行排序
	for (j = 0; j < iFilterLen - 1; j ++)
	{
		for (i = 0; i < iFilterLen - j - 1; i ++)
		{
			if (imgTemp[i] > imgTemp[i + 1])
			{
				// 互换
				bTemp = imgTemp[i];
				imgTemp[i] = imgTemp[i + 1];
				imgTemp[i + 1] = bTemp;
			}
		}
	}

	// 计算中值
	if ((iFilterLen & 1) > 0)
	{
		// 数组有奇数个元素，返回中间一个元素
		bTemp = imgTemp[(iFilterLen + 1) / 2];
	}
	else
	{
		// 数组有偶数个元素，返回中间两个元素平均值
		bTemp = (imgTemp[iFilterLen / 2] + imgTemp[iFilterLen / 2 + 1]) / 2;
	}

	return bTemp;
}
void spFliter::MedFliter(float*imgBufferIn,int width,int heigh,int xpos,int ypos,int xoffset,int yoffset,int xfsize,int yfsize,float* imgBufferOut)
{
	if (imgBufferIn==NULL||imgBufferOut==NULL)
		return ;
	if (xpos<0||ypos<0||xpos>width||ypos>heigh||xpos+xoffset>width||ypos+yoffset>heigh)
		return ;

	//设置初始值
	memcpy(imgBufferOut,imgBufferIn,sizeof(float)*width*heigh);

	//循环处理
	float* imgTemp=new float[xfsize*yfsize];
	float  med;
	for (int i=xpos+xfsize/2;i<xpos+xoffset-(xfsize/2);i++)
	{
		for (int j=ypos+yfsize/2;j<ypos+yoffset-(yfsize/2);j++)
		{
			for (int k=0;k<yfsize;k++)
				memcpy(imgTemp+k*xfsize,imgBufferIn+i-xfsize/2+j*width,sizeof(float)*xfsize);
			med=getMedValue(imgTemp,xfsize,yfsize);
			imgBufferOut[i+j*width]=med;
		}
	}
	delete[]imgTemp;
}

/********************************************************************************
*函数描述：spFliterOperation设置影像滤波核大小和滤波核内容					    *	
*函数参数：float  *imgBufferIn：输入影像的数据空间								*
*		   int   width		  ：影像的宽度										*
*		   int	 heigh	      ：影像的高度										*
*		   int	 xpos	      ：滤波影像的起始x位置								*
*		   int	 ypos	      ：滤波影像的起始y位置								*
*		   int	 xoffset	  ：影像x方向的偏移									*
*		   int	 yoffset	  ：影像y方向的偏移									*
*		   float  *imgBufferOut：输出影像的数据空间								*
********************************************************************************/
void spFliter::spFliterOperation(float*imgBufferIn,int width,int heigh,int xpos,int ypos,int xoffset,int yoffset,float* imgBufferOut)
{
	m_width=width;
	m_heigh=heigh;
	if (xpos<0||ypos<0||xpos>width||ypos>heigh||xpos+xoffset>width||ypos+yoffset>heigh)
		return;
	if (imgBufferIn==NULL||imgBufferOut==NULL)
		return ;
	for (int i=xpos;i<xpos+xoffset;i++)
		for (int j=ypos;j<ypos+yoffset;j++)
			imgBufferOut[j*width+i]=spFliterOperationCore(imgBufferIn,i,j);
}


/********************************************************************************
*函数描述：spFliterOperation设置影像滤波核大小和滤波核内容					    *	
*函数参数：float  *imgBufferIn：输入影像的数据空间								*
*		   int   xpos		  ：模板处理位置x									*
*		   int	 ypos	      ：模板处理位置y									*
********************************************************************************/
float __fastcall spFliter::spFliterOperationCore(float*imgBufferIn,int xpos,int ypos)
{
	if (imgBufferIn==NULL)
		return -100000;
	if (xpos<0||ypos<0)
		return -100000;

	int xcoresize=m_core_xsize;
	int ycoresize=m_core_ysize;
	int tempx=0,tempy=0;
	float tempData=0.0f;
	for (int i=-int(xcoresize/2);i<=int(xcoresize/2);i++)
	{
		for (int j=-int(ycoresize/2);j<=int(ycoresize/2);j++)
		{
			tempx=xpos+i;
			tempy=ypos+j;
			if (tempx<0)
				tempx=xpos-i;
			if (tempx>=m_width)
				tempx=xpos-i;
			if (tempy<0)
				tempy=ypos-j;
			if (tempy>=m_heigh)
				tempy=ypos-j;
			tempData+=imgBufferIn[tempy*m_width+tempx]*m_fliter_core[j+int(ycoresize/2)][i+int(xcoresize/2)];
		}
	}
	return abs(tempData);
}

//////////////////////////////////////////////////////////////////////////
//  [4/23/2014 wuwei just you]
//	添加高斯滤波和差分函数
/********************************************************************************
*函数描述：SetGaussCore设置影像gauss滤波核大小和滤波核内容					    *	
*函数参数：float  **kernel：二维指针，设置的滤波核								*
*		   int   xsize		  ：滤波核的x大小									*
*		   int	 ysize	      ：滤波核的y大小									*
********************************************************************************/
long SetGaussCore(float **kernel,float sigma, int xsize/* =3 */,int ysize/* =3 */)
{
	//检查输入
	if (kernel==NULL)
		return -1;

	int center_x = xsize/2,center_y = ysize/2;

	//记录不同方差对应的不同高斯值
	double gaussian;
	double distance;
	float temp;
	float h_sum = 0;
	for(int i=0;i<xsize;i++)
	{
		for(int j=0;j<ysize;j++)
		{
			distance = (center_x - i) * (center_x - i)+ (center_y - j) * (center_y - j);
			gaussian = exp( (0-distance)/(2* sigma * sigma) )/2*PI * sigma * sigma;
			temp = (float)gaussian;
			h_sum += temp;
			kernel[i][j] = temp;
		}
	}

	//归一化

	for(int i=0;i<xsize;i++)
		for(int j=0; j<ysize; j++)
			kernel[i][j]/=h_sum;

	return 0;
}
long SetGaussCore(float *kernel, float sigma, int xsize/* =3 */,int ysize/* =3 */)
{
	if(kernel==NULL)
		return -1;

	float **kerneldata;
	kerneldata=new float*[ysize];
	for (int i=0;i<ysize;i++)
	{
		kerneldata[i]=new float [xsize];
	}
	SetGaussCore(kerneldata,sigma,xsize,ysize);
	for (int i=0;i<ysize;i++)
		memcpy(kernel+i*xsize,kerneldata[i],sizeof(float)*xsize);
	return 0;
}

/********************************************************************************
*函数描述：GaussFliter设置影像gauss滤波核大小和滤波核内容					    *	
*函数参数：float  *srcImg	  ：源影像											*
*          float  *dstImg	  ：高斯滤波影像输出								*
*		   int   xImg		  ：影像的x大小	    								*
*		   int	 yImg	      ：影像的y大小										*
*		   float ** kernel	  ：影像滤波核										*
*		   int   xkernel	  ：滤波核的x大小									*
*		   int	 ykernel	  ：滤波核的y大小									*
********************************************************************************/
long GaussFliter(float *srcImg,float *dstImg,int xImg,int yImg,float ** kernel,int xkernel,int ykernel)
{
	//检查输入
	if(srcImg==NULL||dstImg==NULL||kernel==NULL)
		return -1;
	int number = xkernel*ykernel;			//储存模板数组，使得一维化
	float *temp_array = new float[number];
	float *temp_img = new float[number];	//储存要用到的相关点
	int k = 0;

	for(int i=0;i<ykernel;i++)
		for(int j=0;j<xkernel;j++)
			temp_array[k++] = kernel[i][j];

	int height = yImg;
	int width  = xImg;
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			int k=0;
			int col_use = ykernel/2;
			int row_use = xkernel/2;
			for(int i=y-row_use; i<=y+row_use; i++)			//相关row*column点储存起来
			{
				for(int j=x-col_use; j<=x+col_use; j++)
				{
					/*如果越界，让它等于某个接近他的图像已有数值*/
					int num_r = i;							
					int num_c = j;
					while(num_r<0)
						num_r++;
					while(num_r>=yImg)
						num_r--;
					while(num_c<0)
						num_c++;
					while(num_c>=xImg)
						num_c--;
					/******************************************/
					temp_img[k] = srcImg[num_r*xImg+num_c];
					k++;
				}
			}

			float sum=0;
			for(int i=0;i<number;i++)
				sum += (temp_img[i] * temp_array[i]);
			dstImg[y*xImg+x] = sum;
		}
	}
	delete []temp_array;
	delete []temp_img;

	return 0;
}

//GDAL封装高斯滤波
long  GaussFliter(char* pathSrc,char* pathDst,float** kernel, int xkernel, int ykernel)
{
	//检查输入
	if(pathSrc==NULL||pathDst==NULL||kernel==NULL)
		return -1;
	
	GDALAllRegister();
	GDALDatasetH m_datasetSrc, m_datasetDst;
	m_datasetSrc=GDALOpen(pathSrc,GA_ReadOnly);
	int xImg=GDALGetRasterXSize(m_datasetSrc);
	int yImg=GDALGetRasterYSize(m_datasetSrc);
	int band=GDALGetRasterCount(m_datasetSrc);

	float* datasrc=new float[xImg*yImg];
	float* datadst=new float[xImg*yImg];
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetDst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,xImg,yImg,band,GDT_Float32,papszOptions);
	for (int i=0;i<band;i++)
	{
		GDALRasterBandH hband;
		hband=GDALGetRasterBand(m_datasetSrc,i+1);
		GDALRasterIO(hband,GF_Read,0,0,xImg,yImg,datasrc,xImg,yImg,GDT_Float32,0,0);
		GaussFliter(datasrc,datadst,xImg,yImg,kernel,xkernel,ykernel);
		hband=GDALGetRasterBand(m_datasetDst,i+1);
		GDALRasterIO(hband,GF_Write,0,0,xImg,yImg,datadst,xImg,yImg,GDT_Float32,0,0);
	}

	//投影
	const char* proj=GDALGetProjectionRef(m_datasetSrc);
	GDALSetProjection(m_datasetDst,proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_datasetSrc,adfGeoTrans);
	GDALSetGeoTransform(m_datasetDst,adfGeoTrans);
	//清除指针
	GDALClose(m_datasetSrc);
	GDALClose(m_datasetDst);
	delete[]datadst;
	delete[]datasrc;

	return 0;
}

/********************************************************************************
*函数描述：GaussDifference对高斯滤波后的影像进行差分						    *	
*函数参数：float  *srcImg1	  ：滤波后影像1										*
*		   float  *srcImg2	  ：滤波后影像2										*
*		   float  *dstImg	  ：输出结果影像									* 
*		   int   xsize		  ：影像的x大小										*
*		   int	 ysize	      ：影像的y大小										*
********************************************************************************/
long GaussDifference(float* srcImg1,float* srcImg2,float* dstImg,int xImg,int yImg)
{
	//检查输入
	if(srcImg1==NULL||srcImg2==NULL||dstImg==NULL)
		return -1;
	int pixelNum=xImg*yImg;
	for(int i=0;i<pixelNum;i++)
		dstImg[i]=srcImg1[i]-srcImg2[i];

	return 0;
	
}

//GDAL封装高斯差分
long GaussDifference(char* pathsrc1,char* pathsrc2,char* pathdst)
{
	//检查数据输入
	if(pathsrc1==NULL||pathsrc2==NULL||pathdst==NULL)
		return -1;

	GDALAllRegister();
	GDALDatasetH m_dataset1,m_dataset2,m_datasetdst;

	m_dataset1=GDALOpen(pathsrc1,GA_ReadOnly);
	m_dataset2=GDALOpen(pathsrc2,GA_ReadOnly);
	int xsrc1=GDALGetRasterXSize(m_dataset1);
	int ysrc1=GDALGetRasterYSize(m_dataset1);
	int xsrc2=GDALGetRasterXSize(m_dataset2);
	int ysrc2=GDALGetRasterYSize(m_dataset2);
	int band1=GDALGetRasterCount(m_dataset1);
	int band2=GDALGetRasterCount(m_dataset2);
	if (xsrc1!=xsrc2||ysrc1!=ysrc2||band1!=band2)
		return -1;

	float* datasrc1=new float [xsrc1*ysrc1];
	float* datasrc2=new float [xsrc2*ysrc2];
	float* datadst =new float [xsrc1*ysrc1];
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathdst,xsrc1,ysrc1,band1,GDT_Float32,papszOptions);
	for (int i=0;i<band1;i++)
	{
		GDALRasterBandH hBand;
		hBand=GDALGetRasterBand(m_dataset1,i+1);
		GDALRasterIO(hBand,GF_Read,0,0,xsrc1,ysrc1,datasrc1,xsrc1,ysrc1,GDT_Float32,0,0);
		hBand=GDALGetRasterBand(m_dataset2,i+1);
		GDALRasterIO(hBand,GF_Read,0,0,xsrc2,ysrc2,datasrc2,xsrc2,ysrc2,GDT_Float32,0,0);
		hBand=GDALGetRasterBand(m_datasetdst,i+1);
		GaussDifference(datasrc1,datasrc2,datadst,xsrc2,ysrc2);
		GDALRasterIO(hBand,GF_Write,0,0,xsrc1,ysrc2,datadst,xsrc1,ysrc1,GDT_Float32,0,0);
	}

	//投影
	const char* proj=GDALGetProjectionRef(m_dataset1);
	GDALSetProjection(m_datasetdst,proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_dataset1,adfGeoTrans);
	GDALSetGeoTransform(m_datasetdst,adfGeoTrans);

	//清除数据
	GDALClose(m_dataset1);
	GDALClose(m_dataset2);
	GDALClose(m_datasetdst);

	delete[]datadst;
	delete[]datasrc1;
	delete[]datasrc2;

	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////////
//影像空域滤波多线程数据结构体
struct spFliterThreadStruct 
{
	GDALDatasetH dataset_src;	//输入数据集
	GDALDatasetH dataset_dst;	//输出数据集
	float* fliter_core;			//滤波核
	int corexsize;				//滤波核大小
	int coreysize;
	BlockLabel block;			//影像块
	int image_band;				//待处理的影像波段
};

//影像分块空域滤波
long ImageSpaitalBlockFliter(char* pathsrc,char* pathdst,float* fliterCore,int xcoresize,int ycoresize)
{
	GDALAllRegister();
	GDALDatasetH m_dataset_src=GDALOpen(pathsrc,GA_ReadOnly);
	
	int xsize=GDALGetRasterXSize(m_dataset_src);
	int ysize=GDALGetRasterYSize(m_dataset_src);
	int bands=GDALGetRasterCount(m_dataset_src);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataset_dst=GDALCreate(GDALGetDriverByName("GTiff"),pathdst,xsize,ysize,bands,GDT_Float32,papszOptions);
	//GDALDatasetH m_dataset_dst=GDALCreateCopy(GDALGetDriverByName("GTiff"),pathdst,m_dataset_src,)

	//获取影像分块信息
	int xblock=GetImageXBlockNum(m_dataset_src);
	int yblock=GetImageYBlockNum(m_dataset_src);
	BlockLabel *blocks=new BlockLabel[xblock*yblock];
	int buffer=max(xcoresize,ycoresize)+10;
	GetImageBlock(m_dataset_src,blocks,xblock,yblock,buffer);

	//分块滤波并写入文件中
	spFliter spatialfliter;spatialfliter.spFliterCore(fliterCore,xcoresize,ycoresize);
	for (int i=0;i<xblock*yblock;i++)
	{
		printf("\r处理影像块%d/%d",xblock*yblock,i+1);
		for (int j=0;j<bands;j++)
		{
			int readWidth =blocks[i].m_edgeH[1]-blocks[i].m_edgeH[0];
			int readHeight=blocks[i].m_edgeV[1]-blocks[i].m_edgeV[0];

			float* dataIn =new float[readWidth*readHeight];memset(dataIn,0,sizeof(float)*readHeight*readWidth);
			float* dataOut=new float[readWidth*readHeight];memset(dataOut,0,sizeof(float)*readHeight*readWidth);
			ReadBlockImageData(m_dataset_src,blocks[i],dataIn ,j+1);
			spatialfliter.spFliterOperation(dataIn,readWidth,readHeight,0,0,readWidth,readHeight,dataOut);
			WriteBlockImageData(m_dataset_dst,blocks[i],dataOut,j+1);

			delete[]dataIn;
			delete[]dataOut;
		}
	}
	printf("\n");

	GDALClose(m_dataset_dst);
	GDALClose(m_dataset_src);
	delete[]blocks;

	return 0;
}

//多线程空域滤波线程函数
DWORD WINAPI thread_spatial_Fliter_core(LPVOID dataStruct) 
{
	UserPoolData* poolData = (UserPoolData*)dataStruct;
	spFliterThreadStruct* sampleSt=(spFliterThreadStruct*)poolData->pData;
	spFliter spatialfliter;
	//数据
	BlockLabel m_block=sampleSt->block;
	GDALDatasetH m_dataset_src=sampleSt->dataset_src;
	GDALDatasetH m_dataset_dst=sampleSt->dataset_dst;
	int m_corexsize=sampleSt->corexsize;
	int m_coreysize=sampleSt->coreysize;
	float* coredata=sampleSt->fliter_core;
	int band=sampleSt->image_band;

	int readWidth =m_block.m_edgeH[1]-m_block.m_edgeH[0];
	int readHeight=m_block.m_edgeV[1]-m_block.m_edgeV[0];

	float* dataIn =new float[readWidth*readHeight];memset(dataIn,0,sizeof(float)*readHeight*readWidth);
	float* dataOut=new float[readWidth*readHeight];memset(dataOut,0,sizeof(float)*readHeight*readWidth);
	spatialfliter.spFliterCore(coredata,m_corexsize,m_coreysize);
	ReadBlockImageData(m_dataset_src,m_block,dataIn ,band);
	spatialfliter.spFliterOperation(dataIn,readWidth,readHeight,0,0,readWidth,readHeight,dataOut);
	WriteBlockImageData(m_dataset_dst,m_block,dataOut,band);

	delete[]dataIn;
	delete[]dataOut;

	return 0;
}

//多线程空域滤波
long ThreadImageSpaitalFliter(char* pathsrc,char* pathdst,float* fliterCore,int xcoresize,int ycoresize)
{
	GDALAllRegister();
	GDALDatasetH m_dataset_src;
	GDALDatasetH m_dataset_dst;
	spFliterThreadStruct* threadSt;

	m_dataset_src=GDALOpen(pathsrc,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset_src);
	int ysize=GDALGetRasterYSize(m_dataset_src);
	int bands=GDALGetRasterCount(m_dataset_src);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_dataset_dst=GDALCreate(GDALGetDriverByName("GTiff"),pathdst,xsize,ysize,bands,GDT_Float32,papszOptions);

	//获取影像分块信息
	int xblock=GetImageXBlockNum(m_dataset_src);
	int yblock=GetImageYBlockNum(m_dataset_src);
	BlockLabel *blocks=new BlockLabel[xblock*yblock];
	GetImageBlock(m_dataset_src,blocks,xblock,yblock,max(xcoresize,ycoresize));
	threadSt=new spFliterThreadStruct[xblock*yblock*bands];
	//多线程
	CThreadPool m_threadpool;
	for (int i=0;i<xblock*yblock;i++)
	{
		for (int j=0;j<bands;j++)
		{
			threadSt[j*xblock*yblock+i].block=blocks[i];
			threadSt[j*xblock*yblock+i].corexsize=xcoresize;
			threadSt[j*xblock*yblock+i].coreysize=ycoresize;
			threadSt[j*xblock*yblock+i].dataset_dst=m_dataset_dst;
			threadSt[j*xblock*yblock+i].dataset_src=m_dataset_src;
			threadSt[j*xblock*yblock+i].fliter_core=fliterCore;
			threadSt[j*xblock*yblock+i].image_band=j+1;
			m_threadpool.Run(thread_spatial_Fliter_core,&threadSt[j*xblock*yblock+i],Low);
		}
	}
	
	do 
	{
		Sleep(5);
	} while (m_threadpool.GetWorkingThreadCount()!=0);

	GDALClose(m_dataset_src);
	GDALClose(m_dataset_dst);
	delete[]blocks;
	delete[]threadSt;
	m_threadpool.Destroy();

	return 0;
}

/********************************************************************************
*类信息描述：				 影像形态学滤波操作								    *	
*							1.设置影像滤波核，目标位置为1其他位置为0				*
*							2.影像形态学腐蚀操作									*
*							3.影像形态学膨胀操作									*
*							4.影像开闭运算										*
********************************************************************************/
//设置滤波核
void MorphologyFliter::MorphologyCore(float** core,int xsize,int ysize,int centerx,int centery)
{
	if (core==NULL)
		return;

	m_core_xsize=xsize;
	m_core_ysize=ysize;

	m_fliter_core=new float *[ysize];
	for (int i=0;i<ysize;i++)
	{
		m_fliter_core[i]=new float [xsize];
		memcpy(m_fliter_core[i],core[i],sizeof(float)*xsize);
	}
	m_is_core=true;
}

void MorphologyFliter::MorphologyCore(float* core,int xsize,int ysize,int centerx,int centery)
{
	if (core==NULL)
		return;

	m_core_xsize=xsize;
	m_core_ysize=ysize;

	m_fliter_core=new float *[ysize];
	for (int i=0;i<ysize;i++)
	{
		m_fliter_core[i]=new float [xsize];
		memcpy(m_fliter_core[i],core+i*xsize,sizeof(float)*xsize);
	}
	m_is_core=true;
}

//形态学腐蚀操作
void MorphologyFliter::MorphologyCorrode(float* dataIn,int sizex,int sizey,int bands,float* dataOut)
{
	if(dataIn==NULL||dataOut==NULL)
		return ;
	if(!this->m_is_core)
	{
		printf("do not set fliter core!\n");
		return ;
	}
	//腐蚀操作滤波核窗口内最小的像素
	memcpy(dataOut,dataIn,sizeof(float)*sizex*sizey);
	for (int i=0;i<sizex;i++)
	{
		for (int j=0;j<sizey;j++)
		{
			if(i+this->m_core_xsize>=sizex||j+this->m_core_ysize>=sizey)
				continue;

			vector<float> m_tempData;
			for (int l=0;l<m_core_xsize;l++)
			{
				for (int m=0;m<m_core_ysize;m++)
				{
					if(this->m_fliter_core[m*this->m_core_xsize+l]!=0)
						m_tempData.push_back(dataIn[(j+m)*sizex+(l+i)]);
				}
			}
			float mindata=0;//min_element(m_tempData.begin(),m_tempData.end());
			dataOut[(j+this->m_centery)*sizex+(i+this->m_centerx)]=mindata;
			m_tempData.clear();
		}
	}
}
void MorphologyFliter::MorphologyDilation(float* dataIn,int sizex,int sizey,int bands,float* dataOut)
{
	if(dataIn==NULL||dataOut==NULL)
		return ;
	if(!this->m_is_core)
	{
		printf("do not set fliter core!\n");
		return ;
	}
	//腐蚀操作滤波核窗口内最小的像素
	memcpy(dataOut,dataIn,sizeof(float)*sizex*sizey);
	for (int i=0;i<sizex;i++)
	{
		for (int j=0;j<sizey;j++)
		{
			if(i+this->m_core_xsize>=sizex||j+this->m_core_ysize>=sizey)
				continue;

			vector<float> m_tempData;
			for (int l=0;l<m_core_xsize;l++)
			{
				for (int m=0;m<m_core_ysize;m++)
				{
					if(this->m_fliter_core[m*this->m_core_xsize+l]!=0)
						m_tempData.push_back(dataIn[(j+m)*sizex+(l+i)]);
				}
			}
			float maxdata=0;//max_element(m_tempData.begin(),m_tempData.end());
			dataOut[(j+this->m_centery)*sizex+(i+this->m_centerx)]=maxdata;
			m_tempData.clear();
		}
	}
}

//GDAL封装的形态学腐蚀与膨胀
//形态学腐蚀
void MorphologyFliter::MorphologyCorrode(char* pathIn,char* pathOut)
{
	if(!this->m_is_core)
	{
		printf("do not set fliter core!\n");
		return ;
	}

	GDALAllRegister();
	GDALDatasetH m_dataset=GDALOpen(pathIn,GA_ReadOnly);
	
	int xsize=GDALGetRasterXSize(m_dataset);
	int ysize=GDALGetRasterYSize(m_dataset);
	int bands=GDALGetRasterCount(m_dataset);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_datasetDS=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsize,ysize,bands,GDT_Float32,papszOptions);
	float *dataIn=NULL,*dataOut=NULL;
	dataIn =new float[xsize*ysize];
	dataOut=new float[xsize*ysize];
	for(int i=0;i<bands;i++)
	{
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,dataIn,xsize,ysize,GDT_Float32,0,0);
		MorphologyCorrode(dataIn,xsize,ysize,1,dataOut);
		GDALRasterIO(GDALGetRasterBand(m_datasetDS,i+1),GF_Write,0,0,xsize,ysize,dataOut,xsize,ysize,GDT_Float32,0,0);
	}
	//投影
	const char* proj=GDALGetProjectionRef(m_dataset);
	GDALSetProjection(m_datasetDS,proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_dataset,adfGeoTrans);
	GDALSetGeoTransform(m_datasetDS,adfGeoTrans);

	GDALClose(m_datasetDS);
	GDALClose(m_dataset);
	delete[]dataIn;
	delete[]dataOut;
}

//形态学膨胀
void MorphologyFliter::MorphologyDilation(char* pathIn,char* pathOut)
{
	if(!this->m_is_core)
	{
		printf("do not set fliter core!\n");
		return ;
	}

	GDALAllRegister();
	GDALDatasetH m_dataset=GDALOpen(pathIn,GA_ReadOnly);

	int xsize=GDALGetRasterXSize(m_dataset);
	int ysize=GDALGetRasterYSize(m_dataset);
	int bands=GDALGetRasterCount(m_dataset);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_datasetDS=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsize,ysize,bands,GDT_Float32,papszOptions);
	float *dataIn=NULL,*dataOut=NULL;
	dataIn =new float[xsize*ysize];
	dataOut=new float[xsize*ysize];
	for(int i=0;i<bands;i++)
	{
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,dataIn,xsize,ysize,GDT_Float32,0,0);
		MorphologyDilation(dataIn,xsize,ysize,1,dataOut);
		GDALRasterIO(GDALGetRasterBand(m_datasetDS,i+1),GF_Write,0,0,xsize,ysize,dataOut,xsize,ysize,GDT_Float32,0,0);
	}

	//投影
	const char* proj=GDALGetProjectionRef(m_dataset);
	GDALSetProjection(m_datasetDS,proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_dataset,adfGeoTrans);
	GDALSetGeoTransform(m_datasetDS,adfGeoTrans);

	GDALClose(m_datasetDS);
	GDALClose(m_dataset);
	delete[]dataIn;
	delete[]dataOut;
}