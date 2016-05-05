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
	void BSBCImageInpaintingProcess(const char* pathImgIn, const char* pathImgMask, const char* pathImgRepair);
private:
	void DiffuseLoop(int* data, int* img, int x, int y, int xsize, int ysize);

	void RepaireLoop(int* data, int* img, double delta, int x, int y, int xsize, int ysize);

	double RepairChange(int* img, int x, int y, int xsize, int ysize);

	double Laplace(int* img, int x, int y, int xsize, int ysize);

	bool TerminateBSCBCondition(int* preData, int* afterData, int *maskData, int xsize, int ysize);
};
