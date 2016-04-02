#include"stdafx.h"
#include"GeoPOSProcess.h"
#include"CoordinateTrans.h"
#include"Global.h"
#include<windows.h>
//根据POS数据解算EO元素
long GeoPOSProcess::GeoPOSProc_ExtractEO(const char *pPOSFile, int nLines, const  char *pEOFile, THREEDPOINT THETA, float *fSetupAngle, int nbeginLine)
{
	long   lError = 0;
	double  dB = 0, dL = 0, dH = 0;
	int nQuarFlag = 0;
	FILE *fEO = NULL;
	m_geo_EO = new EO[nLines];
	float tempangle1 = float((fSetupAngle[0])*PI / 180.0);
	float tempangle2 = float((fSetupAngle[1])*PI / 180.0);

	//安置角和安置矢量
	THREEDPOINT ANGLETHETA, POSISTTHETA;
	THREEDPOINT XYZPnt;
	CoordinateTrans m_coordinate;

	ANGLETHETA.dX = fSetupAngle[0]; ANGLETHETA.dY = fSetupAngle[1]; ANGLETHETA.dZ = fSetupAngle[2];
	POSISTTHETA.dX = THETA.dX;      POSISTTHETA.dY = THETA.dY;      POSISTTHETA.dZ = THETA.dZ;

	if ((lError = GeoPOSProc_ReadPartPOS(pPOSFile, nLines, dB, dL, dH, nbeginLine)) != 0)
		goto ErrEnd;

	m_geo_dB = dB;
	m_geo_dL = dL;
	m_geo_dH = dH;

	//成图坐标系到地心坐标系
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

	//物方坐标系以测区的重心为原点, 方向为东(X)北(Y)天(Z)的局部切平面(大地高为0)坐标系
	m_coordinate.BLHToXYZ(dB, dL, 0, XYZPnt);
	nQuarFlag = GeoPOSProc_EOQuadrant(m_nPOSLines, EMMatrix, XYZPnt);  //计算象限失败

	if (nQuarFlag == 0)
	{
		lError = -1; //生成EO文件失败
		goto ErrEnd;
	}

	//输出路径不为空则将EO数据输出
	if (pEOFile != NULL)
	{
		fopen_s(&fEO,pEOFile, "w");
		if (fEO == NULL)
		{
			lError = -1; //生成EO文件失败
			goto ErrEnd;
		}

		fprintf(fEO, "image_Line_Numbers:  %d\n", m_nPOSLines);
		fprintf(fEO, "EO_samples_of_lines: %d\n", 1);
		fprintf(fEO, "Average_Latitude: %.8f\n", dB);
		fprintf(fEO, "Average_Longitude: %.8f\n", dL);
		fprintf(fEO, "Average_Altitude: %.6f\n", dH);
		fprintf(fEO, "%20s%20s%20s%15s%15s%15s%10s%10s%10s%10s%10s%10s%10s%10s%10s",
			"Xs(m)", "Ys(m)", "Zs(m)", "phi(deg)", "omega(deg)", "kappa(deg)",
			"M[1]", "M[2]", "M[3]", "M[4]", "M[5]", "M[6]", "M[7]", "M[8]", "M[9]");

		ANGLETHETA.dX = ANGLETHETA.dX*PI / 180.0;
		ANGLETHETA.dY = ANGLETHETA.dY*PI / 180.0;
		ANGLETHETA.dZ = ANGLETHETA.dZ*PI / 180.0;
		for (int i = 0; i<m_nPOSLines; i++)
			GeoPOSProc_EOMatrixTurn(i, XYZPnt, nQuarFlag, EMMatrix, ANGLETHETA, POSISTTHETA, fEO);

	}
	else
	{
		ANGLETHETA.dX = ANGLETHETA.dX*PI / 180.0;
		ANGLETHETA.dY = ANGLETHETA.dY*PI / 180.0;
		ANGLETHETA.dZ = ANGLETHETA.dZ*PI / 180.0;
		for (int i = 0; i<m_nPOSLines; i++)
			GeoPOSProc_EOMatrixTurn(i, XYZPnt, nQuarFlag, EMMatrix, ANGLETHETA, POSISTTHETA, fEO);
	}
ErrEnd:
	if (fEO)
	{
		fclose(fEO);
		fEO = NULL;
	}

	return lError;
}


//根据POS数据计算航带所在象限
long GeoPOSProcess::GeoPOSProc_EOQuadrant(int nLines, double EMMatrix[], THREEDPOINT &XYZPoint)
{
	long nFlag = 0;
	CoordinateTrans m_coordinate;
	THREEDPOINT Pntstart, PntEnd;
	POS posBegin = this->m_geo_POS[0];
	POS posEnd = this->m_geo_POS[nLines - 1];
	m_coordinate.BLHToXYZ(posBegin.m_latitude, posBegin.m_longitude, posBegin.m_height, Pntstart);
	double dXstart = (Pntstart.dX - XYZPoint.dX)*EMMatrix[0] + (Pntstart.dY - XYZPoint.dY)*EMMatrix[1] + (Pntstart.dZ - XYZPoint.dZ)*EMMatrix[2];
	double dYstart = (Pntstart.dX - XYZPoint.dX)*EMMatrix[3] + (Pntstart.dY - XYZPoint.dY)*EMMatrix[4] + (Pntstart.dZ - XYZPoint.dZ)*EMMatrix[5];

	m_coordinate.BLHToXYZ(posEnd.m_latitude, posEnd.m_longitude, posEnd.m_height, PntEnd);			//WGS84
	double dXend = (PntEnd.dX - XYZPoint.dX)*EMMatrix[0] + (PntEnd.dY - XYZPoint.dY)*EMMatrix[1] + (PntEnd.dZ - XYZPoint.dZ)*EMMatrix[2];
	double dYend = (PntEnd.dX - XYZPoint.dX)*EMMatrix[3] + (PntEnd.dY - XYZPoint.dY)*EMMatrix[4] + (PntEnd.dZ - XYZPoint.dZ)*EMMatrix[5];

	double dx, dy;
	dx = dXend - dXstart;
	dy = dYend - dYstart;

	if (dx>0 && dy>0)		//东北--1象限
		nFlag = 1;
	else if (dx<0 && dy>0)    //西北--2象限
		nFlag = 2;
	else if (dx<0 && dy<0)    //西南--3象限
		nFlag = 3;
	else if (dx>0 && dy<0)    //东南--4象限
		nFlag = 4;

	return nFlag;
}

long GeoPOSProcess::GeoPOSProc_EOQuadrant(POS curPOS, double EMMatrix[], THREEDPOINT &XYZPoint)
{
	long nFlag = 0;
	double temps = curPOS.m_yaw*180.0 / PI;
	if (abs(int(temps)) % 360<10)					//North-East--1 quandrant
		nFlag = 1;
	else if (abs(int(temps - 90)) % 360<10)			//North_West--2 quandrant
		nFlag = 2;
	else if (abs(int(temps - 180)) % 360<10)		//South-West--3 quandrant
		nFlag = 3;
	else if (abs(int(temps - 270)) % 360<10)		//South-East--4 quandrant
		nFlag = 4;

	return nFlag;
}

//根据POS获取EO元素和旋转矩阵并输出到文件中批量处理
long GeoPOSProcess::GeoPOSProc_EOMatrixTurn(int nCurLine, THREEDPOINT &XYZPoint, int nQuadNum, double EMMatrix[], THREEDPOINT ANGLETHETA, THREEDPOINT POSISTTHETA, FILE *fEO)
{
	//rotate matrix 
	double EGMatrix[9];	//	
	double GIMatrix[9];	//
	double CIMatrix[9];	//
	double ICMatrix[9];	//
	POS pdfPOS = this->m_geo_POS[nCurLine];
	double dL, dB, dH;
	double roll, pitch, yaw;
	dB =/*pfPOS[6*curpos+0]*/pdfPOS.m_latitude;
	dL =/*pfPOS[6*curpos+1]*/pdfPOS.m_longitude;
	dH =/*pfPOS[6*curpos+2]*/pdfPOS.m_height;

	roll =/*pfPOS[6*curpos+3]*/pdfPOS.m_roll;
	pitch =/*pfPOS[6*curpos+4]*/pdfPOS.m_pitch;
	yaw =/*pfPOS[6*curpos+5]*/pdfPOS.m_yaw;

	//WGS84 trans to local coordinate system
	EGMatrix[0] = -sin(dB)*cos(dL); EGMatrix[1] = -sin(dL); EGMatrix[2] = -cos(dB)*cos(dL);
	EGMatrix[3] = -sin(dB)*sin(dL); EGMatrix[4] = cos(dL); EGMatrix[5] = -cos(dB)*sin(dL);
	EGMatrix[6] = cos(dB);		 EGMatrix[7] = 0; ; EGMatrix[8] = -sin(dB);

	//Local coordinate system to IMU coordinate system
	GIMatrix[0] = cos(pitch)*cos(yaw);
	GIMatrix[1] = sin(roll)*sin(pitch)*cos(yaw) - cos(roll)*sin(yaw);
	GIMatrix[2] = cos(roll)*sin(pitch)*cos(yaw) + sin(roll)*sin(yaw);
	GIMatrix[3] = cos(pitch)*sin(yaw);
	GIMatrix[4] = sin(roll)*sin(pitch)*sin(yaw) + cos(roll)*cos(yaw);
	GIMatrix[5] = cos(roll)*sin(pitch)*sin(yaw) - sin(roll)*cos(yaw);
	GIMatrix[6] = -sin(pitch);
	GIMatrix[7] = sin(roll)*cos(pitch);
	GIMatrix[8] = cos(roll)*cos(pitch);

	//IMU to sensor coordinate system trans
	CIMatrix[0] = cos(ANGLETHETA.dY)*cos(ANGLETHETA.dZ);
	CIMatrix[1] = cos(ANGLETHETA.dY)*sin(ANGLETHETA.dZ);
	CIMatrix[2] = -sin(ANGLETHETA.dY);
	CIMatrix[3] = sin(ANGLETHETA.dX)*sin(ANGLETHETA.dY)*cos(ANGLETHETA.dZ) - cos(ANGLETHETA.dX)*sin(ANGLETHETA.dZ);
	CIMatrix[4] = sin(ANGLETHETA.dX)*sin(ANGLETHETA.dY)*sin(ANGLETHETA.dZ) + cos(ANGLETHETA.dX)*cos(ANGLETHETA.dZ);
	CIMatrix[5] = sin(ANGLETHETA.dX)*cos(ANGLETHETA.dY);
	CIMatrix[6] = cos(ANGLETHETA.dX)*sin(ANGLETHETA.dY)*cos(ANGLETHETA.dZ) + sin(ANGLETHETA.dX)*sin(ANGLETHETA.dZ);
	CIMatrix[7] = cos(ANGLETHETA.dX)*sin(ANGLETHETA.dY)*sin(ANGLETHETA.dZ) - sin(ANGLETHETA.dX)*cos(ANGLETHETA.dZ);
	CIMatrix[8] = cos(ANGLETHETA.dX)*cos(ANGLETHETA.dY);

	//seneor to image coordinate system which could be modified according to different seneor
	ICMatrix[0] = 0; 	ICMatrix[1] = -1;	ICMatrix[2] = 0;
	ICMatrix[3] = -1;	ICMatrix[4] = 0;	ICMatrix[5] = 0;
	ICMatrix[6] = 0;	ICMatrix[7] = 0;	ICMatrix[8] = -1;

	double IMMatrix[9];
	double M1[9], M2[9], M3[9];
	double pVector[] = { 0,0,0 };

	MatrixMuti(EMMatrix, 3, 3, 3, EGMatrix, M1);
	MatrixMuti(M1, 3, 3, 3, GIMatrix, M2);
	MatrixMuti(M2, 3, 3, 3, CIMatrix, M3);
	MatrixMuti(M3, 3, 3, 3, ICMatrix, IMMatrix);

	double dPhi, dOmega, dKappa;
	dPhi = asin(-IMMatrix[2]);
	dOmega = atan(-IMMatrix[5] / IMMatrix[8]);
	if (nQuadNum == 1 || nQuadNum == 2)
	{
		dKappa = abs(atan(-IMMatrix[1] / IMMatrix[0]));
	}
	else
	{
		dKappa = (atan(-IMMatrix[1] / IMMatrix[0])) + PI;
	}

	//计算镜头透视中心的位置,地心坐标转换为切面坐标系（即成图坐标系m）坐标
	THREEDPOINT curPoint; CoordinateTrans m_tmpCoordinate;
	m_tmpCoordinate.BLHToXYZ(dB, dL, dH, curPoint);
	double dXs = (curPoint.dX - XYZPoint.dX)*EMMatrix[0] + (curPoint.dY - XYZPoint.dY)*EMMatrix[1] + (curPoint.dZ - XYZPoint.dZ)*EMMatrix[2];
	double dYs = (curPoint.dX - XYZPoint.dX)*EMMatrix[3] + (curPoint.dY - XYZPoint.dY)*EMMatrix[4] + (curPoint.dZ - XYZPoint.dZ)*EMMatrix[5];
	double dZs = (curPoint.dX - XYZPoint.dX)*EMMatrix[6] + (curPoint.dY - XYZPoint.dY)*EMMatrix[7] + (curPoint.dZ - XYZPoint.dZ)*EMMatrix[8];

	double Muvw[9] = { POSISTTHETA.dX,POSISTTHETA.dY,POSISTTHETA.dZ }, MXTZ[3] = { 0.0,0.0,0.0 }, MXYZ[3] = { 0.0,0.0,0.0 };
	//double Muvw[9]={0,0.4,1.3},MXTZ[3];
	//double MXYZ[9]={POSISTTHETA.dX,POSISTTHETA.dY,POSISTTHETA.dZ}, MXTZ[3]={0.0,0.0,0.0};
	//double MXYZ[9]={-7.9345,-9.2496,-0.6722},MXTZ[3]={0.0,0.0,0.0};
	MatrixMuti(M3, 3, 3, 1, Muvw, MXTZ);
	dXs = dXs + MXTZ[0] + MXYZ[0];
	dYs = dYs + MXTZ[1] + MXYZ[1];
	dZs = dZs + MXTZ[2] + MXYZ[2];

	m_geo_EO[nCurLine].m_dX = dXs;
	m_geo_EO[nCurLine].m_dY = dYs;
	m_geo_EO[nCurLine].m_dZ = dZs;

	m_geo_EO[nCurLine].m_phia = dPhi;
	m_geo_EO[nCurLine].m_omega = dOmega;
	m_geo_EO[nCurLine].m_kappa = dKappa;

	//double tMatrix[9];
	//MatrixRotate(tMatrix,dPhi,dOmega,dKappa);

	for (int n = 0; n<9; n++)
		m_geo_EO[nCurLine].m_dRMatrix[n] = IMMatrix[n];

	if (fEO != NULL)
	{
		fprintf(fEO, "\n%20.10f	%20.10f	%20.10f	%15.10f	%15.10f	%15.10f ", dXs, dYs, dZs, dPhi, dOmega, dKappa);
		for (int n = 0; n<9; n++)
			fprintf(fEO, "	%20.10f", IMMatrix[n]);
	}
	return 0;
}

long GeoPOSProcess::GeoPOSProc_EOMatrixTurn(POS pdfPOS, THREEDPOINT &XYZPoint, THREEDPOINT THETA, int nQuadNum, double EMMatrix[], EO &pdfEO)
{
	//rotate matrix 
	double EGMatrix[9];	//	
	double GIMatrix[9];	//
	double CIMatrix[9];	//
	double ICMatrix[9];	//

	double dL, dB, dH;
	double roll, pitch, yaw;
	dB =/*pfPOS[6*curpos+0]*/pdfPOS.m_latitude;
	dL =/*pfPOS[6*curpos+1]*/pdfPOS.m_longitude;
	dH =/*pfPOS[6*curpos+2]*/pdfPOS.m_height;

	roll =/*pfPOS[6*curpos+3]*/pdfPOS.m_roll;
	pitch =/*pfPOS[6*curpos+4]*/pdfPOS.m_pitch;
	yaw =/*pfPOS[6*curpos+5]*/pdfPOS.m_yaw;

	//WGS84 trans to local coordinate system
	EGMatrix[0] = -sin(dB)*cos(dL); EGMatrix[1] = -sin(dL); EGMatrix[2] = -cos(dB)*cos(dL);
	EGMatrix[3] = -sin(dB)*sin(dL); EGMatrix[4] = cos(dL); EGMatrix[5] = -cos(dB)*sin(dL);
	EGMatrix[6] = cos(dB);		 EGMatrix[7] = 0; ; EGMatrix[8] = -sin(dB);

	//Local coordinate system to IMU coordinate system
	GIMatrix[0] = cos(pitch)*cos(yaw);
	GIMatrix[1] = sin(roll)*sin(pitch)*cos(yaw) - cos(roll)*sin(yaw);
	GIMatrix[2] = cos(roll)*sin(pitch)*cos(yaw) + sin(roll)*sin(yaw);
	GIMatrix[3] = cos(pitch)*sin(yaw);
	GIMatrix[4] = sin(roll)*sin(pitch)*sin(yaw) + cos(roll)*cos(yaw);
	GIMatrix[5] = cos(roll)*sin(pitch)*sin(yaw) - sin(roll)*cos(yaw);
	GIMatrix[6] = -sin(pitch);
	GIMatrix[7] = sin(roll)*cos(pitch);
	GIMatrix[8] = cos(roll)*cos(pitch);

	//IMU to sensor coordinate system trans
	CIMatrix[0] = cos(THETA.dY)*cos(THETA.dZ);
	CIMatrix[1] = cos(THETA.dY)*sin(THETA.dZ);
	CIMatrix[2] = -sin(THETA.dY);
	CIMatrix[3] = sin(THETA.dX)*sin(THETA.dY)*cos(THETA.dZ) - cos(THETA.dX)*sin(THETA.dZ);
	CIMatrix[4] = sin(THETA.dX)*sin(THETA.dY)*sin(THETA.dZ) + cos(THETA.dX)*cos(THETA.dZ);
	CIMatrix[5] = sin(THETA.dX)*cos(THETA.dY);
	CIMatrix[6] = cos(THETA.dX)*sin(THETA.dY)*cos(THETA.dZ) + sin(THETA.dX)*sin(THETA.dZ);
	CIMatrix[7] = cos(THETA.dX)*sin(THETA.dY)*sin(THETA.dZ) - sin(THETA.dX)*cos(THETA.dZ);
	CIMatrix[8] = cos(THETA.dX)*cos(THETA.dY);

	//seneor to image coordinate system which could be modified according to different seneor
	ICMatrix[0] = 0; 	ICMatrix[1] = -1;	ICMatrix[2] = 0;
	ICMatrix[3] = -1;	ICMatrix[4] = 0;	ICMatrix[5] = 0;
	ICMatrix[6] = 0;	ICMatrix[7] = 0;	ICMatrix[8] = -1;

	double IMMatrix[9];
	double M1[9], M2[9], M3[9];
	double pVector[] = { 0,0,0 };

	MatrixMuti(EMMatrix, 3, 3, 3, EGMatrix, M1);
	MatrixMuti(M1, 3, 3, 3, GIMatrix, M2);
	MatrixMuti(M2, 3, 3, 3, CIMatrix, M3);
	MatrixMuti(M3, 3, 3, 3, ICMatrix, IMMatrix);

	double dPhi, dOmega, dKappa;

	dPhi = asin(-IMMatrix[2]);
	dOmega = atan(-IMMatrix[5] / IMMatrix[8]);
	if (nQuadNum == 1 || nQuadNum == 2)
	{
		dKappa = abs(atan(-IMMatrix[1] / IMMatrix[0]));
	}
	else
	{
		dKappa = abs(atan(-IMMatrix[1] / IMMatrix[0])) - PI / 2;
	}

	//get sensor center pos in the 
	THREEDPOINT curPoint;
	CoordinateTrans m_coordiTrans;
	m_coordiTrans.BLHToXYZ(dB, dL, dH, curPoint);
	double dXs = (curPoint.dX - XYZPoint.dX)*EMMatrix[0] + (curPoint.dY - XYZPoint.dY)*EMMatrix[1] + (curPoint.dZ - XYZPoint.dZ)*EMMatrix[2];
	double dYs = (curPoint.dX - XYZPoint.dX)*EMMatrix[3] + (curPoint.dY - XYZPoint.dY)*EMMatrix[4] + (curPoint.dZ - XYZPoint.dZ)*EMMatrix[5];
	double dZs = (curPoint.dX - XYZPoint.dX)*EMMatrix[6] + (curPoint.dY - XYZPoint.dY)*EMMatrix[7] + (curPoint.dZ - XYZPoint.dZ)*EMMatrix[8];

	// calculate the placement vector
	double transMatrix[9];
	MatrixMuti(EGMatrix, 3, 3, 3, GIMatrix, transMatrix);
	double dXl = transMatrix[0] * pVector[0] + transMatrix[1] * pVector[1] + transMatrix[2] * pVector[2];
	double dYl = transMatrix[3] * pVector[0] + transMatrix[4] * pVector[1] + transMatrix[5] * pVector[2];
	double dZl = transMatrix[6] * pVector[0] + transMatrix[7] * pVector[1] + transMatrix[8] * pVector[2];

	pdfEO.m_dX = dXs + dXl;
	pdfEO.m_dY = dYs + dYl;
	pdfEO.m_dZ = dZs + dZl;
	pdfEO.m_phia = dPhi;
	pdfEO.m_omega = dOmega;
	pdfEO.m_kappa = dKappa;

	//double tMatrix[9];
	//MatrixRotate(tMatrix,dPhi,dOmega,dKappa);

	for (int n = 0; n<9; n++)
		pdfEO.m_dRMatrix[n] = IMMatrix[n];

	return 0;
}

//从文件中读取EO元素
long GeoPOSProcess::GeoPOSProc_ReadEOFile(const char* pathEO, double &dB, double &dL, double &dH)
{
	FILE *fp = NULL;
	errno_t err = 0;
	err=fopen_s(&fp,pathEO, "r");
	if (err!=0)
		return -1;

	int nImgLines = 0;
	int nPOSSamples;
	double dLatitude, dLongitude, dFlightHeight;
	double pdEOData[15];
	char szTemp[256];
	fscanf_s(fp, "%s%d", szTemp,256, &nImgLines);
	fscanf_s(fp, "%s%d", szTemp, 256, &nPOSSamples);
	fscanf_s(fp, "%s%lf", szTemp, 256, &dLatitude);
	fscanf_s(fp, "%s%lf", szTemp, 256, &dLongitude);
	fscanf_s(fp, "%s%lf", szTemp, 256, &dFlightHeight);
	fscanf_s(fp, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", szTemp, 256, szTemp, 256,szTemp, 256, szTemp, 256, szTemp, 256, szTemp
		, 256, szTemp, 256, szTemp, 256, szTemp, 256, szTemp, 256, szTemp, 256,szTemp, 256,szTemp, 256,szTemp, 256, szTemp, 256);

	int nEOLines = nImgLines*nPOSSamples;
	if (m_geo_EO == NULL)
		m_geo_EO = new EO[nEOLines];
	else
	{
		delete[]m_geo_EO;
		m_geo_EO = new EO[nEOLines];
	}
	for (int i = 0; i<nEOLines; i++)
	{

		fscanf_s(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&pdEOData[0], &pdEOData[1],&pdEOData[2], &pdEOData[3], &pdEOData[4],
			&pdEOData[5], &pdEOData[6], &pdEOData[7], &pdEOData[8], &pdEOData[9],
			&pdEOData[10],&pdEOData[11],&pdEOData[12],&pdEOData[13], &pdEOData[14]);

		m_geo_EO[i].m_dX = pdEOData[0];
		m_geo_EO[i].m_dY = pdEOData[1];
		m_geo_EO[i].m_dZ = pdEOData[2];
		m_geo_EO[i].m_phia = pdEOData[3];
		m_geo_EO[i].m_omega = pdEOData[4];
		m_geo_EO[i].m_kappa = pdEOData[5];
		for (int j = 0; j<9; j++)
			m_geo_EO[i].m_dRMatrix[j] = pdEOData[j + 6];
	}
	dL = dLongitude;
	dB = dLatitude;
	dH = dFlightHeight;
	fclose(fp);



	return 0;
}
//===========================================================================================================================================================================================================
//从SBET文件中解算POS数据
long QPDGeoPOSProcess::GeoPOSProc_ExtractSBET(const char* pathSBET, const char* pathEvent, const char* pathPOS, float fOffsetGPS)
{
	long lError = 0;
	FILE *fSbet = NULL, *fEvent = NULL, *fPos = NULL;
	errno_t err = 0;

	//打开SBET文件、EVENT文件、POS文件
	err = fopen_s(&fSbet, pathSBET, "rb");		//二进制
	if (err)
	{
		lError = -1;
	}
	err = fopen_s(&fEvent, pathEvent, "r");		//二进制
	if (err)
	{
		lError = -1;
	}
	err = fopen_s(&fPos, pathPOS, "w");		//二进制
	if (err)
	{
		lError = -1;
	}

	//写入第一行标题
	fprintf(fPos, "%s\n", "SBET文件解算POS文件——中国科学院上海技术物理研究所");

	//写入每列参数信息
	//格式
	fprintf(fPos, "%18s", "Time(s)");
	fprintf(fPos, "%18s", "Latitude(rad)");
	fprintf(fPos, "%18s", "Longitude(rad)");
	fprintf(fPos, "%18s", "Altitude(m)");

	fprintf(fPos, "%18s", "Roll(rad)");
	fprintf(fPos, "%18s", "Pitch(rad)");
	fprintf(fPos, "%18s", "PlatHeading(rad)");
	fprintf(fPos, "%18s", "WanderAngle(rad)");

	fprintf(fPos, "%18s", "X_Velocity(m/s)");
	fprintf(fPos, "%18s", "Y_Velocity(m/s)");
	fprintf(fPos, "%18s", "Z_Velocity(m/s)");

	fprintf(fPos, "%22s", "X_Acceralation(m/s/s)");
	fprintf(fPos, "%22s", "Y_Acceralation(m/s/s)");
	fprintf(fPos, "%22s", "Z_Acceralation(m/s/s)");

	fprintf(fPos, "%22s", "X_Angual Rate(rad/s)");
	fprintf(fPos, "%22s", "Y_Angual Rate(rad/s)");
	fprintf(fPos, "%22s", "Z_Angual Rate(rad/s)");

	int ss = 0;
	double t, t1, t2, a, b;
	SBETELEMENT stLine, stFormerLine, stLatterLine;
	//	char tmp[256];

	//	fgets(tmp, 256, fEvent);	//读取第一行信息
	_fseeki64(fSbet, 0, SEEK_SET);
	fread(&stLatterLine, sizeof(SBETELEMENT), 1, fSbet);	//读取SBET文件

	stFormerLine.dGpsTime = stLatterLine.dGpsTime;
	stFormerLine.dLatitude = stLatterLine.dLatitude;
	stFormerLine.dLongitude = stLatterLine.dLongitude;
	stFormerLine.dHeight = stLatterLine.dHeight;

	stFormerLine.dRoll = stLatterLine.dRoll;
	stFormerLine.dPitch = stLatterLine.dPitch;
	stFormerLine.dHeading = stLatterLine.dHeading;
	stFormerLine.dWander = stLatterLine.dWander;

	stFormerLine.dVx = stLatterLine.dVx;
	stFormerLine.dVy = stLatterLine.dVy;
	stFormerLine.dVz = stLatterLine.dVz;

	stFormerLine.dAx = stLatterLine.dAx;
	stFormerLine.dAy = stLatterLine.dAy;
	stFormerLine.dAz = stLatterLine.dAz;

	stFormerLine.dArx = stLatterLine.dArx;
	stFormerLine.dAry = stLatterLine.dAry;
	stFormerLine.dArz = stLatterLine.dArz;

	int i = 0;
	while (!feof(fEvent))
	{
		//	fgets(tmp, 256, fEvent);			//读取第一行EVENT文件
		fscanf_s(fEvent, "%d	%lf", &ss, &t);
		t += fOffsetGPS;
		i++;
		while (!feof(fSbet))
		{
			fread(&stLatterLine, sizeof(SBETELEMENT), 1, fSbet);	//读取下一行SBET文件
			t1 = stFormerLine.dGpsTime;
			t2 = stLatterLine.dGpsTime;

			if (t >= t1 && t <= t2)
			{
				//t1<t<t2，查找到触发行，进行加权
				a = (t2 - t) / (t2 - t1);
				b = (t - t1) / (t2 - t1);

				stLine.dGpsTime = t;

				stLine.dLatitude = (a*stFormerLine.dLatitude + b*stLatterLine.dLatitude);
				stLine.dLongitude = (a*stFormerLine.dLongitude + b*stLatterLine.dLongitude);
				stLine.dHeight = a*stFormerLine.dHeight + b*stLatterLine.dHeight;

				stLine.dVx = a*stFormerLine.dVx + b*stLatterLine.dVx;
				stLine.dVy = a*stFormerLine.dVy + b*stLatterLine.dVy;
				stLine.dVz = a*stFormerLine.dVz + b*stLatterLine.dVz;

				stLine.dRoll = (a*stFormerLine.dRoll + b*stLatterLine.dRoll);
				stLine.dPitch = (a*stFormerLine.dPitch + b*stLatterLine.dPitch);
				stLine.dHeading = (a*stFormerLine.dHeading + b*stLatterLine.dHeading);
				stLine.dWander = (a*stFormerLine.dWander + b*stLatterLine.dWander);

				stLine.dAx = a*stFormerLine.dAx + b*stLatterLine.dAx;
				stLine.dAy = a*stFormerLine.dAy + b*stLatterLine.dAy;
				stLine.dAz = a*stFormerLine.dAz + b*stLatterLine.dAz;

				stLine.dArx = a*stFormerLine.dArx + b*stLatterLine.dArx;
				stLine.dAry = a*stFormerLine.dAry + b*stLatterLine.dAry;
				stLine.dArz = a*stFormerLine.dArz + b*stLatterLine.dArz;
				//写入提取的POS数据
				fprintf(fPos, "\n%18.10f", stLine.dGpsTime);

				fprintf(fPos, "%18.10f", stLine.dLatitude);
				fprintf(fPos, "%18.10f", stLine.dLongitude);
				fprintf(fPos, "%18.10f", stLine.dHeight);

				fprintf(fPos, "%18.10f", stLine.dRoll);
				fprintf(fPos, "%18.10f", stLine.dPitch);
				fprintf(fPos, "%18.10f", stLine.dHeading);
				fprintf(fPos, "%18.10f", stLine.dWander);

				fprintf(fPos, "%18.10f", stLine.dVx);
				fprintf(fPos, "%18.10f", stLine.dVy);
				fprintf(fPos, "%18.10f", stLine.dVz);

				fprintf(fPos, "%22.10f", stLine.dAx);
				fprintf(fPos, "%22.10f", stLine.dAy);
				fprintf(fPos, "%22.10f", stLine.dAz);

				fprintf(fPos, "%22.10f", stLine.dArx);
				fprintf(fPos, "%22.10f", stLine.dAry);
				fprintf(fPos, "%22.10f", stLine.dArz);

				break;
			}
			else
			{//未找到触发行，将查找的后一行值赋给前一行
				stFormerLine.dGpsTime = stLatterLine.dGpsTime;
				stFormerLine.dLatitude = stLatterLine.dLatitude;
				stFormerLine.dLongitude = stLatterLine.dLongitude;
				stFormerLine.dHeight = stLatterLine.dHeight;

				stFormerLine.dRoll = stLatterLine.dRoll;
				stFormerLine.dPitch = stLatterLine.dPitch;
				stFormerLine.dHeading = stLatterLine.dHeading;
				stFormerLine.dWander = stLatterLine.dWander;

				stFormerLine.dVx = stLatterLine.dVx;
				stFormerLine.dVy = stLatterLine.dVy;
				stFormerLine.dVz = stLatterLine.dVz;

				stFormerLine.dAx = stLatterLine.dAx;
				stFormerLine.dAy = stLatterLine.dAy;
				stFormerLine.dAz = stLatterLine.dAz;

				stFormerLine.dArx = stLatterLine.dArx;
				stFormerLine.dAry = stLatterLine.dAry;
				stFormerLine.dArz = stLatterLine.dArz;
			}
		}
	}

	fclose(fSbet);
	fclose(fEvent);
	fclose(fPos);
	return lError;
}

//读取部分POS数据
long QPDGeoPOSProcess::GeoPOSProc_ReadPartPOS(const char *pPOSFile, long nLines, double &dB, double &dL, double &dH, int nbeginLine)
{
	long   lError = 0;
	int    i;
	FILE   *fpin = NULL;
	char   cA[111], cB[111];
	double fReadData[20];
	double fTemp[6];
	char   cTempchar[2048];
	int realLines = 0;
	if (m_geo_POS != NULL)
		delete[]m_geo_POS;
	m_geo_POS = NULL;
	m_geo_POS = new POS[nLines];
	if ((fopen_s(&fpin,pPOSFile, "r")) == NULL)
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

	//实际的经纬度都是以弧度形式给出的
	for (i = 0; i < nLines&&!feof(fpin); i++)
	{
		realLines++;
		if (feof(fpin))
		{
			lError = 40308; //读取POS文件失败
			goto ErrEnd;
		}
		fgets(cTempchar, 2048, fpin);
		lError = sscanf_s(
			cTempchar,
			"%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
			&fReadData[0], &fReadData[1], &fReadData[2], &fReadData[3],
			&fReadData[4], &fReadData[5], &fReadData[6], &fReadData[7],
			&fReadData[8], &fReadData[9], &fReadData[10], &fReadData[11],
			&fReadData[12], &fReadData[13], &fReadData[14], &fReadData[15], &fReadData[16]);
		if (lError == 0)
		{
			lError = -1; //解析POS文件失败
			goto ErrEnd;
		}

		if (fReadData[6] < -180.0 || fReadData[6] > 180.0)
		{
			lError = 40316;//经度有问题，不是-180到180度之间
			goto ErrEnd;
		}
		if (fReadData[5] <= -90.0 || fReadData[5] >= 90.0)
		{
			lError = 40317;//纬度有问题，不是-90到90度之间
			goto ErrEnd;
		}
		dB += fReadData[1];
		dL += fReadData[2];
		dH += fReadData[3];

		this->m_geo_POS[i].m_latitude = double(fReadData[1]);				 //X
		this->m_geo_POS[i].m_longitude = double(fReadData[2]);				 //Y
		this->m_geo_POS[i].m_height = double(fReadData[3]);                 //height
		this->m_geo_POS[i].m_roll = double(fReadData[4]);				 //roll
		this->m_geo_POS[i].m_pitch = double(fReadData[5]);				 //pitch
		this->m_geo_POS[i].m_yaw = double(fReadData[6]);				 //heading
	}
	m_nPOSLines = realLines;
	dB /= nLines; /*dB = dB*PI/180.0;*/
	dL /= nLines; /*dL = dL*PI/180.0;*/
	dH /= nLines; ;

ErrEnd:
	if (fpin)
	{
		fclose(fpin);
		fpin = NULL;
	}
	return lError;
}

//获取EO元素的行数
long QPDGeoPOSProcess::GeoPOSProc_GetEOLines(const char *pEoFile, int &nEOLines)
{
	long lError = 0;
	FILE *fEO = NULL;
	if(fopen_s(&fEO,pEoFile, "r")!=0)		//打开文件
	{
		lError = -1;
		goto ErrEnd;
	}
	char s[50];
	fscanf_s(fEO, "%s%d", s,50, &nEOLines);
ErrEnd:
	if (fEO)
	{
		fclose(fEO);
		fEO = NULL;
	}
	return lError;
}