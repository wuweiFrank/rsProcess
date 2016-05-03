#include"UAVMosaicFast.h"
#include"..\matrixOperation.h"
#include"..\AuxiliaryFunction.h"
#include"..\gdal\include\gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

long UAVMosaicFast::UAVMosaicFast_AffineTrans(vector<string> pszImages)
{
	ImgFeaturesTools m_featureMatch;
	bool* matchpairs = new bool[pszImages.size()*pszImages.size()];
	memset(matchpairs, 0, sizeof(bool)*pszImages.size()*pszImages.size());
	for (int i = 0; i < pszImages.size() - 1; ++i)
		matchpairs[i*pszImages.size() + i + 1] = true;
	vector<vector<Point2f>> m_matches;
	m_featureMatch.ImgFeaturesTools_ExtracMatches(pszImages, m_matches, matchpairs, "AKAZE", "BruteForce-Hamming");

	//仿射变换系数
	vector<adfAffineTrans> affineTransParameters;
	adfAffineTrans transparameters;
	for (int i = 0; i < m_matches.size() / 2; ++i)
	{
		Mat warp_mat;
		//以后一张图像为标准
		Point2f *point1 = new Point2f[m_matches[2 * i + 0].size()];
		Point2f *point2 = new Point2f[m_matches[2 * i + 0].size()];
		for (int j = 0; j < m_matches[2 * i + 0].size(); ++j)
		{
			point1[j] = m_matches[2 * i + 0][j];
			point2[j] = m_matches[2 * i + 1][j];
		}

		warp_mat = getAffineTransform(point1, point2);
		//cout << warp_mat << endl;
		transparameters.m_affineTransParameters[0] = warp_mat.at<double>(0, 0);
		transparameters.m_affineTransParameters[1] = warp_mat.at<double>(0, 1);
		transparameters.m_affineTransParameters[2] = warp_mat.at<double>(0, 2);
		transparameters.m_affineTransParameters[3] = warp_mat.at<double>(1, 0);
		transparameters.m_affineTransParameters[4] = warp_mat.at<double>(1, 1);
		transparameters.m_affineTransParameters[5] = warp_mat.at<double>(1, 2);
		affineTransParameters.push_back(transparameters);
		delete[]point1;
		delete[]point2;
	}

	//连续仿射变换系数
	for (int i = affineTransParameters.size() - 1; i >=0 ; i--)
	{
		double continueAffine[6];
		memcpy(continueAffine, affineTransParameters[i].m_affineTransParameters, sizeof(double) * 6);

		double pnt1[4] = { continueAffine[0],continueAffine[1],continueAffine[3],continueAffine[4] };
		double pnt2[4];
		for (int j = i-1; j >= 0; j--)
		{
			double pnt3[4] = { affineTransParameters[j].m_affineTransParameters[0] ,affineTransParameters[j].m_affineTransParameters[1],
								affineTransParameters[j].m_affineTransParameters[3],affineTransParameters[j].m_affineTransParameters[4] };
			MatrixMuti(pnt1, 2, 2, 2, pnt3, pnt2);
			memcpy(pnt1, pnt2, sizeof(double) * 4);
			continueAffine[2] += affineTransParameters[j].m_affineTransParameters[2];
			continueAffine[5] += affineTransParameters[j].m_affineTransParameters[5];
		}
		continueAffine[0] = pnt1[0];		continueAffine[1] = pnt1[1];
		continueAffine[3] = pnt1[2];		continueAffine[4] = pnt1[3];
		memcpy(transparameters.m_affineTransParameters, continueAffine, sizeof(double) * 6);
		m_affineTransParameters.push_back(transparameters);
	}
	delete[]matchpairs;
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
		double pnt1[3] = { 0,0,1 },pnt1t[2];
		double pnt2[3] = { width,0,1 }, pnt2t[2];
		double pnt3[3] = { 0,heigh,1 }, pnt3t[2];
		double pnt4[3] = { width,heigh,1 }, pnt4t[2];
		MatrixMuti(m_affineTransParameters[i].m_affineTransParameters, 2, 3, 1, pnt1, pnt1t);
		MatrixMuti(m_affineTransParameters[i].m_affineTransParameters, 2, 3, 1, pnt2, pnt2t);
		MatrixMuti(m_affineTransParameters[i].m_affineTransParameters, 2, 3, 1, pnt3, pnt3t);
		MatrixMuti(m_affineTransParameters[i].m_affineTransParameters, 2, 3, 1, pnt4, pnt4t);
		minx = min(min(min(pnt1t[0], pnt2t[0]), min(pnt3t[0], pnt4t[0])), minx);
		maxx = max(max(max(pnt1t[0], pnt2t[0]), max(pnt3t[0], pnt4t[0])), maxx);
		miny = min(min(min(pnt1t[1], pnt2t[1]), min(pnt3t[1], pnt4t[1])), miny);
		maxy = max(max(max(pnt1t[1], pnt2t[1]), max(pnt3t[1], pnt4t[1])), maxy);
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

long UAVMosaicFast::UAVMosaicFast_AffineTrans(adfAffineTrans& affineTransParam, unsigned char* imgBuffer, int xsize, int ysize, unsigned char* imgMosaic, int mosaicx, int mosaicy)
{
	//只要牺牲内存换代码的简洁了......
	DPOINT *pPositions = new DPOINT[xsize*ysize];
	memset(pPositions, 0, sizeof(DPOINT)*xsize*ysize);
	int maxx = -9999999, minx = 9999999, maxy = -9999999, miny = 9999999;
	for (size_t i = 0; i < xsize; i++)
	{
		for (size_t j = 0; j < ysize; j++)
		{
			int tmpx = (int)(affineTransParam.m_affineTransParameters[0] * i + affineTransParam.m_affineTransParameters[1] * j + affineTransParam.m_affineTransParameters[2]);
			int tmpy = (int)(affineTransParam.m_affineTransParameters[3] * i + affineTransParam.m_affineTransParameters[4] * j + affineTransParam.m_affineTransParameters[5]);
			pPositions[j*xsize + i].dX = tmpx;
			pPositions[j*xsize + i].dY = tmpy;

			maxx = max(tmpx, maxx);
			minx = min(tmpx, minx);
			maxy = max(tmpy, maxy);
			miny = min(tmpy, miny);
		}
	}
	int transx = maxx - minx;
	int transy = maxy - miny;
	unsigned char* transImgBuffer = new unsigned char[transx*transy];
	GetImgSample(imgBuffer, pPositions, xsize, ysize, transx, transy, transImgBuffer);
	UAVMosaicFast_SeamFillFast(miny, minx, imgMosaic, mosaicx, mosaicy, transImgBuffer, transx, transy);
	delete[]pPositions;
	delete[]transImgBuffer;
	return 0;
}

long UAVMosaicFast::UAVMosaicFast_SeamFillFast(int up, int left, unsigned char* imgMosaic, int mosaicx, int mosaicy, unsigned char* imgBuffer, int xsize, int ysize)
{
	vector<DPOINT> edge;
	int maxrelation = -1.1; int posx = 0, posy = 0;
	for (int j = 0; j < ysize; ++j)
	{
		if (posx == 0)
		{
			for (int i = 0; i < xsize - 5; ++i)
			{
				//获取一行的5个像素
				float data1[5], data2[5];
				for (int k = 0; k < 5; ++k)
				{
					data1[k] = imgMosaic[(j + up)*mosaicx + i + left + k];
					data2[k] = imgBuffer[j*xsize + i + k];
				}
				//如果存在像素为0则跳出
				bool isBreak = false;
				for (int k = 0; k < 5; ++k)
				{
					if (data1[k] == 0 || data2[k] == 0)
					{
						isBreak = true;
						break;
					}
				}
				if (isBreak)
					continue;

				//求取这一行5个像素的相关性
				double corr = GetCoefficient(data1, data2, 5);
				maxrelation = max(corr, maxrelation);
				if (maxrelation == corr)
					posx++;
			}
			DPOINT tmp; tmp.dX = posx; tmp.dY = posy;
			edge.push_back(tmp);
		}
		else  //在前后5个相邻位置中选取
		{
			int startxpos = posx - 5 > 0 ? posx - 5 : 0;
			int endxpos = startxpos + 15 < xsize ? startxpos + 15 : xsize;
			posx = startxpos;
			for (int i = startxpos; i < endxpos - 5; ++i)
			{
				//获取一行的5个像素
				float data1[5], data2[5];
				for (int k = 0; k < 5; ++k)
				{
					data1[k] = imgMosaic[(j + up)*mosaicx + i + left + k];
					data2[k] = imgBuffer[j*xsize + i + k];
				}
				//如果存在像素为0则跳出
				bool isBreak = false;
				for (int k = 0; k < 5; ++k)
				{
					if (data1[k] == 0 || data2[k] == 0)
					{
						isBreak = true;
						break;
					}
				}
				if (isBreak)
					continue;

				//求取这一行5个像素的相关性
				double corr = GetCoefficient(data1, data2, 5);
				maxrelation = max(corr, maxrelation);
				if (maxrelation == corr)
					posx++;
			}
			DPOINT tmp; tmp.dX = posx; tmp.dY = posy;
			edge.push_back(tmp);
		}
	}
	//根据拼接边填数据

	for (size_t j = 0; j < ysize; j++)
	{
		if(edge[j].dX==0)
		for (size_t i = 0; i < xsize; i++)
			imgMosaic[(j + up)*mosaicx + i + left] = imgBuffer[j*xsize + i];
		else
		{
			for (size_t i = edge[j].dX; i < xsize; i++)
				imgMosaic[(j + up)*mosaicx + i + left] = imgBuffer[j*xsize + i];
		}
	}


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
			UAVMosaicFast_AffineTrans(m_affineTransParameters[i], data,width,heigh,imgMosaic, mosaic_width, mosaic_height);
			delete[]data;
			GDALClose(m_dataset);
		}
		GDALDatasetH m_dataset = GDALOpen(pszImages[pszImages.size() - 1].c_str(), GA_ReadOnly);
		int width = GDALGetRasterXSize(m_dataset);
		int heigh = GDALGetRasterYSize(m_dataset);
		unsigned char* data = new unsigned char[width*heigh];
		GDALRasterIO(GDALGetRasterBand(m_dataset, j+1), GF_Read, 0, 0, width, heigh, data, width, heigh, GDT_Byte, 0, 0);
		adfAffineTrans tmp;
		memset(&tmp, 0, sizeof(adfAffineTrans));
		tmp.m_affineTransParameters[0] = 1;
		tmp.m_affineTransParameters[4] = 1;
		UAVMosaicFast_AffineTrans(tmp, data, width, heigh, imgMosaic, mosaic_width, mosaic_height);
		delete[]data;
		GDALClose(m_dataset);

		//重采样
		GDALRasterIO(GDALGetRasterBand(m_dstdataset, j+1), GF_Write, 0, 0, mosaic_width, mosaic_height, imgMosaic, mosaic_width, mosaic_height, GDT_Byte, 0, 0);
	}


	delete[]imgMosaic;
	return 0;
}