#pragma once

#include"..\gdal\include\gdal_priv.h"
#include"..\Global.h"

#pragma comment(lib,"gdal_i.lib")

#include<vector>
using namespace std;

//1级数据产品生产
//1级数据产品的基本功能为辐射校正，主要包括相对辐射校正，绝对辐射校正和数据标定
class Level1Process
{
public:
	//根据绝对辐射定标系数进行绝对辐射定标
	virtual long Level1Proc_RadiationAbsolute(const char* pathImg, const char* pathImgRad, const char* pathAbsRegFile) = 0;
	//获取绝对辐射定标系数文件
	//第一个参数为绝对辐射校正系数文件，第二个参数为乘性系数，第三个参数为加性系数，第四个参数为可选参数，可以不用默认为空
	virtual long Level1Proc_AbsoluteParameters(const char* pathAbsRegFile, float* paramA, float* paramB, float* paramAux = NULL)=0;

	//相对辐射校正
	virtual long Level1Proc_RadiationRelative(const char* pathImg, const char* pathImgRad, const char* pathRelRegFile) = 0;
	//获取相对辐射校正参数
	////第一个参数为相对对辐射校正系数文件，第二个参数为乘性系数，第三个参数为加性系数，第四个参数为可选参数，可以不用默认为空
	virtual long LevelProc_RelativeParameters(const char* pathRelRegFile, float* paramA, float* paramB, float* paramAux = NULL) = 0;

	//大气校正，通过模拟太阳光辐亮度值，对比影像辐亮度值得到反射率
	virtual long Level1Proc_AtmosphereCorrect(const char* pathImgRad, const char* pathRef, const char* pathAtmosphere) = 0;

};


//全谱段1级数据产品生产
class QPDLevel1Process : public Level1Process
{
public:
	//===============================================================================================================================================
	//辐射校正
	//绝对辐射校正
	long Level1Proc_RadiationAbsolute(const char* pathImg, const char* pathImgRad, const char* pathAbsRegFile);
	//获取绝对辐射校正系数
	long Level1Proc_AbsoluteParameters(const char* pathAbsRegFile, float* paramA, float* paramB, float* paramAux = NULL);

	//相对辐射校正
	long Level1Proc_RadiationRelative(const char* pathImg, const char* pathImgRad, const char* pathRelRegFile);
	//获取相对辐射校正系数
	long LevelProc_RelativeParameters(const char* pathRelRegFile, float* paramA, float* paramB, float* paramAux = NULL);

	//对于QPD数据采用分段线性校正 最后一个系数为分段数
	long LevelProc_GetParameterInfo(const char* pathImgRad, int& nSamples, int &nBands, int &nLevels);

	//重载的大气校正函数，由于没有参数，只好空着了
	long Level1Proc_AtmosphereCorrect(const char* pathImgRad, const char* pathRef, const char* pathAtmosphere) { return 0; }
	//=================================================================================================================================================
	//下面是QPD数据处理特有的函数
	//三个视场的视场拼接
	/*****************************************************************************
	* @brief : 进行影像视场拼接
	* @author : W.W.Frank
	* @date : 2015/11/12 23:34
	* @version : version 1.0
	* @inparam : const char* pathview1,const char* pathview2,const char* pathview3,三个待拼接视场影像路径
	float over_left_center_x,float over_right_center_x,左中视场的x重叠 中右视场的x重叠
	float err_left_center_y,float err_right_center_y,	左中视场y错位，中右视场y错位，高于中视场为正，低于中视场为负
	const char* pathJointView							输出结果影像
	* @outparam : 拼接后数据
			*思考：对于亚像元精度的需求，我认为单纯的从常规的影像角度是没有办法做的，但是一定要做还是有办法的，
			不过会破坏影像光谱的完整性，首先对影像按照要求的精度进行升采样，采样后换算成凭借像元的精度，
			然后对影像进行降采样，这样的话理论上应该可以达到亚像元的精度，但是采样操作是不可逆的，对影
			像进行首先升采样，然后降采样的操作势必会破坏影像光谱特征（并不推荐这么做，如果只看几何精度应该这么做！）
	*****************************************************************************/
	long Level1Proc_ViewJointThree(const char* pathview1, const char* pathview2, const char* pathview3,
		float over_left_center_x, float over_right_center_x, float err_left_center_y, float err_right_center_y, const char* pathJointView);
	//两个视场的拼接(两个视场的拼接比三个视场的拼接要简单的多)
	long Level1Proc_ViewJointTwo(const char* pathview1, const char* pathview2, float over_x, float err_y, const char* pathJointView);

private:
	//视场拼接的内部接口==================================================================================================================================
	//视场拼接色调调整
	//三个视场将色调调整到中间的影像上
	long Level1Proc_ColorMatch(unsigned short* imgBuffer1, unsigned short* imgBuffer2, unsigned short* imgBuffer3, int xsize1, int ysize1, int xsize2, int ysize2, int xsize3, int ysize3,
		int over_left_center_x, int over_right_center_x, int err_left_center_y, int err_right_center_y);
	//两个视场利用重叠区域匀色
	long Level1Proc_ColorMatch(unsigned short* imgBuffer1, unsigned short* imgBuffer2, int xsize1, int ysize1, int xsize2, int ysize2, int over_x, int err_y);

	/*
	功能：获取视场拼接后影像大小
	参数：int xsizeLeft,int ysizeLeft,			左视场影像大小
	*/
	long Level1Proc_ViewJointRange(int xsize1, int ysize1, int xsize2, int ysize2, int xsize3, int ysize3, float over_left_centerx,
		float err_left_centery, float over_right_centerx, float err_right_centery, int &xmosaic, int &ymosaic);
	long Level1Proc_ViewJointRange(int xsize1, int ysize1, int xsize2, int ysize2, float over_x, float err_y, int &xmosaic, int &ymosaic);
	/*
	功能：获取视场拼接后影像拼接边位置
	参数：int xsizeLeft,int ysizeLeft,			左视场影像大小
	*/
	long Level1Proc_ViewJointEdge(int xsize1, int ysize1, int xsize2, int ysize2, int xsize3, int ysize3, float over_left_centerx, float err_left_centery,
		float over_right_centerx, float err_right_centery, vector<CPOINT>& edge_position);
	long Level1Proc_ViewJointEdge(int xsize1, int ysize1, int xsize2, int ysize2, float over_x, float err_y, vector<CPOINT>& edge_position);

	/*
	功能：对影像拼接边进行羽化处理
	参数：float* imagedata,						视场拼接后影像数据
	int xsize,int ysize,					视场拼接后影像尺寸
	int* xmosaicedge,int *ymosaicedge,int edgePixels	拼接边位置和拼接边像元数目
	*/
	long Level1Proc_ViewJointFeather(unsigned short* imagedata, int xsize, int ysize, vector<CPOINT>& edge_position);


	/*
	功能：视场拼接，向拼接后影像中填数据
	参数：float* imgViewData,								视场数据级
	int xsize,int ysize,								视场影像大小
	float over_left_center_x,float over_right_center_x,左中影像，中右影像x重叠
	float err_left_center_y,float err_right_center_y,	 左中影像，中右影像y错位
	GDALDatasetH m_datasetLeft,GDALDatasetH m_datasetCenter,GDALDatasetH m_datasetRight,左，中，右影像数据集
	float* datamosaic,GDALDatasetH m_datasetOut,int xmosaic,int ymosaic	输出数据，输出数据集，x大小，y大小
	*/
	long Level1Proc_ViewJointFillData(unsigned short* imgViewData, int xsize1, int ysize1, int xsize2, int ysize2, int xsize3, int ysize3, float over_left_center_x, float over_right_center_x, float err_left_center_y, float err_right_center_y,
		GDALDatasetH m_datasetLeft, GDALDatasetH m_datasetCenter, GDALDatasetH m_datasetRight, unsigned short* datamosaic, FILE* fDst, int xmosaic, int ymosaic, vector<CPOINT>& edge_position);
	long Level1Proc_ViewJointFillData(unsigned short* imgViewData, int xsize1, int ysize1, int xsize2, int ysize2, float over_x, float err_y, GDALDatasetH m_datasetLeft, GDALDatasetH m_datasetRight,
		unsigned short* datamosaic, FILE* fDst, int xmosaic, int ymosaic, vector<CPOINT>& edge_position);
	/*
	功能：根据最终影像范围和每景影像结果像数据域中填数据
	参数：float* imgViewData		某个视场数据
	int xsize,int ysize		视场数据大小
	int stposx,int stposy		视场数据起始位置
	float* jointData			视场拼接后数据
	*/
	long Level1Proc_ViewJointFillData(unsigned short* imgViewData, int xsize, int ysize, float stposx, float stposy, unsigned short* jointData, int xmosaic, int ymosaic);
};


//热红外数据的辐射校正
//热红外数据辐射校正主要特点为包含一个光谱校正模块
class THRLevel1Process : public Level1Process
{
public:
	//辐射校正没有参数，不知道怎么校正，先把接口重载了
	//绝对辐射校正
	long Level1Proc_RadiationAbsolute(const char* pathImg, const char* pathImgRad, const char* pathAbsRegFile) { return 0; }
	//获取绝对辐射校正系数
	long Level1Proc_AbsoluteParameters(const char* pathAbsRegFile, float* paramA, float* paramB, float* paramAux = NULL) { return 0; }

	//相对辐射校正
	long Level1Proc_RadiationRelative(const char* pathImg, const char* pathImgRad, const char* pathRelRegFile) { return 0; }
	//获取相对辐射校正系数
	long LevelProc_RelativeParameters(const char* pathRelRegFile, float* paramA, float* paramB, float* paramAux = NULL) { return 0; }

	//对于QPD数据采用分段线性校正 最后一个系数为分段数
	long LevelProc_GetParameterInfo(const char* pathImgRad, int& nSamples, int &nBands, int &nLevels) { return 0; }

	long Level1Proc_AtmosphereCorrect(const char* pathImgRad, const char* pathRef, const char* pathAtmosphere) { return 0; }
	//===============================================================================================================================
	/*
	功能：对热红外高光谱数据进行光谱定标
			参数：1.const char* hyperModtran		：modtran模型模拟的辐亮度文件路径
			2.const char* bandInfo,			：波段信息 （中心波长和半波宽）
			3.const char* fileImg,			：影像路径
			4.const char* plank_line_coeff,	：普朗克线性化系数
			5.float bin,						：模拟光谱分辨率
			6.float begWaveLength,			：光谱波长起始位置
			7.float endWaveLength				：光谱波长终止的位置
	*/
	long Level1Proc_SpectarlCalibTIR(const char* hyperModtran, const char* bandInfo, const char* fileImg, const char* plank_line_coeff, float bin, float begWaveLength, float endWaveLength, const char* pathShift);

	/*
	功能：对热红外高光谱数据进行光谱定标
			参数：1.const char* hyperModtran		：modtran模型模拟的辐亮度文件路径
			2.const char* bandInfo,			：波段信息 （中心波长和半波宽）
			3.const char* fileImg,			：影像路径
			4.const char* plank_line_coeff,	：普朗克线性化系数
			5.float bin,						：模拟光谱分辨率
			6.float begWaveLength,			：光谱波长起始位置
			7.float endWaveLength				：光谱波长终止的位置
			8.int sampleNum					:采样间隔
	*/
	long Level1Proc_SpectarlCalibTIRSample(const char* hyperModtran, const char* bandInfo, const char* fileImg,
		const char* plank_line_coeff, float bin, float begWaveLength,
		float endWaveLength, int sampleNum, const char* pathShift);

private:
	/*
		功能：通过MODTRAN模型生成.tp5格式文件供MODTRAN5模型调用作为输入参数
		参数：1.const char* tap_name   MODTRAN参数输出文件名
		2.const char* modtranDir MODTRAN可执行程序路径
		3.char* SURREF1反射率模型 "BRDF" "LAMBER" >=0或为空 <0
		4.float V11 起始（频率）波长
		5.float V12 最终（频率）波长
	*/
	long Level1Proc_MODTRAN_Batch(const char* tap_name, const char* modtranDir, char* SURREF1, float V11, float V21);
	/*
		功能：进行光谱数据的模拟
		参数：1.float *hyperRadiance 超光谱影像的辐亮度
		2.float* hyperWaveLength 超光谱影像的波长
		3.float *center_wave_Length 实际波段数据的中心波长
		4.int hyperNum		  超光谱影像的波段数
		5.float *band_FWHM	  每个波段的FWHM
		6.int bandNum			  实际数据的波段数
		7.float shift_lamda,float delta_shift_lamda 光谱中心波长的偏移和FWHM的偏移
		8.float* simuluate_radiance 得到的模拟光谱辐亮度数据
	*/
	long Level1Proc_Simulate(float *hyperRadiance, float* hyperWaveLength, float *center_wave_Length, int hyperNum,
		float *band_FWHM, int bandNum, float shift_lamda, float delta_shift_lamda, float* simuluate_radiance);
	/*
		功能：NODD变换（Normalized optical depth derivative）
		参数：1.float* radiance 辐亮度数据
		2.int index_min   大气吸收谷起始位置
		3.int index_max	大气吸收谷终止位置
		4.float* radiance_NODD 变换后的归一化辐亮度
	*/
	long Level1Proc_NODD(float* radiance, int index_min, int index_max, float* radiance_NODD);

	/*
		功能：计算光谱代价函数
		参数：1.float* simulateRadiance 模拟辐亮度
		2.float* realRadiance	   真实辐亮度
		3.int index_min		   大气吸收谷起始位置
		4.int index_max		   大气吸收谷终止位置
		返回误差
	*/
	float Level1Proc_MeritFunc(float* simulateRadiance, float* realRadiance, int index_min, int index_max);

	/*
		功能：使用鲍威尔算法进行迭代求解,初始值为（0，0）；两个方向为（0，1）（1，0），代价函数为Level1Proc_MeritFunc；
		参数：1.float* optParam	最优解
		2.float* hyperSpectral 超光谱辐亮度数据
		3.float *hyperWavelength 超光谱波段波长
		4.int hyperNum	超光谱波段数
		5.float* bandCenterLength 实验室定标中心波长
		6.float* bandFWHM 实验室定标的半波宽
		7.float* realSpectral 实际光谱数据
		8.int realNum	实际光谱波段数
		9.int index_min 大气吸收波段起始波长
		10.int index_max 大气吸收波段最大波长
	*/
	long Level1Proc_Powell(float* optParam, float* hyperSpectral, float *hyperWavelength, int hyperNum,
		float* bandCenterLength, float* bandFWHM, float* realSpectral, int realNum,
		int index_min, int index_max);

};

