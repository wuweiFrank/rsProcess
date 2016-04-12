#pragma once

//矩形结构体
struct Rect
{
	Rect() {}

	Rect(double a_minX, double a_minY, double a_maxX, double a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}
	void setRect(double a_minX, double a_minY, double a_maxX, double a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}
	bool isInRect(double x, double y)
	{
		if (x > min[0] && y > min[0] && x < min[1] && y < min[1])
			return true;
		return false;
	}


	double min[2];
	double max[2];
};

//三维点结构体
struct Vec3d
{
	double x;
	double y;
	double z;
};

struct Las
{

};