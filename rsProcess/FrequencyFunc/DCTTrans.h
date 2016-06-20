#pragma once

//朴素DCT变换效率极低，最好能够通过蝶形变换加速！！

//1维DCT变换
void DCT1D(float* dataIn, int length, float* DCTData);
void IDCT1D(float* DCTData, int length, float* IDCTData);

//2维DCT变换
void DCT2D(float* dataIn, int xsize,int ysize, float* DCTData);
void IDCT2D(float* DCTData, int xsize, int ysize, float* IDCTData);

//快速变换
void FDCT2D(float* dataIn, int xsize, int ysize, float* DCTData);
void IFDCT2D(float* DCTData, int xsize, int ysize, float* IDCTData);

//3维DCT变换 
void DCT3D(float* dataIn, int xsize, int ysize, int bands, float* DCTData);
void IDCT3D(float* DCTData, int xsize, int ysize, int bands, float* IDCTData);

//图像DCT变换
void DCT2D(const char* pathImgIn, const char* pathOut, int bandIdx);
void IDCT2D(const char* pathImgIn, const char* pathOut, int bandIdx);
void DCT3D(const char* pathImgIn, const char* pathOut);
void IDCT3D(const char* pathImgIn, const char* pathOut);

//图像滤波
void DCTFliter2D(const char* pathImgIn, const char* pathOut, int bandIdx, float thresthod);
void DCTFliter3D(const char* pathImgIn, const char* pathOut, float thresthod);