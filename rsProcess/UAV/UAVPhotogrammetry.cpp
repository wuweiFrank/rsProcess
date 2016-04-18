#include"UAVPhotogrammetry.h"
#include"..\matrixOperation.h"
#include"..\Global.h"
void PhotogrammetryToolsTest()
{
	CornerCoordi4 m_coordiTrans4;
	m_coordiTrans4.xPixel[0] = 10566;	m_coordiTrans4.xPixel[1] = 466;
	m_coordiTrans4.xPixel[2] = 475.5;	m_coordiTrans4.xPixel[3] = 10576;
	m_coordiTrans4.yPixel[0] = 587.5;	m_coordiTrans4.yPixel[1] = 596.5;
	m_coordiTrans4.yPixel[2] = 10698;	m_coordiTrans4.yPixel[3] = 10688.3;

	m_coordiTrans4.xPic[0] = 106.002;	m_coordiTrans4.xPic[1] = -106.001;
	m_coordiTrans4.xPic[2] = -106.000;	m_coordiTrans4.xPic[3] = 105.999;
	m_coordiTrans4.yPic[0] = -106.003;	m_coordiTrans4.yPic[1] = -106.004;
	m_coordiTrans4.yPic[2] = 106.002;	m_coordiTrans4.yPic[3] = 106.002;
	UAVPhotogrammetryTools m_Tools;
	m_Tools.UAVPhotogrammetryTools_PixelToImgTrans(m_coordiTrans4);
	printf("%lf  %lf  %lf\n", m_Tools.m_PixelToImgTrans[0], m_Tools.m_PixelToImgTrans[1], m_Tools.m_PixelToImgTrans[2]);
	printf("%lf  %lf  %lf\n", m_Tools.m_PixelToImgTrans[3], m_Tools.m_PixelToImgTrans[4], m_Tools.m_PixelToImgTrans[5]);
}


//获取数据转换系数
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_PixelToImgTrans(CornerCoordi4 m_coordiTrans4)
{
	double paramO[6 * 8];
	double paramT[6 * 8];
	double paramL[8];
	double paramM[6 * 6];
	double paramI[6 * 6];
	double paramR[6];

	//这个两个转换 x，y的放一起了 
	for (int i = 0; i < 4; ++i)
	{
		paramO[12 * i + 0] = 1.0;
		paramO[12 * i + 1] = m_coordiTrans4.xPixel[i];
		paramO[12 * i + 2] = m_coordiTrans4.yPixel[i];
		paramO[12 * i + 3] = 0.0;
		paramO[12 * i + 4] = 0.0;
		paramO[12 * i + 5] = 0.0;

		paramO[12 * i + 6] = 0.0;
		paramO[12 * i + 7] = 0.0;
		paramO[12 * i + 8] = 0.0;
		paramO[12 * i + 9] = 1.0;
		paramO[12 * i + 10] = m_coordiTrans4.xPixel[i];
		paramO[12 * i + 11] = m_coordiTrans4.yPixel[i];
		
		paramL[2 * i + 0] = m_coordiTrans4.xPic[i];
		paramL[2 * i + 1] = m_coordiTrans4.yPic[i];

	}

	//仿射变换系数
	MatrixTrans(paramO,8,6,paramT);
	MatrixMuti(paramT, 6, 8, 6, paramO, paramM);
	MatrixMuti(paramT, 6, 8, 1, paramL, paramR);
	MatrixInverse(paramM, 6, paramI);
	MatrixMuti(paramI, 6, 6, 1, paramR, m_PixelToImgTrans);

	return 0;
}
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_PixelToImgTrans(CornerCoordi8 m_coordiTrans8)
{
	double paramO[6 * 16];
	double paramT[6 * 16];
	double paramL[16];
	double paramM[6 * 6];
	double paramI[6 * 6];
	double paramR[6];

	//这个两个转换 x，y的放一起了 
	for (int i = 0; i < 8; ++i)
	{
		paramO[12 * i + 0] = 1.0;
		paramO[12 * i + 1] = m_coordiTrans8.xPixel[i];
		paramO[12 * i + 2] = m_coordiTrans8.yPixel[i];
		paramO[12 * i + 3] = 0.0;
		paramO[12 * i + 4] = 0.0;
		paramO[12 * i + 5] = 0.0;

		paramO[12 * i + 6] = 0.0;
		paramO[12 * i + 7] = 0.0;
		paramO[12 * i + 8] = 0.0;
		paramO[12 * i + 9] = 1.0;
		paramO[12 * i + 10] = m_coordiTrans8.xPixel[i];
		paramO[12 * i + 11] = m_coordiTrans8.yPixel[i];

		paramL[2 * i + 0] = m_coordiTrans8.xPic[i];
		paramL[2 * i + 1] = m_coordiTrans8.yPic[i];
	}

	//仿射变换系数
	MatrixTrans(paramO, 16, 6, paramT);
	MatrixMuti(paramT, 6, 16, 6, paramO, paramM);
	MatrixMuti(paramT, 6, 16, 1, paramL, paramR);
	MatrixInverse(paramM, 6, paramI);
	MatrixMuti(paramI, 6, 6, 1, paramR, m_PixelToImgTrans);

	return 0;
}

//影像数据点的内定向
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_IOrientation(vector<Point2f> &pointImg)
{
	for (size_t i = 0; i < pointImg.size(); i++)
	{
		double x = pointImg[i].x;
		double y = pointImg[i].y;

		pointImg[i].x = m_PixelToImgTrans[0] + m_PixelToImgTrans[1] * x + m_PixelToImgTrans[2] * y;
		pointImg[i].y = m_PixelToImgTrans[3] + m_PixelToImgTrans[4] * x + m_PixelToImgTrans[5] * y;
	}
	return 0;
}

//空间前方交会
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_UAVFesction(vector<Point2f> pnt1, EO imgEO1, vector<Point2f> pnt2, EO imgEO2, vector<Point3d> points)
{


	return 0;
}

//空间后方交会
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_UAVResction(vector<Point2f> pnt1, vector<Point3d> pnt2, double Height, EO &eoElement)
{
	UAVPhotogrammetryTools_IOrientation(pnt1);
	int point_num = pnt1.size();
	double *A = NULL, *AT = NULL, *L = NULL, *LM = NULL;
	try
	{
		A = new double[12 * point_num];
		AT = new double[12 * point_num];
		L = new double[2 * point_num];
		LM = new double[2 * point_num];
	}
	catch (bad_alloc &e)
	{
		printf("%s", e.what());
		exit(-1);
	}

	double FA[25];
	double CH[6];
	double var_add[6] = { 100,100,100,100,100,100 };				//变量的增量
	int iteratornumber = 0;

	//必须给出初始参数
	memset(&eoElement, 0, sizeof(EO));
	for (int i = 0; i < point_num; ++i)
	{
		eoElement.m_dX = pnt2[i].x / point_num;
		eoElement.m_dY = pnt2[i].y / point_num;
		eoElement.m_dZ = pnt2[i].z / point_num;
	}
	MatrixRotate(eoElement.m_dRMatrix, eoElement.m_phia, eoElement.m_omega, eoElement.m_kappa);

	while ((OVER_LIMIT(var_add[0]) || OVER_LIMIT(var_add[1]) || OVER_LIMIT(var_add[2]) ||
			OVER_LIMIT(var_add[3]) || OVER_LIMIT(var_add[4])) /*&& MaxTime < 20*/)
	{
		memset(FA, 0, sizeof(double) * 25);
		memset(CH, 0, sizeof(double) * 5);
		int num = 0;
		for (int i = 0; i < point_num; ++i)
		{
			A[12 * i + 0] = -m_fLen / Height; A[12 * i + 1] = 0; A[12 * i + 2] = -pnt1[i].x / Height;
			A[12 * i + 3] = 0; A[12 * i + 4] = -m_fLen / Height; A[12 * i + 5] = -pnt1[i].y / Height;
			
			A[12 * i + 6] = -m_fLen *(1+ pnt1[i].x*pnt1[i].x/ m_fLen/ m_fLen); A[12 * i + 7] = -pnt1[i].x*pnt1[i].y/ m_fLen; A[12 * i + 8] = pnt1[i].y;
			A[12 * i + 9] = -pnt1[i].x*pnt1[i].y / m_fLen; A[12 * i + 10] = -m_fLen *(1 + pnt1[i].x*pnt1[i].x / m_fLen / m_fLen); A[12 * i + 11] = -pnt1[i].x;
		
			//计算结果
			//double a1, a2, a3, b1, b2, b3, c1, c2, c3;
			//a1 = eoElement.m_dRMatrix[0]; a2 = eoElement.m_dRMatrix[1]; a3 = eoElement.m_dRMatrix[2];
			//b1 = eoElement.m_dRMatrix[3]; b2 = eoElement.m_dRMatrix[4]; b3 = eoElement.m_dRMatrix[5];
			//c1 = eoElement.m_dRMatrix[6]; c2 = eoElement.m_dRMatrix[7]; c3 = eoElement.m_dRMatrix[8];
			double tmp[3],tmpM[3];
			tmp[0] = pnt2[i].x - eoElement.m_dX;
			tmp[1] = pnt2[i].y - eoElement.m_dY;
			tmp[2] = pnt2[i].z - eoElement.m_dZ;
			MatrixMuti(eoElement.m_dRMatrix, 3, 3, 1, tmp, tmpM);
			double x = -m_fLen*tmpM[0] / tmpM[2];
			double y = -m_fLen*tmpM[1] / tmpM[2];
			L[2 * i + 0] = pnt1[i].x - x;
			L[2 * i + 0] = pnt1[i].y - y;
		}
		double IFA[36];
		double MCH[6];
		MatrixTrans(A, point_num * 2, 6, AT);
		MatrixMuti(AT, 6,point_num * 2, 6, A, FA);
		MatrixMuti(AT, 6, point_num * 2, 1, L, CH);
		MatrixInverse(FA, 6, IFA);
		MatrixMuti(IFA, 6, 6, 1, CH, MCH);

		eoElement.m_dX += MCH[0];eoElement.m_dY += MCH[1];eoElement.m_dZ += MCH[2];
		eoElement.m_phia += MCH[3];eoElement.m_omega += MCH[4];eoElement.m_kappa += MCH[5];
		MatrixRotate(eoElement.m_dRMatrix, eoElement.m_phia, eoElement.m_omega, eoElement.m_kappa);
	}
	A = new double[12 * point_num];
	AT = new double[12 * point_num];
	L = new double[2 * point_num];
	LM = new double[2 * point_num];

	if (A != NULL)
		delete[]A;
	if (AT != NULL)
		delete[]AT;
	if (L != NULL)
		delete[]L;
	if (LM != NULL)
		delete[]LM;
	A = NULL; AT = NULL; L = NULL; LM = NULL;
	return 0;
}


//相对定向
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_ROrientation(vector<Point2f> pnt1, vector<Point2f> pnt2, REO &reoRElementL, REO &reoRElementR)
{
	//必须先做内定向，将像素坐标转换到相片坐标
	UAVPhotogrammetryTools_IOrientation(pnt1);
	UAVPhotogrammetryTools_IOrientation(pnt2);

	//然后进行相对定向获取相对定向元素
	int num_point = pnt1.size();
	double var_add[5] = { 100,100,100,100,100 };				//五个变量的增量
																//临时变量
	double var[5];											//5个参数
	memset(var, 0, 5 * sizeof(double));
	double B[3];
	B[0] = pnt1[1].x - pnt2[1].x;//Bx本应当取2号点(6个点时)的视差，此时随机取2号点
											//var[3]=atan((pnts_left[1].y-pnts_right[1].y)/(pnts_left[1].x-pnts_right[2].x));

	unsigned MaxTime = 0;					 //最大运算次数
	double *A = new double[5 * num_point];
	double *BP = new double[4 * num_point];
	double *L = new double[num_point];

	double FA[25];							//法方程系数
	double CH[5];							//常数项
	double a1r, a2r, a3r, b1r, b2r, b3r, c1r, c2r, c3r;
	double a1l, a2l, a3l, b1l, b2l, b3l, c1l, c2l, c3l;

	double phial = reoRElementL.m_phia;
	double omegal = reoRElementL.m_omega;
	double kappal = reoRElementL.m_kappa;

	a1l = cos(phial)*cos(kappal) + sin(phial)*sin(omegal)*sin(kappal);
	a2l = -cos(phial)*sin(omegal) - sin(phial)*sin(omegal)*sin(kappal);
	a3l = -sin(phial)*cos(omegal);

	b1l = cos(omegal)*sin(kappal);
	b2l = cos(omegal)*cos(kappal);
	b3l = -sin(omegal);

	c1l = sin(phial)*cos(kappal) + cos(phial)*sin(omegal)*sin(kappal);
	c2l = -sin(omegal)*sin(kappal) + cos(phial)*sin(omegal)*sin(kappal);
	c3l = cos(phial)*cos(omegal);

	int iteratornumber = 0;
	while ((OVER_LIMIT(var_add[0]) || OVER_LIMIT(var_add[1]) || OVER_LIMIT(var_add[2]) ||
		OVER_LIMIT(var_add[3]) || OVER_LIMIT(var_add[4])) /*&& MaxTime < 20*/)
	{
		memset(FA, 0, sizeof(double) * 25);
		memset(CH, 0, sizeof(double) * 5);
		int num = 0;

		//不严密求解
		for (int i = 0; i< num_point; ++i)
		{
			num++;
			double R_XYZ[3], L_XYZ[3];
			double phiar = var[0]; double omegar = var[1]; double kappar = var[2];
			//微小角运算
			//计算X2,Y2,Z2; X1, Y1, Z1
			a1r = cos(phiar)*cos(kappar) + sin(phiar)*sin(omegar)*sin(kappar);
			a2r = -cos(phiar)*sin(omegar) - sin(phiar)*sin(omegar)*sin(kappar);
			a3r = -sin(phiar)*cos(omegar);

			b1r = cos(omegar)*sin(kappar);
			b2r = cos(omegar)*cos(kappar);
			b3r = -sin(omegar);

			c1r = sin(phiar)*cos(kappar) + cos(phiar)*sin(omegar)*sin(kappar);
			c2r = -sin(omegar)*sin(kappar) + cos(phiar)*sin(omegar)*sin(kappar);
			c3r = cos(phiar)*cos(omegar);

			R_XYZ[0] = a1r*pnt2[i].x + a2r*pnt2[i].y - a3r*m_fLen;
			R_XYZ[1] = b1r*pnt2[i].x + b2r*pnt2[i].y - b3r*m_fLen;
			R_XYZ[2] = c1r*pnt2[i].x + c2r*pnt2[i].y - c3r*m_fLen;

			L_XYZ[0] = a1l*pnt1[i].x + a2l*pnt1[i].y - a3l*m_fLen;
			L_XYZ[1] = b1l*pnt1[i].x + b2l*pnt1[i].y - b3l*m_fLen;
			L_XYZ[2] = c1l*pnt1[i].x + c2l*pnt1[i].y - c3l*m_fLen;

			//计算By,Bz,N',N,Q
			B[1] = tan(var[3]) * B[0];					/*var[3]*B[0]*/;
			B[2] = tan(var[4]) * B[0] / cos(var[3]);		/*var[4]*B[0]*/;

			double RN = (B[0] * L_XYZ[2] - B[2] * L_XYZ[0]) / (L_XYZ[0] * R_XYZ[2] - L_XYZ[2] * R_XYZ[0]);   //N'
			double LN = (B[0] * R_XYZ[2] - B[2] * R_XYZ[0]) / (L_XYZ[0] * R_XYZ[2] - L_XYZ[2] * R_XYZ[0]);   //N
			L[i] = LN*L_XYZ[1] - RN*R_XYZ[1] - B[1];

			//计算系数
			A[5 * i + 0] = -(R_XYZ[0] * R_XYZ[1] / R_XYZ[2]) * RN;
			A[5 * i + 1] = -(R_XYZ[2] + (R_XYZ[1] * R_XYZ[1]) / R_XYZ[2]) * RN;
			A[5 * i + 2] = R_XYZ[0] * RN;
			A[5 * i + 3] = B[0];
			A[5 * i + 4] = -R_XYZ[1] * B[0] / R_XYZ[2];

			for (int j = 0; j<5; ++j)
			{
				CH[j] += A[5 * i + j] * L[i];
				for (int m = 0; m<5; ++m)
					FA[j * 5 + m] += A[5 * i + j] * A[5 * i + m];
			}
		}

		double IFA[5 * 5];
		double MCH[5];

		MatrixInverse(FA, 5, IFA);
		MatrixMuti(CH, 1, 5, 5, IFA, MCH);

		for (int ii = 0; ii < 5; ++ii)
			var_add[ii] = MCH[ii];
		MaxTime++;
		for (int ii = 0; ii<5; ++ii)
			var[ii] += var_add[ii];
		iteratornumber++;
		printf("\r迭代次数：%d", iteratornumber);
	}
	printf("\n");
	//求解结果输出
	reoRElementR.m_Bx	 = B[0];
	reoRElementR.m_By	 = B[1];
	reoRElementR.m_Bz	 = B[2];
	reoRElementR.m_phia  = var[0];
	reoRElementR.m_omega = var[1];
	reoRElementR.m_kappa = var[2];

	MatrixRotate(reoRElementR.m_dRMatrix, var[0], var[1], var[2]);

	delete[]A;
	delete[]L;

	return 0;
}