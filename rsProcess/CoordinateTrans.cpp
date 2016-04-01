#include"stdafx.h"
#include"CoordinateTrans.h"
#include"GeoPOSProcess.h"

//经纬度转地心直角坐标系坐标
long CoordinateTrans::BLHToXYZ(double dB, double dL, double dH, THREEDPOINT &XYZPnt)//经纬度->WGS84
{
	double dN = WGS84LRadius / sqrt(1 - WGS84Eccentricity*sin(dB)*sin(dB));
	XYZPnt.dX = (dN + dH)*cos(dB)*cos(dL);
	XYZPnt.dY = (dN + dH)*cos(dB)*sin(dL);
	XYZPnt.dZ = (dN*(1 - WGS84Eccentricity) + dH)*sin(dB);
	return 0;
}

//地心直角坐标系转经纬度
long CoordinateTrans::XYZToBLHS(double dB, double dL, double dH, THREEDPOINT *pGoundPnt, int PxelNum)
{
	THREEDPOINT Pnt;
	this->BLHToXYZ(dB, dL, 0, Pnt);
	double dEMMatrix[9], dPt[3], dModelPt[3];
	double dTempB, dTempL, dTempH;
	if (pGoundPnt == NULL)
		return  -1;
	dEMMatrix[0] = -sin(dL);
	dEMMatrix[1] = -sin(dB)*cos(dL);
	dEMMatrix[2] = cos(dB)*cos(dL);
	dEMMatrix[3] = cos(dL);
	dEMMatrix[4] = -sin(dB)*sin(dL);
	dEMMatrix[5] = cos(dB)*sin(dL);
	dEMMatrix[6] = 0;
	dEMMatrix[7] = cos(dB);
	dEMMatrix[8] = sin(dB);

	DPOINT minPt, maxPt;
	minPt.dX = MAX_NUM; minPt.dY = MAX_NUM;
	maxPt.dX = MIN_NUM; maxPt.dY = MIN_NUM;
	THREEDPOINT TemPnt;

	for (int i = 0; i<PxelNum; i++)
	{
		dModelPt[0] = pGoundPnt[i].dX;
		dModelPt[1] = pGoundPnt[i].dY;
		dModelPt[2] = pGoundPnt[i].dZ;

		MatrixMuti(dEMMatrix, 3, 3, 1, dModelPt, dPt);

		TemPnt.dX = Pnt.dX + dPt[0];
		TemPnt.dY = Pnt.dY + dPt[1];
		TemPnt.dZ = Pnt.dZ + dPt[2];

		XYZToBLH(TemPnt, dTempB, dTempL, dTempH);
		pGoundPnt[i].dX = dTempL;
		pGoundPnt[i].dY = dTempB;
	}

	return 0;
}

//地形直角坐标系转经纬度
long CoordinateTrans::XYZToBLH(THREEDPOINT XYZPnt, double &dB, double &dL, double &dH)//WGS84->经纬度
{
	double dB0 = 0;
	double dB1 = 0;
	double dThreshold = 0;
	double dN = 0;

	dB0 = atan(XYZPnt.dZ / sqrt(XYZPnt.dX*XYZPnt.dX + XYZPnt.dY*XYZPnt.dY));
	do
	{
		dN = WGS84LRadius / sqrt(1 - WGS84Eccentricity*sin(dB0)*sin(dB0));
		dB1 = atan((XYZPnt.dZ + dN*WGS84Eccentricity*sin(dB0)) / sqrt(XYZPnt.dX*XYZPnt.dX + XYZPnt.dY*XYZPnt.dY));
		dThreshold = fabs(dB0 - dB1);
		dB0 = dB1;
	} while (dThreshold >= 0.00001);
	dB = dB0;
	dH = XYZPnt.dZ / sin(dB) - dN*(1 - WGS84Eccentricity);
	dL = atan(fabs(XYZPnt.dY / XYZPnt.dX)) * 180 / PI;
	if (XYZPnt.dX>0 && XYZPnt.dY>0)
	{
		;
	}
	if (XYZPnt.dX<0 && XYZPnt.dY>0)
	{
		dL = 180 - dL;
	}
	if (XYZPnt.dX<0 && XYZPnt.dY<0)
	{
		dL = 180 + dL;
	}
	if (XYZPnt.dX>0 && XYZPnt.dY<0)
	{
		dL = 360 - dL;
	}
	dB = dB * 180 / PI;
	return 0;
}