#pragma once
/*
1.栅格数据矢化
2.通过ROI区域文件进行影像裁剪
3.离散数据网格化，离散点构建三角网
4.SURF算子进行同名点匹配
5.通过地理坐标对影像进行拼接
*/

#include "gdal/include/gdal_priv.h"
#include "gdal/include/gdalwarper.h" 
#include "gdal/include/gdal_alg_priv.h"
#include "gdal/include/gdal_alg.h"
#include "gdal/include/ogrsf_frmts.h"

#include <iostream>
#include <io.h>
#include <iomanip>
#include <fstream>

#include<vector>
#include<string>

using namespace std;
#pragma comment(lib,"gdal_i.lib")
/*
栅格数据矢量化
功能：1.将给定的栅格数据矢量化，矢量化的方法为将同一灰度值且相邻的区域合并为一个矢量
2.对所有影像获取其矢量区域，并将其矢量区域求并集，获取矢量区域的方法为所有非0像元转换为一个矢量
3.将影响所有非0像素转换为矢量区域
4.矢量区域求并
*/
class RasterToPolygon
{
public:
	/*
	功能：将栅格数据矢量化
	参数：1.const char * pszSrcFile 源影像文件
	2.const char* pszDstFile  得到的矢量数据
	3.const char* pszFormat   矢量数据格式如：ESRI Shapefile
	*/
	long GDALTool_RasterToSHP(const char * pszSrcFile, const char* pszDstFile, const char* pszFormat);

	/*
	功能：将拼接影像边界转换为同一矢量文件
	参数：1.vector<string> vStrSrcFiles 影像路径
	2.char* pathDst 合并后的矢量路径
	*/
	long GDALTool_TransImageToPolygon(vector<string> vStrSrcFiles, char* pathDst);

	/*
	功能：将所有影像非0值得区域转换为矢量文件
	参数：1.const char* pathSrc 影像路径
	2.const char* pathDst 矢量结果路径
	*/
	long GDALTool_ImageNoneZeroToPolygon(const char* pathSrc, const char* pathDst);

	/*
	功能：合并矢量区文件
	参数：1.char** path 待合并的矢量区文件路径
	2.char* pathDst 输出结果路径
	3.int number 需要合并的文件数目
	*/
	long GDALTool_ImageMergePolygon(char** path, char* pathDst, int number);
};
//=========================================================================================================================================

/*
影像进行ROI裁剪
注意：ROI裁剪必须影像与矢量都有地理坐标且地理坐标想匹配
*/
class ROICut {
public:
	/*
	功能：进行影像AOI区域裁剪
	参数：1.const char* pszInFile输入的影像文件
	2.const char* pszOutFile 输出裁剪后的结果
	3. const char* pszAOIFile裁剪区域文件
	4.const char* pszSQL SQL语句获取裁剪区域
	5.int *pBandInex 指定裁剪的波段，默认为NULL，表示裁剪所有波段
	6.int *piBandCount 指定裁剪波段的个数，同上一个参数同时使用
	7.int iBuffer指定AOI文件外扩范围，默认为0，表示不外扩
	8.const char* pszFormat截图后输出文件的格式
	*/
	long GDALTool_ROICut(const char* pszInFile, const char* pszOutFile, const char* pszAOIFile, const char* pszSQL,
		int *pBandInex, int *piBandCount, int iBuffer, const char* pszFormat);

private:
	void
		GDALTool_LoadCutline(const char *pszCutlineDSName, const char *pszCLayer,
			const char *pszCWHERE, const char *pszCSQL,
			void **phCutlineRet);


	void
		GDALTool_TransformCutlineToSource(GDALDatasetH hSrcDS, void *hCutline,
			char ***ppapszWarpOptions, char **papszTO_In);


	//影像坐标和投影坐标之间的转换
	bool GDALTool_Projection2ImageRowCol(double *adfGeoTransform, double dProjX, double dProjY, int &iCol, int &iRow);

	bool GDALTool_ImageRowCol2Projection(double *adfGeoTransform, int iCol, int iRow, double &dProjX, double &dProjY);
};
//=========================================================================================================================================

/*
离散点网格化
功能：1.离散点构成规则格网，以影像形式显示
2.离散点构成三角网，以文件形式存储
*/
class GridTrans {
public:
	/*
	功能：离散数据点构成规则格网
	参数：1.GDALGridAlgorithm gridAlg 插值算法
	typedef enum {
	GGA_InverseDistanceToAPower = 1,
	GGA_MovingAverage = 2,
	GGA_NearestNeighbor = 3,
	GGA_MetricMinimum = 4,
	GGA_MetricMaximum = 5,
	GGA_MetricRange = 6,
	GGA_MetricCount = 7,
	GGA_MetricAverageDistance = 8,
	GGA_MetricAverageDistancePts = 9,
	GGA_Linear = 10,
	GGA_InverseDistanceToAPowerNearestNeighbor = 11
	} GDALGridAlgorithm;
	2.const void *poOptions 插值参数：
	// 构造插值算法参数并设置插值参数值
	GDALGridInverseDistanceToAPowerOptions *poOptions = new GDALGridInverseDistanceToAPowerOptions();
	poOptions->dfPower = 2;				反幂法的系数
	poOptions->dfRadius1 = 20;			最大搜索半径
	poOptions->dfRadius2 = 15;			最小搜索半径

	GDALGridMovingAverageOptions *poOptions = new GDALGridMovingAverageOptions();
	poOptions->dfRadius1 = 20;
	poOptions->dfRadius2 = 15;

	GDALGridNearestNeighborOptions  *poOptions = new GDALGridNearestNeighborOptions();
	poOptions->dfRadius1 = 20;
	poOptions->dfRadius2 = 15;

	3.double* points 离散数据点，数据点构成为 x，y，value
	4.int pointNumber 离散数据点个数
	5.double pixelResolution 像元的分辨率
	6.const char* pathDst 输出结果影像
	*/
	long GDALTool_Grid(GDALGridAlgorithm gridAlg, const void *poOptions, double* points, int pointNumber, double pixelResolution, const char* pathDst);
	long GDALGridCreate(GDALGridAlgorithm eAlgorithm, const void *poOptions, GUInt32 nPoints,
		const double *padfX, const double *padfY, const double *padfZ,
		double dfXMin, double dfXMax, double dfYMin, double dfYMax,
		GUInt32 nXSize, GUInt32 nYSize, GDALDataType eType, void *pData);

	/*
	功能：根据影像的经纬度转换为矢量点数据
	参数：1.const char* pathLatLong 经纬度文件：经纬度的文件格式为
	pointID		Longitude	  Latitude
	dataItem:
	2.const char* pathShp 生成的矢量点文件
	*/
	long GDALLatLongToSHP(const char* pathLatLong, const char* pathShp);

	/*
	功能：创建Delaunay三角网
	参数：1.double* points 离散点
	2.int pointNumber 离散点的个数
	3.const char* pathDst 输出文件路径
	*/
	long GDALTool_TriangleDelaunay(double* points, int pointNumber, const char* pathDst);

	/*
	功能：从文件中获取Delaunary三角网
	参数：1.const char* pathTri 文件路径
	2.double* pdfTriVex   顶点文件
	3.int number			定点个数
	*/
	GDALTriangulation* GDALTool_LoadTriangle(const char* pathTri, double* pdfTriVex, int &number);
};
//=========================================================================================================================================

/*
通过地理坐标对影像进行拼接
功能：1.获取去除后缀的文件名
2.创建输出文件
3.转换AOI到源文件之间的行列号
4.加载镶嵌块
5.影像镶嵌
*/
class GeoMosaic {
private:

	//图像范围
	double	       dfMinX, dfMinY, dfMaxX, dfMaxY;
	//图像分辨率
	double	       dfXRes, dfYRes;
	//图像宽高
	int             nForcePixels, nForceLines;
	//图像是否具有Alpha通道
	int             bEnableDstAlpha, bEnableSrcAlpha;

public:
	GeoMosaic() :dfMinX(0.0), dfMinY(0.0), dfMaxX(0.0), dfMaxY(0.0), dfXRes(0.0), dfYRes(0.0),
		nForceLines(0), nForcePixels(0), bEnableDstAlpha(FALSE), bEnableSrcAlpha(FALSE)
	{

	}

	/**
	转换投影坐标到图像行列号坐标
	用于图像裁切中，按照AOI来裁切时转换AOI坐标到图像行列号坐标
	**/
	class CutlineTransformer : public OGRCoordinateTransformation
	{
	public:

		/**
		* @brief 源图像转换参数
		*/
		void *hSrcImageTransformer;

		/**
		* @brief 获取源数据空间参考坐标系
		* @return 返回源数据空间参考坐标系
		*/
		virtual OGRSpatialReference *GetSourceCS() { return NULL; }

		/**
		* @brief 获取目标数据空间参考坐标系
		* @return 返回目标数据空间参考坐标系
		*/
		virtual OGRSpatialReference *GetTargetCS() { return NULL; }

		/**
		* @brief 批量转换投影坐标到行列号
		* @param nCount		坐标点个数
		* @param x			x坐标串
		* @param y			y坐标串
		* @param z			z坐标串
		* @return 返回是否执行正确
		*/
		virtual int Transform(int nCount, double *x, double *y, double *z = NULL)
		{
			int nResult;

			int *pabSuccess = (int *)CPLCalloc(sizeof(int), nCount);
			nResult = TransformEx(nCount, x, y, z, pabSuccess);
			CPLFree(pabSuccess);

			return nResult;
		}

		/**
		* @brief 批量转换投影坐标到行列号
		* @param nCount		坐标点个数
		* @param x			x坐标串
		* @param y			y坐标串
		* @param z			z坐标串
		* @param pabSuccess	保存执行成功的数组
		* @return 返回是否执行正确
		*/
		virtual int TransformEx(int nCount, double *x, double *y,
			double *z = NULL, int *pabSuccess = NULL)
		{
			return GDALGenImgProjTransform(hSrcImageTransformer, TRUE,
				nCount, x, y, z, pabSuccess);
		}
	};


	/*
	功能：获取文件名除去后缀
	参数：1.const char* pszFile：输入影像名称
	*/
	inline string GDALTool_GetFileName(const char* pszFile);


	/*
	功能：创建输出文件
	参数：1.char **papszSrcFiles：输入文件列表
	2.const char *pszFilename  ：输出文件路径
	3.const char *pszFormat    : 输出文件格式
	4.char **papszTO		    : 转换选项
	5.char ***ppapszCreateOptions: 创建文件选项
	6.GDALDataType eDT			：创建文件数据类型
	*/
	GDALDatasetH GDALTool_GDALWarpCreateOutput(char **papszSrcFiles, const char *pszFilename,
		const char *pszFormat, char **papszTO,
		char ***ppapszCreateOptions, GDALDataType eDT);


	/*
	功能：转换AOI到源文件之间的行列号
	参数：1.GDALDatasetH hSrcDS ：输入文件GDAL数据集句柄
	2.void *hCutline      ：裁切的几何形状
	3.char ***ppapszWarpOptionst: 转换选项，用于配置裁切参数
	4.char **papszTO		     : 转换选项
	5.char **papszTO_In		 : 转换选项
	*/
	void GDALTool_TransformCutlineToSource(GDALDatasetH hSrcDS, void *hCutline,
		char ***ppapszWarpOptions, char **papszTO_In);


	/*
	功能：加载镶嵌块
	参数：1.const char *pszCutlineDSName ：镶嵌块文件
	2.void *hCutline       ：裁切的几何形状
	3.const char *pszCLayer: 镶嵌块图层名称，可以为NULL
	4.const char *pszCWHERE: 镶嵌块过滤字段
	5.const char *pszCSQL	: 镶嵌块SQL过滤字段
	6.void **phCutlineRet : 返回的镶嵌块数据指针
	*/
	long GDALTool_LoadCutline(const char *pszCutlineDSName, const char *pszCLayer,
		const char *pszCWHERE, const char *pszCSQL,
		void **phCutlineRet);

	/*
	功能：影像镶嵌
	* 注意事项，调用此函数之前，请先对影像进行几何纠正到同一投影坐标系统下，分辨率可以不同，
	* 但是投影信息以及输入的各个数据的波段个数必须一致，否则会出现不能正常完成镶嵌操作。
	* 图像的分辨率会按照输入影像的第一个影像获取，包括投影等信息					*
	* 函数参数：vector<string> vStrSrcFiles ：输入文件以及镶嵌线路径数组，			*
	默认第一个为参考影像，第一个图像在最下层，后面的依次向上				*
	*          const char* pszCutLineFile  ：输入镶嵌块文件文件路径数组				*
	*		   const char* pszOutFile: 输出文件路径									*
	*		   GDALResampleAlg eResampleMethod: 重采样方式							*
	*		   const char *pszCSQL	: 镶嵌块SQL过滤字段								*
	*		   const char *pszFormat:输出文件格式，详细参考GDAL支持数据类型			*
	*/
	long GDALTool_ImageMosaicing(vector<string> vStrSrcFiles, const char* pszCutLineFile, const char* pszOutFile,
		GDALResampleAlg eResampleMethod, const char *pszFormat);

	//封装为影像起止标号的格式
	long GDALTool_ImageMosaicing(char* path, int numBeg, int numEnd, const char* pszCutLineFile, const char* pszOutFile,
		GDALResampleAlg eResampleMethod, const char *pszFormat);
};
//=========================================================================================================================================

/*
根据影像点文件生成shp格式的区域文件
*/
struct GdPos
{
	double x;
	double y;
};
class CAreaFileGet
{
public:
	/*
	功能：设置点坐标
	参数：组成封闭区域的点的坐标
	*/
	long setPolygon(vector<GdPos> polygon);

	/*
	功能：点文件转换为区文件
	参数：1.const char* projRef 投影信息
	2.const char* pathOut 输出shp文件路径
	*/
	long transTOpolygon(const char* projRef, const char* pathOut);

private:
	vector<vector<GdPos>> m_polygons;
};
//=========================================================================================================================================


/*
使用控制点进行几何校正
功能：1.使用控制点进行多项式几何校正
2.使用控制点进行TPS几何校正
3.使用控制点进行Triangle分割几何校正
4.使用RPC参数进行校正
5.根据GeoLoc进行校正
*/
class GeoGCPProcess
{
public:
	/*
	功能：使用控制点进行几何校正
	参数：1.const char *pszSrcFilename 待校正的影像
	2.const char *pszDstFilename 校正后的影像
	3.int nReqOrder 多项式阶数
	4.int bReversed 作用不明
	5.const char *pszReferenceWKT 投影系
	6.double *dGroundSize 影像分辨率大小
	7.double* GCPs,int gcpNum 控制点对和控制点的数目
	*/
	long GeoProc_GCPWarpOrder(const char *pszSrcFilename, const char *pszDstFilename, int nReqOrder, int bReversed, const char *pszReferenceWKT, double *dGroundSize, double* GCPs, int gcpNum);

	/*
	功能：使用TPS方法进行几何校正
	参数：1.const char *pszSrcFilename 待校正的影像
	2.const char *pszDstFilename 校正后的影像
	3.const char* pszDstWkt 投影坐标系
	4.double* dGroundSize 影像分辨率
	5.GDALResampleAlg eResampleMethod 重采样方法
	6.,double* GCPs,int gcpNum 控制点对和控制点的数目
	*/
	long GeoProc_GCPWarpTPS(const char* pszSrcFilename, const char* pszDstFilename, const char* pszDstWkt, double* dGroundSize, GDALResampleAlg eResampleMethod, double* GCPs, int gcpNum);

	/*
	功能：使用RPC参数对影像进行几何校正
	参数：1.const char * pszSrcFile 输入文件路径
	2.const char * pszDstFile 输出文件路径
	3.GDALRPCInfo *psRPC  RPC信息
	4.double dfPixErrThreshold RPC坐标反算迭代阈值
	5.char **papszOptions RPC校正高程设置参数
	6.GDALResampleAlg eResampleMethod 重采样方法
	7.const char * pszFormat 输出文件格式驱动
	*/
	long GeoProc_GCPWarpRPC(const char * pszSrcFile, const char * pszDstFile, GDALRPCInfo *psRPC, double dfPixErrThreshold, char **papszOptions,
		GDALResampleAlg eResampleMethod, const char * pszFormat);

	/*
	功能：使用三角网法对影像分区域进行几何校正，首先将控制点生成三角网，对三角网内数据进行校正
	参数：1.const char* pathSrcImg 源影像路径
	2.const char* pathDstImg 输出影像路径
	3.const char* pathTri	输出三角网文件
	4.double* GCPs,int gcpNum 控制点和控制点数目
	5.UTMZone   UTM投影带号
	6.GroundSize 地面分辨率
	*/
	long GeoProc_GCPTriangle(char* pathSrcImg, char* pathDstImg, char* pathTri, double* GCPs, int gcpNum, int UTMZone, double GroundSize);

	/*
	功能：针对HDF影像使用的GeoLoc影像校正方法
	参数：1.const char * pszSrcFile 输入影像路径
	2.const char * pszDstFile 输出影像路径
	3. char **papszGeoLocOptions  GeoLoc信息
	4.GDALResampleAlg eResampleMethod 重采样方法
	5. const char * pszFormat 数据格式驱动
	*/
	long GeoProc_GCPWarpGeoLoc(const char * pszSrcFile, const char * pszDstFile, char **papszGeoLocOptions, GDALResampleAlg eResampleMethod, const char * pszFormat);

	/*
	功能：获取GCP数据参数
	参数：1.const char* pathGCP ：控制点文件
	2.double *gcps        ：得到的控制点
	3.int &num			：控制点数目
	4.int headline=0		：头文件行数
	*/
	long GeoProc_GetENVIGcp(const char* pathGCP, double *gcps, int num, int headline = 0);
};

;