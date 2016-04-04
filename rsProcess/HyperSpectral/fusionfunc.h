#ifndef _IMAGE_FUSION_
#define _IMAGE_FUSION_
#include"..\Global.h"

//模拟实验数据
void simulate_image_data(char* pathEnd,int bands,int endnumbers,int xsize,int ysize,int* oriPos,int* endtype,int oriPosNum,char* pathsimulateImage);
void simulateTest(char* pathEnd,int bands,int endnumbers,char* pathOri,char* pathRatio,char* pathoutS,char* pathRecov);

//获取影像端元光谱
float*  get_endmenber_spectralf(char* pathEnd,int bands,int endnumbers);
double* get_endmenber_spectrald(char* pathEnd,int bands,int endnumbers);

//导出端元光谱到文件中
void   export_endmenber_spectral(char* path,double *enddata,int bands,int endnumbers);
void   export_endmenber_spectral(char* path,float *enddata,int bands,int endnumbers);

//求解出来的丰度进行求和
void reduance_total(char* pathRed,char* pathTotal);

//光谱响应函数
//通过高光谱端元获取多光谱端元
//判断是输入的多光谱波段的每个波段光谱范围还是中心波长
//如果是中心波长，则通过比较波长相近的高光谱波段计算光谱响应
//如果是光谱范围则选择范围内的高光谱波段计算光谱响应
/*
	char* pathHy:输入的高光谱端元
	char* pathM	:输出的多光谱端元
	float* hysp int bandsHy:高光谱波段的中心波长&波段数
	float* msp  int bandm:多光谱波段的中心波长或波长范围&波段数
	int endnumber:	端元数目
	bool range:判断输入的是光谱范围还是中心波长
*/
void spectral_response_endmenber(char* pathHy,char* pathM,float* hysp,int bandsHy,float* msp,int bandm,int endnumber,bool range);
void spectral_response_image(char* pathHyImage,char* pathMImage,float* hysp,int bandsHy,float* msp,int bandm,int endnumber,bool range);

//空间点扩散函数，采用高斯函数进行降采样
void spatial_spread_function(char* pathImg,char* pathSample);

//计算光谱角和绝对值残差
void residual_spAngle(char* pathrecovery,char* pathOri,char* pathresidual);


//基于解混的影像融合 不考虑端元提取方法 直接获取得到的端元

//根据端元光谱进行最小二乘影像解混 
//输出解混丰度和影像
void endmember_unmix_LSE(char* pathEnd,char* pathImg,char* pathRed,int endnumbers);

//通过解混结果进行高斯点扩散函数重采样
void endmember_sample(char* pathRed,char* pathSample);

//计算求解的残差每一个波段的残差和总残差
void endmember_residual(char* pathimg,char* pathend,char* pathred,char* pathres,int endnumbers);

//修正端元光谱
void endmember_modify(char* pathred,char* pathimg,char* pathmodify);

//根据修正的端元对影像进行重建
//输入丰度信息 修正后的端元光谱和恢复影像的波段数
void endmember_recovery_image(char* pathred,char* pathmodend,char* pathout,int hyspectral);

//计算恢复的影像和原始影像之间的残差和光谱角
//第一个波段为计算的光谱角 第二个波段为计算的影像值的差值
void endmember_residual_spAngle(char* pathrecovery,char* pathOri,char* pathresidual);


//直接迭代求解影像的值 根据成像原理
//效率太低 没有合适的衡量手段 不太适合
void get_image_iterator(char* pathHy,char* pathMs,char* pathout);

//获取稀疏组合
void sparse_represent_combine(int numsparse,int selsparse,int endnumber,int* selindex);

//选取每个像素最合适的稀疏组合
//输入为：端元光谱 象元光谱 稀疏组合 稀疏组合数目 选取的稀疏表示个数 波段数 端元数
//输出为：求解的结果 各个成分的丰度
//float* sparse_represent_fitness(float* endmemberdata,double* pixeldata,int* spareIdx,
//											int numspare,int selsparse,int bands,int endnum);

//稀疏最小二乘求解
void sparse_represent_LES(char* pathEnd,char* pathImg,char* pathRed,int endnumbers,int sparsesel);

//端元光谱在空间上进行重采样
void sparse_represent_sample(char* pathred,char* pathredSmp);

//根据解混的丰度修正端元光谱
void sparse_represent_modify(char* pathred,char* pathimg,char* pathmodify);

//通过丰度反解影像
void sparse_represent_recovery(char* pathred,char* pathEnd,char* pathout,int recbands);

//计算光谱角和光谱残差
void sparse_represent_residual_spAngle(char* pathrecovery,char* pathOri,char* pathresidual);


//基于投影变换的影像融合方法
//基于PCA  SVD方法的影像融合
//对高光谱影像重采样 使其分辨率与高分影像一致
void projection_image_sample(char* pathInHy,char* pathInMs,char* pathSample);

//对高分和高光谱影像进行投影变换
void projection_proj_trans(char* pathhyin,char* pathhyout,char* pathmsin,char* pathmsout,char* pathmathy,char* pathmatms);

//投影反变换 
void projection_fusion_trans(char* pathhyin,char* pathmsin,char* pathmathy,char* pathmatms,char* pathout,int numcomponent=1);

//小波变换结合PCA融合
void projection_fusion_wavelet_forward(char* pathHyIn,char* pathMsIn,char* pathHywave1,char* pathHywave2,
											char* pathMswave1,char* pathMswane2,char* pathFusion,int bands=1);

//小波变换结合PCA融合
void projection_fusion_wavelet_inverse(char* pathwavefusion,char* pathHy,char* pathmat,char* pathOut);


//小波变换的影像融合 
//光谱分组
void wavelet_get_spectral_group(float* hywavelen,int hynum,float* mswavelen,int msnum,int *group,bool range);

//光谱重采样
void wavelet_image_sample(char* pathInHy,char* pathInMs,char* pathOut);

//影像小波变换
void wavelet_to_image(char* pathHy,char* pathHyW1,char* pathHyW2,char* pathMs,char* pathMsW1,char* pathMsW2);

//小波变换的影像融合
void wavelet_fusion(char* pathHyW1,char* pathHyW2,char* pathMsW2,char* pathRecov,int* group);

#endif