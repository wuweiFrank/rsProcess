#pragma once

//1άDCT�任
void DCT1D(float* dataIn, int length, float* DCTData);
void IDCT1D(float* DCTData, int length, float* IDCTData);

//2άDCT�任
void DCT2D(float* dataIn, int xsize,int ysize, float* DCTData);
void IDCT2D(float* DCTData, int xsize, int ysize, float* IDCTData);