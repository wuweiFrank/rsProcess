#include "HyperSpectural.h"
#include <memory>
#include <fstream>
#include <algorithm>

#include "..\Global.h"
#include "..\gdal/include/gdal_priv.h"
#include "..\matrixOperation.h"
using namespace std;

#pragma comment(lib,"gdal_i.lib")
#pragma warning(disable : 4996)

//============================================================================
// Name        : HyperSpectural.cpp
// Author      : ww
// Version     : 2.0
// Copyright   : ww
// Description : 高光谱影像处理（投影空间变换）
//============================================================================
//[10/18/2014 wuwei just you]
// Version	   :2.0  1.增加了高光谱图像RXD算法；2.修正了MNF算法
// Version	   :2.1  1.增加了FastICA算法

// Version	   :2.2  1.增加了快速单形体端元提取 FastFinder算法
//					 2.增加了光谱角距离测度
//					 3.增加了端元输出函数
//  [1/29/2015 wuwei just you]
// Version	   :2.2.1 1.增加测试函数测试所有代码
//  [2/7/2015 wuwei just you]
//  Modified by w.w.Frank
//  [4/2/2015 ygoffice]
// Version	   :2.2.2 1.增加投影变换的代码
//					  2.增加读取ENVI的ROI区域的代码
//  Modified by w.w.Frank

//文件处理
/*------------------------将矩阵写入文件中------------------------------*/
long Matrix_Export_File(const char* fileName,double **matrix,int size)
{
	int i,j;			//循环变量
	ofstream outFile;	//读入文件
	if(fileName==NULL||matrix==NULL)
		return 1;

	//打开文件文件
	outFile.open(fileName,ios_base::binary);
	if(!outFile.is_open())
		return 3;

	//第一行为矩阵的大小
	/*outFile.write((char*)&size,sizeof(size));*/
	outFile<<size<<"  "<<endl;;

	//将矩阵写入文件
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			/*outFile.write((char*)&matrix[i][j],sizeof(matrix[i][j]));*/
			outFile<<matrix[i][j]<<"   ";
		}
		outFile<<endl;
	}
	outFile.close();

	if(!outFile.is_open())
		return 0;
	else
		return 3;
}

/*--------------------------获得矩阵的大小------------------------------*/
long Matrix_Get_Size(const char* fileName,int &size)
{
	//int i,j;			//循环迭代变量
	ifstream inFile;
	size=0;				//初始值
	if(fileName==NULL)
		return 1;
	inFile.open(fileName,ios_base::in||ios_base::binary);
	if(!inFile.is_open())
		return 3;
	/*inFile.read((char*)&size,sizeof(int));*/
	inFile>>size;
	if(size>1)
		return 0;
	else
		return 3;

}

/*-----------------------从文件中读出变换矩阵---------------------------*/
long Matrix_Import_File(const char* fileName,double **matrix)
{

	int i,j;			//循环迭代变量
	ifstream inFile;
	int size;
	if(fileName==NULL||matrix==NULL)
		return 1;

	inFile.open(fileName,ios_base::in||ios_base::binary);
	if(!inFile.is_open())
		return 3;
	/*inFile.read((char*)&size,sizeof(int));*/
	inFile>>size;
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			/*inFile.read((char*)&matrix[i][j],sizeof(double));*/
			inFile>>matrix[i][j];
		}
	}

	inFile.close();
	if(!inFile.is_open())
		return 0;
	else
		return 3;
}

/*--------------------------端元输出到文件中----------------------------*/
long EndMember_Export_File(const char* fileName,double *endmember,int bands,int numbers)
{
	ofstream ofs;
	ofs.open(fileName);
	if(!ofs.is_open())
		return -1;

	//文件头
	time_t curtime;
	curtime=time(NULL);
	ofs<<"ENVI ASCII Plot File"<<"["<<ctime(&curtime)<<"]"<<endl;
	ofs<<"wavelength"<<endl;
	for(int i=0;i<numbers;i++)
		ofs<<"class:"<<i+1<<endl;
	//输出数据
	for (int i=0;i<bands;i++)
	{
		ofs<<i+1<<" ";
		for (int j=0;j<numbers;j++)
		{
			ofs<<endmember[j*bands+i]<<" ";
		}
		ofs<<endl;
	}
	ofs.close();
	return 0;
}
/*--------------------------从文件中获取端元----------------------------*/
long EndMember_Import_File(const char* fileName,double *endmember,int bands,int numbers)
{
	ifstream ifs;
	ifs.open(fileName,ios_base::in);
	if(ifs.is_open()!=true)
		return NULL;
	char tempchr[2048];
	for(int i=0;i<numbers+2;i++)
		ifs.getline(tempchr,2048);

	for (int i=0;i<bands;i++)
	{
		double temp;
		ifs>>temp;
		for (int j=0;j<numbers;j++)
		{
			ifs>>temp;
			endmember[i*numbers+j]=temp/10;
		}
	}
	return 0;
}

/*----------------------获取ENVI输出的感兴趣区域------------------------*/
long  EnviROINumbers(const char* roiFile,int &roiNumbers)
{
	ifstream ifs(roiFile,ios_base::in);
	char szTemp[2048];memset(szTemp,0,sizeof(char)*2048);
	char sztmp[10];	  memset(sztmp,0,sizeof(char)*10);
	ifs.getline(szTemp,2048);
	memset(szTemp,0,sizeof(char)*2048);
	ifs.getline(szTemp,2048);
	
	for (int i=18,j=0;i<28;i++)
	{
		if(szTemp[i]!='\n')
		{
			sztmp[j]=szTemp[i];
			++j;
		}
	}
	roiNumbers=atoi(sztmp);
	ifs.close();
	return 0;
}

long  EnviROIPts(const char* roiFile,int* roiPts,int roiNmbers)
{
	ifstream ifs(roiFile,ios_base::in);
	char szTemp[2048];memset(szTemp,0,sizeof(char)*2048);
	char sztmp[10];	  memset(sztmp,0,sizeof(char)*10);
	ifs.getline(szTemp,2048);
	ifs.getline(szTemp,2048);
	ifs.getline(szTemp,2048);
	ifs.getline(szTemp,2048);
	memset(szTemp,0,sizeof(char)*2048);

	for (int k=0;k<roiNmbers;k++)
	{
		ifs.getline(szTemp,2048);
		ifs.getline(szTemp,2048);
		memset(szTemp,0,sizeof(char)*2048);
		ifs.getline(szTemp,2048);
		for (int i=12,j=0;i<22;i++)
		{
			if(szTemp[i]!='\n')
			{
				sztmp[j]=szTemp[i];
				++j;
			}
		}
		roiPts[k]=atoi(sztmp);
		memset(sztmp,0,sizeof(char)*10);
		ifs.getline(szTemp,2048);
	}
	return 0;
}

long  EnviROIData(const char* roiFile,int* roiPts,int roiNumbers,int bands,double** roiData)
{
	ifstream ifs(roiFile,ios_base::in);
	char szTemp[2048];memset(szTemp,0,sizeof(char)*2048);
	char sztmp[10];	  memset(sztmp,0,sizeof(char)*10);
	for (int i=0;i<(roiNumbers+1)*4;i++)
		ifs.getline(szTemp,2048);
	int totalnum=0;
	for(int i=0;i<roiNumbers;i++)
		totalnum+=roiPts[i];
	double* data=new double[bands*totalnum];
	for (int i=0,k=0;i<roiNumbers;i++)
	{
		for (int j=0;j<roiPts[i];j++,k++)
		{
			double temp;
			ifs>>temp;ifs>>temp;ifs>>temp;
			for (int l=0;l<bands;l++)
				ifs>>data[k*bands+l];
		}
		ifs.getline(szTemp,2048);
	}
	ifs.close();

	//BSQ存储方式
	for (int i=0,l=0;i<roiNumbers;i++,l+=roiPts[i])
		for (int k=0;k<bands;k++)
			for (int j=0;j<roiPts[i];j++)
				roiData[i][roiPts[i]*k+j]=data[(l+j)*bands+k];
	delete[]data;
	return 0;

}

/********************************************************************************************************/
/*											影像统计量                                                  */
/********************************************************************************************************/
//计算光谱角距离
long Spectral_Angle_Distance(double* data1,double* data2,int bands,double &angle)
{
	//检查数据
	if(data1==NULL||data2==NULL)
		return -1;

	//计算光谱角
	double tempa1=0,tempa2=0,tempa3=0;
	for(int i=0;i<bands;i++)
	{
		tempa1=data1[i]*data1[i];
		tempa2=data2[i]*data2[i];
		tempa3=data1[i]*data2[i];
	}
	angle=tempa3/sqrt(tempa1)/sqrt(tempa2);

	return 0;
}

long Spectral_Angle_Distance(double* imgBuffer,int xsize,int ysize,int xpos,int ypos,double *data,int bands,double& angle)
{
	//检查输入
	long lError=0;
	if(imgBuffer==NULL||data==NULL)
		return -1;
	double* data2=new double[bands];
	for (int i=0;i<bands;i++)
	{
		data2[i]=imgBuffer[i*xsize*ysize+ypos*xsize+xpos];
	}
	lError=Spectral_Angle_Distance(data,data2,bands,angle);
	if(lError!=0)
		return lError;
	else
		return 0;
}

//图像各个波段融合
void Layerstacking(char *pathIn, int begNum,int endNum,char* pathOut)
{
	//创建数据集
	GDALAllRegister();
	GDALDatasetH m_datasetIn =GDALOpen(pathIn,GA_ReadOnly);
	if(m_datasetIn==NULL)
		return;

	int x=GDALGetRasterXSize(m_datasetIn);
	int y=GDALGetRasterYSize(m_datasetIn);
	const char* proj=GDALGetProjectionRef(m_datasetIn);
	GDALClose(m_datasetIn);

	//拆分文件名
	char driveIn[_MAX_DRIVE];
	char dirIn[_MAX_DIR];
	char fnameIn[_MAX_FNAME];
	char extIn[_MAX_EXT];

	char sName[280];
	char tempIn[10];

	errno_t    err;
	err = _splitpath_s( pathIn, driveIn,_MAX_DRIVE, dirIn, _MAX_DIR, fnameIn,_MAX_FNAME, extIn, _MAX_EXT );
	if (err)
		return ;

	double* data=new double[x*y];
	int bands=0;

	//获取波段数
	for (int i=begNum;i<=endNum;i++)
	{
		sprintf(tempIn,"%d",i);
		err = _makepath_s(sName, _MAX_PATH, driveIn,dirIn,tempIn,extIn);
		if (err)
			return ;

		//获取影像数据
		m_datasetIn=GDALOpen(sName,GA_ReadOnly);
		int tempy=GDALGetRasterYSize(m_datasetIn);
		int tempbands=GDALGetRasterCount(m_datasetIn);
		bands+=tempbands;
		GDALClose(m_datasetIn);
	}
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_datasetOut=GDALCreate(GDALGetDriverByName("HFA"),pathOut,x,y,bands,GDT_Float64,papszOptions);
	int  bandtempNum=0;
	for (int i=begNum;i<=endNum;i++)
	{
		sprintf(tempIn,"%d",i);
		err = _makepath_s(sName, _MAX_PATH, driveIn,dirIn,tempIn,extIn);
		if (err)
			return ;

		//获取影像数据
		m_datasetIn=GDALOpen(sName,GA_ReadOnly);
		int tempx=GDALGetRasterXSize(m_datasetIn);
		int tempy=GDALGetRasterYSize(m_datasetIn);
		int tempband=GDALGetRasterCount(m_datasetIn);

		if(tempx!=x||tempy!=y)
			return;

		memset(data,0,sizeof(double)*x*y);
		for (int j=0;j<tempband;j++)
		{
			GDALRasterIO(GDALGetRasterBand(m_datasetIn,j+1),GF_Read,0,0,x,y,data,x,y,GDT_Float64,0,0);
			GDALRasterIO(GDALGetRasterBand(m_datasetOut,j+bandtempNum+1),GF_Write,0,0,x,y,data,x,y,GDT_Float64,0,0);
		}
		bandtempNum+=tempband;
		GDALClose(m_datasetIn);
	}

	GDALSetProjection(m_datasetOut,proj);
	GDALClose(m_datasetOut);
	delete[]data;
}

//统计过程中认为所有影像值都是有效值，异常值需要提前进行处理
/*-----------------------------获取影像的熵-----------------------------*/
long GetImageEntropy(double *imgBuffer,int bandSum,int* bandNo,int xsize,
	int ysize,double* entropy)
{
	if (imgBuffer==NULL||entropy==NULL)
		return 1;
	if(xsize<=0||ysize<=0||bandSum<=0)
		return 2;
	memset(entropy,0,sizeof(double)*bandSum);
	//获取图像的最大最小值
	double maxnum,minnum;
	for (int i=0;i<bandSum;i++)
	{
		maxnum=minnum=imgBuffer[i*xsize*ysize];
		for (int k=0;k<xsize*ysize;k++)
		{
			//获取波段的最大最小值
			maxnum=max(maxnum,imgBuffer[i*xsize*ysize+k]);
			minnum=max(minnum,imgBuffer[i*xsize*ysize+k]);
		}
		//获取灰度级数
		int  graylevel=int(maxnum-minnum);
		int* histrogram=new int[graylevel];memset(histrogram,0,sizeof(int)*graylevel);
		double* histroRato=new double[graylevel];memset(histroRato,0,sizeof(double)*graylevel);

		for (int k=0;k<xsize*ysize;k++)
		{
			histrogram[int(imgBuffer[i*xsize*ysize+k])]++;
		}
		for (int k=0;k<graylevel;k++)
		{
			histroRato[k]=double(histrogram[k])/double(xsize)/double(ysize);
			entropy[i]+=histroRato[k]*log(histroRato[k]);
		}
	}
	return 0;
}

/*-----------------------------获取部分影像-----------------------------*/
double* GetpartImage(double* imgBuffer,int bands,int imgxsize,int imgysize,int xpos,int ypos,int xsize,int ysize)
{
	if(xpos+xsize>imgxsize||ypos+ysize>imgysize)
		return NULL;
	double* imgpart=NULL;
	imgpart=new double[xsize*ysize*bands];
	if(imgpart==NULL)
		return imgpart;
	for(int j=0;j<bands;j++)
	{
		for (int i=0;i<ysize;i++)
		{
			memcpy(imgpart+i*xsize+j*xsize*ysize,imgBuffer+(i+ypos)*imgxsize+xpos+j*imgxsize*imgysize,sizeof(double)*xsize);
		}
	}

	return imgpart;
}

/*-----------------------------计算影像波段的均值-----------------------*/
long GetImgMean(double *imgBuffer,int bandSum,int* bandNo,int xsize,int ysize,double* meanValue)
{
	int i,j,k;		//迭代变量
	double tempValue=0;

	if (imgBuffer==NULL||meanValue==NULL)
		return 1;
	if(xsize<=0||ysize<=0||bandSum<=0)
		return 2;

	//初始化
	memset(meanValue,0,sizeof(double)*bandSum);
	for (i=0;i<bandSum;i++)
	{
		for (j=0;j<ysize;j++)
		{
			for (k=0;k<xsize;k++)
			{
				tempValue+=imgBuffer[i*xsize*ysize+j*xsize+k];
			}
			meanValue[i]+=tempValue/xsize;
			tempValue=0;
		}
		meanValue[i]/=ysize;
	}

	//计算完成返回成功
	return 0;
}

/*-------------------------------计算影像的标准差-----------------------*/
long GetImgStdCovarianve(double *imgBuffer,int bandSum,int* bandNo,
	int xsize,int ysize,double *meanValue,double* stdv)
{
	int i,j,k;		//迭代变量

	if (imgBuffer==NULL||meanValue==NULL||stdv==NULL)
		return 1;
	if(xsize<=0||ysize<=0||bandSum<=0)
		return 2;

	//初始化
	memset(stdv,0,sizeof(double)*bandSum);
	for (i=0;i<bandSum;i++)
	{
		for (j=0;j<ysize;j++)
		{
			for (k=0;k<xsize;k++)
			{
				stdv[i]+=pow(imgBuffer[i*xsize*ysize+j*xsize+k]-meanValue[i],2.0);
			}
		}
		stdv[i]/=(xsize*ysize);
		stdv[i]=sqrt(stdv[i]);
	}

	//计算完成返回成功
	return 0;
}

/*--------------------------计算波段间的协方差矩阵----------------------*/
long GetCovarianceMatrix(double *imgBuffer,int bandSum,int* bandNo,
                                int xsize,int ysize,double** covarianceMat)
{
	int i,j,k,l;				//迭代变量
	double tempAvg=0.0f;
	double* avgVector=NULL;		//每个波段的均值
	double temcov=0.0f;

	if (imgBuffer==NULL||covarianceMat==NULL)
		return 1;
	if(xsize<=0||ysize<=0||bandSum<=0)
		return 2;

	//计算每个波段影像的均值
	if(NULL==(avgVector=new double[bandSum]))
		return 3;
	memset(avgVector,0,sizeof(double)*bandSum);
	for(i=0;i<bandSum;i++)
	{
		for(j=0;j<ysize;j++)
		{
			for(k=0;k<xsize;k++)
				tempAvg+=imgBuffer[i*xsize*ysize+j*xsize+k];
			avgVector[i]+=tempAvg/xsize;
			tempAvg=0;
		}
		avgVector[i]/=ysize;
	}

	for (i=0;i<bandSum;i++)
		memset(covarianceMat[i],0,sizeof(double)*bandSum);	//初始化协方差矩阵

	//计算协方差矩阵
	for (i=0;i<bandSum;i++)
	{
		for(j=i;j<bandSum;j++)
		{
			for(k=0;k<ysize;k++)
			{
				//协方差矩阵为对称矩阵
				for (l=0;l<xsize;l++)
				{
					temcov+=(imgBuffer[(k*xsize+l)+i*xsize*ysize]-avgVector[i])*
						(imgBuffer[(k*xsize+l)+j*xsize*ysize]-avgVector[j]);
				}
				covarianceMat[i][j]+=temcov/xsize;
				temcov=0.0f;
			}
			covarianceMat[j][i]=covarianceMat[i][j]=covarianceMat[i][j]/ysize;
			//covarianceMat[j][i]=covarianceMat[i][j]=covarianceMat[i][j]*xsize*ysize/(xsize*ysize-1);
		}
	}

	//清除指针
	if (avgVector==NULL)
		return 3;
	else
		delete []avgVector;avgVector=NULL;

	//成功执行 返回0
	return 0;
}

/*-------------------------------计算噪声影像----------------------------*/
long GetNoiseImg(double *imgBuffer,int bandSum,int* bandNo, int sampleX,
                                int sampleY,int xsize,int ysize,double* imgNoise)
{
	int i,j,k;				//迭代变量

	if(imgBuffer==NULL||imgNoise==NULL)
		return 1;
	if (bandSum<=0||sampleX<=0||sampleY<=0||xsize<=0||ysize<=0)
		return 2;

	//每个波段计算的噪声
	for (i =0;i<bandSum;i++)
	{
		for (j=0;j<ysize;j++)
		{
			for (k=0;k<xsize;k++)
			{
				//判断加上采样距离是否超过影像的大小
				//若超过影像大小则去对称方向上的像素
				if (j+sampleY>=ysize&&k+sampleX<xsize)
				{
					imgNoise[i*xsize*ysize+j*xsize+k]=abs(imgBuffer[i*xsize*ysize+j*xsize+k]-
						imgBuffer[i*xsize*ysize+(j-sampleY)*xsize+k+sampleX]);
				}
				else if(k+sampleX>=xsize&&j+sampleY<ysize)
				{
					imgNoise[i*xsize*ysize+j*xsize+k]=abs(imgBuffer[i*xsize*ysize+j*xsize+k]-
						imgBuffer[i*xsize*ysize+(j+sampleY)*xsize+k-sampleX]);
				}
				else if(k+sampleX>=xsize&&j+sampleY>=ysize)
				{
					imgNoise[i*xsize*ysize+j*xsize+k]=abs(imgBuffer[i*xsize*ysize+j*xsize+k]-
						imgBuffer[i*xsize*ysize+(j-sampleY)*xsize+k-sampleX]);
				}
				else
				{
					imgNoise[i*xsize*ysize+j*xsize+k]=abs(imgBuffer[i*xsize*ysize+j*xsize+k]-
						imgBuffer[i*xsize*ysize+(j+sampleY)*xsize+k+sampleX]);
				}
			}
		}
	}

	//计算完成返回成功
	return 0;
}
/*-------------------------------某一行横切面影像------------------------*/
long  GetCutLineImg(const char*pathIn,int lineIdx,const char* pathOut)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO" );	//中文路径
	GDALDatasetH m_datasetIn;
	m_datasetIn=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_datasetIn);
	int ysize=GDALGetRasterYSize(m_datasetIn);
	int bands=GDALGetRasterCount(m_datasetIn);
	if(lineIdx>ysize)
		return 1;
	GDALDatasetH m_datasetOut;
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetOut=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsize,bands,1,GDT_Float32,papszOptions);
	float *data=new float[xsize*bands];
	for (int i=0;i<bands;i++)
		GDALRasterIO(GDALGetRasterBand(m_datasetIn,i+1),GF_Read,0,lineIdx,xsize,1,data+i*xsize,xsize,1,GDT_Float32,0,0);
	GDALClose(m_datasetIn);
	GDALRasterIO(GDALGetRasterBand(m_datasetOut,1),GF_Write,0,0,xsize,bands,data,xsize,bands,GDT_Float32,0,0);
	GDALClose(m_datasetOut);
	delete[]data;
	return 0;
}
long  GetCutLineImg(const char*pathIn,const char* path_plant_coeff,int lineIdx,const char* pathOut)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO" );	//中文路径
	GDALDatasetH m_datasetIn;
	m_datasetIn=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_datasetIn);
	int ysize=GDALGetRasterYSize(m_datasetIn);
	int bands=GDALGetRasterCount(m_datasetIn);
	if(lineIdx>ysize)
		return 1;
	GDALDatasetH m_datasetOut;
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetOut=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsize,bands,1,GDT_Float32,papszOptions);

	float *data=new float[xsize*bands];
	float* plankData=new float[2*bands];
	char tmpchr[2048];

	ifstream ifs;
	ifs.open(path_plant_coeff);
	if(!ifs.is_open())
		return 2;
	ifs.getline(tmpchr,2048);
	for (int i=0;i<bands;i++)
	{
		ifs.getline(tmpchr,2048);
		sscanf(tmpchr,"%f%f",&plankData[2*i+0],&plankData[2*i+1]);
	}
	ifs.close();

	for (int i=0;i<bands;i++)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetIn,i+1),GF_Read,0,lineIdx,xsize,1,data+i*xsize,xsize,1,GDT_Float32,0,0);
		for (int j=0;j<xsize;j++)
		{
			data[i*xsize+j]=data[i*xsize+j]*plankData[2*i+1]+plankData[2*i+0];
		}
	}
	GDALClose(m_datasetIn);
	GDALRasterIO(GDALGetRasterBand(m_datasetOut,1),GF_Write,0,0,xsize,bands,data,xsize,bands,GDT_Float32,0,0);
	GDALClose(m_datasetOut);
	delete[]data;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************************/
/*								  数据球化变换                                                          */
/********************************************************************************************************/
/*-----------------------------数据去中心化-----------------------------*/
long Decentered(double *imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut)
{
	int i,j,k;
	double *meanvalue=NULL;
	double **covmatrix=NULL;
	long rs;

	if(imgBufferIn==NULL||imgBufferOut==NULL)
		return 1;

	meanvalue=new double[bandSum];
	covmatrix=new double*[bandSum];
	for (int i=0;i<bandSum;i++)
		covmatrix[i]=new double [bandSum];

	//rs=GetCovarianceMatrix(imgBufferIn,bandSum,bandNo,xsize,ysize,covmatrix);
	//if(0!=rs)
	//	goto ERROROUT;

	rs=GetImgMean(imgBufferIn,bandSum,bandNo,xsize,ysize,meanvalue);
	if(0!=rs)
		goto ERROROUT;

	for (i=0;i<bandSum;i++)
	{
		for(j=0;j<xsize*ysize;j++)
			imgBufferOut[j+i*xsize*ysize]=(imgBufferIn[j+i*xsize*ysize]-meanvalue[i])/*/sqrt(covmatrix[i][i])*/;
	}

ERROROUT:
	if(meanvalue==NULL)
		return 3;
	delete[]meanvalue;
	for (int i=0;i<bandSum;i++)
		delete[]covmatrix[i];
	delete[]covmatrix;
	return 0;
}

/*-----------------------------数据去相关性-----------------------------*/
long Derelated(double *imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut)
{
	int i,j,k;
	long rs;
	double **covImg=NULL;
	double *eigValue=NULL;
	double **eigMat =NULL;
	double **eigMatTemp=NULL;
	double **eigTemp=NULL;
	double **temp   =NULL;
	double **matTran=NULL;

	//调试文件
	ofstream ofs("test.txt");

	if(NULL==imgBufferIn||NULL==imgBufferOut)
		return 1;
	memset(imgBufferOut,0,sizeof(double)*bandSum*xsize*ysize);

	//数据空间的申请
	covImg=new double *[bandSum];
	eigMat=new double *[bandSum];
	eigMatTemp=new double *[bandSum];
	eigTemp=new double *[bandSum];
	temp   =new double *[bandSum];
	matTran=new double *[bandSum];
	eigValue=new double[bandSum];

	for (i=0;i<bandSum;i++)
	{
		covImg[i]=new double  [bandSum];
		eigMat[i]=new double  [bandSum];
		eigMatTemp[i]=new double [bandSum];
		eigTemp[i]=new double [bandSum];
		temp[i]  =new double [bandSum];
		matTran[i]=new double [bandSum];
		memset(temp[i],0,sizeof(double)*bandSum);
		memset(eigTemp[i],0,sizeof(double)*bandSum);
		memset(eigMatTemp[i],0,sizeof(double)*bandSum);
	}

	//获取白化矩阵
	rs=GetCovarianceMatrix(imgBufferIn,bandSum,bandNo,xsize,ysize,covImg);
	if(rs!=0)
		goto ERROROUT;

	//////////////////////////////////////////////////////////////////////////
	//调试输出

	for (int i=0;i<bandSum;i++)
	{
		for (int j=0;j<bandSum;j++)
		{
			ofs<<covImg[i][j]<<"  ";
		}
		ofs<<endl;
	}
	ofs.close();
	//////////////////////////////////////////////////////////////////////////

	rs=MatrixEigen_value_vec_Jccob(covImg,eigValue,bandSum,eigMat);
	if(rs!=0)
		goto ERROROUT;

	//特征值从小到大排列
	for (i=0;i<bandSum;i++)
	{
		for (j=0;j<int(bandSum/2);j++)
		{
			swap(eigMat[i][j],eigMat[i][bandSum-1-j]);
		}
	}


	for(i=0;i<bandSum;i++)
		eigTemp[i][i]=eigValue[i];

	//rs=Matrix_Cholesky(eigTemp,temp,bandSum);
	//if(rs!=0)
	//	goto ERROROUT;
	for (int i=0;i<bandSum;i++)
	{
		temp[i][i]=1/sqrt(eigTemp[bandSum-1-i][bandSum-1-i]);
	}
	/////////////////////////////////////////////////
	//测试数据
	//if (ofs.is_open())
	//{
	//	for (int i=0;i<bandSum;i++)
	//	{
	//		for (int j=0;j<bandSum;j++)
	//		{
	//			ofs<<covImg[i][j]<<"    ";
	//		}
	//		ofs<<endl;
	//	}
	//	ofs.close();
	//}

	///////////////////////////////////////////////////

	rs=MatrixMuti(eigMat,bandSum,bandSum,bandSum,temp,matTran);
	if(rs!=0)
		goto ERROROUT;

	//通过白化矩阵对数据进行处理
	for (i=0;i<xsize*ysize;i++)
	{
		for (j=0;j<bandSum;j++)
		{
			for (k=0;k<bandSum;k++)
			{
				imgBufferOut[i+j*xsize*ysize]+=imgBufferIn[i+k*xsize*ysize]*matTran[k][j];
			}
		}
	}


ERROROUT:
	if(NULL==covImg||NULL==eigMat||NULL==eigTemp
		||NULL==temp||NULL==matTran||NULL==eigValue||NULL==eigMatTemp)
		return 3;

	for (i=0;i<bandSum;i++)
	{
		delete []covImg[i];
		delete []eigMat[i];
		delete []eigTemp[i];
		delete []temp[i];
		delete []matTran[i];
		delete []eigMatTemp[i];
	}
	delete []covImg;
	delete []eigMat;
	delete []eigTemp;
	delete []temp ;
	delete []matTran;
	delete []eigValue;
	delete []eigMatTemp;

	return 0;
}

/*-------------------------------数据球化-------------------------------*/
void SphereTrans(double *imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut)
{
	long lError=0;
	double* m_deData =new double[xsize*ysize*bandSum];
	memset(m_deData,0,xsize*ysize*bandSum*sizeof(double));
	lError=Decentered(imgBufferIn,bandSum,bandNo,xsize,ysize,m_deData);
	if (lError!=0)
	{
		delete[]m_deData;
		printf("Decentered Error!");
		return;
	}
	lError=Derelated(m_deData,bandSum,bandNo,xsize,ysize,imgBufferOut);
	if (lError!=0)
	{
		delete[]m_deData;
		printf("Derelated Error!");
		return;
	}

	delete[]m_deData;
	return ;
}

/*----------------------------数据球化GDAL封装--------------------------*/
void SphereTrans(char* pathSrc,char* pathDst,char* pathDecenter,int* bandNo,int num)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);

	//获取实际处理波段数
	int realBand=0;
	if(bandNo==NULL)
		realBand=bands;
	else
	{
		for (int i=0;i<num;i++)
		{
			if(bandNo[i]!=0)
				realBand++;
		}
	}

	//bands=realBand;
	int *band=new int[realBand];
	double* m_Indata =new double[width*heigh*realBand];
	double* m_deData =new double[width*heigh*realBand];
	double* m_Outdata=new double[width*heigh*realBand];
	memset(m_deData,0,sizeof(double)*width*heigh*realBand);
	memset(m_Outdata,0,sizeof(double)*width*heigh*realBand);

	GDALRasterBandH m_band;

	//读数据  根据选取的波段读取数据进行数据球化变换
	if(bandNo==NULL)
	{
		for (int i=0;i<bands;i++)
		{
			band[i]=i+1;
			m_band=GDALGetRasterBand(m_datasetsrc,i+1);
			GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
		}
		Decentered(m_Indata,realBand,bandNo,width,heigh,m_deData);
		Derelated(m_deData,realBand,bandNo,width,heigh,m_Outdata);
	}
	else
	{
		for (int i=0;i<bands;i++)
		{
			if(bandNo[i]!=0)
			{

				band[i]=i+1;
				m_band=GDALGetRasterBand(m_datasetsrc,i+1);
				GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
			}
		}
		Decentered(m_Indata,realBand,bandNo,width,heigh,m_deData);
		Derelated(m_deData,realBand,bandNo,width,heigh,m_Outdata);
	}
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	//写数据将PCA变换后的数据写入文件中
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,realBand,GDT_Float64,papszOptions);
	for (int i=0;i<realBand;i++)
	{
		m_band=GDALGetRasterBand(m_datasetdst,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata+i*width*heigh,width,heigh,GDT_Float64,0,0);
	}
	GDALSetProjection(m_datasetdst,proj);
	GDALClose(m_datasetdst);
	GDALClose(m_datasetsrc);

	if (pathDecenter!=NULL)
	{
		GDALDatasetH dedataset=GDALCreate(GDALGetDriverByName("GTiff"),pathDecenter,width,heigh,realBand,GDT_Float64,papszOptions);

		for (int i=0;i<realBand;i++)
		{
			m_band=GDALGetRasterBand(dedataset,i+1);
			GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_deData+i*width*heigh,width,heigh,GDT_Float64,0,0);
		}
		GDALClose(dedataset);
	}

	delete[]m_Indata;
	delete[]m_Outdata;
	delete[]m_deData;
	delete[]band;
}


/********************************************************************************************************/
/*								          MNF变换                                                       */
/********************************************************************************************************/

/*-------------------------------获取MNF算子----------------------------*/
long GetMNFOperator(double **covmatNoise,double **covmatImg,double** mnfOperator,int size)
{
	int i;
	int rs;				// 返回值
	double **invData;
	double **temp;
	double **tempF;
	double **tempCovImg;
	double **transTempF;
	double *eigValue;
	double **eigMat;
	double **tempMat;
	ofstream ofs("test.txt");
	//检查输入
	if (covmatNoise==NULL||covmatNoise==NULL||mnfOperator==NULL)
		return 1;
	if(size<1)
		return 2;

	if(NULL==(invData=new double *[size])||NULL==(temp=new double *[size])
		||NULL==(tempCovImg=new double *[size])||NULL==(tempF=new double *[size])
		||NULL==(transTempF=new double *[size])||NULL==(eigMat=new double *[size])
		||NULL==(tempMat=new double*[size]))
		return 3;
	for(i=0;i<size;i++)
	{
		invData[i]=new double [size];
		temp[i]=new double [size];
		tempCovImg[i]=new double [size];
		tempF[i]=new double [size];
		transTempF[i]=new double [size];
		eigMat[i]  =new double[size];
		tempMat[i]=new double [size];
		memset(tempMat[i],0,sizeof(double)*size);
	}
	if (NULL==(eigValue=new double [size]))
		return 3;

	rs=MatrixEigen_value_vec_Jccob(covmatNoise,eigValue,size,eigMat);

	///////////////////////////////////////////////////////////////////////////
	//测试
	//ofstream ofile("temp.txt");
	//for (i=0;i<size;i++)
	//{
	//	for ( int j=0;j<size;j++)
	//	{
	//		ofile<<covmatNoise[i][j]<<"    ";
	//	}
	//	ofile<<endl;
	//}
	//ofile.close();
	//////////////////////////////////////////////////////////////////////////

	for (i=0;i<size;i++)
	{
		tempMat[i][i]=eigValue[i];
	}

	rs=MatrixInverse(tempMat,size,invData);
	if(rs!=0)
		goto ERROROUT;
	rs=Matrix_Cholesky(invData,tempF,size);
	if(rs!=0)
		goto ERROROUT;

	//rs=MatrixTrans(tempF,size,size,transTempF);
	rs=MatrixMuti(eigMat,size,size,size,tempF,transTempF);
	if(rs!=0)
		goto ERROROUT;

	rs=MatrixMuti(transTempF,size,size,size,covmatImg,temp);
	if(rs!=0)
		goto ERROROUT;
	rs=MatrixTrans(transTempF,size,size,eigMat);
	if(rs!=0)
		goto ERROROUT;
	rs=MatrixMuti(temp,size,size,size,eigMat,tempCovImg);
	if(rs!=0)
		goto ERROROUT;

	//求解tempCovImg的特征值和特征向量
	rs=MatrixEigen_value_vec_Jccob(tempCovImg,eigValue,size,eigMat);
	if(rs!=0)
		goto ERROROUT;

	rs=MatrixMuti(transTempF,size,size,size,eigMat,mnfOperator);
	if(rs!=0)
		goto ERROROUT;

	//rs=MatrixEigen_value_vec_InvPower(temp,eigValue,size,mnfOperator);
	//if(rs!=0)
	//	return rs;

	//清除指针
ERROROUT:
	if (invData==NULL||temp==NULL||eigValue==NULL||tempF==NULL
		||tempCovImg==NULL||transTempF==NULL||eigMat==NULL||tempMat==NULL)
		return 3;
	for (i=0;i<size;i++)
	{
		delete[]invData[i];
		delete[]temp[i];
		delete[]tempF[i];
		delete[]tempCovImg[i];
		delete[]transTempF[i];
		delete[]eigMat[i];
		delete[]tempMat[i];
	}

	delete[]invData;
	delete[]temp;
	delete[]tempF;
	delete[]tempCovImg;
	delete[]transTempF;
	delete[]eigMat;
	delete[]eigValue;
	delete[]tempMat;

	return 0;
}

/*-------------------------------进行MNF变换----------------------------*/
long  MNF(double *imgBufferIn,int bandSum,int *bandNo,
									int xsize,int ysize,double *imgBufferOut,char* path)
{
	int i,j,k;
	int rs;
	/*double *imgNoise;*/	//噪声影像
	double *imgDeal;			//处理的影像波段
	double **covNoise;
	double **covImg;
	double **mnfOperator;
	double *imgNoise;

	//检查输入
	if(imgBufferIn==NULL||imgBufferOut==NULL)
		return 1;
	if (bandSum<=1||xsize<=1||ysize<=1)
		return 2;

	//申请空间
	if(NULL==(imgNoise=new double[xsize*ysize*bandSum])||NULL==(imgDeal=new double[xsize*ysize*bandSum]))
		return 3;
	if (NULL==(covNoise=new double*[bandSum])||NULL==(covImg=new double*[bandSum])||NULL==(mnfOperator=new double*[bandSum]))
		return 3;

	for (i=0;i<bandSum;i++)
	{
		covNoise[i]=new double [bandSum];
		covImg[i]=new double [bandSum];
		mnfOperator[i]=new double [bandSum];
	}
	memset(imgBufferOut,0,sizeof(double)*xsize*ysize*bandSum);

	//提取处理影像
	//获得噪声影像
	memcpy(imgDeal,imgBufferIn,sizeof(double)*xsize*ysize*bandSum);

	/*求解似乎有点问题 先就这样吧*/
	rs=GetNoiseImg(imgDeal,bandSum,bandNo,1,1,xsize,ysize,imgNoise);
	if(rs!=0)
		return rs;

	//获取噪声和影像的协方差矩阵
	//影像数据白化
	SphereTrans(imgBufferIn,bandSum,bandNo,xsize,ysize,imgDeal);

	rs=GetCovarianceMatrix(imgDeal,bandSum,bandNo,xsize,ysize,covImg);
	if(rs!=0)
		return rs;
	rs=GetCovarianceMatrix(imgNoise,bandSum,bandNo,xsize,ysize,covNoise);
	if(rs!=0)
		return rs;

	//获取mnf变换矩阵
	rs=GetMNFOperator(covNoise,covImg,mnfOperator,bandSum);
	if(rs!=0)
		return rs;

	//进行MNF变换
	for (i=0;i<xsize*ysize;i++)
	{
		for (j=0;j<bandSum;j++)
		{
			for (k=0;k<bandSum;k++)
			{
				imgBufferOut[i+j*xsize*ysize]+=imgBufferIn[i+k*xsize*ysize]*mnfOperator[k][j];
			}
		}
	}

	//输出变换矩阵到文件中
	Matrix_Export_File(path,mnfOperator,bandSum);

	//清除指针并返回
	if (imgNoise==NULL||imgDeal==NULL||covImg==NULL||covNoise==NULL||mnfOperator==NULL)
		return 3;
	delete[]imgDeal;
	//delete[]imgNoise;
	for (i=0;i<bandSum;i++)
	{
		delete[]covImg[i];
		delete[]covNoise[i];
		delete[]mnfOperator[i];
	}
	delete[]imgNoise;
	delete[]covImg;
	delete[]covNoise;
	delete[]mnfOperator;
	return 0;
}

/*-------------------------------进行MNF反变换--------------------------*/
long InvMNF(double* imgBufferIn,int bandSum,int *bandNo,
					int xsize,int ysize,double *imgBufferOut,char *path)
{
	int i,j,k;
	int rs;
	int size;
	int bandNum=0;
	double **matTrans=NULL;
	double **InvmnfOperator=NULL;
	double *data;

	//检查输入
	if(NULL==imgBufferIn||NULL==imgBufferOut||NULL==path)
		return 1;
	if(bandSum<=1||xsize<=1||ysize<=1)
		return 2;

	//初始值
	memset(imgBufferOut,0,sizeof(double)*xsize*ysize*bandSum);
	//获得变换矩阵的大小
	if(0!=(rs=Matrix_Get_Size(path,size)))
		return rs;
	//申请空间
	matTrans=new double *[size];
	for(i=0;i<bandSum;i++)
		matTrans[i]=new double [size];
	if(NULL==matTrans)
		return 3;

	//获得变换矩阵数据
	if(0!=(rs=Matrix_Import_File(path,matTrans)))
		return rs;

	//如果bandNo为空则所有波段都未被剔除
	if (bandNo==NULL)
	{
		//申请空间
		InvmnfOperator=new double *[size];
		for(i=0;i<size;i++)
			InvmnfOperator[i]=new double [size];
		//矩阵的转置
		double **tmatTrans=new double *[size];
		for(i=0;i<size;i++)
		{
			tmatTrans[i]=new double [size];
			memcpy(tmatTrans[i],matTrans[i],sizeof(double)*size);
		}
		//MatrixTrans(matTrans,size,size,tmatTrans);
		MatrixInverse(tmatTrans,size,InvmnfOperator);

		for (i=0;i<xsize*ysize;i++)
		{
			for (j=0;j<bandSum;j++)
			{
				for (k=0;k<bandSum;k++)
				{
					imgBufferOut[i+j*xsize*ysize]+=imgBufferIn[i+k*xsize*ysize]*InvmnfOperator[k][j];
				}
			}
		}

		//删除指针
		//delete[]data;
		for (i=0;i<bandSum;i++)
		{
			delete[]matTrans[i];
			delete[]InvmnfOperator[i];
		}
		delete[]matTrans;
		delete[]InvmnfOperator;
	}
	else
	{
		//申请空间
		double **tempOperator=new double *[size];
		double **tmatTrans=new double *[size];
		for(i=0;i<size;i++)
		{
			tempOperator[i]=new double [size];
			tmatTrans[i]=new double [size];
			memcpy(tmatTrans[i],matTrans[i],sizeof(double)*size);
		}
		//MatrixTrans(matTrans,size,size,tmatTrans);
		MatrixInverse(tmatTrans,size,tempOperator);

		for (i=0;i<size;i++)
			memcpy(matTrans[i],tmatTrans[i],sizeof(double)*size);

		//获取未剔除的波段数目
		for(i=0;i<bandSum;i++)
		{
			if(bandNo[i]!=0)
				bandNum++;
		}
		//申请空间
		InvmnfOperator=new double *[bandNum];
		for(i=0;i<bandNum;i++)
			InvmnfOperator[i]=new double [size];
		for(i=0;i<size;i++)
		{
			k=0;
			for(j=0;j<size;j++)
			{
				if (bandNo[j]!=0)
				{
					InvmnfOperator[k][i]=tempOperator[i][j];
					k++;
				}
			}
		}
		//获取待处理的波段数据
		data=new double [xsize*ysize*bandNum];
		for(i=0,k=0;i<bandSum;i++)
		{
			if(bandNo[i]!=0)
			{
				memcpy(data+k*xsize*ysize,imgBufferIn+i*xsize*ysize,sizeof(double)*xsize*ysize);
				k++;
			}
		}

		//对数据进行处理
		for (i=0;i<xsize*ysize;i++)
		{
			for (j=0;j<bandSum;j++)
			{
				for (k=0;k<bandNum;k++)
				{
					imgBufferOut[i+j*xsize*ysize]+=data[i+k*xsize*ysize]*InvmnfOperator[k][j];
				}
			}
		}

		delete[]data;
		for(i=0;i<bandSum;i++)
		{
			delete[]matTrans[i];
			delete[]tmatTrans[i];
		}
		delete[]tmatTrans;
		delete[]matTrans;
		for(i=0;i<bandNum;i++)
			delete[]InvmnfOperator[i];
		delete[]InvmnfOperator;
		for (int i=0;i<size;i++)
			delete[]tempOperator[i];
		delete[]tempOperator;
	}

	return 0;
}

/*------------------------GDAL封装MNF变换和MNF反变换--------------------*/
void MNFTrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix)
{
	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);

	//获取实际处理波段数
	int realBand=0;
	if(bandNo==NULL)
		realBand=bands;
	else
	{
		for (int i=0;i<num;i++)
		{
			if(bandNo[i]!=0)
				realBand++;
		}
	}
	//bands=realBand;
	int *band=new int[realBand];
	double* m_Indata =new double[width*heigh*realBand];
	double* m_Outdata=new double[width*heigh*realBand];
	//double* m_Whiten_Data=new double[width*heigh*realBand];

	GDALRasterBandH m_band;
	//读数据  根据选取的波段读取数据进行PCA变换
	if(bandNo==NULL)
	{
		for (int i=0;i<bands;i++)
		{
			band[i]=i+1;
			m_band=GDALGetRasterBand(m_datasetsrc,i+1);
			GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
		}
		MNF(m_Indata,bands,band,width,heigh,m_Outdata,pathMartix);
	}
	else
	{
		for (int i=0;i<bands;i++)
		{
			if(bandNo[i]!=0)
			{

				band[i]=i+1;
				m_band=GDALGetRasterBand(m_datasetsrc,i+1);
				GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
			}
		}
		const char* proj=GDALGetProjectionRef(m_datasetsrc);
		MNF(m_Indata,bands,band,width,heigh,m_Outdata,pathMartix);
	}
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	//写数据将PCA变换后的数据写入文件中
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,realBand,GDT_Float64,papszOptions);
	for (int i=0;i<realBand;i++)
	{
		m_band=GDALGetRasterBand(m_datasetdst,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata+i*width*heigh,width,heigh,GDT_Float64,0,0);
	}
	GDALSetProjection(m_datasetdst,proj);
	GDALClose(m_datasetdst);
	GDALClose(m_datasetsrc);

	delete[]m_Indata;
	delete[]m_Outdata;
	delete[]band;
}

void InvMNFTrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix)
{

	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
		return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);
	int *band=new int[bands];
	double* m_Indata =new double[width*heigh*bands];
	double* m_Outdata=new double[width*heigh*bands];
	GDALRasterBandH m_band;

	//读数据
	for (int i=0;i<bands;i++)
	{
		band[i]=i+1;
		m_band=GDALGetRasterBand(m_datasetsrc,i+1);
		GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
	}
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	//PCA(m_Indata,bands,band,width,heigh,m_Outdata,pathMartix);
	if(bandNo==NULL)
		InvMNF(m_Indata,bands,bandNo,width,heigh,m_Outdata,pathMartix);
	else
		InvMNF(m_Indata,bands,bandNo,width,heigh,m_Outdata,pathMartix);

	//写数据
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,bands,GDT_Float64,papszOptions);
	GDALSetProjection(m_datasetdst,proj);
	for (int i=0;i<bands;i++)
	{
		m_band=GDALGetRasterBand(m_datasetdst,i+1);
		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata+i*width*heigh,width,heigh,GDT_Float64,0,0);
	}

	GDALClose(m_datasetdst);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata;
	delete[]band;
}

/********************************************************************************************************/
/*								  PCA变换                                                               */
/********************************************************************************************************/
/*-------------------------------获得PCA变换算子------------------------*/
long GetPCAOperator(double **covmatImg,double **pcaOperator,int size)
{
	int rs;				// 返回值
	double *eigValue;
	//检查输入
	if (covmatImg==NULL||pcaOperator==NULL)
		return 1;
	if(size<1)
		return 2;

	if(NULL==(eigValue=new double [size]))
		return 3;
	rs=MatrixEigen_value_vec_Jccob(covmatImg,eigValue,size,pcaOperator);
	if(rs!=0)
		return rs;

	//清除指针
	if (eigValue==NULL)
		return 3;
	delete[]eigValue;
	return 0;
}


/*---------------------------------进行PCA变换--------------------------*/
long PCA(double *imgBufferIn,int bandSum,int *bandNo,
					int xsize,int ysize,double *imgBufferOut,char* path)
{
	int i,j,k;
	int rs;
	double *imgDeal;		//处理的影像波段
	double **covImg;
	double **pcaOperator;

	//检查输入
	if(imgBufferIn==NULL||imgBufferOut==NULL)
		return 1;
	if (bandSum<=1||xsize<1||ysize<1)
		return 2;

	//申请空间
	if(NULL==(imgDeal=new double[xsize*ysize*bandSum]))
		return 3;
	if (NULL==(covImg=new double*[bandSum])||NULL==(pcaOperator=new double*[bandSum]))
		return 3;
	for (i=0;i<bandSum;i++)
	{
		covImg[i]=new double [bandSum];
		pcaOperator[i]=new double [bandSum];
	}
	memset(imgBufferOut,0,sizeof(double)*xsize*ysize*bandSum);

	//提取处理影像
	//获得噪声影像
	memcpy(imgDeal,imgBufferIn,sizeof(double)*xsize*ysize*bandSum);

	//影像的协方差矩阵
	rs=GetCovarianceMatrix(imgDeal,bandSum,bandNo,xsize,ysize,covImg);
	if(rs!=0)
		return rs;

	//获取PCA变换矩阵
	rs=GetPCAOperator(covImg,pcaOperator,bandSum);
	if(rs!=0)
		return rs;

	//进行PCA变换
	for (i=0;i<xsize*ysize;i++)
	{
		for (j=0;j<bandSum;j++)
		{
			for (k=0;k<bandSum;k++)
			{
				imgBufferOut[i+j*xsize*ysize]+=imgBufferIn[i+k*xsize*ysize]*pcaOperator[k][j];
			}
		}
	}

	//将MNF变换矩阵输出
	if(path!=NULL)
		Matrix_Export_File(path,pcaOperator,bandSum);
	else
		printf("注意：未取得转换矩阵!\n");

	//清除指针并返回
	if (imgDeal==NULL||covImg==NULL||pcaOperator==NULL)
		return 3;
	delete[]imgDeal;
	for (i=0;i<bandSum;i++)
	{
		delete[]covImg[i];
		delete[]pcaOperator[i];
	}
	delete[]covImg;
	delete[]pcaOperator;
	return 0;
}


/*--------------------------------进行PCA反变换-------------------------*/
long InvPCA(double *imgBufferIn,int bandSum,int *bandNo,
						int xsize,int ysize,double *imgBufferOut,char* path)
{
	int i,j,k;
	int rs;
	int size;
	int bandNum=0;
	double **matTrans=NULL;
	double **InvpcaOperator=NULL;
	double *data;

	//检查输入
	if(NULL==imgBufferIn||NULL==imgBufferOut||NULL==path)
		return 1;
	if(bandSum<=1||xsize<=1||ysize<=1)
		return 2;

	//初始值
	memset(imgBufferOut,0,sizeof(double)*xsize*ysize*bandSum);
	//获得变换矩阵的大小
	if(0!=(rs=Matrix_Get_Size(path,size)))
		return rs;
	//申请空间
	matTrans=new double *[size];
	for(i=0;i<bandSum;i++)
		matTrans[i]=new double [size];
	if(NULL==matTrans)
		return 3;

	//获得变换矩阵数据
	if(0!=(rs=Matrix_Import_File(path,matTrans)))
		return rs;

	//如果bandNo为空则所有波段都未被剔除
	if (bandNo==NULL)
	{
		//申请空间
		InvpcaOperator=new double *[size];
		for(i=0;i<size;i++)
			InvpcaOperator[i]=new double [size];
		//矩阵的转置
		double **tmatTrans=new double *[size];
		for(i=0;i<size;i++)
		{
			tmatTrans[i]=new double [size];
		}
		MatrixTrans(matTrans,size,size,tmatTrans);
		MatrixInverse(tmatTrans,size,InvpcaOperator);

		for (i=0;i<xsize*ysize;i++)
		{
			for (j=0;j<bandSum;j++)
			{
				for (k=0;k<bandSum;k++)
				{
					imgBufferOut[i+j*xsize*ysize]+=imgBufferIn[i+k*xsize*ysize]*InvpcaOperator[k][j];
				}
			}
		}

		//删除指针
		//delete[]data;
		for (i=0;i<bandSum;i++)
		{
			delete[]matTrans[i];
			delete[]InvpcaOperator[i];
		}
		delete[]matTrans;
		delete[]InvpcaOperator;
	}
	else
	{
		//申请空间
		double **tempOperator=new double *[size];
		double **tmatTrans   =new double *[size];
		for(i=0;i<size;i++)
		{
			tempOperator[i]=new double [size];
			tmatTrans[i]   =new double [size];
		}
		MatrixTrans(matTrans,size,size,tmatTrans);
		MatrixInverse(tmatTrans,size,tempOperator);

		for (i=0;i<size;i++)
			memcpy(matTrans[i],tmatTrans[i],sizeof(double)*size);

		//获取未剔除的波段数目
		for(i=0;i<bandSum;i++)
		{
			if(bandNo[i]!=0)
				bandNum++;
		}
		//申请空间
		InvpcaOperator=new double *[bandNum];
		for(i=0;i<bandNum;i++)
			InvpcaOperator[i]=new double [size];
		for(i=0;i<size;i++)
		{
			k=0;
			for(j=0;j<size;j++)
			{
				if (bandNo[j]!=0)
				{
					InvpcaOperator[k][i]=tempOperator[i][j];
					k++;
				}
			}
		}
		//获取待处理的波段数据
		data=new double [xsize*ysize*bandNum];
		for(i=0,k=0;i<bandSum;i++)
		{
			if(bandNo[i]!=0)
			{
				memcpy(data+k*xsize*ysize,imgBufferIn+i*xsize*ysize,sizeof(double)*xsize*ysize);
				k++;
			}
		}

		//对数据进行处理
		for (i=0;i<xsize*ysize;i++)
		{
			for (j=0;j<bandSum;j++)
			{
				for (k=0;k<bandNum;k++)
				{
					imgBufferOut[i+j*xsize*ysize]+=data[i+k*xsize*ysize]*InvpcaOperator[k][j];
				}
			}
		}

		delete[]data;
		for(i=0;i<bandSum;i++)
		{
			delete[]matTrans[i];
			delete[]tmatTrans[i];
		}
		delete[]tmatTrans;
		delete[]matTrans;
		for(i=0;i<bandNum;i++)
			delete[]InvpcaOperator[i];
		delete[]InvpcaOperator;
		for (int i=0;i<size;i++)
			delete[]tempOperator[i];
		delete[]tempOperator;
	}

	return 0;

}


/*------------------------GDAL封装PCA变换和PCA反变换--------------------*/
void PCATrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix)
 {
 	GDALAllRegister();
 	GDALDatasetH m_datasetsrc,m_datasetdst;
 	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
 	if(!m_datasetsrc)
 		return ;

 	//获取数据和影像信息
 	int width=GDALGetRasterBandXSize(m_datasetsrc);
 	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
 	int bands=GDALGetRasterCount(m_datasetsrc);

	//获取实际处理波段数
	int realBand=0;
	if(bandNo==NULL)
		realBand=bands;
	else
	{
		for (int i=0;i<num;i++)
		{
			if(bandNo[i]!=0)
				realBand++;
		}
	}
	//bands=realBand;
 	int *band=new int[realBand];
 	double* m_Indata =new double[width*heigh*realBand];
 	double* m_Outdata=new double[width*heigh*realBand];
 	GDALRasterBandH m_band;

	//读数据  根据选取的波段读取数据进行PCA变换
	if(bandNo==NULL)
	{
		for (int i=0;i<bands;i++)
		{
			band[i]=i+1;
			m_band=GDALGetRasterBand(m_datasetsrc,i+1);
			GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
		}
		PCA(m_Indata,bands,band,width,heigh,m_Outdata,pathMartix);
	}
	else
	{
		for (int i=0;i<bands;i++)
		{
			if(bandNo[i]!=0)
			{

				band[i]=i+1;
				m_band=GDALGetRasterBand(m_datasetsrc,i+1);
				GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
			}
		}
		const char* proj=GDALGetProjectionRef(m_datasetsrc);
		PCA(m_Indata,realBand,band,width,heigh,m_Outdata,pathMartix);
	}
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
 	//写数据将PCA变换后的数据写入文件中
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
 	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,realBand,GDT_Float64,papszOptions);
 	for (int i=0;i<realBand;i++)
 	{
 		m_band=GDALGetRasterBand(m_datasetdst,i+1);
 		GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata+i*width*heigh,width,heigh,GDT_Float64,0,0);
 	}
 	GDALSetProjection(m_datasetdst,proj);
 	GDALClose(m_datasetdst);
 	GDALClose(m_datasetsrc);

 	delete[]m_Indata;
 	delete[]m_Outdata;
 	delete[]band;
 }

void InvPCATrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix)
{

	GDALAllRegister();
	GDALDatasetH m_datasetsrc,m_datasetdst;
	m_datasetsrc=GDALOpen(pathSrc,GA_ReadOnly);
	if(!m_datasetsrc)
	 	return ;

	//获取数据和影像信息
	int width=GDALGetRasterBandXSize(m_datasetsrc);
	int heigh=GDALGetRasterBandYSize(m_datasetsrc);
	int bands=GDALGetRasterCount(m_datasetsrc);
	int *band=new int[bands];
	double* m_Indata =new double[width*heigh*bands];
	double* m_Outdata=new double[width*heigh*bands];
	GDALRasterBandH m_band;

	//读数据
	for (int i=0;i<bands;i++)
	{
	 	band[i]=i+1;
	 	m_band=GDALGetRasterBand(m_datasetsrc,i+1);
	 	GDALRasterIO(m_band,GF_Read,0,0,width,heigh,m_Indata+i*width*heigh,width,heigh,GDT_Float64,0,0);
	}
	const char* proj=GDALGetProjectionRef(m_datasetsrc);
	//PCA(m_Indata,bands,band,width,heigh,m_Outdata,pathMartix);
	if(bandNo==NULL)
		InvPCA(m_Indata,bands,bandNo,width,heigh,m_Outdata,pathMartix);
	else
		InvPCA(m_Indata,bands,bandNo,width,heigh,m_Outdata,pathMartix);

	//写数据
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetdst=GDALCreate(GDALGetDriverByName("GTiff"),pathDst,width,heigh,bands,GDT_Float64,papszOptions);
	GDALSetProjection(m_datasetdst,proj);
	for (int i=0;i<bands;i++)
	{
	 	m_band=GDALGetRasterBand(m_datasetdst,i+1);
	 	GDALRasterIO(m_band,GF_Write,0,0,width,heigh,m_Outdata+i*width*heigh,width,heigh,GDT_Float64,0,0);
	}

	GDALClose(m_datasetdst);
	GDALClose(m_datasetsrc);
	delete[]m_Indata;
	delete[]m_Outdata;
	delete[]band;
}


/********************************************************************************************************/
/*						                        RXD算法                                                 */
/********************************************************************************************************/
void RXD(double *imgBufferIn,double* imgBufferOut,int xsize,int ysize,int bands,int bgWndsize,int tWndsize)
{
	int xtwnd,ytwnd;	//目标窗口的位置 相对于背景窗口
	int xbwnd,ybwnd;	//背景窗口的位置 相对于影像
	double thresthold;	//阈值

	/*
		数据空间的申请
	*/
	double* bwndmean=NULL;	//背景窗口均值
	double* twndmean=NULL;	//目标窗口的均值
	double* bgwndPixels=NULL;//背获取背景像素

	double** bcovMat=NULL;	//背景窗口的协方差矩阵
	double** bInvcovMat=NULL;//背景协方差矩阵的逆矩阵
	double** eigMat=NULL;	//特征向量矩阵
	double* eigValue=NULL;	//特征值

	bwndmean   =new double[bands];memset(bwndmean,0,sizeof(double)*bands);
	twndmean   =new double[bands];memset(twndmean,0,sizeof(double)*bands);
	bgwndPixels=new double[bgWndsize*bgWndsize*bands];
	memset(imgBufferOut,0,sizeof(double)*xsize*ysize);

	bcovMat    =new double*[bands];
	bInvcovMat =new double*[bands];
	eigMat     =new double*[bands];
	for (int i =0;i<bands;i++)
	{
		bcovMat[i]   =new double[bands];
		bInvcovMat[i]=new double[bands];
		eigMat[i]    =new double[bands];
	}
	eigValue  =new double[bands];
	if(bcovMat==NULL||bInvcovMat==NULL||eigMat==NULL||eigValue==NULL||bgwndPixels==NULL)
		return ;
	//获取影像每一个波段
	int xwndsize,ywndsize;
	for (xbwnd=0;xbwnd<xsize;xbwnd+=bgWndsize-tWndsize)
	{
		for (ybwnd=0;ybwnd<ysize;ybwnd+=bgWndsize-tWndsize)
		{
			xwndsize=xsize-xbwnd;
			ywndsize=ysize-ybwnd;

			//判断是否为边缘 对于边缘特殊处理
			if(xwndsize<bgWndsize&&ywndsize<bgWndsize)
			{
				int tempbgxWndsize=xwndsize;
				int tempbgyWndsize=ywndsize;
				double *tempPixels =NULL;
				tempPixels=new double[tempbgxWndsize*tempbgyWndsize];
				tempPixels=GetpartImage(imgBufferIn,bands,xsize,ysize,xbwnd,ybwnd,tempbgxWndsize,tempbgyWndsize);

				//获取了一个背景区域 则计算背景区域所有像素
				bgwndPixels=GetpartImage(imgBufferIn,bands,xsize,ysize,xbwnd,ybwnd,bgWndsize,bgWndsize);
				if(bgwndPixels==NULL)
					return;

				long lError=0;
				//计算背景影像的协方差、特征值特征向量、均值
				lError=GetImgMean(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bwndmean);
				if(lError!=0)
					return;
				lError=GetCovarianceMatrix(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bcovMat);
				if(lError!=0)
					return;
				lError=MatrixInverse(bcovMat,bands,bInvcovMat);
				if(lError!=0)
					return;
				lError=MatrixEigen_value_vec_Jccob(bInvcovMat,eigValue,bands,eigMat);
				if(lError!=0)
					return;


				//获取中心像素的均值
				//目标窗口在背景窗口的位置的移动
				for (xtwnd=0;xtwnd<tempbgxWndsize-tWndsize-1;xtwnd++)
				{
					for (ytwnd=0;ytwnd<tempbgyWndsize-tWndsize-1;ytwnd++)
					{
						//计算目标窗口的均值
						for (int i=0;i<tWndsize;i++)
						{
							for (int j=0;j<tWndsize;j++)
							{
								//对所有波段
								for (int k=0;k<bands;k++)
								{
									twndmean[k]+=bgwndPixels[(ytwnd+j)*bgWndsize+xtwnd+j+k*bgWndsize*bgWndsize]/double(tWndsize)/double(tWndsize);
								}
							}
						}

						//计算求解结果
						for (int i=0;i<bands;i++)
						{
							//减去背景的均值
							imgBufferOut[(ybwnd+ytwnd+tWndsize/2)*xsize+xbwnd+xtwnd+tWndsize/2]+=(twndmean[i]-bwndmean[i])*eigMat[i][0];
						}
						memset(twndmean,0,sizeof(double)*bands);

					}
				}//背景框中中心像素求解结束

				memset(bwndmean,0,sizeof(double)*bands);
				delete[]tempPixels;tempPixels=NULL;
			}
			else if (xwndsize>bgWndsize&&ywndsize<bgWndsize)
			{
				int tempbgxWndsize=bgWndsize;
				int tempbgyWndsize=ywndsize;
				double *tempPixels =NULL;
				tempPixels=new double[tempbgxWndsize*tempbgyWndsize];
				tempPixels=GetpartImage(imgBufferIn,bands,xsize,ysize,xbwnd,ybwnd,tempbgxWndsize,tempbgyWndsize);
				if(tempPixels==NULL)
					return;

				long lError=0;
				//计算背景影像的协方差、特征值特征向量、均值
				lError=GetImgMean(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bwndmean);
				if(lError!=0)
					return;
				lError=GetCovarianceMatrix(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bcovMat);
				if(lError!=0)
					return;
				lError=MatrixInverse(bcovMat,bands,bInvcovMat);
				if(lError!=0)
					return;
				lError=MatrixEigen_value_vec_Jccob(bInvcovMat,eigValue,bands,eigMat);
				if(lError!=0)
					return;


				//获取中心像素的均值
				//目标窗口在背景窗口的位置的移动
				for (xtwnd=0;xtwnd<tempbgxWndsize-tWndsize;xtwnd++)
				{
					for (ytwnd=0;ytwnd<tempbgyWndsize-tWndsize;ytwnd++)
					{
						//计算目标窗口的均值
						for (int i=0;i<tWndsize;i++)
						{
							for (int j=0;j<tWndsize;j++)
							{
								//对所有波段
								for (int k=0;k<bands;k++)
								{
									twndmean[k]+=bgwndPixels[(ytwnd+j)*bgWndsize+xtwnd+j+k*bgWndsize*bgWndsize]/double(tWndsize)/double(tWndsize);
								}
							}
						}

						//计算求解结果
						for (int i=0;i<bands;i++)
						{
							//没有减去背景的均值
							imgBufferOut[(ybwnd+ytwnd+tWndsize/2)*xsize+xbwnd+xtwnd+tWndsize/2]+=(twndmean[i]-bwndmean[i])*eigMat[i][0];
						}
						memset(twndmean,0,sizeof(double)*bands);

					}
				}//背景框中中心像素求解结束

				memset(bwndmean,0,sizeof(double)*bands);
				delete[]tempPixels;tempPixels=NULL;
			}
			else if (xwndsize<bgWndsize&&ywndsize>bgWndsize)
			{
				int tempbgxWndsize=xwndsize;
				int tempbgyWndsize=bgWndsize;
				double *tempPixels =NULL;
				tempPixels=new double[tempbgxWndsize*tempbgyWndsize];
				tempPixels=GetpartImage(imgBufferIn,bands,xsize,ysize,xbwnd,ybwnd,tempbgxWndsize,tempbgyWndsize);

				//获取了一个背景区域 则计算背景区域所有像素
				if(tempPixels==NULL)
					return;

				long lError=0;
				//计算背景影像的协方差、特征值特征向量、均值
				lError=GetImgMean(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bwndmean);
				if(lError!=0)
					return;
				lError=GetCovarianceMatrix(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bcovMat);
				if(lError!=0)
					return;
				lError=MatrixInverse(bcovMat,bands,bInvcovMat);
				if(lError!=0)
					return;
				lError=MatrixEigen_value_vec_Jccob(bInvcovMat,eigValue,bands,eigMat);
				if(lError!=0)
					return;


				//获取中心像素的均值
				//目标窗口在背景窗口的位置的移动
				for (xtwnd=0;xtwnd<tempbgxWndsize-tWndsize;xtwnd++)
				{
					for (ytwnd=0;ytwnd<tempbgyWndsize-tWndsize;ytwnd++)
					{
						//计算目标窗口的均值
						for (int i=0;i<tWndsize;i++)
						{
							for (int j=0;j<tWndsize;j++)
							{
								//对所有波段
								for (int k=0;k<bands;k++)
								{
									twndmean[k]+=bgwndPixels[(ytwnd+j)*bgWndsize+xtwnd+j+k*bgWndsize*bgWndsize]/double(tWndsize)/double(tWndsize);
								}
							}
						}

						//计算求解结果
						for (int i=0;i<bands;i++)
						{
							//没有减去背景的均值
							imgBufferOut[(ybwnd+ytwnd+tWndsize/2)*xsize+xbwnd+xtwnd+tWndsize/2]+=(twndmean[i]-bwndmean[i])*eigMat[i][0];
						}
						memset(twndmean,0,sizeof(double)*bands);

					}
				}//背景框中中心像素求解结束

				memset(bwndmean,0,sizeof(double)*bands);
				delete[]tempPixels;tempPixels=NULL;
			}
			else
			{
				//获取了一个背景区域 则计算背景区域所有像素
				bgwndPixels=GetpartImage(imgBufferIn,bands,xsize,ysize,xbwnd,ybwnd,bgWndsize,bgWndsize);
				if(bgwndPixels==NULL)
					return;

				long lError=0;
				//计算背景影像的协方差、特征值特征向量、均值
				lError=GetImgMean(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bwndmean);
				if(lError!=0)
					return;
				lError=GetCovarianceMatrix(bgwndPixels,bands,NULL,bgWndsize,bgWndsize,bcovMat);
				if(lError!=0)
					return;
				//ofstream ofs("test.txt");
				//for (int i=0;i<bands;i++)
				//{
				//	for (int j=0;j<bands;j++)
				//	{
				//		ofs<<bcovMat[i][j]<<"  ";
				//	}
				//	ofs<<endl;
				//}
				//ofs.close();
				lError=MatrixInverse(bcovMat,bands,bInvcovMat);
				if(lError!=0)
					return;
				lError=MatrixEigen_value_vec_Jccob(bInvcovMat,eigValue,bands,eigMat);
				if(lError!=0)
					return;

				//获取中心像素的均值
				//目标窗口在背景窗口的位置的移动
				for (xtwnd=0;xtwnd<bgWndsize-tWndsize;xtwnd++)
				{
					for (ytwnd=0;ytwnd<bgWndsize-tWndsize;ytwnd++)
					{
						//计算目标窗口的均值
						for (int i=0;i<tWndsize;i++)
						{
							for (int j=0;j<tWndsize;j++)
							{
								//对所有波段
								for (int k=0;k<bands;k++)
								{
									twndmean[k]+=bgwndPixels[(ytwnd+j)*bgWndsize+xtwnd+j+k*bgWndsize*bgWndsize]/double(tWndsize)/double(tWndsize);
								}
							}
						}

						//计算求解结果
						for (int i=0;i<bands;i++)
						{
							//没有减去背景的均值
							imgBufferOut[(ybwnd+ytwnd+tWndsize/2)*xsize+xbwnd+xtwnd+tWndsize/2]+=(twndmean[i]-bwndmean[i])*eigMat[i][0];
						}
						memset(twndmean,0,sizeof(double)*bands);

					}
				}//背景框中中心像素求解结束

				memset(bwndmean,0,sizeof(double)*bands);
			}
		}
	}//所有背景框求解结束

	//清空所有变量
	delete[]bwndmean;bwndmean=NULL;
	delete[]twndmean;twndmean=NULL;
	delete[]bgwndPixels;bgwndPixels=NULL;
	delete[]eigValue;eigValue=NULL;

	for (int i =0;i<bands;i++)
	{
		delete[]bcovMat[i];
		delete[]bInvcovMat[i];
		delete[]eigMat[i];
	}
	delete[]bcovMat;
	delete[]bInvcovMat;
	delete[]eigMat;

}

/*----------------------------GDAL封装----------------------------------*/
void RXD(char* pathSrc,char* pathDst,int bgWndsize,int tWndsize)
{
	GDALAllRegister();
	GDALDatasetH m_datasetIn;
	GDALDatasetH m_datasetOut;

	m_datasetIn=GDALOpen(pathSrc,GA_ReadOnly);
	int bands=GDALGetRasterCount(m_datasetIn);
	int xsize=GDALGetRasterXSize(m_datasetIn);
	int ysize=GDALGetRasterYSize(m_datasetIn);

	double *dataIn=new double[xsize*ysize*bands];
	double *dataOut=new double[xsize*ysize];
	for (int i=0;i<bands;i++)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetIn,i+1),GF_Read,0,0,xsize,ysize,dataIn+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	}

	RXD(dataIn,dataOut,xsize,ysize,bands,bgWndsize,tWndsize);

	const char* proj=GDALGetProjectionRef(m_datasetIn);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetOut=GDALCreate(GDALGetDriverByName("HFA"),pathDst,xsize,ysize,1,GDT_Float64,papszOptions);
	GDALRasterIO(GDALGetRasterBand(m_datasetOut,1),GF_Write,0,0,xsize,ysize,dataOut,xsize,ysize,GDT_Float64,0,0);
	GDALSetProjection(m_datasetOut,proj);
	GDALClose(m_datasetIn);
	GDALClose(m_datasetOut);
	delete[]dataIn;
	delete[]dataOut;

}

/********************************************************************************************************/
/*						                        FastICA                                                 */
/********************************************************************************************************/
//tan函数的转换
long g_tanhFunction(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output)
{
	double temp=0;
	if(x_output==NULL)
		return -1;
	for (int i=0;i<xsize*ysize;i++)
	{
		temp=0;
		for (int j=0;j<bands;j++)
		{
			temp+=w[j]*x_imput[j*xsize*ysize+i];
		}
		x_output[i]=tanh(temp);
	}
	return 0;
}
long g_derivative_tanh(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output)
{
	double *wtemp=NULL;
	double temp=0;
	wtemp   =new double [bands];
	if(x_output==NULL)
		return -1;
	for (int i=0;i<bands;i++)
	{
		wtemp[i]=tanh(w[i]);
	}
	for (int i=0;i<xsize*ysize;i++)
	{
		temp=0;
		for (int j=0;j<bands;j++)
		{
			temp+=wtemp[j]*x_imput[j*xsize*ysize+i];
		}
		x_output[i]=1-temp*temp;
	}

	delete[]wtemp;
	return 0;
}

//幂函数的转换
long g_power3Function(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output)
{
	if(x_output==NULL)
		return -1;
	for (int i=0;i<xsize*ysize;i++)
		x_output[i]=x_imput[i]*x_imput[i]*x_imput[i];
	return 0;
}
long g_derivative_power3(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output)
{
	if(x_output==NULL)
		return -1;
	for (int i=0;i<xsize*ysize;i++)
		x_output[i]=3.0f*x_imput[i]*x_imput[i];
	return 0;
}

/*-----------------------------FastICA算法-------------------------*/
long FastICA(double* imgBufferIn,int xsize,int ysize,int bandSum,int IpNum,double* imgBufferOut)
{
	if(IpNum>bandSum)
		return -1;
	if(imgBufferIn==NULL||imgBufferOut==NULL)
		return -2;

	int iteratorNum;
	int maxIterator=100;
	double sumline=0;

	//数据白化
	double* imgBufferWhiten=new double[xsize*ysize*bandSum];
	//memcpy(imgBufferWhiten,imgBufferIn,sizeof(double)*xsize*ysize*bandSum);
	SphereTrans(imgBufferIn,bandSum,NULL,xsize,ysize,imgBufferWhiten);

	//权重矩阵
	double* weighMat=new double[bandSum*IpNum];
	double* weighp  =new double[bandSum];memset(weighp,0,sizeof(double)*bandSum);
	double* weighpp =new double[bandSum];memset(weighpp,0,sizeof(double)*bandSum);
	double* lastwp  =new double[bandSum];memset(lastwp,0,sizeof(double)*bandSum);

	double* tempadd  =new double[bandSum];
	double* tempminus=new double[bandSum];

	double *temp1=new double[xsize*ysize];memset(temp1,0,sizeof(double)*xsize*ysize);
	double *temp2=new double[xsize*ysize];memset(temp2,0,sizeof(double)*xsize*ysize);

	//随机权重函数
	double temp=(double)rand();
	for (int i=0;i<IpNum*bandSum;i++)
		weighMat[i]= (double)rand() / RAND_MAX ;

	for (int i=0;i<IpNum;i++)
	{
		sumline=0;
		iteratorNum=0;
		memset(lastwp,0,sizeof(double)*bandSum);
		memset(weighp,0,sizeof(double)*bandSum);
		memset(weighpp,0,sizeof(double)*bandSum);

		//获取第i列权重
		for (int j=0;j<bandSum;j++)
		{
			weighp[j]=weighMat[j*IpNum+i];
			sumline +=weighMat[j*IpNum+i]*weighMat[j*IpNum+i];
		}

		//归一化第一列
		sumline=sqrt(sumline);
		for (int j=0;j<bandSum;j++)
			weighMat[j*IpNum+i]/=sumline;

		double maxadd=0;
		double maxminu=0;

		do
		{
		    printf("\r迭代%d次",iteratorNum);
			memset(tempadd,0,sizeof(bandSum));
			memset(tempminus,0,sizeof(bandSum));

			iteratorNum++;
			memcpy(lastwp,weighp,sizeof(double)*bandSum);

			for (int j=0;j<bandSum;j++)
			{
				double mean1=0.0f,mean2=0.0f;

				memset(temp1,0,sizeof(double)*xsize*ysize);
				memset(temp2,0,sizeof(double)*xsize*ysize);

				g_tanhFunction(lastwp,imgBufferWhiten,bandSum,xsize,ysize,temp1);
				for (int k=0;k<xsize*ysize;k++)
				{
					temp1[k]=temp1[k]*imgBufferWhiten[k+j*xsize*ysize];
				}
				g_derivative_tanh(lastwp,imgBufferWhiten,bandSum,xsize,ysize,temp2);

				GetImgMean(temp1,1,NULL,xsize,ysize,&mean1);
				GetImgMean(temp2,1,NULL,xsize,ysize,&mean2);
				mean2*=lastwp[j];
				weighp[j]=mean1-mean2;
			}

			memset(weighpp,0,sizeof(double)*bandSum);
			for (int j=0;j<=i-1;j++)
			{
				double temp=0;
				for (int l=0;l<bandSum;l++)
				{
					temp+=weighp[l]*weighMat[l*IpNum+j];
				}

				for (int l=0;l<bandSum;l++)
				{
					weighpp[l]+=temp*weighMat[l*IpNum+j];
				}
			}

			sumline=0;
			for (int j=0;j<bandSum;j++)
			{
				weighp[j]=weighp[j]-weighpp[j];
				sumline+=weighp[j]*weighp[j];
			}
			for(int j=0;j<bandSum;j++)
				weighp[j]/=sqrt(sumline);
			for (int j=0;j<bandSum;j++)
			{
				tempadd[j]  =lastwp[j]+weighp[j];
				tempminus[j]=lastwp[j]-weighp[j];
			}

			maxadd =*max_element(tempadd,tempadd+bandSum);
			maxminu=*max_element(tempminus,tempminus+bandSum);

		} while (abs(maxminu)>0.01/*&&abs(maxadd)>0.0001*/&&iteratorNum<maxIterator);

		for (int j=0;j<bandSum;j++)
		{
			weighMat[j*IpNum+i]=weighp[j];
		}
	}
    printf("\n");

	//进行转换
	memset(imgBufferOut,0,sizeof(double)*xsize*ysize*IpNum);
	for (int j=0;j<xsize*ysize;j++)
	{
		for (int k=0;k<IpNum;k++)
		{
			for (int i=0;i<bandSum;i++)
			{
				imgBufferOut[k*xsize*ysize+j]+=imgBufferWhiten[i*xsize*ysize+j]*weighMat[i*IpNum+k];
			}
		}
	}

	//清除指针
	delete[]weighMat;	weighMat=NULL;
	delete[]weighp;		weighp =NULL;
	delete[]weighpp;	weighpp=NULL;
	delete[]lastwp;		lastwp=NULL;
	delete[]imgBufferWhiten;imgBufferWhiten=NULL;

	delete[]tempadd;	tempadd=NULL;
	delete[]tempminus;	tempminus=NULL;

	delete[]temp1;temp1=NULL;
	delete[]temp2;temp2=NULL;

	return 0;

}

/*---------------------FastICA算法GDAL封装-------------------------*/
void FastICA(char* pathSrc,char* pathDst,int IpNum)
{
	GDALAllRegister();
	GDALDatasetH m_datasetIn;
	GDALDatasetH m_datasetOut;

	m_datasetIn=GDALOpen(pathSrc,GA_ReadOnly);
	int bands=GDALGetRasterCount(m_datasetIn);
	int xsize=GDALGetRasterXSize(m_datasetIn);
	int ysize=GDALGetRasterYSize(m_datasetIn);

	if (IpNum>bands)
		return ;

	double *dataIn=new double[xsize*ysize*bands];
	double *dataOut=new double[xsize*ysize*IpNum];
	for (int i=0;i<bands;i++)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetIn,i+1),GF_Read,0,0,xsize,ysize,dataIn+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	}
	FastICA(dataIn,xsize,ysize,bands,IpNum,dataOut);

	const char* proj=GDALGetProjectionRef(m_datasetIn);
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	m_datasetOut=GDALCreate(GDALGetDriverByName("HFA"),pathDst,xsize,ysize,IpNum,GDT_Float64,papszOptions);
	for (int i=0;i<IpNum;i++)
	{
		GDALRasterIO(GDALGetRasterBand(m_datasetOut,i+1),GF_Write,0,0,xsize,ysize,dataOut+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	}
	GDALSetProjection(m_datasetOut,proj);

	GDALClose(m_datasetIn);
	GDALClose(m_datasetOut);
}


/********************************************************************************************************/
/*						                   FastFinder算法                                               */
/********************************************************************************************************/
//计算单形体体积
long SingleVloumeFunc(double* data,double*dataCal,double* dataTra,double* dataMul,int bands,int number,double&det)
{
	//简化算法 并根据数据进行缩放 防止数据越界
	if(data==NULL||dataCal==NULL||dataTra==NULL||dataMul==NULL)
		return -1;

	det     =0;		//矩阵行列式

	//对原始数据进行归一化
	double* tempdata=new double[bands*number];
	for (int i=0;i<number*bands;i++)
	{
		tempdata[i]=data[i]/10.0f;
	}

	double total=0.0f;
	long lError   =0;

	for (int i=0;i<number-1;i++)
	{
		for (int j=0;j<bands;j++)
		{
			dataCal[i*bands+j]=tempdata[(i+1)*bands+j]-tempdata[j];
		}
	}

	//错误处理
 	lError=MatrixTrans(dataCal,bands,number-1,dataTra);
 	lError=MatrixMuti(dataTra,number-1,bands,number-1,dataCal,dataMul);

 	//归一化数据使求解比较稳定
//  	for (int i=0;i<(number-1)*(number-1);i++)
//  		total+=dataMul[i];
//  	for (int i=0;i<(number-1)*(number-1);i++)
//  		dataMul[i]=dataMul[i]*100*(number-1)*(number-1)/total;

 	lError=MatrixDet(dataMul,number-1,det);
	delete[]tempdata;
	return 0;
}

//fast finder算法进行端元提取
long FastFinder(double* imgBuffer,double* endmember,int xsize,int ysize,int bandSum,int endMemberNum)
{
	long LError=0;
	if (imgBuffer==NULL||endmember==NULL)
		return -1;

	//随机给出端元
	double det=0;			//单形体体积
	double iterdet=0;		//迭代单形体体积

	double* randEndmember=NULL;
	double* tempEnd=NULL;
	double* temp=NULL;

	double *dataCal=NULL;	//数据
	double *dataTra=NULL;	//数据转置
	double *dataMul=NULL;	//矩阵乘积

	if((dataTra=new double[bandSum*(endMemberNum-1)])==NULL)
		return -1;
	if((dataMul=new double[(endMemberNum-1)*(endMemberNum-1)])==NULL)
		return -1;
	if((dataCal=new double[bandSum*(endMemberNum-1)])==NULL)
		return -1;
	tempEnd=new double[bandSum];
	temp=new double[bandSum];
	randEndmember=new double[bandSum*endMemberNum];

	rand();
	//随机获取位置数据计算端元光谱
	for (int i=0;i<endMemberNum;i++)
	{
		int pos=rand()%xsize*ysize;
		for (int j=0;j<bandSum;j++)
		{
			randEndmember[i*bandSum+j]=imgBuffer[j*xsize*ysize+pos];
		}
	}

	//计算单形体体积
	LError=SingleVloumeFunc(randEndmember,dataCal,dataTra,dataMul,bandSum,endMemberNum,det);
	if(LError!=0)
		return -2;

	//遍历所有影像获取顶点
	int maxMember=0;
	for (int i=0;i<xsize*ysize;i++)
	{
		printf("\r处理像素%d/%d...",xsize*ysize,i+1);
		//获取该点影像光谱
		for (int j=0;j<bandSum;j++)
			tempEnd[j]=imgBuffer[j*xsize*ysize+i];

		//分别替代各个影像波段获取最大体积
		iterdet=det;
		maxMember=-1;
		for (int j=0;j<endMemberNum;j++)
		{
// 			for(int k=0;k<bandSum;k++)
// 			{
// 				temp[k]=randEndmember[j*bandSum+k];
// 				randEndmember[j*bandSum+k]=tempEnd[k];
// 			}
			memcpy(temp,randEndmember+j*bandSum,sizeof(double)*bandSum);
			memcpy(randEndmember+j*bandSum,tempEnd,sizeof(double)*bandSum);
			LError=SingleVloumeFunc(randEndmember,dataCal,dataTra,dataMul,bandSum,endMemberNum,det);
			if(LError!=0)
				return -2;
			if(det>iterdet)
			{
				maxMember=j;
			}
			iterdet = det;

			//for(int k=0;k<bandSum;k++)
			//	randEndmember[j*bandSum+k]=temp[k];
			memcpy(randEndmember+j*bandSum,temp,sizeof(double)*bandSum);
		}

		//将像素值赋给最大体积的端元
		if(maxMember!=-1)
		{
			for(int k=0;k<bandSum;k++)
				randEndmember[maxMember*bandSum+k]=tempEnd[k];
		}

	}
	printf("\n");

	//获取最后结果
	memcpy(endmember,randEndmember,sizeof(double)*bandSum*endMemberNum);

	//清除数据
	delete[]randEndmember;randEndmember=NULL;
	delete[]tempEnd;tempEnd=NULL;
	delete[]temp;temp=NULL;
	delete[]dataCal;dataCal=NULL;
	delete[]dataMul;dataMul=NULL;
	delete[]dataTra;dataTra=NULL;

	return 0;
}

//GDAL封装
void FastFinder(char* pathImg,int endMemberNum,char* pathOut)
{
	GDALAllRegister();

	GDALDatasetH m_dataset=GDALOpen(pathImg,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset);
	int ysize=GDALGetRasterYSize(m_dataset);
	int bands=GDALGetRasterCount(m_dataset);

	//数据空间的申请
	try
	{
		double *testdata=NULL;
		testdata=new double[xsize*ysize*bands];
		if(testdata==NULL)
			throw(1);
		else
		{
			delete[]testdata;
			testdata=NULL;
		}
	}
	catch (int)
	{
		printf("memory limited！");
		exit(0);
	}

	//内存足够
	double* imgBuffer=new double[xsize*ysize*bands];
	double* endmember=new double[bands*endMemberNum];
	//获取数据
	for (int i=0;i<bands;i++)
	{
		printf("\r获取处理波段%d/%d...",bands,i+1);
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,imgBuffer+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	}
	printf("\n");
	printf("处理中...\n");

	FastFinder(imgBuffer,endmember,xsize,ysize,bands,endMemberNum);

	//输出端元光谱
	EndMember_Export_File(pathOut,endmember,bands,endMemberNum);
}

/********************************************************************************************************/
/*						                   SVD影像分解算法                                              */
/********************************************************************************************************/
long SVD(double *dataIn,int bands,int xsize,int ysize,char* pathS,char* pathV,char* pathD)
{
	if(dataIn==NULL)
		return -1;
	GDALAllRegister();
	double* datas =new double[xsize*xsize];
	double* datav =new double[xsize*ysize];
	double* datad =new double[ysize*ysize];
	double* datat =new double[xsize*ysize];
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_datasets=GDALCreate(GDALGetDriverByName("GTiff"),pathS,xsize,xsize,bands,GDT_Float64,papszOptions);
	GDALDatasetH m_datasetv=GDALCreate(GDALGetDriverByName("GTiff"),pathV,xsize,xsize,bands,GDT_Float64,papszOptions);
	GDALDatasetH m_datasetd=GDALCreate(GDALGetDriverByName("GTiff"),pathD,xsize,xsize,bands,GDT_Float64,papszOptions);

	for (int i=0;i<bands;i++)
	{
		printf("\r处理波段%d/%d",bands,i+1);
		memset(datas,0,sizeof(double)*xsize*xsize);
		memset(datav,0,sizeof(double)*xsize*ysize);
		memset(datad,0,sizeof(double)*ysize*ysize);
		memcpy(datat,dataIn+i*xsize*ysize,sizeof(double)*xsize*ysize);

		//进行分解
		Matrix_SVD(dataIn,datas,datav,datad,xsize,ysize);
		
		//结果输出
		GDALRasterIO(GDALGetRasterBand(m_datasets,i+1),GF_Read,0,0,xsize,xsize,datas,xsize,xsize,GDT_Float64,0,0);
		GDALRasterIO(GDALGetRasterBand(m_datasetv,i+1),GF_Read,0,0,xsize,xsize,datav,xsize,ysize,GDT_Float64,0,0);
		GDALRasterIO(GDALGetRasterBand(m_datasetd,i+1),GF_Read,0,0,ysize,ysize,datad,ysize,ysize,GDT_Float64,0,0);
	}
	printf("\n");
	GDALClose(m_datasets);
	GDALClose(m_datasetv);
	GDALClose(m_datasetd);

	delete[]datas;
	delete[]datav;
	delete[]datad;
	delete[]datat;

	return 0;
}

long SVDTrans(char* pathIn,int *bandNo,char* pathS,char* pathV,char* pathD)
{
	GDALAllRegister();
	CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "NO" );	//中文路径

	//选取所有波段
	if (bandNo==NULL)
	{
		GDALDatasetH m_dataset=GDALOpen(pathIn,GA_ReadOnly);
		int xsize=GDALGetRasterXSize(m_dataset);
		int ysize=GDALGetRasterYSize(m_dataset);
		int bands=GDALGetRasterCount(m_dataset);

		double *datain=new double[xsize*ysize*bands];
		for(int i=0;i<bands;i++)
			GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,datain,xsize,ysize,GDT_Float64,0,0);
		GDALClose(m_dataset);
		SVD(datain,bands,xsize,ysize,pathS,pathV,pathD);
		delete[]datain;
	}
	else
	{
		GDALDatasetH m_dataset=GDALOpen(pathIn,GA_ReadOnly);
		int xsize=GDALGetRasterXSize(m_dataset);
		int ysize=GDALGetRasterYSize(m_dataset);
		int bands=GDALGetRasterCount(m_dataset);

		int numbands=0;
		for (int i=0;i<bands;i++)
			if(bandNo[i]!=0)
				numbands++;

		double *datain=new double[xsize*ysize*numbands];
		for (int i=0,j=0;i<bands;i++)
		{
			if(bandNo[i]!=0)
			{
				GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,datain+j*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
				j++;
			}
		}
		GDALClose(m_dataset);
		SVD(datain,numbands,xsize,ysize,pathS,pathV,pathD);
		delete[]datain;

	}
	return 0;
}
/********************************************************************************************************/
/*						                   OSP影像投影方法                                              */
/********************************************************************************************************/
long  OSP(double* dataIn,int xsize,int ysize,int bands,double* transMat,int matx,int maty,double* dataOut)
{
	//检查输入
	if(dataIn==NULL||transMat==NULL||dataOut==NULL)
		return -1;
	//矩阵相乘
	MatrixMuti(transMat,matx,bands,xsize*ysize,dataIn,dataOut);
	return 0;
}

long  OSPTrans(char* pathIn,double* transMat,int matx,int maty,char* pathOut)
{
	//检查输入
	if(pathIn==NULL||pathOut==NULL)
		return -1;

	GDALAllRegister();
	GDALDatasetH m_dataset=GDALOpen(pathIn,GA_ReadOnly);
	int xsize=GDALGetRasterXSize(m_dataset);
	int ysize=GDALGetRasterYSize(m_dataset);
	int bands=GDALGetRasterCount(m_dataset);

	//数据空间申请
	double* dataIn =new double[xsize*ysize*bands];
	double* dataOut=new double[xsize*ysize*matx];
	for(int i=0;i<bands;i++)
		GDALRasterIO(GDALGetRasterBand(m_dataset,i+1),GF_Read,0,0,xsize,ysize,dataIn+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	GDALClose(m_dataset);
	OSP(dataIn,xsize,ysize,bands,transMat,matx,maty,dataOut);
	
	//创建输出数据集
	char **papszOptions = NULL;  
	papszOptions = CSLSetNameValue( papszOptions, "INTERLEAVE", "BAND" );
	GDALDatasetH m_dataout=GDALCreate(GDALGetDriverByName("GTiff"),pathOut,xsize,ysize,matx,GDT_Float64,papszOptions);
	for(int i=0;i<matx;i++)
		GDALRasterIO(GDALGetRasterBand(m_dataout,i+1),GF_Write,0,0,xsize,ysize,dataOut+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	GDALClose(m_dataout);

	delete[]dataIn;
	delete[]dataOut;

	return 0;
}

//endMat以BIP方式存储
long  GetProjMat(double* endMat,int endNumber,int bands,int selEndIdx,double* selEnd,double* p)
{
	double* endMat_1;
	
	//获取提取出的端元
	for (int i=0;i<bands;++i)
		selEnd[i]=endMat[i*endNumber+selEndIdx];
	
	//获取除了剔除的端元之外的端元
	endMat_1=new double[bands*(endNumber-1)];
	memset(endMat_1,0,sizeof(double)*bands*(endNumber-1));

	for (int i=0;i<bands;++i)
	{
		int iterator=0;
		for (int j=0;j<endNumber;++j)
		{
			if(j!=selEndIdx)
			{
				endMat_1[i*(endNumber-1)+iterator]=endMat[i*endNumber+j];
				iterator++;
			}
		}
	}

	//求矩阵的伪逆
	double *endMat_1T  =new double[bands*(endNumber-1)];
	double *endMat_1T1 =new double[(endNumber-1)*(endNumber-1)];
	double *endMat_inv =new double[(endNumber-1)*(endNumber-1)];
	double *endMat_pinv=new double[(endNumber-1)*(bands)];

	MatrixTrans(endMat_1,bands,endNumber-1,endMat_1T);
	MatrixMuti(endMat_1T,endNumber-1,bands,endNumber-1,endMat_1,endMat_1T1);
	MatrixInverse(endMat_1T1,endNumber-1,endMat_inv);

	//测试代码
	//double *endMatTest =new double[(endNumber-1)*(endNumber-1)];
	//MatrixMuti(endMat_inv,endNumber-1,endNumber-1,endNumber-1,endMat_1T1,endMatTest);
	//delete[]endMatTest;

	MatrixMuti(endMat_inv,endNumber-1,endNumber-1,bands,endMat_1T,endMat_pinv);
	MatrixMuti(endMat_1,bands,endNumber-1,bands,endMat_pinv,p);
	for (int i=0;i<bands;++i)
	{
		for (int j=0;j<bands;++j)
		{
			if(i==j)
				p[i*bands+j]=1-p[i*bands+j];
			else
				p[i*bands+j]=-p[i*bands+j];
		}
	}

	if(endMat_1T!=NULL)
		delete[]endMat_1T;
	if(endMat_1T1!=NULL)
		delete[]endMat_1T1;
	if(endMat_inv!=NULL)
		delete[]endMat_inv;
	if(endMat_pinv!=NULL)
		delete[]endMat_pinv;
	if(endMat_1!=NULL)
		delete[]endMat_1;

	return 0;
}
long  GetProjMatTilt(double* endMat,int endNumber,int bands,int selEndIdx,double* selEnd,double* p)
{
	double* endMat_1;

	//获取提取出的端元
	for (int i=0;i<bands;++i)
		selEnd[i]=endMat[i*endNumber+selEndIdx];

	//获取除了剔除的端元之外的端元
	double* endMat_tmp=new double[bands*(endNumber-1)];
	endMat_1=new double[bands*(endNumber-1)];
	memset(endMat_1,0,sizeof(double)*bands*(endNumber-1));

	for (int i=0;i<bands;++i)
	{
		int iterator=0;
		for (int j=0;j<endNumber;++j)
		{
			if(j!=selEndIdx)
			{
				endMat_1[i*(endNumber-1)+iterator]=endMat[i*endNumber+j];
				iterator++;
			}
		}
	}
	MatrixTrans(endMat_1,bands,endNumber-1,endMat_tmp);
	memcpy(endMat_1,endMat_tmp,sizeof(double)*(endNumber-1)*bands);
	if(endMat_tmp!=NULL)
		delete[]endMat_tmp;


	//求矩阵的伪逆
	double *endMat_1T  =new double[bands*(endNumber-1)];
	double *endMat_1T1 =new double[(endNumber-1)*(endNumber-1)];
	double *endMat_inv =new double[(endNumber-1)*(endNumber-1)];
	double *endMat_pinv=new double[(endNumber-1)*(bands)];

	MatrixTrans(endMat_1,endNumber-1,bands,endMat_1T);
	MatrixMuti(endMat_1,endNumber-1,bands,endNumber-1,endMat_1T,endMat_1T1);
	MatrixInverse(endMat_1T1,endNumber-1,endMat_inv);

	MatrixMuti(endMat_inv,endNumber-1,endNumber-1,bands,endMat_1,endMat_pinv);
	MatrixMuti(endMat_1T,bands,endNumber-1,bands,endMat_pinv,p);
	for (int i=0;i<bands;++i)
	{
		for (int j=0;j<bands;++j)
		{
			if(i==j)
				p[i*bands+j]=1-p[i*bands+j];
			else
				p[i*bands+j]=-p[i*bands+j];
		}
	}

	if(endMat_1T!=NULL)
		delete[]endMat_1T;
	if(endMat_1T1!=NULL)
		delete[]endMat_1T1;
	if(endMat_inv!=NULL)
		delete[]endMat_inv;
	if(endMat_pinv!=NULL)
		delete[]endMat_pinv;
	if(endMat_1!=NULL)
		delete[]endMat_1;

	return 0;
}
//根据像元和端元本组份丰度
double  GetAbundance(double* selEnd,double* p,double *pixelElement,int bands)
{
	double *dtp		=new double[bands];
	double dtpe;
	double belta;

	MatrixMuti(selEnd,1,bands,bands,p,dtp);
	MatrixMuti(dtp,1,bands,1,selEnd,&belta);
	belta=1.0/belta;
	MatrixMuti(dtp,1,bands,1,pixelElement,&dtpe);

	if(dtp!=NULL)
		delete[]dtp;

	return belta*dtpe;
}
double  GetAbundanceTilt(double* selEnd,double* p,double *pixelElement,int bands)
{
	double *ap=new double[bands];
	double *cp=new double[bands];
	double *cpInv=new double[bands];
	double rs=0;
	MatrixMuti(pixelElement,1,bands,bands,p,ap);
	MatrixMuti(selEnd,1,bands,bands,p,cp);
	Matrix_GenInverse(cp,1,bands,cpInv);
	MatrixMuti(ap,1,bands,1,cpInv,&rs);

	if(ap!=NULL)
		delete[]ap;
	if(cp!=NULL)
		delete[]cp;
	if(cpInv!=NULL)
		delete[]cpInv;
	
	return rs;
}

long  OSPUnmix(double* dataIn,int xsize,int ysize,int bands,double *endMat,int endNumber,double *dataout)
{
	double *p=new double[bands*bands];
	double *selEnd=new double[bands];
	double *pixelData=new double[bands];

	//我觉得以BIP格式存储高光谱数据更加有利于高光谱数据的处理
	for (int i=0;i<endNumber;++i)
	{
		//GetProjMatTilt(endMat,endNumber,bands,i,selEnd,p);
		GetProjMat(endMat,endNumber,bands,i,selEnd,p);
		for (int j=0;j<xsize*ysize;++j)
		{
			printf("process band %d, pixel %d\r",i,j);
			for(int k=0;k<bands;++k)
				pixelData[k]=dataIn[k*xsize*ysize+j];
			//dataout[i*xsize*ys5ize+j]=GetAbundanceTilt(selEnd,p,pixelData,bands);
			dataout[i*xsize*ysize+j]=GetAbundance(selEnd,p,pixelData,bands);
			int  temp=0;
		}
	}
	printf("\n");
	if(p!=NULL)
		delete[]p;
	if(selEnd!=NULL)
		delete[]selEnd;
	if(pixelData!=NULL)
		delete[]pixelData;
	return 0;
}
long  OSPUnmix(const char* pathImgIn,const char* pathImgOut,const char* endFile,int endnumber,int bands)
{
	//获取影像数据
	GDALAllRegister();
	CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "NO" );	//中文路径
	GDALDatasetH m_datasetIn=GDALOpen(pathImgIn,GA_ReadOnly);
	int xsize =GDALGetRasterXSize(m_datasetIn);
	int ysize =GDALGetRasterYSize(m_datasetIn);
	int ibands=GDALGetRasterCount(m_datasetIn);
	if(bands!=ibands)
		return -1;		

	double* dataImg=new double[xsize*ysize*bands];
	for (int i=0;i<bands;++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetIn,i+1),GF_Read,0,0,xsize,ysize,dataImg+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	for (int i=0;i<xsize*ysize*bands;++i)
		dataImg[i]=dataImg[i]/10;
	double* endMat=new double[endnumber*bands];
	EndMember_Import_File(endFile,endMat,bands,endnumber);
	double* dataout=new double[xsize*ysize*endnumber];

	OSPUnmix(dataImg,xsize,ysize,bands,endMat,endnumber,dataout);

	GDALDatasetH m_datasetOut=GDALCreate(GDALGetDriverByName("GTiff"),pathImgOut,xsize,ysize,endnumber,GDT_Float64,NULL);
	for (int i=0;i<endnumber;++i)
		GDALRasterIO(GDALGetRasterBand(m_datasetOut,i+1),GF_Write,0,0,xsize,ysize,dataout+i*xsize*ysize,xsize,ysize,GDT_Float64,0,0);
	GDALClose(m_datasetIn);
	GDALClose(m_datasetOut);

	if(dataImg!=NULL)
		delete[]dataImg;
	if(dataout!=NULL)
		delete[]dataout;
	if(endMat!=NULL)
		delete[]endMat;
	return 0;
}
