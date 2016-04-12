// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"AerialProduct\ProductTest.h"
#include"UAV\UAVUnityTest.h"
#include"lidar\LidarReader.h"

float main()
{
	//QPDPreProcessUnitTestFunc();
	//QPDLevel0ProcessUnitTestFunc();
	//QPDLevel1ProcessUnitTestFunc();
	//QPDLevel2ProcessUnitTestFunc();
	//UAVGeoCorrectionTest();
	LASLidarReader m_lasReader;
	GDALProcessBase* m_progress=new GDALConsoleProcess();
	m_lasReader.LidarReader_SetProgress(m_progress);
	m_lasReader.LidarReader_Open("D:\\las文件\\1.las");
	m_lasReader.LidarReader_Read(true);
	m_lasReader.LidarReader_Write("D:\\las文件\\100.las");

	return 0;
}

