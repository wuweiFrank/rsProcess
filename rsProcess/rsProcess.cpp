// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"lidar\LidarAlgorithm.h"
#include"lidar\LidarReader.h"
#include"RenderProcess.h"
#include"UAV\UAVGeoCorrection.h"
#include"UAV\UAVMosaicFast.h"
#include"AuxiliaryFunction.h"
#include"experiment\HyperRepair.h"
#include"experiment\BSCB.h"
#include"experiment\ExemplarBased.h"
#include"FrequencyFunc\DCTTrans.h"

float main()
{

	//UAVMosaicFast m_mosaic_fast;
	//ImgFeaturesTools m_img_tools;
	//vector<vector<Point2f>> m_pnts;
	//vector<string> pszPath;
	//int width, height;
	//GetImageList("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\test.txt", pszPath);
	//m_mosaic_fast.UAVMosaicFast_Mosaic(pszPath, "D:\\img.tif");

	//m_mosaic_fast.UAVMosaicFast_AffineTrans(pszPath);
	//m_mosaic_fast.UAVMosaicFast_GetMosaicRange(pszPath,width, height);
	//m_mosaic_fast.UAVMosaicFast_AffineTrans(pszPath);
	//
	//LidarReader *reader = new LASLidarReader();
	//reader->LidarReader_Open("D:\\las文件\\cc_000341.las");
	//reader->LidarReader_Read(true, 10);
	////LidarTinCreateLocal(reader->m_lasDataset);
	//GDALTriangulation *lasTriangle = NULL;
	//lasTriangle=LidarTinCreateGlobal(reader->m_lasDataset);
	//LidarTriangleRenderGlobal(&reader->m_lasDataset, lasTriangle);

	//QPDPreProcessUnitTestFunc();
	//QPDLevel0ProcessUnitTestFunc();
	//QPDLevel1ProcessUnitTestFunc();
	//QPDLevel2ProcessUnitTestFunc();
	//
	//UAVGeoCorrectionTest();

	//ImageSegmentTools tools;
	//tools.ImgSegmentTools_WaterShed("C:\\Users\\Public\\Pictures\\Sample Pictures\\repairTest.bmp", "D:\\watershed.bmp");
	//tools.ImgSegmentTools_Canny("C:\\Users\\Public\\Pictures\\Sample Pictures\\badimg.bmp", "D:\\cannyedge.bmp");

	//BezierCurve curve;
	//curve.BezierCurve_BezierDraw();
	//
	//experiment_process();

	//ImageInpaint m_inPaint;
	//m_inPaint.ImageInpaint_Inpaint("D:\\1.bmp","test.jpg");
	//UAVGeoCorrection m_uav_correct;
	//m_uav_correct.UAVGeoCorrection_GeoCorrect("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF", "D:", "D:\\my_doc\\2015.12.18岳阳无人机数据\\新建文本文档.txt", NULL, 6, 0, 1, 0.035, 0.2, 0);

	//PhotogrammetryToolsTest();

	//correct_non_nonhomogeneity("C:\\Users\\Public\\Pictures\\Sample Pictures\\hyperspectralRGB.tif","C:\\Users\\Public\\Pictures\\Sample Pictures\\hyperspectralCorrectRGB.tif");
	
	CPOINT pnt1, pnt2;
	pnt1.x = 160; pnt1.y = 170;
	pnt2.x = 190; pnt2.y = 200;
	//set_mask_region("D:\\can_tmr.bmp", "D:\\mf.tif", pnt1, pnt2);
	//set_mask_region("D:\\msk.bmp", "D:\\msk.tif", pnt1, pnt2);
	//set_mask_region("D:\\my_doc\\2016-05-11测试数据\\TestData1", "D:\\test.tif", pnt1, pnt2);
	//vector<Edge_Pixels> edgeinter;
	//get_segment_edge("D:\\ImgCutSimulate1", edgeinter);
	set_mask_region("D:\\img.tif", "D:\\m.tif", "D:\\imgA.tif");

	ExemplarBased m_eb;
	m_eb.ExemplarBased_Inpaint("D:\\imgA.tif", "D:\\m.tif", "D:\\Repair.tif");
	//m_eb.ExemplarBased_InpaintTexture("D:\\mf.tif", "D:\\msk.tif", "D:\\Repair.tif");
	//BSCB m_BSCB;
	//m_BSCB.BSCBImageInpaintingProcess("D:\\imgA.tif", "D:\\m.tif", "D:\\Repair.tif");

	//ImgFeaturesTools imgFeature;
	//char* pathImg1 = "D:\\ZY3_TEST\\ZY3_MUX_E126.1_N50.0_20150915.jpg";
	//char* pathImg2 = "D:\\ZY3_TEST\\ZY3_NAD_E126.1_N50.0_20150915.jpg";
	//char* pathPtsDat = "D:\\test.pts";
	//vector<Point2f> pt1, pt2;
	//imgFeature.ImgFeaturesTools_ExtractMatch(pathImg1, pt1, pathImg2, pt2, "AKAZE", "BruteForce-Hamming");
	//imgFeature.ImgFeaturesTools_SaveENVIMatches(pathPtsDat, pathImg1, pathImg2, pt1, pt2);

	//repair("D:\\mf.tif", "D:\\Repair.tif");

	float data1[8] = { 1,1,1,1,1,1,1,1 };
	float data2[8];
	float data3[8];
	//DCT1D(data1, 8, data2);
	//IDCT1D(data2, 8, data3);
	return 0;
}
