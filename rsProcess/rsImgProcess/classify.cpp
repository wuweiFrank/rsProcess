#include"classify.h"
#include"..\stdafx.h"
#include<math.h>
category createcategory(int demension)
{
	category m_categ;
	m_categ.center = new float[demension];
	m_categ.objdervation = new float[demension];
	m_categ.demension = demension;
	m_categ.objindex = NULL;
	m_categ.objnumbers = 0;
	return m_categ;
}

void setcategoryobjs(int objnumbers, category &m_categ)
{
	m_categ.objnumbers = objnumbers;
	m_categ.objindex = new int[objnumbers];
}

void getstatisticcategory(float* data, category &m_categ, int demensionIdx)
{
	double tmp = 0;
	for (int i = 0; i < m_categ.objnumbers; ++i)
	{
		tmp += data[m_categ.objindex[i]];
	}
	tmp /= m_categ.objnumbers;
	m_categ.center[demensionIdx] = tmp;

	double dev = 0;
	for (int i = 0; i < m_categ.objnumbers; ++i)
	{
		dev += (data[m_categ.objindex[i]] - tmp)*(data[m_categ.objindex[i]] - tmp);
	}
	dev = sqrt(dev / m_categ.objnumbers);
	m_categ.objdervation[demensionIdx] = dev;
}

void splitcategory(category m_categ, category &m_categ1, category &m_categ2)
{
	int demension = m_categ.demension;
	m_categ1 = createcategory(demension);
	m_categ2 = createcategory(demension);

	for (int i = 0; i < demension; ++i)
	{
		m_categ1.center[i] = m_categ.center[i] - m_categ.objdervation[i];
		m_categ2.center[i] = m_categ.center[i] + m_categ.objdervation[i];
	}
}

void combinecategory(category m_categ1, category m_categ2, category &m_categ)
{
	if (m_categ1.demension != m_categ2.demension)
	{
		printf("两个类维度不同\n");
		return;
	}
	int demension = m_categ1.demension;
	m_categ = createcategory(demension);
	for (int i = 0; i < demension; ++i)
	{
		m_categ.center[i] = (m_categ1.center[i] + m_categ2.center[i]) / 2;
	}
}

void freecategory(category &m_categ)
{
	if (m_categ.center != NULL)
		delete[]m_categ.center;
	if (m_categ.objdervation != NULL)
		delete[]m_categ.objdervation;
	if (m_categ.objindex != NULL)
		delete[]m_categ.objindex;

	m_categ.center = NULL;
	m_categ.objdervation = NULL;
	m_categ.objindex = NULL;
}