#include"CVMachineLearning.h"

#pragma comment(lib,"opencv_world310d.lib")

#include<fstream>
#include"mnistFile.h"



void CVMachineLearningTrain::CV_ANN_BP_Train(const char* pathDataset, const char* pathLabelSet, const char* pathNet, DatasetTypes datasetType)
{
	Ptr<ANN_MLP> ann = ANN_MLP::create();
	int layers[3] = { 28 * 28,50,10 };
	Mat_<int> layerSize(1, 3);
	memcpy(layerSize.data, layers, sizeof(int) * 3);
	ann->setLayerSizes(layerSize);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, FLT_EPSILON));
	ann->setTrainMethod(ANN_MLP::BACKPROP, 0.001);
	Mat trainMat, labelMat;
	if(datasetType== DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainMat, labelMat);
	Ptr<TrainData> tData = TrainData::create(trainMat, COL_SAMPLE, labelMat);
	printf("BP Netural Network train~ing...\n");
	ann->train(tData);
	printf("-done\n");
	ann->save(pathNet);
}

void CVMachineLearningTrain::CV_SVM_Train(const char* pathDataset,double C ,const char* pathLabelSet, const char* pathSVM, DatasetTypes datasetType)
{
	Mat trainData;
	Mat labelsMat;
	if(datasetType== DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainData, labelsMat);
	Ptr<TrainData> tData = TrainData::create(trainData, COL_SAMPLE, labelsMat);

	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::POLY); //SVM::LINEAR;
	svm->setDegree(0.5);
	svm->setGamma(1);
	svm->setCoef0(1);
	svm->setNu(0.5);
	svm->setP(0);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 0.01));
	svm->setC(C);
	printf("SVM train~ing...\n");
	svm->train(tData);
	printf("-done\n");
	svm->save(pathSVM);
}

void CVMachineLearningTrain::CV_LogisticRegression_Train(const char* pathDataset, const char* pathLabelSet, const char* pathLogisticRegression, DatasetTypes datasetType)
{
	Mat trainData;
	Mat labelsMat;
	if (datasetType == DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainData, labelsMat);

	Ptr<LogisticRegression> lr1 = LogisticRegression::create();
	lr1->setLearningRate(0.001);
	lr1->setIterations(10);
	lr1->setRegularization(LogisticRegression::REG_L2);
	lr1->setTrainMethod(LogisticRegression::BATCH);
	lr1->setMiniBatchSize(1);
	printf("logistic regression train~ing...\n");
	lr1->train(trainData, COL_SAMPLE, labelsMat);
	printf("-done\n");
	lr1->save(pathLogisticRegression);
}


//==================================================================================================================================
void CVMachineLearningTrain::CV_GetMnistTrainData(const char* pathMnist, const char* pathLabel, Mat &trianMat, Mat &labelMat)
{
	ifstream ifs(pathMnist, ios_base::binary);
	char magicNum[4], ccount[4], crows[4], ccols[4];
	ifs.read(magicNum, sizeof(magicNum)); ifs.read(ccount, sizeof(ccount));
	ifs.read(crows, sizeof(crows)); ifs.read(ccols, sizeof(ccols));
	mnistFile tmp;
	int count, rows, cols;
	tmp.swapBuffer(ccount); tmp.swapBuffer(crows); tmp.swapBuffer(ccols);
	memcpy(&count, ccount, sizeof(count));
	memcpy(&rows, crows, sizeof(rows));
	memcpy(&cols, ccols, sizeof(cols));
	//count = 500;
	unsigned char* imgData = new unsigned char[count*rows*cols];
	float* imgDataf = new float[count*rows*cols];
	int num = 0;
	for (int j = 0; j < count; ++j)
	{
		ifs.read((char*)imgData + num*rows*cols, rows * cols);
		for (int i = 0; i < rows * cols; ++i)
			imgDataf[j*rows * cols + i] = float(imgData[j*rows * cols + i]) / 255.f;
	}
	ifs.close();
	Mat tmp1(rows*cols, count, CV_32FC1, imgDataf);
	trianMat = tmp1.clone();
	ifstream lab_ifs(pathLabel, ios_base::binary);
	lab_ifs.read(magicNum, sizeof(char) * 4);
	lab_ifs.read(ccount, sizeof(char) * 4);
	int intmagicNum, ccountint;

	tmp.swapBuffer(magicNum);
	tmp.swapBuffer(ccount);
	memcpy(&intmagicNum, magicNum, sizeof(magicNum));
	memcpy(&ccountint, ccount, sizeof(ccount));
	//ccountint = 500;
	float* label = new float[ccountint * 10];		//number label
	memset(label, 0, sizeof(float)*ccountint * 10);
	char tmpLabel;
	num = 0;
	for (int i = 0; i < ccountint; ++i)
	{
		lab_ifs.read(&tmpLabel, sizeof(tmpLabel));
		int tlabel = tmpLabel;
		label[10 * i + tlabel] = 1.0f;
	}

	lab_ifs.close();
	Mat labelsMat(10, count, CV_32FC1, label);
	labelMat = labelsMat.clone();

	delete[]imgData; imgData = NULL;
	delete[]label; label = NULL;
	delete[]imgDataf; imgDataf = NULL;
}

//==================================================================================================================================
void CVMachineLearningPredict::CV_ANN_BP_PredictMnist(const char* pathPredict, const char* pathNet)
{
	Ptr<ANN_MLP> ann = ANN_MLP::create();
	ann = Algorithm::load<ANN_MLP>(pathNet);
	ifstream ifs(pathPredict, ios_base::binary);

	char magicNum[4], ccount[4], crows[4], ccols[4];
	ifs.read(magicNum, sizeof(magicNum)); ifs.read(ccount, sizeof(ccount));
	ifs.read(crows, sizeof(crows)); ifs.read(ccols, sizeof(ccols));

	mnistFile tmp;
	int count, rows, cols;
	tmp.swapBuffer(ccount); tmp.swapBuffer(crows); tmp.swapBuffer(ccols);
	memcpy(&count, ccount, sizeof(count));
	memcpy(&rows, crows, sizeof(rows));
	memcpy(&cols, ccols, sizeof(cols));

	unsigned char* imgData = new unsigned char[rows*cols];
	float* imgDataf = new float[rows*cols];
	int num = 0;
	for (int j = 0; j < count; ++j)
	{
		ifs.read((char*)imgData, rows * cols);
		for (int i = 0; i < rows*cols; ++i)
			imgDataf[i] = imgData[i];
		Mat tData(1, rows*cols, CV_32FC1, imgDataf);
		Mat pData;// = Mat::zeros(1, 10, CV_32FC1);
		Mat ShowData(rows, cols, CV_8UC1, imgData);
		ann->predict(tData, pData);

		imshow("data", ShowData);
		float* pdataf = (float*)pData.data;
		float max = -100.0f;
		int idx = 0;
		for (int i = 0; i < 10; ++i)
		{
			if (pdataf[i] > max)
			{
				max = pdataf[i];
				idx = i;
			}
		}
		printf("predict number:%d\n", idx);
		waitKey(500);
	}
	delete[]imgData;
	delete[]imgDataf;
	ifs.close();
}