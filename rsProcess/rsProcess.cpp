// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"lidar\LidarAlgorithm.h"
#include"lidar\LidarReader.h"
#include"RenderProcess.h"
float main()
{

	LidarReader *reader = new LASLidarReader();
	reader->LidarReader_Open("D:\\las文件\\cc_000341.las");
	reader->LidarReader_Read(true, 10);
	//LidarTinCreateLocal(reader->m_lasDataset);
	GDALTriangulation *lasTriangle = NULL;
	lasTriangle=LidarTinCreateGlobal(reader->m_lasDataset);
	LidarTriangleRenderGlobal(&reader->m_lasDataset, lasTriangle);
	//GDALTriangulationFree(lasTriangle);
	//Mat img = imread("C:\\Users\\Public\\Pictures\\Sample Pictures\\lena.jpg");
	//imshow("img", img);
	//waitKey(0);
	//return 0;
	//QPDPreProcessUnitTestFunc();
	//QPDLevel0ProcessUnitTestFunc();
	//QPDLevel1ProcessUnitTestFunc();
	//QPDLevel2ProcessUnitTestFunc();
	
	//UAVGeoCorrectionTest();

	//ImageSegmentTools tools;
	//tools.ImgSegmentTools_WaterShed("C:\\Users\\Public\\Pictures\\Sample Pictures\\repairTest.bmp", "D:\\watershed.bmp");
	//tools.ImgSegmentTools_Canny("C:\\Users\\Public\\Pictures\\Sample Pictures\\badimg.bmp", "D:\\cannyedge.bmp");

	//BezierCurve curve;
	//curve.BezierCurve_BezierDraw();
	
	//experiment_process();

	//ImageInpaint m_inPaint;
	//m_inPaint.ImageInpaint_Inpaint("C:\\Users\\Public\\Pictures\\Sample Pictures\\lena.jpg","test.jpg");

	//UAVGeoCorrectionTest();

	//PhotogrammetryToolsTest();

	return 0;
}
