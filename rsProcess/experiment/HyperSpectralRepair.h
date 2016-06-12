#pragma once
#include"..\gdal\include\gdal_priv.h"
#include"..\Global.h"
#include<vector>
using namespace std;
#pragma comment(lib,"gdal_i.lib")

//高光谱图像修复代码
class HyperRepairFunc
{
public:
	//影像修复主函数，通过输入待修复的影像，mask影像和待修复的起始和终止波段得到修复结果
	void RepairFunc_Run(const char* pathDeImg, const char* pathMaskImg, const char* pathReDst, int st, int end);

	//模拟退化影像
	void RepairFunc_SimuliDegradImg(const char* pathImg, const char* pathDeImg, int st, int end, CPOINT lup, CPOINT rbottom);
	
	//构建纹理光谱
	void RepairFunc_GenerateTexture(const char* pathImg, int rangesize, const char* pathOut);

	//部分影像波段缺失的修复
	void RepairFunc_RepairPartial(int* dataImg, int* dataMask,int xsize,int ysize,int bands);

	//全部影像波段缺失的修复
	void RepairFunc_RepairTotal(int* dataImg, int* dataMask, int xsize, int ysize, int bands);

	//获取纹理基元
	void RepairFunc_TextureElement(const char* pathEle, int rangesize, int num, const char* pathDist);

private:
	//设置影像修复的起始和终止波段
	void RepairFunc_SetLossBands(int st, int end);

	//缺失数据和完整数据进行匹配并求权重
	//(由于要进行所有波段的去做，所以必须将所有数据读入，内存要求比较大，内存映射？)
	void RepairFunc_MatchLoss(int* dataImg, int* dataMask,int posx,int posy,int xsize, int ysize, int bands, vector<int>&matchPos, vector<float> &matchWeight);

	//然后进行修复
	void RepairFunc_Repair(int* dataImg, int posx, int posy, int xsize, int ysize,vector<int>matchPos, vector<float> matchWeight);

private:
	int m_HyperRepairFunc_st;
	int m_HyperRepairFunc_end;
};

//高光谱修复测试函数
void HyperRepairFuncTest();