#include"stdafx.h"
#include"ProductTest.h"

//全谱段数据预处理单元测试
void QPDPreProcessUnitTestFunc()
{
	char* pathRaw1 = "E:\\73\\rawdata\\_0101_20160229_115734783.dat";
	char* pathRaw2 = "E:\\73\\rawdata\\_0301_20160229_115734783.dat";
	char* pathRaw3 = "E:\\73\\rawdata\\_0401_20160229_115734783.dat";
	char* pathRaw4 = "E:\\73\\rawdata\\_0501_20160229_115734783.dat";
	char* pathRaw5 = "E:\\73\\rawdata\\_0601_20160229_115734783.dat";
	char* pathRaw6 = "E:\\73\\rawdata\\_0701_20160229_115734783.dat";

	char* pathRaw1Out = "E:\\73\\test\\Catalog\\D0_0101_20160229_115734783.dat";
	char* pathRaw2Out = "E:\\73\\test\\Catalog\\D0_0301_20160229_115734783.dat";
	char* pathRaw3Out = "E:\\73\\test\\Catalog\\D0_0401_20160229_115734783.dat";
	char* pathRaw4Out = "E:\\73\\test\\Catalog\\D0_0501_20160229_115734783.dat";
	char* pathRaw5Out = "E:\\73\\test\\Catalog\\D0_0601_20160229_115734783.dat";
	char* pathRaw6Out = "E:\\73\\test\\Catalog\\D0_0701_20160229_115734783.dat";

	char* pathLeak1Out = "E:\\73\\test\\Catalog\\D0Leak_0101_20160229_115734783.dat";
	char* pathLeak2Out = "E:\\73\\test\\Catalog\\D0Leak_0301_20160229_115734783.dat";
	char* pathLeak3Out = "E:\\73\\test\\Catalog\\D0Leak_0401_20160229_115734783.dat";
	char* pathLeak4Out = "E:\\73\\test\\Catalog\\D0Leak_0501_20160229_115734783.dat";
	char* pathLeak5Out = "E:\\73\\test\\Catalog\\D0Leak_0601_20160229_115734783.dat";
	char* pathLeak6Out = "E:\\73\\test\\Catalog\\D0Leak_0701_20160229_115734783.dat";

	QPDPreProcess mQPDProcess;
	const int nFixLines = 500;
	DINFO mDataHeader;
	vector<short> nLeakFrameType; vector<int> nLeakFrameSize; vector<__int64> nLeakByteCount; int nLeakFrameCount = 0;
	mQPDProcess.PreProc_GenerateD0Data(pathRaw1, pathRaw1Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	mQPDProcess.PrePRoc_WriteLeakInfo(pathLeak1Out, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);

	mQPDProcess.PreProc_GenerateD0Data(pathRaw2, pathRaw2Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	mQPDProcess.PrePRoc_WriteLeakInfo(pathLeak2Out, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);

	mQPDProcess.PreProc_GenerateD0Data(pathRaw3, pathRaw3Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	mQPDProcess.PrePRoc_WriteLeakInfo(pathLeak3Out, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);

	mQPDProcess.PreProc_GenerateD0Data(pathRaw4, pathRaw4Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	mQPDProcess.PrePRoc_WriteLeakInfo(pathLeak4Out, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);

	mQPDProcess.PreProc_GenerateD0Data(pathRaw5, pathRaw5Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	mQPDProcess.PrePRoc_WriteLeakInfo(pathLeak5Out, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);

	mQPDProcess.PreProc_GenerateD0Data(pathRaw6, pathRaw6Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	mQPDProcess.PrePRoc_WriteLeakInfo(pathLeak6Out, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);
}

//全谱段数据处理0级数据产品生产测试
void QPDLevel0ProcessUnitTestFunc()
{
	char* pathRaw1Out = "E:\\73\\test\\Catalog\\D0_0101_20160229_115734783.dat";
	char* pathRaw2Out = "E:\\73\\test\\Catalog\\D0_0301_20160229_115734783.dat";
	char* pathRaw3Out = "E:\\73\\test\\Catalog\\D0_0401_20160229_115734783.dat";
	char* pathRaw4Out = "E:\\73\\test\\Catalog\\D0_0501_20160229_115734783.dat";
	char* pathRaw5Out = "E:\\73\\test\\Catalog\\D0_0601_20160229_115734783.dat";
	char* pathRaw6Out = "E:\\73\\test\\Catalog\\D0_0701_20160229_115734783.dat";

	char* pathEventOut1 = "E:\\73\\test\\Catalog\\EVENT_0101_20160229_115734783.txt";
	char* pathEventOut2 = "E:\\73\\test\\Catalog\\EVENT_0301_20160229_115734783.txt";
	char* pathEventOut3 = "E:\\73\\test\\Catalog\\EVENT_0401_20160229_115734783.txt";
	char* pathEventOut4 = "E:\\73\\test\\Catalog\\EVENT_0501_20160229_115734783.txt";
	char* pathEventOut5 = "E:\\73\\test\\Catalog\\EVENT_0601_20160229_115734783.txt";
	char* pathEventOut6 = "E:\\73\\test\\Catalog\\EVENT_0701_20160229_115734783.txt";
	char* pathModifyEvent1 = "E:\\73\\test\\Catalog\\EVENTModify_0101_20160229_115734783.txt";
	char* pathModifyEvent2 = "E:\\73\\test\\Catalog\\EVENTModify_0101_20160229_115734783.txt";

	char* pathBSQ1 = "E:\\73\\test\\Level0Product\\P0\\P0_0101_20160229_115734783.dat";
	char* pathBSQ2 = "E:\\73\\test\\Level0Product\\P0\\P0_0301_20160229_115734783.dat";
	char* pathBSQ3 = "E:\\73\\test\\Level0Product\\P0\\P0_0401_20160229_115734783.dat";
	char* pathBSQ4 = "E:\\73\\test\\Level0Product\\P0\\P0_0501_20160229_115734783.dat";
	char* pathBSQ5 = "E:\\73\\test\\Level0Product\\P0\\P0_0601_20160229_115734783.dat";
	char* pathBSQ6 = "E:\\73\\test\\Level0Product\\P0\\P0_0701_20160229_115734783.dat";

	char* pathBSQUseful1 = "E:\\73\\test\\Level0Product\\P0\\P01_0101_20160229_115734783.dat";
	char* pathBSQUseful2 = "E:\\73\\test\\Level0Product\\P0\\P01_0301_20160229_115734783.dat";
	char* pathBSQUseful3 = "E:\\73\\test\\Level0Product\\P0\\P01_0401_20160229_115734783.dat";
	char* pathBSQUseful4 = "E:\\73\\test\\Level0Product\\P0\\P01_0501_20160229_115734783.dat";
	char* pathBSQUseful5 = "E:\\73\\test\\Level0Product\\P0\\P01_0601_20160229_115734783.dat";
	char* pathBSQUseful6 = "E:\\73\\test\\Level0Product\\P0\\P01_0701_20160229_115734783.dat";

	char* pathBSQNonuniform1 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0101_20160229_115734783.dat";
	char* pathBSQNonuniform2 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0301_20160229_115734783.dat";
	char* pathBSQNonuniform3 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0401_20160229_115734783.dat";
	char* pathBSQNonuniform4 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0501_20160229_115734783.dat";
	char* pathBSQNonuniform5 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0601_20160229_115734783.dat";
	char* pathBSQNonuniform6 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0701_20160229_115734783.dat";

	//char* pathBSQNonuniform1 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0101_20160229_115734783.dat";
	//char* pathBSQNonuniform2 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0301_20160229_115734783.dat";
	//char* pathBSQNonuniform3 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0401_20160229_115734783.dat";
	//char* pathBSQNonuniform4 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0501_20160229_115734783.dat";
	//char* pathBSQNonuniform5 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0601_20160229_115734783.dat";
	//char* pathBSQNonuniform6 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0701_20160229_115734783.dat";

	QPDLevel0Process m_level0Proc;
	//解算EVNET文件
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw1Out, pathEventOut1, 19);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw2Out, pathEventOut2, 19);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw3Out, pathEventOut3, 19);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw4Out, pathEventOut4, 19);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw5Out, pathEventOut5, 19);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw6Out, pathEventOut6, 19);
	//m_level0Proc.Level0Proc_ModifySWIREvent(pathModifyEvent1);
	//m_level0Proc.Level0Proc_ModifySWIREvent(pathModifyEvent2);

	//BIL转换为BSQ
	m_level0Proc.Level0Proc_RawToBSQ(pathRaw1Out, pathBSQ1);
	m_level0Proc.Level0Proc_RawToBSQ(pathRaw2Out, pathBSQ2);
	m_level0Proc.Level0Proc_RawToBSQ(pathRaw3Out, pathBSQ3);
	m_level0Proc.Level0Proc_RawToBSQ(pathRaw4Out, pathBSQ4);
	m_level0Proc.Level0Proc_RawToBSQ(pathRaw5Out, pathBSQ5);
	m_level0Proc.Level0Proc_RawToBSQ(pathRaw6Out, pathBSQ6);

	//除去无用数据
	vector<int> bandSWIR;
	for (int i = 2; i <= 513; ++i)
		bandSWIR.push_back(i);
	m_level0Proc.Level0Proc_GetUsefulData(pathBSQ1, pathBSQUseful1, 8, 0, bandSWIR);
	m_level0Proc.Level0Proc_GetUsefulData(pathBSQ2, pathBSQUseful2, 8, 0, bandSWIR);
	vector<int> bandVNirTir;
	for (int i = 2; i <= 257; ++i)
		bandVNirTir.push_back(i);
	m_level0Proc.Level0Proc_GetUsefulData(pathBSQ3, pathBSQUseful3, 8, 0, bandVNirTir);
	m_level0Proc.Level0Proc_GetUsefulData(pathBSQ4, pathBSQUseful4, 8, 0, bandVNirTir);
	m_level0Proc.Level0Proc_GetUsefulData(pathBSQ5, pathBSQUseful5, 8, 0, bandVNirTir);
	m_level0Proc.Level0Proc_GetUsefulData(pathBSQ6, pathBSQUseful6, 8, 0, bandVNirTir);

	//非均匀性校正
	m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful1, pathBSQNonuniform1);
	m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful2, pathBSQNonuniform2);
	m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful3, pathBSQNonuniform3);
	m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful4, pathBSQNonuniform4);
	m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful5, pathBSQNonuniform5);
	m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful6, pathBSQNonuniform6);
}