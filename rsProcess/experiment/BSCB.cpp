#include"BSCB.h"
#include"..\gdal\include\gdal_priv.h"
#include"..\AuxiliaryFunction.h"
using namespace std;
#pragma comment(lib,"gdal_i.lib")

void BSCB::BSCBImageInpaintingProcess(const char* pathImgIn, const char* pathImgMask, const char* pathImgRepair)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImgIn, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathImgMask, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetMsk);

	//数据申请
	float* imgData = new float[xsize*ysize];
	float* mskData = new float[xsize*ysize];
	float* rpaData = new float[xsize*ysize];
	float* pocData = new float[xsize*ysize];

	// 不知道要设置多少才合适
	double deltaT = 0.1;
	int    IteratorN = 0;	//第N次迭代
	
	//获取像素值
	GDALRasterIO(GDALGetRasterBand(m_datasetImg, 1), GF_Read, 0, 0, xsize, ysize, imgData, xsize, ysize, GDT_Float32, 0, 0);
	GDALRasterIO(GDALGetRasterBand(m_datasetMsk, 1), GF_Read, 0, 0, xsize, ysize, mskData, xsize, ysize, GDT_Float32, 0, 0);
	InterpolationInit(imgData, mskData, xsize, ysize);
	for (int i = 0; i < xsize*ysize; ++i)
		imgData[i] = float(imgData[i]) / 255.0f;
	memcpy(rpaData, imgData, sizeof(float)*xsize*ysize);
	memcpy(pocData, imgData, sizeof(float)*xsize*ysize);

	// 先进行扩散
	for (int i = 1; i < xsize-1; ++i)
	{
		for (int j = 1; j < ysize-1; ++j)
		{
			if (mskData[j*xsize + i] == -1)
				DiffuseLoop(pocData, imgData,i,j,xsize,ysize);
		}
	}

	do
	{
		memcpy( rpaData, pocData, sizeof(float)*xsize*ysize);
		//修补和扩散交替进行
		for (int eachLoop = 0; eachLoop < 15; ++eachLoop)
		{
			//修复
			for (int i = 1; i < xsize - 1; ++i)
			{
				for (int j = 1; j < ysize - 1; ++j)
				{
					if (mskData[j*xsize + i] == -1)
						RepaireLoop(pocData, rpaData, deltaT, i, j, xsize, ysize);
				}
			}
		}
		memcpy(rpaData, pocData, sizeof(float)*xsize*ysize);
		for (int eachLoop = 0; eachLoop < 2; ++eachLoop)
		{
			//扩散
			for (int i = 1; i < xsize - 1; ++i)
			{
				for (int j = 1; j < ysize - 1; ++j)
				{
					if (mskData[j*xsize + i] == -1)
						DiffuseLoop(pocData, rpaData, i, j, xsize, ysize);
				}
			}
		}
		IteratorN++;
		printf("iterator number :%d\r", IteratorN);
	} while (TerminateBSCBCondition(pocData, rpaData, mskData,xsize,ysize)&& IteratorN<100);

	GDALDatasetH m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathImgRepair, xsize, ysize, 1, GDT_Float32, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetDst, 1), GF_Write, 0, 0, xsize, ysize, pocData, xsize, ysize, GDT_Float32, 0, 0);

	GDALClose(m_datasetDst);
	GDALClose(m_datasetImg);
	GDALClose(m_datasetMsk);


	delete[]imgData;
	delete[]mskData;
	delete[]rpaData;
	delete[]pocData;
}

void BSCB::DiffuseLoop(float* data, float *img, int x, int y, int xsize, int ysize)
{
	double Ix = (0.5*(img[(x + 1) + y*xsize] - img[(x - 1) + y*xsize]));
	double Iy = (0.5*(img[x + (y + 1)*xsize] - img[x + (y - 1)*xsize]));
	double Ixx = (img[(x + 1) + y*xsize] + img[(x - 1) + y*xsize] - 2 * img[x + y*xsize]);
	double Iyy = (img[x + (y + 1)*xsize] + img[x + (y - 1)*xsize] - 2 * img[x + y*xsize]);
	double Ixy = (0.5*(0.5*(img[(x + 1) + (y + 1)*xsize] + img[(x - 1) + (y + 1)*xsize]) - 0.5*(img[(x + 1) + (y - 1)*xsize] + img[(x - 1) + (y - 1)*xsize])));
	double normN2 = Ix*Ix + Iy*Iy;
	if (normN2 == 0)
		data[y*xsize + x] = 0;
	else
		data[y*xsize + x] = img[y*xsize+x]+0.1*(Ixx*(Iy*Iy) + Iyy*(Ix*Ix) - 2 * Ix*Iy*Ixy) / (normN2);
}

void BSCB::RepaireLoop(float* data, float* img, double delta, int x, int y, int xsize, int ysize)
{
	double iteratorIt = RepairChange(img, x, y, xsize, ysize);
	data[y*xsize + x] = img[y*xsize + x] + delta*iteratorIt;
 }

double BSCB::RepairChange(float* img, int x, int y, int xsize, int ysize)
{
	double laplacex = 0.5*(MinAvg(MaxAvg(img[x + 2 + y*xsize], img[x + 1 + y*xsize], img[x + y*xsize]),
	max(img[x + 1 + (y + 1)*xsize], img[x + 1 + y*xsize]),
	max(img[x + 1 + (y - 1)*xsize], img[x + 1 + y*xsize]) - MinAvg(MaxAvg(img[x - 2 + y*xsize], img[x - 1 + y*xsize], img[x + y*xsize]), max(img[x - 1 + (y + 1)*xsize], img[x - 1 + y*xsize]), max(img[x - 1 + (y - 1)*xsize], img[x - 1 + y*xsize]))));
	double laplacey = 0.5*(MinAvg(MaxAvg(img[x + (y+2)*xsize], img[x  + (y + 1)*xsize], img[x + y*xsize]),
	max(img[x + 1 + (y + 1)*xsize], img[x + (y+1)*xsize]),
	max(img[x - 1 + (y + 1)*xsize], img[x + 1 + y*xsize]) - MinAvg(MaxAvg(img[x + (y - 2 )*xsize], img[x + (y - 1)*xsize], img[x + y*xsize]), max(img[x + 1 + (y - 1)*xsize], img[x - 1 + y*xsize]), max(img[x - 1 + (y - 1)*xsize], img[x - 1 + y*xsize]))));
	
	double directionx = 0, directiony = 0;
	double Ix = (0.5*(img[x + 1 + y*xsize] - img[x - 1 + y*xsize]));
	double Iy = (0.5*(img[x + (y + 1)*xsize] - img[x + (y - 1)*xsize]));
	if (Ix == 0 && Iy == 0)
	{
		directionx = directiony = 0;
	}
	else if (Ix == 0)
	{
		directionx = -Iy / sqrt(Ix*Ix + Iy*Iy);
		directiony = 0;
	}
	else if (Iy == 0)
	{
		directionx = 0;
		directiony = Ix / sqrt(Ix*Ix + Iy*Iy);
	}
	else
	{
		directionx = -Iy / sqrt(Ix*Ix + Iy*Iy);
		directiony = Ix / sqrt(Ix*Ix + Iy*Iy);
	}
	double x1 = laplacex, x2 = directionx, y1 = laplacey, y2 = directiony;

	double Ixhm = min(img[x+y*xsize] - img[x-1+y*xsize], 0);
	double Iyhm = min(img[x + y*xsize] - img[x + (y-1)*xsize], 0);
	double Ixqm = min(img[x+1 + y*xsize] - img[x + y*xsize], 0);
	double Iyqm = min(img[x + (y+1)*xsize] - img[x + y*xsize], 0);
	double IxhM = max(img[x + y*xsize] - img[x-1 + y*xsize], 0);
	double IyhM = max(img[x + y*xsize] - img[x + (y-1)*xsize], 0);
	double IxqM = max(img[x+1 + y*xsize] - img[x + y*xsize], 0);
	double IyqM = max(img[x + (y+1)*xsize] - img[x + y*xsize], 0);
	double m = 0;
	if (x1*x2 + y1*y2 > 0)
		m = sqrt(Ixhm*Ixhm + IxqM*IxqM + Iyhm*Iyhm + IyqM*IyqM);
	else
		m = sqrt(IxhM*IxhM + Ixqm*Ixqm + IyhM*IyhM + Iyqm*Iyqm);
	return (x1*x2 + y1*y2)*m;
}

double BSCB::Laplace(float* img, int x, int y, int xsize, int ysize)
{
	double Ixx = img[x + 1 + y*xsize] + img[x - 1 + y*xsize] - 2 * img[x + y*xsize];
	double Iyy = img[x + (y + 1)*xsize] + img[x + (y - 1) *xsize] - 2 * img[x + y*xsize];
	return (Ixx) + (Iyy);
}

bool BSCB::TerminateBSCBCondition(float* preData, float* afterData, float *maskData, int xsize, int ysize)
{
	double thresthod = 0.02/255;
	double err = 0;
	int iterator = 0;
	for (int i = 2; i < xsize - 2; ++i)
	{
		for (int j = 2; j < ysize - 2; ++j)
		{
			if (maskData[j*xsize + i] == -1)
			{
				err += (preData[j*xsize + i] - afterData[j*xsize + i])*(preData[j*xsize + i] - afterData[j*xsize + i]);
				iterator++;
			}
		}
	}
	if (abs(err / iterator) < thresthod)
		return 0;
	else
		return 1;
}

void BSCB::InterpolationInit(float* data, float *maskData, int xsize, int ysize)
{
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			if (maskData[j*xsize + i] == -1)
			{
				CPOINT pnt[8];
				int nerb = 0, iter = 0;
				//找到四邻域方向的像素值
				int tempxleft = i, tempxright = i, tempyup = j, tempydown = j;
				bool left = false, right = false, up = false, down = false;
				do
				{
					if (tempxleft > 0)
						tempxleft--;
					if (tempxright < xsize-1)
						tempxright++;
					if (tempyup > 0)
						tempyup--;
					if (tempydown < ysize - 1)
						tempydown++;

					if (maskData[tempxleft + j*xsize] != -1&&!left)
					{
						pnt[nerb].x = tempxleft; pnt[nerb].y = j;
						left=true;
						nerb++;
						iter++;
					}
					if (maskData[tempxright + j*xsize] != -1&&!right)
					{
						pnt[nerb].x = tempxright; pnt[nerb].y = j;
						right = true;
						nerb++;
						iter++;
					}
					if (maskData[i + tempyup*xsize] != -1&&!up)
					{
						pnt[nerb].x = i; pnt[nerb].y = tempyup;
						up = true;
						nerb++;
						iter++;
					}
					if (maskData[i + tempydown*xsize] != -1&&!down)
					{
						pnt[nerb].x = i; pnt[nerb].y = tempydown;
						down = true;
						nerb++;
						iter++;
					}
					if (tempxleft == 0)
					{
						iter++;
						left = true;
					}
					if (tempxright == xsize - 1)
					{
						iter++;
						right = true;
					}
					if (tempyup == 0)
					{
						iter++;
						up = true;
					}
					if (tempydown == ysize - 1)
					{
						down = true;
						iter++;
					}
				} while (iter<4);

				//四个方向考虑距离的插值
				double totaldis = 0;
				for (int iterator = 0; iterator < nerb; ++iterator)
				{
					CPOINT tpnt; tpnt.x = i; tpnt.y = j;
					totaldis += 1.0/GetDisofPoints(tpnt, pnt[iterator]);
				}
				float datavalue = 0;
				for (int iterator = 0; iterator < nerb; ++iterator)
				{
					CPOINT tpnt; tpnt.x = i; tpnt.y = j;
					double dis = 1.0 / GetDisofPoints(tpnt, pnt[iterator]);
					datavalue+= dis /totaldis*data[pnt[iterator].x+ pnt[iterator].y*xsize];
				}
				data[j*xsize + i] = datavalue;
			}
		}
	}
}