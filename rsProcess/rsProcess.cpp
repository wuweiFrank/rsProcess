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
#include"experiment\HyperSpectralRepair.h"
#include"rsImgProcess\KMeans.h"
#include"HyperSpectral\fusionfunc.h"

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
	//m_inPaint.ImageInpaint_Inpaint("D:\\lena.jpg","D:\\test.bmp");
	//UAVGeoCorrection m_uav_correct;
	//m_uav_correct.UAVGeoCorrection_GeoCorrect("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF", "D:", "D:\\my_doc\\2015.12.18岳阳无人机数据\\新建文本文档.txt", NULL, 6, 0, 1, 0.035, 0.2, 0);

	//PhotogrammetryToolsTest();

	//correct_non_nonhomogeneity("C:\\Users\\Public\\Pictures\\Sample Pictures\\hyperspectralRGB.tif","C:\\Users\\Public\\Pictures\\Sample Pictures\\hyperspectralCorrectRGB.tif");
	//
	//CPOINT pnt1, pnt2;
	//pnt1.x = 164; pnt1.y = 21;
	//pnt2.x = 181; pnt2.y = 37;
	//set_mask_region("D:\\lena", "D:\\lenaMask.tif", pnt1, pnt2);
	//set_mask_region("D:\\msk.bmp", "D:\\msk.tif", pnt1, pnt2);

	//set_mask_region("D:\\my_doc\\2016-05-11测试数据\\TestData1", "D:\\test.tif", pnt1, pnt2);
	//vector<Edge_Pixels> edgeinter;
	//get_segment_edge("D:\\ImgCutSimulate1", edgeinter);
	//set_mask_region("D:\\img.tif", "D:\\m.tif", "D:\\imgA.tif");
	//set_mask_region("D:\\img.tif", "D:\\mask.tif", "D:\\imgA.tif");
	//ExemplarBased m_eb;
	//m_eb.ExemplarBased_Inpaint("D:\\lenaMask.tif", "D:\\msk.tif", "D:\\repair.tif");
	//m_eb.ExemplarBased_Inpaint("D:\\my_doc\\2015.10.20数据\\hyper\\hypertestSimulate", "D:\\my_doc\\2015.10.20数据\\hyper\\msk.bmp", "D:\\my_doc\\2015.10.20数据\\hyper\\repairtest.tif");
	//m_eb.ExemplarBased_InpaintTexture("D:\\mf.tif", "D:\\msk.tif", "D:\\Repair.tif");
	//BSCB m_BSCB;
	//m_BSCB.BSCBImageInpaintingProcess("D:\\mf.tif", "D:\\msk.tif", "D:\\Repair.tif");

	//ImgFeaturesTools imgFeature;
	//char* pathImg1 = "D:\\ZY3_TEST\\ZY3_MUX_E126.1_N50.0_20150915.jpg";
	//char* pathImg2 = "D:\\ZY3_TEST\\ZY3_NAD_E126.1_N50.0_20150915.jpg";
	//char* pathPtsDat = "D:\\test.pts";
	//vector<Point2f> pt1, pt2;
	//imgFeature.ImgFeaturesTools_ExtractMatch(pathImg1, pt1, pathImg2, pt2, "AKAZE", "BruteForce-Hamming");
	//imgFeature.ImgFeaturesTools_SaveENVIMatches(pathPtsDat, pathImg1, pathImg2, pt1, pt2);

	//repair("D:\\mf.tif", "D:\\Repair.tif");

	//float data1[64];
	//float data2[64];
	//float data3[64];
	//for (int j = 0; j < 4; ++j)
	//{
	//	for (int i = 0; i < 16; ++i)
	//	{
	//		data1[j * 16 + i] = j + 1;
	//	}
	//}
	//DCT3D(data1, 4, 4, 4, data2);
	//IDCT3D(data2, 4, 4, 4, data3);


	//HyperRepairFuncTest();
	//HyperRepairFunc hyrepair;
	//hyrepair.RepairFunc_GenerateTexture("D:\\lenaFilterH.tif",5,"D:\\lenaHyper.tif");
	//KMeansClassify("D:\\alos.tif", "D:\\classify.tif", 7);
	//ISODataClassify("D:\\alos.tif", "D:\\classify.tif", 3, 20, 1);
	//float data1[16] = {1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4};
	//float data2[16];
	//float data3[16];
	//DCT2D(data1, 4, 4, data2);
	//IDCT2D(data2, 4, 4, data3);

	//DCT2D("D:\\lena", "D:\\lenaDCT.tif",1);
	//IDCT3D("D:\\lenaDCT.tif", "D:\\lenaIDCT.tif");
	//IDCT2D("D:\\lenaDCT.tif", "D:\\lenaIDCT.tif", 1);
	//DCTFliter2D("D:\\lena", "D:\\lenaFilterH.tif", 1, 200);
	//DCTFliter3D("D:\\lenaMuti", "D:\\lenaDCTFliter.tif", 100);
	//HyperRepairFunc m_test;
	//m_test.RepairFunc_TextureElement("D:\\end.txt", 5, 30, "D:\\ele.tif");
	//sparse_GeneralizedInverseLES("D:\\end.txt", "D:\\lenaHyper.tif","D:\\red.tif", 30, 25);
	//endmember_recovery_image("D:\\red.tif", "D:\\end.txt", "D:\\lenaHyperrecovery.tif", 25);

	ImgFeaturesTools featureaTools;
	vector<Point2f> pnt1, pnt2;
	char* path1 = "D:\\my_doc\\2015.09.28无人机影像\\UAV\\IMG_0036.JPG";
	char* path2 = "D:\\my_doc\\2015.09.28无人机影像\\UAV\\IMG_0037.JPG";
	featureaTools.ImgFeaturesTools_ExtractMatch(path1, pnt1, path2, pnt2, "SIFTGPU", "");
	featureaTools.ImgFeaturesTools_SaveENVIMatches("D:\\pts.pts", path1, path2, pnt1, pnt2);
	return 0;
}
