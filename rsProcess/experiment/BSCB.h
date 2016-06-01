#pragma once
//BSCB影像修复方法
/*BSCB影像修复方法的整个过程为：
1.信息传递
2.信息扩散
3.终止条件
*/
class BSCB
{
public:
	void BSCBImageInpaintingProcess(const char* pathImgIn, const char* pathImgMask, const char* pathImgRepair);
private:
	void InterpolationInit(float* data, float *maskData, int xsize, int ysize);

	void DiffuseLoop(float* data, float* img, int x, int y, int xsize, int ysize);

	void RepaireLoop(float* data, float* img, double delta, int x, int y, int xsize, int ysize);

	double RepairChange(float* img, int x, int y, int xsize, int ysize);

	double Laplace(float* img, int x, int y, int xsize, int ysize);

	bool TerminateBSCBCondition(float* preData, float* afterData, float *maskData, int xsize, int ysize);
};
