#pragma once
#include"..\AuxiliaryFunction.h"
#include"..\Global.h"

class ExemplarBased
{
private:
	//通过Mask数据更新边界
	void ExemplarBased_UpdateEdge(float* mskData,int xsize,int ysize,vector<CPOINT> &edge);

	//通过影像更新Mask数据
	void ExemplarBased_UpdateMask(float* imgData, float *mskData, int xsize, int ysize);

	//通过边界和影像计算最优先修复位置
	void ExemplarBased_GetPriority(float* imgData, int xsize, int ysize, vector<CPOINT> edge,CPOINT &pos);

	//对最优先修复位置进行修复
	void ExemplarBased_PriorityInpaint(CPOINT pos, int regionSize, float* imgData, float* mskData, int xsize, int ysize);

	//考虑纹理信息的影像修复
	void ExemplarBased_ProprityInpaintTexture(CPOINT pos, int regionSize, float* imgData, float* mskData,float* txtData, int xsize, int ysize);

	//计算切线的垂直方向
	void ExemplarBased_Np(vector<CPOINT> edge, CPOINT pos, float* imgData,int xisze,int ysize, double* np);

	//计算等照度线方向
	void ExemplarBased_Ip(float* imgData, CPOINT pos, int xsize, int ysize, double*ip);

	//最佳修补线进行修补
	void ExemplarBased_BestInpaint(float* data1,float* data2,int xsize,int ysize);

public:
	//影像修复
	void ExemplarBased_Inpaint(const char* pathImg,const char* pathMsk,const char* pathDst);

	//考虑纹理信息的影像修复
	void ExemplarBased_InpaintTexture(const char* pathImg, const char* pathMsk,const char* pathTexture,const char* pathDst);
};