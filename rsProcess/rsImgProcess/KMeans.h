#pragma once
#include"classify.h"

//初始化聚类
void InitKMeans(float* dataImg, int xsize, int ysize, int bands, int classNum, category* m_categories);

//根据分类结果和数据计算类中心
void CenterKMeans(float* dataImg, int xsize, int ysize, int bands, int classNum, category* m_categories);

//进行k均值的迭代
void IteratorKMeans(float* dataImg, int xsize, int ysize, int bands, int classNum, category* m_categories);

//K均值分类
void KMeansClassify(const char* pathImgIn, const char* pathImgOut, int classNum);

//ISOData分类
void ISODataClassify(const char* pathImgIn, const char* pathImgOut, int classNum,float maxDev,float minDis);