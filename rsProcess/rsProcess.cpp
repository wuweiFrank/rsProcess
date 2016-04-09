// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"AerialProduct\ProductTest.h"
#include"OPENCVTools.h"
#include"matrixOperation.h"

int main()
{
	//QPDPreProcessUnitTestFunc();
	//QPDLevel0ProcessUnitTestFunc();
	//QPDLevel1ProcessUnitTestFunc();
	//QPDLevel2ProcessUnitTestFunc();
	ImgFeaturesTools imgFeatures;
	PhotogrammetryTools imgPhotogrammetry;

	int matchPairs[10000], matchnum;;
	vector<Point2f> pnts1, pnts2, pnts3,pnts4,pnts5,pnts6;
	ImgFeaturesThreadSt st1, st2, st3;
	st1.img1 = "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00005.tif";
	st1.img2 = "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00006.tif";

	st2.img1 = "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00006.tif";
	st2.img2 = "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00007.tif";

	st3.img1 = "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00005.tif";
	st3.img2 = "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00007.tif";
	st1.descriptorMethod = st2.descriptorMethod = st3.descriptorMethod = "AKAZE";
	st1.matchMethod = st2.matchMethod = st3.matchMethod = "BruteForce-Hamming";
	CThreadPool threadPool;
	threadPool.Create();
	threadPool.SetPoolSize(2);
	clock_t stTime=clock(), endTime;
	threadPool.Run(ImgFeaturesTools_ExtractMatchThread, &st1);
	threadPool.Run(ImgFeaturesTools_ExtractMatchThread, &st2);
	threadPool.Run(ImgFeaturesTools_ExtractMatchThread, &st3);
	do
	{
		Sleep(10);
	} while (threadPool.GetWorkingThreadCount()!=0);
	endTime = clock();
	printf("%d\n", endTime - stTime);
	//Mat rot(3,3, CV_64FC1), tvec(3,1, CV_64FC1), rodigues;
	//Mat F1,F2,F3,E;
	stTime = clock();
	imgFeatures.ImgFeaturesTools_ExtractMatch("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00005.tif", pnts1, "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00006.tif", pnts2, "AKAZE", "BruteForce-Hamming");
	imgFeatures.ImgFeaturesTools_ExtractMatch("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00006.tif", pnts3, "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00007.tif", pnts4, "AKAZE", "BruteForce-Hamming");
	imgFeatures.ImgFeaturesTools_ExtractMatch("D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00005.tif", pnts5, "D:\\my_doc\\2015.12.18岳阳无人机数据\\DCIM\\100MSDCF\\warper\\DSC00007.tif", pnts6, "AKAZE", "BruteForce-Hamming");
	endTime = clock();
	printf("%d\n", endTime - stTime);
	//imgPhotogrammetry.PhotogrammetryTools_EssitialMat(pnts1, pnts2, E, 1, Point2d(0, 0));
	
	//imgPhotogrammetry.PhotogrammetryTools_ExtractRT(E, pnts1, pnts2, rot, tvec, 1, Point2d(0, 0));
	//imgPhotogrammetry.PhotogrammetryTools_FundamentalMat(pnts1, pnts2, F1);
	//imgPhotogrammetry.PhotogrammetryTools_FundamentalMat(pnts3, pnts4, F2);
	//imgPhotogrammetry.PhotogrammetryTools_FundamentalMat(pnts5, pnts6, F3);

	//F12 = F1*F2;
	//cout << F3 << endl;
	//cout << F12 << endl;
	//cout << F1 << endl;
	//cout << F2 << endl;
	//Rodrigues(rot, rodigues);
	//double temp3[9];
	//for (int i = 0; i < 3; ++i)
	//{
	//	for (int j = 0; j < 3; ++j)
	//	{
	//		temp3[j * 3 + i] = E.at<double>(i, j);
	//	}
	//}

	//for (int i = 0; i < pnts2.size(); ++i)
	//{
	//	double temp1[3],temp2[3];
	//	temp1[0] = pnts1[i].x;
	//	temp1[1] = pnts1[i].y;
	//	temp1[2] = 1;

	//	temp2[0] = pnts2[i].x;
	//	temp2[1] = pnts2[i].y;
	//	temp2[2] = 1;

	//	double temp4[3];
	//	double temp5[1];
	//	MatrixMuti(temp1, 1, 3, 3, temp3, temp4);
	//	MatrixMuti(temp4, 1, 3, 1, temp2, temp5);
	//	//cout << temp5[0] << endl;
	//}
	//cout << rot << endl;
	//cout << tvec << endl;
	//cout << rodigues << endl;

	return 0;
}

