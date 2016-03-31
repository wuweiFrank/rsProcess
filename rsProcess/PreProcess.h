#pragma once
#include"Global.h"
#include<vector>
using namespace std;
/*
	数据预处理基类
	1.数据漏行检测
	2.数据头信息获取，这里的数据头是指设定的影像波段数、行列数、起始第一个完整帧的偏移量
	3.数据漏行插值修复函数
	4.生成0级数据，0级数据与原始数据有区别，0级数据是经过数据检查和数据修复的数据
*/
class PreProcess 
{
public:
	virtual long PreProc_GetHeadInfo(FILE *fRAW, DINFO &mDataHeader, vector<short> &nLeakFrameType, vector<int> &nLeakFrameSize, int &nLeakFrameCount) = 0;

	/*
		nLeakFrameType 0为无漏行；1为漏整数帧；2为漏行；3为漏行且漏帧头文件
	*/
	virtual long PreProc_LeakLineCheck(const char *pRAWData, FILE *fRAW, DINFO &mDataHeader, vector<short> &nLeakFrameType, vector<int> &nLeakFrameSize, int &nLeakFrameCount) = 0;

	virtual long PreProc_LeakLineInterpolate(FILE *fRAW, unsigned short *pRepairBuffer, DINFO mDataHeader, vector<short> nLeakFrameType,vector<int> nLeakFrameSize, 
															 int nLoc, __int64 nOffset, unsigned short *pfBuffer, unsigned short *plBuffer, unsigned short *pLeakBuffer)=0;

	/*
		nFixLines为一次读取的帧数，采用较大的读取帧数处理效率比较高,但是要考虑内存的消耗
	*/
	virtual long PreProc_GenerateD0Data(const char *pRAWData, const  char *pData, DINFO mDataHeader, vector<short> nLeakFrameType, vector<int> nLeakFrameSize, int nLeakFrameCount, const int nFixLines)=0;
};

//全谱段数据预处理
class QPDPreProcess: public PreProcess
{
public:
	long PreProc_GetHeadInfo(FILE *fRAW, DINFO &mDataHeader, vector<short> &nLeakFrameType, vector<int> &nLeakFrameSize, int &nLeakFrameCount);

	long PreProc_LeakLineCheck(const char *pRAWData, DINFO &mDataHeader, vector<short> &nLeakFrameType, vector<int> &nLeakFrameSize, int &nLeakFrameCount);

	long PreProc_LeakLineInterpolate(FILE *fRAW, unsigned short *pRepairBuffer, DINFO mDataHeader, vector<short> nLeakFrameType, vector<int> nLeakFrameSize,
											int nLoc, __int64 nOffset, unsigned short *pfBuffer, unsigned short *plBuffer, unsigned short *pLeakBuffer);

	long PreProc_GenerateD0Data(const char *pRAWData, const  char *pData, DINFO mDataHeader, vector<short> nLeakFrameType, vector<int> nLeakFrameSize, int nLeakFrameCount, const int nFixLines);
};