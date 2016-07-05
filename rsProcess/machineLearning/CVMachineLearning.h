#pragma once
#include"../opencv/opencv2/ml.hpp"
#include "../opencv/opencv2/core/core.hpp"
#include"../opencv/opencv2/imgproc.hpp"
#include "../opencv/opencv2/highgui/highgui.hpp"
#include"../opencv/opencv2/objdetect.hpp"
using namespace std;
//using namespace cv;
using namespace cv::ml;

//数据集类型
enum DatasetTypes
{
	DATASET_MNIST = 0,
	ENVI_IMAGEROI = 1,
	IMAGE_LIST	  = 3
};

//机器学习训练算法
class CVMachineLearningTrain
{
public:
	//BP神经网络训练MNIST库并将训练结果输出
	void CV_ANN_BP_Train(const char* pathDataset, const char* pathLabelSet, const char* pathNet, DatasetTypes datasetType);

	//SVM分类器
	void CV_SVM_Train(const char* pathDataset,double C, const char* pathLabelSet, const char* pathSVM, DatasetTypes datasetType);

	//logistic regression逻辑回归训练库
	void CV_LogisticRegression_Train(const char* pathDataset, const char* pathLabelSet, const char* pathLogisticRegression, DatasetTypes datasetType);

	//AdaBoost算法
	void CV_AdaBoostTrain(const char* pathPosVec, const char* pathNegList, const char* pathAdaboost,int posNum,int negNum);
	
public:
	//获取训练数据集
	void CV_GetMnistTrainData(const char* pathMnist,const char* pathLabel, cv::Mat &trianMat, cv::Mat &labelMat);
	//获取车辆识别数据集
	void CV_GetVehicleTrainData(const char* pathDataset, cv::Mat &trainMat, cv::Mat &label);
	//获取训练数据集
	//生成正样本描述
	void CV_HaarSampleDescriptor(const char* pathPosSampleList,const char* pathOutVec, int width, int height,int num );

};

//机器学习预测算法
class CVMachineLearningPredict
{
public:
	//各种算法对Mnist数据进行预测
	void CV_ANN_BP_PredictMnist(const char* pathPredict, const char* pathNet, const char* pathLabel = NULL);
	void CV_SVM_PredictMnist(const char* pathPredict, const char* pathSVM,const char* pathLabel=NULL);
	void CV_LogisticRegression_PredictMnist(const char* pathPredict, const char* pathLogisticRegression, const char* pathLabel = NULL);

	//svm对车辆数据集进行识别
	void CV_SVM_PredictVehicle(const char* pathPredictImg, const char* pathSVM);
};