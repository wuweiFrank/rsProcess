#ifndef __MATRIX_OPERATION__
#define __MATRIX_OPERATION__
/*
								矩阵设定：
											Y
							      ______________________
								  |__|__|__|__|__|__|__|
								  |__|__|__|__|__|__|__|
						       X  |__|__|__|__|__|__|__|
								  |__|__|__|__|__|__|__|
								  |__|__|__|__|__|__|__|

*/
/********************************************************************************************************/
/* 								       矩阵基本运算                                                     */
/********************************************************************************************************/
/*----------------------------- 矩阵乘常数------------------------------*/
long  MatrixMutiConstant(double **dataIn,int size1,int size2,double cnst,double** dataOut);
long  MatrixMutiConstant(double *dataIn, int size1,int size2,double cnst,double *dataOut);

/*----------------------------- 矩阵除常数------------------------------*/
long  MatrixDiviConstant(double **dataIn,int size1,int size2,double cnst,double** dataOut);
long  MatrixDiviConstant(double *dataIn, int size1,int size2,double cnst,double *dataOut);

/*-------------------------------矩阵转置-------------------------------*/
long  MatrixTrans(double **dataIn,int size1,int size2,double** dataOut);
long  MatrixTrans(double * dataIn,int size1,int size2,double*  dataOut);

/*--------------------------------矩阵乘积------------------------------*/
long  MatrixMuti(double **dataIn1,int size1,int size2,int size3,double** dataIn2,double** dataOut);
long  MatrixMuti(double *dataIn1,int size1,int size2,int size3,double* dataIn2,double* dataOut);

/*--------------------------------矩阵求逆------------------------------*/
long  MatrixInverse(double **dataIn,int size,double **dataOut);
long  MatrixInverse(double *dataIn,int size,double *dataOut);

/*-----------------------------矩阵求行列式------------------------------*/
long  MatrixDet(double **dataIn,int size,double& det);
long  MatrixDet(double *dataIn ,int size,double& det);

/*-------------------------------旋转矩阵-------------------------------*/
long   MatrixRotate(double **dataIn,double phia,double omega,double kappa);
long   MatrixRotate(double *dataIn,double phia,double omega,double kappa);

/********************************************************************************************************/
/*								    矩阵高阶运算                                                        */
/********************************************************************************************************/
long MatrixLST(double** dataSrc,double *dataDst,double *params,int variableNum,int equationNum,bool isConstant=true);
long MatrixLST(double*  dataSrc,double *dataDst,double *params,int variableNum,int equationNum,bool isConstant=true);

/*--------------------------线性方程组的稀疏求解------------------------*/
//MP方法进行求解
long Matrix_Sparse_MatchPursuit(double** dictionary, double* data1, double* sparse, int size1, int size2);
long Matrix_Sparse_MatchPursuit(double* dictionary, double* data1, double* sparse, int size1, int size2);

/*----------------------------特征值和特征向量--------------------------*/
//雅可比法是求对称矩阵的特征值和特征向量
long   MatrixEigen_value_vec_Jccob(double **dataIn,double *eignValue,int size,double **mat);
long   MatrixEigen_value_vec_Jccob(double *dataIn,double *eignValue,int size,double *mat);

/*----------------------矩阵化为上Hseesnberg矩阵------------------------*/
long   MatrixHessenberg(double **dataIn,int size,double **dataOut);
long   MatrixEigenValue(double **dataIn,int size,int loopNum,double error,double *eignvalue);

/*----------------------根据特征值使用反幂法求特征向量------------------*/
long   MatrixEigen_value_vec(double **dataIn,double *eignValue,int size,double **mat);
long   MatrixEigen_value_vec_InvPower(double **dataIn,double *eignValue,int size,double **mat);
long   MatrixEigen_value_vec_InvPower(double *dataIn,double*eignValue,int size,double *mat);

/*----------------------------正定矩阵的Cholesky分解---------------------------*/
//直接分解法：需要进行开方运算
long   Matrix_Cholesky(double **dataIn,double** dataOut,int size);
long   Matrix_Cholesky(double *dataIn,double* dataOut,int size);

/*----------------------------矩阵的LU分解---------------------------*/
long   Matrix_Doolittle(double **dataIn, double **L, double **U, int size);
long   Matrix_Doolittle(double *dataIn,  double *L ,  double *U, int size);

/*----------------------------非负矩阵分解---------------------------*/
long   Matrix_NMF(double** dataIn,double** W,double** H, int size1,int size2,int sizeF);
long   Matrix_NMF(double* dataIn,double* W,double* H, int size1,int size2,int sizeF);

//SVD分解 矩阵的奇异值分解 A=SVD 
//A为一个m*n的矩阵 S为一个m*m的矩阵，V为一个m*n的矩阵，D为一个n*n的矩阵
long   Matrix_SVD(double** dataIn,double** dataS,double** dataV,double**dataD,int size1,int size2);
long   Matrix_SVD(double*  dataIn,double*  dataS,double*  dataV,double* dataD,int size1,int size2);

//求矩阵的秩
long   Matrix_Rank(double** dataIn,int size1,int size2,int &rank);
long   Matrix_Rank(double * dataIn,int size1,int size2,int &rank);

//矩阵的满秩分解 full rank decomposition
long   Matrix_FRD(double** dataIn,double** dataG,double** dataF,int size1,int size2,int rank);
long   Matrix_FRD(double * dataIn,double * dataG,double * dataF,int size1,int size2,int rank);

//求解矩阵的广义逆
long   Matrix_GenInverse(double** dataIn,int size1,int size2,double** dataInverse);
long   Matrix_GenInverse(double * dataIn,int size1,int size2,double * dataInverse);

/*---------------------------求矩阵分解-X=AS--------------------------*/
long   Matrix_AS(double** dataIn,double** dataA,double** dataS,int size1,int size2,int sized);
long   Matrix_AS(double * dataIn,double * dataA,double * dataS,int size1,int size2,int sized);

//-------------------------------------------------------------FLOAT类型------------------------------------------------------------------
long  MatrixMutiConstant(float **dataIn, int size1, int size2, float cnst, float** dataOut);
long  MatrixMutiConstant(float *dataIn, int size1, int size2, float cnst, float *dataOut);

/*----------------------------- 矩阵除常数------------------------------*/
long  MatrixDiviConstant(float **dataIn, int size1, int size2, float cnst, float** dataOut);
long  MatrixDiviConstant(float *dataIn, int size1, int size2, float cnst, float *dataOut);

/*-------------------------------矩阵转置-------------------------------*/
long  MatrixTrans(float **dataIn, int size1, int size2, float** dataOut);
long  MatrixTrans(float * dataIn, int size1, int size2, float*  dataOut);

/*--------------------------------矩阵乘积------------------------------*/
long  MatrixMuti(float **dataIn1, int size1, int size2, int size3, float** dataIn2, float** dataOut);
long  MatrixMuti(float *dataIn1, int size1, int size2, int size3, float* dataIn2, float* dataOut);

/*--------------------------------矩阵求逆------------------------------*/
long  MatrixInverse(float **dataIn, int size, float **dataOut);
long  MatrixInverse(float *dataIn, int size, float *dataOut);

/*-----------------------------矩阵求行列式------------------------------*/
long  MatrixDet(float **dataIn, int size, float& det);
long  MatrixDet(float *dataIn, int size, float& det);

/*-------------------------------旋转矩阵-------------------------------*/
long   MatrixRotate(float **dataIn, float phia, float omega, float kappa);
long   MatrixRotate(float *dataIn, float phia, float omega, float kappa);

/********************************************************************************************************/
/*								    矩阵高阶运算                                                        */
/********************************************************************************************************/
long MatrixLST(float** dataSrc, float *dataDst, float *params, int variableNum, int equationNum, bool isConstant = true);
long MatrixLST(float*  dataSrc, float *dataDst, float *params, int variableNum, int equationNum, bool isConstant = true);

/*--------------------------线性方程组的稀疏求解------------------------*/
//MP方法进行求解
long Matrix_Sparse_MatchPursuit(float** dictionary, float* data1, float* sparse, int size1, int size2);
long Matrix_Sparse_MatchPursuit(float* dictionary, float* data1, float* sparse, int size1, int size2);

//使用OMP方法进行求解
long Matrix_Sparse_OrthoMatchPursuit(float** dictionary, float* data1, float* sparse, int size1, int size2);
long Matrix_Sparse_OrthoMatchPursuit(float*  dictionary, float* data1, float* sparse, int size1, int size2);


/*----------------------------特征值和特征向量--------------------------*/
//雅可比法是求对称矩阵的特征值和特征向量
long   MatrixEigen_value_vec_Jccob(float **dataIn, float *eignValue, int size, float **mat);
long   MatrixEigen_value_vec_Jccob(float *dataIn, float *eignValue, int size, float *mat);

/*----------------------矩阵化为上Hseesnberg矩阵------------------------*/
long   MatrixHessenberg(float **dataIn, int size, float **dataOut);
long   MatrixEigenValue(float **dataIn, int size, int loopNum, float error, float *eignvalue);

/*----------------------根据特征值使用反幂法求特征向量------------------*/
long   MatrixEigen_value_vec(float **dataIn, float *eignValue, int size, float **mat);
long   MatrixEigen_value_vec_InvPower(float **dataIn, float *eignValue, int size, float **mat);
long   MatrixEigen_value_vec_InvPower(float *dataIn, float*eignValue, int size, float *mat);

/*----------------------------正定矩阵的Cholesky分解---------------------------*/
//直接分解法：需要进行开方运算
long   Matrix_Cholesky(float **dataIn, float** dataOut, int size);
long   Matrix_Cholesky(float *dataIn, float* dataOut, int size);

/*----------------------------矩阵的LU分解---------------------------*/
long   Matrix_Doolittle(float **dataIn, float **L, float **U, int size);
long   Matrix_Doolittle(float *dataIn, float *L, float *U, int size);

/*----------------------------非负矩阵分解---------------------------*/
long   Matrix_NMF(float** dataIn, float** W, float** H, int size1, int size2, int sizeF);
long   Matrix_NMF(float* dataIn, float* W, float* H, int size1, int size2, int sizeF);

//SVD分解 矩阵的奇异值分解 A=SVD 
//A为一个m*n的矩阵 S为一个m*m的矩阵，V为一个m*n的矩阵，D为一个n*n的矩阵
long   Matrix_SVD(float** dataIn, float** dataS, float** dataV, float**dataD, int size1, int size2);
long   Matrix_SVD(float*  dataIn, float*  dataS, float*  dataV, float* dataD, int size1, int size2);

//求矩阵的秩
long   Matrix_Rank(float** dataIn, int size1, int size2, int &rank);
long   Matrix_Rank(float * dataIn, int size1, int size2, int &rank);

//矩阵的满秩分解 full rank decomposition
long   Matrix_FRD(float** dataIn, float** dataG, float** dataF, int size1, int size2, int rank);
long   Matrix_FRD(float * dataIn, float * dataG, float * dataF, int size1, int size2, int rank);

//求解矩阵的广义逆
long   Matrix_GenInverse(float** dataIn, int size1, int size2, float** dataInverse);
long   Matrix_GenInverse(float * dataIn, int size1, int size2, float * dataInverse);

/*---------------------------求矩阵分解-X=AS--------------------------*/
long   Matrix_AS(float** dataIn, float** dataA, float** dataS, int size1, int size2, int sized);
long   Matrix_AS(float * dataIn, float * dataA, float * dataS, int size1, int size2, int sized);

#endif // MATRIXOPERATION_H
