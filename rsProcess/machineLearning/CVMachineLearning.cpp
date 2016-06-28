#include"CVMachineLearning.h"
#include"..\\AuxiliaryFunction.h"
#pragma comment(lib,"opencv_world310d.lib")

#include<fstream>
#include"mnistFile.h"

//==================================================================================================================================
void CVMachineLearningTrain::CV_ANN_BP_Train(const char* pathDataset, const char* pathLabelSet, const char* pathNet, DatasetTypes datasetType)
{
	Ptr<ANN_MLP> ann = ANN_MLP::create();
	int layers[3] = { 28 * 28,15,10 };
	Mat_<int> layerSize(1, 3);
	memcpy(layerSize.data, layers, sizeof(int) * 3);
	ann->setLayerSizes(layerSize);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, FLT_EPSILON));
	ann->setTrainMethod(ANN_MLP::BACKPROP, 0.001);
	Mat trainMat, labelMat;
	if (datasetType == DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainMat, labelMat);
	Ptr<TrainData> tData = TrainData::create(trainMat, ROW_SAMPLE, labelMat);
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

	float* labelf =(float*)labelsMat.data;
	Mat labelsMatAdapter=Mat::zeros(labelsMat.rows, 1, CV_32SC1);
	for (int i = 0; i < labelsMat.rows; ++i)
	{
		int j = 0;
		for (j = 0; j < labelsMat.cols; ++j)
		{
			if (abs(labelf[i*labelsMat.cols +j] - 1) < 0.0001)
				break;
		}
		labelsMatAdapter.at<int>(i, 0)  = j;
	}

	Ptr<TrainData> tData = TrainData::create(trainData, ml::ROW_SAMPLE, labelsMatAdapter);
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::POLY); //SVM::LINEAR;
	svm->setDegree(1);
	svm->setGamma(0.3);
	svm->setCoef0(1.0);
	svm->setNu(1);
	svm->setP(0.5);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 10000, 0.01));
	svm->setC(C);
	printf("SVM train~ing...\n");
	svm->train(tData);
	printf("-done\n");
	svm->save(pathSVM);

	///////////////////////////////////////////////////////////
}

void CVMachineLearningTrain::CV_LogisticRegression_Train(const char* pathDataset, const char* pathLabelSet, const char* pathLogisticRegression, DatasetTypes datasetType)
{
	Mat trainData;
	Mat labelsMat;
	if (datasetType == DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainData, labelsMat);
	float* labelf = (float*)labelsMat.data;
	Mat labelsMatAdapter = Mat::zeros(labelsMat.rows, 1, CV_32FC1);
	for (int i = 0; i < labelsMat.rows; ++i)
	{
		int j = 0;
		for (j = 0; j < labelsMat.cols; ++j)
		{
			if (abs(labelf[i*labelsMat.cols + j] - 1) < 0.0001)
				break;
		}
		labelsMatAdapter.at<float>(i, 0) = j;
	}
	float* data = (float*)labelsMatAdapter.data;
	Ptr<LogisticRegression> lr1 = LogisticRegression::create();
	lr1->setLearningRate(0.001);
	lr1->setIterations(10);
	lr1->setRegularization(LogisticRegression::REG_L2);
	lr1->setTrainMethod(LogisticRegression::BATCH);
	lr1->setMiniBatchSize(1);
	printf("logistic regression train~ing...\n");
	lr1->train(trainData, ROW_SAMPLE, labelsMatAdapter);
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

	ifstream lab_ifs(pathLabel, ios_base::binary);
	lab_ifs.read(magicNum, sizeof(char) * 4);
	lab_ifs.read(ccount, sizeof(char) * 4);
	int intmagicNum, ccountint;

	tmp.swapBuffer(magicNum);
	tmp.swapBuffer(ccount);
	memcpy(&intmagicNum, magicNum, sizeof(magicNum));
	memcpy(&ccountint, ccount, sizeof(ccount));

	unsigned char* imgData = new unsigned char[rows*cols];
	float* imgDataf = new float[rows*cols];
	float* label = new float[10];
	int num = 0;
	double totalNormal = 0;
	Mat tmp1(count, rows*cols, CV_32FC1);
	Mat labelsMat(count, 10, CV_32FC1);
	char tmpLabel;

	for (int j = 0; j < count; ++j)
	{
		ifs.read((char*)imgData, rows * cols);
		for (int i = 0; i < rows * cols; ++i)
			imgDataf[i]= float(imgData[i])/255.f;

		memset(label, 0, sizeof(float) * 10);
		lab_ifs.read(&tmpLabel, sizeof(tmpLabel));
		int tlabel = tmpLabel;
		label[tlabel] = 1.0f;

		memcpy(tmp1.data + j*rows*cols*sizeof(float), imgDataf, rows*cols*sizeof(float));
		memcpy(labelsMat.data + j*10*sizeof(float), label, 10*sizeof(float));
	}
	
	trianMat = tmp1.clone();
	labelMat = labelsMat.clone();
	float* p = (float*)labelsMat.data;
	lab_ifs.close();
	ifs.close();

	delete[]imgData; imgData = NULL;
	delete[]label; label = NULL;
	delete[]imgDataf; imgDataf = NULL;
}


//==================================================================================================================================
void CVMachineLearningPredict::CV_ANN_BP_PredictMnist(const char* pathPredict, const char* pathNet, const char* pathLabel/* = NULL*/)
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

	ifstream lab_ifs;
	if (pathLabel != NULL)
	{
		lab_ifs.open(pathLabel, ios_base::binary);

		lab_ifs.read(magicNum, sizeof(char) * 4);
		lab_ifs.read(ccount, sizeof(char) * 4);
		int intmagicNum, ccountint;

		tmp.swapBuffer(magicNum);
		tmp.swapBuffer(ccount);
		memcpy(&intmagicNum, magicNum, sizeof(magicNum));
		memcpy(&ccountint, ccount, sizeof(ccount));
	}


	unsigned char* imgData = new unsigned char[rows*cols];
	float* imgDataf = new float[rows*cols];
	int num = 0; char tmpLabel;
	for (int j = 0; j < count; ++j)
	{
		ifs.read((char*)imgData, rows * cols);
		if (lab_ifs.is_open())
			lab_ifs.read(&tmpLabel, sizeof(tmpLabel));

		for (int i = 0; i < rows*cols; ++i)
			imgDataf[i] = float(imgData[i]) / 255.f;
		//NormalizeData(imgDataf, rows*cols, imgDataf);
		Mat tData(1, rows*cols, CV_32FC1, imgDataf);
		Mat pData;// = Mat::zeros(1, 10, CV_32FC1);
		ann->predict(tData, pData);

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
		if (lab_ifs.is_open())
		{
			int label = tmpLabel;
			if (label == idx)
				num++;
		}

		Mat ShowData(rows, cols, CV_8UC1, imgData);
		imshow("data", ShowData);
		printf("predict number:%d\n", idx);
		waitKey(500);
	}
	printf("precentage of correct :%lf %\n", float(num) / float(count) * 100);
	delete[]imgData;
	delete[]imgDataf;
	ifs.close();
}

void CVMachineLearningPredict::CV_SVM_PredictMnist(const char* pathPredict, const char* pathSVM, const char* pathLabel)
{
	Ptr<SVM> svm= Algorithm::load<SVM>(pathSVM);
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

	ifstream lab_ifs;
	if (pathLabel != NULL)
	{
		lab_ifs.open(pathLabel, ios_base::binary);

		lab_ifs.read(magicNum, sizeof(char) * 4);
		lab_ifs.read(ccount, sizeof(char) * 4);
		int intmagicNum, ccountint;

		tmp.swapBuffer(magicNum);
		tmp.swapBuffer(ccount);
		memcpy(&intmagicNum, magicNum, sizeof(magicNum));
		memcpy(&ccountint, ccount, sizeof(ccount));
	}

	unsigned char* imgData = new unsigned char[rows*cols];
	float* imgDataf = new float[rows*cols];
	char tmpLabel;
	int num = 0;
	for (int j = 0; j < count; ++j)
	{
		ifs.read((char*)imgData, rows * cols);

		if(lab_ifs.is_open())
			lab_ifs.read(&tmpLabel, sizeof(tmpLabel));

		for (int i = 0; i < rows*cols; ++i)
			imgDataf[i] = double(imgData[i])/255.0f;

		Mat tData(1, rows*cols, CV_32FC1, imgDataf);
		int tmp=(int)svm->predict(tData);

		if (lab_ifs.is_open())
		{
			int label = tmpLabel;
			if (label == tmp)
				num++;
		}

		Mat ShowData(rows, cols, CV_8UC1, imgData);
		cv::resize(ShowData, ShowData, Size(4* rows, 4 * cols));
		imshow("data", ShowData);
		cout << tmp << endl;
		waitKey(500);
	}
	printf("precentage of correct :%lf %\n",float(num)/float(count)*100);
	delete[]imgData;
	delete[]imgDataf;
	ifs.close();
}

void CVMachineLearningPredict::CV_LogisticRegression_PredictMnist(const char* pathPredict, const char* pathLogisticRegression, const char* pathLabel)
{
	Ptr<LogisticRegression> lr2 = Algorithm::load<LogisticRegression>(pathLogisticRegression);
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

	ifstream lab_ifs;
	if (pathLabel != NULL)
	{
		lab_ifs.open(pathLabel, ios_base::binary);

		lab_ifs.read(magicNum, sizeof(char) * 4);
		lab_ifs.read(ccount, sizeof(char) * 4);
		int intmagicNum, ccountint;

		tmp.swapBuffer(magicNum);
		tmp.swapBuffer(ccount);
		memcpy(&intmagicNum, magicNum, sizeof(magicNum));
		memcpy(&ccountint, ccount, sizeof(ccount));
	}

	unsigned char* imgData = new unsigned char[rows*cols];
	float* imgDataf = new float[rows*cols];
	char tmpLabel;
	int num = 0;
	for (int j = 0; j < count; ++j)
	{
		ifs.read((char*)imgData, rows * cols);

		if (lab_ifs.is_open())
			lab_ifs.read(&tmpLabel, sizeof(tmpLabel));

		for (int i = 0; i < rows*cols; ++i)
			imgDataf[i] = double(imgData[i]) / 255.0f;

		Mat tData(1, rows*cols, CV_32FC1, imgDataf);
		Mat responses2;
		int tmp = (int)lr2->predict(tData, responses2);
		float* data = (float*)responses2.data;
		if (lab_ifs.is_open())
		{
			int label = tmpLabel;
			if (label == tmp)
				num++;
		}

		Mat ShowData(rows, cols, CV_8UC1, imgData);
		cv::resize(ShowData, ShowData, Size(4 * rows, 4 * cols));
		imshow("data", ShowData);
		cout << tmp << endl;
		waitKey(500);
	}
	printf("precentage of correct :%lf %\n", float(num) / float(count) * 100);
	delete[]imgData;
	delete[]imgDataf;
	ifs.close();
}