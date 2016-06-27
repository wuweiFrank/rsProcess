// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"machineLearning\CVMachineLearning.h"
#include"opencv\opencv2\imgproc.hpp"
#include"UAV\UAVGeoCorrection.h"
#include<iostream>
#include<string>
using namespace std;
int main()
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
	//HyperRepairFunc m_Repair;
	//m_Repair.RepairFunc_GenerateTexture("D:\\my_doc\\2015.10.20数据\\hyper\\hypertest",12,"D:\\my_doc\\2015.10.20数据\\hyper\\hypertestGenTexture.tif");
	//m_Repair.RepairFunc_TextureElement("D:\\my_doc\\2015.10.20数据\\hyper\\speclib.txt",12,100, "D:\\my_doc\\2015.10.20数据\\hyper\\hypertestGenTextureSpeclib.tif");
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

	
	//ImgFeaturesTools m_tools;
	//vector<Point2f> pnt1, pnt2;
	//m_tools.ImgFeaturesTools_ExtractMatch("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\DSC00004.JPG", pnt1, "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\DSC00005.JPG", pnt2, "SIFTGPU", "");
	//m_tools.ImgFeaturesTools_SaveENVIMatches("D:\\test.pts", "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\DSC00004.JPG", "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\DSC00005.JPG", pnt1, pnt2);

	//char* pathEnd = "D:\\my_doc\\2015.10.20数据\\hyper\\speclib.txt";
	//char* pathImg = "D:\\my_doc\\2015.10.20数据\\hyper\\hypertestGenTexture.tif";
	//char* pathRed = "D:\\my_doc\\2015.10.20数据\\hyper\\sparseAbundacne.tif";
	//sparse_unmix(pathEnd, pathImg, pathRed, 100, 144);
	CVMachineLearningTrain mlt;
	char* pathMnist = "D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\train-images-idx3-ubyte\\train-images.idx3-ubyte";
	char* pathLabel = "D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\train-labels-idx1-ubyte\\train-labels.idx1-ubyte";
	char* pathBPNet = "D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\LR.xml";
	//mlt.CV_ANN_BP_Train(pathMnist, pathLabel, pathBPNet, DATASET_MNIST);
	//mlt.CV_SVM_Train(pathMnist, 0.5, pathLabel, pathBPNet, DATASET_MNIST);
	//mlt.CV_LogisticRegression_Train(pathMnist, pathLabel, pathBPNet, DATASET_MNIST);
	char* pathPredict = "D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-images-idx3-ubyte\\t10k-images.idx3-ubyte";
	char* pathPredictLabel = "D:\\my_doc\\2015.10.20数据\\rsProcess-演示\\mnist\\t10k-labels-idx1-ubyte\\t10k-labels.idx1-ubyte";
	CVMachineLearningPredict mlp;
	//mlp.CV_SVM_PredictMnist(pathPredict, pathBPNet, pathPredictLabel);
	//mlp.CV_ANN_BP_PredictMnist(pathPredict, pathBPNet, pathPredictLabel);
	//mlp.CV_LogisticRegression_PredictMnist(pathPredict, pathBPNet, pathPredictLabel);

	//UAVGeoCorrection m_geo;
	//char* pathSrc = "D:\\第二组(无人机像对)\\第二组(无人机像对)\\IMG_0183.jpg";
	//char* pathDst = "D:\\第二组(无人机像对)\\第二组(无人机像对)\\2.tif";
	//
	//	
	//double rotParam[3] = { 0.054792    ,    0.084261    ,    0.003354 };
	//double transParam[3] = { 241693.5550  ,   3362627.8108      ,   551.2075 };
	//double iMat[4] = { 1,0,0,1 };
	//DPOINT center; center.dX = 1872.00; center.dY = 2808.00;
	//m_geo.UAVGeoCorrection_GeoCorrect(pathSrc, pathDst, rotParam,transParam, iMat, center, 35.4904, 0.0064,49,0);
	//BPNetMnist bpMnist;
	//bpMnist.bp_train(pathMnist, pathLabel);
	//bpMnist.bp_load(pathBPNet);
	//bpMnist.bp_predict(pathMnist, pathLabel);

	return 0;
}
//static void showImage(const Mat &data, int columns, const String &name)
//{
//	Mat bigImage;
//	for (int i = 0; i < data.rows; ++i)
//	{
//		bigImage.push_back(data.row(i).reshape(0, columns));
//	}
//	imshow(name, bigImage.t());
//}
//
//static float calculateAccuracyPercent(const Mat &original, const Mat &predicted)
//{
//	return 100 * (float)countNonZero(original == predicted) / predicted.rows;
//}
//
//int main()
//{
//	const String filename = "D:\\cache\\opencv\\sources\\samples\\data\\data01.xml";
//	cout << "**********************************************************************" << endl;
//	cout << filename
//		<< " contains digits 0 and 1 of 20 samples each, collected on an Android device" << endl;
//	cout << "Each of the collected images are of size 28 x 28 re-arranged to 1 x 784 matrix"
//		<< endl;
//	cout << "**********************************************************************" << endl;
//
//	Mat data, labels;
//	{
//		cout << "loading the dataset...";
//		FileStorage f;
//		if (f.open(filename, FileStorage::READ))
//		{
//			f["datamat"] >> data;
//			f["labelsmat"] >> labels;
//			f.release();
//		}
//		else
//		{
//			cerr << "file can not be opened: " << filename << endl;
//			return 1;
//		}
//		data.convertTo(data, CV_32F);
//		labels.convertTo(labels, CV_32F);
//		cout << "read " << data.rows << " rows of data" << endl;
//	}
//
//	Mat data_train, data_test;
//	Mat labels_train, labels_test;
//	for (int i = 0; i < data.rows; i++)
//	{
//		if (i % 2 == 0)
//		{
//			data_train.push_back(data.row(i));
//			labels_train.push_back(labels.row(i));
//		}
//		else
//		{
//			data_test.push_back(data.row(i));
//			labels_test.push_back(labels.row(i));
//		}
//	}
//	cout << "training/testing samples count: " << data_train.rows << "/" << data_test.rows << endl;
//
//	// display sample image
//	showImage(data_train, 28, "train data");
//	showImage(data_test, 28, "test data");
//
//	// simple case with batch gradient
//	cout << "training...";
//	//! [init]
//	Ptr<LogisticRegression> lr1 = LogisticRegression::create();
//	lr1->setLearningRate(0.001);
//	lr1->setIterations(10);
//	lr1->setRegularization(LogisticRegression::REG_L2);
//	lr1->setTrainMethod(LogisticRegression::BATCH);
//	lr1->setMiniBatchSize(1);
//	//! [init]
//	//cout << labels_train << endl;
//
//	lr1->train(data_train, ROW_SAMPLE, labels_train);
//	cout << "done!" << endl;
//
//	cout << "predicting...";
//	Mat responses;
//	lr1->predict(data_test, responses);
//	cout << "done!" << endl;
//
//	// show prediction report
//	cout << "original vs predicted:" << endl;
//	labels_test.convertTo(labels_test, CV_32S);
//	cout << labels_test.t() << endl;
//	cout << responses.t() << endl;
//	cout << "accuracy: " << calculateAccuracyPercent(labels_test, responses) << "%" << endl;
//
//	// save the classfier
//	const String saveFilename = "NewLR_Trained.xml";
//	cout << "saving the classifier to " << saveFilename << endl;
//	lr1->save(saveFilename);
//
//	// load the classifier onto new object
//	cout << "loading a new classifier from " << saveFilename << endl;
//	Ptr<LogisticRegression> lr2 = StatModel::load<LogisticRegression>(saveFilename);
//
//	// predict using loaded classifier
//	cout << "predicting the dataset using the loaded classfier...";
//	Mat responses2;
//	lr2->predict(data_test, responses2);
//	cout << "done!" << endl;
//
//	// calculate accuracy
//	cout << labels_test.t() << endl;
//	cout << responses2.t() << endl;
//	cout << "accuracy: " << calculateAccuracyPercent(labels_test, responses2) << "%" << endl;
//
//	waitKey(0);
//	return 0;
//}
