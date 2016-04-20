#include"UAVPhotogrammetry.h"
#include<fstream>
#include"..\matrixOperation.h"
#include"..\Global.h"
#include<stdio.h>
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

	//首先设置相机径向和切向畸变
	double tk[3] = { -5.994e-005,2.927e-008,0 };
	double tp[3] = { -2.713e-006,3.156e-006,0 };
	double x0 = 0.4321, y0 = 0.1174, alpha= 8.447e-005, belta = 1.237e-004, flen = 40.9349;
	m_Tools.UAVPhotogrammetryTools_SetParam(flen, x0, y0, tk, tp, alpha, belta);
	
	vector<Point2f> pntl, pntr; vector<Point3d> pntGeo;
	//从文件中读取点的坐标
	ifstream  fin;
	fin.open("D:\\bundleadjustment_SCBA_Point_Result.txt");
	int n, u; float z;
	fin >> n;
	fin >> u;
	for (int i = 0; i<n; i++)
	{
		Point2f pntTmpl, pntTmpr;
		Point3d pntTmpGeo;
		fin >> u;
		fin >> pntTmpGeo.x;
		fin >> pntTmpGeo.y;
		fin >> pntTmpGeo.z;
		fin >> u >> u >> u;
		fin >> pntTmpl.x;
		fin >> pntTmpl.y;
		fin >> u;
		fin >> pntTmpr.x;
		fin >> pntTmpr.y;
		pntl.push_back(pntTmpl);
		pntr.push_back(pntTmpr);
		pntGeo.push_back(pntTmpGeo);
	}
	fin.close();
	m_Tools.UAVPhotogrammetryTools_DistortionCorrection(pntl);
	m_Tools.UAVPhotogrammetryTools_DistortionCorrection(pntr);
	
	REO lEOt, rEOt;
	memset(&lEOt, 0, sizeof(REO));
	m_Tools.UAVPhotogrammetryTools_ROrientation(pntl, pntr, lEOt, rEOt);
	printf("%lf  %lf  %lf  %lf  %lf  %lf\n", rEOt.m_phia, rEOt.m_omega, rEOt.m_kappa, rEOt.m_Bx, rEOt.m_By, rEOt.m_Bz);

	EO aEO; double lamda;
	aEO.m_dX = 1000; aEO.m_dY = 2000; aEO.m_dZ = 1500;
	aEO.m_phia = 0.011; aEO.m_omega = 0.022; aEO.m_kappa = 0.033;
	lamda = 4.0;
	m_Tools.UAVPhotogrammetryTools_AOrientation(pntl, pntr, lEOt, rEOt, pntGeo, aEO, lamda);
	printf("%lf  %lf  %lf  %lf  %lf  %lf  %lf\n", aEO.m_phia, aEO.m_omega, aEO.m_kappa, aEO.m_dX, aEO.m_dY, aEO.m_dZ, lamda);
	
	EO EOt;
	EOt.m_dX = 756.0875; EOt.m_dY = -131.6018; EOt.m_dZ = -15.0643;
	EOt.m_phia = 0.225967; EOt.m_omega = 0.112503; EOt.m_kappa = -0.081294;
	m_Tools.UAVPhotogrammetryTools_UAVResction(pntl, pntGeo, EOt);
	printf("%lf  %lf  %lf  %lf  %lf  %lf\n", EOt.m_phia, EOt.m_omega, EOt.m_kappa, EOt.m_dX, EOt.m_dY, EOt.m_dZ);

}
//径向和切向畸变校正
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_DistortionCorrection(vector<Point2f> &pntCamera)
{
	if (!isInternal)
		return -1;
#pragma omp parallel for
	for (int i = 0; i < pntCamera.size(); ++i)
	{
		double r= r = sqrt((pntCamera[i].x - m_px)*(pntCamera[i].x - m_px) + (pntCamera[i].y - m_py)*(pntCamera[i].y - m_py));
		double delta_x = (pntCamera[i].x - m_px)*(m_k[0]*r*r + m_k[1] *r*r*r*r) + m_p[0]*(r*r + 2 * (pntCamera[i].x - m_px)*(pntCamera[i].x - m_px)) + 2 * m_p[1] *(pntCamera[i].x - m_px)*(pntCamera[i].y - m_py) + m_alpha*(pntCamera[i].x - m_px) + m_belta*(pntCamera[i].y - m_py);
		double delta_y = (pntCamera[i].y - m_py)*(m_k[0] *r*r + m_k[1] *r*r*r*r) + m_p[1]*(r*r + 2 * (pntCamera[i].y - m_py)*(pntCamera[i].y - m_py)) + 2 * m_p[0] *(pntCamera[i].x - m_px)*(pntCamera[i].y - m_py);
		pntCamera[i].x = pntCamera[i].x - m_px - delta_x;
		pntCamera[i].y = pntCamera[i].y - m_py - delta_y;
	}
	return 0;
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

//设置转换参数 
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_SetParam(double len, double px0, double py0, double tk[3], double tp[3], double alpha, double belta)
{
	m_fLen = len;
	m_px = px0;
	m_py = py0;
	memcpy(m_k, tk, sizeof(double) * 3);
	memcpy(m_p, tp, sizeof(double) * 3);
	m_alpha = alpha;
	m_belta = belta;
	isInternal = true;
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

//相对定向
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_ROrientation(vector<Point2f> pnt1, vector<Point2f> pnt2, REO &reoRElementL, REO &reoRElementR)
{
	//必须先做内定向，将像素坐标转换到相片坐标
	//UAVPhotogrammetryTools_IOrientation(pnt1);
	//UAVPhotogrammetryTools_IOrientation(pnt2);

	//然后进行相对定向获取相对定向元素
	int num_point = pnt1.size();
	double var_add[5] = { 100,100,100,100,100 };				//五个变量的增量
																//临时变量
	double var[5];											//5个参数
	memset(var, 0, 5 * sizeof(double));
	double B[3];
	B[0] = 14.288;/*pnt1[1].x - pnt2[1].x;*///Bx本应当取2号点(6个点时)的视差，此时随机取2号点
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
		double phiar = var[0]; double omegar = var[1]; double kappar = var[2];
		//微小角运算
		//计算X2,Y2,Z2; X1, Y1, Z1
		a1r = cos(phiar)*cos(kappar) - sin(phiar)*sin(omegar)*sin(kappar);
		a2r = -cos(phiar)*sin(kappar) - sin(phiar)*sin(omegar)*cos(kappar);
		a3r = -sin(phiar)*cos(omegar);

		b1r = cos(omegar)*sin(kappar);
		b2r = cos(omegar)*cos(kappar);
		b3r = -sin(omegar);

		c1r = sin(phiar)*cos(kappar) + cos(phiar)*sin(omegar)*sin(kappar);
		c2r = -sin(phiar)*sin(kappar) + cos(phiar)*sin(omegar)*cos(kappar);
		c3r = cos(phiar)*cos(omegar);

		for (int i = 0; i< num_point; ++i)
		{
			num++;
			double R_XYZ[3], L_XYZ[3];

			R_XYZ[0] = a1r*pnt2[i].x + a2r*pnt2[i].y - a3r*m_fLen;
			R_XYZ[1] = b1r*pnt2[i].x + b2r*pnt2[i].y - b3r*m_fLen;
			R_XYZ[2] = c1r*pnt2[i].x + c2r*pnt2[i].y - c3r*m_fLen;

			L_XYZ[0] = a1l*pnt1[i].x + a2l*pnt1[i].y - a3l*m_fLen;
			L_XYZ[1] = b1l*pnt1[i].x + b2l*pnt1[i].y - b3l*m_fLen;
			L_XYZ[2] = c1l*pnt1[i].x + c2l*pnt1[i].y - c3l*m_fLen;

			//计算By,Bz,N',N,Q
			B[1] = /*tan(var[3]) * B[0];	*/					var[3]*B[0];
			B[2] = /*tan(var[4]) * B[0] / cos(var[3]);*/		var[4]*B[0];

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

//绝对定向
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_AOrientation(vector<Point2f> pntModel1, vector<Point2f> pntModel2, REO &REOl, REO &REOr, vector<Point3d> pntGeo, EO &eoAElement, double &lamda)
{
	//求出模型点在辅助坐标系上的坐标
	int pntsize = pntModel1.size();
	double *U = NULL, *V = NULL, *W = NULL;
	try
	{
		U = new double[pntsize];
		V = new double[pntsize];
		W = new double[pntsize];
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}
	double a1r, a2r, a3r, b1r, b2r, b3r, c1r, c2r, c3r;
	double a1l, a2l, a3l, b1l, b2l, b3l, c1l, c2l, c3l;

	double phial = REOl.m_phia;	double omegal = REOl.m_omega;double kappal = REOl.m_kappa;
	a1l = cos(phial)*cos(kappal) + sin(phial)*sin(omegal)*sin(kappal);
	a2l = -cos(phial)*sin(omegal) - sin(phial)*sin(omegal)*sin(kappal);
	a3l = -sin(phial)*cos(omegal);

	b1l = cos(omegal)*sin(kappal);
	b2l = cos(omegal)*cos(kappal);
	b3l = -sin(omegal);

	c1l = sin(phial)*cos(kappal) + cos(phial)*sin(omegal)*sin(kappal);
	c2l = -sin(omegal)*sin(kappal) + cos(phial)*sin(omegal)*sin(kappal);
	c3l = cos(phial)*cos(omegal);
	double phiar = REOr.m_phia; double omegar = REOr.m_omega; double kappar = REOr.m_kappa;
	a1r = cos(phiar)*cos(kappar) - sin(phiar)*sin(omegar)*sin(kappar);
	a2r = -cos(phiar)*sin(kappar) - sin(phiar)*sin(omegar)*cos(kappar);
	a3r = -sin(phiar)*cos(omegar);

	b1r = cos(omegar)*sin(kappar);
	b2r = cos(omegar)*cos(kappar);
	b3r = -sin(omegar);

	c1r = sin(phiar)*cos(kappar) + cos(phiar)*sin(omegar)*sin(kappar);
	c2r = -sin(phiar)*sin(kappar) + cos(phiar)*sin(omegar)*cos(kappar);
	c3r = cos(phiar)*cos(omegar);

	//求模型点坐标
	for (int i = 0; i < pntsize; ++i)
	{
		double R_XYZ[3], L_XYZ[3];

		R_XYZ[0] = a1r*pntModel2[i].x + a2r*pntModel2[i].y - a3r*m_fLen;
		R_XYZ[1] = b1r*pntModel2[i].x + b2r*pntModel2[i].y - b3r*m_fLen;
		R_XYZ[2] = c1r*pntModel2[i].x + c2r*pntModel2[i].y - c3r*m_fLen;

		L_XYZ[0] = a1l*pntModel1[i].x + a2l*pntModel1[i].y - a3l*m_fLen;
		L_XYZ[1] = b1l*pntModel1[i].x + b2l*pntModel1[i].y - b3l*m_fLen;
		L_XYZ[2] = c1l*pntModel1[i].x + c2l*pntModel1[i].y - c3l*m_fLen;

		double RN = (REOr.m_Bx * L_XYZ[2] - REOr.m_Bz * L_XYZ[0]) / (L_XYZ[0] * R_XYZ[2] - L_XYZ[2] * R_XYZ[0]);   //N'
		double LN = (REOr.m_Bx * R_XYZ[2] - REOr.m_Bz * R_XYZ[0]) / (L_XYZ[0] * R_XYZ[2] - L_XYZ[2] * R_XYZ[0]);   //N
	
		U[i] = REOr.m_Bx + RN * R_XYZ[0];
		V[i] = REOr.m_By + RN * R_XYZ[1];
		W[i] = REOr.m_Bz + RN * R_XYZ[2];
	}

	//赋初值重心化坐标
	double Xg = 0.0,Yg = 0.0,Zg = 0.0,Ug = 0.0,Vg = 0.0,Wg = 0.0;
	for (int i = 0; i<pntsize; i++)
	{
		Xg += pntGeo[i].x ;
		Yg += pntGeo[i].y ;
		Zg += pntGeo[i].z ;
		Ug += U[i];
		Vg += V[i];
		Wg += W[i];
	}
	Xg /= pntsize;
	Yg /= pntsize;
	Zg /= pntsize;
	Ug /= pntsize;
	Vg /= pntsize;
	Wg /= pntsize;

	//比例尺
	double S1 = sqrt(Xg*Xg + Yg*Yg + Zg*Zg);
	double S2 = sqrt(Ug*Ug + Vg*Vg + Wg*Wg);
	double K = S1 / S2;

	//去重心化坐标
	double *Xba = NULL, *Yba = NULL, *Zba = NULL, *Uba  = NULL, *Vba = NULL, *Wba = NULL;
	try
	{
		Xba= new double[pntsize];
		Yba = new double[pntsize];
		Zba = new double[pntsize];
		Uba = new double[pntsize];
		Vba = new double[pntsize];
		Wba = new double[pntsize];
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}
	for (int i = 0; i<pntsize; i++)
	{
		Xba[i] = pntGeo[i].x - Xg;
		Yba[i] = pntGeo[i].y - Yg;
		Zba[i] = pntGeo[i].z- Zg;
		Uba[i] = (U[i] - Ug)*K;
		Vba[i] = (V[i] - Vg)*K;
		Wba[i] = (W[i] - Wg)*K;
	}

	//迭代求解
	double *AA = NULL, *LL = NULL, *AAT = NULL, *AAIM = NULL;
	double AAM[49], AAI[49], LLM[7],LLIM[7];
	try
	{
		LL = new double[pntsize * 3];
		AA = new double[pntsize*21]; 
		AAT = new double[pntsize * 21];
		AAIM = new double[pntsize * 21];
	}
	catch (bad_alloc &e)
	{
		printf("%s\n", e.what());
		exit(-1);
	}
	int iteratorNum = 0;
	do
	{
		//旋转矩阵
		double R0[9];
		double Phi0 = eoAElement.m_phia, Omega0 = eoAElement.m_omega, Kappa0 = eoAElement.m_kappa;
		R0[0] = cos(Phi0)*cos(Kappa0) - sin(Phi0)*sin(Omega0)*sin(Kappa0);
		R0[1] = -cos(Phi0)*sin(Kappa0) - sin(Phi0)*sin(Omega0)*cos(Kappa0);
		R0[2] = -sin(Phi0)*cos(Omega0);
		R0[3] = cos(Omega0)*sin(Kappa0);
		R0[4] = cos(Omega0)*cos(Kappa0);
		R0[5] = -sin(Omega0);
		R0[6] = sin(Phi0)*cos(Kappa0) + cos(Phi0)*sin(Omega0)*sin(Kappa0);
		R0[7] = -sin(Phi0)*sin(Kappa0) + cos(Phi0)*sin(Omega0)*cos(Kappa0);
		R0[8] = cos(Phi0)*cos(Omega0);

		for (int i = 0; i<pntsize; i++)
		{
			AA[21 * i+0] = 1;
			AA[21 * i+1] = 0;
			AA[21 * i+2] = 0;
			AA[21 * i+3] = Uba[i];
			AA[21 * i+4] = -Wba[i];
			AA[21 * i+5] = 0;
			AA[21 * i+6] = -Vba[i];

			AA[21 * i + 7] = 0;
			AA[21 * i + 8] = 1;
			AA[21 * i + 9] = 0;
			AA[21 * i + 10]= Vba[i];
			AA[21 * i + 11] = 0;
			AA[21 * i + 12] = -Wba[i];
			AA[21 * i + 13] = Uba[i];

			AA[21 * i + 14] = 0;
			AA[21 * i + 15] = 0;
			AA[21 * i + 16] = 1;
			AA[21 * i + 17] = Wba[i];
			AA[21 * i + 18] = Uba[i];
			AA[21 * i + 19] = Vba[i];
			AA[21 * i + 20] = 0;
		}

		//求出改正数的常数项
		for (int i = 0; i<pntsize; i++)
		{
			LL[3 * i + 0] = Xba[i] - lamda*(R0[0] * Uba[i] + R0[1] * Vba[i] + R0[2] * Wba[i]) - eoAElement.m_dX;
			LL[3 * i + 1] = Yba[i] - lamda*(R0[3] * Uba[i] + R0[4] * Vba[i] + R0[5] * Wba[i]) - eoAElement.m_dY;
			LL[3 * i + 2] = Zba[i] - lamda*(R0[6] * Uba[i] + R0[7] * Vba[i] + R0[8] * Wba[i]) - eoAElement.m_dZ;
		}
		
		MatrixTrans(AA, pntsize * 3, 7, AAT);
		MatrixMuti(AAT, 7, pntsize * 3, 7, AA, AAM);
		MatrixInverse(AAM, 7, AAI);
		MatrixMuti(AAI, 7, 7, pntsize * 3, AAT, AAIM);
		MatrixMuti(AAIM, 7, pntsize * 3, 1, LL, LLIM);

		eoAElement.m_dX += LLIM[0];
		eoAElement.m_dY += LLIM[1];
		eoAElement.m_dZ += LLIM[2];
		lamda+= LLIM[3];
		eoAElement.m_phia += LLIM[4];
		eoAElement.m_omega += LLIM[5];
		eoAElement.m_kappa += LLIM[6];
		iteratorNum++;
		printf("迭代次数：%d\r", iteratorNum);
	} while (OVER_LIMIT(LLIM[0])|| OVER_LIMIT(LLIM[1]) || OVER_LIMIT(LLIM[2]) ||
		OVER_LIMIT(LLIM[3]) || OVER_LIMIT(LLIM[4]) || OVER_LIMIT(LLIM[5]) || OVER_LIMIT(LLIM[6]));

	//清空内存
	if (U != NULL)
		delete[]U;
	if (V != NULL)
		delete[]V;
	if (W != NULL)
		delete[]W;
	if (Xba != NULL)
		delete[]Xba;
	if (Yba != NULL)
		delete[]Yba;
	if (Zba != NULL)
		delete[]Zba;
	if (Uba != NULL)
		delete[]Uba;
	if (Vba != NULL)
		delete[]Vba;
	if (Wba != NULL)
		delete[]Wba;
	if (AA != NULL)
		delete[]AA;
	if (LL != NULL)
		delete[]LL;
	if (AAT != NULL)
		delete[]AAT;
	if (AAIM != NULL)
		delete[]AAIM;
	U =V =W = NULL;
	Xba = Yba = Zba = Uba = Vba = Wba = NULL;
	AAIM=AA = LL = AAT = NULL;

	return 0;
}

//空间前方交会
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_UAVFesction(vector<Point2f> pnt1, EO imgEO1, vector<Point2f> pnt2, EO imgEO2, vector<Point3d> &points)
{
	double tmp1[3], tmp2[3];
	double pt1[3], pt2[3];
	for (size_t i = 0; i < pnt1.size(); i++)
	{
		pt1[0] = pnt1[i].x; pt1[1] = pnt1[i].y; pt1[2] = -m_fLen;
		pt2[0] = pnt2[i].x; pt2[1] = pnt2[i].y; pt2[2] = -m_fLen;
		MatrixMuti(imgEO1.m_dRMatrix, 3, 3, 1, pt1, tmp1);
		MatrixMuti(imgEO2.m_dRMatrix, 3, 3, 1, pt2, tmp2);
		double rato1[2], rato2[2];
		rato1[0] = tmp1[0] / tmp1[2]; rato1[1] = tmp1[1] / tmp1[2];
		rato2[0] = tmp2[0] / tmp2[2]; rato2[1] = tmp2[1] / tmp2[2];

		double A[12], L[4], AT[12], ATA[9], IATA[9], ML[3];
		A[0] = 1; A[1] = 0;  A[2] = -rato1[0]; L[0] = imgEO1.m_dX - rato1[0] * imgEO1.m_dZ;
		A[3] = 0; A[4] = 1;  A[5] = -rato1[1]; L[1] = imgEO1.m_dY - rato1[1] * imgEO1.m_dZ;
		A[6] = 1; A[7] = 0;  A[8] = -rato2[0]; L[2] = imgEO2.m_dX - rato2[0] * imgEO2.m_dZ;
		A[9] = 0; A[10] = 1; A[11] = -rato2[0]; L[3] = imgEO2.m_dY - rato2[1] * imgEO2.m_dZ;

		MatrixTrans(A, 4, 3, AT);
		MatrixMuti(AT, 3, 4, 1, L, ML);
		MatrixMuti(AT, 3, 4, 3, A, ATA);
		MatrixInverse(ATA, 3, IATA);
		double pt3d[3];
		MatrixMuti(IATA, 3, 3, 1, ML, pt3d);
		Point3d pnt;
		pnt.x = pt3d[0];
		pnt.y = pt3d[1];
		pnt.z = pt3d[2];
		points.push_back(pnt);
	}

	return 0;
}

//空间后方交会
long UAVPhotogrammetryTools::UAVPhotogrammetryTools_UAVResction(vector<Point2f> pnt1, vector<Point3d> pnt2, EO &eoElement)
{
	//UAVPhotogrammetryTools_IOrientation(pnt1);
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

	double FA[36];
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
	int total = 0;
	MatrixRotate(eoElement.m_dRMatrix, eoElement.m_phia, eoElement.m_omega, eoElement.m_kappa);
	while ((OVER_LIMIT(var_add[0]) || OVER_LIMIT(var_add[1]) || OVER_LIMIT(var_add[2]) ||
		OVER_LIMIT(var_add[3]) || OVER_LIMIT(var_add[4])) /*&& MaxTime < 20*/)
	{
		memset(FA, 0, sizeof(double) * 36);
		memset(CH, 0, sizeof(double) * 5);
		int num = 0;
		double Phi = eoElement.m_phia, Omega = eoElement.m_omega, Kappa = eoElement.m_kappa;
		double a1 = cos(Phi)*cos(Kappa) - sin(Phi)*sin(Omega)*sin(Kappa);
		double a2 = -cos(Phi)*sin(Kappa) - sin(Phi)*sin(Omega)*cos(Kappa);
		double a3 = -sin(Phi)*cos(Omega);
		double b1 = cos(Omega)*sin(Kappa);
		double b2 = cos(Omega)*cos(Kappa);
		double b3 = -sin(Omega);
		double c1 = sin(Phi)*cos(Kappa) + cos(Phi)*sin(Omega)*sin(Kappa);
		double c2 = -sin(Phi)*sin(Kappa) + cos(Phi)*sin(Omega)*cos(Kappa);
		double c3 = cos(Phi)*cos(Omega);
		for (int i = 0; i < point_num; ++i)
		{
			A[12 * i + 0] = (a1*m_fLen + a3*pnt1[i].x) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			A[12 * i + 1] = (b1*m_fLen + b3*pnt1[i].x) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			A[12 * i + 2] = (c1*m_fLen + c3*pnt1[i].x) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			
			A[12 * i + 6] = (a2*m_fLen + a3*pnt1[i].y) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			A[12 * i + 7] = (b2*m_fLen + b3*pnt1[i].y) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			A[12 * i + 8] = (c2*m_fLen + c3*pnt1[i].y) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			
			A[12 * i + 3] = pnt1[i].y * sin(Omega) - (pnt1[i].x * (pnt1[i].x * cos(Kappa) - pnt1[i].y * sin(Kappa)) / m_fLen + m_fLen*cos(Kappa))*cos(Omega);
			A[12 * i + 4] = -m_fLen*sin(Kappa) - pnt1[i].x * (pnt1[i].x * sin(Kappa) + pnt1[i].y * cos(Kappa)) / m_fLen;
			A[12 * i + 5] = pnt1[i].y;
			
			A[12 * i + 9] = pnt1[i].x * sin(Omega) - (pnt1[i].y * (pnt1[i].x * cos(Kappa) - pnt1[i].y * sin(Kappa)) / m_fLen - m_fLen*sin(Kappa))*cos(Omega);
			A[12 * i + 10] = -m_fLen*cos(Kappa) - pnt1[i].y * (pnt1[i].x * sin(Kappa) + pnt1[i].y * cos(Kappa)) / m_fLen;
			A[12 * i + 11] = -pnt1[i].x;
			//计算结果
			//double a1, a2, a3, b1, b2, b3, c1, c2, c3;
			//a1 = eoElement.m_dRMatrix[0]; a2 = eoElement.m_dRMatrix[1]; a3 = eoElement.m_dRMatrix[2];
			//b1 = eoElement.m_dRMatrix[3]; b2 = eoElement.m_dRMatrix[4]; b3 = eoElement.m_dRMatrix[5];
			//c1 = eoElement.m_dRMatrix[6]; c2 = eoElement.m_dRMatrix[7]; c3 = eoElement.m_dRMatrix[8];
			double x = -m_fLen*(a1*(pnt2[i].x - eoElement.m_dX) + b1*(pnt2[i].y - eoElement.m_dY) + c1*(pnt2[i].z - eoElement.m_dZ)) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			double y = -m_fLen*(a2*(pnt2[i].x - eoElement.m_dX) + b2*(pnt2[i].y - eoElement.m_dY) + c2*(pnt2[i].z - eoElement.m_dZ)) / (a3*(pnt2[i].x - eoElement.m_dX) + b3*(pnt2[i].y - eoElement.m_dY) + c3*(pnt2[i].z - eoElement.m_dZ));
			L[2 * i + 0] = pnt1[i].x - x;
			L[2 * i + 1] = pnt1[i].y - y;
		}
		double IFA[36];
		double MCH[6];
		MatrixTrans(A, point_num * 2, 6, AT);
		MatrixMuti(AT, 6, point_num * 2, 6, A, FA);
		MatrixMuti(AT, 6, point_num * 2, 1, L, CH);
		MatrixInverse(FA, 6, IFA);
		MatrixMuti(IFA, 6, 6, 1, CH, MCH);

		memcpy(var_add, MCH, sizeof(double) * 6);

		eoElement.m_dX += MCH[0]; eoElement.m_dY += MCH[1]; eoElement.m_dZ += MCH[2];
		eoElement.m_phia += MCH[3]; eoElement.m_omega += MCH[4]; eoElement.m_kappa += MCH[5];
		MatrixRotate(eoElement.m_dRMatrix, eoElement.m_phia, eoElement.m_omega, eoElement.m_kappa);
		printf("迭代次数 ：%d\r", total);
		total++;
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
