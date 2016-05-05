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
	//m_inPaint.ImageInpaint_Inpaint("C:\\Users\\Public\\Pictures\\Sample Pictures\\lena.jpg","test.jpg");
	//UAVGeoCorrection m_uav_correct;
	//m_uav_correct.UAVGeoCorrection_GeoCorrect("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF", "D:", "D:\\my_doc\\2015.12.18岳阳无人机数据\\新建文本文档.txt", NULL, 6, 0, 1, 0.035, 0.2, 0);

	//PhotogrammetryToolsTest();

	//correct_non_nonhomogeneity("C:\\Users\\Public\\Pictures\\Sample Pictures\\hyperspectralRGB.tif","C:\\Users\\Public\\Pictures\\Sample Pictures\\hyperspectralCorrectRGB.tif");
	//CPOINT pnt1, pnt2;
	//pnt1.x = 0; pnt1.y = 256;
	//pnt2.x = 1024; pnt2.y = 296;
	//set_mask_region("D:\\imgCut", "D:\\ImgCutSimulate", pnt1, pnt2);

	//vector<Edge_Pixels> edgeinter;
	//get_segment_edge("D:\\ImgCutSimulate1", edgeinter);

	ImgFeaturesTools features;
	vector<Point2f> pnt1, pnt2;
	features.ImgFeaturesTools_ExtractMatch("D:\\my_doc\\2015.12.18岳阳无人机数据\\GeoCorrect\\DSC00006.tif", pnt1, "D:\\my_doc\\2015.12.18岳阳无人机数据\\GeoCorrect\\DSC00007.tif", pnt2, "AKAZE", "BruteForce-Hamming");
	features.ImgFeaturesTools_SaveENVIMatches("D:\\test.pts", "D:\\my_doc\\2015.12.18岳阳无人机数据\\GeoCorrect\\DSC00006.tif", "D:\\my_doc\\2015.12.18岳阳无人机数据\\GeoCorrect\\DSC00007.tif", pnt1, pnt2);

	return 0;
}
