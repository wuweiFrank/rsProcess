#pragma once

//每一个类别信息
struct category {
	float* center;
	int*   objindex;
	int    demension;
	int    objnumbers;
	float*  objdervation;
};

//创建一个类别
category createcategory(int demension);

//设置类别内对象数目
void setcategoryobjs(int objnumbers, category &m_categ);

//计算内对象的均值和方差
//这里假设把所有数据都读入，不考虑数据量的大小
void getstatisticcategory(float* data, category &m_categ,int demensionIdx);

//类分类操作
void splitcategory(category m_categ, category &m_categ1, category &m_categ2);

//类合并操作
void combinecategory(category m_categ1, category m_categ2, category &m_categ);

//释放类空间
void freecategory(category &m_categ);
