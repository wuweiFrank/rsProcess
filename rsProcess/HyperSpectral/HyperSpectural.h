#ifndef HYPERSPECTURAL_H
#define HYPERSPECTURAL_H
//#include "Eigen/Core"

/********************************************************************************************************/
/*						                        文件操作                                                */
/********************************************************************************************************/
//将变换矩阵存入文件中
//fileName		:输入的文件名
//matrix		:将写入文件的矩阵
//size			:矩阵的大小
//返回值0		:执行成功
//返回值1		:输入指针为空
//返回值2		:输入数据有错误
//返回值3		:其他错误
long  Matrix_Export_File(const char* fileName,double **matrix,int size);


//获得变换矩阵的大小
//fileName		:输入的文件名
//size			:矩阵的大小
//返回值0		:执行成功
//返回值1		:输入指针为空
//返回值2		:输入数据有错误
//返回值3		:其他错误
long  Matrix_Get_Size(const char* fileName,int &size);


//将文件中的数据读入变换矩阵中
//fileName		:输入的文件名
//matrix		:将文件读入的矩阵
//size			:矩阵的大小
//返回值0		:执行成功
//返回值1		:输入指针为空
//返回值2		:输入数据有错误
//返回值3		:其他错误
long  Matrix_Import_File(const char* fileName,double **matrix);

//将端元输出到文件中
//fileName		:输入的文件名
//endmember		：待输出的端元数据
//bands			：影像波段数
//numbers		：端元数
//返回值0		:执行成功
//返回值1		:输入指针为空
//返回值2		:输入数据有错误
//返回值3		:其他错误
//  [1/22/2015 wuwei just you]
long  EndMember_Export_File(const char* fileName,double *endmember,int bands,int numbers);

//从文件中读取端元
long  EndMember_Import_File(const char* fileName,double *endmember,int bands,int numbers);

//获取训练区样本数据
//获取envi训练区个数
long  EnviROINumbers(const char* roiFile,int &roiNumbers);

//获取每个训练区域样本个数
long  EnviROIPts(const char* roiFile,int* roiPts,int roiNmbers);

//获取数据
//获取ROI区域数据
long  EnviROIData(const char* roiFile,int* roiPts,int roiNumbers,int bands,double** roiData);

//////////////////////////////////////////////////////////////////////////
/********************************************************************************************************/
/*						                      影像统计量                                                */
/********************************************************************************************************/
//  [1/22/2015 wuwei just you]
//计算两个数据之间的光谱角
long  Spectral_Angle_Distance(double* data1,double* data2,int bands,double &angle);
//计算影像上某一个像元与数据之间的光谱角
//xsize	,ysize ：影像大小
//xpos  ,ypos  ：影像上的位置
//data		   ：待计算的数据
//bands		   ：波段数
//angle		   ：计算的光谱角
long  Spectral_Angle_Distance(double* imgBuffer,int xsize,int ysize,int xpos,int ypos,double *data,int bands,double& angle);


//多光谱数据整合为一个波段
void  Layerstacking(char *pathIn, int begNum,int endNum,char* pathOut);

//imgBuffer		:多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//entropy		:计算每个波段的信息熵
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  GetImageEntropy(double *imgBuffer,int bandSum,int* bandNo,int xsize,
														int ysize,double* entropy);

//获取影像部分区域
//imgBuffer		:输入影像
//bands			:输入数据波段数
//xpos			:获取区域的起始位置
//ypos			:获取区域的起始位置
//xsize			:获取区域x的大小
//ysize			:获取区域y的大小
double* GetpartImage(double* imgBuffer,int bands,int imgxsize,int imgysize,int xpos,int ypos,int xsize,int ysize);

//计算影像的平均值
//imgBuffer		:多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//meanValue		:计算获得的波段的均值
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  GetImgMean(double *imgBuffer,int bandSum,int* bandNo,int xsize,
													int ysize,double* meanValue);

//计算每个波段影像的标准差
//imgBuffer		:多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//meanValue		:输入的波段均值
//stdv			:每个波段影像的标准差
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  GetImgStdCovarianve(double *imgBuffer,int bandSum,int* bandNo,
								int xsize,int ysize,double *meanValue,double* stdv);


//计算对应波段的协方差矩阵
//imgBuffer		:多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//covarianceMat	:计算获得的波段间的协方差矩阵
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  GetCovarianceMatrix(double *imgBuffer,int bandSum,int* bandNo,
									int xsize,int ysize,double** covarianceMat);


//计算每个波段的噪声影像
//imgBuffer		:多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//sampleX		:噪声的X采样间隔
//sampleY		:噪声的Y采样间隔
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//imgNoise		:噪声影像按照BSQ格式存储
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  GetNoiseImg(double *imgBuffer,int bandSum,int* bandNo,int sampleX,
								int sampleY,int xsize,int ysize,double* imgNoise);
//获取高光谱影像某一行的横切面组成影像
//const char*pathIn		：输入影像路径
//int lineIdx			：影像很切面线
//const char* pathOut	：输出影像路径
long  GetCutLineImg(const char*pathIn,int lineIdx,const char* pathOut);

//获取高光谱影像某一行的横切面组成影像
//const char*pathIn		：输入影像路径
//const char* path_plant_coeff:普朗克线性化系数路径
//int lineIdx			：影像很切面线
//const char* pathOut	：输出影像路径
long  GetCutLineImg(const char*pathIn,const char* path_plant_coeff,int lineIdx,const char* pathOut);

/********************************************************************************************************/
/*						                        数据球化                                                 */
/********************************************************************************************************/
//数据去中心化
//imgBufferIn	:输入多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
long  Decentered(double *imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut);


//数据白化
//去除数据之间波段的相关性
//imgBufferIn	:输入多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
long  Derelated(double *imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut);


//数据球化
//imgBufferIn	:输入多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
void  SphereTrans(double *imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut);


//添加GDAL影像读写库时直接以路径封装P数据球化
//进行PCA变换
//pathSrc		:输入影像路径
//pathDst		:输出影像路径
//pathDecenter  :输出去中心化影像
//bandNo		:选取进行变换的波段
void  SphereTrans(char* pathSrc,char* pathDst,char* pathDecenter,int* bandNo,int num);

/********************************************************************************************************/
/*						                        MNF变换                                                 */
/********************************************************************************************************/
//获得MNF算子
//covmatNoise	:噪声影像的协方差矩阵
//covmatImg		:影像的协方差矩阵
//mnfOperator	:MNF变换算子
//size			:矩阵的阶数
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  GetMNFOperator(double **covmatNoise,double **covmatImg,double** mnfOperator,int size);


//根据已有的矩阵进行MNF变换
//imgBufferIn	:输入多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//mnfOperator	:MNF变换算子
//imgBufferOut	:输出的影像按BSQ存储
//path			:输出变换矩阵到文件中的位置
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  MNF(double *imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut,char* path);


//根据已有的矩阵进行MNF反变换
//imgBufferIn	:输入多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//mnfOperator	:MNF变换算子
//imgBufferOut	:输出的影像按BSQ存储
//path			:输出变换矩阵到文件中的位置
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  InvMNF(double* imgBufferIn,int bandSum,int *bandNo,int xsize,int ysize,double *imgBufferOut,char *path);

//添加GDAL影像读写库时直接以路径封装MNF变换和MNF反变换
//进行MNF变换
//pathSrc		:输入影像路径
//pathDst		:输出影像路径
//bandNo		:选取进行变换的波段
//pathMartix	:变换矩阵路径
void  MNFTrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix);

//进行PCA反变换
//pathSrc		:输入影像路径
//pathDst		:输出影像路径
//bandNo		:选取进行反变换的波段
//pathMartix	:变换矩阵路径
void  InvMNFTrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix);

/********************************************************************************************************/
/*						                        PCA变换                                                 */
/********************************************************************************************************/
//获得PCA算子
//covmatNoise	:噪声影像的协方差矩阵
//covmatImg		:影像的协方差矩阵
//mnfOperator	:MNF变换算子
//size			:矩阵的阶数
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  GetPCAOperator(double **covmatImg,double **pcaOperator,int size);


//根据已有的矩阵进行PCA变换
//imgBufferIn	:输入多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//pcaOperator	:PCA变换算子
//imgBufferOut	:输出的影像按BSQ存储
//path			:输出变换矩阵
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  PCA(double *imgBufferIn,int bandSum,int *bandNo,
				int xsize,int ysize,double *imgBufferOut,char* path);


//根据已有的矩阵进行PCA反变换变换
//imgBufferIn	:输入多个波段的影像数组按BSQ存储
//bandSum		:读入影像的波段数
//bandNo		:读入影像的波段序号
//xsize			:影像一行像元的数目
//ysize			:影像一列像元的数目
//pcaOperator	:PCA变换算子
//imgBufferOut	:输出的影像按BSQ存储
//path			:输入变换矩阵
//返回值1：传入的指针为空
//返回值2：传入的数据有误
//返回值3：其他错误
long  InvPCA(double *imgBufferIn,int bandSum,int *bandNo,
					int xsize,int ysize,double *imgBufferOut,char* path);

//添加GDAL影像读写库时直接以路径封装PCA变换和PCA反变换
//进行PCA变换
//pathSrc		:输入影像路径
//pathDst		:输出影像路径
//bandNo		:选取进行变换的波段
//pathMartix	:变换矩阵路径
void  PCATrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix);

//进行PCA反变换
//pathSrc		:输入影像路径
//pathDst		:输出影像路径
//bandNo		:选取进行反变换的波段
//pathMartix	:变换矩阵路径
void  InvPCATrans(char* pathSrc,char* pathDst,int* bandNo,int num,char* pathMartix);

/********************************************************************************************************/
/*						                        RXD算法                                                 */
/********************************************************************************************************/
//RX算法检测影像的异常
//imgBufferIn	:输入影像
//imgBufferOut	:输出影像
//xsize			:影像x方向大小
//ysize			:影像y方向大小
//bgWndsize		:背景窗口大小
//tWndsize		:目标窗口大小
//bands			:影像的波段数
void  RXD(double *imgBufferIn,double* imgBufferOut,int xsize,int ysize,int bands,int bgWndsize,int tWndsize);

//pathSrc		:输入影像路径
//pathDst		:输出影像路径
//bgwndsize		:背景窗口大小
//tWndsize		:目标窗口的大小
//一般背景窗口比目标窗口大得多
void  RXD(char* pathSrc,char* pathDst,int bgWndsize,int tWndsize);


//////////////////////////////////////////////////////////////////////////
/********************************************************************************************************/
/*						                     FastICA算法                                                */
/********************************************************************************************************/
//非线性函数转换
//tan 函数 a1为函数的参数
long g_tanhFunction(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output);
long g_derivative_tanh(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output);
//exp 指数函数
long g_expFunction(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output);
long g_derivative_exp(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output);
//三次幂函数
long g_power3Function(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output);
long g_derivative_power3(double* w,double* x_imput,int bands,int xsize,int ysize,double* x_output);

//基于负熵最大化的FastICA算法
//imgBufferIn	输入影像
//xsize ysize	影像的大小
//bandSum		影像波段数
//IpNum			独立成分个数
//imgBufferOut	输出结果
long  FastICA(double* imgBufferIn,int xsize,int ysize,int bandSum,int IpNum,double* imgBufferOut);

//GDAL封装的FastICA算法
//pathSrc		输入影像路径
//pathDst		输出影像路径
//IpNum			独立成分个数
void  FastICA(char* pathSrc,char* pathDst,int IpNum);


/********************************************************************************************************/
/*						                             FastFinder算法                                     */
/********************************************************************************************************/
//计算单形体体积
long  SingleVloumeFunc(double* data,double*dataCal,double* dataTra,double* dataMul,int bands,int number,double&det);
//通过FastFinder算法获取影像端元
//算法同样是通过计算最大体积获取端元
long  FastFinder(double* imgBuffer,int xsize,int ysize,int bandSum,int endMemberNum);

//GDAL封装
void  FastFinder(char* pathImg,int endMemberNum,char* pathOut);

/********************************************************************************************************/
/*						                   SVD影像分解算法                                              */
/********************************************************************************************************/
//svd分解后数据空间太大 暂时不去实现
long  SVD(double *dataIn,int xsize,int ysize,double *dataOut,char* pathS,char* pathV,char* pathD);

//GDAL封装影像SVD分解
long  SVDTrans(char* pathIn,int *bandNo,char* pathS,char* pathV,char* pathD);

/********************************************************************************************************/
/*						                   OSP影像投影方法                                              */
/********************************************************************************************************/
//根据投影方法对影像进行投影
//transMat：投影矩阵
//matx maty：投影矩阵的大小
//dataOut：输出矩阵
long  OSP(double* dataIn,int xsize,int ysize,int bands,double* transMat,int matx,int maty,double* dataOut);

//使用正交子空间投影的方法进行光谱解混
//double* dataIn							输入的影像数据
//int int xsize,int ysize,int bands			影像的大小和影像波段数
//double *endMat,int endNumber,int bands	端元光谱，端元数目和端元波段数
//double *dataout							输出结果
long  OSPUnmix(double* dataIn,int xsize,int ysize,int bands,double *endMat,int endNumber,double *dataout);
//GDAL封装的正交子空间投影的光谱解混方法
long  OSPUnmix(const char* pathImgIn,const char* pathImgOut,const char* endFile,int endnumber,int bands);

#endif
