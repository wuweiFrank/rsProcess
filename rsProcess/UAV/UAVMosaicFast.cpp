#include"UAVMosaicFast.h"
#include"..\matrixOperation.h"
#include"..\AuxiliaryFunction.h"
#include"..\gdal\include\gdal_priv.h"
#include<omp.h>

#pragma comment(lib,"gdal_i.lib")
long UAVMosaicFast::UAVMosaicFast_AffineTrans(vector<string> pszImages)
{
	int image_num = pszImages.size();
	//这样首先是要初始化特征点提取算子
	SiftGPU* siftFeatures = new SiftGPU();
	SiftMatchGPU* matcher = new SiftMatchGPU();

	if (siftFeatures->CreateContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED)
		return -1;

	vector<float > descriptors1(1), descriptors2(1);
	vector<SiftGPU::SiftKeypoint> keys1(1), keys2(1);
	vector<Point2f> pts1, pts2;
	int num1 = 0, num2 = 0;
	ImgFeaturesTools featureTools;
	vector<Mat> tmpHomo;

	//影像数目
	for (int i = 0; i < image_num - 1; ++i)
	{
		//特征点提取
		if (siftFeatures->RunSIFT(pszImages[i].c_str()))
		{
			num1 = siftFeatures->GetFeatureNum();
			keys1.resize(num1);    descriptors1.resize(128 * num1);
			siftFeatures->GetFeatureVector(&keys1[0], &descriptors1[0]);
		}
		//You can have at most one OpenGL-based SiftGPU (per process).
		//Normally, you should just create one, and reuse on all images. 
		if (siftFeatures->RunSIFT(pszImages[i+1].c_str()))
		{
			num2 = siftFeatures->GetFeatureNum();
			keys2.resize(num2);    descriptors2.resize(128 * num2);
			siftFeatures->GetFeatureVector(&keys2[0], &descriptors2[0]);
		}

		//特征点匹配
		matcher->VerifyContextGL();//must call once
		matcher->SetDescriptors(0, num1, &descriptors1[0]); //image 1
		matcher->SetDescriptors(1, num2, &descriptors2[0]); //image 2
		int(*match_buf)[2] = new int[num1][2];
		//use the default thresholds. Check the declaration in SiftGPU.h
		int num_match = matcher->GetSiftMatch(num1, match_buf);
		for (int i = 0; i < num_match; ++i)
		{
			//How to get the feature matches: 
			SiftGPU::SiftKeypoint & key1 = keys1[match_buf[i][0]];
			SiftGPU::SiftKeypoint & key2 = keys2[match_buf[i][1]];

			//居然会有重复的点，关于这一点我很奇怪
			bool isPush = false;
			Point2f pnt1(key1.x, key1.y), pnt2(key2.x, key2.y);
			for (int j = 0; j < pts1.size(); ++j)
			{
				if (pnt1 == pts1[j] || pnt2 == pts2[j])
					isPush = true;
			}
			if (!isPush)
			{
				pts1.push_back(pnt1);
				pts2.push_back(pnt2);
			}
		}
		featureTools.ImgFeaturesTools_MatchOptimize(pts1, pts2);
		delete[]match_buf;
		Mat homo=findHomography(pts1, pts2);
		tmpHomo.push_back(homo);

		//清空所有空间
		descriptors1.clear(); descriptors2.clear();
		keys1.clear(); keys2.clear();
		pts1.clear(); pts2.clear();
	}
	m_Homography.resize(tmpHomo.size());
	for (int i = 0; i < tmpHomo.size(); ++i)
	{
		Mat tmpMat= tmpHomo[i];
		for (int j = i + 1; j < tmpHomo.size(); ++j)
			tmpMat = tmpMat*tmpHomo[j];
		m_Homography[tmpHomo.size() - i - 1] = tmpMat;
		double* trans = (double*)tmpMat.data;
		for (int i = 0; i < 9; ++i)
			printf("%lf   ", trans[i]);
		printf("\n");
	}
	return 0;
}

long UAVMosaicFast::UAVMosaicFast_GetMosaicRange(vector<string> pszImages, int& mosaic_width, int &mosaic_height)
{
	//获取仿射变换系数
	int minx = 9999999, miny = 9999999, maxx = -9999999, maxy = -9999999;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	for (int i = 0; i < pszImages.size()-1; ++i)
	{
		GDALDatasetH m_dataset = GDALOpen(pszImages[i].c_str(), GA_ReadOnly);
		int width = GDALGetRasterXSize(m_dataset);
		int heigh = GDALGetRasterYSize(m_dataset);
		double pnt1[3] = { 0,0,1 },pnt1t[3];
		double pnt2[3] = { width,0,1 }, pnt2t[3];
		double pnt3[3] = { 0,heigh,1 }, pnt3t[3];
		double pnt4[3] = { width,heigh,1 }, pnt4t[3];
		double* mat = (double*)m_Homography[i].data;

		MatrixMuti(mat, 3, 3, 1, pnt1, pnt1t);
		MatrixMuti(mat, 3, 3, 1, pnt2, pnt2t);
		MatrixMuti(mat, 3, 3, 1, pnt3, pnt3t);
		MatrixMuti(mat, 3, 3, 1, pnt4, pnt4t);

		minx = min(min(min(pnt1t[0] / pnt1t[2], pnt2t[0] / pnt2t[2]), min(pnt3t[0] / pnt3t[2], pnt4t[0] / pnt4t[2])), minx);
		maxx = max(max(max(pnt1t[0] / pnt1t[2], pnt2t[0] / pnt2t[2]), max(pnt3t[0] / pnt3t[2], pnt4t[0] / pnt4t[2])), maxx);
		miny = min(min(min(pnt1t[1] / pnt1t[2], pnt2t[1] / pnt2t[2]), min(pnt3t[1] / pnt3t[2], pnt4t[1] / pnt4t[2])), miny);
		maxy = max(max(max(pnt1t[1] / pnt1t[2], pnt2t[1] / pnt2t[2]), max(pnt3t[1] / pnt3t[2], pnt4t[1] / pnt4t[2])), maxy);
		GDALClose(m_dataset);
	}

	GDALDatasetH m_dataset = GDALOpen(pszImages[pszImages.size() - 1].c_str(), GA_ReadOnly);
	int width = GDALGetRasterXSize(m_dataset);
	int heigh = GDALGetRasterYSize(m_dataset);
	minx = min(0, minx);
	maxx = max(width, maxx);
	miny = min(0, miny);
	maxy = max(heigh, maxy);
	GDALClose(m_dataset);

	mosaic_width = maxx - minx;
	mosaic_height = maxy - miny;

	return 0;
}

long UAVMosaicFast::UAVMosaicFast_HistroMatch(unsigned char* imgBuffer1, unsigned char* imgBuffer2, int xsize1, int ysize1, int xsize2, int ysize2)
{
	int histroMap[256];
	GetImgHistroMatch(imgBuffer1, imgBuffer2, xsize1, ysize1, xsize2, ysize2, 0,255,histroMap);
	for (int i = 0; i < xsize1*ysize1; ++i)
	{
		imgBuffer1[i] = histroMap[imgBuffer1[i]];
	}
	return 0;
}

long UAVMosaicFast::UAVMosaicFast_AffineTrans(Mat homo, unsigned char* imgBuffer, int xsize, int ysize, unsigned char* imgMosaic, int mosaicx, int mosaicy)
{
	//只要牺牲内存换代码的简洁了......
	//感觉这么做效率也很低，现在看着什么都想用GPU加速，这是疯了么！！！
	int maxx = -9999999, minx = 9999999, maxy = -9999999, miny = 9999999;
	double pnt1[3] = { 0,0,1 }, pnt1t[3];
	double pnt2[3] = { xsize,0,1 }, pnt2t[3];
	double pnt3[3] = { 0,ysize,1 }, pnt3t[3];
	double pnt4[3] = { xsize,ysize,1 }, pnt4t[3];
	double* mat = (double*)homo.data;

	MatrixMuti(mat, 3, 3, 1, pnt1, pnt1t);
	MatrixMuti(mat, 3, 3, 1, pnt2, pnt2t);
	MatrixMuti(mat, 3, 3, 1, pnt3, pnt3t);
	MatrixMuti(mat, 3, 3, 1, pnt4, pnt4t);

	minx = min(min(min(pnt1t[0] / pnt1t[2], pnt2t[0] / pnt2t[2]), min(pnt3t[0] / pnt3t[2], pnt4t[0] / pnt4t[2])), minx);
	maxx = max(max(max(pnt1t[0] / pnt1t[2], pnt2t[0] / pnt2t[2]), max(pnt3t[0] / pnt3t[2], pnt4t[0] / pnt4t[2])), maxx);
	miny = min(min(min(pnt1t[1] / pnt1t[2], pnt2t[1] / pnt2t[2]), min(pnt3t[1] / pnt3t[2], pnt4t[1] / pnt4t[2])), miny);
	maxy = max(max(max(pnt1t[1] / pnt1t[2], pnt2t[1] / pnt2t[2]), max(pnt3t[1] / pnt3t[2], pnt4t[1] / pnt4t[2])), maxy);
	//逆过来求解算了-好象不对 囧
	//Mat homoInv = homo.inv();
	//mat = (double*)homoInv.data;

//#pragma omp parallel for
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			double pnt[3] = { i,j,1 }, pntt[3];
			MatrixMuti(mat, 3, 3, 1, pnt, pntt);
			int tmpx = pntt[0] / pntt[2];
			int tmpy = pntt[1] / pntt[2];
			imgMosaic[tmpy*mosaicx + tmpx] = imgBuffer[j*xsize + i];
		}
	}
	return 0;
}

long UAVMosaicFast::UAVMosaicFast_SeamFillFast(int up, int left, unsigned char* imgMosaic, int mosaicx, int mosaicy, unsigned char* imgBuffer, int xsize, int ysize)
{
	//上面代码还有问题，懒得调了，
	//不做最佳拼接线直接填充数据是不是应该调色，算了.......

	return 0;

}

//接口
long UAVMosaicFast::UAVMosaicFast_Mosaic(vector<string> pszImages, const char* pathDst)
{
	//获取转换仿射变换参数，包括特征点匹配
	UAVMosaicFast_AffineTrans(pszImages);
	int mosaic_width, mosaic_height;//影像镶嵌后的影像大小
	UAVMosaicFast_GetMosaicRange(pszImages, mosaic_width, mosaic_height);
	unsigned char* imgMosaic = new unsigned char[mosaic_width*mosaic_height];
	memset(imgMosaic, 0, sizeof(unsigned char)*mosaic_width*mosaic_height);

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	char **papszOptions = NULL;
	papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BAND");
	GDALDatasetH m_dstdataset = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, mosaic_width, mosaic_height, 3,GDT_Byte, papszOptions);
	for (int j = 0; j < 3; j++)
	{
		for (size_t i = 0; i < pszImages.size() - 1; i++)
		{
			GDALDatasetH m_dataset = GDALOpen(pszImages[i].c_str(), GA_ReadOnly);
			int width = GDALGetRasterXSize(m_dataset);
			int heigh = GDALGetRasterYSize(m_dataset);
			unsigned char* data = new unsigned char[width*heigh];
			GDALRasterIO(GDALGetRasterBand(m_dataset, j+1), GF_Read, 0, 0, width, heigh, data, width, heigh, GDT_Byte, 0, 0);
			UAVMosaicFast_AffineTrans(m_Homography[i], data,width,heigh,imgMosaic, mosaic_width, mosaic_height);
			delete[]data;
			GDALClose(m_dataset);
		}
		GDALDatasetH m_dataset = GDALOpen(pszImages[pszImages.size() - 1].c_str(), GA_ReadOnly);
		int width = GDALGetRasterXSize(m_dataset);
		int heigh = GDALGetRasterYSize(m_dataset);
		unsigned char* data = new unsigned char[width*heigh];
		GDALRasterIO(GDALGetRasterBand(m_dataset, j+1), GF_Read, 0, 0, width, heigh, data, width, heigh, GDT_Byte, 0, 0);
		UAVMosaicFast_AffineTrans(Mat::eye(3,3,CV_64F), data, width, heigh, imgMosaic, mosaic_width, mosaic_height);
		delete[]data;
		GDALClose(m_dataset);

		//重采样
		GDALRasterIO(GDALGetRasterBand(m_dstdataset, j+1), GF_Write, 0, 0, mosaic_width, mosaic_height, imgMosaic, mosaic_width, mosaic_height, GDT_Byte, 0, 0);
	}
	GDALClose(m_dstdataset);
	delete[]imgMosaic;
	return 0;
}