#include"CVMachineLearning.h"
#include"..\\AuxiliaryFunction.h"
#include"AdaBoostUtility.h"
#include"Cascadeclassifier.h"
#pragma comment(lib,"opencv_world310d.lib")

#include<fstream>
#include"mnistFile.h"
//==================================================================================================================================
void CVMachineLearningTrain::CV_ANN_BP_Train(const char* pathDataset, const char* pathLabelSet, const char* pathNet, DatasetTypes datasetType)
{
	cv::Ptr<ANN_MLP> ann = ANN_MLP::create();
	int layers[3] = { 28 * 28,15,10 };
	cv::Mat_<int> layerSize(1, 3);
	memcpy(layerSize.data, layers, sizeof(int) * 3);
	ann->setLayerSizes(layerSize);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	ann->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 300, FLT_EPSILON));
	ann->setTrainMethod(ANN_MLP::BACKPROP, 0.001);
	cv::Mat trainMat, labelMat;
	if (datasetType == DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainMat, labelMat);
	cv::Ptr<TrainData> tData = TrainData::create(trainMat, ROW_SAMPLE, labelMat);
	printf("BP Netural Network train~ing...\n");
	ann->train(tData);
	printf("-done\n");
	ann->save(pathNet);
}

void CVMachineLearningTrain::CV_SVM_Train(const char* pathDataset,double C ,const char* pathLabelSet, const char* pathSVM, DatasetTypes datasetType)
{
	cv::Mat trainData;
	cv::Mat labelsMat;
	if(datasetType== DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainData, labelsMat);
	//if(datasetType== IMAGE_LIST)
	//	CV_GetVehicleTrainData()
	float* labelf =(float*)labelsMat.data;
	cv::Mat labelsMatAdapter= cv::Mat::zeros(labelsMat.rows, 1, CV_32SC1);
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

	cv::Ptr<TrainData> tData = TrainData::create(trainData, cv::ml::ROW_SAMPLE, labelsMatAdapter);
	cv::Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::POLY); //SVM::LINEAR;
	svm->setDegree(1);
	svm->setGamma(0.3);
	svm->setCoef0(1.0);
	svm->setNu(1);
	svm->setP(0.5);
	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 10000, 0.01));
	svm->setC(C);
	printf("SVM train~ing...\n");
	svm->train(tData);
	printf("-done\n");
	svm->save(pathSVM);

	///////////////////////////////////////////////////////////
}

void CVMachineLearningTrain::CV_LogisticRegression_Train(const char* pathDataset, const char* pathLabelSet, const char* pathLogisticRegression, DatasetTypes datasetType)
{
	cv::Mat trainData;
	cv::Mat labelsMat;
	if (datasetType == DATASET_MNIST)
		CV_GetMnistTrainData(pathDataset, pathLabelSet, trainData, labelsMat);
	float* labelf = (float*)labelsMat.data;
	cv::Mat labelsMatAdapter = cv::Mat::zeros(labelsMat.rows, 1, CV_32FC1);
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
	cv::Ptr<LogisticRegression> lr1 = LogisticRegression::create();
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

void CVMachineLearningTrain::CV_AdaBoostTrain(const char* pathPosVec, const char* pathNegList, const char* pathAdaboost, int posNum, int negNum)
{
	CvCascadeClassifier classifier;
	int numStages = 18;
	int numThreads = cv::getNumThreads();
	int precalcValBufSize = 1024,
		precalcIdxBufSize = 1024;
	bool baseFormatSave = false;
	double acceptanceRatioBreakValue = -1.0;

	CvCascadeParams cascadeParams;
	cascadeParams.winSize = cv::Size(50,50);
	CvCascadeBoostParams stageParams;
	cv::Ptr<CvFeatureParams> featureParams[] = { cv::makePtr<CvHaarFeatureParams>(),
		cv::makePtr<CvLBPFeatureParams>(),
		cv::makePtr<CvHOGFeatureParams>()
	};
	int fc = sizeof(featureParams) / sizeof(featureParams[0]);
	cv::setNumThreads(numThreads);
	stageParams.maxFalseAlarm = 0.5;
	stageParams.minHitRate = 0.99;
	classifier.train(pathAdaboost,
		pathPosVec,
		pathNegList,
		posNum, negNum,
		precalcValBufSize, precalcIdxBufSize,
		numStages,
		cascadeParams,
		*featureParams[cascadeParams.featureType],
		stageParams,
		baseFormatSave,
		acceptanceRatioBreakValue);
}

//==================================================================================================================================
void CVMachineLearningTrain::CV_GetMnistTrainData(const char* pathMnist, const char* pathLabel, cv::Mat &trianMat, cv::Mat &labelMat)
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
	cv::Mat tmp1(count, rows*cols, CV_32FC1);
	cv::Mat labelsMat(count, 10, CV_32FC1);
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
	lab_ifs.close();
	ifs.close();

	delete[]imgData; imgData = NULL;
	delete[]label; label = NULL;
	delete[]imgDataf; imgDataf = NULL;
}

void CVMachineLearningTrain::CV_GetVehicleTrainData(const char* pathDataset, cv::Mat &trainMat, cv::Mat &label)
{
	ifstream ifs(pathDataset, ios_base::in);
	if (!ifs.is_open())
		return;
	char datasetPos[256], datasetNeg[256];
	ifs.getline(datasetPos, 256);
	ifs.getline(datasetNeg, 256);
	ifs.close();

	//获取图像路径
	vector<string> m_pos_list, m_neg_list;
	GetImageList(datasetPos, m_pos_list);
	GetImageList(datasetNeg, m_neg_list);

	int num_pos = m_pos_list.size();
	int num_neg = m_neg_list.size();
	cv::HOGDescriptor hog(cv::Size(64, 64), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
	cv::Mat img = cv::imread(m_pos_list[0].c_str());
	if (img.data == NULL)
		return;
	vector<float> featureVec;
	hog.compute(img, featureVec, cv::Size(8, 8));
	int featureSize = featureVec.size();

	float* trainData = new float[(num_pos+num_neg)*featureSize];
	float* labelData = new float[(num_pos+num_neg)*2];
	memset(trainData, 0, sizeof(float)*(num_pos+num_neg)*featureSize);
	memset(labelData, 0, sizeof(float)*(num_pos+num_neg)*2);
	
	printf("处理正样本....\n");
	for (int i = 0; i < num_pos; ++i)
	{
		img = cv::imread(m_pos_list[i].c_str());
		if (img.data == NULL)
			return;
		featureVec.clear();
		hog.compute(img, featureVec, cv::Size(8, 8));
		for (int j = 0; j < featureSize; ++j)
			trainData[i*featureSize + j] = featureVec[j];
		labelData[2 * i + 0] = 1;
	}

	printf("处理负样本...\n");
	for (int i = 0; i < num_neg; ++i)
	{
		img = cv::imread(m_neg_list[i].c_str());
		if (img.data == NULL)
			return;
		featureVec.clear();
		hog.compute(img, featureVec, cv::Size(8, 8));
		for (int j = 0; j < featureSize; ++j)
			trainData[(i+num_pos)*featureSize + j] = featureVec[j];
		labelData[2 * (i+ num_pos) + 1] = 1;
	}
	
	cv::Mat tmpTrainMat((num_pos + num_neg), featureSize, CV_32FC1, trainData);
	cv::Mat tmpLabelMat((num_pos + num_neg), 2, CV_32FC1, labelData);
	delete[]trainData; delete[]labelData;
	trainMat = tmpTrainMat.clone();
	label    = tmpLabelMat.clone();
}

void CVMachineLearningTrain::CV_HaarSampleDescriptor(const char* pathPosSampleList, const char* pathOutVec, int width, int height, int num)
{
	printf("Create training samples from single image applying distortions...\n");
	int bgthreshold = 80;	//奇怪的数需要调整
	int bgcolor = 0;
	int maxintensitydev = 40;
	int invert = 0;
	double maxxangle = 1.1;
	double maxyangle = 1.1;
	double maxzangle = 0.5;
	int showsamples = 0;
	/* the samples are adjusted to this scale in the sample preview window */
	double scale = 4.0;
	vector<string> m_list;
	GetImageList(pathPosSampleList, m_list);
	int total = cvCreateTrainingSamplesFromInfo(pathPosSampleList, pathOutVec, num, showsamples,width, height);
	printf("%d Done\n",total);
}

//==================================================================================================================================
void CVMachineLearningPredict::CV_ANN_BP_PredictMnist(const char* pathPredict, const char* pathNet, const char* pathLabel/* = NULL*/)
{
	cv::Ptr<ANN_MLP> ann = ANN_MLP::create();
	ann = cv::Algorithm::load<ANN_MLP>(pathNet);
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
		cv::Mat tData(1, rows*cols, CV_32FC1, imgDataf);
		cv::Mat pData;// = Mat::zeros(1, 10, CV_32FC1);
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

		cv::Mat ShowData(rows, cols, CV_8UC1, imgData);
		cv::imshow("data", ShowData);
		printf("predict number:%d\n", idx);
		cv::waitKey(500);
	}
	printf("precentage of correct :%lf %\n", float(num) / float(count) * 100);
	delete[]imgData;
	delete[]imgDataf;
	ifs.close();
}

void CVMachineLearningPredict::CV_SVM_PredictMnist(const char* pathPredict, const char* pathSVM, const char* pathLabel)
{
	cv::Ptr<SVM> svm= cv::Algorithm::load<SVM>(pathSVM);
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

		cv::Mat tData(1, rows*cols, CV_32FC1, imgDataf);
		int tmp=(int)svm->predict(tData);

		if (lab_ifs.is_open())
		{
			int label = tmpLabel;
			if (label == tmp)
				num++;
		}

		cv::Mat ShowData(rows, cols, CV_8UC1, imgData);
		cv::resize(ShowData, ShowData, cv::Size(4* rows, 4 * cols));
		cv::imshow("data", ShowData);
		cout << tmp << endl;
		cv::waitKey(500);
	}
	printf("precentage of correct :%lf %\n",float(num)/float(count)*100);
	delete[]imgData;
	delete[]imgDataf;
	ifs.close();
}

void CVMachineLearningPredict::CV_LogisticRegression_PredictMnist(const char* pathPredict, const char* pathLogisticRegression, const char* pathLabel)
{
	cv::Ptr<LogisticRegression> lr2 = cv::Algorithm::load<LogisticRegression>(pathLogisticRegression);
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

		cv::Mat tData(1, rows*cols, CV_32FC1, imgDataf);
		cv::Mat responses2;
		int tmp = (int)lr2->predict(tData, responses2);
		float* data = (float*)responses2.data;
		if (lab_ifs.is_open())
		{
			int label = tmpLabel;
			if (label == tmp)
				num++;
		}

		cv::Mat ShowData(rows, cols, CV_8UC1, imgData);
		cv::resize(ShowData, ShowData, cv::Size(4 * rows, 4 * cols));
		cv::imshow("data", ShowData);
		cout << tmp << endl;
		cv::waitKey(500);
	}
	printf("precentage of correct :%lf %\n", float(num) / float(count) * 100);
	delete[]imgData;
	delete[]imgDataf;
	ifs.close();
}

void CVMachineLearningPredict::CV_SVM_PredictVehicle(const char* pathPredictImg, const char* pathSVM)
{
	cv::Ptr<SVM> svm = cv::Algorithm::load<SVM>(pathSVM);
	cv::Mat supportVector = svm->getSupportVectors();//

				  //获取alpha和rho
	cv::Mat alpha;//每个支持向量对应的参数α(拉格朗日乘子)，默认alpha是float64的
	cv::Mat svIndex;//支持向量所在的索引
	float rho = svm->getDecisionFunction(0, alpha, svIndex);

	//转换类型:这里一定要注意，需要转换为32的
	cv::Mat alpha2;
	alpha.convertTo(alpha2, CV_32FC1);
	cv::Mat result = alpha2*supportVector;
	int num = result.rows;
	vector<float> m_hog_classifier;
	for (int i = 0; i < num; ++i)
	{
		float t=result.at<float>(0, i) *= -1;
		m_hog_classifier.push_back(t);
	}
	cv::Mat img = cv::imread(pathPredictImg, ios_base::in);
	vector<cv::Rect> detectRect,detectFliterRect;
	cv::HOGDescriptor hog(cv::Size(64, 64), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
	hog.setSVMDetector(m_hog_classifier);
	hog.detectMultiScale(img, detectRect, 0, cv::Size(8, 8), cv::Size(64, 64), 1.05, 2);


	for (int i = 0; i < detectRect.size(); i++)
	{
		cv::Rect r = detectRect[i];
		int j;
		for (j = 0; j < detectRect.size(); j++)
			if (j != i && (r & detectRect[j]) == r)
				break;
		if (j == detectRect.size())
			detectFliterRect.push_back(r);
	}

	// 适当缩小矩形
	for (int i = 0; i < detectFliterRect.size(); i++)
	{
		cv::Rect r = detectFliterRect[i];
		// the HOG detector returns slightly larger rectangles than the real objects.
		// so we slightly shrink the rectangles to get a nicer output.
		r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);
		rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
	}

	imshow("people detector", img);
	cv::waitKey(0);
}