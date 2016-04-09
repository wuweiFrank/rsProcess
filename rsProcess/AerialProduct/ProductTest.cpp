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
	//mQPDProcess.PreProc_GenerateD0Data(pathRaw1, pathRaw1Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	//mQPDProcess.PreProc_GenerateD0Data(pathRaw2, pathRaw2Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	//mQPDProcess.PreProc_GenerateD0Data(pathRaw3, pathRaw3Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	//mQPDProcess.PreProc_GenerateD0Data(pathRaw4, pathRaw4Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	//mQPDProcess.PreProc_GenerateD0Data(pathRaw5, pathRaw5Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
	//mQPDProcess.PreProc_GenerateD0Data(pathRaw6, pathRaw6Out, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount, nFixLines);
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

	char* pathEventOut1 = "E:\\73\\test\\Event_POS\\EVENT_0101_20160229_115734783.txt";
	char* pathEventOut2 = "E:\\73\\test\\Event_POS\\EVENT_0301_20160229_115734783.txt";
	char* pathEventOut3 = "E:\\73\\test\\Event_POS\\EVENT_0401_20160229_115734783.txt";
	char* pathEventOut4 = "E:\\73\\test\\Event_POS\\EVENT_0501_20160229_115734783.txt";
	char* pathEventOut5 = "E:\\73\\test\\Event_POS\\EVENT_0601_20160229_115734783.txt";
	char* pathEventOut6 = "E:\\73\\test\\Event_POS\\EVENT_0701_20160229_115734783.txt";
	char* pathModifyEvent1 = "E:\\73\\test\\Event_POS\\EVENTModify_0101_20160229_115734783.txt";
	char* pathModifyEvent2 = "E:\\73\\test\\Event_POS\\EVENTModify_0301_20160229_115734783.txt";

	char* pathBSQ1 = "E:\\73\\test\\Level0Product\\P0\\P0_0101_20160229_115734783.dat";
	char* pathBSQ2 = "E:\\73\\test\\Level0Product\\P0\\P0_0301_20160229_115734783.dat";
	char* pathBSQ3 = "E:\\73\\test\\Level0Product\\P0\\P0_0401_20160229_115734783.dat";
	char* pathBSQ4 = "E:\\73\\test\\Level0Product\\P0\\P0_0501_20160229_115734783.dat";
	char* pathBSQ5 = "E:\\73\\test\\Level0Product\\P0\\P0_0601_20160229_115734783.dat";
	char* pathBSQ6 = "E:\\73\\test\\Level0Product\\P0\\P0_0701_20160229_115734783.dat";

 	char* pathBSQUseful1 = "E:\\73\\test\\Level0Product\\P1\\P0_0101_20160229_115734783.dat";
	char* pathBSQUseful2 = "E:\\73\\test\\Level0Product\\P1\\P0_0301_20160229_115734783.dat";
	char* pathBSQUseful3 = "E:\\73\\test\\Level0Product\\P1\\P0_0401_20160229_115734783.dat";
	char* pathBSQUseful4 = "E:\\73\\test\\Level0Product\\P1\\P0_0501_20160229_115734783.dat";
	char* pathBSQUseful5 = "E:\\73\\test\\Level0Product\\P1\\P0_0601_20160229_115734783.dat";
	char* pathBSQUseful6 = "E:\\73\\test\\Level0Product\\P1\\P0_0701_20160229_115734783.dat";

	char* pathBSQNonuniform1 = "E:\\73\\test\\Level0Product\\P2\\P0_0101_20160229_115734783.dat";
	char* pathBSQNonuniform2 = "E:\\73\\test\\Level0Product\\P2\\P0_0301_20160229_115734783.dat";
	char* pathBSQNonuniform3 = "E:\\73\\test\\Level0Product\\P2\\P0_0401_20160229_115734783.dat";
	char* pathBSQNonuniform4 = "E:\\73\\test\\Level0Product\\P2\\P0_0501_20160229_115734783.dat";
	char* pathBSQNonuniform5 = "E:\\73\\test\\Level0Product\\P2\\P0_0601_20160229_115734783.dat";
	char* pathBSQNonuniform6 = "E:\\73\\test\\Level0Product\\P2\\P0_0701_20160229_115734783.dat";

	//char* pathBSQNonuniform1 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0101_20160229_115734783.dat";
	//char* pathBSQNonuniform2 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0301_20160229_115734783.dat";
	//char* pathBSQNonuniform3 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0401_20160229_115734783.dat";
	//char* pathBSQNonuniform4 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0501_20160229_115734783.dat";
	//char* pathBSQNonuniform5 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0601_20160229_115734783.dat";
	//char* pathBSQNonuniform6 = "E:\\73\\test\\Level0Product\\P0Noneuniform\\P01_0701_20160229_115734783.dat";

	QPDLevel0Process m_level0Proc;
	//解算EVNET文件
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw1Out, pathEventOut1, 18);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw2Out, pathEventOut2, 18);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw3Out, pathEventOut3, 18);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw4Out, pathEventOut4, 18);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw5Out, pathEventOut5, 18);
	//m_level0Proc.Level0Proc_ExtractEvent(pathRaw6Out, pathEventOut6, 18);
	//m_level0Proc.Level0Proc_ModifySWIREvent(pathEventOut1, pathModifyEvent1, 100592.364000);
	//m_level0Proc.Level0Proc_ModifySWIREvent(pathEventOut2, pathModifyEvent2, 100592.364000);

	//BIL转换为BSQ
	//m_level0Proc.Level0Proc_RawToBSQ(pathRaw1Out, pathBSQ1, true);
	//m_level0Proc.Level0Proc_RawToBSQ(/*pathRaw2Out*/"D:\\D0_0101_20160229_115734783.dat", /*pathBSQ2*/"D:\\P0_0101_20160229_115734783.dat", true);
	//m_level0Proc.Level0Proc_RawToBSQ(pathRaw3Out, pathBSQ3, false);
	//m_level0Proc.Level0Proc_RawToBSQ(pathRaw4Out, pathBSQ4, false);
	//m_level0Proc.Level0Proc_RawToBSQ(pathRaw5Out, pathBSQ5, false);
	//m_level0Proc.Level0Proc_RawToBSQ(pathRaw6Out, pathBSQ6, false);

	//除去无用数据
	//vector<int> bandSWIR;
	//for (int i = 1; i <= 512; ++i)
	//	bandSWIR.push_back(i);
	//m_level0Proc.Level0Proc_GetUsefulData(pathBSQ1, pathBSQUseful1, 8, 0, bandSWIR);
	//m_level0Proc.Level0Proc_GetUsefulData(pathBSQ2, pathBSQUseful2, 8, 0, bandSWIR);
	//vector<int> bandVNirTir;
	//for (int i = 2; i <= 257; ++i)
	//	bandVNirTir.push_back(i);
	//m_level0Proc.Level0Proc_GetUsefulData(pathBSQ3, pathBSQUseful3, 8, 0, bandVNirTir);
	//m_level0Proc.Level0Proc_GetUsefulData(pathBSQ4, pathBSQUseful4, 8, 0, bandVNirTir);
	//m_level0Proc.Level0Proc_GetUsefulData(pathBSQ5, pathBSQUseful5, 8, 0, bandVNirTir);
	//m_level0Proc.Level0Proc_GetUsefulData(pathBSQ6, pathBSQUseful6, 8, 0, bandVNirTir);

	////非均匀性校正
	//m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful1, pathBSQNonuniform1);
	//m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful2, pathBSQNonuniform2);
	//m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful3, pathBSQNonuniform3);
	//m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful4, pathBSQNonuniform4);
	//m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful5, pathBSQNonuniform5);
	//m_level0Proc.Level0Proc_Nonuniform(pathBSQUseful6, pathBSQNonuniform6);
}

//全谱段数据处理1级数据产品生产测试
void QPDLevel1ProcessUnitTestFunc()
{
	char* pathBSQUseful1 = "E:\\73\\test\\Level0Product\\P1\\P0_0101_20160229_115734783.dat";
	char* pathBSQUseful2 = "E:\\73\\test\\Level0Product\\P1\\P0_0301_20160229_115734783.dat";
	char* pathBSQUseful3 = "E:\\73\\test\\Level0Product\\P1\\P0_0401_20160229_115734783.dat";
	char* pathBSQUseful4 = "E:\\73\\test\\Level0Product\\P1\\P0_0501_20160229_115734783.dat";
	char* pathBSQUseful5 = "E:\\73\\test\\Level0Product\\P1\\P0_0601_20160229_115734783.dat";
	char* pathBSQUseful6 = "E:\\73\\test\\Level0Product\\P1\\P0_0701_20160229_115734783.dat";

	char* pathRelativeCalib1 = "E:\\外场定标系数\\RC_0101_20160225.txt";
	char* pathRelativeCalib2 = "E:\\外场定标系数\\RC_0301_20160225.txt";
	char* pathRelativeCalib3 = "E:\\外场定标系数\\RC_0401_20160225.txt";
	char* pathRelativeCalib4 = "E:\\外场定标系数\\RC_0501_20160225.txt";
	char* pathRelativeCalib5 = "E:\\外场定标系数\\RC_0601_20160225.txt";
	char* pathRelativeCalib6 = "E:\\外场定标系数\\RC_0701_20160225.txt";

	char* pathRelativeRad1 = "E:\\73\\test\\Level1Product\\P1_0101_20160229_115734783";
	char* pathRelativeRad2 = "E:\\73\\test\\Level1Product\\P1_0301_20160229_115734783";
	char* pathRelativeRad3 = "E:\\73\\test\\Level1Product\\P1_0401_20160229_115734783";
	char* pathRelativeRad4 = "E:\\73\\test\\Level1Product\\P1_0501_20160229_115734783";
	char* pathRelativeRad5 = "E:\\73\\test\\Level1Product\\P1_0601_20160229_115734783";
	char* pathRelativeRad6 = "E:\\73\\test\\Level1Product\\P1_0701_20160229_115734783";
	char* pahJointRad = "E:\\73\\test\\Level1Product\\P1_06050401_20160229_115734783.dat";
	QPDLevel1Process m_level1Proc;
	//相对辐射校正
	//m_level1Proc.Level1Proc_RadiationRelative(pathBSQUseful1, pathRelativeRad1, pathRelativeCalib1);
	//m_level1Proc.Level1Proc_RadiationRelative(pathBSQUseful2, pathRelativeRad2, pathRelativeCalib2);
	//m_level1Proc.Level1Proc_RadiationRelative(pathBSQUseful3, pathRelativeRad3, pathRelativeCalib3);
	//m_level1Proc.Level1Proc_RadiationRelative(pathBSQUseful4, pathRelativeRad4, pathRelativeCalib4);
	//m_level1Proc.Level1Proc_RadiationRelative(pathBSQUseful5, pathRelativeRad5, pathRelativeCalib5);
	//m_level1Proc.Level1Proc_RadiationRelative(pathBSQUseful6, pathRelativeRad6, pathRelativeCalib6);

	//视场拼接这个匀色似乎也没有什么很好的办法
	//m_level1Proc.Level1Proc_ViewJointTwo( "D:\\SWIR3","D:\\SWIR1", 204, 83, "D:\\Joint.tif");
	m_level1Proc.Level1Proc_ViewJointThree(pathRelativeRad5, pathRelativeRad4, pathRelativeRad3, 124, 103, -4, 2, pahJointRad);
}

//全谱段2级数据产品生产测试
void QPDLevel2ProcessUnitTestFunc()
{
	char* pathSBET = "E:\\73\\auxfile\\20160225_GNSS_SBET.out";

	char* pathRelativeRad1 = "E:\\73\\test\\Level1Product\\P1_0101_20160229_115734783";
	char* pathRelativeRad2 = "E:\\73\\test\\Level1Product\\P1_0301_20160229_115734783";
	char* pathRelativeRad3 = "E:\\73\\test\\Level1Product\\P1_0401_20160229_115734783";
	char* pathRelativeRad4 = "E:\\73\\test\\Level1Product\\P1_0501_20160229_115734783";
	char* pathRelativeRad5 = "E:\\73\\test\\Level1Product\\P1_0601_20160229_115734783";
	char* pathRelativeRad6 = "E:\\73\\test\\Level1Product\\P1_0701_20160229_115734783";
	char* pathRelativeRadJoint = "E:\\73\\test\\Level1Product\\P1_06050401_20160229_115734783.dat";

	char* pathEventOut1 = "E:\\73\\test\\Event_POS\\EVENTModify_0101_20160229_115734783.txt";
	char* pathEventOut2 = "E:\\73\\test\\Event_POS\\EVENTModify_0301_20160229_115734783.txt";
	char* pathEventOut3 = "E:\\73\\test\\Event_POS\\EVENT_0401_20160229_115734783.txt";
	char* pathEventOut4 = "E:\\73\\test\\Event_POS\\EVENT_0501_20160229_115734783.txt";
	char* pathEventOut5 = "E:\\73\\test\\Event_POS\\EVENT_0601_20160229_115734783.txt";
	char* pathEventOut6 = "E:\\73\\test\\Event_POS\\EVENT_0701_20160229_115734783.txt";

	char* pathPOS1 = "E:\\73\\test\\Event_POS\\POS_0101_20160229_115734783.txt";
	char* pathPOS2 = "E:\\73\\test\\Event_POS\\POS_0301_20160229_115734783.txt";
	char* pathPOS3 = "E:\\73\\test\\Event_POS\\POS_0401_20160229_115734783.txt";
	char* pathPOS4 = "E:\\73\\test\\Event_POS\\POS_0501_20160229_115734783.txt";
	char* pathPOS5 = "E:\\73\\test\\Event_POS\\POS_0601_20160229_115734783.txt";
	char* pathPOS6 = "E:\\73\\test\\Event_POS\\POS_0701_20160229_115734783.txt";

	char* pathEO1 = "E:\\73\\test\\Event_POS\\EO_0101_20160229_115734783.txt";
	char* pathEO2 = "E:\\73\\test\\Event_POS\\EO_0101_20160229_115734783.txt";
	char* pathEO3 = "E:\\73\\test\\Event_POS\\EO_0401_20160229_115734783.txt";
	char* pathEO4 = "E:\\73\\test\\Event_POS\\EO_0501_20160229_115734783.txt";
	char* pathEO5 = "E:\\73\\test\\Event_POS\\EO_0601_20160229_115734783.txt";
	char* pathEO6 = "E:\\73\\test\\Event_POS\\EO_0701_20160229_115734783.txt";
	QPDGeoPOSProcess m_QPDPOSProc;

	//解算POS数据
	//m_QPDPOSProc.GeoPOSProc_ExtractSBET(pathSBET, pathEventOut1, pathPOS1, 19);
	//m_QPDPOSProc.GeoPOSProc_ExtractSBET(pathSBET, pathEventOut2, pathPOS2, 19);
	//m_QPDPOSProc.GeoPOSProc_ExtractSBET(pathSBET, pathEventOut3, pathPOS3, 19);
	//m_QPDPOSProc.GeoPOSProc_ExtractSBET(pathSBET, pathEventOut4, pathPOS4, 19);
	//m_QPDPOSProc.GeoPOSProc_ExtractSBET(pathSBET, pathEventOut5, pathPOS5, 19);
	//m_QPDPOSProc.GeoPOSProc_ExtractSBET(pathSBET, pathEventOut6, pathPOS6, 19);

	//解算EO数据
	THREEDPOINT setupVec;setupVec.dX= setupVec.dY=setupVec.dZ=0; float setupAngle[] = { 0,0,0 };
	//m_QPDPOSProc.GeoPOSProc_ExtractEO(pathPOS1, 12806, pathEO1, setupVec, setupAngle, 2);
	//m_QPDPOSProc.GeoPOSProc_ExtractEO(pathPOS2, 12805, pathEO2, setupVec, setupAngle, 2);
	//m_QPDPOSProc.GeoPOSProc_ExtractEO(pathPOS3, 23956, pathEO3, setupVec, setupAngle, 4);
	m_QPDPOSProc.GeoPOSProc_ExtractEO(pathPOS4, 23956, pathEO4, setupVec, setupAngle, 2);
	//m_QPDPOSProc.GeoPOSProc_ExtractEO(pathPOS5, 23958, pathEO5, setupVec, setupAngle, 1);
	//m_QPDPOSProc.GeoPOSProc_ExtractEO(pathPOS6, 0, pathEO6, setupVec, setupAngle, 1);

	char* pathLevel21 = "E:\\73\\test\\Level2Product\\2A\\P2_0101_20160229_115734783.tif";
	char* pathLevel22 = "E:\\73\\test\\Level2Product\\2A\\P2_0301_20160229_115734783.tif";
	char* pathLevel23 = "E:\\73\\test\\Level2Product\\2A\\P2_0401_20160229_115734783.tif";
	char* pathLevel24 = "E:\\73\\test\\Level2Product\\2A\\P2_0501_20160229_115734783.tif";
	char* pathLevel25 = "E:\\73\\test\\Level2Product\\2A\\P2_0601_20160229_115734783.tif";

	char* pathLevelJoint = "E:\\73\\test\\Level2Product\\2A\\P2_06050401_20160229_115734783.tif";
	//char* pathLevel26 = "E:\\73\\test\\P2\\P2_0701_20160229_115734783.tif";
	QPDLevel2Process m_QDPLevel2Proc;
	float fGSDX = 2, fGSDY = 2;
	double fElevation = 0;
	int nEoOffset = 0;
	float fFov = 14, fIFov = 1,fFocalLen = 40;
	bool bDEM = 0,bIGM = 1;
	int nInterval = 1;
	//A级数据产品处理
	//m_QDPLevel2Proc.Level2Proc_Product2A(pathRelativeRad1, pathLevel21, pathEO1, 1.5, 1.5, fElevation, nEoOffset, fFov, fIFov, fFocalLen, false, NULL,false);
	//m_QDPLevel2Proc.Level2Proc_Product2A(pathRelativeRad2, pathLevel22, pathEO2, 1.5, 1.5, fElevation, nEoOffset, fFov, fIFov, fFocalLen, false, NULL, false);
	m_QDPLevel2Proc.Level2Proc_Product2A(pathRelativeRadJoint, pathLevelJoint, pathEO4, 0.75, 0.75, fElevation, nEoOffset, 40, 0.0192, fFocalLen, false, NULL, false);
	//m_QDPLevel2Proc.Level2Proc_Product2A(pathRelativeRad4, pathLevel24, pathEO4, fGSDX, fGSDY, fElevation, nEoOffset, 0.0192, 20, fFocalLen, false, NULL, false);
	//m_QDPLevel2Proc.Level2Proc_Product2A(pathRelativeRad5, pathLevel25, pathEO5, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, false, NULL, true);
	//m_QDPLevel2Proc.Level2Proc_Product2A(pathRelativeRad6, pathLevel26, pathEO6, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, false, NULL, true);

	////B级数据产品处理
	//char* pathDEM = "";
	//m_QDPLevel2Proc.Level2Proc_Product2B(pathBSQNonuniform1, pathLevel21, pathEO1, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, pathDEM,false,  NULL, true);
	//m_QDPLevel2Proc.Level2Proc_Product2B(pathBSQNonuniform2, pathLevel22, pathEO2, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, pathDEM, false, NULL, true);
	//m_QDPLevel2Proc.Level2Proc_Product2B(pathBSQNonuniform3, pathLevel23, pathEO3, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, pathDEM, false, NULL, true);
	//m_QDPLevel2Proc.Level2Proc_Product2B(pathBSQNonuniform4, pathLevel24, pathEO4, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, pathDEM, false, NULL, true);
	//m_QDPLevel2Proc.Level2Proc_Product2B(pathBSQNonuniform5, pathLevel25, pathEO5, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, pathDEM, false, NULL, true);
	//m_QDPLevel2Proc.Level2Proc_Product2B(pathBSQNonuniform6, pathLevel26, pathEO6, fGSDX, fGSDY, fElevation, nEoOffset, fFov, fIFov, fFocalLen, pathDEM, false, NULL, true);

}