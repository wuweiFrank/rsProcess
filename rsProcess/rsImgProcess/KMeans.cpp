#include"KMeans.h"
#include"..\AuxiliaryFunction.h"
#include"..\gdal\include\gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

void InitKMeans(float* dataImg, int xsize, int ysize, int bands, int classNum, category* m_categories)
{
	for (int i = 0; i < classNum; ++i)
		m_categories[i] = createcategory(bands);

	//初始的给定n个像元为类中心
	for (int i = 0; i<classNum; ++i)
		for (int k = 0; k<bands; ++k)
			m_categories[i].center[k] = dataImg[k*xsize*ysize + i];

	float* pixelElement = new float[bands];
	float* centerdis = new float[classNum];
	int* objnumbers = new int[classNum];
	memset(objnumbers, 0, sizeof(int)*classNum);

	//获取每一类的像元数目
	int idx = 0;
	for (int i = 0; i<xsize*ysize; ++i)
	{
		for (int j = 0; j<bands; ++j)
			pixelElement[j] = dataImg[j*xsize*ysize + i];

		for (int j = 0; j<classNum; ++j)
			centerdis[j] = GetSSD(pixelElement, m_categories[j].center, bands);

		idx = 0;
		for (int j = 1; j<classNum; ++j)
		{
			if (centerdis[idx]>centerdis[j])
				idx = j;
		}
		objnumbers[idx]++;
	}

	//空间申请
	for (int i = 0; i < classNum; ++i)
	{
		setcategoryobjs(objnumbers[i], m_categories[i]);
	}


	//每一个像素属于哪个类
	memset(objnumbers, 0, sizeof(int)*classNum);
	for (int i = 0; i<xsize*ysize; ++i)
	{
		for (int j = 0; j<bands; ++j)
			pixelElement[j] = dataImg[j*xsize*ysize + i];

		for (int j = 0; j<classNum; ++j)
			centerdis[j] = GetSSD(pixelElement, m_categories[j].center, bands);

		idx = 0;
		for (int j = 1; j<classNum; ++j)
		{
			if (centerdis[idx]>centerdis[j])
				idx = j;
		}
		m_categories[idx].objindex[objnumbers[idx]] = i;
		objnumbers[idx]++;
	}

	delete[]pixelElement;
	delete[]centerdis;
	delete[]objnumbers;
}

void CenterKMeans(float* dataImg, int xsize, int ysize, int bands, int classNum, category* m_categories)
{
	for (int i = 0; i < classNum; ++i)
	{
		for (int j = 0; j <bands; ++j)
		{
			getstatisticcategory(dataImg + j*xsize*ysize, m_categories[i], j);
		}
	}
}

void IteratorKMeans(float* dataImg, int xsize, int ysize, int bands, int classNum, category* m_categories)
{
	//重新计算聚类重新计算聚类中心
	//获取每一类的像元数目
	float* pixelElement = new float[bands];
	float* centerdis = new float[classNum];
	int* objnumbers = new int[classNum];
	memset(objnumbers, 0, sizeof(int)*classNum);

	//获取每一类的像元数目
	int idx = 0;
	for (int i = 0; i < xsize*ysize; ++i)
	{
		for (int j = 0; j < bands; ++j)
			pixelElement[j] = dataImg[j*xsize*ysize + i];

		for (int j = 0; j < classNum; ++j)
			centerdis[j] = GetSSD(pixelElement, m_categories[j].center, bands);

		idx = 0;
		for (int j = 1; j < classNum; ++j)
		{
			if (centerdis[idx] > centerdis[j])
				idx = j;
		}
		objnumbers[idx]++;
	}

	//删除以前的空间
	for (int i = 0; i < classNum; ++i)
		delete[] m_categories[i].objindex;
	//空间申请
	for (int i = 0; i < classNum; ++i)
	{
		setcategoryobjs(objnumbers[i], m_categories[i]);
	}
	//每一个像素属于哪个类
	memset(objnumbers, 0, sizeof(int)*classNum);
	for (int i = 0; i<xsize*ysize; ++i)
	{
		for (int j = 0; j<bands; ++j)
			pixelElement[j] = dataImg[j*xsize*ysize + i];

		for (int j = 0; j<classNum; ++j)
			centerdis[j] = GetSSD(pixelElement, m_categories[j].center, bands);

		idx = 0;
		for (int j = 1; j<classNum; ++j)
		{
			if (centerdis[idx]>centerdis[j])
				idx = j;
		}
		m_categories[idx].objindex[objnumbers[idx]] = i;
		objnumbers[idx]++;
	}

	delete[]pixelElement;
	delete[]centerdis;
	delete[]objnumbers;

	//计算聚类中心
	CenterKMeans(dataImg, xsize, ysize, bands, classNum, m_categories);
}

void KMeansClassify(const char* pathImgIn, const char* pathImgOut, int classNum)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen(pathImgIn, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);

	char* classrs = new char[xsize*ysize];
	float* centersPre = new float[classNum*bands];
	float* centersLast = new float[classNum*bands];
	float* dataimg = new float[xsize*ysize*bands];
	category* m_categroies = new category[classNum];

	for (int i = 0; i<bands; i++)
		GDALRasterIO(GDALGetRasterBand(m_datasetin, i + 1), GF_Read, 0, 0, xsize, ysize, dataimg + i*xsize*ysize, xsize, ysize, GDT_Float32, 0, 0);

	//初始化分类结果
	InitKMeans(dataimg, xsize, ysize, bands, classNum, m_categroies);

	//计算分类后类中心
	CenterKMeans(dataimg, xsize, ysize, bands,classNum, m_categroies);
	for (int i = 0; i < classNum; ++i)
	{
		memcpy(centersPre, m_categroies[i].center, sizeof(float)*bands);
	}

	int iterator_i = 0;
	do
	{
		printf("迭代次数：%d\r", iterator_i);
		//迭代计算分类结果和类中心
		IteratorKMeans(dataimg, xsize, ysize, bands, classNum, m_categroies);
		for (int i = 0; i < classNum; ++i)
		{
			memcpy(centersLast + i*bands, m_categroies[i].center, sizeof(float)*bands);
		}

		//两次类中心误差
		float disCenter = 0;
		for (int i = 0; i<classNum*bands; ++i)
			disCenter += fabs(centersLast[i] - centersPre[i]);
		if (disCenter<1)
			break;
		else
			memcpy(centersPre, centersLast, sizeof(float)*bands*classNum);
		++iterator_i;
	} while (iterator_i<30);
	printf("\n");

	//构造分类图
	for (int i = 0; i < classNum; ++i)
	{
		for (int j = 0; j < m_categroies[i].objnumbers; ++j)
			classrs[m_categroies[i].objindex[j]] = char(i);
	}

	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathImgOut, xsize, ysize, 1, GDT_Byte, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetout, 1), GF_Write, 0, 0, xsize, ysize, classrs, xsize, ysize, GDT_Byte, 0, 0);

	const char* proj = GDALGetProjectionRef(m_datasetin);
	GDALSetProjection(m_datasetout, proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_datasetin, adfGeoTrans);
	GDALSetGeoTransform(m_datasetout, adfGeoTrans);

	//清理内存
	if (classrs != NULL)
		delete[]classrs;
	if (dataimg != NULL)
		delete[]dataimg;
	if (centersPre != NULL)
		delete[]centersPre;
	if (centersLast != NULL)
		delete[]centersLast;

	classrs = NULL;
	dataimg = NULL;
	centersPre = centersLast = NULL;

	GDALClose(m_datasetin);
	GDALClose(m_datasetout);

	for (int i = 0; i < classNum; ++i)
		freecategory(m_categroies[i]);
	delete[]m_categroies;
}

void ISODataClassify(const char* pathImgIn, const char* pathImgOut, int classNum, float maxDev, float minDis)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
	GDALDatasetH m_datasetin = GDALOpen(pathImgIn, GA_ReadOnly);

	int xsize = GDALGetRasterXSize(m_datasetin);
	int ysize = GDALGetRasterYSize(m_datasetin);
	int bands = GDALGetRasterCount(m_datasetin);

	char* classrs = new char[xsize*ysize];
	float* dataimg = new float[xsize*ysize*bands];
	category* m_categroies = new category[classNum];

	for (int i = 0; i<bands; i++)
		GDALRasterIO(GDALGetRasterBand(m_datasetin, i + 1), GF_Read, 0, 0, xsize, ysize, dataimg + i*xsize*ysize, xsize, ysize, GDT_Float32, 0, 0);

	//初始化分类结果
	InitKMeans(dataimg, xsize, ysize, bands, classNum, m_categroies);
	//计算分类后类中心
	CenterKMeans(dataimg, xsize, ysize, bands, classNum, m_categroies);

	int preclassnum = classNum, lastclassnum = classNum;

	do
	{
		float* tempdev = new float[classNum*bands];
		float* centersPre = new float[classNum*bands];
		float* centersLast = new float[classNum*bands];
		for (int i = 0; i < classNum; ++i)
		{
			memcpy(centersPre, m_categroies[i].center, sizeof(float)*bands);
		}

		int iterator_i = 0;
		do
		{
			printf("迭代次数：%d\r", iterator_i);
			//迭代计算分类结果和类中心
			IteratorKMeans(dataimg, xsize, ysize, bands, classNum, m_categroies);
			for (int i = 0; i < classNum; ++i)
				memcpy(centersLast+i*bands, m_categroies[i].center, sizeof(float)*bands);
			for (int i = 0; i < classNum; ++i)
				memcpy(tempdev + i*bands, m_categroies[i].objdervation, sizeof(float)*bands);

			//两次类中心误差
			float disCenter = 0;
			for (int i = 0; i<classNum*bands; ++i)
				disCenter += fabs(centersLast[i] - centersPre[i]);
			if (disCenter<1)
				break;
			else
				memcpy(centersPre, centersLast, sizeof(float)*bands*classNum);
			++iterator_i;
		} while (iterator_i<100);
		printf("\n");

		//判断分裂之后的类的个数
		for (int i = 0; i < classNum; ++i)
		{
			float meandev=0;
			for (int j = 0; j < bands; ++j)
				meandev += m_categroies[i].objdervation[j];
			if (meandev / bands > maxDev)
				lastclassnum++;
		}
		if (lastclassnum == classNum)
			break;
		else
		{

			//首先清空所有类
			for (int i = 0; i < classNum; ++i)
				freecategory(m_categroies[i]);
			delete[]m_categroies;

			//重新申请空间
			m_categroies = new category[lastclassnum];
			for (int i = 0; i < lastclassnum; ++i)
			{
				m_categroies[i] = createcategory(bands);
			}

			//分裂
			//初始值
			int precategoryidx = 0;
			int lastcategoryidx = 0;//处理到第几个类别了
			for (int i = 0; i < classNum; ++i)
			{
				float meandev = 0;
				for (int j = 0; j < bands; ++j)
					meandev += tempdev[i*bands+j];

				//大于阈值则分裂
				if (meandev / bands > maxDev)
				{
					for (int j = 0; j < bands; ++j)
					{
						m_categroies[lastcategoryidx].center[j] = abs(centersLast[precategoryidx*bands + j]- 0.5*tempdev[precategoryidx*bands + j]);
						m_categroies[lastcategoryidx+1].center[j] = abs(centersLast[precategoryidx*bands + j]+ 0.5*tempdev[precategoryidx*bands + j]);
					}
					lastcategoryidx += 2;
					precategoryidx++;
				}
				else
				{
					for (int j = 0; j < bands; ++j)
					{
						m_categroies[lastcategoryidx].center[j] = centersLast[precategoryidx*bands + j];
					}
					lastcategoryidx++;
					precategoryidx++;
				}
			}

			classNum = lastclassnum;
		}
		delete[]tempdev;
		delete[]centersLast;
		delete[]centersPre;
	} while (true);
	printf("\n");

	//构造分类图
	for (int i = 0; i < classNum; ++i)
	{
		for (int j = 0; j < m_categroies[i].objnumbers; ++j)
			classrs[m_categroies[i].objindex[j]] = char(i);
	}

	GDALDatasetH m_datasetout = GDALCreate(GDALGetDriverByName("GTiff"), pathImgOut, xsize, ysize, 1, GDT_Byte, NULL);
	GDALRasterIO(GDALGetRasterBand(m_datasetout, 1), GF_Write, 0, 0, xsize, ysize, classrs, xsize, ysize, GDT_Byte, 0, 0);

	const char* proj = GDALGetProjectionRef(m_datasetin);
	GDALSetProjection(m_datasetout, proj);
	double adfGeoTrans[6];
	GDALGetGeoTransform(m_datasetin, adfGeoTrans);
	GDALSetGeoTransform(m_datasetout, adfGeoTrans);

	//清理内存
	if (classrs != NULL)
		delete[]classrs;
	if (dataimg != NULL)
		delete[]dataimg;

	classrs = NULL;
	dataimg = NULL;

	GDALClose(m_datasetin);
	GDALClose(m_datasetout);

	for (int i = 0; i < classNum; ++i)
		freecategory(m_categroies[i]);
	delete[]m_categroies;
}