#pragma once
#ifndef _WaveLets_
#define _WaveLets_

//小波变换GDAL封装
#include "..\gdal/include/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

//////////////////////////////////////////////////////////////////////////
//小波变换的影像滤波
//高通滤波
void  HPFliter(char* pathSrc,char* pathDst,int band);
void  HPFliter(char* pathSrc,char* pathDst);

//低通滤波
void  LPFliter(char* pathSrc,char* pathDst,int band);
void  LPFliter(char* pathSrc,char* pathDst);

//////////////////////////////////////////////////////////////////////////

//图像一层小波变换
void  WaveletTransOnce(char* pathSrc,char* pathDst1,char* pathDst2,int band);
void  WaveletTransOnce(char* pathSrc,char* pathDst1,char* pathDst2);	//所有波段

//图像二层小波变换
void  WaveletTransTwice(char* pathSrc,char* pathDst1,char* pathDst2,int band);
void  WaveletTransTwice(char* pathSrc,char* pathDst1,char* pathDst2);

//图像三层小波变换
void  WaveletTransTri(char* pathSrc,char* pathDst1,char* pathDst2,int band);
void  WaveletTransTri(char* pathSrc,char* pathDst1,char* pathDst2);

//图像小波变换的逆变换
void  WaveletInvTrans(char* pathDst,char* pathSrc1,char* pathSrc2,int layer,int band);
void  WaveletInvTrans(char* pathDst,char* pathSrc1,char* pathSrc2,int layer);

//////////////////////////////////////////////////////////////////////////
//图像小波变换的逆变换
void  DWTi_Once(float ** spData, float ** spTransData0, float** spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);
void  DWTi_Once(float*   spData, float*   spTransData0, float*  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);


//三次小波变换
void  DWT_TriLayers(float ** spOriginData, float ** spTransData0, float ** spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);
void  DWT_TriLayers(float *  spOriginData, float *  spTransData0, float *  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);


//两次小波变换
void  DWT_TwoLayers(float ** spOriginData, float ** spTransData0, float ** spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);
void  DWT_TwoLayers(float *  spOriginData, float *  spTransData0, float *  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);


//一次小波变换
void  DWT_Once(float ** spOriginData, float ** spTransData0, float ** spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);
void  DWT_Once(float *  spOriginData, float *  spTransData0, float *  spTransData1, int nHeight, int nHeight_H, int nWidth, int nWidth_H, int layer, float fRadius);


//竖直方向的小波变换
void  Vert_Transform(float ** spOriginData, float ** spTransData1, int nHeight_H, int nWidth, float fRadius);
void  Vert_Transform(float *  spOriginData, float *  spTransData1, int nHeight_H, int nWidth, float fRadius);


//水平方向的小波变换
void  Hori_Transform(float ** spOriginData, float ** spTransData0, int nHeight, int nWidth_H, float fRadius);
void  Hori_Transform(float *  spOriginData, float *  spTransData0, int nHeight, int nWidth_H, float fRadius);

#endif

//  [3/25/2014 wuwei just you]
