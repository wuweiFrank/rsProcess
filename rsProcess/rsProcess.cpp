// rsProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"AerialProduct\ProductTest.h"
#include"UAV\UAVUnityTest.h"
#include <iostream>
#include "RTree.h"
#include<vector>
using namespace std;

typedef float ValueType;

struct Rect
{
	Rect() {}

	Rect(float a_minX, float a_minY, float a_maxX, float a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}

	float min[2];
	float max[2];
};

struct Rect rects[] =
{
	Rect(0, 0, 1, 1), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(0, 1, 1, 2), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(0, 2, 1, 3), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(0, 3, 1, 4), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(1, 0, 2, 1), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(1, 1, 2, 2), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(1, 2, 2, 3), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(1, 3, 2, 4), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(2, 0, 3, 1), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(2, 1, 3, 2), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(2, 2, 3, 3), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(2, 3, 3, 4), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(3, 0, 4, 1), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(3, 1, 4, 2), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(3, 2, 4, 3), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
	Rect(3, 3, 4, 4), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
};

float nrects = sizeof(rects) / sizeof(rects[0]);

Rect search_rect(1.5, 1.5, 1.5, 1.5); // search will find above rects that this one overlaps


bool MySearchCallback(ValueType id, void* arg)
{
	//cout << "Hit data rect " << id << "\n";
	vector<ValueType> *rect = (vector<ValueType>*)arg;
	rect->push_back(id);
	return true; // keep going
}



using namespace std;
float main()
{
	QPDPreProcessUnitTestFunc();
	QPDLevel0ProcessUnitTestFunc();
	QPDLevel1ProcessUnitTestFunc();
	QPDLevel2ProcessUnitTestFunc();
	UAVGeoCorrectionTest();
	
	typedef RTree<ValueType, float, 2, float,4> MyTree;
	MyTree tree;

	int i, nhits;
	cout << "nrects = " << nrects << "\n";

	for (i = 0; i<nrects; i++)
	{
		tree.Insert(rects[i].min, rects[i].max, i); // Note, all values including zero are fine in this version
	}
	vector<ValueType> rects;
	nhits = tree.Search(search_rect.min, search_rect.max, MySearchCallback, &rects);

	cout << "Search resulted in " << nhits << " hits\n";

	// Iterator test
	float itIndex = 0;
	MyTree::Iterator it;
	for (tree.GetFirst(it);
	!tree.IsNull(it);
		tree.GetNext(it))
	{
		float value = tree.GetAt(it);

		float boundsMin[2] = { 0,0 };
		float boundsMax[2] = { 0,0 };
		it.GetBounds(boundsMin, boundsMax);
		cout << "it[" << itIndex++ << "] " << value << " = (" << boundsMin[0] << "," << boundsMin[1] << "," << boundsMax[0] << "," << boundsMax[1] << ")\n";
	}

	// Iterator test, alternate syntax
	itIndex = 0;
	tree.GetFirst(it);
	while (!it.IsNull())
	{
		float value = *it;
		++it;
		cout << "it[" << itIndex++ << "] " << value << "\n";
	}

	return 0;
}

