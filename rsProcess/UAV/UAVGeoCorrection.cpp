#include"..\CoordinateTrans.h"
#include"..\gdal\include\ogr_spatialref.h"
#include"..\gdal\include\gdal_priv.h"
#include"..\GDALTools.h"
#include"..\AuxiliaryFunction.h"
#include"UAVGeoCorrection.h"
#include<omp.h>
using namespace std;

long UAVPOSProcess::GeoPOSProc_ReadPartPOS(const char *pPOSFile, long nLines, double &dB, double &dL, double &dH, int nbeginLine)
{
	long   lError = 0;
	int    i;
	FILE   *fpin = NULL;
	double fReadData[20];
	char   cTempchar[2048];
	int	   realLines = 0;
	if (m_geo_POS != NULL)
		delete[]m_geo_POS;
	m_geo_POS = NULL;
	m_geo_POS = new POS[nLines];
	if ((fopen_s(&fpin,pPOSFile, "r")) != NULL)
	{
		lError = -1;  //打开POS文件失败
		goto ErrEnd;
	}

	dB = dL = dH = 0.0;
	for (i = 0; i < nbeginLine; i++)
	{
		if (feof(fpin))
		{
			lError = -1; //读取POS文件失败
			goto ErrEnd;
		}
		fgets(cTempchar, 2048, fpin);
	}

	for (i = 0; i < nLines; i++)
	{
		realLines++;
		if (feof(fpin))
		{
			lError = 40308; //读取POS文件失败
			goto ErrEnd;
		}
		fgets(cTempchar, 2048, fpin);
		lError = sscanf_s(cTempchar,"%lf%lf%lf%lf%lf%lf%lf",
			&fReadData[0], &fReadData[1], &fReadData[2], &fReadData[3], &fReadData[4], &fReadData[5], &fReadData[6]);
		if (lError == 0)
		{
			lError = 40309; //解析POS文件失败
			goto ErrEnd;
		}

		dB += fReadData[1];
		dL += fReadData[2];
		dH += fReadData[3];

		this->m_geo_POS[i].m_latitude	= double(fReadData[1] * PI / 180.0);       //X
		this->m_geo_POS[i].m_longitude	= double(fReadData[2] * PI / 180.0);        //Y
		this->m_geo_POS[i].m_height		= double(fReadData[3]);                 //height
		this->m_geo_POS[i].m_roll		= double(fReadData[4] * PI / 180.0);        //roll
		this->m_geo_POS[i].m_pitch		= double(fReadData[5] * PI / 180.0);        //pitch
		this->m_geo_POS[i].m_yaw		= double((fReadData[6])*PI / 180.0);     //heading
	}
	lError = 0;
	m_nPOSLines = realLines;
	dB /= nLines; dB = dB*PI / 180.0;
	dL /= nLines; dL = dL*PI / 180.0;
	dH /= nLines; ;

ErrEnd:
	if (fpin)
	{
		fclose(fpin);
		fpin = NULL;
	}
	return lError;
}

long UAVPOSProcess::GeoPOSProc_ExtractEO(POS m_perpos, EO &m_pereo)
{
	long lError = 0;

	THREEDPOINT theta;
	double dB, dL, dH;

	//get BLH value
	dB = m_perpos.m_latitude; dL = m_perpos.m_longitude; dH = m_perpos.m_height;
	double EMMatrix[9];
	EMMatrix[0] = -sin(dL);
	EMMatrix[1] = cos(dL);
	EMMatrix[2] = 0;

	EMMatrix[3] = -sin(dB)*cos(dL);
	EMMatrix[4] = -sin(dB)*sin(dL);
	EMMatrix[5] = cos(dB);

	EMMatrix[6] = cos(dB)*cos(dL);
	EMMatrix[7] = cos(dB)*sin(dL);
	EMMatrix[8] = sin(dB);

	//coordinate trans from BLH to XYZ
	THREEDPOINT XYZPnt;
	CoordinateTrans m_coordTrans;
	m_coordTrans.BLHToXYZ(dB, dL, 0, XYZPnt);		//BL0->XYZ
	int nQuandNum = GeoPOSProc_EOQuadrant(m_perpos, EMMatrix, XYZPnt);

	theta.dX = theta.dY = theta.dZ = 0;	//do not take the palcement angle into consideration
	double pVector[3] = { 0.0,0.0,0.0 };//do not take the palcement vector into consideration

	if (GeoPOSProc_EOMatrixTurn(m_perpos, XYZPnt, theta, nQuandNum, EMMatrix, m_pereo) == 0)
		return -1;
	else
		return 0;
}

//===================================================================================================================================================
long UAVGeoCorrection::UAVGeoCorrection_GeoCorrect(const char* pathSrcDir, const char* pathDstDir, const char* pathPOS, const char* pathDEM,
																				int IMGbegline, int POSbegline, int lines, double fLen, double fGSD, double AvgHeight)
{
	UAVPOSProcess m_POS_Proc;
	THREEDPOINT theta;
	theta.dX = theta.dY = theta.dZ = 0;
	float setupAngle[] = { 0,0,0 };
	double dL, dB, dH;
	long lError = 0;
	if (lError != 0)
		return lError;
	m_POS_Proc.GeoPOSProc_ReadPartPOS(pathPOS, lines, dB, dL, dH, POSbegline);
	//lError = m_POS_Proc.GeoPOSProc_ReadEOFile(pathDstEO, dB, dL, dH);

#pragma omp parallel for
	for (int i = IMGbegline; i<lines + IMGbegline; i++)
	{
		char pathSrcImg[256], pathDstImg[256], cA[20], cB[20];
		EO tmpEO;
		m_POS_Proc.GeoPOSProc_ExtractEO(m_POS_Proc.m_geo_POS[i - IMGbegline], tmpEO);
		//指定格式
		sprintf_s(cA, "\\IMG_%004d.JPG", i);
		sprintf_s(cB, "\\IMG_%004d.tif", i);

		strcpy_s(pathSrcImg, pathDstDir);
		strcat_s(pathSrcImg, cA);
		strcpy_s(pathDstImg, pathDstDir);
		strcat_s(pathDstImg, cB);
		lError = UAVGeoCorrection_GeoCorrect(pathSrcImg, pathDstImg, /*m_POS_Proc.m_geo_EO[i - IMGbegline]*/tmpEO, dL, dB, fLen, fGSD, AvgHeight);
		if (lError != 0)
		{
			printf("%s image correct error\n", pathSrcImg);
			continue;
		}
	}
	return lError;
}

long UAVGeoCorrection::UAVGeoCorrection_GeoCorrect(const char* pathSrc, const char* pathDst, EO  m_preeo, double dL, double dB, double fLen, double fGSD, double AvgHeight, char* pathDEM)
{
	UAVPOSProcess m_GeoPOS_Proc;
	int 	nBandCount, ImageWidth, ImageHeight;
	GDALDataset    *poDataset;
	long    lError = 0;
	double  dLatitude = 0;
	double  dLongitude = 0;
	double  dFlightHeight = 0;

	DPOINT  *pPositions = NULL;
	THREEDPOINT* pGoundPnt;

	double  *pImgBuffer = NULL;
	double  *pRegBuffer = NULL;

	int     nEOCounts = 0;
	double  centereast, centernorth;
	int     centerzone;
	OGRSpatialReference  oSRS;
	char *pszSRS_WKT = NULL;

	//判断文件是否存在
	GDALAllRegister();//GDAL数据集
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	poDataset = (GDALDataset *)GDALOpen(pathSrc, GA_ReadOnly);
	if (poDataset == NULL)
		return -2;

	nBandCount = poDataset->GetRasterCount();
	ImageWidth = poDataset->GetRasterXSize();
	ImageHeight = poDataset->GetRasterYSize();

	if (lError != 0)
		return lError;

	if (dLongitude*180.0 / PI < -180.0 || dLongitude*180.0 / PI > 180.0)
		return -1;
	if (dLatitude*180.0 / PI <= -90.0 || dLatitude*180.0 / PI >= 90.0)
		return -1;

	//计算中心经纬度和投影带
	lError = tsmLatLongToUTM(dLatitude*180.0 / PI, dLongitude*180.0 / PI, &centerzone, &centereast, &centernorth);
	if (lError == 0)
		return -1;

	//均匀选取5个地面点和影像点计算坐标
	bool probOrNot = true;
	if (pathDEM != NULL)
		probOrNot = false;

	if (probOrNot)
	{
		pGoundPnt = new THREEDPOINT[5];
		DPOINT pModelPnts[5];
		for (int i = 0; i<5; i++)
			pGoundPnt[i].dZ = AvgHeight;
		DPOINT pImagePnts[5];
		//计算对应的地面点的坐标
		lError = UAVGeoCorrection_GeoPntsProb(dLatitude, dLongitude, AvgHeight, m_preeo, fLen,ImageWidth,ImageHeight, pGoundPnt, pModelPnts, 5);

		double* pdfGCPs = new double[5 * 4];
		for (int i = 0; i<5; i++)
		{
			pdfGCPs[4 * i + 0] = pImagePnts[i].dX;
			pdfGCPs[4 * i + 1] = pImagePnts[i].dY;
			pdfGCPs[4 * i + 2] = pGoundPnt[i].dX;
			pdfGCPs[4 * i + 3] = pGoundPnt[i].dY;
		}
		oSRS.SetUTM(centerzone, TRUE);
		oSRS.SetWellKnownGeogCS("WGS84");
		oSRS.exportToWkt(&pszSRS_WKT);
		double dGroundSize[2] = { fGSD,-fGSD };
		GeoGCPProcess m_GcpProc;
		//lError=m_GcpProc.GeoProc_GCPWarpTPS(pathSrc,pathDst,pszSRS_WKT,dGroundSize,GRA_Bilinear,pdfGCPs,64);
		lError = m_GcpProc.GeoProc_GCPWarpOrder(pathSrc, pathDst, 1, TRUE, pszSRS_WKT, dGroundSize, pdfGCPs, 5);
		if (lError != 0)
			return lError;
		CPLFree(pszSRS_WKT);
		delete[]pGoundPnt;
		delete[]pdfGCPs;
		GDALClose(poDataset);
	}
	else
	{
		unsigned char* imgBuffer = NULL;
		unsigned char* dstBuffer = NULL;
		try
		{
			pGoundPnt = new THREEDPOINT[ImageWidth*ImageHeight];
		}
		catch (bad_alloc &e)
		{
			printf("%s", e.what());
			exit(-1);
		}
		//char* pathDEM = "";//定义的地面高程信息，如果知道直接在代码中修改
		lError = UAVGeoCorrection_GeoPntsAccu(dLatitude, dLongitude, AvgHeight, m_preeo, fLen, ImageWidth, ImageHeight, pGoundPnt, pathDEM);
		//初始化最大最小点
		DPOINT minPt, maxPt;
		minPt.dX = MAX_NUM; minPt.dY = MAX_NUM;
		maxPt.dX = MIN_NUM; maxPt.dY = MIN_NUM;
		for (int j = 0; j<ImageWidth*ImageHeight; j++)
		{
			//寻找最大最小点
			if (minPt.dX>pGoundPnt[j].dX)
				minPt.dX = pGoundPnt[j].dX;
			if (minPt.dY>pGoundPnt[j].dY)
				minPt.dY = pGoundPnt[j].dY;
			if (maxPt.dX<pGoundPnt[j].dX)
				maxPt.dX = pGoundPnt[j].dX;
			if (maxPt.dY<pGoundPnt[j].dY)
				maxPt.dY = pGoundPnt[j].dY;
		}
		int dstWidth  = (maxPt.dX - minPt.dX) / fGSD;
		int dstHeight = (maxPt.dX - minPt.dX) / fGSD;
		try
		{
			imgBuffer = new unsigned char[ImageWidth*ImageHeight];
			dstBuffer = new unsigned char[dstWidth*dstHeight];
		}
		catch (bad_alloc &e)
		{
			printf("%s", e.what());
			exit(-1);
		}
		char **papszOptions = NULL;
		papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BAND");
		GDALDatasetH dstDataset = GDALCreate(GDALGetDriverByName("GTiff"), pathDst, dstWidth, dstHeight, nBandCount, GDT_Byte, papszOptions);
		for (size_t i = 0; i < nBandCount; i++)
		{
			poDataset->GetRasterBand(i + 1)->RasterIO(GF_Read, 0, 0, ImageWidth, ImageHeight, imgBuffer, ImageWidth, ImageHeight, GDT_Byte, 0, 0);
			GetImgSample(imgBuffer, minPt, maxPt, pGoundPnt, fGSD, fGSD, ImageWidth, ImageHeight, dstWidth, dstHeight, dstBuffer);
			GDALRasterIO(GDALGetRasterBand(dstDataset, i + 1), GF_Write, 0, 0, dstWidth, dstHeight, dstBuffer, dstWidth, dstHeight, GDT_Byte, 0, 0);

		}
		double dGeoTransform[6];
		//设置仿射变换参数
		dGeoTransform[0] = minPt.dX;
		dGeoTransform[1] = fGSD;
		dGeoTransform[2] = 0;
		dGeoTransform[3] = maxPt.dY;
		dGeoTransform[4] = 0;
		dGeoTransform[5] = fGSD;
		//设置地图投影信息
		int nZone; double x, y;
		tsmLatLongToUTM(dB * 180 / PI, dL * 180 / PI, &nZone, &x, &y);
		oSRS.SetProjCS("UTM (WGS84) in northern hemisphere");
		oSRS.SetWellKnownGeogCS("WGS84");
		oSRS.SetUTM(nZone, TRUE);
		oSRS.exportToWkt(&pszSRS_WKT);
		//写入投影信息和仿射变换参数
		GDALSetProjection(dstDataset, pszSRS_WKT);
		GDALSetGeoTransform(dstDataset,dGeoTransform);
		GDALClose(dstDataset);
		GDALClose(poDataset);
		CPLFree(pszSRS_WKT);
	}

	return 0;
}

long UAVGeoCorrection::UAVGeoCorrection_GeoPntsProb(double dB, double dL, double dH, EO pEO, double fLen, int width, int height, THREEDPOINT *pGoundPnt, DPOINT* pMapImgPnt, int pntNum)
{
	pMapImgPnt[0].dX = width / 2; pMapImgPnt[0].dY = height / 2;
	pMapImgPnt[1].dX = 0; pMapImgPnt[1].dY = 0;
	pMapImgPnt[2].dX = 0; pMapImgPnt[2].dY = height;
	pMapImgPnt[3].dX = width; pMapImgPnt[3].dY = 0;
	pMapImgPnt[4].dX = width; pMapImgPnt[4].dY = height;
	for (int i = 0; i<5; ++i)
	{
		pMapImgPnt[i].dX = pMapImgPnt[i].dX - width / 2;
		pMapImgPnt[i].dY = height / 2 - pMapImgPnt[i].dY;
	}

	CoordinateTrans m_coordiTrans;
	int i = 0, j = 0;
	double dPhotoPt[3]; double dModelPt[3];
	memset(dPhotoPt, 0, 3 * sizeof(double));
	memset(dModelPt, 0, 3 * sizeof(double));

	double dXs = 0, dYs = 0, dZs = 0;

	double dRMatrix[9];
	double ccdsize = 0.0000032;	//单位是m 一般相机ccd大小为3.12um 全画幅
	dXs = pEO.m_dX; dYs = pEO.m_dY; dZs = pEO.m_dZ;
	dRMatrix[0] = pEO.m_dRMatrix[0]; dRMatrix[1] = pEO.m_dRMatrix[1]; dRMatrix[2] = pEO.m_dRMatrix[2];
	dRMatrix[3] = pEO.m_dRMatrix[3]; dRMatrix[4] = pEO.m_dRMatrix[4]; dRMatrix[5] = pEO.m_dRMatrix[5];
	dRMatrix[6] = pEO.m_dRMatrix[6]; dRMatrix[7] = pEO.m_dRMatrix[7]; dRMatrix[8] = pEO.m_dRMatrix[8];

	for (i = 0; i<pntNum; i++)
	{
		memset(dPhotoPt, 0, 3 * sizeof(double));
		memset(dModelPt, 0, 3 * sizeof(double));

		//像平面坐标系转到像空间坐标系
		double img_pnt[2];
		dPhotoPt[0] = pMapImgPnt[i].dX*ccdsize;
		dPhotoPt[1] = pMapImgPnt[i].dY*ccdsize;
		dPhotoPt[2] = -fLen;					//单位是m
		MatrixMuti(dRMatrix, 3, 3, 1, dPhotoPt, dModelPt);
		pGoundPnt[i].dX = dXs + (pGoundPnt[i].dZ - dZs)*dModelPt[0] / dModelPt[2];
		pGoundPnt[i].dY = dYs + (pGoundPnt[i].dZ - dZs)*dModelPt[1] / dModelPt[2];
	}

	THREEDPOINT Pnt;
	THREEDPOINT TemPnt;

	m_coordiTrans.BLHToXYZ(dB, dL, 0, Pnt);
	double dEMMatrix[9], dPt[3];
	double dTempB, dTempL, dTempH;
	double x, y;
	int nZone;

	dEMMatrix[0] = -sin(dL);
	dEMMatrix[1] = -sin(dB)*cos(dL);
	dEMMatrix[2] = cos(dB)*cos(dL);
	dEMMatrix[3] = cos(dL);
	dEMMatrix[4] = -sin(dB)*sin(dL);
	dEMMatrix[5] = cos(dB)*sin(dL);
	dEMMatrix[6] = 0;
	dEMMatrix[7] = cos(dB);
	dEMMatrix[8] = sin(dB);

	//获取各个点的坐标
	for (i = 0; i<pntNum; i++)
	{
		dModelPt[0] = pGoundPnt[i].dX;
		dModelPt[1] = pGoundPnt[i].dY;
		dModelPt[2] = pGoundPnt[i].dZ;
		MatrixMuti(dEMMatrix, 3, 3, 1, dModelPt, dPt);

		TemPnt.dX = Pnt.dX + dPt[0];
		TemPnt.dY = Pnt.dY + dPt[1];
		TemPnt.dZ = Pnt.dZ + dPt[2];

		m_coordiTrans.XYZToBLH(TemPnt, dTempB, dTempL, dTempH);
		pGoundPnt[i].dX = dTempB;
		pGoundPnt[i].dY = dTempL;
		pGoundPnt[i].dZ = dTempH;
		tsmLatLongToUTM(pGoundPnt[i].dX, pGoundPnt[i].dY, &nZone, &x, &y);
		pGoundPnt[i].dX = x;
		pGoundPnt[i].dY = y;
	}
	return 0;
}

long UAVGeoCorrection::UAVGeoCorrection_GeoPntsAccu(double dB, double dL, double dH, EO pEO, double fLen, int width, int height, THREEDPOINT *pGoundPnt, const char* pathDEM)
{
	CoordinateTrans m_coordiTrans;
	double dGeoTransform[6] = { 0 };
	double *pDEMPt = NULL;
	int nDEMSamples = 0, nDEMLines = 0;
	int nDEMZone = 0;
	DPOINT originPt;
	double dGSDX = 0, dGSDY = 0;
	int nC = 0, nY = 0;			//坐标取整变量
	double fDX = 0, fDY = 0;	//坐标取余数变量
	double dElevation = 0;
	DPOINT minPt, maxPt;			//影像上的最大最小点的范围
	DPOINT rangPnt[2];			//DEM数据的范围
								//根据中心点的经纬度获取投影带信息
	double dRMatrix[9], dEMMatrix[9];

	//成图坐标系到地心坐标系
	dEMMatrix[0] = -sin(dL);
	dEMMatrix[1] = -sin(dB)*cos(dL);
	dEMMatrix[2] = cos(dB)*cos(dL);
	dEMMatrix[3] = cos(dL);
	dEMMatrix[4] = -sin(dB)*sin(dL);
	dEMMatrix[5] = cos(dB)*sin(dL);
	dEMMatrix[6] = 0;
	dEMMatrix[7] = cos(dB);
	dEMMatrix[8] = sin(dB);

	//初始化最大最小点
	minPt.dX = MAX_NUM; minPt.dY = MAX_NUM;
	maxPt.dX = MIN_NUM; maxPt.dY = MIN_NUM;

	THREEDPOINT XYZPt;
	m_coordiTrans.BLHToXYZ(dB, dL, 0, XYZPt);
	int tmpnZone;
	int projType = 0;
	double tmpx, tmpy;
	tsmLatLongToUTM(dB * 180 / PI, dL * 180 / PI, &tmpnZone, &tmpx, &tmpy);
	//读取DEM文件，获取DEM文件的地理坐标和高程信息
	Level2Proc_ReadDEMFile(pathDEM, nDEMSamples, nDEMLines, rangPnt, dGeoTransform, nDEMZone, projType, pDEMPt);
	if (tmpnZone != nDEMZone)
	{
		//DEM文件区域和图像区域不匹配
		//此处可能存在6度分带和三度分带的问题，目前还没遇到
		printf("影像投影带与DEM投影带不一致\n");
		if (pDEMPt != NULL)
			delete[]pDEMPt;
		exit(-1);
	}
	if (projType != 2)
	{
		printf("影像投影与DEM投影不一致\n");
		if (pDEMPt != NULL)
			delete[]pDEMPt;
		exit(-1);
	}

	int i = 0, j = 0;
	double dPhotoPt[3]; double dModelPt[3], dGroundPnt[3];
	memset(dPhotoPt, 0, 3 * sizeof(double));
	memset(dModelPt, 0, 3 * sizeof(double));
	double dXs = 0, dYs = 0, dZs = 0;
	dXs = pEO.m_dX; dYs = pEO.m_dY; dZs = pEO.m_dZ;
	dRMatrix[0] = pEO.m_dRMatrix[0]; dRMatrix[1] = pEO.m_dRMatrix[1]; dRMatrix[2] = pEO.m_dRMatrix[2];
	dRMatrix[3] = pEO.m_dRMatrix[3]; dRMatrix[4] = pEO.m_dRMatrix[4]; dRMatrix[5] = pEO.m_dRMatrix[5];
	dRMatrix[6] = pEO.m_dRMatrix[6]; dRMatrix[7] = pEO.m_dRMatrix[7]; dRMatrix[8] = pEO.m_dRMatrix[8];


	THREEDPOINT Pnt;
	THREEDPOINT TemPnt;
	double dPt[3];
	double dTempB, dTempL, dTempH;
	double x, y;
	int nZone;
	m_coordiTrans.BLHToXYZ(dB, dL, 0, Pnt);
	originPt.dX = dGeoTransform[0];
	originPt.dY = dGeoTransform[3];
	dGSDX = dGeoTransform[1];
	dGSDY = dGeoTransform[5];
	double ccdSize = 0.00003125;
	//首次计算各个点的坐标
	for (i = 0; i<width; i++)
	{
		for (int j = 0; j < height; ++j)
		{
			memset(dPhotoPt, 0, 3 * sizeof(double));
			memset(dModelPt, 0, 3 * sizeof(double));

			//像平面坐标系转到像空间坐标系
			dPhotoPt[0] = (i - width / 2)*ccdSize;
			dPhotoPt[1] = (height / 2 - j)*ccdSize;
			dPhotoPt[2] = -fLen;
			MatrixMuti(dRMatrix, 3, 3, 1, dPhotoPt, dModelPt);
			pGoundPnt[i].dX = dXs + (pGoundPnt[i].dZ - dZs)*dModelPt[0] / dModelPt[2];
			pGoundPnt[i].dY = dYs + (pGoundPnt[i].dZ - dZs)*dModelPt[1] / dModelPt[2];
		}
	}

	//获取各个点的坐标
	for (i = 0; i<width*height; i++)
	{
		dModelPt[0] = pGoundPnt[i].dX;
		dModelPt[1] = pGoundPnt[i].dY;
		dModelPt[2] = pGoundPnt[i].dZ;
		MatrixMuti(dEMMatrix, 3, 3, 1, dModelPt, dPt);

		TemPnt.dX = Pnt.dX + dPt[0];
		TemPnt.dY = Pnt.dY + dPt[1];
		TemPnt.dZ = Pnt.dZ + dPt[2];

		m_coordiTrans.XYZToBLH(TemPnt, dTempB, dTempL, dTempH);
		pGoundPnt[i].dX = dTempB;
		pGoundPnt[i].dY = dTempL;

		tsmLatLongToUTM(pGoundPnt[i].dX, pGoundPnt[i].dY, &nZone, &x, &y);
		pGoundPnt[i].dX = x;
		pGoundPnt[i].dY = y;
		pGoundPnt[i].dZ = dTempH;
	}
	for (j = 0; j<width*height; j++)
	{
		//寻找最大最小点
		if (minPt.dX>pGoundPnt[j].dX)
			minPt.dX = pGoundPnt[j].dX;
		if (minPt.dY>pGoundPnt[j].dY)
			minPt.dY = pGoundPnt[j].dY;
		if (maxPt.dX<pGoundPnt[j].dX)
			maxPt.dX = pGoundPnt[j].dX;
		if (maxPt.dY<pGoundPnt[j].dY)
			maxPt.dY = pGoundPnt[j].dY;
	}
	double tmp1 = minPt.dX - rangPnt[0].dX;
	double tmp2 = minPt.dY - rangPnt[0].dY;
	double tmp3 = maxPt.dX - rangPnt[1].dX;
	double tmp4 = maxPt.dY - rangPnt[1].dY;
	if (!(tmp1>0 && tmp2<0 && tmp3<0 && tmp4>0))
	{
		printf("DEM范围小于影像范围\n");
		return 0;
	}
	else	//迭代求解
	{
		DPOINT tmpPnt;
		double xError, yError;
		int iterator_number = 0;
		//迭代求解准确的地理坐标
		for (i = 0; i<width; i++)
		{
			for (int j = 0; j < height; ++j)
			{
				memset(dPhotoPt, 0, 3 * sizeof(double));
				memset(dModelPt, 0, 3 * sizeof(double));

				//像平面坐标系转到像空间坐标系
				double img_pnt[2];
				dPhotoPt[0] = (i - width / 2)*ccdSize;
				dPhotoPt[1] = (height / 2 - j)*ccdSize;
				dPhotoPt[2] = -fLen;


				//像平面坐标系转到像空间坐标系
				MatrixMuti(dRMatrix, 3, 3, 1, dPhotoPt, dModelPt);
				pGoundPnt[i].dX = dXs + (pGoundPnt[i].dZ - dZs)*dModelPt[0] / dModelPt[2];
				pGoundPnt[i].dY = dYs + (pGoundPnt[i].dZ - dZs)*dModelPt[1] / dModelPt[2];
				tsmLatLongToUTM(pGoundPnt[i].dX, pGoundPnt[i].dY, &nZone, &x, &y);
				pGoundPnt[i].dX = x;
				pGoundPnt[i].dY = y;

				do
				{
					tmpPnt.dX = pGoundPnt[i].dX;
					tmpPnt.dY = pGoundPnt[i].dY;

					//获取高程值
					x = pGoundPnt[i].dX;
					y = pGoundPnt[i].dY;
					fDX = (x - originPt.dX) / dGSDX;
					fDY = (y - originPt.dY) / dGSDY;
					if (fDX>nDEMSamples - 1 || fDX<0 || fDY>nDEMLines - 1 || fDY<0)
						dElevation = pGoundPnt[i].dZ;
					else
					{
						nC = (int)fDX;
						nY = (int)fDY;
						//进行取余
						fDX -= nC;
						fDY -= nY;
						//双线性差值
						dElevation = (1 - fDX)*(1 - fDY)*pDEMPt[nY*nDEMSamples + nC] + fDX*(1 - fDY)*pDEMPt[nY*nDEMSamples + nC + 1] + (1 - fDX)*fDY*pDEMPt[(nY + 1)*nDEMSamples + nC] + fDX*fDY*pDEMPt[nY*(nDEMSamples + 1) + nC + 1];
					}

					pGoundPnt[i].dZ = dElevation;
					pGoundPnt[i].dX = dXs + (pGoundPnt[i].dZ - dZs)*dModelPt[0] / dModelPt[2];	//共线方程解算Xs点
					pGoundPnt[i].dY = dYs + (pGoundPnt[i].dZ - dZs)*dModelPt[1] / dModelPt[2]; 	//共线方程解算Ys点
					dGroundPnt[0] = pGoundPnt[i].dX;
					dGroundPnt[1] = pGoundPnt[i].dY;
					dGroundPnt[2] = pGoundPnt[i].dZ;
					MatrixMuti(dEMMatrix, 3, 3, 1, dGroundPnt, dPt);

					TemPnt.dX = Pnt.dX + dPt[0];
					TemPnt.dY = Pnt.dY + dPt[1];
					TemPnt.dZ = Pnt.dZ + dPt[2];

					m_coordiTrans.XYZToBLH(TemPnt, dTempB, dTempL, dTempH);
					pGoundPnt[i].dX = dTempB;
					pGoundPnt[i].dY = dTempL;

					tsmLatLongToUTM(pGoundPnt[i].dX, pGoundPnt[i].dY, &nZone, &x, &y);
					pGoundPnt[i].dX = x;
					pGoundPnt[i].dY = y;
					xError = pGoundPnt[i].dY - tmpPnt.dY;
					yError = pGoundPnt[i].dX - tmpPnt.dX;
					iterator_number++;
				} while ((abs(xError)>0.01 || abs(yError)>0.01) && iterator_number<30);

				MatrixMuti(dRMatrix, 3, 3, 1, dPhotoPt, dModelPt);
				pGoundPnt[i].dX = dXs + (pGoundPnt[i].dZ - dZs)*dModelPt[0] / dModelPt[2];
				pGoundPnt[i].dY = dYs + (pGoundPnt[i].dZ - dZs)*dModelPt[1] / dModelPt[2];
			}
		}
	}
	if (pDEMPt != NULL)
		delete[]pDEMPt;
	return 0;
}
