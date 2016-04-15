#pragma once

#include<vector>
#include<string>
#include<Windows.h>
using namespace std;

#include"ThreadPool.h"

////////////////////////////////////////////////////////////////////////////////////////////////

//OPENCV工具函数
#include"opencv\opencv2\stitching\detail\camera.hpp"
#include"opencv\opencv2\opencv.hpp"
#pragma comment(lib,"opencv_world310d.lib")
using namespace cv;

//影像匹配线程参数
struct ImgFeaturesThreadSt
{
	string img1;
	string img2;
	vector<Point2f> pts1;
	vector<Point2f> pts2;
	string descriptorMethod;
	string matchMethod;
};
//解算特征点线程函数
DWORD WINAPI ImgFeaturesTools_ExtractMatchThread(LPVOID lpParameters);

//影像特征点提取和匹配的工具类
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
*/
class ImgFeaturesTools
{
public:
	//1.降提取的匹配特征点保存为ENVI格式
	long ImgFeaturesTools_SaveENVIMatches(const char* pathDst, const char* pathSrc1, const char* pathSrc2, vector<Point2f> pts1, vector<Point2f> pts2);

	//2.读取ENVI格式的匹配点
	long ImgFeaturesTools_ReadENVIMatches(const char* pathPoints, vector<Point2f> &pts1, vector<Point2f> &pts2);

	//3.解算SURF匹配特征点<对于单个图像>
	long ImgFeaturesTools_ExtractMatch(const char* pathImage1, vector<Point2f> &pts1, const char* pathImage2, vector<Point2f> &pts2, string descriptorMethod, string matchMethod);
	static long ImgFeaturesTools_ExtractMatchThread(ImgFeaturesThreadSt &featureExtractThreadParam);

	//4.解算SURF匹配算子<对于序列图像>
	long ImgFeaturesTools_ExtracMatches(vector<string> pathList, vector<vector<Point2f>> &pts, bool* ismatchpair, string descriptorMethod, string matchMethod);
private:
	//5.获取SURF匹配特征点和特征描述
	long ImgFeatruesTools_ExtractFeatures(const char* pathImage, Mat& descriptor, vector<Point2f> &keypoints,string descriptorMethod);

	//6.匹配特征点优化
	long ImgFeaturesTools_MatchOptimize(vector<Point2f> &pts1, vector<Point2f> &pts2);
};

//摄影测量的工具类
class PhotogrammetryTools
{
public:
	//1.空间后方交会，通过地面点坐标和影像坐标求解出影像的外方位元素
	long PhotogrammetryTools_Resection(vector<Point3f> ptsCoordinate, vector<Point2f> ptsImg, double* cameraMat, double* rotationMat, double* translateMat, bool isCameraGuess = true);

	//2.获取基础矩阵x1Fx2=0
	long PhotogrammetryTools_FundamentalMat(vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &fundmentalMat);

	//3.获取本征矩阵
	long PhotogrammetryTools_EssitialMat(vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &essitialMat,double fLen,Point2d principalPnt);

	//4.获取相对定向元素的旋转和平移矩阵
	long PhotogrammetryTools_ExtractRT(Mat essitialMat, vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &rotMatrix, Mat &translateMatrix, double fLen, Point2d principalPnt);

	//5.获取单应矩阵
	long PhotogrammetryTools_Homography(vector<Point2f> ptsImg1, vector<Point2f> ptsImg2, Mat &homographyMat);
};


//影像分割工具类
//分水岭算法预定义工具
class ImageSegmentTools
{
public:
	//1.分水岭算法，分水岭算法需要选取标记，因此需要获取鼠标点击
	long ImgSegmentTools_WaterShed(const char* path,const char* pathOutput);

	//2.使用canny算子进行分割
	long ImgSegmentTools_Canny(const char* path, const char* pathOutput);

public:

};

//opencv贝塞尔曲线工具
const int WW_MAX_MARK_COUNT = 40; //最大40个控制点
static int mark_count = 4;
static int conner_pt_index = -1;
static CvPoint3D32f Control_pts[WW_MAX_MARK_COUNT];
static IplImage *image = NULL; //原始图像
static bool is_showControlLines = true;

class BezierCurve
{
private:

	// 两个向量相加，p=p+q
	static CvPoint3D32f BezierCurve_PointAdd(CvPoint3D32f p, CvPoint3D32f q) {
		p.x += q.x;		p.y += q.y;		p.z += q.z;
		return p;
	}

	// 向量和标量相乘p=c*p
	static CvPoint3D32f BezierCurve_PointTimes(float c, CvPoint3D32f p) {
		p.x *= c;	p.y *= c;	p.z *= c;
		return p;
	}

	// 计算贝塞尔方程的值
	// 变量u的范围在0-1之间
	//P1*t^3 + P2*3*t^2*(1-t) + P3*3*t*(1-t)^2 + P4*(1-t)^3 = Pnew 
	static CvPoint3D32f BezierCurve_Bernstein(float u, CvPoint3D32f *p) {
		CvPoint3D32f	a, b, c, d, r;

		a = BezierCurve_PointTimes(pow(u, 3), p[0]);
		b = BezierCurve_PointTimes(3 * pow(u, 2)*(1 - u), p[1]);
		c = BezierCurve_PointTimes(3 * u*pow((1 - u), 2), p[2]);
		d = BezierCurve_PointTimes(pow((1 - u), 3), p[3]);

		r = BezierCurve_PointAdd(BezierCurve_PointAdd(a, b), BezierCurve_PointAdd(c, d));

		return r;
	}

	//画控制线
	static void BezierCurve_DrawControlLine(CvPoint3D32f *p) {

		CvPoint pc[4];
		for (int i = 0; i<4; i++)
		{
			pc[i].x = (int)p[i].x;
			pc[i].y = (int)p[i].y;
		}

		cvLine(image, pc[0], pc[1], CV_RGB(0, 0, 255), 1, CV_AA, 0);
		cvLine(image, pc[2], pc[3], CV_RGB(0, 0, 255), 1, CV_AA, 0);
	}

	//得到最近Control_pts的index
	static int BezierCurve_GetNearPointIndex(CvPoint mouse_pt)
	{
		CvPoint pt;
		for (int i = 0; i<mark_count; i++)
		{
			pt.x = mouse_pt.x - (int)Control_pts[i].x;
			pt.y = mouse_pt.y - (int)Control_pts[i].y;
			float distance = sqrt((float)(pt.x*pt.x + pt.y*pt.y));
			if (distance<10) return i;

		}
		return -1;
	}

	static void BezierCurve_On_mouse(int event, int x, int y, int flags, void *param)
	{
		if (event == CV_EVENT_LBUTTONDOWN)
		{
			CvPoint pt = cvPoint(x, y);
			//cout<<x<<","<<y<<endl;

			if (conner_pt_index >-1)
				conner_pt_index = -1;
			else
			{
				conner_pt_index = BezierCurve_GetNearPointIndex(pt);
				//添加新的控制点
				if (conner_pt_index == -1)
				{
					if (mark_count <= (WW_MAX_MARK_COUNT - 1))
					{
						Control_pts[mark_count].x = (float)pt.x;
						Control_pts[mark_count].y = (float)pt.y;
						Control_pts[mark_count].z = 0;
						mark_count++;
					}
				}
			}
		}
		else if (event == CV_EVENT_MOUSEMOVE) //修改控制点坐标
		{
			if (conner_pt_index >-1)
			{
				Control_pts[conner_pt_index].x = (float)x;
				Control_pts[conner_pt_index].y = (float)y;
			}
		}

	};

public:
	//绘制贝塞尔曲线
	void BezierCurve_BezierDraw();
};

//opencv 影像修复方法
static Mat ImageInpaint_img, ImageInpaint_inpaintMask;
static Point ImageInpaint_prevPt(-1, -1);
class ImageInpaint
{
private:
	static void ImageInpaint_onMouse(int event, int x, int y, int flags, void*)
	{
		if (event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))
			ImageInpaint_prevPt = Point(-1, -1);
		else if (event == CV_EVENT_LBUTTONDOWN)
			ImageInpaint_prevPt = Point(x, y);
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
		{
			Point pt(x, y);
			if (ImageInpaint_prevPt.x < 0)
				ImageInpaint_prevPt = pt;
			line(ImageInpaint_inpaintMask, ImageInpaint_prevPt, pt, Scalar::all(255), 5, 8, 0);
			line(ImageInpaint_img, ImageInpaint_prevPt, pt, Scalar::all(255), 5, 8, 0);
			ImageInpaint_prevPt = pt;
			imshow("image", ImageInpaint_img);
		}
	}

public:
	void ImageInpaint_Inpaint(const char* pathImg,const char* pathDst);
};