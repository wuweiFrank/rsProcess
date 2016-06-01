#include"DCTTrans.h"
#include"..\Global.h"

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
				IDCTData[i] += sqrt(2.0f / float(length))*DCTData[j] * cos(PI*(2 * i + 1)*j / 2 / length);
		}
	}
}