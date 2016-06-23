#include"stdafx.h"
#include"matrixOperation.h"
#include"AuxiliaryFunction.h"
#include<string.h>
#include <fstream>
#include<stdlib.h>
#include<cmath>
#include<set>

using namespace std;
/*
								矩阵设定：
											Y
								  ______________________
								  |__|__|__|__|__|__|__|
								  |__|__|__|__|__|__|__|
								X |__|__|__|__|__|__|__|
								  |__|__|__|__|__|__|__|
								  |__|__|__|__|__|__|__|

*/
// 增加求取矩阵行列式的操作
// 矩阵求取行列式通过矩阵LU分解实现
// [1/29/2015 wuwei just you]
// 增加了矩阵乘常数和矩阵除常数的运算
// [2/2/2015 wuwei just you]
//增加了非负矩阵分解和矩阵的SVD分解
//  [3/9/2015 wuwei just you]
//1.增加了矩阵求秩的代码
//2.增加了矩阵满秩分解的代码
//3.增加了矩阵求广义逆的代码
//4.增加了矩阵分X=AS代码 X为N*M矩阵 A为行满秩矩阵，S为列满秩矩阵
//  [4/7/2015 wuwei just you]
//1.将矩阵运算基本代码和部分矩阵分解算法使用openMP并行
//  [8/28/2015 Administrator]
//1.增加了矩阵的过完备字典的稀疏求解方法，包括BP算法、MP算法和OMP算法
//  [6/13/2016 Administrator]
//TODO:对于小规模（小于100阶的矩阵）运算能够有比较良好的效果；
//     但是对于大规模的矩阵运算效率比较低（目前来看比不上Matlab）
//	   如果有机会，希望能够做成GPU加速的算法，CUDA加速或者用OpenGL、OpenCL算法，类似SiftGPU算法
/********************************************************************************************************/
/*								    矩阵基本运算                                                        */
/********************************************************************************************************/
/*----------------------------- 矩阵乘常数------------------------------*/
long   MatrixMutiConstant(double **dataIn,int size1,int size2,double cnst,double** dataOut)
{
	//检查输入
	if(dataIn==NULL||dataOut==NULL)
		return -1;
	if(size1<=0||size2<=0)
		return -2;
	for (int i=0;i<size1;i++)
	{
		for (int j=0;j<size2;j++)
		{
			dataOut[i][j]=dataIn[i][j]*cnst;
		}
	}
	return 0;
}

long   MatrixMutiConstant(double *dataIn, int size1,int size2,double cnst,double *dataOut)
{
	//检查输入
	if(dataIn==NULL||dataOut==NULL)
		return -1;
	if(size1<=0||size2<=0)
		return -2;

	for (int i=0;i<size1*size2;i++)
		dataOut[i]=dataIn[i]*cnst;

	return 0;
}

/*----------------------------- 矩阵除常数------------------------------*/
long   MatrixDiviConstant(double **dataIn,int size1,int size2,double cnst,double** dataOut)
{
	//检查输入
	if(dataIn==NULL||dataOut==NULL)
		return -1;
	if(size1<=0||size2<=0)
		return -2;
	for (int i=0;i<size1;i++)
	{

		for (int j=0;j<size2;j++)
		{
			dataOut[i][j]=dataIn[i][j]/cnst;
		}
	}
	return 0;
}

long   MatrixDiviConstant(double *dataIn, int size1,int size2,double cnst,double *dataOut)
{
	//检查输入
	if(dataIn==NULL||dataOut==NULL)
		return -1;
	if(size1<=0||size2<=0)
		return -2;

	for (int i=0;i<size1*size2;i++)
		dataOut[i]=dataIn[i]/cnst;

	return 0;
}

/*-------------------------------矩阵转置-------------------------------*/
long   MatrixTrans(double **dataIn,int size1,int size2,double** dataOut)
{
	int i,j; //迭代变量

	//判断输入是否正确
	if((dataIn==NULL)||dataOut==NULL)
		return 1;
	if (size1<=0||size2<=0)
		return 2;

	for (i=0;i<size1;i++)
	{

		for (j=0;j<size2;j++)
		{
			dataOut[j][i]=dataIn[i][j];
		}
	}

	return 0;
}

long   MatrixTrans(double * dataIn,int size1,int size2,double*  dataOut)
{
	//判断输入是否正确
	if(dataIn==NULL||dataOut==NULL)
		return 1;
	if (size1<=0||size2<=0||size2<=0)
		return 2;

	for (int i=0;i<size2;i++)
	{

		for (int j=0;j<size1;j++)
			dataOut[i*size1+j]=dataIn[j*size2+i];
	}
	return 0;
}

/*--------------------------------矩阵乘积------------------------------*/
long   MatrixMuti(double **dataIn1,int size1,int size2,int size3,double** dataIn2,double** dataOut)
{
    int i,j,k; //迭代变量

    if((dataIn1==NULL)||(dataIn2==NULL)||dataOut==NULL)
        return 1;
    if (size1<=0||size2<=0||size2<=0)
        return 2;

    //初始化
    for(i=0;i<size1;i++)
        memset(dataOut[i],0,sizeof(double)*size3);

    //求矩阵的乘积
    for (i=0;i<size1;i++)
	{
		for(k=0;k<size3;k++)
		{
			for(j=0;j<size2;j++)
				dataOut[i][k]+=dataIn1[i][j]*dataIn2[j][k];
		}
	}

    //计算完成
    return 0;
}

long   MatrixMuti(double *dataIn1,int size1,int size2,int size3,double* dataIn2,double* dataOut)
{
    int i; //迭代变量

    //判断输入是否正确
    if((dataIn1==NULL)||(dataIn2==NULL)||dataOut==NULL)
        return 1;
    if (size1<=0||size2<=0||size2<=0)
        return 2;
    memset(dataOut,0,sizeof(double)*size1*size3);

    double** m_data1;
    double** m_data2;
    double** m_data3;

    m_data1=new double*[size1];
    m_data2=new double*[size2];
	m_data3=new double*[size1];


	for(i=0;i<size1;i++)
	{
		m_data3[i]=new double[size3];
		m_data1[i]=new double[size2];
		memcpy(m_data1[i],dataIn1+i*size2,sizeof(double)*size2);
	}

		for(i=0;i<size2;i++)
		{
			m_data2[i]=new double[size3];
			memcpy(m_data2[i],dataIn2+i*size3,sizeof(double)*size3);
		}
		MatrixMuti(m_data1,size1,size2,size3,m_data2,m_data3);

	for(i=0;i<size1;i++)
	{
		memcpy(dataOut+i*size3,m_data3[i],sizeof(double)*size3);
	}

		//删除指针

		for(i=0;i<size1;i++)
		{
			delete[]m_data1[i];
			delete[]m_data3[i];
		}

	for (i=0;i<size2;i++)
    delete[]m_data2[i];

    delete[]m_data1;
    delete[]m_data2;
    delete[]m_data3;

    return 0;
}

/*--------------------------------矩阵求逆------------------------------*/
long   MatrixInverse(double **dataIn,int size,double **dataOut)
{
	int i,j,k,l;		//迭代变量
	double **temp1;	//将计算矩阵传入
	double temp=0;
	double maxnum;

	if(dataIn==NULL||dataOut==NULL)
		return 1;
	if(size<=0)
		return 2;

	if(NULL==(temp1=new double *[size]))
		return 3;

	for (i=0;i<size;i++)
	{
		temp1[i]=new double [size];
		//dataOut[i]=new double [size];
		memcpy(temp1[i],dataIn[i],sizeof(double)*size);
		memset(dataOut[i],0,sizeof(double)*size);		//数据赋初始值
	}

	//将dataOut设置为单位矩阵
	//for (i=0;i<size;i++)
	//	dataOut[i][i]=1;

	//记录变换的行数
	int *p=new int[size];
	memset(p,0,sizeof(int)*size);
	i=j=k=l=0;
	//高斯消去法进行求解
	for(i=0;i<size;i++)
	{
		//寻找主元
		maxnum=fabs(temp1[i][i]);
		k=i;
		for (j=i+1;j<size;j++)
		{
			if (fabs(temp1[j][i])>fabs(maxnum))
			{
				maxnum = abs(temp1[j][i]);
				k=j;
			}
		}
		//如果主元所在行不是第i行，进行行交换
		if (k != i)
		{
			for (j = 0; j < size; j++)
			{
				p[i]=k;						//第i行和第k行交换
				temp = temp1[i][j];
				temp1[i][j] = temp1[k][j];
				temp1[k][j] = temp;

				//伴随交换
				//temp = dataOut[i][j];
				//dataOut[i][j] = dataOut[k][j];
				//dataOut[k][j] = temp;
			}
		}

		//判断主元是否为0，若主元为0则没有逆矩阵
		if (temp1[i][i]==0)
			return 3;

		//消去
		for (j=0;j<size;j++)
		{
			if(j!=i)
			{
				for (k=0;k<size;k++)
				{
					if(k!=i)
						temp1[j][k]=temp1[j][k]-temp1[i][k]*temp1[j][i]/temp1[i][i];
				}
			}
			else
				continue;
			temp1[j][i]=-temp1[j][i]/temp1[i][i];
		}

		for (j=0;j<size;j++)
		{
			if(j<i)
				temp1[i][j]=temp1[i][j]/temp1[i][i];
			if(j==i)
				temp1[i][j]=1.0f/temp1[i][i];
			if(j>i)
				temp1[i][j]=temp1[i][j]*temp1[i][i];
		}

		////消去A的第i列除去i行以外的各行元素
		//temp = temp1[i][i];
		//for (int j=i;j<size;j++)
		//{
		//	temp1[i][j]=temp1[i][j]/temp;		    //主对角线上的元素变为1
		//	dataOut[i][j]=dataOut[i][j]/temp;       //伴随计算
		//}
		//for (int j=0;j<size;j++)					//第0行->第n行
		//{
		//	if (j!=i)								//不是第i行
		//	{
		//		temp=temp1[j][i];
		//		for (int k=0;k<size;k++)	       //第j行元素 - i行元素*j列i行元素
		//		{
		//			temp1[j][k]=temp1[j][k]-temp1[i][k]*temp;
		//			dataOut[j][k]=dataOut[j][k]-dataOut[i][k]*temp;
		//		}
		//	}
		//}
	}

	//回代
	for (i=size-1;i>=0;i--)
	{
		if(p[i]!=0)
		{
			for (int j=0;j<size;j++)
			{
				temp=temp1[j][i];
				temp1[j][i]=temp1[j][p[i]];
				temp1[j][p[i]]=temp;
			}
		}
	}

	for (int i=0;i<size;i++)
		memcpy(dataOut[i],temp1[i],sizeof(double)*size);

	//清除指针

	for (int i=0;i<size;i++)
	{
		delete[]temp1[i];
	}
	delete []temp1;
	delete []p;

	//完成计算返回成功
	return 0;
}

long   MatrixInverse(double *dataIn,int size,double *dataOut)
{
	if (!dataIn||!dataOut)
		return 1;
	if(size<=0)
		return 2;
	double** mdataIn;
	double** mdataOut;
	mdataIn=new double*[size];
	mdataOut=new double*[size];

	for (int i=0;i<size;i++)
	{
		mdataIn[i]=new double[size];
		memcpy(mdataIn[i],dataIn+i*size,sizeof(double)*size);
		mdataOut[i]=new double[size];
	}

	MatrixInverse(mdataIn,size,mdataOut);

	for (int i=0;i<size;i++)
	{
		memcpy(dataOut+i*size,mdataOut[i],sizeof(double)*size);
	}

	for (int i=0;i<size;i++)
	{
		delete[]mdataIn[i];
		delete[]mdataOut[i];
	}
	delete[]mdataIn;
	delete[]mdataOut;

	return 0;
}

/*-----------------------------矩阵求行列式------------------------------*/
long   MatrixDet(double **dataIn,int size,double& det)
{
	//求行列式的方法：
	//首先经过LU分解分解为下三角矩阵和上三角矩阵
	//然后对角线元素的乘积即为矩阵的行列式

	//检查输入
	if(dataIn==NULL)
		return -1;

	//获取分解矩阵
	double **L, **U;
	L=new double *[size];U=new double *[size];

	for (int i=0;i<size;i++)
	{
		L[i]=new double [size];memset(L[i],0,sizeof(double)*size);
		U[i]=new double [size];memset(U[i],0,sizeof(double)*size);
	}

	//无法进行LU分解 则行列式值为0
	if(Matrix_Doolittle(dataIn,L,U,size)!=0)
		det=0;
	else
	{
		//对角线元素的乘积
		det=1.0f;
		for (int i=0;i<size;i++)
			det*=U[i][i];
	}

	//清除指针

	for (int i=0;i<size;i++)
	{
		delete[]L[i];
		delete[]U[i];
	}
	delete[]L;L=NULL;
	delete[]U;U=NULL;
	return 0;

}

long   MatrixDet(double *dataIn ,int size,double& det)
{
	//检查输入
	if (dataIn==NULL)
		return -1;
	double **dataTran;
	long lError=0;
	dataTran=new double *[size];

	for(int i=0;i<size;i++)
	{
		dataTran[i]=new double [size];
		memcpy(dataTran[i],dataIn+i*size,sizeof(double)*size);
	}

	lError=MatrixDet(dataTran,size,det);

	for (int i=0;i<size;i++)
		delete[]dataTran[i];
	delete []dataTran;dataTran=NULL;

	return lError;
}


/*-------------------------------旋转矩阵-------------------------------*/
long   MatrixRotate(double **dataIn,double phia,double omega,double kappa)
{
	dataIn[0][0]=cos(phia)*cos(kappa)-sin(phia)*sin(omega)*sin(kappa);
	dataIn[0][1]=-cos(phia)*sin(kappa)-sin(phia)*sin(omega)*cos(kappa);
	dataIn[0][2]=-sin(phia)*cos(omega);

	dataIn[1][0]=cos(omega)*sin(kappa);
	dataIn[1][1]=cos(omega)*cos(kappa);
	dataIn[1][2]=-sin(omega);

	dataIn[2][0]=sin(phia)*cos(kappa)+cos(phia)*sin(omega)*sin(kappa);
	dataIn[2][1]=-sin(phia)*sin(kappa)+cos(phia)*sin(omega)*cos(kappa);
	dataIn[2][2]=cos(phia)*cos(omega);

	return 0;
}

long   MatrixRotate(double *dataIn,double phia,double omega,double kappa)
{
	dataIn[0]=cos(phia)*cos(kappa)-sin(phia)*sin(omega)*sin(kappa);
	dataIn[1]=-cos(phia)*sin(kappa)-sin(phia)*sin(omega)*cos(kappa);
	dataIn[2]=-sin(phia)*cos(omega);

	dataIn[3]=cos(omega)*sin(kappa);
	dataIn[4]=cos(omega)*cos(kappa);
	dataIn[5]=-sin(omega);

	dataIn[6]= sin(phia)*cos(kappa)+cos(phia)*sin(omega)*sin(kappa);
	dataIn[7]=-sin(phia)*sin(kappa)+cos(phia)*sin(omega)*cos(kappa);
	dataIn[8]=cos(phia)*cos(omega);
	return 0;
}

/********************************************************************************************************/
/*								    矩阵高阶运算                                                        */
/********************************************************************************************************/
long MatrixLST(double** dataSrc,double *dataDst,double *params,int variableNum,int equationNum,bool isConstant/* =true */)
{
	if(equationNum<variableNum+1)
		return -1;
	if(!isConstant)
	{
		//临时数据空间的申请
		double **dataSrcT=new double*[equationNum];
		for(int i=0;i<equationNum;++i)
			dataSrcT[i]=new double [variableNum];
		double **dataMutiSrc=new double*[variableNum];
		for (int i=0;i<variableNum;++i)
			dataMutiSrc[i]=new double [variableNum];
		double **dataDstTmp=new double*[equationNum];
		for(int i=0;i<equationNum;++i)
		{
			dataDstTmp[i]=new double [1];
			dataDstTmp[i][0]=dataDst[i];
		}
		double **dataDstMuti=new double *[variableNum];
		for(int i=0;i<variableNum;++i)
			dataDstMuti[i]=new double [1];
		double **dataSrcInv =new double *[variableNum];
		for(int i=0;i<variableNum;++i)
			dataSrcInv[i]=new double [variableNum];
		double **dataResult=new double*[variableNum];
		for (int i=0;i<variableNum;++variableNum)
			dataResult[i]=new double [1];

		//处理
		MatrixTrans(dataSrc,variableNum,equationNum,dataSrcT);
		MatrixMuti(dataSrcT,variableNum,equationNum,variableNum,dataSrcT,dataMutiSrc);
		MatrixMuti(dataSrcT,variableNum,equationNum,1,dataDstTmp,dataDstMuti);
		MatrixInverse(dataDstMuti,variableNum,dataSrcInv);
		MatrixMuti(dataSrcInv,variableNum,variableNum,1,dataDstMuti,dataResult);
		for(int i=0;i<variableNum;++i)
			params[i]=dataResult[i][0];


		for(int i=0;i<equationNum;++i)
			delete[]dataSrcT[i];
		delete[]dataSrcT;

		for (int i=0;i<variableNum;++i)
			delete[]dataMutiSrc[i];
		delete[]dataMutiSrc;

		for(int i=0;i<equationNum;++i)
			delete[]dataDstTmp[i];
		delete[]dataDstTmp;

		for(int i=0;i<variableNum;++i)
			delete[]dataDstMuti[i];
		delete[]dataDstMuti;

		for(int i=0;i<variableNum;++i)
			delete[]dataSrcInv[i];
		delete[]dataSrcInv;

		for (int i=0;i<variableNum;++variableNum)
			delete[]dataResult[i];
		delete[]dataResult;
	}
	else
	{
		//临时数据空间的申请
		double **dataSrcTmp=new double *[equationNum];		//临时数据空间的申请
		for(int i=0;i<equationNum;++i)
			dataSrcTmp[i]=new double [variableNum+1];

		double **dataSrcT=new double*[equationNum];
		for(int i=0;i<equationNum;++i)
			dataSrcT[i]=new double [variableNum+1];
		double **dataMutiSrc=new double*[variableNum+1];
		for (int i=0;i<variableNum+1;++i)
			dataMutiSrc[i]=new double [variableNum+1];
		double **dataDstTmp=new double*[equationNum];
		for(int i=0;i<equationNum;++i)
		{
			dataDstTmp[i]=new double [1];
			dataDstTmp[i][0]=dataDst[i];
		}
		double **dataDstMuti=new double *[variableNum+1];
		for(int i=0;i<variableNum+1;++i)
			dataDstMuti[i]=new double [1];
		double **dataSrcInv =new double *[variableNum+1];
		for(int i=0;i<variableNum+1;++i)
			dataSrcInv[i]=new double [variableNum+1];
		double **dataResult=new double*[variableNum+1];
		for (int i=0;i<variableNum+1;++variableNum)
			dataResult[i]=new double [1];

		//处理
		for (int i=0;i<equationNum;++i)
		{
			for (int j=0;j<variableNum;++j)
				dataSrcTmp[i][j]=dataSrc[i][j];
			dataSrcTmp[i][variableNum]=1;
		}
		MatrixTrans(dataSrc,variableNum+1,equationNum,dataSrcT);
		MatrixMuti(dataSrcT,variableNum+1,equationNum,variableNum+1,dataSrcT,dataMutiSrc);
		MatrixMuti(dataSrcT,variableNum+1,equationNum,1,dataDstTmp,dataDstMuti);
		MatrixInverse(dataDstMuti,variableNum+1,dataSrcInv);
		MatrixMuti(dataSrcInv,variableNum+1,variableNum+1,1,dataDstMuti,dataResult);
		for(int i=0;i<variableNum+1;++i)
			params[i]=dataResult[i][0];


		for(int i=0;i<equationNum;++i)
			delete[]dataSrcTmp[i];
		delete[]dataSrcTmp;

		for(int i=0;i<equationNum;++i)
			delete[]dataSrcT[i];
		delete[]dataSrcT;

		for (int i=0;i<variableNum;++i)
			delete[]dataMutiSrc[i];
		delete[]dataMutiSrc;

		for(int i=0;i<equationNum;++i)
			delete[]dataDstTmp[i];
		delete[]dataDstTmp;

		for(int i=0;i<variableNum;++i)
			delete[]dataDstMuti[i];
		delete[]dataDstMuti;

		for(int i=0;i<variableNum;++i)
			delete[]dataSrcInv[i];
		delete[]dataSrcInv;

		for (int i=0;i<variableNum;++variableNum)
			delete[]dataResult[i];
		delete[]dataResult;
	}
	return 0;
}
long MatrixLST(double*  dataSrc,double *dataDst,double *params,int variableNum,int equationNum,bool isConstant/*=true*/)
{
	if(equationNum<variableNum+1)
		return -1;
	if(!isConstant)
	{
		double *dataSrcT=new double[variableNum*equationNum];
		double *dataSrcM=new double[variableNum*variableNum];
		double *dataSrcI=new double[variableNum*variableNum];
		double *dataDstM=new double[variableNum];

		MatrixTrans(dataSrc,equationNum,variableNum,dataSrcT);
		MatrixMuti(dataSrcT,variableNum,equationNum,variableNum,dataSrc,dataSrcM);
		MatrixMuti(dataSrcT,variableNum,equationNum,1,dataDst,dataDstM);
		MatrixInverse(dataSrcM,variableNum,dataSrcI);
		MatrixMuti(dataSrcI,variableNum,variableNum,1,dataDstM,params);

		delete[]dataSrcT;
		delete[]dataSrcM;
		delete[]dataSrcI;
		delete[]dataDstM;
	}
	else
	{
		double *dataSrcTmp=new double[(variableNum+1)*equationNum];
		for (int j=0;j<equationNum;++j)
		{
			for(int i=0;i<variableNum;++i)
			{
				dataSrcTmp[j*variableNum+i]=dataSrc[j*variableNum+i];
			}
			dataSrcTmp[j*variableNum+variableNum]=1;
		}
		double *dataSrcT=new double[(variableNum+1)*equationNum];
		double *dataSrcM=new double[(variableNum+1)*variableNum];
		double *dataSrcI=new double[(variableNum+1)*variableNum];
		double *dataDstM=new double[(variableNum+1)];

		MatrixTrans(dataSrcTmp,equationNum,variableNum+1,dataSrcT);
		MatrixMuti(dataSrcT,variableNum+1,equationNum,variableNum+1,dataSrcTmp,dataSrcM);
		MatrixMuti(dataSrcT,variableNum+1,equationNum,1,dataDst,dataDstM);
		MatrixInverse(dataSrcM,variableNum+1,dataSrcI);
		MatrixMuti(dataSrcI,variableNum+1,variableNum+1,1,dataDstM,params);

		delete[]dataSrcTmp;
		delete[]dataSrcT;
		delete[]dataSrcM;
		delete[]dataSrcI;
		delete[]dataDstM;
	}
	return 0;
}

/*--------------------------线性方程组的稀疏求解------------------------*/
//MP方法进行求解
long Matrix_Sparse_MatchPursuit(double** dictionary, double* data1, double* sparse, int size1, int size2)
{
	if (dictionary == NULL || data1 == NULL || sparse == NULL)
		return -1;

	//首先将字典归一化
	double** normalDictionary = new double*[size1];
	for (int i = 0; i < size1; ++i)
		normalDictionary[i] = new double[size2];
	for (int i = 0; i < size2; ++i)
	{
		double total = 0;
		for (int j = 0; j < size1; ++j)
			total += dictionary[j][i] * dictionary[j][i];
		total = sqrt(total);
		for (int j = 0; j < size1; ++j)
			normalDictionary[j][i] = dictionary[j][i] / total;
	}
	//然后计算目标向量与字典各个元素的内积
	memset(sparse, 0, sizeof(double)*size2);
	double *leftResidual = new double[size1];
	double *projection = new double[size2];
	memcpy(leftResidual, data1, sizeof(double)*size1);
	double totalData = 0;
	for (int i = 0; i < size1; ++i)
		totalData += data1[i];
	do
	{
		for (int i = 0; i < size2; ++i)
		{
			projection[i] = 0;
			for (int j = 0; j < size1; ++j)
				projection[i] += normalDictionary[j][i] * leftResidual[j];
			//projection[i] = abs(projection[i]);
		}

		//获取内积最大的元素的投影长度和下标
		double maxele = -9999;
		int maxindex = 0;
		for (int i = 0; i < size2; ++i)
		{
			if (abs(projection[i]) > maxele)
			{
				maxele = abs(projection[i]);
				maxindex = i;
			}
		}

		//获取残差
		for (int i = 0; i < size1; ++i)
			leftResidual[i] = leftResidual[i] - projection[i] * normalDictionary[i][maxindex];

		//计算残差和
		double totalResidual = 0;
		for (int i = 0; i < size1; ++i)
			totalResidual += leftResidual[i];
		if (totalResidual < totalData / 100.0f)
			break;

	} while (true);

	//清理内存空间
	for (int i = 0; i < size1; ++i)
		delete[]normalDictionary[i];
	delete[]normalDictionary; normalDictionary = NULL;
	delete[]leftResidual; leftResidual = NULL;
	delete[]projection; projection = NULL;
	return 0;

}
long Matrix_Sparse_MatchPursuit(double* dictionary, double* data1, double* sparse, int size1, int size2)
{
	long lError = 0;
	double** dict2d = new double*[size1];
	for (int i = 0; i < size1; ++i)
		dict2d[i] = new double[size2];
	lError = Matrix_Sparse_MatchPursuit(dict2d, data1, sparse, size1, size2);

	for (int i = 0; i < size1; ++i)
		delete[]dict2d[i];
	delete[]dict2d; dict2d = NULL;
	return lError;
}

//OMP方法进行求解
long Matrix_Sparse_OrthoMatchPursuit(double** dictionary, double* data1, double* sparse, int size1, int size2)
{
	//首先将字典归一化
	double** normalDictionary = new double*[size1];
	for (int i = 0; i < size1; ++i)
		normalDictionary[i] = new double[size2];
	for (int i = 0; i < size2; ++i)
	{
		double total = 0;
		for (int j = 0; j < size1; ++j)
			total += dictionary[j][i] * dictionary[j][i];
		total = sqrt(total);
		for (int j = 0; j < size1; ++j)
			normalDictionary[j][i] = dictionary[j][i] / total;
	}
	//然后计算目标向量与字典各个元素的内积
	memset(sparse, 0, sizeof(double)*size2);
	//初始化残差为data1
	double *leftResidual = new double[size1];
	memcpy(leftResidual, data1, sizeof(double)*size1);
	double *projection = new double[size2];
	double totalData = 0;
	for (int i = 0; i < size1; ++i)
		totalData += data1[i];
	set<int> index;
	do
	{
		memset(sparse, 0, sizeof(double)*size2);
		for (int i = 0; i < size2; ++i)
		{
			projection[i] = 0;
			for (int j = 0; j < size1; ++j)
				projection[i] += normalDictionary[j][i] * leftResidual[j];
		}

		//获取内积最大的元素的投影长度和下标
		double maxele = -9999;
		int maxindex = 0;
		for (int i = 0; i < size2; ++i)
		{
			if (abs(projection[i]) > maxele)
			{
				maxele = abs(projection[i]);
				maxindex = i;
			}
		}
		index.insert(maxindex);
		int size = index.size();

		if (size > size1)
			break;

		double* tmpDic = new double[size1*size];
		set<int>::iterator it;
		for (it = index.begin(); it != index.end(); it++)
		{
			int iidx = *it;
			for (int i = 0; i < size1; ++i)
			{
				tmpDic[iidx*size + i] = normalDictionary[i][iidx];
			}
		}

		//最小二乘求解
		//求广义逆
		double* tmpDicPinv = new double[size1*size];
		double* result = new double[size];
		Matrix_GenInverse(tmpDic, size1, size, tmpDicPinv);
		MatrixMuti(tmpDicPinv, size, size1, 1, data1, result);

		//更新残差值
		for (int i = 0; i < size1; ++i)
			MatrixMuti(tmpDic, size1, size, 1, result, leftResidual);

		int account_param = 0;
		for (it = index.begin(); it != index.end(); it++)
		{
			int iidx = *it;
			sparse[iidx] = result[account_param];
			account_param++;
		}

		//内存清空
		delete[]tmpDic; tmpDic = NULL;
		delete[]tmpDicPinv; tmpDicPinv = NULL;
		delete[]result; result = NULL;

		//判断重建误差是否确定
		double totalResidual = 0;
		for (int i = 0; i < size1; ++i)
			totalResidual += leftResidual[i];
		if (totalResidual < totalData / 100.0f)
			break;

	} while (true);

	delete[]leftResidual; leftResidual = NULL;
	delete[]projection; projection = NULL;
	for (int i = 0; i < size1; ++i)
		delete[]normalDictionary[i];
	delete[]normalDictionary; normalDictionary = NULL;
	return 0;
}
long Matrix_Sparse_OrthoMatchPursuit(double* dictionary, double* data1, double* sparse, int size1, int size2)
{
	long lError = 0;
	double** dict2d = new double*[size1];
	for (int i = 0; i < size1; ++i)
		dict2d[i] = new double[size2];
	lError = Matrix_Sparse_OrthoMatchPursuit(dict2d, data1, sparse, size1, size2);

	for (int i = 0; i < size1; ++i)
		delete[]dict2d[i];
	delete[]dict2d; dict2d = NULL;
	return lError;
}

/*----------------------------特征值和特征向量--------------------------*/
//雅可比法是求对称矩阵的特征值和特征向量
long   MatrixEigen_value_vec_Jccob(double **dataIn,double *eignValue,int size,double **mat)
{
    /*定义变量*/
    int i,j,k;
    int p,q;
    int MAXL=100000;
    double eps=0.000001f;
    double d,m,x,y;
    double sn,cn,w;
    double **a;

    //检查输入
    if (dataIn==NULL||eignValue==NULL||mat==NULL)
        return 1;
    if(size<=0)
        return 2;

    //申请空间并赋值
    a=new double *[size];
    for(i=0;i<size;i++)
    {
        a[i]=new double [size];
        memcpy(a[i],dataIn[i],sizeof(double)*size);
    }
    for (i=0;i<size;i++)
    {

        for (j=0;j<size;j++)
        {
            //生成单位矩阵
            if(i==j)
                mat[i][j]=1.0f;
            else
                mat[i][j]=0.0f;
        }
    }

    //雅可比法求矩阵的特征值和特征向量
    k=1;
    while(1)
    {
        m=0.0;
        for(i=1;i<=size-1;i++) /* 选取绝对值最大的对角线元素 */
        {
            for(j=0;j<=i-1;j++)
            {
                d=fabs(a[i][j]);
                if((i!=j) && (d>m))
                {
                    m=d;
                    p=i;
                    q=j;
                }
            }
        }
        if(m<eps)   /* 满足精度要求,正常返回 */
            break;
        if(k>MAXL)   /* 超过最大迭代次数返回 */
            return 3;
        k=k+1;
        x=-a[p][q];
        y=(a[q][q]-a[p][p])/2.0f;
        w=x/sqrt(x*x+y*y);
        if(y<0.0)  w=-w;
        sn=1+sqrt(1.0f-w*w);
        sn=w/sqrt(2.0f*sn);
        cn=sqrt(1.0f-sn*sn);
        m=a[p][p]; /* 计算矩阵的新元素 */
        a[p][p]=m*cn*cn+a[q][q]*sn*sn+a[p][q]*w;
        a[q][q]=m*sn*sn+a[q][q]*cn*cn-a[p][q]*w;
        a[p][q]=0.0f;
        a[q][p]=0.0f;
        for(j=0;j<size;j++)
        {
            if((j!=p) && (j!=q))
            {
                m=a[p][j];
                a[p][j]=m*cn+a[q][j]*sn;
                a[q][j]=-m*sn+a[q][j]*cn;
            }
        }
        for(i=0;i<size;i++)
        {
            if((i!=p) && (i!=q))
            {
                m=a[i][p];
                a[i][p]=m*cn+a[i][q]*sn;
                a[i][q]=-m*sn+a[i][q]*cn;
            }
        }
        for(i=0;i<size;i++)
        {
            m=mat[i][p];
            mat[i][p]=m*cn+mat[i][q]*sn;
            mat[i][q]=-m*sn+mat[i][q]*cn;
        }
    }

    //按特征值从大到小的顺序排列

    for (i=0;i<size;i++)
        eignValue[i]=a[i][i];

    for (i=0;i<size-1;i++)
    {
        for (j=i;j<size;j++)
        {
            if (abs(eignValue[i])<abs(eignValue[j]))
            {
                w=eignValue[i];
                eignValue[i]=eignValue[j];
                eignValue[j]=w;
                for (k=0;k<size;k++)
                {
                    w=mat[k][i];
                    mat[k][i]=mat[k][j];
                    mat[k][j]=w;
                }
            }
        }
    }

    //清除指针
    if (a!=NULL)
        for(i=0;i<size;i++)
            delete[]a[i];
    delete []a;
    return 0;
}
long   MatrixEigen_value_vec_Jccob(double *dataIn,double *eignValue,int size,double *mat)
{
	if(dataIn==NULL||eignValue==NULL||mat==NULL)
		return 1;
	double **_dataIn;
	double **_mat;
	_dataIn=new double *[size];
	_mat   =new double *[size];

	for (int i=0;i<size;i++)
	{
		_dataIn[i]=new double [size];
		_mat[i]   =new double [size];
		memcpy(_dataIn[i],dataIn+i*size,sizeof(double)*size);
	}

	MatrixEigen_value_vec_Jccob(_dataIn,eignValue,size,_mat);

	for (int i=0;i<size;i++)
		memcpy(mat+i*size,_mat[i],sizeof(double)*size);

	for(int i=0;i<size;i++)
	{
		delete[]_dataIn[i];
		delete[]_mat[i];
	}
	delete[]_dataIn;
	delete[]_mat;
	return 0;
}

/*----------------------矩阵化为上Hseesnberg矩阵------------------------*/
long   MatrixHessenberg(double **dataIn,int size,double **dataOut)
{
    int i,j,k;			//迭代变量
    int maxNumber;
    double temp;
    double *data;

    if(dataIn==NULL||dataOut==NULL)
        return 1;
    if(size<=0)
        return 2;

    //将二维矩阵化为一维
    if(NULL==(data=new double [size*size]))
        return 3;

    for(i=0;i<size;i++)
        memcpy(data+i*size,dataIn[i],sizeof(double)*size);

    for (k=1;k<size-1;k++)
    {
        i=k-1;
        maxNumber=k;
        temp=abs(data[k*size+i]);
        //选择列最大元素将最大元素下标存入maxNumber中
        for (j=k+1;j<size;j++)
        {
            if (abs(data[j*size+i])>temp)
            {
                temp=abs(data[j*size+i]);
                maxNumber=j;
            }
        }
        dataOut[0][0]=data[maxNumber*size+i];
        i=maxNumber;


        if (dataOut[0][0]!=0)
        {
            if (i!=k)
            {
                //交换顺序
                for (j=k-1;j<size;j++)
                    std::swap(data[i*size+j],data[k*size+j]);
                for(j=0;j<size;j++)
                    std::swap(data[j*size+i],data[j*size+k]);
            }
            for (i=k+1;i<size;i++)
            {
                temp=data[i*size+k-1]/dataOut[0][0];
                data[i*size+k-1]=0;
                for(j=k;j<size;j++)
                    data[i*size+j]-=temp*data[k*size+j];
                for (j=0;j<size;j++)
                    data[j*size+k]+=temp*data[j*size+i];
            }
        }
    }

    for(i=0;i<size;i++)
        memcpy(dataOut[i],data+i*size,sizeof(double)*size);

    //清除指针
    if (data!=NULL)
        delete[]data;

    //完成返回值为0
    return 0;
}

/*----------------------求Hsseenberg矩阵的全部特征值--------------------*/
long   MatrixEigenValue(double **dataIn,int size,int loopNum,double error,double *eignvalue)
{
    int i,j,k,t,m;			//迭代变量
    int loop=loopNum;		//最大迭代次数
    double b,c,d,g;
    double xy,p,q,r,x,s,e,f,z,y;
    double temp;
    double **data=NULL;			//计算化为上Hessenberg矩阵
    double *oneData;			//将二维矩阵化为一维向量进行运算
    double *cpxeignvalue=NULL;	//含复数的特征值

    if (dataIn==NULL||eignvalue==NULL)
        return 1;
    if(size<=0||loopNum<=1||error<=0)
        return 2;

    //化为上Hessenberg矩阵
    if(NULL==(data=new double*[size])||NULL==(cpxeignvalue=new double [2*size]))
        return 3;

    for(i=0;i<size;i++)
        data[i]=new double [size];
    MatrixHessenberg(dataIn,size,data);

    //二维矩阵化为一维矩阵
    oneData=new double[size*size];

    for(i=0;i<size;i++)
        memcpy(oneData+i*size,data[i],sizeof(double)*size);
    m=size;
    while(m!=0)
    {
        t=m-1;
        while(t>0)
        {
            temp=abs(oneData[(t-1)*size+t-1]);
            temp+=abs(oneData[t*size+t]);
            temp=temp*error;
            if (abs(oneData[t*size+t-1])>temp)
                t--;
            else
                break;
        }
        if (t==m-1)
        {
            cpxeignvalue[(m-1)*2]=oneData[(m-1)*size+m-1];
            cpxeignvalue[(m-1)*2+1]=0;
            m-=1;
            loop=loopNum;
        }
        else if(t==m-2)
        {
            b=-oneData[(m-1)*size+m-1]-oneData[(m-2)*size+m-2];
            c=oneData[(m-1)*size+m-1]*oneData[(m-2)*size+m-2]-oneData[(m-1)*size+m-2]*oneData[(m-2)*size+m-1];
            d=b*b-4*c;
            y=sqrt(abs(d));
            if (d>0)
            {
                xy=1;
                if (b<0)
                {
                    xy=-1;
                }
                cpxeignvalue[(m-1)*2]=-(b+xy*y)/2;
                cpxeignvalue[(m-1)*2+1]=0;
                cpxeignvalue[(m-2)*2]=c/cpxeignvalue[(m-1)*2];
                cpxeignvalue[(m-2)*2+1]=0;
            }
            else
            {
                cpxeignvalue[(m-1)*2]=-b/2;
                cpxeignvalue[(m-2)*2]=-b/2;
                cpxeignvalue[(m-1)*2+1]=y/2;
                cpxeignvalue[(m-2)*2+1]=-y/2;
            }
            m-=2;
            loop=loopNum;
        }
        else
        {
            if (loop<1)
                return false;
            loop--;
            j=t+2;
            while (j<m)
            {
                oneData[j*size+j-2]=0;
                j++;
            }
            j=t+3;
            while (j<m)
            {
                oneData[j*size+j-3]=0;
                j++;
            }
            k=t;
            while (k<m-1)
            {
                if (k!=t)
                {
                    p=oneData[k*size+k-1];
                    q=oneData[(k+1)*size+k-1];
                    if (k!=m-2)
                    {
                        r=oneData[(k+2)*size+k-1];
                    }
                    else
                    {
                        r=0;
                    }
                }
                else
                {
                    b=oneData[(m-1)*size+m-1];
                    c=oneData[(m-2)*size+m-2];
                    x=b+c;
                    y=b*c-oneData[(m-2)*size+m-1]*oneData[(m-1)*size+m-2];
                    p=oneData[t*size+t]*(oneData[t*size+t]-x)+oneData[t*size+t+1]*oneData[(t+1)*size+t]+y;
                    q=oneData[(t+1)*size+t]*(oneData[t*size+t]+oneData[(t+1)*size+t+1]-x);
                    r=oneData[(t+1)*size+t]*oneData[(t+2)*size+t+1];
                }
                if (p!=0 || q!=0 || r!=0)
                {
                    if (p<0)
                        xy=-1;
                    else
                        xy=1;
                    s=xy*sqrt(p*p+q*q+r*r);
                    if (k!=t)
                        oneData[k*size+k-1]=-s;
                    e=-q/s;
                    f=-r/s;
                    x=-p/s;
                    y=-x-f*r/(p+s);
                    g=e*r/(p+s);
                    z=-x-e*q/(p+s);
                    for (j=k;j<m;j++)
                    {
                        b=oneData[k*size+j];
                        c=oneData[(k+1)*size+j];
                        p=x*b+e*c;
                        q=e*b+y*c;
                        r=f*b+g*c;
                        if (k!=m-2)
                        {
                            b=oneData[(k+2)*size+j];
                            p+=f*b;
                            q+=g*b;
                            r+=z*b;
                            oneData[(k+2)*size+j]=r;
                        }
                        oneData[(k+1)*size+j]=q;
                        oneData[k*size+j]=p;
                    }
                    j=k+3;
                    if (j>m-2)
                    {
                        j=m-1;
                    }
                    for (i=t;i<j+1;i++)
                    {
                        b=oneData[i*size+k];
                        c=oneData[i*size+k+1];
                        p=x*b+e*c;
                        q=e*b+y*c;
                        r=f*b+g*c;
                        if (k!=m-2)
                        {
                            b=oneData[i*size+k+2];
                            p+=f*b;
                            q+=g*b;
                            r+=z*b;
                            oneData[i*size+k+2]=r;
                        }
                        oneData[i*size+k+1]=q;
                        oneData[i*size+k]=p;
                    }
                }
                k++;
            }
        }
    }

    //只保留实数部分
    for(i=0;i<size;i++)
        eignvalue[i]=cpxeignvalue[2*i];

    //计算完成 清除指针
    if(data!=NULL)
        for(i=0;i<size;i++)
            delete[]data[i];
    delete[]data;
    if (oneData!=NULL)
        delete[]oneData;
    if(cpxeignvalue!=NULL)
        delete[]cpxeignvalue;
    //成功返回0
    return 0;
}

/*----------------------根据特征值使用反幂法求特征向量------------------*/
long   MatrixEigen_value_vec(double **dataIn,double *eignValue,int size,double **mat)
{
    int i,j,k;
    int rs;
    int MAXL=1000;
    double esp=0.000001f;
    double sotrNum;
    double maxNum;
    double **data=NULL;
    double **invData=NULL;
    double **temp1=NULL,**temp2=NULL,**temp3=NULL;

    //检查输入
    if(dataIn==NULL||eignValue==NULL||mat==NULL)
        return 1;
    if(size<=1)
        return 2;

    //空间申请
    if (NULL==(data=new double *[size])||NULL==(temp1=new double *[size])
        ||NULL==(invData=new double *[size])||NULL==(temp2=new double *[size])
                ||NULL==(temp3=new double *[size]))
        return 3;

    for (i=0;i<size;i++)
    {
        data[i]=new double [size];
        invData[i]=new double [size];
        temp1[i]=new double [1];
        temp2[i]=new double [1];
        temp3[i]=new double [1];
        memset(mat[i],0,sizeof(double)*size);
        memset(invData[i],0,sizeof(double)*size);
    }
    //特征值按从大到小排序
    for (i=0;i<size;i++)
    {
        for (j=i;j<size;j++)
        {
            if(eignValue[i]<eignValue[j])
            {
                sotrNum=eignValue[i];
                eignValue[i]=eignValue[j];
                eignValue[j]=sotrNum;
            }
        }
    }

    //计算每一个特征向量
    for (i=0;i<size;i++)
    {
        //令初始特征矩阵为1

        for(j=0;j<size;j++)
            temp3[j][0]=temp2[j][0]=temp1[j][0]=1;

        for (j=0;j<size;j++)
        {
            memcpy(data[j],dataIn[j],sizeof(double)*size);
            data[j][j]=dataIn[j][j]-eignValue[i]+0.1f;
        }

        rs=MatrixInverse(data,size,invData);
        if(rs!=0)
            return rs;

        k=0;
        while(k<MAXL)
        {
            k++;
            rs=MatrixMuti(invData,size,size,1,temp1,temp2);
            if(rs!=0)
                return rs;

            maxNum=temp2[0][0];
            for(j=1;j<size;j++)
                maxNum=max(maxNum,temp2[j][0]);

            for(j=0;j<size;j++)
                temp1[j][0]=temp2[j][0]/maxNum;

            //maxNum=abs(abs(temp1[0][0])-abs(temp3[0][0]));
            //for(j=1;j<size;j++)
            //	maxNum=std::max(abs(maxNum),abs(abs(temp1[j][0])-abs(temp3[j][0])));
            //
            //for(j=1;j<size;j++)
            //	temp3[j][0]=temp1[j][0];
            //
            //if(maxNum<esp)
            //	break;
            maxNum=0.0f;
        }
        //if(k>=MAXL)
        //	return 3;
        //else
            for(j=0;j<size;j++)
                mat[j][i]=temp1[j][0];
    }


    //运行完毕清除指针并返回正确
    if (NULL==data||NULL==invData||NULL==temp1||NULL==temp2||NULL==temp3)
        return 3;

    for (i=0;i<size;i++)
    {
        delete[]data[i];
        delete[]invData[i];
        delete[]temp1[i];
        delete[]temp2[i];
        delete[]temp3[i];
    }
    delete[]data;
    delete[]invData;
    delete[]temp1;
    delete[]temp2;
    delete[]temp3;

    return 0;

}

//反幂法求特征向量
long   MatrixEigen_value_vec_InvPower(double **dataIn,double *eignValue,int size,double **mat)
{
    int loopNum=10000;
    double error=0.000001f;
    int rs;
    rs=MatrixEigenValue(dataIn,size,loopNum,error,eignValue);
    if(rs!=0)
        return rs;
    rs=MatrixEigen_value_vec(dataIn,eignValue,size,mat);
    if(rs!=0)
        return rs;
    return 0;
}

long   MatrixEigen_value_vec_InvPower(double *dataIn,double*eignValue,int size,double *mat)
{
	if(dataIn==NULL||eignValue==NULL||mat==NULL)
		return 1;
	double **_dataIn;
	double **_mat;
	_dataIn=new double *[size];
	_mat   =new double *[size];

	for (int i=0;i<size;i++)
	{
		_dataIn[i]=new double [size];
		_mat[i]   =new double [size];
		memcpy(_dataIn[i],dataIn+i*size,sizeof(double)*size);
	}

	MatrixEigen_value_vec_InvPower(_dataIn,eignValue,size,_mat);


	for (int i=0;i<size;i++)
		memcpy(mat+i*size,_mat[i],sizeof(double)*size);

	for(int i=0;i<size;i++)
	{
		delete[]_dataIn[i];
		delete[]_mat;
	}
	return 0;
}

/*----------------------------正定矩阵的Cholesky分解---------------------------*/
//直接分解法：需要进行开方运算
long   Matrix_Cholesky(double **dataIn,double** dataOut,int size)
{
    if (dataIn==NULL||dataOut==NULL)
        return 1;
    if(size<=1)
        return 2;
    //数据赋值

    for(int i=0;i<size;i++)
        memcpy(dataOut[i],dataIn[i],sizeof(double)*size);


	for (int i=0;i<size;i++)
	{

		for (int j=0;j<i;j++)
		{
			dataOut[i][i]-=dataOut[j][i]*dataOut[j][i];
		}
		dataOut[i][i]=sqrt(dataOut[i][i]);

		if(abs(dataOut[i][i])<0.000001)
			return 1;

		for (int j=i+1;j<size;j++)
		{

			for (int k=0;k<i;k++)
			{
				dataOut[j][i]-=dataOut[j][k]*dataOut[k][i];
			}
		}

		for (int j=i+1; j<size; j++)
		{

			for (int k=0; k<i; k++)
			{
				dataOut[i][j] -= dataOut[k][i]*dataOut[k][j];
			}
			dataOut[i][j] /= dataOut[i][i];
		}
	}
	return 0;
}

long   Matrix_Cholesky(double *dataIn,double* dataOut,int size)
{
	if (dataIn==NULL||dataOut==NULL)
		return 1;
	double **_dataIn;
	double **_dataOut;
	_dataIn=new double* [size];
	_dataOut=new double*[size];

	for (int i=0;i<3;i++)
	{
		_dataIn[i]   =new double [size];
		_dataOut[i]  =new double [size];
		memcpy(_dataIn[i],dataIn+i*size,sizeof(double)*size);
	}

	Matrix_Cholesky(_dataIn,_dataOut,size);

	for (int i=0;i<size;i++)
		memcpy(dataOut+i*size,_dataOut[i],sizeof(double)*size);

	for (int i=0;i<size;i++)
	{
		delete[]_dataOut[i];
		delete[]_dataIn[i];
	}
	delete[]dataOut;
	delete[]dataIn;

	return 0;
}

/*----------------------------矩阵的LU分解---------------------------*/
long   Matrix_Doolittle(double **dataIn, double **L, double **U, int size)
{
    //检查输入
    if(dataIn==NULL||L==NULL||U==NULL)
        return 1;

    //double **tempMat=new double[size];
    for(int i=0;i<size;i++)
    {
        for(int j=0;j<size;j++)
        {
            if(i<=j)
            {
                if(i!=j)
                    L[i][j]=0;
                else
                    L[i][j]=1;
            }
            else
                U[i][j]=0;
        }
    }

    for(int i=0;i<size;i++)
    {
		//U的第一行元素
        for(int j=i;j<size;j++)
        {
            double sum=0;
            for(int k=0;k<i;k++)
                sum+=L[i][k]*U[k][j];
            U[i][j]=dataIn[i][j]-sum;
        }

		//L的第一列元素
        for(int j=i+1;j<size;j++)
        {
            double sum=0;
            for(int k=0;k<i;k++)
                sum+=L[j][k]*U[k][i];
			if(U[i][i]==0)
				return -1;
            L[j][i]=(dataIn[j][i]-sum)/U[i][i];
        }
    }
    return 0;
}

long   Matrix_Doolittle(double *dataIn,  double *L ,  double *U, int size)
{
	if (dataIn==NULL||L==NULL||U==NULL)
		return 1;
	double **_dataIn;
	double **_L;
	double **_U;

	_dataIn=new double* [size];
	_L=new double *[size];
	_U=new double *[size];

	for (int i=0;i<size;i++)
	{
		_dataIn[i]=new double [size];
		_L[i]  =new double [size];
		_U[i]  =new double [size];
		memcpy(_dataIn[i],dataIn+i*size,sizeof(double)*size);
	}
	Matrix_Doolittle(_dataIn,_L,_U,size);
	for (int i=0;i<size;i++)
	{
		memcpy(L+i*size,_L[i],sizeof(double)*size);
		memcpy(U+i*size,_U[i],sizeof(double)*size);
	}
	for (int i=0;i<size;i++)
	{
		delete[]_L[i];
		delete[]_U[i];
		delete[]_dataIn[i];
	}
	delete[]_U;
	delete[]_L;
	delete[]dataIn;
	return 0;
}


/*----------------------------非负矩阵分解---------------------------*/
long   Matrix_NMF(double** dataIn,double** W,double** H, int size1,int size2,int sizeF)
{
	//检查初始值
	if(dataIn==NULL||W==NULL||H==NULL)
		return -1;

	//设置最大迭代次数
	int maxIterator=200;
	long lError=0;

	//W，H的初始值设定都为随机的正数
	rand();
	for (int i=0;i<size1;i++)
		for (int j=0;j<sizeF;j++)
			W[i][j]=abs((double)rand()/RAND_MAX) ;
	for (int i=0;i<sizeF;i++)
		for (int j=0;j<size2;j++)
			H[i][j]=abs((double)rand()/RAND_MAX) ;

	//对W列进行归一化
	for (int i=0;i<sizeF;i++)
	{
		double sum=0.0f;
		for (int j=0;j<size1;j++)
			sum+=W[j][i];
		for (int j=0;j<size1;j++)
			W[j][i]/=sum;
	}

	//迭代求解
	for (int i=0;i<maxIterator;i++)
	{
		//临时数据申请
		double **tempWH   =new double *[size1];
		double **tempDataIn=new double*[size1];
		double **tempWTrans=new double*[sizeF];
		double **tempWMuti =new double*[sizeF];
		double **tempHTrans=new double*[size2];
		double **tempHMuti =new double*[size1];
		for (int j=0;j<size1;j++)
		{
			tempWH[j]=new double [size2];    memset(tempWH[j],0,sizeof(double)*size2);
			tempDataIn[j]=new double [size2];memset(tempDataIn[j],0,sizeof(double)*size2);
			tempHMuti[j] =new double[sizeF];memset(tempHMuti[j],0,sizeof(double)*sizeF);
		}
		for (int j=0;j<sizeF;j++)
		{
			tempWTrans[j]=new double [size1];memset(tempWTrans[j],0,sizeof(double)*size1);
			tempWMuti[j] =new double [size2];memset(tempWMuti[j],0,sizeof(double)*size2);
		}
		for (int j=0;j<size2;j++)
		{
			tempHTrans[j]=new double [sizeF];memset(tempHTrans[j],0,sizeof(double)*sizeF);
		}

		lError=MatrixMuti(W,size1,sizeF,size2,H,tempWH);
		if(lError!=0)
			return lError;
		for (int j=0;j<size1;j++)
			for (int k=0;k<size2;k++)
				tempDataIn[j][k]=dataIn[j][k]/tempWH[j][k];

		//求W
		lError=MatrixTrans(H,sizeF,size2,tempHTrans);
		if(lError!=0)
			return lError;
		lError=MatrixMuti(tempDataIn,size1,size2,sizeF,tempHTrans,tempHMuti);
		if(lError!=0)
			return lError;

		for (int j=0;j<size1;j++)
			for (int k=0;k<sizeF;k++)
				W[j][k]=W[j][k]*tempHMuti[j][k];

		//对W列进行归一化
		for (int i=0;i<sizeF;i++)
		{
			double sum=0.0f;
			for (int j=0;j<size1;j++)
				sum+=W[j][i];
			for (int j=0;j<size1;j++)
				W[j][i]/=sum;
		}


		//求H
		lError=MatrixTrans(W,size1,sizeF,tempWTrans);
		if(lError!=0)
			return lError;
		lError=MatrixMuti(tempWTrans,sizeF,size1,size2,tempDataIn,tempWMuti);
		if(lError!=0)
			return lError;

		for (int j=0;j<sizeF;j++)
			for (int k=0;k<size2;k++)
				H[j][k]=H[j][k]*tempWMuti[j][k];

		//清除所有指针释放内存
		for (int j=0;j<size1;j++)
		{
			delete[]tempWH[j];
			delete[]tempDataIn[j];
			delete[]tempHMuti[j];
		}
		for (int j=0;j<sizeF;j++)
		{
			delete[]tempWTrans[j];
			delete[]tempWMuti[j];
		}
		for (int j=0;j<size2;j++)
		{
			delete[]tempHTrans[j];
		}
		delete[]tempWH;tempWH=NULL;
		delete[]tempDataIn;tempDataIn=NULL;
		delete[]tempHMuti;tempHMuti=NULL;
		delete[]tempWTrans;tempWTrans=NULL;
		delete[]tempWMuti;tempWMuti=NULL;
		delete[]tempHTrans;tempHTrans=NULL;
	}
	return lError;
}

long   Matrix_NMF(double* dataIn,double* W,double* H, int size1,int size2,int sizeF)
{
	if (dataIn==NULL||W==NULL||H==NULL)
		return -1;

	long lError=0;
	double** tempdataIn=new double*[size1];
	double** tempW=new double*[size1];
	double** tempH=new double*[sizeF];

	for (int i=0;i<size1;i++)
	{
		tempdataIn[i]=new double[size2];
		memcpy(dataIn+i*size2,tempdataIn[i],sizeof(double)*size2);
		tempW[i]=new double[sizeF];
		memcpy(tempW[i],W+i*sizeF,sizeof(double)*sizeF);
	}
	for (int i=0;i<sizeF;i++)
	{
		tempH[i]=new double[size2];
		memcpy(tempH[i],H+i*size2,sizeof(double)*size2);
	}
	lError=Matrix_NMF(tempdataIn,tempW,tempH,size1,size2,sizeF);
	if(lError!=0)
		return lError;

	for (int i=0;i<size1;i++)
	{
		memcpy(tempdataIn[i],dataIn+i*size2,sizeof(double)*size2);
		memcpy(W+i*sizeF,tempW[i],sizeof(double)*sizeF);
	}
	for (int i=0;i<sizeF;i++)
	{
		memcpy(H+i*size2,tempH[i],sizeof(double)*size2);
	}

	//清除数据
	for (int i=0;i<size1;i++)
	{
		delete[]tempdataIn[i];
		delete[]tempW[i];
	}
	for (int i=0;i<sizeF;i++)
	{
		delete[]tempH[i];
	}
	delete[]tempdataIn;tempdataIn=NULL;
	delete[]tempW;tempW=NULL;
	delete[]tempH;tempH=NULL;

	return 0;
}


/*---------------------------矩阵的奇异值分解-------------------------*/
long   Matrix_SVD(double** dataIn,double** dataS,double** dataV,double**dataD,int size1,int size2)
{
	//判断输入
	if(dataIn==NULL||dataS==NULL||dataV==NULL||dataD==NULL)
		return -1;
	if(size1<1||size2<1)
		return -2;

	//初始化S V D矩阵为0
	for (int i=0;i<size1;i++)
		memset(dataS[i],0,sizeof(double)*size1);
	for (int i=0;i<size1;i++)
		memset(dataV[i],0,sizeof(double)*size2);
	for (int i=0;i<size2;i++)
		memset(dataD[i],0,sizeof(double)*size2);

	//中间过程空间申请
	double **dataInT =new double*[size2];
	for(int i=0;i<size2;i++)
		dataInT[i]=new double [size1];
	double **dataInM1=new double*[size1];
	double **dataInI1=new double*[size1];
	for (int i=0;i<size1;i++)
	{
		dataInM1[i]=new double [size1];
		dataInI1[i]=new double [size1];
	}
	double **dataInM2=new double*[size2];
	double **dataInI2=new double*[size2];
	for (int i=0;i<size2;i++)
	{
		dataInM2[i]=new double [size2];
		dataInI2[i]=new double [size2];
	}

	//特征值和特征向量空间申请
	double *eigvalue1=new double [size1];
	double **eigmat1 =new double*[size1];
	for(int i=0;i<size1;i++)
		eigmat1[i]=new double [size1];

	double *eigvalue2=new double [size2];
	double **eigmat2 =new double*[size2];
	for(int i=0;i<size2;i++)
		eigmat2[i]=new double [size2];

	//求解过程
	MatrixTrans(dataIn,size1,size2,dataInT);
	MatrixMuti(dataIn,size1,size2,size1,dataInT,dataInM1);
	MatrixMuti(dataInT,size2,size1,size2,dataIn,dataInM2);

	MatrixEigen_value_vec_Jccob(dataInM1,eigvalue1,size1,eigmat1);
	MatrixEigen_value_vec_Jccob(dataInM2,eigvalue2,size2,eigmat2);

	int minsize=min(size1,size2);

	//赋值
	for (int i=0;i<size1;i++)
		memcpy(dataS[i],eigmat1[i],sizeof(double)*size1);
	for (int i=0;i<size2;i++)
		memcpy(dataD[i],eigmat2[i],sizeof(double)*size2);
	for (int i=0;i<minsize;i++)



	//清空指针
	for (int i=0;i<size1;i++)
	{
		delete[]dataInM1[i];
		delete[]dataInI1[i];
		delete[]eigmat1[i];
	}
	delete[]dataInM1;dataInM1=NULL;
	delete[]dataInI1;dataInI1=NULL;
	delete[]eigmat1;eigmat1=NULL;
	delete[]eigvalue1;eigvalue1=NULL;

	for (int i=0;i<size2;i++)
	{
		delete[]dataInT[i];
		delete[]dataInM2[i];
		delete[]dataInI2[i];
		delete[]eigmat2[i];
	}
	delete[]dataInT; dataInT=NULL;
	delete[]dataInM2;dataInM2=NULL;
	delete[]dataInI2;dataInI2=NULL;
	delete[]eigmat2;eigmat2=NULL;
	delete[]eigvalue2;eigvalue2=NULL;
	return 0;
}

long   Matrix_SVD(double* dataIn,double* dataS,double* dataV,double* dataD,int size1,int size2)
{
	if (dataIn==NULL||dataS==NULL||dataV==NULL||dataD==NULL)
		return 1;
	double **_dataIn;
	double **_dataS;
	double **_dataV;
	double **_dataD;

	_dataIn=new double* [size1];
	_dataS=new double *[size1];
	_dataV=new double *[size1];
	_dataD=new double *[size2];

	for (int i=0;i<size1;i++)
	{
		_dataIn[i]=new double  [size2];
		_dataS[i]  =new double [size1];
		_dataV[i]  =new double [size2];
		memcpy(_dataIn[i],dataIn+i*size2,sizeof(double)*size2);
	}
	for(int i=0;i<size2;i++)
		_dataD[i]=new double [size2];
	Matrix_SVD(_dataIn,_dataS,_dataV,_dataD,size1,size2);

	for (int i=0;i<size1;i++)
	{
		memcpy(dataS+i*size1,_dataS[i],sizeof(double)*size1);
		memcpy(dataV+i*size2,_dataV[i],sizeof(double)*size2);
	}
	for (int i=0;i<size2;i++)
		memcpy(dataD+i*size2,_dataD[i],sizeof(double)*size2);
	
	for (int i=0;i<size1;i++)
	{
		delete[]_dataS[i];
		delete[]_dataV[i];
		delete[]_dataIn[i];
	}
	for(int i=0;i<size2;i++)
		delete[]_dataD[i];
	delete[]_dataS;
	delete[]_dataV;
	delete[]_dataIn;
	delete[]_dataD;
	return 0;
}


/*---------------------------求矩阵的秩------------------------*/
long   Matrix_Rank(double** dataIn,int size1,int size2,int &rank)
{
	//检查输入
	if(dataIn==NULL)
		return -1;

	double **datatemp=new double *[size1];
	for (int i=0;i<size1;i++)
	{
		datatemp[i]=new double [size2];
		memcpy(datatemp[i],dataIn[i],sizeof(double)*size2);
	}

	//高斯消去  采用行消去法
	for (int i=0;i<size2;i++)
	{
		//找到非0的行
		for (int j=i;j<size1;j++)
		{
			 if(abs(datatemp[j][i])>0.0000001)
			 {
				 //消去所有行
				 for (int k=0;k<size1;k++)
				 {
					 double temp=datatemp[k][i]/datatemp[j][i];
					 if(k==j)
						 continue;
					 //每一行
					 for (int l=0;l<size2;l++)
						 datatemp[k][l]=datatemp[k][l]-datatemp[j][l]*temp;
				 }
			 }
			 else
				 continue;
		}
	}

	//求秩
	rank =0;
	for (int i=0;i<size1;i++)
	{
		bool iszero=true;
		for (int j=0;j<size2;j++)
			if(abs(datatemp[i][j])>=0.0000001)
				iszero=false;
		if(!iszero)
			rank++;
	}

	return 0;
}

long   Matrix_Rank(double*  dataIn,int size1,int size2,int &rank)
{
	//检查输入
	long lError=0;
	if(dataIn==NULL)
		return -1;
	double** datatemp=new double*[size1];
	for (int i=0;i<size1;i++)
	{
		datatemp[i]=new double[size2];
		memcpy(datatemp[i],dataIn+i*size2,sizeof(double)*size2);
	}
	lError=Matrix_Rank(datatemp,size1,size2,rank);
	if(lError!=0)
		return lError;
	for (int i=0;i<size1;i++)
		delete[]datatemp[i];
	delete[]datatemp;

	return 0;
}


/*---------------------------矩阵的满秩分解-------------------------*/
long   Matrix_FRD(double** dataIn,double** dataG,double** dataF,int size1,int size2,int rank)
{
	//检查输入矩阵是否正确
	if(dataIn==NULL||dataF==NULL||dataG==NULL)
		return -1;
	if(rank>min(size1,size2))
		return -2;
	double** dataRows;			//初等行变换
	double** dataCols;			//初等列变换
	int *dataLabel=new int[size2];
	memset(dataLabel,0,sizeof(int)*size2);

	dataRows=new double*[size1];
	dataCols=new double*[size1];
	for (int i=0;i<size1;i++)
	{
		dataRows[i]=new double[size2];memset(dataRows[i],0,sizeof(double)*size2);
		dataCols[i]=new double[size2];memset(dataCols[i],0,sizeof(double)*size2);
		memcpy(dataRows[i],dataIn[i],sizeof(double)*size2);
		memcpy(dataCols[i],dataIn[i],sizeof(double)*size2);
	}

	//初等行变换选取所有非0行
	//高斯消去  采用行消去法
	int size=min(size1,size2);
	for (int i=0;i<size;i++)
	{
		//找到第一个不为0的
		//为0  则向下搜索 交换两行进行处理
		if(abs(dataRows[i][i])>=0.0000001)
		{
			dataLabel[i]=1;	//确定保留的列
			//归一化此行
			double temp1=dataRows[i][i];
			for(int j=0;j<size2;j++)
				dataRows[i][j]/=temp1;
			//消去第i列
			for (int j=0;j<size1;j++)
			{
				if(j!=i)
				{
					temp1=dataRows[j][i];
					for (int k=0;k<size2;k++)
						dataRows[j][k]=dataRows[j][k]-temp1*dataRows[i][k];
				}
			}
		}
		else	
		{
			//找到不为0的行
			int nonezeroLine=0;
			for (int j=i;j<size1;j++)
			{
				if(abs(dataRows[j][i])>=0.0000001)
				{
					nonezeroLine=j;
					break;
				}
			}
			if(nonezeroLine==0)
				continue;
			dataLabel[i]=nonezeroLine;
			//交换两行
			for (int j=0;j<size2;j++)
				swap(dataRows[nonezeroLine][j],dataRows[i][j]);

			//归一化此行
			double temp1=dataRows[i][i];
			for(int j=0;j<size2;j++)
				dataRows[i][j]/=temp1;
			//消去第i列
			for (int j=0;j<size1;j++)
			{
				if(j!=i)
				{
					temp1=dataRows[j][i];
					for (int k=0;k<size2;k++)
						dataRows[j][k]=dataRows[j][k]-temp1*dataRows[i][k];

				}
			}
		}
	}
 
 	//得到分解的矩阵
 	//行矩阵
	for (int i=0;i<rank;i++)
		memcpy(dataG[i],dataRows[i],sizeof(double)*size2);
	for(int i=0,k=0;i<size2;i++)
	{
		if(dataLabel[i]!=0)
		{
			for(int j=0;j<size1;j++)
				dataF[j][k]=dataIn[j][i];
			k++;
		}
	}

	//清除数据
	for(int i=0;i<size1;i++)
	{
		delete[]dataRows[i];
		delete[]dataCols[i];
	}
	delete[]dataRows;
	delete[]dataCols;
	delete[]dataLabel;

	return 0;

}

long   Matrix_FRD(double * dataIn,double * dataG,double * dataF,int size1,int size2,int rank)
{
	if(dataIn==NULL||dataF==NULL||dataG==NULL)
		return -1;
	double** dataTIn=new double*[size1];
	double** dataTF =new double*[rank];
	double** dataTG =new double*[size1];
	for (int i=0;i<size1;i++)
	{
		dataTIn[i]=new double[size2];
		memcpy(dataTIn[i],dataIn+i*size2,sizeof(double)*size2);
		dataTG[i] =new double[rank]; 
	}
	for(int i=0;i<rank;i++)
		dataTF[i]=new double[size2];


	Matrix_FRD(dataTIn,dataTF,dataTG,size1,size2,rank);

	for (int i=0;i<size1;i++)
		memcpy(dataG+i*rank,dataTG[i],sizeof(double)*rank);
	for(int i=0;i<rank;i++)
		memcpy(dataF,dataTF[i],sizeof(double)*size2);

	for (int i=0;i<size1;i++)
	{
		delete[]dataTIn[i];
		delete[]dataTG[i];
	}
	for (int i=0;i<rank;i++)
		delete[]dataTF[i];

	delete[]dataTIn;
	delete[]dataTF;
	delete[]dataTG;

	return 0;
}


/*---------------------------求解矩阵的广义逆-------------------------*/
long   Matrix_GenInverse(double** dataIn,int size1,int size2,double** dataInverse)
{
	//检查输入
	if(dataIn==NULL||dataInverse==NULL)
		return -1;

	//求矩阵的秩
	int rank=0;
	Matrix_Rank(dataIn,size1,size2,rank);
	
	//计算空间申请
	double** dataF=new double*[size1];
	for(int i=0;i<size1;i++)
		dataF[i]=new double[rank];
	double** dataG=new double*[rank];
	for(int i=0;i<rank;i++)
		dataG[i]=new double[size2];
	double** dataTG=new double*[size2];
	for(int i=0;i<size2;i++)
		dataTG[i]=new double[rank];
	double** dataTF=new double*[rank];
	for(int i=0;i<rank;i++)
		dataTF[i]=new double[size1];
	double** FTA=new double*[rank];
	for(int i=0;i<rank;i++)
		FTA[i]=new double[size2];
	double** FTAGT=new double*[rank];
	for(int i=0;i<rank;i++)
		FTAGT[i]=new double[rank];
	double** invFTAGT=new double*[rank];
	for(int i=0;i<rank;i++)
		invFTAGT[i]=new double[rank];
	double** GTinvFTAGT=new double*[size2];
	for (int i=0;i<size2;i++)
		GTinvFTAGT[i]=new double[rank];

	//计算
	//A—=Gt(FtAGt)^-1Ft
	Matrix_FRD(dataIn,dataG,dataF,size1,size2,rank);
	MatrixTrans(dataF,size1,rank,dataTF);
	MatrixTrans(dataG,rank,size2,dataTG);
	MatrixMuti(dataTF,rank,size1,size2,dataIn,FTA);
	MatrixMuti(FTA,rank,size2,rank,dataTG,FTAGT);
	MatrixInverse(FTAGT,rank,invFTAGT);
	MatrixMuti(dataTG,size2,rank,rank,invFTAGT,GTinvFTAGT);
	MatrixMuti(GTinvFTAGT,size2,rank,size1,dataTF,dataInverse);

	//清除指针
	for(int i=0;i<size1;i++)
		delete[] dataF[i];
	for(int i=0;i<rank;i++)
		delete[] dataG[i];
	for(int i=0;i<size2;i++)
		delete[] dataTG[i];
	for(int i=0;i<rank;i++)
		delete[] dataTF[i];
	for(int i=0;i<rank;i++)
		delete[] FTA[i];
	for(int i=0;i<rank;i++)
		delete[] FTAGT[i];
	for(int i=0;i<rank;i++)
		delete[] invFTAGT[i];
	for (int i=0;i<size2;i++)
		delete[] GTinvFTAGT[i];

	delete[]  dataF;
	delete[]  dataG;
	delete[]  dataTG;
	delete[]  dataTF;
	delete[]  FTA;
	delete[]  FTAGT;
	delete[]  invFTAGT;
	delete[]  GTinvFTAGT;

	return 0;
}

long   Matrix_GenInverse(double * dataIn,int size1,int size2,double * dataInverse)
{
	if (!dataIn||!dataInverse)
		return 1;
	if(size1<=0||size2<=0)
		return 2;
	double** mdataIn;
	double** mdataOut;
	mdataIn=new double*[size1];
	mdataOut=new double*[size2];
	for (int i=0;i<size1;i++)
	{
		mdataIn[i]=new double[size2];
		memcpy(mdataIn[i],dataIn+i*size2,sizeof(double)*size2);
	}
	for(int i=0;i<size2;i++)
		mdataOut[i]=new double[size1];
	Matrix_GenInverse(mdataIn,size1,size2,mdataOut);
	for (int i=0;i<size2;i++)
		memcpy(dataInverse+i*size1,mdataOut[i],sizeof(double)*size1);

	for (int i=0;i<size1;i++)
		delete[]mdataIn[i];
	for (int i=0;i<size2;i++)
		delete[]mdataOut[i];

	delete[]mdataIn;
	delete[]mdataOut;

	return 0;
}

/*---------------------------求矩阵分解-X=AS--------------------------*/
long   Matrix_AS(double** dataIn,double** dataA,double** dataS,int size1,int size2,int sized)
{
	//检查输入
	if(dataIn==NULL||dataA==NULL||dataS==NULL)
		return -1;
	
	if(size1<size2)
	{
		printf("行数小于列数！\n");
		return -1;
	}

	double** data1=new double*[size1];
	double** data2=new double*[size1];
	double** data3=new double*[size2];
	for(int i=0;i<size1;i++)
	{
		data1[i]= new double[size1];
		data2[i]= new double[size2];
	}
	for(int i=0;i<size2;i++)
		data3[i]=new double[size2];

	double** data1d=new double*[size1];
	for(int i=0;i<size1;i++)
		data1d[i]=new double[sized];

	//进行SVD分解
	Matrix_SVD(dataIn,data1,data2,data3,size1,size2);

	//获取截尾奇异值
	for(int i=0;i<size1;i++)
		for(int j=0;j<sized;j++)
			data1d[i][j]=data1[i][j];

	double** dataU1,**dataU2;
	dataU1=new double*[size1-1];
	dataU2=new double*[size1-1];
	for (int i=0;i<size1-1;i++)
	{
		dataU1[i]=new double[sized];
		dataU2[i]=new double[sized];
	}

	//取截尾奇异值的上M-1行U1和下M-1行U2
	for(int i=0;i<size1-1;i++)
		for(int j=0;j<sized;j++)
			dataU1[i][j]=data1d[i][j];
	for(int i=1;i<size1;i++)
		for(int j=0;j<sized;j++)
			dataU2[i-1][j]=data1d[i][j];

	double** dataGInv=new double*[sized];
	for(int i=0;i<sized;i++)
		dataGInv[i]=new double[size1-1];
	Matrix_GenInverse(dataU1,size1-1,sized,dataGInv);

	double** dataMuti=new double*[sized];
	for (int i=0;i<sized;i++)
		dataMuti[i]=new double[sized];
	MatrixMuti(dataGInv,sized,size1-1,sized,dataU2,dataMuti);

	double** data4=new double*[sized];
	double** data5=new double*[sized];
	double** data6=new double*[sized];
	double*  dataEigv=new double[sized];
	for (int i=0;i<sized;i++)
	{
		data4[i]=new double[sized];memset(data4[i],0,sizeof(double)*sized);
		data5[i]=new double[sized];memset(data5[i],0,sizeof(double)*sized);
		data6[i]=new double[sized];memset(data6[i],0,sizeof(double)*sized);
	}

	//相似变换
	//Matrix_SVD(dataMuti,data4,data5,data6,sized,sized);

	ofstream ofs("test.txt");
	for (int i=0;i<sized;i++)
	{
		for (int k=0;k<sized;k++)
		{
			ofs<<dataMuti[i][k]<<" ";
		}
		ofs<<endl;
	}
	ofs.close();

	MatrixEigen_value_vec_InvPower(dataMuti,dataEigv,sized,data4);

	double** dataTU=new double*[sized];
	for(int i=0;i<sized;i++)
		dataTU[i]=new double[size1];
	MatrixTrans(data1d,size1,sized,dataTU);

	double** dataTUM=new double*[sized];
	for(int i=0;i<sized;i++)
		dataTUM[i]=new double[size1];

	//求S
	MatrixMuti(data4,sized,sized,size1,dataTU,dataTUM);
	MatrixMuti(dataTUM,sized,size1,size2,dataIn,dataS);
	
	//求A
	double** dataInv1d=new double*[sized];
	for(int i=0;i<sized;i++)
		dataInv1d[i]=new double[sized];
	MatrixInverse(data4,sized,dataInv1d);
	for (int i=0;i<size1;i++)
		memset(dataA[i],0,sizeof(double)*sized);
	MatrixMuti(data1d,size1,sized,sized,dataInv1d,dataA);

	/////////////////////////////////清除指针/////////////////////////////////////////
	for(int i=0;i<size1;i++)
	{
		delete[]data1[i];
		delete[]data2[i];
	}
	for(int i=0;i<size2;i++)
		delete[]data3[i];
	delete[] data1;
	delete[] data2;
	delete[] data3;

	for(int i=0;i<size1;i++)
		delete[] data1d[i];
	delete[]data1d;

	for (int i=0;i<size1-1;i++)
	{
		delete[]dataU1[i];
		delete[]dataU2[i];
	}
	delete[]dataU1;
	delete[]dataU2;

	for(int i=0;i<sized;i++)
		delete[]dataGInv[i];
	delete[] dataGInv;

	for (int i=0;i<sized;i++)
		delete[]dataMuti[i];
	delete[]dataMuti;

	for (int i=0;i<sized;i++)
	{
		delete[]data4[i];
		delete[]data5[i];
		delete[]data6[i];
	}
	delete[]data4;
	delete[]data5;
	delete[]data6;

	for(int i=0;i<sized;i++)
		delete[]dataTU[i];
	delete[] dataTU;

	for(int i=0;i<sized;i++)
		delete[]dataTUM[i];
	delete[]dataTUM;
	
	for(int i=0;i<sized;i++)
		delete[]dataInv1d[i];
	delete[]dataInv1d;

	delete[]dataEigv;

	return 0;
}

long   Matrix_AS(double * dataIn,double * dataA,double * dataS,int size1,int size2,int sized)
{
	//判断输入是存在问题
	if(dataIn==NULL||dataA==NULL||dataS==NULL)
		return -1;

	double** dataX=new double*[size1];
	double** dataA1=new double*[size1];
	double** dataS1=new double*[sized];
	for(int i=0;i<size1;i++)
	{
		dataX[i]=new double[size2];
		memcpy(dataX[i],dataIn+i*size2,sizeof(double)*size2);
		dataA1[i]=new double[sized];
		memset(dataA1[i],0,sizeof(double)*sized);
	}
	for(int i=0;i<sized;i++)
		dataS1[i]=new double[size2];

	Matrix_AS(dataX,dataA1,dataS1,size1,size2,sized);

	for(int i=0;i<size1;i++)
		memcpy(dataA+i*sized,dataA1[i],sizeof(double)*sized);
	for(int i=0;i<sized;i++)
		memcpy(dataS+i*size2,dataS1[i],sizeof(double)*size2);

	for (int i=0;i<size1;i++)
	{
		delete[]dataA1[i];
		delete[]dataX[i];
	}
	for(int i=0;i<sized;i++)
		delete[]dataS1[i];

	delete[]dataX;
	delete[]dataA1;
	delete[]dataS1;

	return 0;
}

//----------------------------------------------------------------------FLOAT--------------------------------------------------------------------------------------
/*----------------------------- 矩阵乘常数------------------------------*/
long   MatrixMutiConstant(float **dataIn, int size1, int size2, float cnst, float** dataOut)
{
	//检查输入
	if (dataIn == NULL || dataOut == NULL)
		return -1;
	if (size1 <= 0 || size2 <= 0)
		return -2;
	for (int i = 0; i<size1; i++)
	{
		for (int j = 0; j<size2; j++)
		{
			dataOut[i][j] = dataIn[i][j] * cnst;
		}
	}
	return 0;
}

long   MatrixMutiConstant(float *dataIn, int size1, int size2, float cnst, float *dataOut)
{
	//检查输入
	if (dataIn == NULL || dataOut == NULL)
		return -1;
	if (size1 <= 0 || size2 <= 0)
		return -2;

	for (int i = 0; i<size1*size2; i++)
		dataOut[i] = dataIn[i] * cnst;

	return 0;
}

/*----------------------------- 矩阵除常数------------------------------*/
long   MatrixDiviConstant(float **dataIn, int size1, int size2, float cnst, float** dataOut)
{
	//检查输入
	if (dataIn == NULL || dataOut == NULL)
		return -1;
	if (size1 <= 0 || size2 <= 0)
		return -2;
	for (int i = 0; i<size1; i++)
	{

		for (int j = 0; j<size2; j++)
		{
			dataOut[i][j] = dataIn[i][j] / cnst;
		}
	}
	return 0;
}

long   MatrixDiviConstant(float *dataIn, int size1, int size2, float cnst, float *dataOut)
{
	//检查输入
	if (dataIn == NULL || dataOut == NULL)
		return -1;
	if (size1 <= 0 || size2 <= 0)
		return -2;

	for (int i = 0; i<size1*size2; i++)
		dataOut[i] = dataIn[i] / cnst;

	return 0;
}

/*-------------------------------矩阵转置-------------------------------*/
long   MatrixTrans(float **dataIn, int size1, int size2, float** dataOut)
{
	int i, j; //迭代变量

			  //判断输入是否正确
	if ((dataIn == NULL) || dataOut == NULL)
		return 1;
	if (size1 <= 0 || size2 <= 0)
		return 2;

	for (i = 0; i<size1; i++)
	{

		for (j = 0; j<size2; j++)
		{
			dataOut[j][i] = dataIn[i][j];
		}
	}

	return 0;
}

long   MatrixTrans(float * dataIn, int size1, int size2, float*  dataOut)
{
	//判断输入是否正确
	if (dataIn == NULL || dataOut == NULL)
		return 1;
	if (size1 <= 0 || size2 <= 0 || size2 <= 0)
		return 2;

	for (int i = 0; i<size2; i++)
	{

		for (int j = 0; j<size1; j++)
			dataOut[i*size1 + j] = dataIn[j*size2 + i];
	}
	return 0;
}

/*--------------------------------矩阵乘积------------------------------*/
long   MatrixMuti(float **dataIn1, int size1, int size2, int size3, float** dataIn2, float** dataOut)
{
	int i, j, k; //迭代变量

	if ((dataIn1 == NULL) || (dataIn2 == NULL) || dataOut == NULL)
		return 1;
	if (size1 <= 0 || size2 <= 0 || size2 <= 0)
		return 2;

	//初始化
	for (i = 0; i<size1; i++)
		memset(dataOut[i], 0, sizeof(float)*size3);

	//求矩阵的乘积
	for (i = 0; i<size1; i++)
	{
		for (k = 0; k<size3; k++)
		{
			for (j = 0; j<size2; j++)
				dataOut[i][k] += dataIn1[i][j] * dataIn2[j][k];
		}
	}

	//计算完成
	return 0;
}

long   MatrixMuti(float *dataIn1, int size1, int size2, int size3, float* dataIn2, float* dataOut)
{
	int i; //迭代变量

		   //判断输入是否正确
	if ((dataIn1 == NULL) || (dataIn2 == NULL) || dataOut == NULL)
		return 1;
	if (size1 <= 0 || size2 <= 0 || size2 <= 0)
		return 2;
	memset(dataOut, 0, sizeof(float)*size1*size3);

	float** m_data1;
	float** m_data2;
	float** m_data3;

	m_data1 = new float*[size1];
	m_data2 = new float*[size2];
	m_data3 = new float*[size1];


	for (i = 0; i<size1; i++)
	{
		m_data3[i] = new float[size3];
		m_data1[i] = new float[size2];
		memcpy(m_data1[i], dataIn1 + i*size2, sizeof(float)*size2);
	}

	for (i = 0; i<size2; i++)
	{
		m_data2[i] = new float[size3];
		memcpy(m_data2[i], dataIn2 + i*size3, sizeof(float)*size3);
	}
	MatrixMuti(m_data1, size1, size2, size3, m_data2, m_data3);

	for (i = 0; i<size1; i++)
	{
		memcpy(dataOut + i*size3, m_data3[i], sizeof(float)*size3);
	}

	//删除指针

	for (i = 0; i<size1; i++)
	{
		delete[]m_data1[i];
		delete[]m_data3[i];
	}

	for (i = 0; i<size2; i++)
		delete[]m_data2[i];

	delete[]m_data1;
	delete[]m_data2;
	delete[]m_data3;

	return 0;
}

/*--------------------------------矩阵求逆------------------------------*/
long   MatrixInverse(float **dataIn, int size, float **dataOut)
{
	int i, j, k, l;		//迭代变量
	float **temp1;	//将计算矩阵传入
	float temp = 0;
	float maxnum;

	if (dataIn == NULL || dataOut == NULL)
		return 1;
	if (size <= 0)
		return 2;

	if (NULL == (temp1 = new float *[size]))
		return 3;

	for (i = 0; i<size; i++)
	{
		temp1[i] = new float[size];
		//dataOut[i]=new float [size];
		memcpy(temp1[i], dataIn[i], sizeof(float)*size);
		memset(dataOut[i], 0, sizeof(float)*size);		//数据赋初始值
	}

	//将dataOut设置为单位矩阵
	//for (i=0;i<size;i++)
	//	dataOut[i][i]=1;

	//记录变换的行数
	int *p = new int[size];
	memset(p, 0, sizeof(int)*size);
	i = j = k = l = 0;
	//高斯消去法进行求解
	for (i = 0; i<size; i++)
	{
		//寻找主元
		maxnum = fabs(temp1[i][i]);
		k = i;
		for (j = i + 1; j<size; j++)
		{
			if (fabs(temp1[j][i])>fabs(maxnum))
			{
				maxnum = abs(temp1[j][i]);
				k = j;
			}
		}
		//如果主元所在行不是第i行，进行行交换
		if (k != i)
		{
			for (j = 0; j < size; j++)
			{
				p[i] = k;						//第i行和第k行交换
				temp = temp1[i][j];
				temp1[i][j] = temp1[k][j];
				temp1[k][j] = temp;

				//伴随交换
				//temp = dataOut[i][j];
				//dataOut[i][j] = dataOut[k][j];
				//dataOut[k][j] = temp;
			}
		}

		//判断主元是否为0，若主元为0则没有逆矩阵
		if (temp1[i][i] == 0)
			return 3;

		//消去
		for (j = 0; j<size; j++)
		{
			if (j != i)
			{
				for (k = 0; k<size; k++)
				{
					if (k != i)
						temp1[j][k] = temp1[j][k] - temp1[i][k] * temp1[j][i] / temp1[i][i];
				}
			}
			else
				continue;
			temp1[j][i] = -temp1[j][i] / temp1[i][i];
		}

		for (j = 0; j<size; j++)
		{
			if (j<i)
				temp1[i][j] = temp1[i][j] / temp1[i][i];
			if (j == i)
				temp1[i][j] = 1.0f / temp1[i][i];
			if (j>i)
				temp1[i][j] = temp1[i][j] * temp1[i][i];
		}

		////消去A的第i列除去i行以外的各行元素
		//temp = temp1[i][i];
		//for (int j=i;j<size;j++)
		//{
		//	temp1[i][j]=temp1[i][j]/temp;		    //主对角线上的元素变为1
		//	dataOut[i][j]=dataOut[i][j]/temp;       //伴随计算
		//}
		//for (int j=0;j<size;j++)					//第0行->第n行
		//{
		//	if (j!=i)								//不是第i行
		//	{
		//		temp=temp1[j][i];
		//		for (int k=0;k<size;k++)	       //第j行元素 - i行元素*j列i行元素
		//		{
		//			temp1[j][k]=temp1[j][k]-temp1[i][k]*temp;
		//			dataOut[j][k]=dataOut[j][k]-dataOut[i][k]*temp;
		//		}
		//	}
		//}
	}

	//回代
	for (i = size - 1; i >= 0; i--)
	{
		if (p[i] != 0)
		{
			for (int j = 0; j<size; j++)
			{
				temp = temp1[j][i];
				temp1[j][i] = temp1[j][p[i]];
				temp1[j][p[i]] = temp;
			}
		}
	}

	for (int i = 0; i<size; i++)
		memcpy(dataOut[i], temp1[i], sizeof(float)*size);

	//清除指针

	for (int i = 0; i<size; i++)
	{
		delete[]temp1[i];
	}
	delete[]temp1;
	delete[]p;

	//完成计算返回成功
	return 0;
}

long   MatrixInverse(float *dataIn, int size, float *dataOut)
{
	if (!dataIn || !dataOut)
		return 1;
	if (size <= 0)
		return 2;
	float** mdataIn;
	float** mdataOut;
	mdataIn = new float*[size];
	mdataOut = new float*[size];

	for (int i = 0; i<size; i++)
	{
		mdataIn[i] = new float[size];
		memcpy(mdataIn[i], dataIn + i*size, sizeof(float)*size);
		mdataOut[i] = new float[size];
	}

	MatrixInverse(mdataIn, size, mdataOut);

	for (int i = 0; i<size; i++)
	{
		memcpy(dataOut + i*size, mdataOut[i], sizeof(float)*size);
	}

	for (int i = 0; i<size; i++)
	{
		delete[]mdataIn[i];
		delete[]mdataOut[i];
	}
	delete[]mdataIn;
	delete[]mdataOut;

	return 0;
}

/*-----------------------------矩阵求行列式------------------------------*/
long   MatrixDet(float **dataIn, int size, float& det)
{
	//求行列式的方法：
	//首先经过LU分解分解为下三角矩阵和上三角矩阵
	//然后对角线元素的乘积即为矩阵的行列式

	//检查输入
	if (dataIn == NULL)
		return -1;

	//获取分解矩阵
	float **L, **U;
	L = new float *[size]; U = new float *[size];

	for (int i = 0; i<size; i++)
	{
		L[i] = new float[size]; memset(L[i], 0, sizeof(float)*size);
		U[i] = new float[size]; memset(U[i], 0, sizeof(float)*size);
	}

	//无法进行LU分解 则行列式值为0
	if (Matrix_Doolittle(dataIn, L, U, size) != 0)
		det = 0;
	else
	{
		//对角线元素的乘积
		det = 1.0f;
		for (int i = 0; i<size; i++)
			det *= U[i][i];
	}

	//清除指针

	for (int i = 0; i<size; i++)
	{
		delete[]L[i];
		delete[]U[i];
	}
	delete[]L; L = NULL;
	delete[]U; U = NULL;
	return 0;

}

long   MatrixDet(float *dataIn, int size, float& det)
{
	//检查输入
	if (dataIn == NULL)
		return -1;
	float **dataTran;
	long lError = 0;
	dataTran = new float *[size];

	for (int i = 0; i<size; i++)
	{
		dataTran[i] = new float[size];
		memcpy(dataTran[i], dataIn + i*size, sizeof(float)*size);
	}

	lError = MatrixDet(dataTran, size, det);

	for (int i = 0; i<size; i++)
		delete[]dataTran[i];
	delete[]dataTran; dataTran = NULL;

	return lError;
}


/*-------------------------------旋转矩阵-------------------------------*/
long   MatrixRotate(float **dataIn, float phia, float omega, float kappa)
{
	dataIn[0][0] = cos(phia)*cos(kappa) - sin(phia)*sin(omega)*sin(kappa);
	dataIn[0][1] = -cos(phia)*sin(kappa) - sin(phia)*sin(omega)*cos(kappa);
	dataIn[0][2] = -sin(phia)*cos(omega);

	dataIn[1][0] = cos(omega)*sin(kappa);
	dataIn[1][1] = cos(omega)*cos(kappa);
	dataIn[1][2] = -sin(omega);

	dataIn[2][0] = sin(phia)*cos(kappa) + cos(phia)*sin(omega)*sin(kappa);
	dataIn[2][1] = -sin(phia)*sin(kappa) + cos(phia)*sin(omega)*cos(kappa);
	dataIn[2][2] = cos(phia)*cos(omega);

	return 0;
}

long   MatrixRotate(float *dataIn, float phia, float omega, float kappa)
{
	dataIn[0] = cos(phia)*cos(kappa) - sin(phia)*sin(omega)*sin(kappa);
	dataIn[1] = -cos(phia)*sin(kappa) - sin(phia)*sin(omega)*cos(kappa);
	dataIn[2] = -sin(phia)*cos(omega);

	dataIn[3] = cos(omega)*sin(kappa);
	dataIn[4] = cos(omega)*cos(kappa);
	dataIn[5] = -sin(omega);

	dataIn[6] = sin(phia)*cos(kappa) + cos(phia)*sin(omega)*sin(kappa);
	dataIn[7] = -sin(phia)*sin(kappa) + cos(phia)*sin(omega)*cos(kappa);
	dataIn[8] = cos(phia)*cos(omega);
	return 0;
}

/********************************************************************************************************/
/*								    矩阵高阶运算                                                        */
/********************************************************************************************************/
long MatrixLST(float** dataSrc, float *dataDst, float *params, int variableNum, int equationNum, bool isConstant/* =true */)
{
	if (equationNum<variableNum + 1)
		return -1;
	if (!isConstant)
	{
		//临时数据空间的申请
		float **dataSrcT = new float*[equationNum];
		for (int i = 0; i<equationNum; ++i)
			dataSrcT[i] = new float[variableNum];
		float **dataMutiSrc = new float*[variableNum];
		for (int i = 0; i<variableNum; ++i)
			dataMutiSrc[i] = new float[variableNum];
		float **dataDstTmp = new float*[equationNum];
		for (int i = 0; i<equationNum; ++i)
		{
			dataDstTmp[i] = new float[1];
			dataDstTmp[i][0] = dataDst[i];
		}
		float **dataDstMuti = new float *[variableNum];
		for (int i = 0; i<variableNum; ++i)
			dataDstMuti[i] = new float[1];
		float **dataSrcInv = new float *[variableNum];
		for (int i = 0; i<variableNum; ++i)
			dataSrcInv[i] = new float[variableNum];
		float **dataResult = new float*[variableNum];
		for (int i = 0; i<variableNum; ++variableNum)
			dataResult[i] = new float[1];

		//处理
		MatrixTrans(dataSrc, variableNum, equationNum, dataSrcT);
		MatrixMuti(dataSrcT, variableNum, equationNum, variableNum, dataSrcT, dataMutiSrc);
		MatrixMuti(dataSrcT, variableNum, equationNum, 1, dataDstTmp, dataDstMuti);
		MatrixInverse(dataDstMuti, variableNum, dataSrcInv);
		MatrixMuti(dataSrcInv, variableNum, variableNum, 1, dataDstMuti, dataResult);
		for (int i = 0; i<variableNum; ++i)
			params[i] = dataResult[i][0];


		for (int i = 0; i<equationNum; ++i)
			delete[]dataSrcT[i];
		delete[]dataSrcT;

		for (int i = 0; i<variableNum; ++i)
			delete[]dataMutiSrc[i];
		delete[]dataMutiSrc;

		for (int i = 0; i<equationNum; ++i)
			delete[]dataDstTmp[i];
		delete[]dataDstTmp;

		for (int i = 0; i<variableNum; ++i)
			delete[]dataDstMuti[i];
		delete[]dataDstMuti;

		for (int i = 0; i<variableNum; ++i)
			delete[]dataSrcInv[i];
		delete[]dataSrcInv;

		for (int i = 0; i<variableNum; ++variableNum)
			delete[]dataResult[i];
		delete[]dataResult;
	}
	else
	{
		//临时数据空间的申请
		float **dataSrcTmp = new float *[equationNum];		//临时数据空间的申请
		for (int i = 0; i<equationNum; ++i)
			dataSrcTmp[i] = new float[variableNum + 1];

		float **dataSrcT = new float*[equationNum];
		for (int i = 0; i<equationNum; ++i)
			dataSrcT[i] = new float[variableNum + 1];
		float **dataMutiSrc = new float*[variableNum + 1];
		for (int i = 0; i<variableNum + 1; ++i)
			dataMutiSrc[i] = new float[variableNum + 1];
		float **dataDstTmp = new float*[equationNum];
		for (int i = 0; i<equationNum; ++i)
		{
			dataDstTmp[i] = new float[1];
			dataDstTmp[i][0] = dataDst[i];
		}
		float **dataDstMuti = new float *[variableNum + 1];
		for (int i = 0; i<variableNum + 1; ++i)
			dataDstMuti[i] = new float[1];
		float **dataSrcInv = new float *[variableNum + 1];
		for (int i = 0; i<variableNum + 1; ++i)
			dataSrcInv[i] = new float[variableNum + 1];
		float **dataResult = new float*[variableNum + 1];
		for (int i = 0; i<variableNum + 1; ++variableNum)
			dataResult[i] = new float[1];

		//处理
		for (int i = 0; i<equationNum; ++i)
		{
			for (int j = 0; j<variableNum; ++j)
				dataSrcTmp[i][j] = dataSrc[i][j];
			dataSrcTmp[i][variableNum] = 1;
		}
		MatrixTrans(dataSrc, variableNum + 1, equationNum, dataSrcT);
		MatrixMuti(dataSrcT, variableNum + 1, equationNum, variableNum + 1, dataSrcT, dataMutiSrc);
		MatrixMuti(dataSrcT, variableNum + 1, equationNum, 1, dataDstTmp, dataDstMuti);
		MatrixInverse(dataDstMuti, variableNum + 1, dataSrcInv);
		MatrixMuti(dataSrcInv, variableNum + 1, variableNum + 1, 1, dataDstMuti, dataResult);
		for (int i = 0; i<variableNum + 1; ++i)
			params[i] = dataResult[i][0];


		for (int i = 0; i<equationNum; ++i)
			delete[]dataSrcTmp[i];
		delete[]dataSrcTmp;

		for (int i = 0; i<equationNum; ++i)
			delete[]dataSrcT[i];
		delete[]dataSrcT;

		for (int i = 0; i<variableNum; ++i)
			delete[]dataMutiSrc[i];
		delete[]dataMutiSrc;

		for (int i = 0; i<equationNum; ++i)
			delete[]dataDstTmp[i];
		delete[]dataDstTmp;

		for (int i = 0; i<variableNum; ++i)
			delete[]dataDstMuti[i];
		delete[]dataDstMuti;

		for (int i = 0; i<variableNum; ++i)
			delete[]dataSrcInv[i];
		delete[]dataSrcInv;

		for (int i = 0; i<variableNum; ++variableNum)
			delete[]dataResult[i];
		delete[]dataResult;
	}
	return 0;
}
long MatrixLST(float*  dataSrc, float *dataDst, float *params, int variableNum, int equationNum, bool isConstant/*=true*/)
{
	if (equationNum<variableNum + 1)
		return -1;
	if (!isConstant)
	{
		float *dataSrcT = new float[variableNum*equationNum];
		float *dataSrcM = new float[variableNum*variableNum];
		float *dataSrcI = new float[variableNum*variableNum];
		float *dataDstM = new float[variableNum];

		MatrixTrans(dataSrc, equationNum, variableNum, dataSrcT);
		MatrixMuti(dataSrcT, variableNum, equationNum, variableNum, dataSrc, dataSrcM);
		MatrixMuti(dataSrcT, variableNum, equationNum, 1, dataDst, dataDstM);
		MatrixInverse(dataSrcM, variableNum, dataSrcI);
		MatrixMuti(dataSrcI, variableNum, variableNum, 1, dataDstM, params);

		delete[]dataSrcT;
		delete[]dataSrcM;
		delete[]dataSrcI;
		delete[]dataDstM;
	}
	else
	{
		float *dataSrcTmp = new float[(variableNum + 1)*equationNum];
		for (int j = 0; j<equationNum; ++j)
		{
			for (int i = 0; i<variableNum; ++i)
			{
				dataSrcTmp[j*variableNum + i] = dataSrc[j*variableNum + i];
			}
			dataSrcTmp[j*variableNum + variableNum] = 1;
		}
		float *dataSrcT = new float[(variableNum + 1)*equationNum];
		float *dataSrcM = new float[(variableNum + 1)*variableNum];
		float *dataSrcI = new float[(variableNum + 1)*variableNum];
		float *dataDstM = new float[(variableNum + 1)];

		MatrixTrans(dataSrcTmp, equationNum, variableNum + 1, dataSrcT);
		MatrixMuti(dataSrcT, variableNum + 1, equationNum, variableNum + 1, dataSrcTmp, dataSrcM);
		MatrixMuti(dataSrcT, variableNum + 1, equationNum, 1, dataDst, dataDstM);
		MatrixInverse(dataSrcM, variableNum + 1, dataSrcI);
		MatrixMuti(dataSrcI, variableNum + 1, variableNum + 1, 1, dataDstM, params);

		delete[]dataSrcTmp;
		delete[]dataSrcT;
		delete[]dataSrcM;
		delete[]dataSrcI;
		delete[]dataDstM;
	}
	return 0;
}

/*--------------------------线性方程组的稀疏求解------------------------*/
//MP方法进行求解
long Matrix_Sparse_MatchPursuit(float** dictionary, float* data1, float* sparse, int size1, int size2)
{
	if (dictionary == NULL || data1 == NULL || sparse == NULL)
		return -1;
	
	//首先将字典归一化
	float** normalDictionary = new float*[size1];
	for (int i = 0; i < size1; ++i)
		normalDictionary[i] = new float[size2];
	for (int i = 0; i < size2; ++i)
	{
		double total=0;
		for (int j = 0; j < size1; ++j)
			total += dictionary[j][i]* dictionary[j][i];
		total = sqrt(total);
		for (int j = 0; j < size1; ++j)
			normalDictionary[j][i] = dictionary[j][i] / total;
	}
	//然后计算目标向量与字典各个元素的内积
	memset(sparse, 0, sizeof(float)*size2);
	float *leftResidual = new float[size1];
	float *projection   = new float[size2];
	memcpy(leftResidual, data1, sizeof(float)*size1);
	float totalData = 0;
	for (int i = 0; i < size1; ++i )
		totalData += data1[i];
	do
	{
		for (int i = 0; i < size2; ++i)
		{
			projection[i] = 0;
			for (int j = 0; j < size1; ++j)
				projection[i] += normalDictionary[j][i]* leftResidual[j];
		}

		//获取内积最大的元素的投影长度和下标
		float maxele = -9999;
		int maxindex = 0;
		for (int i = 0; i < size2; ++i)
		{
			if (abs(projection[i]) > maxele)
			{
				maxele = abs(projection[i]);
				maxindex = i;
			}
		}
		sparse[maxindex] = projection[maxindex];
		//获取残差
		for (int i = 0; i < size1; ++i)
			leftResidual[i] = leftResidual[i]- projection[i] * normalDictionary[i][maxindex];

		//计算残差和
		float totalResidual = 0;
		for (int i = 0; i < size1; ++i)
			totalResidual += leftResidual[i];
		if (totalResidual < totalData / 100.0f)
			break;

	} while (true);
	//归一化
	float normalParam = 0;
	for (int i = 0; i < size2; ++i)
		normalParam += sparse[i];
	if (abs(normalParam) > 0.001)
	{
		for (int i = 0; i < size2;++i)
			sparse[i] /= normalParam;
	}

	//清理内存空间
	for (int i = 0; i < size1; ++i)
		delete[]normalDictionary[i];
	delete[]normalDictionary; normalDictionary = NULL;
	delete[]leftResidual; leftResidual = NULL;
	delete[]projection; projection = NULL;
	return 0;

}
long Matrix_Sparse_MatchPursuit(float* dictionary, float* data1, float* sparse, int size1, int size2)
{
	long lError = 0;
	float** dict2d = new float*[size1];
	for (int i = 0; i < size1; ++i)
	{
		dict2d[i] = new float[size2];
		memcpy(dict2d[i], dictionary + i*size2, sizeof(float)*size2);
	}
	lError = Matrix_Sparse_MatchPursuit(dict2d, data1, sparse, size1, size2);

	for (int i = 0; i < size1; ++i)
		delete[]dict2d[i];
	delete[]dict2d; dict2d = NULL;
	return lError;
}

//OMP方法进行求解
long Matrix_Sparse_OrthoMatchPursuit(float** dictionary, float* data1, float* sparse, int size1, int size2)
{
	//首先将字典归一化
	float** normalDictionary = new float*[size1];
	for (int i = 0; i < size1; ++i)
		normalDictionary[i] = new float[size2];
	for (int i = 0; i < size2; ++i)
	{
		double total = 0;
		for (int j = 0; j < size1; ++j)
			total += dictionary[j][i] * dictionary[j][i];
		total = sqrt(total);
		for (int j = 0; j < size1; ++j)
			normalDictionary[j][i] = dictionary[j][i] / total;
	}
	//然后计算目标向量与字典各个元素的内积
	memset(sparse, 0, sizeof(float)*size2);
	//初始化残差为data1
	float *leftResidual = new float[size1];
	memcpy(leftResidual, data1, sizeof(float)*size1);
	float *projection = new float[size2];
	float totalData = 0;
	for (int i = 0; i < size1; ++i)
		totalData += data1[i];
	set<int> index;
	do
	{
		memset(sparse, 0, sizeof(float)*size2);
		for (int i = 0; i < size2; ++i)
		{
			projection[i] = 0;
			for (int j = 0; j < size1; ++j)
				projection[i] += normalDictionary[j][i] * leftResidual[j];
		}

		//获取内积最大的元素的投影长度和下标
		float maxele = -9999;
		int maxindex = 0;
		for (int i = 0; i < size2; ++i)
		{
			if (abs(projection[i]) > maxele)
			{
				maxele = abs(projection[i]);
				maxindex = i;
			}
		}
		index.insert(maxindex);
		int size = index.size();
		
		if (size > size1)
			break;

		float* tmpDic = new float[size1*size];
		set<int>::iterator it;
		for (it = index.begin(); it != index.end(); it++)
		{
			int iidx = *it;
			for (int i = 0; i < size1; ++i)
			{
				tmpDic[iidx*size + i] = normalDictionary[i][iidx];
			}
		}

		//最小二乘求解
		//求广义逆
		float* tmpDicPinv = new float[size1*size];
		float* result = new float[size];
		Matrix_GenInverse(tmpDic, size1, size, tmpDicPinv);
		MatrixMuti(tmpDicPinv, size, size1, 1, data1, result);
		
		//更新残差值
		for (int i = 0; i < size1; ++i)
			MatrixMuti(tmpDic, size1, size, 1, result, leftResidual);

		int account_param = 0;
		for (it = index.begin(); it != index.end(); it++)
		{
			int iidx = *it;
			sparse[iidx] = result[account_param];
			account_param++;
		}

		//内存清空
		delete[]tmpDic; tmpDic = NULL;
		delete[]tmpDicPinv; tmpDicPinv = NULL;
		delete[]result; result = NULL;

		//判断重建误差是否确定
		float totalResidual = 0;
		for (int i = 0; i < size1; ++i)
			totalResidual += leftResidual[i];
		if (totalResidual < totalData / 100.0f)
			break;

	} while (true);

	delete[]leftResidual; leftResidual = NULL;
	delete[]projection; projection = NULL;
	for (int i = 0; i < size1; ++i)
		delete[]normalDictionary[i];
	delete[]normalDictionary; normalDictionary = NULL;
	return 0;
}
long Matrix_Sparse_OrthoMatchPursuit(float* dictionary, float* data1, float* sparse, int size1, int size2)
{
	long lError = 0;
	float** dict2d = new float*[size1];
	for (int i = 0; i < size1; ++i)
	{
		dict2d[i] = new float[size2];
		memcpy(dict2d[i], dictionary + i*size2, sizeof(float)*size2);
	}
	lError = Matrix_Sparse_OrthoMatchPursuit(dict2d, data1, sparse, size1, size2);

	for (int i = 0; i < size1; ++i)
		delete[]dict2d[i];
	delete[]dict2d; dict2d = NULL;
	return lError;
}

/*----------------------------特征值和特征向量--------------------------*/
//雅可比法是求对称矩阵的特征值和特征向量
long   MatrixEigen_value_vec_Jccob(float **dataIn, float *eignValue, int size, float **mat)
{
	/*定义变量*/
	int i, j, k;
	int p, q;
	int MAXL = 100000;
	float eps = 0.000001f;
	float d, m, x, y;
	float sn, cn, w;
	float **a;

	//检查输入
	if (dataIn == NULL || eignValue == NULL || mat == NULL)
		return 1;
	if (size <= 0)
		return 2;

	//申请空间并赋值
	a = new float *[size];
	for (i = 0; i<size; i++)
	{
		a[i] = new float[size];
		memcpy(a[i], dataIn[i], sizeof(float)*size);
	}
	for (i = 0; i<size; i++)
	{

		for (j = 0; j<size; j++)
		{
			//生成单位矩阵
			if (i == j)
				mat[i][j] = 1.0f;
			else
				mat[i][j] = 0.0f;
		}
	}

	//雅可比法求矩阵的特征值和特征向量
	k = 1;
	while (1)
	{
		m = 0.0;
		for (i = 1; i <= size - 1; i++) /* 选取绝对值最大的对角线元素 */
		{
			for (j = 0; j <= i - 1; j++)
			{
				d = fabs(a[i][j]);
				if ((i != j) && (d>m))
				{
					m = d;
					p = i;
					q = j;
				}
			}
		}
		if (m<eps)   /* 满足精度要求,正常返回 */
			break;
		if (k>MAXL)   /* 超过最大迭代次数返回 */
			return 3;
		k = k + 1;
		x = -a[p][q];
		y = (a[q][q] - a[p][p]) / 2.0f;
		w = x / sqrt(x*x + y*y);
		if (y<0.0)  w = -w;
		sn = 1 + sqrt(1.0f - w*w);
		sn = w / sqrt(2.0f*sn);
		cn = sqrt(1.0f - sn*sn);
		m = a[p][p]; /* 计算矩阵的新元素 */
		a[p][p] = m*cn*cn + a[q][q] * sn*sn + a[p][q] * w;
		a[q][q] = m*sn*sn + a[q][q] * cn*cn - a[p][q] * w;
		a[p][q] = 0.0f;
		a[q][p] = 0.0f;
		for (j = 0; j<size; j++)
		{
			if ((j != p) && (j != q))
			{
				m = a[p][j];
				a[p][j] = m*cn + a[q][j] * sn;
				a[q][j] = -m*sn + a[q][j] * cn;
			}
		}
		for (i = 0; i<size; i++)
		{
			if ((i != p) && (i != q))
			{
				m = a[i][p];
				a[i][p] = m*cn + a[i][q] * sn;
				a[i][q] = -m*sn + a[i][q] * cn;
			}
		}
		for (i = 0; i<size; i++)
		{
			m = mat[i][p];
			mat[i][p] = m*cn + mat[i][q] * sn;
			mat[i][q] = -m*sn + mat[i][q] * cn;
		}
	}

	//按特征值从大到小的顺序排列

	for (i = 0; i<size; i++)
		eignValue[i] = a[i][i];

	for (i = 0; i<size - 1; i++)
	{
		for (j = i; j<size; j++)
		{
			if (abs(eignValue[i])<abs(eignValue[j]))
			{
				w = eignValue[i];
				eignValue[i] = eignValue[j];
				eignValue[j] = w;
				for (k = 0; k<size; k++)
				{
					w = mat[k][i];
					mat[k][i] = mat[k][j];
					mat[k][j] = w;
				}
			}
		}
	}

	//清除指针
	if (a != NULL)
		for (i = 0; i<size; i++)
			delete[]a[i];
	delete[]a;
	return 0;
}
long   MatrixEigen_value_vec_Jccob(float *dataIn, float *eignValue, int size, float *mat)
{
	if (dataIn == NULL || eignValue == NULL || mat == NULL)
		return 1;
	float **_dataIn;
	float **_mat;
	_dataIn = new float *[size];
	_mat = new float *[size];

	for (int i = 0; i<size; i++)
	{
		_dataIn[i] = new float[size];
		_mat[i] = new float[size];
		memcpy(_dataIn[i], dataIn + i*size, sizeof(float)*size);
	}

	MatrixEigen_value_vec_Jccob(_dataIn, eignValue, size, _mat);

	for (int i = 0; i<size; i++)
		memcpy(mat + i*size, _mat[i], sizeof(float)*size);

	for (int i = 0; i<size; i++)
	{
		delete[]_dataIn[i];
		delete[]_mat[i];
	}
	delete[]_dataIn;
	delete[]_mat;
	return 0;
}

/*----------------------矩阵化为上Hseesnberg矩阵------------------------*/
long   MatrixHessenberg(float **dataIn, int size, float **dataOut)
{
	int i, j, k;			//迭代变量
	int maxNumber;
	float temp;
	float *data;

	if (dataIn == NULL || dataOut == NULL)
		return 1;
	if (size <= 0)
		return 2;

	//将二维矩阵化为一维
	if (NULL == (data = new float[size*size]))
		return 3;

	for (i = 0; i<size; i++)
		memcpy(data + i*size, dataIn[i], sizeof(float)*size);

	for (k = 1; k<size - 1; k++)
	{
		i = k - 1;
		maxNumber = k;
		temp = abs(data[k*size + i]);
		//选择列最大元素将最大元素下标存入maxNumber中
		for (j = k + 1; j<size; j++)
		{
			if (abs(data[j*size + i])>temp)
			{
				temp = abs(data[j*size + i]);
				maxNumber = j;
			}
		}
		dataOut[0][0] = data[maxNumber*size + i];
		i = maxNumber;


		if (dataOut[0][0] != 0)
		{
			if (i != k)
			{
				//交换顺序
				for (j = k - 1; j<size; j++)
					std::swap(data[i*size + j], data[k*size + j]);
				for (j = 0; j<size; j++)
					std::swap(data[j*size + i], data[j*size + k]);
			}
			for (i = k + 1; i<size; i++)
			{
				temp = data[i*size + k - 1] / dataOut[0][0];
				data[i*size + k - 1] = 0;
				for (j = k; j<size; j++)
					data[i*size + j] -= temp*data[k*size + j];
				for (j = 0; j<size; j++)
					data[j*size + k] += temp*data[j*size + i];
			}
		}
	}

	for (i = 0; i<size; i++)
		memcpy(dataOut[i], data + i*size, sizeof(float)*size);

	//清除指针
	if (data != NULL)
		delete[]data;

	//完成返回值为0
	return 0;
}

/*----------------------求Hsseenberg矩阵的全部特征值--------------------*/
long   MatrixEigenValue(float **dataIn, int size, int loopNum, float error, float *eignvalue)
{
	int i, j, k, t, m;			//迭代变量
	int loop = loopNum;		//最大迭代次数
	float b, c, d, g;
	float xy, p, q, r, x, s, e, f, z, y;
	float temp;
	float **data = NULL;			//计算化为上Hessenberg矩阵
	float *oneData;			//将二维矩阵化为一维向量进行运算
	float *cpxeignvalue = NULL;	//含复数的特征值

	if (dataIn == NULL || eignvalue == NULL)
		return 1;
	if (size <= 0 || loopNum <= 1 || error <= 0)
		return 2;

	//化为上Hessenberg矩阵
	if (NULL == (data = new float*[size]) || NULL == (cpxeignvalue = new float[2 * size]))
		return 3;

	for (i = 0; i<size; i++)
		data[i] = new float[size];
	MatrixHessenberg(dataIn, size, data);

	//二维矩阵化为一维矩阵
	oneData = new float[size*size];

	for (i = 0; i<size; i++)
		memcpy(oneData + i*size, data[i], sizeof(float)*size);
	m = size;
	while (m != 0)
	{
		t = m - 1;
		while (t>0)
		{
			temp = abs(oneData[(t - 1)*size + t - 1]);
			temp += abs(oneData[t*size + t]);
			temp = temp*error;
			if (abs(oneData[t*size + t - 1])>temp)
				t--;
			else
				break;
		}
		if (t == m - 1)
		{
			cpxeignvalue[(m - 1) * 2] = oneData[(m - 1)*size + m - 1];
			cpxeignvalue[(m - 1) * 2 + 1] = 0;
			m -= 1;
			loop = loopNum;
		}
		else if (t == m - 2)
		{
			b = -oneData[(m - 1)*size + m - 1] - oneData[(m - 2)*size + m - 2];
			c = oneData[(m - 1)*size + m - 1] * oneData[(m - 2)*size + m - 2] - oneData[(m - 1)*size + m - 2] * oneData[(m - 2)*size + m - 1];
			d = b*b - 4 * c;
			y = sqrt(abs(d));
			if (d>0)
			{
				xy = 1;
				if (b<0)
				{
					xy = -1;
				}
				cpxeignvalue[(m - 1) * 2] = -(b + xy*y) / 2;
				cpxeignvalue[(m - 1) * 2 + 1] = 0;
				cpxeignvalue[(m - 2) * 2] = c / cpxeignvalue[(m - 1) * 2];
				cpxeignvalue[(m - 2) * 2 + 1] = 0;
			}
			else
			{
				cpxeignvalue[(m - 1) * 2] = -b / 2;
				cpxeignvalue[(m - 2) * 2] = -b / 2;
				cpxeignvalue[(m - 1) * 2 + 1] = y / 2;
				cpxeignvalue[(m - 2) * 2 + 1] = -y / 2;
			}
			m -= 2;
			loop = loopNum;
		}
		else
		{
			if (loop<1)
				return false;
			loop--;
			j = t + 2;
			while (j<m)
			{
				oneData[j*size + j - 2] = 0;
				j++;
			}
			j = t + 3;
			while (j<m)
			{
				oneData[j*size + j - 3] = 0;
				j++;
			}
			k = t;
			while (k<m - 1)
			{
				if (k != t)
				{
					p = oneData[k*size + k - 1];
					q = oneData[(k + 1)*size + k - 1];
					if (k != m - 2)
					{
						r = oneData[(k + 2)*size + k - 1];
					}
					else
					{
						r = 0;
					}
				}
				else
				{
					b = oneData[(m - 1)*size + m - 1];
					c = oneData[(m - 2)*size + m - 2];
					x = b + c;
					y = b*c - oneData[(m - 2)*size + m - 1] * oneData[(m - 1)*size + m - 2];
					p = oneData[t*size + t] * (oneData[t*size + t] - x) + oneData[t*size + t + 1] * oneData[(t + 1)*size + t] + y;
					q = oneData[(t + 1)*size + t] * (oneData[t*size + t] + oneData[(t + 1)*size + t + 1] - x);
					r = oneData[(t + 1)*size + t] * oneData[(t + 2)*size + t + 1];
				}
				if (p != 0 || q != 0 || r != 0)
				{
					if (p<0)
						xy = -1;
					else
						xy = 1;
					s = xy*sqrt(p*p + q*q + r*r);
					if (k != t)
						oneData[k*size + k - 1] = -s;
					e = -q / s;
					f = -r / s;
					x = -p / s;
					y = -x - f*r / (p + s);
					g = e*r / (p + s);
					z = -x - e*q / (p + s);
					for (j = k; j<m; j++)
					{
						b = oneData[k*size + j];
						c = oneData[(k + 1)*size + j];
						p = x*b + e*c;
						q = e*b + y*c;
						r = f*b + g*c;
						if (k != m - 2)
						{
							b = oneData[(k + 2)*size + j];
							p += f*b;
							q += g*b;
							r += z*b;
							oneData[(k + 2)*size + j] = r;
						}
						oneData[(k + 1)*size + j] = q;
						oneData[k*size + j] = p;
					}
					j = k + 3;
					if (j>m - 2)
					{
						j = m - 1;
					}
					for (i = t; i<j + 1; i++)
					{
						b = oneData[i*size + k];
						c = oneData[i*size + k + 1];
						p = x*b + e*c;
						q = e*b + y*c;
						r = f*b + g*c;
						if (k != m - 2)
						{
							b = oneData[i*size + k + 2];
							p += f*b;
							q += g*b;
							r += z*b;
							oneData[i*size + k + 2] = r;
						}
						oneData[i*size + k + 1] = q;
						oneData[i*size + k] = p;
					}
				}
				k++;
			}
		}
	}

	//只保留实数部分
	for (i = 0; i<size; i++)
		eignvalue[i] = cpxeignvalue[2 * i];

	//计算完成 清除指针
	if (data != NULL)
		for (i = 0; i<size; i++)
			delete[]data[i];
	delete[]data;
	if (oneData != NULL)
		delete[]oneData;
	if (cpxeignvalue != NULL)
		delete[]cpxeignvalue;
	//成功返回0
	return 0;
}

/*----------------------根据特征值使用反幂法求特征向量------------------*/
long   MatrixEigen_value_vec(float **dataIn, float *eignValue, int size, float **mat)
{
	int i, j, k;
	int rs;
	int MAXL = 1000;
	float esp = 0.000001f;
	float sotrNum;
	float maxNum;
	float **data = NULL;
	float **invData = NULL;
	float **temp1 = NULL, **temp2 = NULL, **temp3 = NULL;

	//检查输入
	if (dataIn == NULL || eignValue == NULL || mat == NULL)
		return 1;
	if (size <= 1)
		return 2;

	//空间申请
	if (NULL == (data = new float *[size]) || NULL == (temp1 = new float *[size])
		|| NULL == (invData = new float *[size]) || NULL == (temp2 = new float *[size])
		|| NULL == (temp3 = new float *[size]))
		return 3;

	for (i = 0; i<size; i++)
	{
		data[i] = new float[size];
		invData[i] = new float[size];
		temp1[i] = new float[1];
		temp2[i] = new float[1];
		temp3[i] = new float[1];
		memset(mat[i], 0, sizeof(float)*size);
		memset(invData[i], 0, sizeof(float)*size);
	}
	//特征值按从大到小排序
	for (i = 0; i<size; i++)
	{
		for (j = i; j<size; j++)
		{
			if (eignValue[i]<eignValue[j])
			{
				sotrNum = eignValue[i];
				eignValue[i] = eignValue[j];
				eignValue[j] = sotrNum;
			}
		}
	}

	//计算每一个特征向量
	for (i = 0; i<size; i++)
	{
		//令初始特征矩阵为1

		for (j = 0; j<size; j++)
			temp3[j][0] = temp2[j][0] = temp1[j][0] = 1;

		for (j = 0; j<size; j++)
		{
			memcpy(data[j], dataIn[j], sizeof(float)*size);
			data[j][j] = dataIn[j][j] - eignValue[i] + 0.1f;
		}

		rs = MatrixInverse(data, size, invData);
		if (rs != 0)
			return rs;

		k = 0;
		while (k<MAXL)
		{
			k++;
			rs = MatrixMuti(invData, size, size, 1, temp1, temp2);
			if (rs != 0)
				return rs;

			maxNum = temp2[0][0];
			for (j = 1; j<size; j++)
				maxNum = max(maxNum, temp2[j][0]);

			for (j = 0; j<size; j++)
				temp1[j][0] = temp2[j][0] / maxNum;

			//maxNum=abs(abs(temp1[0][0])-abs(temp3[0][0]));
			//for(j=1;j<size;j++)
			//	maxNum=std::max(abs(maxNum),abs(abs(temp1[j][0])-abs(temp3[j][0])));
			//
			//for(j=1;j<size;j++)
			//	temp3[j][0]=temp1[j][0];
			//
			//if(maxNum<esp)
			//	break;
			maxNum = 0.0f;
		}
		//if(k>=MAXL)
		//	return 3;
		//else
		for (j = 0; j<size; j++)
			mat[j][i] = temp1[j][0];
	}


	//运行完毕清除指针并返回正确
	if (NULL == data || NULL == invData || NULL == temp1 || NULL == temp2 || NULL == temp3)
		return 3;

	for (i = 0; i<size; i++)
	{
		delete[]data[i];
		delete[]invData[i];
		delete[]temp1[i];
		delete[]temp2[i];
		delete[]temp3[i];
	}
	delete[]data;
	delete[]invData;
	delete[]temp1;
	delete[]temp2;
	delete[]temp3;

	return 0;

}

//反幂法求特征向量
long   MatrixEigen_value_vec_InvPower(float **dataIn, float *eignValue, int size, float **mat)
{
	int loopNum = 10000;
	float error = 0.000001f;
	int rs;
	rs = MatrixEigenValue(dataIn, size, loopNum, error, eignValue);
	if (rs != 0)
		return rs;
	rs = MatrixEigen_value_vec(dataIn, eignValue, size, mat);
	if (rs != 0)
		return rs;
	return 0;
}

long   MatrixEigen_value_vec_InvPower(float *dataIn, float*eignValue, int size, float *mat)
{
	if (dataIn == NULL || eignValue == NULL || mat == NULL)
		return 1;
	float **_dataIn;
	float **_mat;
	_dataIn = new float *[size];
	_mat = new float *[size];

	for (int i = 0; i<size; i++)
	{
		_dataIn[i] = new float[size];
		_mat[i] = new float[size];
		memcpy(_dataIn[i], dataIn + i*size, sizeof(float)*size);
	}

	MatrixEigen_value_vec_InvPower(_dataIn, eignValue, size, _mat);


	for (int i = 0; i<size; i++)
		memcpy(mat + i*size, _mat[i], sizeof(float)*size);

	for (int i = 0; i<size; i++)
	{
		delete[]_dataIn[i];
		delete[]_mat;
	}
	return 0;
}

/*----------------------------正定矩阵的Cholesky分解---------------------------*/
//直接分解法：需要进行开方运算
long   Matrix_Cholesky(float **dataIn, float** dataOut, int size)
{
	if (dataIn == NULL || dataOut == NULL)
		return 1;
	if (size <= 1)
		return 2;
	//数据赋值

	for (int i = 0; i<size; i++)
		memcpy(dataOut[i], dataIn[i], sizeof(float)*size);


	for (int i = 0; i<size; i++)
	{

		for (int j = 0; j<i; j++)
		{
			dataOut[i][i] -= dataOut[j][i] * dataOut[j][i];
		}
		dataOut[i][i] = sqrt(dataOut[i][i]);

		if (abs(dataOut[i][i])<0.000001)
			return 1;

		for (int j = i + 1; j<size; j++)
		{

			for (int k = 0; k<i; k++)
			{
				dataOut[j][i] -= dataOut[j][k] * dataOut[k][i];
			}
		}

		for (int j = i + 1; j<size; j++)
		{

			for (int k = 0; k<i; k++)
			{
				dataOut[i][j] -= dataOut[k][i] * dataOut[k][j];
			}
			dataOut[i][j] /= dataOut[i][i];
		}
	}
	return 0;
}

long   Matrix_Cholesky(float *dataIn, float* dataOut, int size)
{
	if (dataIn == NULL || dataOut == NULL)
		return 1;
	float **_dataIn;
	float **_dataOut;
	_dataIn = new float*[size];
	_dataOut = new float*[size];

	for (int i = 0; i<3; i++)
	{
		_dataIn[i] = new float[size];
		_dataOut[i] = new float[size];
		memcpy(_dataIn[i], dataIn + i*size, sizeof(float)*size);
	}

	Matrix_Cholesky(_dataIn, _dataOut, size);

	for (int i = 0; i<size; i++)
		memcpy(dataOut + i*size, _dataOut[i], sizeof(float)*size);

	for (int i = 0; i<size; i++)
	{
		delete[]_dataOut[i];
		delete[]_dataIn[i];
	}
	delete[]dataOut;
	delete[]dataIn;

	return 0;
}

/*----------------------------矩阵的LU分解---------------------------*/
long   Matrix_Doolittle(float **dataIn, float **L, float **U, int size)
{
	//检查输入
	if (dataIn == NULL || L == NULL || U == NULL)
		return 1;

	//float **tempMat=new float[size];
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			if (i <= j)
			{
				if (i != j)
					L[i][j] = 0;
				else
					L[i][j] = 1;
			}
			else
				U[i][j] = 0;
		}
	}

	for (int i = 0; i<size; i++)
	{
		//U的第一行元素
		for (int j = i; j<size; j++)
		{
			float sum = 0;
			for (int k = 0; k<i; k++)
				sum += L[i][k] * U[k][j];
			U[i][j] = dataIn[i][j] - sum;
		}

		//L的第一列元素
		for (int j = i + 1; j<size; j++)
		{
			float sum = 0;
			for (int k = 0; k<i; k++)
				sum += L[j][k] * U[k][i];
			if (U[i][i] == 0)
				return -1;
			L[j][i] = (dataIn[j][i] - sum) / U[i][i];
		}
	}
	return 0;
}

long   Matrix_Doolittle(float *dataIn, float *L, float *U, int size)
{
	if (dataIn == NULL || L == NULL || U == NULL)
		return 1;
	float **_dataIn;
	float **_L;
	float **_U;

	_dataIn = new float*[size];
	_L = new float *[size];
	_U = new float *[size];

	for (int i = 0; i<size; i++)
	{
		_dataIn[i] = new float[size];
		_L[i] = new float[size];
		_U[i] = new float[size];
		memcpy(_dataIn[i], dataIn + i*size, sizeof(float)*size);
	}
	Matrix_Doolittle(_dataIn, _L, _U, size);
	for (int i = 0; i<size; i++)
	{
		memcpy(L + i*size, _L[i], sizeof(float)*size);
		memcpy(U + i*size, _U[i], sizeof(float)*size);
	}
	for (int i = 0; i<size; i++)
	{
		delete[]_L[i];
		delete[]_U[i];
		delete[]_dataIn[i];
	}
	delete[]_U;
	delete[]_L;
	delete[]dataIn;
	return 0;
}


/*----------------------------非负矩阵分解---------------------------*/
long   Matrix_NMF(float** dataIn, float** W, float** H, int size1, int size2, int sizeF)
{
	//检查初始值
	if (dataIn == NULL || W == NULL || H == NULL)
		return -1;

	//设置最大迭代次数
	int maxIterator = 200;
	long lError = 0;

	//W，H的初始值设定都为随机的正数
	rand();
	for (int i = 0; i<size1; i++)
		for (int j = 0; j<sizeF; j++)
			W[i][j] = abs((float)rand() / RAND_MAX);
	for (int i = 0; i<sizeF; i++)
		for (int j = 0; j<size2; j++)
			H[i][j] = abs((float)rand() / RAND_MAX);

	//对W列进行归一化
	for (int i = 0; i<sizeF; i++)
	{
		float sum = 0.0f;
		for (int j = 0; j<size1; j++)
			sum += W[j][i];
		for (int j = 0; j<size1; j++)
			W[j][i] /= sum;
	}

	//迭代求解
	for (int i = 0; i<maxIterator; i++)
	{
		//临时数据申请
		float **tempWH = new float *[size1];
		float **tempDataIn = new float*[size1];
		float **tempWTrans = new float*[sizeF];
		float **tempWMuti = new float*[sizeF];
		float **tempHTrans = new float*[size2];
		float **tempHMuti = new float*[size1];
		for (int j = 0; j<size1; j++)
		{
			tempWH[j] = new float[size2];    memset(tempWH[j], 0, sizeof(float)*size2);
			tempDataIn[j] = new float[size2]; memset(tempDataIn[j], 0, sizeof(float)*size2);
			tempHMuti[j] = new float[sizeF]; memset(tempHMuti[j], 0, sizeof(float)*sizeF);
		}
		for (int j = 0; j<sizeF; j++)
		{
			tempWTrans[j] = new float[size1]; memset(tempWTrans[j], 0, sizeof(float)*size1);
			tempWMuti[j] = new float[size2]; memset(tempWMuti[j], 0, sizeof(float)*size2);
		}
		for (int j = 0; j<size2; j++)
		{
			tempHTrans[j] = new float[sizeF]; memset(tempHTrans[j], 0, sizeof(float)*sizeF);
		}

		lError = MatrixMuti(W, size1, sizeF, size2, H, tempWH);
		if (lError != 0)
			return lError;
		for (int j = 0; j<size1; j++)
			for (int k = 0; k<size2; k++)
				tempDataIn[j][k] = dataIn[j][k] / tempWH[j][k];

		//求W
		lError = MatrixTrans(H, sizeF, size2, tempHTrans);
		if (lError != 0)
			return lError;
		lError = MatrixMuti(tempDataIn, size1, size2, sizeF, tempHTrans, tempHMuti);
		if (lError != 0)
			return lError;

		for (int j = 0; j<size1; j++)
			for (int k = 0; k<sizeF; k++)
				W[j][k] = W[j][k] * tempHMuti[j][k];

		//对W列进行归一化
		for (int i = 0; i<sizeF; i++)
		{
			float sum = 0.0f;
			for (int j = 0; j<size1; j++)
				sum += W[j][i];
			for (int j = 0; j<size1; j++)
				W[j][i] /= sum;
		}


		//求H
		lError = MatrixTrans(W, size1, sizeF, tempWTrans);
		if (lError != 0)
			return lError;
		lError = MatrixMuti(tempWTrans, sizeF, size1, size2, tempDataIn, tempWMuti);
		if (lError != 0)
			return lError;

		for (int j = 0; j<sizeF; j++)
			for (int k = 0; k<size2; k++)
				H[j][k] = H[j][k] * tempWMuti[j][k];

		//清除所有指针释放内存
		for (int j = 0; j<size1; j++)
		{
			delete[]tempWH[j];
			delete[]tempDataIn[j];
			delete[]tempHMuti[j];
		}
		for (int j = 0; j<sizeF; j++)
		{
			delete[]tempWTrans[j];
			delete[]tempWMuti[j];
		}
		for (int j = 0; j<size2; j++)
		{
			delete[]tempHTrans[j];
		}
		delete[]tempWH; tempWH = NULL;
		delete[]tempDataIn; tempDataIn = NULL;
		delete[]tempHMuti; tempHMuti = NULL;
		delete[]tempWTrans; tempWTrans = NULL;
		delete[]tempWMuti; tempWMuti = NULL;
		delete[]tempHTrans; tempHTrans = NULL;
	}
	return lError;
}

long   Matrix_NMF(float* dataIn, float* W, float* H, int size1, int size2, int sizeF)
{
	if (dataIn == NULL || W == NULL || H == NULL)
		return -1;

	long lError = 0;
	float** tempdataIn = new float*[size1];
	float** tempW = new float*[size1];
	float** tempH = new float*[sizeF];

	for (int i = 0; i<size1; i++)
	{
		tempdataIn[i] = new float[size2];
		memcpy(dataIn + i*size2, tempdataIn[i], sizeof(float)*size2);
		tempW[i] = new float[sizeF];
		memcpy(tempW[i], W + i*sizeF, sizeof(float)*sizeF);
	}
	for (int i = 0; i<sizeF; i++)
	{
		tempH[i] = new float[size2];
		memcpy(tempH[i], H + i*size2, sizeof(float)*size2);
	}
	lError = Matrix_NMF(tempdataIn, tempW, tempH, size1, size2, sizeF);
	if (lError != 0)
		return lError;

	for (int i = 0; i<size1; i++)
	{
		memcpy(tempdataIn[i], dataIn + i*size2, sizeof(float)*size2);
		memcpy(W + i*sizeF, tempW[i], sizeof(float)*sizeF);
	}
	for (int i = 0; i<sizeF; i++)
	{
		memcpy(H + i*size2, tempH[i], sizeof(float)*size2);
	}

	//清除数据
	for (int i = 0; i<size1; i++)
	{
		delete[]tempdataIn[i];
		delete[]tempW[i];
	}
	for (int i = 0; i<sizeF; i++)
	{
		delete[]tempH[i];
	}
	delete[]tempdataIn; tempdataIn = NULL;
	delete[]tempW; tempW = NULL;
	delete[]tempH; tempH = NULL;

	return 0;
}


/*---------------------------矩阵的奇异值分解-------------------------*/
long   Matrix_SVD(float** dataIn, float** dataS, float** dataV, float**dataD, int size1, int size2)
{
	//判断输入
	if (dataIn == NULL || dataS == NULL || dataV == NULL || dataD == NULL)
		return -1;
	if (size1<1 || size2<1)
		return -2;

	//初始化S V D矩阵为0
	for (int i = 0; i<size1; i++)
		memset(dataS[i], 0, sizeof(float)*size1);
	for (int i = 0; i<size1; i++)
		memset(dataV[i], 0, sizeof(float)*size2);
	for (int i = 0; i<size2; i++)
		memset(dataD[i], 0, sizeof(float)*size2);

	//中间过程空间申请
	float **dataInT = new float*[size2];
	for (int i = 0; i<size2; i++)
		dataInT[i] = new float[size1];
	float **dataInM1 = new float*[size1];
	float **dataInI1 = new float*[size1];
	for (int i = 0; i<size1; i++)
	{
		dataInM1[i] = new float[size1];
		dataInI1[i] = new float[size1];
	}
	float **dataInM2 = new float*[size2];
	float **dataInI2 = new float*[size2];
	for (int i = 0; i<size2; i++)
	{
		dataInM2[i] = new float[size2];
		dataInI2[i] = new float[size2];
	}

	//特征值和特征向量空间申请
	float *eigvalue1 = new float[size1];
	float **eigmat1 = new float*[size1];
	for (int i = 0; i<size1; i++)
		eigmat1[i] = new float[size1];

	float *eigvalue2 = new float[size2];
	float **eigmat2 = new float*[size2];
	for (int i = 0; i<size2; i++)
		eigmat2[i] = new float[size2];

	//求解过程
	MatrixTrans(dataIn, size1, size2, dataInT);
	MatrixMuti(dataIn, size1, size2, size1, dataInT, dataInM1);
	MatrixMuti(dataInT, size2, size1, size2, dataIn, dataInM2);

	MatrixEigen_value_vec_Jccob(dataInM1, eigvalue1, size1, eigmat1);
	MatrixEigen_value_vec_Jccob(dataInM2, eigvalue2, size2, eigmat2);

	int minsize = min(size1, size2);

	//赋值
	for (int i = 0; i<size1; i++)
		memcpy(dataS[i], eigmat1[i], sizeof(float)*size1);
	for (int i = 0; i<size2; i++)
		memcpy(dataD[i], eigmat2[i], sizeof(float)*size2);
	for (int i = 0; i<minsize; i++)



		//清空指针
		for (int i = 0; i<size1; i++)
		{
			delete[]dataInM1[i];
			delete[]dataInI1[i];
			delete[]eigmat1[i];
		}
	delete[]dataInM1; dataInM1 = NULL;
	delete[]dataInI1; dataInI1 = NULL;
	delete[]eigmat1; eigmat1 = NULL;
	delete[]eigvalue1; eigvalue1 = NULL;

	for (int i = 0; i<size2; i++)
	{
		delete[]dataInT[i];
		delete[]dataInM2[i];
		delete[]dataInI2[i];
		delete[]eigmat2[i];
	}
	delete[]dataInT; dataInT = NULL;
	delete[]dataInM2; dataInM2 = NULL;
	delete[]dataInI2; dataInI2 = NULL;
	delete[]eigmat2; eigmat2 = NULL;
	delete[]eigvalue2; eigvalue2 = NULL;
	return 0;
}

long   Matrix_SVD(float* dataIn, float* dataS, float* dataV, float* dataD, int size1, int size2)
{
	if (dataIn == NULL || dataS == NULL || dataV == NULL || dataD == NULL)
		return 1;
	float **_dataIn;
	float **_dataS;
	float **_dataV;
	float **_dataD;

	_dataIn = new float*[size1];
	_dataS = new float *[size1];
	_dataV = new float *[size1];
	_dataD = new float *[size2];

	for (int i = 0; i<size1; i++)
	{
		_dataIn[i] = new float[size2];
		_dataS[i] = new float[size1];
		_dataV[i] = new float[size2];
		memcpy(_dataIn[i], dataIn + i*size2, sizeof(float)*size2);
	}
	for (int i = 0; i<size2; i++)
		_dataD[i] = new float[size2];
	Matrix_SVD(_dataIn, _dataS, _dataV, _dataD, size1, size2);

	for (int i = 0; i<size1; i++)
	{
		memcpy(dataS + i*size1, _dataS[i], sizeof(float)*size1);
		memcpy(dataV + i*size2, _dataV[i], sizeof(float)*size2);
	}
	for (int i = 0; i<size2; i++)
		memcpy(dataD + i*size2, _dataD[i], sizeof(float)*size2);

	for (int i = 0; i<size1; i++)
	{
		delete[]_dataS[i];
		delete[]_dataV[i];
		delete[]_dataIn[i];
	}
	for (int i = 0; i<size2; i++)
		delete[]_dataD[i];
	delete[]_dataS;
	delete[]_dataV;
	delete[]_dataIn;
	delete[]_dataD;
	return 0;
}


/*---------------------------求矩阵的秩------------------------*/
long   Matrix_Rank(float** dataIn, int size1, int size2, int &rank)
{
	//检查输入
	if (dataIn == NULL)
		return -1;

	float **datatemp = new float *[size1];
	for (int i = 0; i<size1; i++)
	{
		datatemp[i] = new float[size2];
		memcpy(datatemp[i], dataIn[i], sizeof(float)*size2);
	}

	//高斯消去  采用行消去法
	for (int i = 0; i<size2; i++)
	{
		//找到非0的行
		for (int j = i; j<size1; j++)
		{
			if (abs(datatemp[j][i])>0.0000001)
			{
				//消去所有行
				for (int k = 0; k<size1; k++)
				{
					float temp = datatemp[k][i] / datatemp[j][i];
					if (k == j)
						continue;
					//每一行
					for (int l = 0; l<size2; l++)
						datatemp[k][l] = datatemp[k][l] - datatemp[j][l] * temp;
				}
			}
			else
				continue;
		}
	}

	//求秩
	rank = 0;
	for (int i = 0; i<size1; i++)
	{
		bool iszero = true;
		for (int j = 0; j<size2; j++)
			if (abs(datatemp[i][j]) >= 0.0000001)
				iszero = false;
		if (!iszero)
			rank++;
	}

	return 0;
}

long   Matrix_Rank(float*  dataIn, int size1, int size2, int &rank)
{
	//检查输入
	long lError = 0;
	if (dataIn == NULL)
		return -1;
	float** datatemp = new float*[size1];
	for (int i = 0; i<size1; i++)
	{
		datatemp[i] = new float[size2];
		memcpy(datatemp[i], dataIn + i*size2, sizeof(float)*size2);
	}
	lError = Matrix_Rank(datatemp, size1, size2, rank);
	if (lError != 0)
		return lError;
	for (int i = 0; i<size1; i++)
		delete[]datatemp[i];
	delete[]datatemp;

	return 0;
}


/*---------------------------矩阵的满秩分解-------------------------*/
long   Matrix_FRD(float** dataIn, float** dataG, float** dataF, int size1, int size2, int rank)
{
	//检查输入矩阵是否正确
	if (dataIn == NULL || dataF == NULL || dataG == NULL)
		return -1;
	if (rank>min(size1, size2))
		return -2;
	float** dataRows;			//初等行变换
	float** dataCols;			//初等列变换
	int *dataLabel = new int[size2];
	memset(dataLabel, 0, sizeof(int)*size2);

	dataRows = new float*[size1];
	dataCols = new float*[size1];
	for (int i = 0; i<size1; i++)
	{
		dataRows[i] = new float[size2]; memset(dataRows[i], 0, sizeof(float)*size2);
		dataCols[i] = new float[size2]; memset(dataCols[i], 0, sizeof(float)*size2);
		memcpy(dataRows[i], dataIn[i], sizeof(float)*size2);
		memcpy(dataCols[i], dataIn[i], sizeof(float)*size2);
	}

	//初等行变换选取所有非0行
	//高斯消去  采用行消去法
	int size = min(size1, size2);
	for (int i = 0; i<size; i++)
	{
		//找到第一个不为0的
		//为0  则向下搜索 交换两行进行处理
		if (abs(dataRows[i][i]) >= 0.0000001)
		{
			dataLabel[i] = 1;	//确定保留的列
								//归一化此行
			float temp1 = dataRows[i][i];
			for (int j = 0; j<size2; j++)
				dataRows[i][j] /= temp1;
			//消去第i列
			for (int j = 0; j<size1; j++)
			{
				if (j != i)
				{
					temp1 = dataRows[j][i];
					for (int k = 0; k<size2; k++)
						dataRows[j][k] = dataRows[j][k] - temp1*dataRows[i][k];
				}
			}
		}
		else
		{
			//找到不为0的行
			int nonezeroLine = 0;
			for (int j = i; j<size1; j++)
			{
				if (abs(dataRows[j][i]) >= 0.0000001)
				{
					nonezeroLine = j;
					break;
				}
			}
			if (nonezeroLine == 0)
				continue;
			dataLabel[i] = nonezeroLine;
			//交换两行
			for (int j = 0; j<size2; j++)
				swap(dataRows[nonezeroLine][j], dataRows[i][j]);

			//归一化此行
			float temp1 = dataRows[i][i];
			for (int j = 0; j<size2; j++)
				dataRows[i][j] /= temp1;
			//消去第i列
			for (int j = 0; j<size1; j++)
			{
				if (j != i)
				{
					temp1 = dataRows[j][i];
					for (int k = 0; k<size2; k++)
						dataRows[j][k] = dataRows[j][k] - temp1*dataRows[i][k];

				}
			}
		}
	}

	//得到分解的矩阵
	//行矩阵
	for (int i = 0; i<rank; i++)
		memcpy(dataG[i], dataRows[i], sizeof(float)*size2);
	for (int i = 0, k = 0; i<size2; i++)
	{
		if (dataLabel[i] != 0)
		{
			for (int j = 0; j<size1; j++)
				dataF[j][k] = dataIn[j][i];
			k++;
		}
	}

	//清除数据
	for (int i = 0; i<size1; i++)
	{
		delete[]dataRows[i];
		delete[]dataCols[i];
	}
	delete[]dataRows;
	delete[]dataCols;
	delete[]dataLabel;

	return 0;

}

long   Matrix_FRD(float * dataIn, float * dataG, float * dataF, int size1, int size2, int rank)
{
	if (dataIn == NULL || dataF == NULL || dataG == NULL)
		return -1;
	float** dataTIn = new float*[size1];
	float** dataTF = new float*[rank];
	float** dataTG = new float*[size1];
	for (int i = 0; i<size1; i++)
	{
		dataTIn[i] = new float[size2];
		memcpy(dataTIn[i], dataIn + i*size2, sizeof(float)*size2);
		dataTG[i] = new float[rank];
	}
	for (int i = 0; i<rank; i++)
		dataTF[i] = new float[size2];


	Matrix_FRD(dataTIn, dataTF, dataTG, size1, size2, rank);

	for (int i = 0; i<size1; i++)
		memcpy(dataG + i*rank, dataTG[i], sizeof(float)*rank);
	for (int i = 0; i<rank; i++)
		memcpy(dataF, dataTF[i], sizeof(float)*size2);

	for (int i = 0; i<size1; i++)
	{
		delete[]dataTIn[i];
		delete[]dataTG[i];
	}
	for (int i = 0; i<rank; i++)
		delete[]dataTF[i];

	delete[]dataTIn;
	delete[]dataTF;
	delete[]dataTG;

	return 0;
}


/*---------------------------求解矩阵的广义逆-------------------------*/
long   Matrix_GenInverse(float** dataIn, int size1, int size2, float** dataInverse)
{
	//检查输入
	if (dataIn == NULL || dataInverse == NULL)
		return -1;

	//求矩阵的秩
	int rank = 0;
	Matrix_Rank(dataIn, size1, size2, rank);

	//计算空间申请
	float** dataF = new float*[size1];
	for (int i = 0; i<size1; i++)
		dataF[i] = new float[rank];
	float** dataG = new float*[rank];
	for (int i = 0; i<rank; i++)
		dataG[i] = new float[size2];
	float** dataTG = new float*[size2];
	for (int i = 0; i<size2; i++)
		dataTG[i] = new float[rank];
	float** dataTF = new float*[rank];
	for (int i = 0; i<rank; i++)
		dataTF[i] = new float[size1];
	float** FTA = new float*[rank];
	for (int i = 0; i<rank; i++)
		FTA[i] = new float[size2];
	float** FTAGT = new float*[rank];
	for (int i = 0; i<rank; i++)
		FTAGT[i] = new float[rank];
	float** invFTAGT = new float*[rank];
	for (int i = 0; i<rank; i++)
		invFTAGT[i] = new float[rank];
	float** GTinvFTAGT = new float*[size2];
	for (int i = 0; i<size2; i++)
		GTinvFTAGT[i] = new float[rank];

	//计算
	//A—=Gt(FtAGt)^-1Ft
	Matrix_FRD(dataIn, dataG, dataF, size1, size2, rank);
	MatrixTrans(dataF, size1, rank, dataTF);
	MatrixTrans(dataG, rank, size2, dataTG);
	MatrixMuti(dataTF, rank, size1, size2, dataIn, FTA);
	MatrixMuti(FTA, rank, size2, rank, dataTG, FTAGT);
	MatrixInverse(FTAGT, rank, invFTAGT);
	MatrixMuti(dataTG, size2, rank, rank, invFTAGT, GTinvFTAGT);
	MatrixMuti(GTinvFTAGT, size2, rank, size1, dataTF, dataInverse);

	//清除指针
	for (int i = 0; i<size1; i++)
		delete[] dataF[i];
	for (int i = 0; i<rank; i++)
		delete[] dataG[i];
	for (int i = 0; i<size2; i++)
		delete[] dataTG[i];
	for (int i = 0; i<rank; i++)
		delete[] dataTF[i];
	for (int i = 0; i<rank; i++)
		delete[] FTA[i];
	for (int i = 0; i<rank; i++)
		delete[] FTAGT[i];
	for (int i = 0; i<rank; i++)
		delete[] invFTAGT[i];
	for (int i = 0; i<size2; i++)
		delete[] GTinvFTAGT[i];

	delete[]  dataF;
	delete[]  dataG;
	delete[]  dataTG;
	delete[]  dataTF;
	delete[]  FTA;
	delete[]  FTAGT;
	delete[]  invFTAGT;
	delete[]  GTinvFTAGT;

	return 0;
}

long   Matrix_GenInverse(float * dataIn, int size1, int size2, float * dataInverse)
{
	if (!dataIn || !dataInverse)
		return 1;
	if (size1 <= 0 || size2 <= 0)
		return 2;
	float** mdataIn;
	float** mdataOut;
	mdataIn = new float*[size1];
	mdataOut = new float*[size2];
	for (int i = 0; i<size1; i++)
	{
		mdataIn[i] = new float[size2];
		memcpy(mdataIn[i], dataIn + i*size2, sizeof(float)*size2);
	}
	for (int i = 0; i<size2; i++)
		mdataOut[i] = new float[size1];
	Matrix_GenInverse(mdataIn, size1, size2, mdataOut);
	for (int i = 0; i<size2; i++)
		memcpy(dataInverse + i*size1, mdataOut[i], sizeof(float)*size1);

	for (int i = 0; i<size1; i++)
		delete[]mdataIn[i];
	for (int i = 0; i<size2; i++)
		delete[]mdataOut[i];

	delete[]mdataIn;
	delete[]mdataOut;

	return 0;
}

/*---------------------------求矩阵分解-X=AS--------------------------*/
long   Matrix_AS(float** dataIn, float** dataA, float** dataS, int size1, int size2, int sized)
{
	//检查输入
	if (dataIn == NULL || dataA == NULL || dataS == NULL)
		return -1;

	if (size1<size2)
	{
		printf("行数小于列数！\n");
		return -1;
	}

	float** data1 = new float*[size1];
	float** data2 = new float*[size1];
	float** data3 = new float*[size2];
	for (int i = 0; i<size1; i++)
	{
		data1[i] = new float[size1];
		data2[i] = new float[size2];
	}
	for (int i = 0; i<size2; i++)
		data3[i] = new float[size2];

	float** data1d = new float*[size1];
	for (int i = 0; i<size1; i++)
		data1d[i] = new float[sized];

	//进行SVD分解
	Matrix_SVD(dataIn, data1, data2, data3, size1, size2);

	//获取截尾奇异值
	for (int i = 0; i<size1; i++)
		for (int j = 0; j<sized; j++)
			data1d[i][j] = data1[i][j];

	float** dataU1, **dataU2;
	dataU1 = new float*[size1 - 1];
	dataU2 = new float*[size1 - 1];
	for (int i = 0; i<size1 - 1; i++)
	{
		dataU1[i] = new float[sized];
		dataU2[i] = new float[sized];
	}

	//取截尾奇异值的上M-1行U1和下M-1行U2
	for (int i = 0; i<size1 - 1; i++)
		for (int j = 0; j<sized; j++)
			dataU1[i][j] = data1d[i][j];
	for (int i = 1; i<size1; i++)
		for (int j = 0; j<sized; j++)
			dataU2[i - 1][j] = data1d[i][j];

	float** dataGInv = new float*[sized];
	for (int i = 0; i<sized; i++)
		dataGInv[i] = new float[size1 - 1];
	Matrix_GenInverse(dataU1, size1 - 1, sized, dataGInv);

	float** dataMuti = new float*[sized];
	for (int i = 0; i<sized; i++)
		dataMuti[i] = new float[sized];
	MatrixMuti(dataGInv, sized, size1 - 1, sized, dataU2, dataMuti);

	float** data4 = new float*[sized];
	float** data5 = new float*[sized];
	float** data6 = new float*[sized];
	float*  dataEigv = new float[sized];
	for (int i = 0; i<sized; i++)
	{
		data4[i] = new float[sized]; memset(data4[i], 0, sizeof(float)*sized);
		data5[i] = new float[sized]; memset(data5[i], 0, sizeof(float)*sized);
		data6[i] = new float[sized]; memset(data6[i], 0, sizeof(float)*sized);
	}

	//相似变换
	//Matrix_SVD(dataMuti,data4,data5,data6,sized,sized);

	ofstream ofs("test.txt");
	for (int i = 0; i<sized; i++)
	{
		for (int k = 0; k<sized; k++)
		{
			ofs << dataMuti[i][k] << " ";
		}
		ofs << endl;
	}
	ofs.close();

	MatrixEigen_value_vec_InvPower(dataMuti, dataEigv, sized, data4);

	float** dataTU = new float*[sized];
	for (int i = 0; i<sized; i++)
		dataTU[i] = new float[size1];
	MatrixTrans(data1d, size1, sized, dataTU);

	float** dataTUM = new float*[sized];
	for (int i = 0; i<sized; i++)
		dataTUM[i] = new float[size1];

	//求S
	MatrixMuti(data4, sized, sized, size1, dataTU, dataTUM);
	MatrixMuti(dataTUM, sized, size1, size2, dataIn, dataS);

	//求A
	float** dataInv1d = new float*[sized];
	for (int i = 0; i<sized; i++)
		dataInv1d[i] = new float[sized];
	MatrixInverse(data4, sized, dataInv1d);
	for (int i = 0; i<size1; i++)
		memset(dataA[i], 0, sizeof(float)*sized);
	MatrixMuti(data1d, size1, sized, sized, dataInv1d, dataA);

	/////////////////////////////////清除指针/////////////////////////////////////////
	for (int i = 0; i<size1; i++)
	{
		delete[]data1[i];
		delete[]data2[i];
	}
	for (int i = 0; i<size2; i++)
		delete[]data3[i];
	delete[] data1;
	delete[] data2;
	delete[] data3;

	for (int i = 0; i<size1; i++)
		delete[] data1d[i];
	delete[]data1d;

	for (int i = 0; i<size1 - 1; i++)
	{
		delete[]dataU1[i];
		delete[]dataU2[i];
	}
	delete[]dataU1;
	delete[]dataU2;

	for (int i = 0; i<sized; i++)
		delete[]dataGInv[i];
	delete[] dataGInv;

	for (int i = 0; i<sized; i++)
		delete[]dataMuti[i];
	delete[]dataMuti;

	for (int i = 0; i<sized; i++)
	{
		delete[]data4[i];
		delete[]data5[i];
		delete[]data6[i];
	}
	delete[]data4;
	delete[]data5;
	delete[]data6;

	for (int i = 0; i<sized; i++)
		delete[]dataTU[i];
	delete[] dataTU;

	for (int i = 0; i<sized; i++)
		delete[]dataTUM[i];
	delete[]dataTUM;

	for (int i = 0; i<sized; i++)
		delete[]dataInv1d[i];
	delete[]dataInv1d;

	delete[]dataEigv;

	return 0;
}

long   Matrix_AS(float * dataIn, float * dataA, float * dataS, int size1, int size2, int sized)
{
	//判断输入是存在问题
	if (dataIn == NULL || dataA == NULL || dataS == NULL)
		return -1;

	float** dataX = new float*[size1];
	float** dataA1 = new float*[size1];
	float** dataS1 = new float*[sized];
	for (int i = 0; i<size1; i++)
	{
		dataX[i] = new float[size2];
		memcpy(dataX[i], dataIn + i*size2, sizeof(float)*size2);
		dataA1[i] = new float[sized];
		memset(dataA1[i], 0, sizeof(float)*sized);
	}
	for (int i = 0; i<sized; i++)
		dataS1[i] = new float[size2];

	Matrix_AS(dataX, dataA1, dataS1, size1, size2, sized);

	for (int i = 0; i<size1; i++)
		memcpy(dataA + i*sized, dataA1[i], sizeof(float)*sized);
	for (int i = 0; i<sized; i++)
		memcpy(dataS + i*size2, dataS1[i], sizeof(float)*size2);

	for (int i = 0; i<size1; i++)
	{
		delete[]dataA1[i];
		delete[]dataX[i];
	}
	for (int i = 0; i<sized; i++)
		delete[]dataS1[i];

	delete[]dataX;
	delete[]dataA1;
	delete[]dataS1;

	return 0;
}
