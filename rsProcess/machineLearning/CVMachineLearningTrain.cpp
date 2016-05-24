#include"CVMachineLearningTrain.h"
#include"../opencv/opencv2/ml.hpp"
#include "../opencv/opencv2/core/core.hpp"
#include "../opencv/opencv2/highgui/highgui.hpp"
#pragma comment(lib,"opencv_world310d.lib")
#include<fstream>
#include"mnistFile.h"
using namespace std;
using namespace cv;
using namespace ml;

void CVMachineLearningTrain::CV_ANN_BP_TrainMnist(const char* pathMnist, const char* pathLabel, const char* pathNet)
{
	Ptr<ANN_MLP> ann = ANN_MLP::create();
	int layers[3] = { 28 * 28,50,10 };
	Mat_<int> layerSize(1, 3);
	memcpy(layerSize.data, layers, sizeof(int) * 3);
	ann->setLayerSizes(layerSize);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, FLT_EPSILON));
	ann->setTrainMethod(ANN_MLP::BACKPROP, 0.001);

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

	unsigned char* imgData = new unsigned char[count*rows*cols];
	int num = 0;
	while (!ifs.eof())
	{
		ifs.read((char*)imgData + num*rows*cols, rows * cols);
		num++;
	}
	Mat trainData(count, rows*cols, CV_8UC1, imgData);
	ifs.close();

	ifstream lab_ifs(pathLabel, ios_base::binary);
	lab_ifs.read(magicNum, sizeof(magicNum));
	lab_ifs.read(ccount, sizeof(ccount));
	int intmagicNum, ccountint;
	tmp.swapBuffer(magicNum); tmp.swapBuffer(ccount);
	memcpy(&intmagicNum, magicNum, sizeof(magicNum));
	memcpy(&ccountint, ccount, sizeof(ccount));
	unsigned char* label = new unsigned char[ccountint*10];		//number label
	memset(label, 0, sizeof(unsigned char)*ccountint * 10);
	char tmpLabel;
	num = 0;
	while (!lab_ifs.eof())
	{
		lab_ifs.read((char*)&tmpLabel, 1);
		label[10 * num + tmpLabel] = 1;
		num++;
	}
	lab_ifs.close();
	Mat labelsMat(count, 10, CV_8UC1, label);
	Ptr<TrainData> tData = TrainData::create(trainData, ROW_SAMPLE, labelsMat);
	ann->train(tData);
	ann->save(pathNet);
}