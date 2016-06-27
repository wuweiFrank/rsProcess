#pragma once
#include"../opencv/opencv2/ml.hpp"
#include "../opencv/opencv2/core/core.hpp"
#include"../opencv\opencv2\imgproc.hpp"
#include "../opencv/opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
using namespace ml;

//数据集类型
enum DatasetTypes
{
	DATASET_MNIST = 0,
	ENVI_IMAGEROI = 1
	
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

private:
	//获取训练数据集
	void CV_GetMnistTrainData(const char* pathMnist,const char* pathLabel,Mat &trianMat, Mat &labelMat);
};

//机器学习预测算法
class CVMachineLearningPredict
{
public:
	//各种算法对Mnist数据进行预测
	void CV_ANN_BP_PredictMnist(const char* pathPredict, const char* pathNet, const char* pathLabel = NULL);
	void CV_SVM_PredictMnist(const char* pathPredict, const char* pathSVM,const char* pathLabel=NULL);
	void CV_LogisticRegression_PredictMnist(const char* pathPredict, const char* pathLogisticRegression, const char* pathLabel = NULL);
};