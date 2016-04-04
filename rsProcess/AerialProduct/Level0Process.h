#pragma once
#include<vector>
using namespace std;
/*
	0级数据产品处理操作
	1.0级数据转换为BSQ格式数据，原始数据为BIL格式不方便读写，抓换为BSQ后方便处理
	2.0级影像数据非均匀性校正，对于原始影像的非均匀性校正，采用最简单的校正方法（目前）对于不同传感器派生不同的处理方法
	3.由于0级数据可能包含所有信息，需要剔除无用信息只获取有用数据
	4.从原始数据中解算出Event时间数据
*/
class Level0Process
{
public:
	//原始数据转换为BSQ格式数据
	virtual long Level0Proc_RawToBSQ(const char* pathRawBIL, const char* pathBSQ);

	//根据实验室定标数据和暗电流数据进行传感器非均匀性校正
	virtual long Level0Proc_Nonuniform(const char* pathBSQ, const char* pathNonuniform,const char* pathCalibFile= 0, const char* pathDarkFile=0);

	//获取有用的数据，除去两端无用的数据和波段
	virtual long Level0Proc_GetUsefulData(const char* pathBSQ, const char* pathUseful, int leftunuse, int rightunuse, vector<int> usefulBands);

	//解算得到Event文件数据
	virtual long Level0Proc_ExtractEvent(const char* pathRawBIL, const char* pathEvent, const int nEventOffset);
private:
	//获取非均匀性校正参数，通过实验室定标文件和暗电流
	void GetNonuniformParameters(const char* pathCalibFile, const char* pathDarkFile, float* params);
};

class QPDLevel0Process : public Level0Process
{
public:
	//修改短波红外Event信息，由于对于全谱段数据短波Event的百微秒是正确的，整数秒存在异常
	//因此需要对全谱段短波数据进行处理
	long Level0Proc_ModifySWIREvent(const char* pathEvent,const char* pathExEvent, float fTime);
};


//全谱段数据0级数据产品生产测试函数
void QPDLevel0ProcessUnitTestFunc();