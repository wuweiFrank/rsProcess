#include"SegmentBased.h"
#include"..\matrixOperation.h"

void QuadraticCurveFitting(vector<CPOINT> pnts, float* params)
{
	double t1[9],t1Inv[9];
	double t2[3],t3[3];

	for (int i = 0; i < 3; ++i)
	{
		t1[3 * i + 0] = pnts[i].x*pnts[i].x;
		t1[3 * i + 0] = pnts[i].x;
		t1[3 * i + 0] = 1;
		t2[i] = pnts[i].y;
	}

	MatrixInverse(t1, 3, t1Inv);
	MatrixMuti(t1Inv, 3, 3, 1, t2, t3);

	for (int i = 0; i < 3; ++i)
		params[i] = t3[i];
}

