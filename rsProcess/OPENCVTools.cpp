#include"OPENCVTools.h"
#include"AuxiliaryFunction.h"
#include<fstream>
#include<iomanip>
#include<iostream>
using namespace std;

#pragma warning(disable : 4996)

DWORD WINAPI ImgFeaturesTools_ExtractMatchThread(LPVOID lpParameters)
{
	UserPoolData* userPoolData = (UserPoolData*)lpParameters;
	ImgFeaturesThreadSt* param = (ImgFeaturesThreadSt*)userPoolData->pData;

	Mat img1 = imread(param->img1, IMREAD_LOAD_GDAL);
	Mat img2 = imread(param->img2, IMREAD_LOAD_GDAL);
	if (img1.rows*img1.cols <= 0)
	{
		printf("Image %s is empty or cannot be found\n", param->img1);
		return(0);
	}
	if (img2.rows*img2.cols <= 0)
	{
		printf("Image %s is empty or cannot be found\n", param->img2);
		return(0);
	}

	vector<double> desMethCmp;
	Ptr<Feature2D> bFeatures;

	Ptr<DescriptorMatcher> descriptorMatcher;
	// Match between img1 and img2
	vector<DMatch> matches;
	// keypoint  for img1 and img2
	vector<KeyPoint> keyImg1, keyImg2;
	// Descriptor for img1 and img2
	Mat descImg1, descImg2;
	if (param->descriptorMethod == "AKAZE-DESCRIPTOR_KAZE_UPRIGHT") {
		bFeatures = AKAZE::create(AKAZE::DESCRIPTOR_KAZE_UPRIGHT);
	}
	if (param->descriptorMethod == "AKAZE") {
		bFeatures = AKAZE::create();
	}
	if (param->descriptorMethod == "ORB") {
		bFeatures = ORB::create();
	}
	else if (param->descriptorMethod == "BRISK") {
		bFeatures = BRISK::create();
	}

	try
	{
		// We can detect keypoint with detect method
		//bFeatures->detect(img1, keyImg1, Mat());
		// and compute their descriptors with method  compute
		//bFeatures->compute(img1, keyImg1, descImg1);
		// or detect and compute descriptors in one step
		bFeatures->detectAndCompute(img1, Mat(), keyImg1, descImg1, false);
		bFeatures->detectAndCompute(img2, Mat(), keyImg2, descImg2, false);
		descriptorMatcher = DescriptorMatcher::create(param->matchMethod);
		if ((param->matchMethod == "BruteForce-Hamming" || param->matchMethod == "BruteForce-Hamming(2)") && (bFeatures->descriptorType() == CV_32F || bFeatures->defaultNorm() <= NORM_L2SQR))
		{
			printf("**************************************************************************\n");
			printf("It's strange. You should use Hamming distance only for a binary descriptor\n");
			printf("**************************************************************************\n");
		}
		if ((param->matchMethod == "BruteForce" || param->matchMethod == "BruteForce-L1") && (bFeatures->defaultNorm() >= NORM_HAMMING))
		{
			printf("**************************************************************************\n");
			printf("It's strange. You shouldn't use L1 or L2 distance for a binary descriptor\n");
			printf("**************************************************************************\n");
		}

		try
		{
			descriptorMatcher->match(descImg1, descImg2, matches, Mat());
			// Keep best matches only to have a nice drawing.
			// We sort distance between descriptor matches
			Mat index;
			int nbMatch = int(matches.size());
			Mat tab(nbMatch, 1, CV_32F);
			for (int i = 0; i<nbMatch; i++)
			{
				tab.at<float>(i, 0) = matches[i].distance;
			}
			sortIdx(tab, index, SORT_EVERY_COLUMN + SORT_ASCENDING);
			vector<DMatch> bestMatches;
			vector<DMatch>::iterator it;
			int num = nbMatch > 110 ? 110 : nbMatch;
			for (int i = 0; i<num; i++)
			{
				bestMatches.push_back(matches[index.at<int>(i, 0)]);
			}

			// Saved result could be wrong due to bug 4308
			// Use to compute distance between keyPoint matches and to evaluate match algorithm
			double cumSumDist2 = 0;
			for (it = bestMatches.begin(); it != bestMatches.end(); it++)
			{
				Point2f pt1, pt2;
				pt1 = keyImg1[it->queryIdx].pt;
				pt2 = keyImg2[it->trainIdx].pt;
				param->pts1.push_back(pt1);
				param->pts2.push_back(pt2);

				Point2d p = keyImg1[it->queryIdx].pt - keyImg2[it->trainIdx].pt;
				cumSumDist2 = p.x*p.x + p.y*p.y;
			}
			//ImgFeaturesTools_MatchOptimize(pts1, pts2);
			//===================================================================================
			if (param->pts1.size() < 3 || param->pts2.size() < 3)
			{
				printf("match points too little\n");
				exit(-1);
			}
			Mat homography = findHomography(param->pts1, param->pts2, CV_RANSAC);
			vector<Point2f> pts2Homography(param->pts1.size());
			perspectiveTransform(param->pts1, pts2Homography, homography);
			float *err = NULL;
			try
			{
				err = new float[param->pts1.size()];
			}
			catch (bad_alloc &e)
			{
				printf(e.what());
			}

			//获取误差
			for (size_t i = 0; i < param->pts1.size(); i++)
				err[i] = sqrt((pts2Homography[i].x - param->pts2[i].x)*(pts2Homography[i].x - param->pts2[i].x) + (pts2Homography[i].y - param->pts2[i].y)*(pts2Homography[i].y - param->pts2[i].y));

			//均值和标准差
			float fAverage = 0, fDeviation = 0;
			GetAveDev(err, 1, param->pts1.size(), 0, fAverage, fDeviation);

			vector<Point2f> ptsOpt1, ptsOpt2;
			for (size_t i = 0; i < param->pts1.size(); i++)
			{
				if (fabs(err[i] - fAverage) < 2 * fDeviation)
				{
					ptsOpt1.push_back(param->pts1[i]);
					ptsOpt2.push_back(param->pts2[i]);
				}
			}

			param->pts1.clear();
			param->pts2.clear();
			param->pts1.insert(param->pts1.begin(), ptsOpt1.begin(), ptsOpt1.end());
			param->pts2.insert(param->pts2.begin(), ptsOpt2.begin(), ptsOpt2.end());
			//===================================================================================
			desMethCmp.push_back(cumSumDist2);
		}
		catch (Exception& e)
		{
			printf("%s\n", e.msg.c_str());
			printf("Cumulative distance cannot be computed.\n");
			desMethCmp.push_back(-1);
		}
	}
	catch (Exception& e)
	{
		printf("Feature : %s\n", param->descriptorMethod);
		printf(e.msg.c_str());
	}

}

long ImgFeaturesTools::ImgFeaturesTools_SaveENVIMatches(const char* pathDst, const char* pathSrc1, const char* pathSrc2, vector<Point2f> pts1, vector<Point2f> pts2)
{
	ofstream file1;
	long IError = 0;

	file1.open(pathDst, ios::out);
	if (!file1.is_open())
		return -1;
	file1 << "; ENVI Image to Image GCP File" << endl;
	file1 << "; base file: " << pathSrc1 << endl;
	file1 << "; warp file: " << pathSrc2 << endl;
	file1 << "; Base Image (x,y), Warp Image (x,y)" << endl;
	file1 << ";" << endl;
	for (int s = 0; s < pts1.size(); s++)
	{
		file1 << setw(10) << pts1[s].x << setw(10) << pts1[s].y
			<< setw(10) << pts2[s].x << setw(10) << pts2[s].y << endl;
	}
	if (file1.is_open())
		file1.close();
	return 0;
}

long ImgFeaturesTools::ImgFeaturesTools_ReadENVIMatches(const char* pathPoints, vector<Point2f> &pts1, vector<Point2f> &pts2)
{
	ifstream ifs(pathPoints, ios_base::in);
	if (!ifs.is_open())
		return -1;
	char chrTemp[2048];
	for (int i = 0; i<5; i++)
		ifs.getline(chrTemp, 2048);

	Point2f pt1, pt2;
	do
	{
		ifs.getline(chrTemp, 2048);
		sscanf(chrTemp, "%f%f%f%f", &pt1.x, &pt1.y, &pt2.x, &pt2.y);
		pts1.push_back(pt1);
		pts2.push_back(pt2);

	} while (!ifs.eof());

	ifs.close();
	return 0;
}

/*
descriptor method
	("AKAZE-DESCRIPTOR_KAZE_UPRIGHT");    // see http://docs.opencv.org/trunk/d8/d30/classcv_1_1AKAZE.html
	("AKAZE");							  // see http://docs.opencv.org/trunk/d8/d30/classcv_1_1AKAZE.html
	("ORB");							  // see http://docs.opencv.org/trunk/de/dbf/classcv_1_1BRISK.html
	("BRISK");							  // see http://docs.opencv.org/trunk/db/d95/classcv_1_1ORB.html
match algorithm
	see http://docs.opencv.org/trunk/db/d39/classcv_1_1DescriptorMatcher.html#ab5dc5036569ecc8d47565007fa518257
	("BruteForce");
	("BruteForce-L1");
	("BruteForce-Hamming");
	("BruteForce-Hamming(2)");
	("FlannBased")
*/
long ImgFeaturesTools::ImgFeaturesTools_ExtractMatch(const char* pathImage1, vector<Point2f> &pts1, const char* pathImage2, vector<Point2f> &pts2,string descriptorMethod, string matchMethod)
{
	Mat img1 = imread(pathImage1, 1);
	cvtColor(img1, img1, CV_RGB2GRAY);
	Mat img2 = imread(pathImage2, 1);
	cvtColor(img2, img2, CV_RGB2GRAY);
	if (img1.rows*img1.cols <= 0)
	{
		printf("Image %s is empty or cannot be found\n", pathImage1);
		return(0);
	}
	if (img2.rows*img2.cols <= 0)
	{
		printf("Image %s is empty or cannot be found\n", pathImage2);
		return(0);
	}

	vector<double> desMethCmp;
	Ptr<Feature2D> bFeatures;

	Ptr<DescriptorMatcher> descriptorMatcher;
	// Match between img1 and img2
	vector<DMatch> matches;
	// keypoint  for img1 and img2
	vector<KeyPoint> keyImg1, keyImg2;
	// Descriptor for img1 and img2
	Mat descImg1, descImg2;
	if (descriptorMethod == "AKAZE-DESCRIPTOR_KAZE_UPRIGHT") {
		bFeatures = AKAZE::create(AKAZE::DESCRIPTOR_KAZE_UPRIGHT);
	}
	if (descriptorMethod == "AKAZE") {
		bFeatures = AKAZE::create();
	}
	if (descriptorMethod == "ORB") {
		bFeatures = ORB::create();
	}
	else if (descriptorMethod == "BRISK") {
		bFeatures = BRISK::create();
	}

	try
	{
		// We can detect keypoint with detect method
		//bFeatures->detect(img1, keyImg1, Mat());
		// and compute their descriptors with method  compute
		//bFeatures->compute(img1, keyImg1, descImg1);
		// or detect and compute descriptors in one step
		bFeatures->detectAndCompute(img1, Mat(), keyImg1, descImg1, false);
		bFeatures->detectAndCompute(img2, Mat(), keyImg2, descImg2, false);
		descriptorMatcher = DescriptorMatcher::create(matchMethod);
		if ((matchMethod == "BruteForce-Hamming" || matchMethod == "BruteForce-Hamming(2)") && (bFeatures->descriptorType() == CV_32F || bFeatures->defaultNorm() <= NORM_L2SQR))
		{
			printf("**************************************************************************\n");
			printf("It's strange. You should use Hamming distance only for a binary descriptor\n");
			printf("**************************************************************************\n");
		}
		if ((matchMethod == "BruteForce" || matchMethod == "BruteForce-L1") && (bFeatures->defaultNorm() >= NORM_HAMMING))
		{
			printf("**************************************************************************\n");
			printf("It's strange. You shouldn't use L1 or L2 distance for a binary descriptor\n");
			printf("**************************************************************************\n");
		}

		try
		{
			descriptorMatcher->match(descImg1, descImg2, matches, Mat());
			// Keep best matches only to have a nice drawing.
			// We sort distance between descriptor matches
			Mat index;
			int nbMatch = int(matches.size());
			Mat tab(nbMatch, 1, CV_32F);
			for (int i = 0; i<nbMatch; i++)
			{
				tab.at<float>(i, 0) = matches[i].distance;
			}
			sortIdx(tab, index, SORT_EVERY_COLUMN + SORT_ASCENDING);
			vector<DMatch> bestMatches;
			vector<DMatch>::iterator it;
			int num = nbMatch > 110 ? 110 : nbMatch;
			for (int i = 0; i<num; i++)
			{
				bestMatches.push_back(matches[index.at<int>(i, 0)]);
			}

			// Saved result could be wrong due to bug 4308
			// Use to compute distance between keyPoint matches and to evaluate match algorithm
			double cumSumDist2 = 0;
			for (it = bestMatches.begin(); it != bestMatches.end(); it++)
			{
				Point2f pt1, pt2;
				pt1 = keyImg1[it->queryIdx].pt;
				pt2 = keyImg2[it->trainIdx].pt;
				pts1.push_back(pt1);
				pts2.push_back(pt2);

				Point2d p = keyImg1[it->queryIdx].pt - keyImg2[it->trainIdx].pt;
				cumSumDist2 = p.x*p.x + p.y*p.y;
			}
			ImgFeaturesTools_MatchOptimize(pts1, pts2);
			desMethCmp.push_back(cumSumDist2);
		}
		catch (Exception& e)
		{
			printf("%s\n",e.msg.c_str());
			printf("Cumulative distance cannot be computed.\n");
			desMethCmp.push_back(-1);
		}
	}
	catch (Exception& e)
	{
		printf("Feature : %s\n", descriptorMethod);
		printf(e.msg.c_str());
	}
}

long ImgFeaturesTools::ImgFeaturesTools_ExtracMatches(vector<string> pathList, vector<vector<Point2f>> &pts, bool* ismatchpair, string descriptorMethod, string matchMethod)
{
	//进行特征点匹配
	int size = pathList.size();
	int matchpairs = 0;
	for (size_t i = 0; i < size; i++)
	{
		for (size_t j = i+1; j < size; j++)
		{
			if (ismatchpair[i*size + j])
			{
				matchpairs++;
				printf("extract match pairs %d\r", matchpairs);
				vector<Point2f> pts1, pts2;
				ImgFeaturesTools_ExtractMatch(pathList[i].c_str(), pts1, pathList[j].c_str(), pts2, descriptorMethod, matchMethod);
				pts.push_back(pts1);
				pts.push_back(pts2);
			}
		}
	}
	printf("\n");
	return 0;
}

long ImgFeaturesTools::ImgFeatruesTools_ExtractFeatures(const char* pathImage, Mat& descriptor, vector<Point2f> &keypoints, string descriptorMethod)
{
	Mat img1 = imread(pathImage, IMREAD_LOAD_GDAL);
	if (img1.rows*img1.cols <= 0)
	{
		printf("Image %s is empty or cannot be found\n", pathImage);
		return(0);
	}

	keypoints.clear();
	vector<double> desMethCmp;
	Ptr<Feature2D> bFeatures;
	vector<DMatch> matches;
	vector<KeyPoint> keyImg1;
	if (descriptorMethod == "AKAZE-DESCRIPTOR_KAZE_UPRIGHT") {
		bFeatures = AKAZE::create(AKAZE::DESCRIPTOR_KAZE_UPRIGHT);
	}
	if (descriptorMethod == "AKAZE") {
		bFeatures = AKAZE::create();
	}
	if (descriptorMethod == "ORB") {
		bFeatures = ORB::create();
	}
	else if (descriptorMethod == "BRISK") {
		bFeatures = BRISK::create();
	}

	try
	{
		// We can detect keypoint with detect method
		bFeatures->detect(img1, keyImg1, Mat());
		for (size_t i = 0; i < keyImg1.size(); i++)
			keypoints.push_back(keyImg1[i].pt);
		// and compute their descriptors with method  compute
		//bFeatures->compute(img1, keyImg1, descImg1);
		// or detect and compute descriptors in one step
	}
	catch (Exception& e)
	{
		printf("Feature : %s\n", descriptorMethod);
		printf(e.msg.c_str());
	}


}

long ImgFeaturesTools::ImgFeaturesTools_MatchOptimize(vector<Point2f> &pts1, vector<Point2f> &pts2)
{
	if (pts1.size() < 3 || pts2.size() < 3)
	{
		printf("match points too little\n");
		exit(-1);
	}
	Mat homography = findHomography(pts1, pts2, CV_RANSAC);
	vector<Point2f> pts2Homography(pts1.size());
	perspectiveTransform(pts1, pts2Homography, homography);
	float *err = NULL;
	try
	{
		err = new float[pts1.size()];
	}
	catch (bad_alloc &e)
	{
		printf(e.what());
	}
	
	//获取误差
	for (size_t i = 0; i < pts1.size(); i++)
		err[i] = sqrt((pts2Homography[i].x - pts2[i].x)*(pts2Homography[i].x - pts2[i].x)+ (pts2Homography[i].y - pts2[i].y)*(pts2Homography[i].y - pts2[i].y));
	
	//均值和标准差
	float fAverage = 0, fDeviation = 0;
	GetAveDev(err, 1, pts1.size(), 0, fAverage, fDeviation);

	vector<Point2f> ptsOpt1, ptsOpt2;
	for (size_t i = 0; i < pts1.size(); i++)
	{
		if (fabs(err[i] - fAverage) < fDeviation)
		{
			ptsOpt1.push_back(pts1[i]);
			ptsOpt2.push_back(pts2[i]);
		}
	}

	pts1.clear();
	pts2.clear();
	pts1.insert(pts1.begin(), ptsOpt1.begin(), ptsOpt1.end());
	pts2.insert(pts2.begin(), ptsOpt2.begin(), ptsOpt2.end());

	return 0;
}

long ImgFeaturesTools::ImgFeaturesTools_WriteENVIMatches(const char* pathDir, vector<string> pathList, vector<vector<Point2f>> &pts)
{
	for (size_t i = 0; i < pts.size()/2; i++)
	{
		char pathDstPts[256], cA[20], cB[20];
		sprintf_s(cA, "\\%d.pts", i);
		strcpy_s(pathDstPts, pathDir);
		strcat_s(pathDstPts, cA);
		ImgFeaturesTools_SaveENVIMatches(pathDstPts, pathList[i].c_str(), pathList[i + 1].c_str(), pts[2 * i + 0], pts[2 * i + 1]);
	}
	return 0;
}

//============================================================================================================================================================================
long PhotogrammetryTools::PhotogrammetryTools_Resection(vector<Point3f> ptsCoordinate, vector<Point2f> ptsImg, double* cameraMat, double* rotationMat, double* translateMat, bool isCameraGuess/* = true*/)
{
	if (ptsCoordinate.size() < 3)
	{
		printf("match points too little\n");
		exit(-1);
	}
	Mat cameraMatrix= Mat::zeros(3,3,CV_32F);
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			cameraMatrix.at<double>(i, j) = cameraMat[j * 3 + i];
	Mat rvec, tvec;
	solvePnP(ptsCoordinate, ptsImg, cameraMatrix, NULL, rvec, tvec, isCameraGuess, SOLVEPNP_ITERATIVE);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			rotationMat[j * 3 + i]= rvec.at<double>(i, j);
	for (int i = 0; i < 3; ++i)
		translateMat[i]= tvec.at<double>(i, 0) ;

	return 0;
}

long PhotogrammetryTools::PhotogrammetryTools_FundamentalMat(vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &fundmentalMat)
{
	if (ptsImg1.size() < 7)
	{
		printf("too little corresponding  points\n");
		exit(-1);
	}
	if (ptsImg1.size() == 7)
	{
		fundmentalMat=findFundamentalMat(ptsImg1, ptsImg2, CV_FM_7POINT);
	}
	if (ptsImg1.size() > 7)
	{
		fundmentalMat = findFundamentalMat(ptsImg1, ptsImg2, CV_FM_RANSAC);
	}

	return 0;
}

long PhotogrammetryTools::PhotogrammetryTools_EssitialMat(vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &essitialMat, double fLen, Point2d principalPnt)
{
	if (ptsImg1.size() < 7)
	{
		printf("too little corresponding  points\n");
		exit(-1);
	}
	else
	{
		essitialMat = findEssentialMat(ptsImg1, ptsImg2,fLen,principalPnt,RANSAC);
	}
	return 0;
}

long PhotogrammetryTools::PhotogrammetryTools_ExtractRT(Mat essitialMat, vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &rotMatrix, Mat &translateMatrix, double fLen, Point2d principalPnt)
{
	recoverPose(essitialMat, ptsImg1, ptsImg2, rotMatrix, translateMatrix, fLen, principalPnt);
	return 0;
}

long PhotogrammetryTools::PhotogrammetryTools_Homography(vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &homographyMat)
{
	homographyMat = findHomography(ptsImg1, ptsImg2);
	return 0;
}
//==============================================================================================================================================================================
Mat watershed_marker_mask;
Mat watershed_g_markers;
Mat watershed_img0, watershed_img, watershed_img_gray, watershed_wshed;
Point_<int> watershed_prev_pt;

int canny_edgeThresh = 1;
Mat canny_image, canny_gray, canny_edge, canny_cedge;

// define a trackbar callback
static void ImgSegmentTools_onTrackbar(int, void*)
{
	blur(canny_gray, canny_edge, Size(3, 3));

	// Run the edge detector on grayscale
	Canny(canny_edge, canny_edge, canny_edgeThresh, canny_edgeThresh * 3, 3);
	canny_cedge = Scalar::all(0);

	canny_image.copyTo(canny_cedge, canny_edge);
	imshow("Edge map", canny_cedge);
}
static void ImageSegmentTools_mouse(int event, int x, int y, int flags, void* param)
{
	if (watershed_img.rows == 0)
		return;
	if (event == CV_EVENT_LBUTTONUP || !(flags&CV_EVENT_FLAG_LBUTTON))
		watershed_prev_pt = cv::Point_<int>(-1, -1);
	else if (event == CV_EVENT_LBUTTONDOWN)
		watershed_prev_pt = cv::Point2i(x, y);
	else if (event == CV_EVENT_MOUSEMOVE && (flags&CV_EVENT_FLAG_LBUTTON))
	{
		cv::Point2i pt(x, y);
		if (watershed_prev_pt.x<0)
			watershed_prev_pt = pt;
		cv::line(watershed_marker_mask, watershed_prev_pt, pt, cv::Scalar(255, 255, 255), 1, 8, 0);
		cv::line(watershed_img, watershed_prev_pt, pt, cv::Scalar(255, 255, 255), 1, 8, 0);
		watershed_prev_pt = pt;
		cv::imshow("image", watershed_img);
	}
}

long ImageSegmentTools::ImgSegmentTools_WaterShed(const char* path,  const char* pathOutput)
{
	watershed_img0 = cv::imread(path, 1);
	watershed_img = watershed_img0.clone();
	CvRNG rng = cvRNG(-1);
	watershed_img_gray = watershed_img0.clone();
	watershed_wshed = watershed_img0.clone();
	watershed_marker_mask = cv::Mat(cv::Size(watershed_img0.cols, watershed_img0.rows), 8, 1);
	watershed_g_markers = cv::Mat(cv::Size(watershed_img0.cols, watershed_img0.rows), CV_32S, 1);
	cv::cvtColor(watershed_img, watershed_marker_mask, CV_BGR2GRAY);
	cv::cvtColor(watershed_marker_mask, watershed_img_gray, CV_GRAY2BGR);
	for (int i = 0; i<watershed_marker_mask.rows; i++)
		for (int j = 0; j<watershed_marker_mask.cols; j++)
			watershed_marker_mask.at<unsigned char>(i, j) = 0;
	for (int i = 0; i<watershed_g_markers.rows; i++)
		for (int j = 0; j<watershed_g_markers.cols; j++)
			watershed_g_markers.at<int>(i, j) = 0;
	cv::imshow("image", watershed_img);
	cv::imshow("watershed transform", watershed_wshed);
	cv::setMouseCallback("image", ImageSegmentTools_mouse, 0);
	for (;;)
	{
		int c = cv::waitKey(0);
		if ((char)c == 27)
			break;
		if ((char)c == 'r')
		{
			for (int i = 0; i<watershed_marker_mask.rows; i++)
				for (int j = 0; j<watershed_marker_mask.cols; j++)
					watershed_marker_mask.at<unsigned char>(i, j) = 0;
			watershed_img0.copyTo(watershed_img);
			cv::imshow("image", watershed_img);
		}
		if ((char)c == 'w' || (char)c == ' ')
		{
			vector<vector<cv::Point>> contours;
			CvMat* color_tab = 0;
			int comp_count = 0;
			cv::findContours(watershed_marker_mask, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
			for (int i = 0; i<watershed_g_markers.rows; i++)
				for (int j = 0; j<watershed_g_markers.cols; j++)
					watershed_g_markers.at<int>(i, j) = 0;
			vector<vector<cv::Point> >::iterator iter = contours.begin();
			for (int i = 0; i<(int)contours.size(); i++)
			{
				cv::drawContours(watershed_g_markers, contours, i, cv::Scalar::all(comp_count + 1),
					1, 8, vector<cv::Vec4i>());
				comp_count++;
			}

			if (comp_count == 0)
				continue;
			color_tab = cvCreateMat(1, comp_count, CV_8UC3);
			for (int i = 0; i<comp_count; i++)
			{
				uchar* ptr = color_tab->data.ptr + i * 3;
				ptr[0] = (uchar)(cvRandInt(&rng) % 180 + 50);
				ptr[1] = (uchar)(cvRandInt(&rng) % 180 + 50);
				ptr[2] = (uchar)(cvRandInt(&rng) % 180 + 50);
			}
			cv::Mat temp = watershed_g_markers.clone();

			double t = (double)cvGetTickCount();
			//my_watershed(img0,g_markers,comp_count);  
			cv::watershed(watershed_img0, watershed_g_markers);
			t = (double)cvGetTickCount() - t;
			printf("exec time= %lf\n", t / (cvGetTickFrequency()*1000.0));
			for (int i = 0; i < watershed_g_markers.rows; i++)
			{
				for (int j = 0; j<watershed_g_markers.cols; j++)
				{
					int idx = watershed_g_markers.at<int>(i, j);
					uchar* dst = &watershed_wshed.at<uchar>(i, j * 3);
					if (idx == -1)
						dst[0] = dst[1] = dst[2] = (uchar)255;
					else if (idx <= 0 || idx>comp_count)
						dst[0] = dst[1] = dst[2] = (uchar)8;
					else {
						uchar* ptr = color_tab->data.ptr + (idx - 1) * 3;
						dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
					}
				}
			}
			Mat imgTemp;
			cv::addWeighted(watershed_wshed, 0.5, watershed_img_gray, 0.5, 0, imgTemp);
			cv::imshow("watershed transform", imgTemp);
			cvReleaseMat(&color_tab);
		}
		if (c == 'q')
			break;
	}
	imwrite(pathOutput, watershed_wshed);
	return 0;
}

long ImageSegmentTools::ImgSegmentTools_Canny(const char* path, const char* pathOutput)
{
	canny_image = imread(path, 1);
	if (canny_image.empty())
	{
		printf("Cannot read image file: %s\n", path);
		return -1;
	}
	canny_cedge.create(canny_image.size(), canny_image.type());
	cvtColor(canny_image, canny_gray, COLOR_BGR2GRAY);
	// Create a window
	namedWindow("Edge map", 1);
	// create a toolbar
	createTrackbar("Canny threshold", "Edge map", &canny_edgeThresh, 100, ImgSegmentTools_onTrackbar);
	// Show the image
	ImgSegmentTools_onTrackbar(0, 0);
	// Wait for a key stroke; the same function arranges events processing
	waitKey(0);
	imwrite(pathOutput,canny_cedge);

	return 0;

}
//================================================================================================================================================================================
void BezierCurve::BezierCurve_BezierDraw()
{

	CvSize image_sz = cvSize(1000, 1000);
	image = cvCreateImage(image_sz, 8, 3);
	cvNamedWindow("Win", 0);
	cvSetMouseCallback("Win", &BezierCurve::BezierCurve_On_mouse, 0);
	cvResizeWindow("Win", 500, 500);

	printf("==============   Bezier curve DEMO  ==============\n");
	printf(" \n");
	printf("1.use mouse to click control point (red) to select a control point\n");
	printf("2.use mouse to modify control point");
	printf("3.click mouse on somewhere to add a control point,add three points for add a new curve\n");
	printf("4.use 'W','S' to add precision or reduce precision.\n");
	printf("5.press 'Z' to show control points.\n");
	printf("===press ESC to exit===\n");

	//初始化四个控制点
	Control_pts[0].x = 200;
	Control_pts[0].y = 200;
	Control_pts[0].z = 0;

	Control_pts[1].x = 300;
	Control_pts[1].y = 500;
	Control_pts[1].z = 0;

	Control_pts[2].x = 400;
	Control_pts[2].y = 560;
	Control_pts[2].z = 0;

	Control_pts[3].x = 500;
	Control_pts[3].y = 100;
	Control_pts[3].z = 0;

	int divs = 50; //控制精细度

	for (;;)
	{

		CvPoint pt_now, pt_pre;
		cvZero(image);


		//绘制控制点
		if (is_showControlLines)
		{
			for (int i = 0; i<mark_count; i++)
			{
				CvPoint ptc;
				ptc.x = (int)Control_pts[i].x;
				ptc.y = (int)Control_pts[i].y;
				cvCircle(image, ptc, 4, CV_RGB(255, 0, 0), 1, CV_AA, 0);
			}
		}

		//绘制Bezier曲线
		CvPoint3D32f *pControls = Control_pts;
		for (int j = 0; j<mark_count - 3; j += 3)
		{
			for (int i = 0; i <= divs; i++)
			{
				float u = (float)i / divs;
				CvPoint3D32f newPt = BezierCurve_Bernstein(u, pControls);

				pt_now.x = (int)newPt.x;
				pt_now.y = (int)newPt.y;

				if (i>0)	cvLine(image, pt_now, pt_pre, CV_RGB(230, 255, 0), 2, CV_AA, 0);
				pt_pre = pt_now;
			}

			//画控制线
			if (is_showControlLines)BezierCurve_DrawControlLine(pControls);
			pControls += 3;
		}

		cvShowImage("Win", image);
		int keyCode = cvWaitKey(20);
		if (keyCode == 27) break;
		if (keyCode == 'w' || keyCode == 'W') divs += 2;
		if (keyCode == 's' || keyCode == 'S') divs -= 2;
		if (keyCode == 'z' || keyCode == 'Z') is_showControlLines = is_showControlLines ^ 1;

		//cout<<"precision : "<<divs<<endl;

	}
	return ;
}

//================================================================================================================================================================================
void ImageInpaint::ImageInpaint_Inpaint(const char* pathImg, const char* pathDst)
{
	Mat img0 = imread(pathImg,1);
	if (img0.empty())
	{
		printf("Couldn't open the image %s  . Usage: inpaint <image_name>\n",pathImg);
		return ;
	}

	namedWindow("image", 1);

	ImageInpaint_img = img0.clone();
	ImageInpaint_inpaintMask = Mat::zeros(ImageInpaint_img.size(), CV_8U);

	imshow("image", ImageInpaint_img);
	setMouseCallback("image", ImageInpaint_onMouse, 0);

	for (;;)
	{
		char c = (char)waitKey();

		if (c == 27)
			break;

		if (c == 'r')
		{
			ImageInpaint_inpaintMask = Scalar::all(0);
			img0.copyTo(ImageInpaint_img);
			imshow("image", ImageInpaint_img);
		}

		Mat inpainted;
		if (c == 'i' || c == ' ')
		{
			inpaint(ImageInpaint_img, ImageInpaint_inpaintMask, inpainted, 5, INPAINT_TELEA);
			imshow("inpainted image", inpainted);
		}
		if (c == 'q')
		{
			imwrite(pathDst, inpainted);
			break;
		}
	}
}

