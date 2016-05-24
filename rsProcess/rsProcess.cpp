// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"lidar\LidarAlgorithm.h"
#include"lidar\LidarReader.h"
#include"RenderProcess.h"
#include"UAV\UAVGeoCorrection.h"
#include"UAV\UAVMosaicFast.h"
#include"AuxiliaryFunction.h"
#include"machineLearning\CVMachineLearningTrain.h"
float main()
{

	//UAVMosaicFast m_mosaic_fast;
	//ImgFeaturesTools m_img_tools;
	//vector<vector<Point2f>> m_pnts;
	//vector<string> pszPath;
	//int width, height;
	////GetImageList("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\test.txt", pszPath);
	////m_mosaic_fast.UAVMosaicFast_Mosaic(pszPath, "D:\\img.tif");
	//float data1[5] = { (float)1,(float)4,(float)6,(float)2,(float)5 };
	//float data2[5] = { (float)2,(float)8,(float)12,(float)4,(float)10 };
	//float tmp=GetCoefficient(data1, data2,5);
	//printf("%lf\n", tmp);
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
	//mnistFile mnistImg;
	//mnistImg.MnistToBmp("D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-images-idx3-ubyte\\t10k-images.idx3-ubyte", "D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-images-idx3-ubyte",
	//	"D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-labels-idx1-ubyte\\t10k-labels.idx1-ubyte", "D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-labels-idx1-ubyte\\link.txt", 100);
	
	CVMachineLearningTrain mlLearn;
	mlLearn.CV_ANN_BP_TrainMnist("D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-images-idx3-ubyte\\t10k-images.idx3-ubyte",
		"D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-images-idx3-ubyte", "test.xml");

	return 0;
}
