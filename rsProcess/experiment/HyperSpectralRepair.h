#pragma once
#include"..\gdal\include\gdal_priv.h"
#include"..\Global.h"
#include"../FrequencyFunc/DCTTrans.h"
#include<ctime>
#include<vector>
#include<algorithm>
#include<numeric>
using namespace std;
#pragma comment(lib,"gdal_i.lib")

//高光谱图像修复代码
class HyperRepairFunc
{
public:
	//影像修复主函数，通过输入待修复的影像，mask影像和待修复的起始和终止波段得到修复结果
	void RepairFunc_SpectralRun(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end);

	//影像修复主函数，通过输入待修复的影像，mask影像和待修复的起始和终止波段得到修复结果
	void RepairFunc_SpatialRun(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end);

	//影像修复主函数，根据空间匹配位置进行光谱修复
	void RepairFunc_SpatialSpectralRun(const char* pathDeImg,const char* pathMaskImg,const char* pathReDst,int st,int end);

	//影像修复主函数，进行边界优先的图像修复
	void RepairFunc_EdgePriorityRun(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end);


	//模拟退化影像
	void RepairFunc_SimuliDegradImg(const char* pathImg, const char* pathDeImg, int st, int end, CPOINT lup, CPOINT rbottom);
	
	//构建纹理光谱
	void RepairFunc_GenerateTexture(const char* pathImg, int rangesize, const char* pathOut);

	//部分影像波段缺失的修复
	void RepairFunc_RepairPartialSpectral(int* dataImg, int* dataMask,int xsize,int ysize,int bands);

	//全部影像波段缺失的修复
	void RepairFunc_RepairTotal(int* dataImg, int* dataMask, int xsize, int ysize, int bands);

	//获取纹理基元
	void RepairFunc_TextureElement(const char* pathEle, int rangesize, int num, const char* pathDist);

	//图像分解分解为高频和低频成分
	void RepairFunc_ImgDecompose(const char* pathImg,const char* pathHyper,const char* pathLow,float threshold);

	//通过图像变换图像将信息集中起来
	void RepairFunc_PCATrans(const char* pathImg, const char* pathPCA, const char* pathParam);

	//对PCA变换后图像进行图像增强使其强度与周围图像一致
	void RepairFunc_PCAIntensity(const char* pathImgPCA,const char* InitialMask,const char* pathPCAIntensity);

private:
	////////////////////////////////光谱匹配的光谱修复方法/////////////////////////////////////////////
	//设置影像修复的起始和终止波段
	void RepairFunc_SetLossBands(int st, int end);

	//缺失数据和完整数据进行匹配并求权重
	//(由于要进行所有波段的去做，所以必须将所有数据读入，内存要求比较大，内存映射？)
	void RepairFunc_MatchLoss(int* dataImg, int* dataMask,int posx,int posy,int xsize, int ysize, int bands, vector<int>&matchPos, vector<float> &matchWeight);

	//然后进行修复
	void RepairFunc_RepairSpectral(int* dataImg, int posx, int posy, int xsize, int ysize,vector<int>matchPos, vector<float> matchWeight);

	/////////////////////////////////空间匹配的纹理修复方法//////////////////////////////////////////////
	//在图像上找到最需要修复的位置
	void RepairFunc_FirstPosition(float* imgData, float* maskRepair, int xsize, int ysize, CPOINT &firstPos);

	//修复影像上的某一个像素,同时更新掩膜区域
	void RepairFunc_ExemplarPos(float* imgData, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos);

	//考虑光谱邻接性的像素匹配修复
	void RepairFunc_ExemplarPosSpectral(float* imgData,float*imgDataPrv1,float* imgDataPrv2, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos);

	//获取单一波段空间匹配最佳匹配点
	void RepairFunc_GetBestMacthPos(float* imgData, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos, CPOINT &bestPos);

	//结合Hellinger距离获取最佳匹配点
	void RepairFunc_GetBestMacthPosWithHellinger(float* imgData, float* maskInitial, float* maskRepair, int xsize, int ysize, CPOINT firstPos, CPOINT &bestPos);

	//检查是否完成初步修复
	bool RepairFunc_ExemplarCheckTerminal(float* maskRepair, int xsize, int ysize);

	//迭代精化修复结果
	void RepairFunc_IterativeRefine(float* imgData, float* maskInitial, int xsize, int ysize);

	//优先边界的修复方法
	//首先获取影像边界 
	void RepairFunc_GetRepairEdge(float* maskRepair,int xsize,int ysize, vector<CPOINT> &edge_list);

	//然后修复影像边界
	void RepairFunc_RepairEdge(float* imgData,float* maskInitial, float* maskRepair, vector<CPOINT> &edge_list,int xsize, int ysize);

	/////////////////////////////////图像分解的影像修复方法///////////////////////////////////////////////




private:
	int m_HyperRepairFunc_st;
	int m_HyperRepairFunc_end;
};

//高光谱修复测试函数
void HyperRepairFuncTest();