#pragma once
class CVMachineLearningTrain
{
public:
	//BP神经网络训练MNIST库并将训练结果输出
	void CV_ANN_BP_TrainMnist(const char* pathMnist, const char* pathLabel, const char* pathNet);
};