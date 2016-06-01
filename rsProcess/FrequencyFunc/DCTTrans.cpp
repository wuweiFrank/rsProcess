#include"DCTTrans.h"
#include"..\Global.h"
#include<math.h>
//1Œ¨¿Î…¢DCT±‰ªª
void DCT1D(float* dataIn, int length, float* DCTData)
{
	for (int i = 0; i < length; ++i)
	{
		DCTData[i] = 0;
		for (int j = 0; j < length; ++j)
		{
			DCTData[i] += dataIn[j] * cos(PI*(2 * j + 1)*i / 2 / length);
		}
		if (i == 0)
			DCTData[i] = sqrt(1.0f / float(length))*DCTData[i];
		else
			DCTData[i] = sqrt(2.0f / float(length))*DCTData[i];
	}
}
void IDCT1D(float* DCTData, int length, float* IDCTData)
{
	for (int i = 0; i < length; ++i)
	{
		IDCTData[i] = 0;
		for (int j = 0; j < length; ++j)
		{
			if (i == 0)
				IDCTData[i] += sqrt(1.0f / float(length))*DCTData[j] * cos(PI*(2 * i + 1)*j / 2 / length);
			if (i != 0)
				IDCTData[i] += sqrt(1.0f / float(length))*DCTData[j] * cos(PI*(2 * i + 1)*j / 2 / length);
		}
	}
}

//2Œ¨¿Î…¢DCT±‰ªª
void DCT2D(float* dataIn, int xsize, int ysize, float* DCTData)
{
	if (xsize != ysize)
	{
		printf("input matrix is not a square£°\n");
	}
	memset(DCTData, 0, sizeof(float)*xsize*ysize);
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{

			//ƒ⁄≤ø—≠ª∑
			float tmp1, tmp2;
			if (i == 0)
				tmp1 = sqrt(1 / xsize);
			else
				tmp1 = sqrt(2 / xsize);
			if (j == 0)
				tmp2 = sqrt(1 / xsize);
			else
				tmp2 = sqrt(2 / xsize);

			double tmp = 0;
			for (int m = 0; m < xsize; ++m)
			{
				for (int n = 0; n < ysize; ++n)
				{
					tmp += dataIn[n*xsize + m] * cos(float((m + 0.5*PI)*i / xsize))*cos(float((n + 0.5*PI)*j / xsize));
				}
			}
			DCTData[j*xsize + i] = tmp*tmp1*tmp2;
		}
	}
}
void IDCT2D(float* DCTData, int xsize, int ysize, float* IDCTData)
{
	if (xsize != ysize)
	{
		printf("input matrix is not a square£°\n");
	}
	memset(IDCTData, 0, sizeof(float)*xsize*ysize);
	for (int i = 0; i < xsize; ++i)
	{
		for (int j = 0; j < ysize; ++j)
		{
			float tmp1, tmp2;
			tmp1 = sqrt(1 / xsize);
			tmp2 = sqrt(1 / ysize);

			float tmp = 0;
			for (int m = 0; m < xsize; ++m)
			{
				for (int n = 0; n < ysize; ++n)
				{
					tmp += tmp1*tmp2*DCTData[n*xsize + m] * cos(float((i*0.5)*PI*m / xsize))*cos(float((j*0.5)*PI*n / xsize));
				}
			}
			IDCTData[j*xsize + i] = tmp;
		}
	}
}