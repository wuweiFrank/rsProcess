// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"AerialProduct\ProductTest.h"
#include"UAV\UAVUnityTest.h"
#include"lidar\LidarReader.h"
#include"OPENCVTools.h"

float main()
{
	//QPDPreProcessUnitTestFunc();
	//QPDLevel0ProcessUnitTestFunc();
	//QPDLevel1ProcessUnitTestFunc();
	//QPDLevel2ProcessUnitTestFunc();
	
	//UAVGeoCorrectionTest();

	//ImageSegmentTools tools;
	//tools.ImgSegmentTools_WaterShed("C:\\Users\\Public\\Pictures\\Sample Pictures\\repairTest.bmp", "D:\\watershed.bmp");
	//tools.ImgSegmentTools_Canny("C:\\Users\\Public\\Pictures\\Sample Pictures\\badimg.bmp", "D:\\cannyedge.bmp");

	BezierCurve curve;
	curve.BezierCurve_BezierDraw();

	return 0;
}

