#include"BSCB.h"
#include"..\gdal\include\gdal_priv.h"
#include"..\AuxiliaryFunction.h"
using namespace std;
#pragma comment(lib,"gdal_i.lib")

void BSCB::BSBCImageInpaintingProcess(const char* pathImgIn, const char* pathImgMask, const char* pathImgRepair)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");			//中文路径
	GDALDatasetH m_datasetImg = GDALOpen(pathImgIn, GA_ReadOnly);
	GDALDatasetH m_datasetMsk = GDALOpen(pathImgMask, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetMsk);
	int ysize = GDALGetRasterYSize(m_datasetMsk);
	int bands = GDALGetRasterCount(m_datasetMsk);

	//数据申请
	int* imgData = new int[xsize*ysize];
	int* mskData = new int[xsize*ysize];
	int* rpaData = new int[xsize*ysize];
	int* pocData = new int[xsize*ysize];

	// 不知道要设置多少才合适
	double deltaT = 0.1;
	int    IteratorN = 0;	//第N次迭代
	
	//获取像素值
	
	memcpy(rpaData, imgData, sizeof(int)*xsize*ysize);
	memcpy(pocData, imgData, sizeof(int)*xsize*ysize);

	// 先进行扩散
	for (int i = 1; i < xsize-1; ++i)
	{
		for (int j = 1; j < ysize-1; ++j)
		{
			if (mskData[j*xsize + i] == 0)
				DiffuseLoop(pocData, imgData,i,j,xsize,ysize);
		}
	}

	do
	{
		memcpy( rpaData, pocData, sizeof(int)*xsize*ysize);

		//修补和扩散交替进行
		for (int eachLoop = 0; eachLoop < 10; ++eachLoop)
		{
			//修复
			for (int i = 1; i < xsize - 1; ++i)
			{
				for (int j = 1; j < ysize - 1; ++j)
				{
					if (mskData[j*xsize + i] == 0)
						RepaireLoop(pocData, rpaData, deltaT, i, j, xsize, ysize);
				}
			}
			//扩散
			for (int i = 1; i < xsize - 1; ++i)
			{
				for (int j = 1; j < ysize - 1; ++j)
				{
					if (mskData[j*xsize + i] == 0)
						DiffuseLoop(pocData, rpaData, i, j, xsize, ysize);
				}
			}
		}

		IteratorN++;
		printf("iterator number :%d\r", IteratorN);
	} while (TerminateBSCBCondition(pocData, rpaData, mskData,xsize,ysize));

	delete[]imgData;
	delete[]mskData;
	delete[]rpaData;
	delete[]pocData;
}

void BSCB::DiffuseLoop(int* data,int *img, int x, int y, int xsize, int ysize)
{
	double Ix = 0.5*(img[(x + 1) + y*xsize] - img[(x - 1) + y*xsize]);
	double Iy = 0.5*(img[x + (y + 1)*xsize] - img[x + (y - 1)*xsize]);
	double Ixx = img[(x + 1) + y*xsize] + img[(x - 1) + y*xsize] - 2 * img[x + y*xsize];
	double Iyy = img[x + (y + 1)*xsize] + img[x + (y - 1)*xsize] - 2 * img[x + y*xsize];
	double Ixy = 0.5*(0.5*(img[(x + 1) + (y + 1)*xsize] + img[(x - 1) + (y + 1)*xsize]) - 0.5*(img[(x + 1) + (y - 1)*xsize] + img[(x - 1) + (y - 1)*xsize]));
	double normN2 = Ix*Ix + Iy*Iy;
	if (normN2 == 0)
		data[y*xsize + x] = 0;
	else
		data[y*xsize + x] = (Ixx*(Iy*Iy) + Iyy*(Ix*Ix) - 2 * Ix*Iy*Ixy) / (normN2);
}

void BSCB::RepaireLoop(int* data, int* img, double delta, int x, int y, int xsize, int ysize)
{
	double iteratorIt = RepairChange(img, x, y, xsize, ysize);
	data[y*xsize + x] = img[y*xsize + x] + delta*iteratorIt;
}

double BSCB::RepairChange(int* img, int x, int y, int xsize, int ysize)
{
	double laplacex = Laplace(img, x + 1, y, xsize, ysize) - Laplace(img, x - 1, y, xsize, ysize);
	double laplacey = Laplace(img, x, y + 1, xsize, ysize) - Laplace(img, x, y - 1, xsize, ysize);
	double directionx = 0, directiony = 0;
	double Ix = 0.5*(img[x + 1 + y*xsize] - img[x - 1 + y*xsize]);
	double Iy = 0.5*(img[x + (y + 1)*xsize] - img[x + (y - 1)*xsize]);
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

double BSCB::Laplace(int* img, int x, int y, int xsize, int ysize)
{
	double Ixx = img[x + 1 + y*xsize] + img[x - 1 + y*xsize] - 2 * img[x + y*xsize];
	double Iyy = img[x + (y + 1)*xsize] + img[x + (y - 1) *xsize] - 2 * img[x + y*xsize];
	return Ixx + Iyy;
}

bool BSCB::TerminateBSCBCondition(int* preData, int* afterData, int *maskData, int xsize, int ysize)
{
	double thresthod = 1;
	double err = 0;
	int iterator = 0;
	for (int i = 2; i < xsize - 2; ++i)
	{
		for (int j = 2; j < ysize - 2; ++j)
		{
			if (maskData[j*xsize + i] == 0)
			{
				err += abs(preData[j*xsize + i] - afterData[j*xsize + i]);
				iterator++;
			}
		}
	}
	if (err / iterator < thresthod)
		return 0;
	else
		return 1;
}