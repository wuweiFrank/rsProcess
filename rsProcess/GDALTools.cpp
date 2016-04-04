#include "GDALTools.h"
#include "Global.h"
#include"AuxiliaryFunction.h"

#include "gdal/include/gdal_frmts.h"
#include "gdal/include/ogrsf_frmts.h"
#include "gdal/include/cpl_vsi.h"
#include "gdal/include/cpl_string.h"
#include "gdal/include/gdalgrid.h"

#include "gdal/include/cpl_multiproc.h"
#include "gdal/include/gdalgrid_priv.h"

#include "tsmUTM.h"
#include <float.h>
#include <limits.h>
#include <fstream>
#include <iomanip>
#include <io.h>

using namespace std;

/*
栅格数据矢量化
功能：1.将给定的栅格数据矢量化，矢量化的方法为将同一灰度值且相邻的区域合并为一个矢量
2.对所有影像获取其矢量区域，并将其矢量区域求并集，获取矢量区域的方法为所有非0像元转换为一个矢量
3.将影响所有非0像素转换为矢量区域
4.矢量区域求并
*/
long RasterToPolygon::GDALTool_RasterToSHP(const char * pszSrcFile, const char* pszDstFile, const char* pszFormat)
{
	GDALAllRegister();
	OGRRegisterAll();//记得添加驱动注册
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	GDALDataset* poSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	if (poSrcDS == NULL)
		return -1;
	// 创建输出矢量文件
	GDALDriver *poDriver;
	poDriver = (GDALDriver *)OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszFormat);
	if (poDriver == NULL)
	{
		GDALClose((GDALDatasetH)poSrcDS);
		return -1;
	}
	//根据文件名创建输出矢量文件
	GDALDataset* poDstDS = poDriver->Create(pszDstFile, 0, 0, 0, GDT_Unknown, NULL);
	if (poDstDS == NULL)
	{
		GDALClose((GDALDatasetH)poSrcDS);
		return -1;
	}
	// 定义空间参考，与输入图像相同;
	const char* proj = poSrcDS->GetProjectionRef();
	OGRSpatialReference *poSpatialRef = new OGRSpatialReference(poSrcDS->GetProjectionRef());
	OGRLayer* poLayer = poDstDS->CreateLayer("Result", poSpatialRef, wkbPolygon, NULL);
	if (poDstDS == NULL)
	{
		GDALClose((GDALDatasetH)poSrcDS);
		GDALClose(poDstDS);
		delete poSpatialRef;
		poSpatialRef = NULL;
		return -1;
	}
	OGRFieldDefn ofieldDef("Segment", OFTInteger); //创建属性表，只有一个字段即“Segment”，里面保存对应的栅格的像元值
	poLayer->CreateField(&ofieldDef);
	GDALRasterBandH hSrcBand = (GDALRasterBandH)poSrcDS->GetRasterBand(1); //获取图像的第一个波段
	GDALPolygonize(hSrcBand, NULL, (OGRLayerH)poLayer, 0, NULL, NULL, NULL); //调用栅格矢量化
	GDALClose(poSrcDS); //关闭文件
	GDALClose(poDstDS);
	return 0;
}
long RasterToPolygon::GDALTool_ImageNoneZeroToPolygon(const char* pathSrc, const char* pathDst)
{
	if (pathSrc == NULL || pathDst == NULL)
		return -1;
	char* temp = "~temp1.tif";
	GDALAllRegister();
	GDALDatasetH datasetsrc;
	GDALDatasetH datasetdst;

	if ((datasetsrc = (GDALOpen(pathSrc, GA_ReadOnly))) == NULL)
		return -1;

	int x = GDALGetRasterXSize(datasetsrc);
	int y = GDALGetRasterYSize(datasetsrc);
	int *datasrc = new int[x*y];
	GDALRasterBandH bandH = GDALGetRasterBand(datasetsrc, 1);
	GDALRasterIO(bandH, GF_Read, 0, 0, x, y, datasrc, x, y, GDT_Float32, 0, 0);
	char **papszOptions = NULL;
	papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BAND");
	datasetdst = GDALCreate(GDALGetDriverByName("GTiff"), temp, x, y, 1, GDT_Int32, papszOptions);

	bool meanvalue = true;
	for (int i = 0; i<x - 2; i++)
	{
		for (int j = 0; j<y - 2; j++)
		{
			for (int k = 0; k<3; k++)
				for (int l = 0; l<3; l++)
					if (datasrc[(l + j)*x + i] == 0)
						meanvalue = false;
			if (meanvalue == true)
				datasrc[j*x + i] = 1;
			else
				datasrc[j*x + i] = 0;
			meanvalue = true;
		}
	}
	GDALRasterIO((GDALGetRasterBand(datasetdst, 1)), GF_Write, 0, 0, x, y, datasrc, x, y, GDT_Int32, 0, 0);
	double adfTransform[6];
	GDALGetGeoTransform(datasetsrc, adfTransform);
	const char* proj = GDALGetProjectionRef(datasetsrc);
	GDALSetGeoTransform(datasetdst, adfTransform);
	GDALSetProjection(datasetdst, proj);

	GDALClose(datasetdst);
	GDALClose(datasetsrc);

	GDALTool_RasterToSHP(temp, pathDst, "ESRI Shapefile");

	std::remove(temp);
	return 0;

}
long RasterToPolygon::GDALTool_ImageMergePolygon(char** path, char* pathDst, int number)
{
	//检查输入
	if (path == NULL || pathDst == NULL || number<1)
		return -1;

	OGRRegisterAll();
	///获取第一幅影像的投影作为投影信息
	GDALDataset *datasource;
	datasource = (GDALDataset*)GDALOpenEx(path[0], GDAL_OF_VECTOR, NULL, NULL, NULL);
	OGRLayer *pLayerdata = datasource->GetLayer(0);
	pLayerdata->ResetReading();

	//数据驱动
	const char *pszDriverName = "ESRI Shapefile";
	GDALDriver *poDriver;

	poDriver = (GDALDriver *)OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	GDALDataset *poDS;
	poDS = poDriver->Create(pathDst, 0, 0, 0, GDT_Unknown, NULL);
	OGRLayer* pLayerDS;
	/*pLayerDS=poDS->CreateLayer("MergePolygon",ogrSpatialRef,wkbPolygon,NULL);*/
	pLayerDS = poDS->CopyLayer(pLayerdata, "MergePolygon", NULL);
	GDALClose(datasource);

	for (int i = 1; i<number; i++)
	{
		//获取输入矢量文件
		GDALDataset*  datasourceSrc;
		datasourceSrc = (GDALDataset*)GDALOpenEx(path[i], GDAL_OF_VECTOR, NULL, NULL, NULL);

		//获取第一层信息  只有一层
		OGRLayer*   pLayer;
		int count1 = datasourceSrc->GetLayerCount();
		pLayer = datasourceSrc->GetLayer(0);
		pLayer->ResetReading();
		OGRFeature* feature;
		feature = NULL;

		while ((feature = pLayer->GetNextFeature()) != NULL)
		{
			if (feature != NULL)
				if (pLayerDS->CreateFeature(feature) != OGRERR_NONE)
					return -2;
			OGRFeature::DestroyFeature(feature);
		}
		GDALClose(datasourceSrc);
	}
	GDALClose(poDS);

	return 0;
}
long RasterToPolygon::GDALTool_TransImageToPolygon(vector<string> vStrSrcFiles, char* pathDst)
{
	//检查输入
	if (pathDst == NULL || vStrSrcFiles.empty())
		return -1;

	long lError;
	int number = vStrSrcFiles.size();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char **fname = new char*[number];
	char ext[_MAX_EXT];
	char *shp = "shp";
	char *prj = "prj";
	char *shx = "shx";
	char *dbf = "dbf";
	char **path = new char *[number];
	for (int i = 0; i<number; i++)
	{
		fname[i] = new char[_MAX_FNAME];
		path[i] = new char[_MAX_PATH];
		_splitpath_s(vStrSrcFiles[i].data(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname[i], _MAX_FNAME, ext, _MAX_EXT);
		_makepath_s(path[i], _MAX_PATH, drive, dir, fname[i], shp);

		//获取影像矢量区域
		lError = GDALTool_ImageNoneZeroToPolygon(vStrSrcFiles[i].c_str(), path[i]);
	}

	//合并影像矢量区域
	lError = GDALTool_ImageMergePolygon(path, pathDst, number);

	//删除多余数据
	for (int i = 0; i<number; i++)
	{
		_makepath_s(path[i], _MAX_PATH, drive, dir, fname[i], shp);
		std::remove(path[i]);
		_makepath_s(path[i], _MAX_PATH, drive, dir, fname[i], prj);
		std::remove(path[i]);
		_makepath_s(path[i], _MAX_PATH, drive, dir, fname[i], shx);
		std::remove(path[i]);
		_makepath_s(path[i], _MAX_PATH, drive, dir, fname[i], dbf);
		std::remove(path[i]);
		delete[]fname[i];
		delete[]path[i];
	}
	return lError;
}
//=========================================================================================================================================

/*
影像进行ROI裁剪
注意：ROI裁剪必须影像与矢量都有地理坐标且地理坐标想匹配
*/
/************************************************************************/
/*                      GeoTransform_Transformer()                      */
/*                                                                      */
/*      Convert points from georef coordinates to pixel/line based      */
/*      on a geotransform.                                              */
/************************************************************************/
class CutlineTransformer : public OGRCoordinateTransformation
{
public:

	void         *hSrcImageTransformer;

	virtual OGRSpatialReference *GetSourceCS() { return NULL; }
	virtual OGRSpatialReference *GetTargetCS() { return NULL; }

	virtual int Transform(int nCount,
		double *x, double *y, double *z = NULL) {
		int nResult;

		int *pabSuccess = (int *)CPLCalloc(sizeof(int), nCount);
		nResult = TransformEx(nCount, x, y, z, pabSuccess);
		CPLFree(pabSuccess);

		return nResult;
	}

	virtual int TransformEx(int nCount,
		double *x, double *y, double *z = NULL,
		int *pabSuccess = NULL) {
		return GDALGenImgProjTransform(hSrcImageTransformer, TRUE,
			nCount, x, y, z, pabSuccess);
	}
};
void
ROICut::GDALTool_LoadCutline(const char *pszCutlineDSName, const char *pszCLayer,
	const char *pszCWHERE, const char *pszCSQL,
	void **phCutlineRet)

{
	OGRRegisterAll();

	/* -------------------------------------------------------------------- */
	/*      Open source vector dataset.                                     */
	/* -------------------------------------------------------------------- */
	OGRDataSourceH hSrcDS;

	hSrcDS = OGROpen(pszCutlineDSName, FALSE, NULL);
	if (hSrcDS == NULL)
		exit(1);

	/* -------------------------------------------------------------------- */
	/*      Get the source layer                                            */
	/* -------------------------------------------------------------------- */
	OGRLayerH hLayer = NULL;

	if (pszCSQL != NULL)
		hLayer = OGR_DS_ExecuteSQL(hSrcDS, pszCSQL, NULL, NULL);
	else if (pszCLayer != NULL)
		hLayer = OGR_DS_GetLayerByName(hSrcDS, pszCLayer);
	else
		hLayer = OGR_DS_GetLayer(hSrcDS, 0);

	if (hLayer == NULL)
	{
		fprintf(stderr, "Failed to identify source layer from datasource./n");
		exit(1);
	}

	/* -------------------------------------------------------------------- */
	/*      Apply WHERE clause if there is one.                             */
	/* -------------------------------------------------------------------- */
	if (pszCWHERE != NULL)
		OGR_L_SetAttributeFilter(hLayer, pszCWHERE);

	/* -------------------------------------------------------------------- */
	/*      Collect the geometries from this layer, and build list of       */
	/*      burn values.                                                    */
	/* -------------------------------------------------------------------- */
	OGRFeatureH hFeat;
	OGRGeometryH hMultiPolygon = OGR_G_CreateGeometry(wkbMultiPolygon);

	OGR_L_ResetReading(hLayer);

	while ((hFeat = OGR_L_GetNextFeature(hLayer)) != NULL)
	{
		OGRGeometryH hGeom = OGR_F_GetGeometryRef(hFeat);

		if (hGeom == NULL)
		{
			fprintf(stderr, "ERROR: Cutline feature without a geometry./n");
			exit(1);
		}

		OGRwkbGeometryType eType = wkbFlatten(OGR_G_GetGeometryType(hGeom));

		if (eType == wkbPolygon)
			OGR_G_AddGeometry(hMultiPolygon, hGeom);
		else if (eType == wkbMultiPolygon)
		{
			int iGeom;

			for (iGeom = 0; iGeom < OGR_G_GetGeometryCount(hGeom); iGeom++)
			{
				OGR_G_AddGeometry(hMultiPolygon,
					OGR_G_GetGeometryRef(hGeom, iGeom));
			}
		}
		else
		{
			fprintf(stderr, "ERROR: Cutline not of polygon type./n");
			exit(1);
		}

		OGR_F_Destroy(hFeat);
	}

	if (OGR_G_GetGeometryCount(hMultiPolygon) == 0)
	{
		fprintf(stderr, "ERROR: Did not get any cutline features./n");
		exit(1);
	}

	/* -------------------------------------------------------------------- */
	/*      Ensure the coordinate system gets set on the geometry.          */
	/* -------------------------------------------------------------------- */
	OGR_G_AssignSpatialReference(
		hMultiPolygon, OGR_L_GetSpatialRef(hLayer));

	*phCutlineRet = (void *)hMultiPolygon;

	/* -------------------------------------------------------------------- */
	/*      Cleanup                                                         */
	/* -------------------------------------------------------------------- */
	if (pszCSQL != NULL)
		OGR_DS_ReleaseResultSet(hSrcDS, hLayer);

	OGR_DS_Destroy(hSrcDS);
}
void
ROICut::GDALTool_TransformCutlineToSource(GDALDatasetH hSrcDS, void *hCutline,
	char ***ppapszWarpOptions, char **papszTO_In)

{
	OGRGeometryH hMultiPolygon = OGR_G_Clone((OGRGeometryH)hCutline);
	char **papszTO = CSLDuplicate(papszTO_In);

	/* -------------------------------------------------------------------- */
	/*      Checkout that SRS are the same.                                 */
	/* -------------------------------------------------------------------- */
	OGRSpatialReferenceH  hRasterSRS = NULL;
	const char *pszProjection = NULL;

	if (GDALGetProjectionRef(hSrcDS) != NULL
		&& strlen(GDALGetProjectionRef(hSrcDS)) > 0)
		pszProjection = GDALGetProjectionRef(hSrcDS);
	else if (GDALGetGCPProjection(hSrcDS) != NULL)
		pszProjection = GDALGetGCPProjection(hSrcDS);

	if (pszProjection != NULL)
	{
		hRasterSRS = OSRNewSpatialReference(NULL);
		if (OSRImportFromWkt(hRasterSRS, (char **)&pszProjection) != CE_None)
		{
			OSRDestroySpatialReference(hRasterSRS);
			hRasterSRS = NULL;
		}
	}

	OGRSpatialReferenceH hCutlineSRS = OGR_G_GetSpatialReference(hMultiPolygon);
	if (hRasterSRS != NULL && hCutlineSRS != NULL)
	{
		/* ok, we will reproject */
	}
	else if (hRasterSRS != NULL && hCutlineSRS == NULL)
	{
		fprintf(stderr,
			"Warning : the source raster dataset has a SRS, but the cutline features/n"
			"not.  We assume that the cutline coordinates are expressed in the destination SRS./n"
			"If not, cutline results may be incorrect./n");
	}
	else if (hRasterSRS == NULL && hCutlineSRS != NULL)
	{
		fprintf(stderr,
			"Warning : the input vector layer has a SRS, but the source raster dataset does not./n"
			"Cutline results may be incorrect./n");
	}

	if (hRasterSRS != NULL)
		OSRDestroySpatialReference(hRasterSRS);

	/* -------------------------------------------------------------------- */
	/*      Extract the cutline SRS WKT.                                    */
	/* -------------------------------------------------------------------- */
	if (hCutlineSRS != NULL)
	{
		char *pszCutlineSRS_WKT = NULL;

		OSRExportToWkt(hCutlineSRS, &pszCutlineSRS_WKT);
		papszTO = CSLSetNameValue(papszTO, "DST_SRS", pszCutlineSRS_WKT);
		CPLFree(pszCutlineSRS_WKT);
	}

	/* -------------------------------------------------------------------- */
	/*      Transform the geometry to pixel/line coordinates.               */
	/* -------------------------------------------------------------------- */
	CutlineTransformer oTransformer;

	/* The cutline transformer will *invert* the hSrcImageTransformer */
	/* so it will convert from the cutline SRS to the source pixel/line */
	/* coordinates */
	oTransformer.hSrcImageTransformer =
		GDALCreateGenImgProjTransformer2(hSrcDS, NULL, papszTO);

	CSLDestroy(papszTO);

	if (oTransformer.hSrcImageTransformer == NULL)
		exit(1);

	OGR_G_Transform(hMultiPolygon,
		(OGRCoordinateTransformationH)&oTransformer);

	GDALDestroyGenImgProjTransformer(oTransformer.hSrcImageTransformer);

	/* -------------------------------------------------------------------- */
	/*      Convert aggregate geometry into WKT.                            */
	/* -------------------------------------------------------------------- */
	char *pszWKT = NULL;

	OGR_G_ExportToWkt(hMultiPolygon, &pszWKT);
	OGR_G_DestroyGeometry(hMultiPolygon);

	*ppapszWarpOptions = CSLSetNameValue(*ppapszWarpOptions,
		"CUTLINE", pszWKT);
	CPLFree(pszWKT);
}
bool ROICut::GDALTool_Projection2ImageRowCol(double *adfGeoTransform, double dProjX, double dProjY, int &iCol, int &iRow)
{
	try
	{
		double dTemp = adfGeoTransform[1] * adfGeoTransform[5] - adfGeoTransform[2] * adfGeoTransform[4];
		double dCol = 0.0, dRow = 0.0;
		dCol = (adfGeoTransform[5] * (dProjX - adfGeoTransform[0]) -
			adfGeoTransform[2] * (dProjY - adfGeoTransform[3])) / dTemp + 0.5;
		dRow = (adfGeoTransform[1] * (dProjY - adfGeoTransform[3]) -
			adfGeoTransform[4] * (dProjX - adfGeoTransform[0])) / dTemp + 0.5;

		iCol = static_cast<int>(dCol);
		iRow = static_cast<int>(dRow);
		return true;
	}
	catch (...)
	{
		return false;
	}
}
bool ROICut::GDALTool_ImageRowCol2Projection(double *adfGeoTransform, int iCol, int iRow, double &dProjX, double &dProjY)
{
	//adfGeoTransform[6]  数组adfGeoTransform保存的是仿射变换中的一些参数，分别含义见下

	//adfGeoTransform[0]  左上角x坐标 
	//adfGeoTransform[1]  东西方向分辨率
	//adfGeoTransform[2]  旋转角度, 0表示图像 "北方朝上"
	//adfGeoTransform[3]  左上角y坐标 
	//adfGeoTransform[4]  旋转角度, 0表示图像 "北方朝上"
	//adfGeoTransform[5]  南北方向分辨率

	try
	{
		dProjX = adfGeoTransform[0] + adfGeoTransform[1] * iCol + adfGeoTransform[2] * iRow;
		dProjY = adfGeoTransform[3] + adfGeoTransform[4] * iCol + adfGeoTransform[5] * iRow;
		return true;
	}
	catch (...)
	{
		return false;
	}
}
long ROICut::GDALTool_ROICut(const char* pszInFile, const char* pszOutFile, const char* pszAOIFile, const char* pszSQL,
	int *pBandInex, int *piBandCount, int iBuffer, const char* pszFormat)
{
	GDALAllRegister();
	void *hCutLine = NULL;
	GDALTool_LoadCutline(pszAOIFile, NULL, NULL, pszSQL, &hCutLine);

	GDALDataset * pSrcDS = (GDALDataset*)GDALOpen(pszInFile, GA_ReadOnly);
	if (pSrcDS == NULL)
		return -1;
	int iBandCount = pSrcDS->GetRasterCount();
	const char* pszWkt = pSrcDS->GetProjectionRef();

	//没有投影则定义任意投影信息

	GDALDataType dT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	double adfGeoTransform[6] = { 0 };
	double newGeoTransform[6] = { 0 };

	pSrcDS->GetGeoTransform(adfGeoTransform);
	memcpy(newGeoTransform, adfGeoTransform, sizeof(double) * 6);

	int *pSrcBand = NULL;
	int iNewBandCount = iBandCount;
	if (pBandInex != NULL && piBandCount != NULL)
	{
		int iMaxBandIndex = pBandInex[0];
		for (int i = 1; i<*piBandCount; i++)
		{
			if (iMaxBandIndex < pBandInex[i])
				iMaxBandIndex = pBandInex[i];
		}

		if (iMaxBandIndex > iBandCount)
		{
			GDALClose((GDALDatasetH)pSrcDS);
			return -1;
		}

		iNewBandCount = *piBandCount;
		pSrcBand = new int[iNewBandCount];
		for (int i = 0; i<iNewBandCount; i++)
			pSrcBand[i] = pBandInex[i];
	}
	else
	{
		pSrcBand = new int[iNewBandCount];
		for (int i = 0; i<iNewBandCount; i++)
			pSrcBand[i] = i + 1;
	}

	OGRGeometryH hGeometry = (OGRGeometryH)hCutLine;
	OGRGeometryH hMultiPoly = NULL;
	if (iBuffer > 0)
	{
		double dDistance = ABS(adfGeoTransform[1] * iBuffer);
		hMultiPoly = OGR_G_Buffer(hGeometry, dDistance, 30);
		OGR_G_AssignSpatialReference(hMultiPoly, OGR_G_GetSpatialReference(hGeometry));
		OGR_G_DestroyGeometry(hGeometry);
	}
	else
		hMultiPoly = hGeometry;

	OGREnvelope eRect;
	OGR_G_GetEnvelope(hMultiPoly, &eRect);

	int iNewWidth = 0, iNewHeigh = 0;
	int iBeginRow = 0, iBeginCol = 0;

	newGeoTransform[0] = eRect.MinX;
	newGeoTransform[3] = eRect.MaxY;

	iNewWidth = static_cast<int>((eRect.MaxX - eRect.MinX) / ABS(adfGeoTransform[1]));
	iNewHeigh = static_cast<int>((eRect.MaxY - eRect.MinY) / ABS(adfGeoTransform[5]));

	GDALTool_Projection2ImageRowCol(adfGeoTransform, newGeoTransform[0], newGeoTransform[3], iBeginCol, iBeginRow);
	GDALTool_ImageRowCol2Projection(adfGeoTransform, iBeginCol, iBeginRow, newGeoTransform[0], newGeoTransform[3]);

	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (pDriver == NULL)
	{
		GDALClose((GDALDatasetH)pSrcDS);
		return -1;
	}
	char **papszOptions = NULL;
	papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BAND");
	GDALDataset* pDstDS = pDriver->Create(pszOutFile, iNewWidth, iNewHeigh, iNewBandCount, dT, papszOptions);
	if (pDstDS == NULL)
	{
		GDALClose((GDALDatasetH)pSrcDS);
		return -1;
	}

	OGRSpatialReference *poSpatialRef = new OGRSpatialReference(pszWkt);

	//无投影信息则修改地理转换第一个位置
	if (strlen(pszWkt) == 0)
	{
		poSpatialRef->SetProjCS("UTM 41(WGS84) in northern hemisphere.");
		poSpatialRef->SetWellKnownGeogCS("WGS84");
		poSpatialRef->SetUTM(41, TRUE);
		char* proj;
		poSpatialRef->exportToWkt(&proj);
		pszWkt = proj;

		adfGeoTransform[0] = adfGeoTransform[3] = 1;
		pSrcDS->SetGeoTransform(adfGeoTransform);

	}


	pDstDS->SetGeoTransform(newGeoTransform);
	pDstDS->SetProjection(pszWkt);

	void *hTransformArg, *hGenImgProjArg = NULL;
	char **papszTO = NULL;
	/* -------------------------------------------------------------------- */
	/*      Create a transformation object from the source to               */
	/*      destination coordinate system.                                  */
	/* -------------------------------------------------------------------- */
	hTransformArg = hGenImgProjArg =
		GDALCreateGenImgProjTransformer2(pSrcDS, (GDALDatasetH)pDstDS, papszTO);

	if (hTransformArg == NULL)
	{
		GDALClose((GDALDatasetH)pSrcDS);
		GDALClose((GDALDatasetH)pDstDS);
		return -1;
	}

	GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = dT;
	psWO->eResampleAlg = GRA_Bilinear;

	psWO->hSrcDS = (GDALDatasetH)pSrcDS;
	psWO->hDstDS = (GDALDatasetH)pDstDS;

	psWO->pfnTransformer = pfnTransformer;
	psWO->pTransformerArg = hTransformArg;

	psWO->nBandCount = iNewBandCount;
	psWO->panSrcBands = (int *)CPLMalloc(iNewBandCount*sizeof(int));
	psWO->panDstBands = (int *)CPLMalloc(iNewBandCount*sizeof(int));
	for (int i = 0; i<iNewBandCount; i++)
	{
		psWO->panSrcBands[i] = pSrcBand[i];
		psWO->panDstBands[i] = i + 1;
	}

	psWO->hCutline = (void*)hMultiPoly;
	GDALTool_TransformCutlineToSource((GDALDatasetH)pSrcDS, (void*)hMultiPoly, &(psWO->papszWarpOptions), papszTO);


	GDALWarpOperation oWO;
	if (oWO.Initialize(psWO) != CE_None)
	{
		GDALClose((GDALDatasetH)pSrcDS);
		GDALClose((GDALDatasetH)pDstDS);

		return -1;
	}

	oWO.ChunkAndWarpImage(0, 0, iNewWidth, iNewHeigh);

	GDALDestroyGenImgProjTransformer(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	return 0;
}
//=========================================================================================================================================


/*
离散点网格化
功能：1.离散点构成规则格网，以影像形式显示
2.离散点构成三角网，以文件形式存储
*/
#define TO_RADIANS (3.14159265358979323846 / 180.0)
typedef struct _GDALGridJob GDALGridJob;
struct _GDALGridJob
{
	GUInt32             nYStart;

	GByte              *pabyData;
	GUInt32             nYStep;
	GUInt32             nXSize;
	GUInt32             nYSize;
	double              dfXMin;
	double              dfYMin;
	double              dfDeltaX;
	double              dfDeltaY;
	GUInt32             nPoints;
	const double       *padfX;
	const double       *padfY;
	const double       *padfZ;
	const void         *poOptions;
	GDALGridFunction    pfnGDALGridMethod;
	GDALGridExtraParameters* psExtraParameters;
	int(*pfnProgress)(GDALGridJob* psJob);
	GDALDataType        eType;

	void           *hThread;
	volatile int   *pnCounter;
	volatile int   *pbStop;
	void           *hCond;
	void           *hCondMutex;

	GDALProgressFunc pfnRealProgress;
	void *pRealProgressArg;
};
static void GDALGridJobProcess(void* user_data)
{
	GDALGridJob* psJob = (GDALGridJob*)user_data;
	GUInt32 nXPoint, nYPoint;

	const GUInt32 nYStart = psJob->nYStart;
	const GUInt32 nYStep = psJob->nYStep;
	GByte *pabyData = psJob->pabyData;

	const GUInt32 nXSize = psJob->nXSize;
	const GUInt32 nYSize = psJob->nYSize;
	const double dfXMin = psJob->dfXMin;
	const double dfYMin = psJob->dfYMin;
	const double dfDeltaX = psJob->dfDeltaX;
	const double dfDeltaY = psJob->dfDeltaY;
	GUInt32 nPoints = psJob->nPoints;
	const double* padfX = psJob->padfX;
	const double* padfY = psJob->padfY;
	const double* padfZ = psJob->padfZ;
	const void *poOptions = psJob->poOptions;
	GDALGridFunction  pfnGDALGridMethod = psJob->pfnGDALGridMethod;
	GDALGridExtraParameters *psExtraParameters = psJob->psExtraParameters;
	GDALDataType eType = psJob->eType;
	int(*pfnProgress)(GDALGridJob* psJob) = psJob->pfnProgress;

	int         nDataTypeSize = GDALGetDataTypeSize(eType) / 8;
	int         nLineSpace = nXSize * nDataTypeSize;

	/* -------------------------------------------------------------------- */
	/*  Allocate a buffer of scanline size, fill it with gridded values     */
	/*  and use GDALCopyWords() to copy values into output data array with  */
	/*  appropriate data type conversion.                                   */
	/* -------------------------------------------------------------------- */
	double      *padfValues = (double *)VSIMalloc2(sizeof(double), nXSize);
	if (padfValues == NULL)
	{
		*(psJob->pbStop) = TRUE;
		pfnProgress(psJob); /* to notify the main thread */
		return;
	}

	for (nYPoint = nYStart; nYPoint < nYSize; nYPoint += nYStep)
	{
		const double    dfYPoint = dfYMin + (nYPoint + 0.5) * dfDeltaY;

		for (nXPoint = 0; nXPoint < nXSize; nXPoint++)
		{
			const double    dfXPoint = dfXMin + (nXPoint + 0.5) * dfDeltaX;

			if ((*pfnGDALGridMethod)(poOptions, nPoints, padfX, padfY, padfZ,
				dfXPoint, dfYPoint,
				padfValues + nXPoint, psExtraParameters) != CE_None)
			{
				CPLError(CE_Failure, CPLE_AppDefined,
					"Gridding failed at X position %lu, Y position %lu",
					(long unsigned int)nXPoint,
					(long unsigned int)nYPoint);
				*(psJob->pbStop) = TRUE;
				pfnProgress(psJob); /* to notify the main thread */
				break;
			}
		}

		GDALCopyWords(padfValues, GDT_Float64, sizeof(double),
			pabyData + nYPoint * nLineSpace, eType, nDataTypeSize,
			nXSize);

		if (*(psJob->pbStop) || pfnProgress(psJob))
			break;
	}

	CPLFree(padfValues);
}
static void GDALGridGetPointBounds(const void* hFeature, CPLRectObj* pBounds)
{
	GDALGridPoint* psPoint = (GDALGridPoint*)hFeature;
	GDALGridXYArrays* psXYArrays = psPoint->psXYArrays;
	int i = psPoint->i;
	double dfX = psXYArrays->padfX[i];
	double dfY = psXYArrays->padfY[i];
	pBounds->minx = dfX;
	pBounds->miny = dfY;
	pBounds->maxx = dfX;
	pBounds->maxy = dfY;
};
static int GDALGridProgressMultiThread(GDALGridJob* psJob)
{
	CPLAcquireMutex(psJob->hCondMutex, 1.0);
	(*(psJob->pnCounter))++;
	CPLCondSignal(psJob->hCond);
	int bStop = *(psJob->pbStop);
	CPLReleaseMutex(psJob->hCondMutex);

	return bStop;
}
static int GDALGridProgressMonoThread(GDALGridJob* psJob)
{
	int nCounter = ++(*(psJob->pnCounter));
	if (!psJob->pfnRealProgress((nCounter / (double)psJob->nYSize),
		"", psJob->pRealProgressArg))
	{
		CPLError(CE_Failure, CPLE_UserInterrupt, "User terminated");
		*(psJob->pbStop) = TRUE;
		return TRUE;
	}
	return FALSE;
}

#pragma region 离散点数据转换为格网数据的差值方法
CPLErr
GDALGridInverseDistanceToAPower(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	const double *padfZ,
	double dfXPoint, double dfYPoint,
	double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double      dfRadius1 =
		((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfRadius1;
	double      dfRadius2 =
		((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double dfAngle = TO_RADIANS
		* ((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	const double    dfPowerDiv2 =
		((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfPower / 2;
	const double    dfSmoothing =
		((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfSmoothing;
	const GUInt32   nMaxPoints =
		((GDALGridInverseDistanceToAPowerOptions *)poOptions)->nMaxPoints;
	double  dfNominator = 0.0, dfDenominator = 0.0;
	GUInt32 i, n = 0;

	for (i = 0; i < nPoints; i++)
	{
		double  dfRX = padfX[i] - dfXPoint;
		double  dfRY = padfY[i] - dfYPoint;
		const double dfR2 =
			dfRX * dfRX + dfRY * dfRY + dfSmoothing * dfSmoothing;

		if (bRotated)
		{
			double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
			double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

			dfRX = dfRXRotated;
			dfRY = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
		{
			// If the test point is close to the grid node, use the point
			// value directly as a node value to avoid singularity.
			if (dfR2 < 0.0000000000001)
			{
				(*pdfValue) = padfZ[i];
				return CE_None;
			}
			else
			{
				const double dfW = pow(dfR2, dfPowerDiv2);
				double dfInvW = 1.0 / dfW;
				dfNominator += dfInvW * padfZ[i];
				dfDenominator += dfInvW;
				n++;
				if (nMaxPoints > 0 && n > nMaxPoints)
					break;
			}
		}
	}

	if (n < ((GDALGridInverseDistanceToAPowerOptions *)poOptions)->nMinPoints
		|| dfDenominator == 0.0)
	{
		(*pdfValue) =
			((GDALGridInverseDistanceToAPowerOptions*)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = dfNominator / dfDenominator;

	return CE_None;
}

CPLErr
GDALGridInverseDistanceToAPowerNoSearch(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	const double *padfZ,
	double dfXPoint, double dfYPoint,
	double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	const double    dfPowerDiv2 =
		((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfPower / 2;
	const double    dfSmoothing =
		((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfSmoothing;
	const double    dfSmoothing2 = dfSmoothing * dfSmoothing;
	double  dfNominator = 0.0, dfDenominator = 0.0;
	GUInt32 i = 0;
	int bPower2 = (dfPowerDiv2 == 1.0);

	if (bPower2)
	{
		if (dfSmoothing2 > 0)
		{
			for (i = 0; i < nPoints; i++)
			{
				const double dfRX = padfX[i] - dfXPoint;
				const double dfRY = padfY[i] - dfYPoint;
				const double dfR2 =
					dfRX * dfRX + dfRY * dfRY + dfSmoothing2;

				double dfInvR2 = 1.0 / dfR2;
				dfNominator += dfInvR2 * padfZ[i];
				dfDenominator += dfInvR2;
			}
		}
		else
		{
			for (i = 0; i < nPoints; i++)
			{
				const double dfRX = padfX[i] - dfXPoint;
				const double dfRY = padfY[i] - dfYPoint;
				const double dfR2 =
					dfRX * dfRX + dfRY * dfRY;

				// If the test point is close to the grid node, use the point
				// value directly as a node value to avoid singularity.
				if (dfR2 < 0.0000000000001)
				{
					break;
				}
				else
				{
					double dfInvR2 = 1.0 / dfR2;
					dfNominator += dfInvR2 * padfZ[i];
					dfDenominator += dfInvR2;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < nPoints; i++)
		{
			const double dfRX = padfX[i] - dfXPoint;
			const double dfRY = padfY[i] - dfYPoint;
			const double dfR2 =
				dfRX * dfRX + dfRY * dfRY + dfSmoothing2;

			// If the test point is close to the grid node, use the point
			// value directly as a node value to avoid singularity.
			if (dfR2 < 0.0000000000001)
			{
				break;
			}
			else
			{
				const double dfW = pow(dfR2, dfPowerDiv2);
				double dfInvW = 1.0 / dfW;
				dfNominator += dfInvW * padfZ[i];
				dfDenominator += dfInvW;
			}
		}
	}

	if (i != nPoints)
	{
		(*pdfValue) = padfZ[i];
	}
	else
		if (dfDenominator == 0.0)
		{
			(*pdfValue) =
				((GDALGridInverseDistanceToAPowerOptions*)poOptions)->dfNoDataValue;
		}
		else
			(*pdfValue) = dfNominator / dfDenominator;

	return CE_None;
}


#ifdef HAVE_SSE_AT_COMPILE_TIME

static CPLErr
GDALGridInverseDistanceToAPower2NoSmoothingNoSearchSSE(
	const void *poOptions,
	GUInt32 nPoints,
	CPL_UNUSED const double *unused_padfX,
	CPL_UNUSED const double *unused_padfY,
	CPL_UNUSED const double *unused_padfZ,
	double dfXPoint, double dfYPoint,
	double *pdfValue,
	void* hExtraParamsIn)
{
	size_t i = 0;
	GDALGridExtraParameters* psExtraParams = (GDALGridExtraParameters*)hExtraParamsIn;
	const float* pafX = psExtraParams->pafX;
	const float* pafY = psExtraParams->pafY;
	const float* pafZ = psExtraParams->pafZ;

	const float fEpsilon = 0.0000000000001f;
	const float fXPoint = (float)dfXPoint;
	const float fYPoint = (float)dfYPoint;
	const __m128 xmm_small = _mm_load1_ps((float*)&fEpsilon);
	const __m128 xmm_x = _mm_load1_ps((float*)&fXPoint);
	const __m128 xmm_y = _mm_load1_ps((float*)&fYPoint);
	__m128 xmm_nominator = _mm_setzero_ps();
	__m128 xmm_denominator = _mm_setzero_ps();
	int mask = 0;

#if defined(__x86_64) || defined(_M_X64)
	/* This would also work in 32bit mode, but there are only 8 XMM registers */
	/* whereas we have 16 for 64bit */
#define LOOP_SIZE   8
	size_t nPointsRound = (nPoints / LOOP_SIZE) * LOOP_SIZE;
	for (i = 0; i < nPointsRound; i += LOOP_SIZE)
	{
		__m128 xmm_rx = _mm_sub_ps(_mm_load_ps(pafX + i), xmm_x);            /* rx = pafX[i] - fXPoint */
		__m128 xmm_rx_4 = _mm_sub_ps(_mm_load_ps(pafX + i + 4), xmm_x);
		__m128 xmm_ry = _mm_sub_ps(_mm_load_ps(pafY + i), xmm_y);            /* ry = pafY[i] - fYPoint */
		__m128 xmm_ry_4 = _mm_sub_ps(_mm_load_ps(pafY + i + 4), xmm_y);
		__m128 xmm_r2 = _mm_add_ps(_mm_mul_ps(xmm_rx, xmm_rx),               /* r2 = rx * rx + ry * ry */
			_mm_mul_ps(xmm_ry, xmm_ry));
		__m128 xmm_r2_4 = _mm_add_ps(_mm_mul_ps(xmm_rx_4, xmm_rx_4),
			_mm_mul_ps(xmm_ry_4, xmm_ry_4));
		__m128 xmm_invr2 = _mm_rcp_ps(xmm_r2);                               /* invr2 = 1.0f / r2 */
		__m128 xmm_invr2_4 = _mm_rcp_ps(xmm_r2_4);
		xmm_nominator = _mm_add_ps(xmm_nominator,                            /* nominator += invr2 * pafZ[i] */
			_mm_mul_ps(xmm_invr2, _mm_load_ps(pafZ + i)));
		xmm_nominator = _mm_add_ps(xmm_nominator,
			_mm_mul_ps(xmm_invr2_4, _mm_load_ps(pafZ + i + 4)));
		xmm_denominator = _mm_add_ps(xmm_denominator, xmm_invr2);           /* denominator += invr2 */
		xmm_denominator = _mm_add_ps(xmm_denominator, xmm_invr2_4);
		mask = _mm_movemask_ps(_mm_cmplt_ps(xmm_r2, xmm_small)) |           /* if( r2 < fEpsilon) */
			(_mm_movemask_ps(_mm_cmplt_ps(xmm_r2_4, xmm_small)) << 4);
		if (mask)
			break;
	}
#else
#define LOOP_SIZE   4
	size_t nPointsRound = (nPoints / LOOP_SIZE) * LOOP_SIZE;
	for (i = 0; i < nPointsRound; i += LOOP_SIZE)
	{
		__m128 xmm_rx = _mm_sub_ps(_mm_load_ps((float*)pafX + i), xmm_x);           /* rx = pafX[i] - fXPoint */
		__m128 xmm_ry = _mm_sub_ps(_mm_load_ps((float*)pafY + i), xmm_y);           /* ry = pafY[i] - fYPoint */
		__m128 xmm_r2 = _mm_add_ps(_mm_mul_ps(xmm_rx, xmm_rx),              /* r2 = rx * rx + ry * ry */
			_mm_mul_ps(xmm_ry, xmm_ry));
		__m128 xmm_invr2 = _mm_rcp_ps(xmm_r2);                              /* invr2 = 1.0f / r2 */
		xmm_nominator = _mm_add_ps(xmm_nominator,                           /* nominator += invr2 * pafZ[i] */
			_mm_mul_ps(xmm_invr2, _mm_load_ps((float*)pafZ + i)));
		xmm_denominator = _mm_add_ps(xmm_denominator, xmm_invr2);           /* denominator += invr2 */
		mask = _mm_movemask_ps(_mm_cmplt_ps(xmm_r2, xmm_small));            /* if( r2 < fEpsilon) */
		if (mask)
			break;
	}
#endif

	/* Find which i triggered r2 < fEpsilon */
	if (mask)
	{
		for (int j = 0; j < LOOP_SIZE; j++)
		{
			if (mask & (1 << j))
			{
				(*pdfValue) = (pafZ)[i + j];
				return CE_None;
			}
		}
	}

	/* Get back nominator and denominator values for XMM registers */
	float afNominator[4], afDenominator[4];
	_mm_storeu_ps(afNominator, xmm_nominator);
	_mm_storeu_ps(afDenominator, xmm_denominator);

	float fNominator = afNominator[0] + afNominator[1] +
		afNominator[2] + afNominator[3];
	float fDenominator = afDenominator[0] + afDenominator[1] +
		afDenominator[2] + afDenominator[3];

	/* Do the few remaining loop iterations */
	for (; i < nPoints; i++)
	{
		const float fRX = pafX[i] - fXPoint;
		const float fRY = pafY[i] - fYPoint;
		const float fR2 =
			fRX * fRX + fRY * fRY;

		// If the test point is close to the grid node, use the point
		// value directly as a node value to avoid singularity.
		if (fR2 < 0.0000000000001)
		{
			break;
		}
		else
		{
			const float fInvR2 = 1.0f / fR2;
			fNominator += fInvR2 * pafZ[i];
			fDenominator += fInvR2;
		}
	}

	if (i != nPoints)
	{
		(*pdfValue) = pafZ[i];
	}
	else
		if (fDenominator == 0.0)
		{
			(*pdfValue) =
				((GDALGridInverseDistanceToAPowerOptions*)poOptions)->dfNoDataValue;
		}
		else
			(*pdfValue) = fNominator / fDenominator;

	return CE_None;
}
#endif // HAVE_SSE_AT_COMPILE_TIME

CPLErr
GDALGridMovingAverage(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	const double *padfZ,
	double dfXPoint, double dfYPoint, double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 = ((GDALGridMovingAverageOptions *)poOptions)->dfRadius1;
	double  dfRadius2 = ((GDALGridMovingAverageOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double    dfAngle =
		TO_RADIANS * ((GDALGridMovingAverageOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	double  dfAccumulator = 0.0;
	GUInt32 i = 0, n = 0;

	while (i < nPoints)
	{
		double  dfRX = padfX[i] - dfXPoint;
		double  dfRY = padfY[i] - dfYPoint;

		if (bRotated)
		{
			double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
			double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

			dfRX = dfRXRotated;
			dfRY = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
		{
			dfAccumulator += padfZ[i];
			n++;
		}

		i++;
	}

	if (n < ((GDALGridMovingAverageOptions *)poOptions)->nMinPoints
		|| n == 0)
	{
		(*pdfValue) =
			((GDALGridMovingAverageOptions *)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = dfAccumulator / n;

	return CE_None;
}

CPLErr
GDALGridNearestNeighbor(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	const double *padfZ,
	double dfXPoint, double dfYPoint, double *pdfValue,
	void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 =
		((GDALGridNearestNeighborOptions *)poOptions)->dfRadius1;
	double  dfRadius2 =
		((GDALGridNearestNeighborOptions *)poOptions)->dfRadius2;
	double  dfR12;
	GDALGridExtraParameters* psExtraParams = (GDALGridExtraParameters*)hExtraParamsIn;
	CPLQuadTree* hQuadTree = psExtraParams->hQuadTree;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double    dfAngle =
		TO_RADIANS * ((GDALGridNearestNeighborOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	// If the nearest point will not be found, its value remains as NODATA.
	double      dfNearestValue =
		((GDALGridNearestNeighborOptions *)poOptions)->dfNoDataValue;
	// Nearest distance will be initialized with the distance to the first
	// point in array.
	double      dfNearestR = DBL_MAX;
	GUInt32 i = 0;

	double dfSearchRadius = psExtraParams->dfInitialSearchRadius;
	if (hQuadTree != NULL && dfRadius1 == dfRadius2 && dfSearchRadius > 0)
	{
		CPLRectObj sAoi;
		if (dfRadius1 > 0)
			dfSearchRadius = ((GDALGridNearestNeighborOptions *)poOptions)->dfRadius1;
		while (dfSearchRadius > 0)
		{
			sAoi.minx = dfXPoint - dfSearchRadius;
			sAoi.miny = dfYPoint - dfSearchRadius;
			sAoi.maxx = dfXPoint + dfSearchRadius;
			sAoi.maxy = dfYPoint + dfSearchRadius;
			int nFeatureCount = 0;
			GDALGridPoint** papsPoints = (GDALGridPoint**)
				CPLQuadTreeSearch(hQuadTree, &sAoi, &nFeatureCount);
			if (nFeatureCount != 0)
			{
				if (dfRadius1 > 0)
					dfNearestR = dfRadius1;
				for (int k = 0; k<nFeatureCount; k++)
				{
					int i = papsPoints[k]->i;
					double  dfRX = padfX[i] - dfXPoint;
					double  dfRY = padfY[i] - dfYPoint;

					const double    dfR2 = dfRX * dfRX + dfRY * dfRY;
					if (dfR2 <= dfNearestR)
					{
						dfNearestR = dfR2;
						dfNearestValue = padfZ[i];
					}
				}

				CPLFree(papsPoints);
				break;
			}
			else
			{
				CPLFree(papsPoints);
				if (dfRadius1 > 0)
					break;
				dfSearchRadius *= 2;
				//CPLDebug("GDAL_GRID", "Increasing search radius to %.16g", dfSearchRadius);
			}
		}
	}
	else
	{
		while (i < nPoints)
		{
			double  dfRX = padfX[i] - dfXPoint;
			double  dfRY = padfY[i] - dfYPoint;

			if (bRotated)
			{
				double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
				double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

				dfRX = dfRXRotated;
				dfRY = dfRYRotated;
			}

			// Is this point located inside the search ellipse?
			if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
			{
				const double    dfR2 = dfRX * dfRX + dfRY * dfRY;
				if (dfR2 <= dfNearestR)
				{
					dfNearestR = dfR2;
					dfNearestValue = padfZ[i];
				}
			}

			i++;
		}
	}

	(*pdfValue) = dfNearestValue;

	return CE_None;
}

CPLErr
GDALGridDataMetricMinimum(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	const double *padfZ,
	double dfXPoint, double dfYPoint, double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius1;
	double  dfRadius2 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double dfAngle =
		TO_RADIANS * ((GDALGridDataMetricsOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	double      dfMinimumValue = 0.0;
	GUInt32     i = 0, n = 0;

	while (i < nPoints)
	{
		double  dfRX = padfX[i] - dfXPoint;
		double  dfRY = padfY[i] - dfYPoint;

		if (bRotated)
		{
			double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
			double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

			dfRX = dfRXRotated;
			dfRY = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
		{
			if (n)
			{
				if (dfMinimumValue > padfZ[i])
					dfMinimumValue = padfZ[i];
			}
			else
				dfMinimumValue = padfZ[i];
			n++;
		}

		i++;
	}

	if (n < ((GDALGridDataMetricsOptions *)poOptions)->nMinPoints
		|| n == 0)
	{
		(*pdfValue) =
			((GDALGridDataMetricsOptions *)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = dfMinimumValue;

	return CE_None;
}

CPLErr
GDALGridDataMetricMaximum(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	const double *padfZ,
	double dfXPoint, double dfYPoint, double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius1;
	double  dfRadius2 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double    dfAngle =
		TO_RADIANS * ((GDALGridDataMetricsOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	double      dfMaximumValue = 0.0;
	GUInt32     i = 0, n = 0;

	while (i < nPoints)
	{
		double  dfRX = padfX[i] - dfXPoint;
		double  dfRY = padfY[i] - dfYPoint;

		if (bRotated)
		{
			double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
			double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

			dfRX = dfRXRotated;
			dfRY = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
		{
			if (n)
			{
				if (dfMaximumValue < padfZ[i])
					dfMaximumValue = padfZ[i];
			}
			else
				dfMaximumValue = padfZ[i];
			n++;
		}

		i++;
	}

	if (n < ((GDALGridDataMetricsOptions *)poOptions)->nMinPoints
		|| n == 0)
	{
		(*pdfValue) =
			((GDALGridDataMetricsOptions *)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = dfMaximumValue;

	return CE_None;
}

CPLErr
GDALGridDataMetricRange(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	const double *padfZ,
	double dfXPoint, double dfYPoint, double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius1;
	double  dfRadius2 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double    dfAngle =
		TO_RADIANS * ((GDALGridDataMetricsOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	double      dfMaximumValue = 0.0, dfMinimumValue = 0.0;
	GUInt32     i = 0, n = 0;

	while (i < nPoints)
	{
		double  dfRX = padfX[i] - dfXPoint;
		double  dfRY = padfY[i] - dfYPoint;

		if (bRotated)
		{
			double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
			double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

			dfRX = dfRXRotated;
			dfRY = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
		{
			if (n)
			{
				if (dfMinimumValue > padfZ[i])
					dfMinimumValue = padfZ[i];
				if (dfMaximumValue < padfZ[i])
					dfMaximumValue = padfZ[i];
			}
			else
				dfMinimumValue = dfMaximumValue = padfZ[i];
			n++;
		}

		i++;
	}

	if (n < ((GDALGridDataMetricsOptions *)poOptions)->nMinPoints
		|| n == 0)
	{
		(*pdfValue) =
			((GDALGridDataMetricsOptions *)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = dfMaximumValue - dfMinimumValue;

	return CE_None;
}

CPLErr
GDALGridDataMetricCount(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	CPL_UNUSED const double *padfZ,
	double dfXPoint, double dfYPoint, double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius1;
	double  dfRadius2 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double    dfAngle =
		TO_RADIANS * ((GDALGridDataMetricsOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	GUInt32     i = 0, n = 0;

	while (i < nPoints)
	{
		double  dfRX = padfX[i] - dfXPoint;
		double  dfRY = padfY[i] - dfYPoint;

		if (bRotated)
		{
			double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
			double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

			dfRX = dfRXRotated;
			dfRY = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
			n++;

		i++;
	}

	if (n < ((GDALGridDataMetricsOptions *)poOptions)->nMinPoints)
	{
		(*pdfValue) =
			((GDALGridDataMetricsOptions *)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = (double)n;

	return CE_None;
}

CPLErr
GDALGridDataMetricAverageDistance(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	CPL_UNUSED const double *padfZ,
	double dfXPoint, double dfYPoint,
	double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius1;
	double  dfRadius2 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double    dfAngle =
		TO_RADIANS * ((GDALGridDataMetricsOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	double      dfAccumulator = 0.0;
	GUInt32     i = 0, n = 0;

	while (i < nPoints)
	{
		double  dfRX = padfX[i] - dfXPoint;
		double  dfRY = padfY[i] - dfYPoint;

		if (bRotated)
		{
			double dfRXRotated = dfRX * dfCoeff1 + dfRY * dfCoeff2;
			double dfRYRotated = dfRY * dfCoeff1 - dfRX * dfCoeff2;

			dfRX = dfRXRotated;
			dfRY = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX * dfRX + dfRadius1 * dfRY * dfRY <= dfR12)
		{
			dfAccumulator += sqrt(dfRX * dfRX + dfRY * dfRY);
			n++;
		}

		i++;
	}

	if (n < ((GDALGridDataMetricsOptions *)poOptions)->nMinPoints
		|| n == 0)
	{
		(*pdfValue) =
			((GDALGridDataMetricsOptions *)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = dfAccumulator / n;

	return CE_None;
}
CPLErr
GDALGridDataMetricAverageDistancePts(const void *poOptions, GUInt32 nPoints,
	const double *padfX, const double *padfY,
	CPL_UNUSED const double *padfZ,
	double dfXPoint, double dfYPoint,
	double *pdfValue,
	CPL_UNUSED void* hExtraParamsIn)
{
	// TODO: For optimization purposes pre-computed parameters should be moved
	// out of this routine to the calling function.

	// Pre-compute search ellipse parameters
	double  dfRadius1 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius1;
	double  dfRadius2 =
		((GDALGridDataMetricsOptions *)poOptions)->dfRadius2;
	double  dfR12;

	dfRadius1 *= dfRadius1;
	dfRadius2 *= dfRadius2;
	dfR12 = dfRadius1 * dfRadius2;

	// Compute coefficients for coordinate system rotation.
	double      dfCoeff1 = 0.0, dfCoeff2 = 0.0;
	const double    dfAngle =
		TO_RADIANS * ((GDALGridDataMetricsOptions *)poOptions)->dfAngle;
	const bool  bRotated = (dfAngle == 0.0) ? false : true;
	if (bRotated)
	{
		dfCoeff1 = cos(dfAngle);
		dfCoeff2 = sin(dfAngle);
	}

	double      dfAccumulator = 0.0;
	GUInt32     i = 0, n = 0;

	// Search for the first point within the search ellipse
	while (i < nPoints - 1)
	{
		double  dfRX1 = padfX[i] - dfXPoint;
		double  dfRY1 = padfY[i] - dfYPoint;

		if (bRotated)
		{
			double dfRXRotated = dfRX1 * dfCoeff1 + dfRY1 * dfCoeff2;
			double dfRYRotated = dfRY1 * dfCoeff1 - dfRX1 * dfCoeff2;

			dfRX1 = dfRXRotated;
			dfRY1 = dfRYRotated;
		}

		// Is this point located inside the search ellipse?
		if (dfRadius2 * dfRX1 * dfRX1 + dfRadius1 * dfRY1 * dfRY1 <= dfR12)
		{
			GUInt32 j;

			// Search all the remaining points within the ellipse and compute
			// distances between them and the first point
			for (j = i + 1; j < nPoints; j++)
			{
				double  dfRX2 = padfX[j] - dfXPoint;
				double  dfRY2 = padfY[j] - dfYPoint;

				if (bRotated)
				{
					double dfRXRotated = dfRX2 * dfCoeff1 + dfRY2 * dfCoeff2;
					double dfRYRotated = dfRY2 * dfCoeff1 - dfRX2 * dfCoeff2;

					dfRX2 = dfRXRotated;
					dfRY2 = dfRYRotated;
				}

				if (dfRadius2 * dfRX2 * dfRX2 + dfRadius1 * dfRY2 * dfRY2 <= dfR12)
				{
					const double dfRX = padfX[j] - padfX[i];
					const double dfRY = padfY[j] - padfY[i];

					dfAccumulator += sqrt(dfRX * dfRX + dfRY * dfRY);
					n++;
				}
			}
		}

		i++;
	}

	if (n < ((GDALGridDataMetricsOptions *)poOptions)->nMinPoints
		|| n == 0)
	{
		(*pdfValue) =
			((GDALGridDataMetricsOptions *)poOptions)->dfNoDataValue;
	}
	else
		(*pdfValue) = dfAccumulator / n;

	return CE_None;
}

#pragma endregion
long GridTrans::GDALGridCreate(GDALGridAlgorithm eAlgorithm, const void *poOptions, GUInt32 nPoints, const double *padfX, const double *padfY, const double *padfZ, double dfXMin, double dfXMax, double dfYMin, double dfYMax, GUInt32 nXSize, GUInt32 nYSize, GDALDataType eType, void *pData)
{
	CPLAssert(poOptions);
	CPLAssert(padfX);
	CPLAssert(padfY);
	CPLAssert(padfZ);
	CPLAssert(pData);

	if (nXSize == 0 || nYSize == 0)
	{
		CPLError(CE_Failure, CPLE_IllegalArg,
			"Output raster dimesions should have non-zero size.");
		return CE_Failure;
	}

	GDALGridFunction    pfnGDALGridMethod;
	int bCreateQuadTree = FALSE;

	/* Potentially unaligned pointers */
	void* pabyX = NULL;
	void* pabyY = NULL;
	void* pabyZ = NULL;

	/* Starting address aligned on 16-byte boundary */
	float* pafXAligned = NULL;
	float* pafYAligned = NULL;
	float* pafZAligned = NULL;

	switch (eAlgorithm)
	{
	case GGA_InverseDistanceToAPower:
		if (((GDALGridInverseDistanceToAPowerOptions *)poOptions)->
			dfRadius1 == 0.0 &&
			((GDALGridInverseDistanceToAPowerOptions *)poOptions)->
			dfRadius2 == 0.0)
		{
			const double    dfPower =
				((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfPower;
			const double    dfSmoothing =
				((GDALGridInverseDistanceToAPowerOptions *)poOptions)->dfSmoothing;

			pfnGDALGridMethod = GDALGridInverseDistanceToAPowerNoSearch;
			if (dfPower == 2.0 && dfSmoothing == 0.0)
			{
#ifdef HAVE_AVX_AT_COMPILE_TIME

#define ALIGN32(x)  (((char*)(x)) + ((32 - (((size_t)(x)) % 32)) % 32))

				if (CSLTestBoolean(CPLGetConfigOption("GDAL_USE_AVX", "YES")) &&
					CPLHaveRuntimeAVX())
				{
					pabyX = (float*)VSIMalloc(sizeof(float) * nPoints + 31);
					pabyY = (float*)VSIMalloc(sizeof(float) * nPoints + 31);
					pabyZ = (float*)VSIMalloc(sizeof(float) * nPoints + 31);
					if (pabyX != NULL && pabyY != NULL && pabyZ != NULL)
					{
						CPLDebug("GDAL_GRID", "Using AVX optimized version");
						pafXAligned = (float*)ALIGN32(pabyX);
						pafYAligned = (float*)ALIGN32(pabyY);
						pafZAligned = (float*)ALIGN32(pabyZ);
						pfnGDALGridMethod = GDALGridInverseDistanceToAPower2NoSmoothingNoSearchAVX;
						GUInt32 i;
						for (i = 0; i<nPoints; i++)
						{
							pafXAligned[i] = (float)padfX[i];
							pafYAligned[i] = (float)padfY[i];
							pafZAligned[i] = (float)padfZ[i];
						}
					}
					else
					{
						VSIFree(pabyX);
						VSIFree(pabyY);
						VSIFree(pabyZ);
						pabyX = pabyY = pabyZ = NULL;
					}
				}
#endif

#ifdef HAVE_SSE_AT_COMPILE_TIME

#define ALIGN16(x)  (((char*)(x)) + ((16 - (((size_t)(x)) % 16)) % 16))

				if (pafXAligned == NULL &&
					CSLTestBoolean(CPLGetConfigOption("GDAL_USE_SSE", "YES")) &&
					CPLHaveRuntimeSSE())
				{
					pabyX = (float*)VSIMalloc(sizeof(float) * nPoints + 15);
					pabyY = (float*)VSIMalloc(sizeof(float) * nPoints + 15);
					pabyZ = (float*)VSIMalloc(sizeof(float) * nPoints + 15);
					if (pabyX != NULL && pabyY != NULL && pabyZ != NULL)
					{
						CPLDebug("GDAL_GRID", "Using SSE optimized version");
						pafXAligned = (float*)ALIGN16(pabyX);
						pafYAligned = (float*)ALIGN16(pabyY);
						pafZAligned = (float*)ALIGN16(pabyZ);
						pfnGDALGridMethod = GDALGridInverseDistanceToAPower2NoSmoothingNoSearchSSE;
						GUInt32 i;
						for (i = 0; i<nPoints; i++)
						{
							pafXAligned[i] = (float)padfX[i];
							pafYAligned[i] = (float)padfY[i];
							pafZAligned[i] = (float)padfZ[i];
						}
					}
					else
					{
						VSIFree(pabyX);
						VSIFree(pabyY);
						VSIFree(pabyZ);
						pabyX = pabyY = pabyZ = NULL;
					}
				}
#endif // HAVE_SSE_AT_COMPILE_TIME
			}
		}
		else
			pfnGDALGridMethod = GDALGridInverseDistanceToAPower;
		break;

	case GGA_MovingAverage:
		pfnGDALGridMethod = GDALGridMovingAverage;
		break;

	case GGA_NearestNeighbor:
		pfnGDALGridMethod = GDALGridNearestNeighbor;
		bCreateQuadTree = (nPoints > 100 &&
			(((GDALGridNearestNeighborOptions *)poOptions)->dfRadius1 ==
				((GDALGridNearestNeighborOptions *)poOptions)->dfRadius2));
		break;

	case GGA_MetricMinimum:
		pfnGDALGridMethod = GDALGridDataMetricMinimum;
		break;

	case GGA_MetricMaximum:
		pfnGDALGridMethod = GDALGridDataMetricMaximum;
		break;

	case GGA_MetricRange:
		pfnGDALGridMethod = GDALGridDataMetricRange;
		break;

	case GGA_MetricCount:
		pfnGDALGridMethod = GDALGridDataMetricCount;
		break;

	case GGA_MetricAverageDistance:
		pfnGDALGridMethod = GDALGridDataMetricAverageDistance;
		break;

	case GGA_MetricAverageDistancePts:
		pfnGDALGridMethod = GDALGridDataMetricAverageDistancePts;
		break;

	default:
		CPLError(CE_Failure, CPLE_IllegalArg,
			"GDAL does not support gridding method %d", eAlgorithm);
		return CE_Failure;
	}

	const double    dfDeltaX = (dfXMax - dfXMin) / nXSize;
	const double    dfDeltaY = (dfYMax - dfYMin) / nYSize;

	/* -------------------------------------------------------------------- */
	/*  Create quadtree if requested and possible.                          */
	/* -------------------------------------------------------------------- */
	CPLQuadTree* hQuadTree = NULL;
	double       dfInitialSearchRadius = 0;
	GDALGridPoint* pasGridPoints = NULL;
	GDALGridXYArrays sXYArrays;
	sXYArrays.padfX = padfX;
	sXYArrays.padfY = padfY;

	if (bCreateQuadTree)
	{
		pasGridPoints = (GDALGridPoint*)VSIMalloc2(nPoints, sizeof(GDALGridPoint));
		if (pasGridPoints != NULL)
		{
			CPLRectObj sRect;
			GUInt32 i;

			/* Determine point extents */
			sRect.minx = padfX[0];
			sRect.miny = padfY[0];
			sRect.maxx = padfX[0];
			sRect.maxy = padfY[0];
			for (i = 1; i < nPoints; i++)
			{
				if (padfX[i] < sRect.minx) sRect.minx = padfX[i];
				if (padfY[i] < sRect.miny) sRect.miny = padfY[i];
				if (padfX[i] > sRect.maxx) sRect.maxx = padfX[i];
				if (padfY[i] > sRect.maxy) sRect.maxy = padfY[i];
			}

			/* Initial value for search radius is the typical dimension of a */
			/* "pixel" of the point array (assuming rather uniform distribution) */
			dfInitialSearchRadius = sqrt((sRect.maxx - sRect.minx) *
				(sRect.maxy - sRect.miny) / nPoints);

			hQuadTree = CPLQuadTreeCreate(&sRect, GDALGridGetPointBounds);

			for (i = 0; i < nPoints; i++)
			{
				pasGridPoints[i].psXYArrays = &sXYArrays;
				pasGridPoints[i].i = i;
				CPLQuadTreeInsert(hQuadTree, pasGridPoints + i);
			}
		}
	}


	GDALGridExtraParameters sExtraParameters;

	sExtraParameters.hQuadTree = hQuadTree;
	sExtraParameters.dfInitialSearchRadius = dfInitialSearchRadius;
	sExtraParameters.pafX = pafXAligned;
	sExtraParameters.pafY = pafYAligned;
	sExtraParameters.pafZ = pafZAligned;

	const char* pszThreads = CPLGetConfigOption("GDAL_NUM_THREADS", "ALL_CPUS");
	int nThreads;
	if (EQUAL(pszThreads, "ALL_CPUS"))
		nThreads = CPLGetNumCPUs();
	else
		nThreads = atoi(pszThreads);
	if (nThreads > 128)
		nThreads = 128;
	if (nThreads >= (int)nYSize / 2)
		nThreads = (int)nYSize / 2;

	volatile int nCounter = 0;
	volatile int bStop = FALSE;

	GDALGridJob sJob;
	sJob.nYStart = 0;
	sJob.pabyData = (GByte*)pData;
	sJob.nYStep = 1;
	sJob.nXSize = nXSize;
	sJob.nYSize = nYSize;
	sJob.dfXMin = dfXMin;
	sJob.dfYMin = dfYMin;
	sJob.dfDeltaX = dfDeltaX;
	sJob.dfDeltaY = dfDeltaY;
	sJob.nPoints = nPoints;
	sJob.padfX = padfX;
	sJob.padfY = padfY;
	sJob.padfZ = padfZ;
	sJob.poOptions = poOptions;
	sJob.pfnGDALGridMethod = pfnGDALGridMethod;
	sJob.psExtraParameters = &sExtraParameters;
	sJob.pfnProgress = NULL;
	sJob.eType = eType;
	sJob.pnCounter = &nCounter;
	sJob.pbStop = &bStop;
	sJob.hCond = NULL;
	sJob.hCondMutex = NULL;
	sJob.hThread = NULL;

	if (nThreads > 1)
	{
		sJob.hCond = CPLCreateCond();
		if (sJob.hCond == NULL)
		{
			CPLError(CE_Warning, CPLE_AppDefined,
				"Cannot create condition. Reverting to monothread processing");
			nThreads = 1;
		}
	}

	if (nThreads <= 1)
	{
		sJob.pfnProgress = GDALGridProgressMonoThread;

		GDALGridJobProcess(&sJob);
	}
	else
	{
		GDALGridJob* pasJobs = (GDALGridJob*)CPLMalloc(sizeof(GDALGridJob) * nThreads);
		int i;

		CPLDebug("GDAL_GRID", "Using %d threads", nThreads);

		sJob.nYStep = nThreads;
		sJob.hCondMutex = CPLCreateMutex(); /* and take implicitely the mutex */
		sJob.pfnProgress = GDALGridProgressMultiThread;

		/* -------------------------------------------------------------------- */
		/*      Start threads.                                                  */
		/* -------------------------------------------------------------------- */
		for (i = 0; i < nThreads && !bStop; i++)
		{
			memcpy(&pasJobs[i], &sJob, sizeof(GDALGridJob));
			pasJobs[i].nYStart = i;
			pasJobs[i].hThread = CPLCreateJoinableThread(GDALGridJobProcess,
				(void*)&pasJobs[i]);
		}

		/* -------------------------------------------------------------------- */
		/*      Report progress.                                                */
		/* -------------------------------------------------------------------- */
		while (nCounter < (int)nYSize && !bStop)
		{
			CPLCondWait(sJob.hCond, sJob.hCondMutex);

			int nLocalCounter = nCounter;
			CPLReleaseMutex(sJob.hCondMutex);

			// 			if( !pfnProgress( nLocalCounter / (double) nYSize, "", pProgressArg ) )
			// 			{
			// 				CPLError( CE_Failure, CPLE_UserInterrupt, "User terminated" );
			// 				bStop = TRUE;
			// 			}

			CPLAcquireMutex(sJob.hCondMutex, 1.0);
		}

		/* Release mutex before joining threads, otherwise they will dead-lock */
		/* forever in GDALGridProgressMultiThread() */
		CPLReleaseMutex(sJob.hCondMutex);

		/* -------------------------------------------------------------------- */
		/*      Wait for all threads to complete and finish.                    */
		/* -------------------------------------------------------------------- */
		for (i = 0; i<nThreads; i++)
		{
			if (pasJobs[i].hThread)
				CPLJoinThread(pasJobs[i].hThread);
		}

		CPLFree(pasJobs);
		CPLDestroyCond(sJob.hCond);
		CPLDestroyMutex(sJob.hCondMutex);
	}

	CPLFree(pasGridPoints);
	if (hQuadTree != NULL)
		CPLQuadTreeDestroy(hQuadTree);

	CPLFree(pabyX);
	CPLFree(pabyY);
	CPLFree(pabyZ);
	return 0;
}
long GridTrans::GDALTool_Grid(GDALGridAlgorithm gridAlg, const void *poOptions, double* points, int pointNumber, double pixelResolution, const char* pathDst)
{
	if (points == NULL || pathDst == NULL)
		return -1;
	//获取最大最小点的坐标
	double dminx = points[0], dminy = points[1], dmaxx = points[0], dmaxy = points[1];
	double *pdfX = new double[pointNumber];
	double *pdfY = new double[pointNumber];
	double *pdfZ = new double[pointNumber];
	for (int i = 0; i<pointNumber; i++)
	{
		dminx = min(dminx, points[3 * i + 0]);
		dmaxx = max(dmaxx, points[3 * i + 0]);
		dminy = min(dminy, points[3 * i + 1]);
		dmaxy = max(dmaxy, points[3 * i + 1]);

		pdfX[i] = points[3 * i + 0];
		pdfY[i] = points[3 * i + 1];
		pdfZ[i] = points[3 * i + 2];
	}
	int xsize = abs(dmaxx - dminx) / pixelResolution;
	int ysize = abs(dmaxy - dminy) / pixelResolution;
	GDALAllRegister();
	unsigned char *pData = new unsigned char[xsize*ysize];
	// 	GDALGridContext *gridContext=GDALGridContextCreate(gridAlg,poOptions,pointNumber,pdfX, pdfY, pdfZ,FALSE);
	// 	GDALGridContextProcess(gridContext,dminx, dmaxx, dminy, dmaxy, xsize, ysize, GDT_Float32, pData,NULL,NULL);
	// 	GDALGridContextFree(gridContext);

	GDALGridCreate(gridAlg, poOptions, pointNumber, pdfX, pdfY, pdfZ, dminx, dmaxx, dminy, dmaxy, xsize, ysize, GDT_Byte, pData);
	//创建新的数据集
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	char **papszOptions = NULL;
	papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BAND");
	GDALDataset *poDataset = pDriver->Create(pathDst, xsize, ysize, 1, GDT_Byte, papszOptions);
	double adfGeoTransform[6] = { dminx, pixelResolution, 0 , dmaxy, 0, -pixelResolution };
	poDataset->SetGeoTransform(adfGeoTransform);

	//写入影像
	poDataset->RasterIO(GF_Write, 0, 0, xsize, ysize, pData, xsize, ysize, GDT_Byte, 1, 0, 0, 0, 0);

	GDALClose(poDataset);
	delete[]pData;
	delete[]pdfX;
	delete[]pdfY;
	delete[]pdfZ;
	return 0;
}
long GridTrans::GDALLatLongToSHP(const char* pathLatLong, const char* pathShp)
{
	ifstream ifs(pathLatLong, ios_base::in);
	vector<DPOINT> m_latlongPnts;
	char tmpchar[2048];
	if (!ifs.is_open())
		return -1;
	ifs.getline(tmpchar, 2048);
	do
	{
		ifs.getline(tmpchar, 2048);
		int tmpID; double tmpx, tmpy;
		sscanf(tmpchar, "%d%lf%lf", &tmpID, &tmpx, &tmpy);
		DPOINT pnt; pnt.dX = tmpx; pnt.dY = tmpy;
		m_latlongPnts.push_back(pnt);
	} while (!ifs.eof());
	ifs.close();

	int utmZone;
	vector<DPOINT> m_utm_coordinate;
	for (int i = 0; i<m_latlongPnts.size(); i++)
	{
		DPOINT pnt;
		tsmLatLongToUTM(m_latlongPnts[i].dY, m_latlongPnts[i].dX, &utmZone, &pnt.dX, &pnt.dY);
		m_utm_coordinate.push_back(pnt);
	}
	//创建矢量文件 获取坐标
	OGRSpatialReference poSpatialRef;
	poSpatialRef.SetWellKnownGeogCS("WGS84");
	poSpatialRef.SetUTM(utmZone, TRUE);

	GDALAllRegister();
	const char* pazDriverName = "ESRI Shapefile";
	GDALDriver*		poDriver;
	GDALDataset*	poDS;
	poDriver = GetGDALDriverManager()->GetDriverByName(pazDriverName);
	if (poDriver == NULL)
		return -1;

	poDS = poDriver->Create(pathShp, 0, 0, 0, GDT_Unknown, NULL);
	if (poDS == NULL)
		return -1;

	OGRLayer* poLayer = poDS->CreateLayer("Point", &poSpatialRef, wkbPoint, NULL);
	if (poLayer == NULL)
		return -1;
	OGRFieldDefn oField("Pnt", OFTString);
	poLayer->CreateField(&oField);
	char szName[33] = "temp";
	for (int i = 0; i<m_utm_coordinate.size(); i++)
	{
		OGRFeature *poFeature;
		poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		poFeature->SetField("Pnt", szName);
		OGRPoint pt;
		pt.setX(m_utm_coordinate[i].dX);
		pt.setY(m_utm_coordinate[i].dY);
		poFeature->SetGeometry(&pt);
		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
			return -1;
		OGRFeature::DestroyFeature(poFeature);
	}
	GDALClose(poDS);

	return 0;
}
long GridTrans::GDALTool_TriangleDelaunay(double* points, int pointNumber, const char* pathDst)
{
	if (points == NULL || pathDst == NULL)
		return -1;

	double *padfX = new double[pointNumber];
	double *padfY = new double[pointNumber];
	for (int i = 0; i<pointNumber; i++)
	{
		padfX[i] = points[2 * i + 0];
		padfY[i] = points[2 * i + 1];
	}

	//构建三角网
	if (GDALHasTriangulation() != TRUE)
		return -1;
	GDALTriangulation *pdfTriangle = GDALTriangulationCreateDelaunay(pointNumber, padfX, padfY);
	GDALTriangulationComputeBarycentricCoefficients(pdfTriangle, padfX, padfY);
	//保存到文件中将三角网
	ofstream ofs(pathDst, ios_base::out);
	if (!ofs.is_open())
		return -1;

	ofs << "POINTS\n";
	ofs << "point numbers of Tin:\n" << pointNumber << endl;
	for (int i = 0; i<pointNumber; i++)
		ofs << i + 1 << setw(15) << padfX[i] << setw(15) << padfY[i] << endl;
	ofs << "TRIANGLES\n";
	ofs << "triangle number:\n" << pdfTriangle->nFacets << endl;
	for (int i = 0; i<pdfTriangle->nFacets; i++)
		ofs << pdfTriangle->pasFacets[i].anVertexIdx[0] << setw(5) << pdfTriangle->pasFacets[i].anVertexIdx[1] << setw(5) << pdfTriangle->pasFacets[i].anVertexIdx[2] << setw(5) <<
		pdfTriangle->pasFacets[i].anNeighborIdx[0] << setw(5) << pdfTriangle->pasFacets[i].anNeighborIdx[1] << setw(5) << pdfTriangle->pasFacets[i].anNeighborIdx[2] << setw(15) <<
		pdfTriangle->pasFacetCoefficients[i].dfCstX << setw(15) << pdfTriangle->pasFacetCoefficients[i].dfCstY << setw(15) <<
		pdfTriangle->pasFacetCoefficients[i].dfMul1X << setw(15) << pdfTriangle->pasFacetCoefficients[i].dfMul1Y << setw(15) <<
		pdfTriangle->pasFacetCoefficients[i].dfMul2X << setw(15) << pdfTriangle->pasFacetCoefficients[i].dfMul2Y << endl;
	ofs << "END TIN";
	ofs.close();

	GDALTriangulationFree(pdfTriangle);
	delete[]padfX;
	delete[]padfY;
	return 0;
}
GDALTriangulation* GridTrans::GDALTool_LoadTriangle(const char* pathTri, double* pdfTriVex, int &number)
{
	GDALTriangulation* m_triangle = NULL;
	GDALTriFacet *m_facets = NULL;
	GDALTriBarycentricCoefficients* m_barycentricCoefficients = NULL;

	int triNumber, nfaces;
	char cnstTmpchar[2048];
	ifstream ifs(pathTri, ios_base::in);
	ifs.getline(cnstTmpchar, 2048);
	ifs.getline(cnstTmpchar, 2048);
	ifs.getline(cnstTmpchar, 2048);
	triNumber = atoi(cnstTmpchar);
	number = triNumber;

	for (int i = 0; i<triNumber; i++)
	{
		ifs.getline(cnstTmpchar, 2048);
		int indexnum;
		sscanf(cnstTmpchar, "%d%lf%lf", &indexnum, &pdfTriVex[2 * i + 0], &pdfTriVex[2 * i + 1]);
	}

	ifs.getline(cnstTmpchar, 2048);
	ifs.getline(cnstTmpchar, 2048);
	ifs.getline(cnstTmpchar, 2048);
	nfaces = atoi(cnstTmpchar);

	m_triangle = (GDALTriangulation*)CPLCalloc(1, sizeof(GDALTriangulation));
	m_facets = (GDALTriFacet*)VSIMalloc2(nfaces, sizeof(GDALTriFacet));
	m_barycentricCoefficients = (GDALTriBarycentricCoefficients*)VSIMalloc2(nfaces, sizeof(GDALTriBarycentricCoefficients));

	m_triangle->nFacets = nfaces;
	m_triangle->pasFacetCoefficients = m_barycentricCoefficients;
	m_triangle->pasFacets = m_facets;

	for (int i = 0; i<nfaces; i++)
	{
		ifs.getline(cnstTmpchar, 2048);
		sscanf(cnstTmpchar, "%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf", &m_triangle->pasFacets[i].anVertexIdx[0], &m_triangle->pasFacets[i].anVertexIdx[1], &m_triangle->pasFacets[i].anVertexIdx[2],
			&m_triangle->pasFacets[i].anNeighborIdx[0], &m_triangle->pasFacets[i].anNeighborIdx[1], &m_triangle->pasFacets[i].anNeighborIdx[2],
			&m_triangle->pasFacetCoefficients->dfCstX, &m_triangle->pasFacetCoefficients->dfCstY, &m_triangle->pasFacetCoefficients->dfMul1X, &m_triangle->pasFacetCoefficients->dfMul1Y,
			&m_triangle->pasFacetCoefficients->dfMul2X, &m_triangle->pasFacetCoefficients->dfMul2Y);

	}
	ifs.close();
	return m_triangle;
}
//=========================================================================================================================================

/*
通过地理坐标对影像进行拼接
功能：1.获取去除后缀的文件名
2.创建输出文件
3.转换AOI到源文件之间的行列号
4.加载镶嵌块
5.影像镶嵌
*/
inline string GeoMosaic::GDALTool_GetFileName(const char* pszFile)
{
	string temp = pszFile;
	size_t a = temp.find_last_of('\\');
	size_t aa = temp.find_last_of('/');

	if (a != string::npos && aa != string::npos)
		a = max(a, aa);
	else
		a = max(a, aa);

	size_t b = temp.find_last_of('.');
	string strLayerName = temp.substr(a + 1);
	return strLayerName;
}
GDALDatasetH
GeoMosaic::GDALTool_GDALWarpCreateOutput(char **papszSrcFiles, const char *pszFilename,
	const char *pszFormat, char **papszTO,
	char ***ppapszCreateOptions, GDALDataType eDT)
{
	GDALDriverH hDriver;
	GDALDatasetH hDstDS;
	void *hTransformArg;
	GDALColorTableH hCT = NULL;
	double dfWrkMinX = 0, dfWrkMaxX = 0, dfWrkMinY = 0, dfWrkMaxY = 0;
	double dfWrkResX = 0, dfWrkResY = 0;
	int nDstBandCount = 0;

	//获取输出文件类型驱动
	hDriver = GDALGetDriverByName(pszFormat);
	if (hDriver == NULL || GDALGetMetadataItem(hDriver, GDAL_DCAP_CREATE, NULL) == NULL)
		return NULL;

	//计算所有拼接影像地理范围
	char    *pszThisTargetSRS = (char*)CSLFetchNameValue(papszTO, "DST_SRS");
	if (pszThisTargetSRS != NULL)
		pszThisTargetSRS = CPLStrdup(pszThisTargetSRS);

	for (int iSrc = 0; papszSrcFiles[iSrc] != NULL; iSrc++)
	{
		GDALDatasetH hSrcDS;
		const char *pszThisSourceSRS = CSLFetchNameValue(papszTO, "SRC_SRS");

		hSrcDS = GDALOpen(papszSrcFiles[iSrc], GA_ReadOnly);
		if (hSrcDS == NULL)
			return NULL;

		// 检查当前文件是否存在波段
		if (GDALGetRasterCount(hSrcDS) == 0)
			return NULL;

		if (eDT == GDT_Unknown)
			eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS, 1));

		//将第一个颜色表的数值应用于最后输出文件
		if (iSrc == 0)
		{
			nDstBandCount = GDALGetRasterCount(hSrcDS);
			hCT = GDALGetRasterColorTable(GDALGetRasterBand(hSrcDS, 1));
			if (hCT != NULL)
				hCT = GDALCloneColorTable(hCT);
		}

		//如果没有设置投影信息则获取投影信息
		if (pszThisSourceSRS == NULL)
		{
			const char *pszMethod = CSLFetchNameValue(papszTO, "METHOD");

			if (GDALGetProjectionRef(hSrcDS) != NULL &&
				strlen(GDALGetProjectionRef(hSrcDS)) > 0 &&
				(pszMethod == NULL || EQUAL(pszMethod, "GEOTRANSFORM")))
				pszThisSourceSRS = GDALGetProjectionRef(hSrcDS);
			else if (GDALGetGCPProjection(hSrcDS) != NULL &&
				strlen(GDALGetGCPProjection(hSrcDS)) > 0 &&
				GDALGetGCPCount(hSrcDS) > 1 &&
				(pszMethod == NULL || EQUALN(pszMethod, "GCP_", 4)))
				pszThisSourceSRS = GDALGetGCPProjection(hSrcDS);
			else if (pszMethod != NULL && EQUAL(pszMethod, "RPC"))
				pszThisSourceSRS = SRS_WKT_WGS84;
			else
				pszThisSourceSRS = "";
		}

		if (pszThisTargetSRS == NULL)
			pszThisTargetSRS = CPLStrdup(pszThisSourceSRS);

		//创建从源影像到目标影像的投影转换
		hTransformArg =
			GDALCreateGenImgProjTransformer2(hSrcDS, NULL, papszTO);

		if (hTransformArg == NULL)
		{
			CPLFree(pszThisTargetSRS);
			GDALClose(hSrcDS);
			return NULL;
		}


		//获取输出影像大致位置
		double adfThisGeoTransform[6];
		double adfExtent[4];
		int    nThisPixels, nThisLines;

		if (GDALSuggestedWarpOutput2(hSrcDS,
			GDALGenImgProjTransform, hTransformArg,
			adfThisGeoTransform,
			&nThisPixels, &nThisLines,
			adfExtent, 0) != CE_None)
		{
			CPLFree(pszThisTargetSRS);
			GDALClose(hSrcDS);
			return NULL;
		}

		if (CPLGetConfigOption("CHECK_WITH_INVERT_PROJ", NULL) == NULL)
		{
			double MinX = adfExtent[0];
			double MaxX = adfExtent[2];
			double MaxY = adfExtent[3];
			double MinY = adfExtent[1];
			int bSuccess = TRUE;

			/* Check that the the edges of the target image are in the validity area */
			/* of the target projection */
#define N_STEPS 20
			int i, j;
			for (i = 0; i <= N_STEPS && bSuccess; i++)
			{
				for (j = 0; j <= N_STEPS && bSuccess; j++)
				{
					double dfRatioI = i * 1.0 / N_STEPS;
					double dfRatioJ = j * 1.0 / N_STEPS;
					double expected_x = (1 - dfRatioI) * MinX + dfRatioI * MaxX;
					double expected_y = (1 - dfRatioJ) * MinY + dfRatioJ * MaxY;
					double x = expected_x;
					double y = expected_y;
					double z = 0;

					/* Target SRS coordinates to source image pixel coordinates */
					if (!GDALGenImgProjTransform(hTransformArg, TRUE, 1, &x, &y, &z, &bSuccess) || !bSuccess)
						bSuccess = FALSE;
					/* Source image pixel coordinates to target SRS coordinates */
					if (!GDALGenImgProjTransform(hTransformArg, FALSE, 1, &x, &y, &z, &bSuccess) || !bSuccess)
						bSuccess = FALSE;

					if (fabs(x - expected_x) > (MaxX - MinX) / nThisPixels ||
						fabs(y - expected_y) > (MaxY - MinY) / nThisLines)
						bSuccess = FALSE;
				}
			}

			/* If not, retry with CHECK_WITH_INVERT_PROJ=TRUE that forces ogrct.cpp */
			/* to check the consistency of each requested projection result with the */
			/* invert projection */
			if (!bSuccess)
			{
				CPLSetConfigOption("CHECK_WITH_INVERT_PROJ", "TRUE");
				CPLDebug("WARP", "Recompute out extent with CHECK_WITH_INVERT_PROJ=TRUE");
				GDALDestroyGenImgProjTransformer(hTransformArg);
				hTransformArg =
					GDALCreateGenImgProjTransformer2(hSrcDS, NULL, papszTO);

				if (GDALSuggestedWarpOutput2(hSrcDS,
					GDALGenImgProjTransform, hTransformArg,
					adfThisGeoTransform,
					&nThisPixels, &nThisLines,
					adfExtent, 0) != CE_None)
				{
					CPLFree(pszThisTargetSRS);
					GDALClose(hSrcDS);
					return NULL;
				}
			}
		}

		/* -------------------------------------------------------------------- */
		/*      Expand the working bounds to include this region, ensure the    */
		/*      working resolution is no more than this resolution.             */
		/* -------------------------------------------------------------------- */
		if (dfWrkMaxX == 0.0 && dfWrkMinX == 0.0)
		{
			dfWrkMinX = adfExtent[0];
			dfWrkMaxX = adfExtent[2];
			dfWrkMaxY = adfExtent[3];
			dfWrkMinY = adfExtent[1];
			dfWrkResX = adfThisGeoTransform[1];
			dfWrkResY = ABS(adfThisGeoTransform[5]);
		}
		else
		{
			dfWrkMinX = MIN(dfWrkMinX, adfExtent[0]);
			dfWrkMaxX = MAX(dfWrkMaxX, adfExtent[2]);
			dfWrkMaxY = MAX(dfWrkMaxY, adfExtent[3]);
			dfWrkMinY = MIN(dfWrkMinY, adfExtent[1]);
			dfWrkResX = MIN(dfWrkResX, adfThisGeoTransform[1]);
			dfWrkResY = MIN(dfWrkResY, ABS(adfThisGeoTransform[5]));
		}

		GDALDestroyGenImgProjTransformer(hTransformArg);

		GDALClose(hSrcDS);
	}


	//检查是否存在不可用源文件
	if (nDstBandCount == 0)
	{
		CPLError(CE_Failure, CPLE_AppDefined, "No usable source images.");
		CPLFree(pszThisTargetSRS);
		return NULL;
	}


	//获取输出影像行列数
	double adfDstGeoTransform[6];
	int nPixels, nLines;

	adfDstGeoTransform[0] = dfWrkMinX;
	adfDstGeoTransform[1] = dfWrkResX;
	adfDstGeoTransform[2] = 0.0;
	adfDstGeoTransform[3] = dfWrkMaxY;
	adfDstGeoTransform[4] = 0.0;
	adfDstGeoTransform[5] = -1 * dfWrkResY;

	nPixels = (int)((dfWrkMaxX - dfWrkMinX) / dfWrkResX + 0.5);
	nLines = (int)((dfWrkMaxY - dfWrkMinY) / dfWrkResY + 0.5);

	//用户是否改变了相应变量
	if (dfXRes != 0.0 && dfYRes != 0.0)
	{
		if (dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0)
		{
			dfMinX = adfDstGeoTransform[0];
			dfMaxX = adfDstGeoTransform[0] + adfDstGeoTransform[1] * nPixels;
			dfMaxY = adfDstGeoTransform[3];
			dfMinY = adfDstGeoTransform[3] + adfDstGeoTransform[5] * nLines;
		}

		nPixels = (int)((dfMaxX - dfMinX + (dfXRes / 2.0)) / dfXRes);
		nLines = (int)((dfMaxY - dfMinY + (dfYRes / 2.0)) / dfYRes);
		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;
	}
	else if (nForcePixels != 0 && nForceLines != 0)
	{
		if (dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0)
		{
			dfMinX = dfWrkMinX;
			dfMaxX = dfWrkMaxX;
			dfMaxY = dfWrkMaxY;
			dfMinY = dfWrkMinY;
		}

		dfXRes = (dfMaxX - dfMinX) / nForcePixels;
		dfYRes = (dfMaxY - dfMinY) / nForceLines;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;

		nPixels = nForcePixels;
		nLines = nForceLines;
	}
	else if (nForcePixels != 0)
	{
		if (dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0)
		{
			dfMinX = dfWrkMinX;
			dfMaxX = dfWrkMaxX;
			dfMaxY = dfWrkMaxY;
			dfMinY = dfWrkMinY;
		}

		dfXRes = (dfMaxX - dfMinX) / nForcePixels;
		dfYRes = dfXRes;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;

		nPixels = nForcePixels;
		nLines = (int)((dfMaxY - dfMinY + (dfYRes / 2.0)) / dfYRes);
	}
	else if (nForceLines != 0)
	{
		if (dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0)
		{
			dfMinX = dfWrkMinX;
			dfMaxX = dfWrkMaxX;
			dfMaxY = dfWrkMaxY;
			dfMinY = dfWrkMinY;
		}

		dfYRes = (dfMaxY - dfMinY) / nForceLines;
		dfXRes = dfYRes;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;

		nPixels = (int)((dfMaxX - dfMinX + (dfXRes / 2.0)) / dfXRes);
		nLines = nForceLines;
	}
	else if (dfMinX != 0.0 || dfMinY != 0.0 || dfMaxX != 0.0 || dfMaxY != 0.0)
	{
		dfXRes = adfDstGeoTransform[1];
		dfYRes = fabs(adfDstGeoTransform[5]);

		nPixels = (int)((dfMaxX - dfMinX + (dfXRes / 2.0)) / dfXRes);
		nLines = (int)((dfMaxY - dfMinY + (dfYRes / 2.0)) / dfYRes);

		dfXRes = (dfMaxX - dfMinX) / nPixels;
		dfYRes = (dfMaxY - dfMinY) / nLines;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;
	}

	//是否要添加alpha波段
	if (bEnableSrcAlpha)
		nDstBandCount--;

	if (bEnableDstAlpha)
		nDstBandCount++;

	//创建输出影像
	hDstDS = GDALCreate(hDriver, pszFilename, nPixels, nLines,
		nDstBandCount, eDT, *ppapszCreateOptions);

	if (hDstDS == NULL)
	{
		CPLFree(pszThisTargetSRS);
		return NULL;
	}

	//将投影信息写入输出影像
	GDALSetProjection(hDstDS, pszThisTargetSRS);
	GDALSetGeoTransform(hDstDS, adfDstGeoTransform);

	/* -------------------------------------------------------------------- */
	/*      Try to set color interpretation of output file alpha band.      */
	/*      TODO: We should likely try to copy the other bands too.         */
	/* -------------------------------------------------------------------- */
	if (bEnableDstAlpha)
	{
		GDALSetRasterColorInterpretation(
			GDALGetRasterBand(hDstDS, nDstBandCount),
			GCI_AlphaBand);
	}

	/* -------------------------------------------------------------------- */
	/*      Copy the color table, if required.                              */
	/* -------------------------------------------------------------------- */
	if (hCT != NULL)
	{
		GDALSetRasterColorTable(GDALGetRasterBand(hDstDS, 1), hCT);
		GDALDestroyColorTable(hCT);
	}

	CPLFree(pszThisTargetSRS);
	return hDstDS;
}
void GeoMosaic::GDALTool_TransformCutlineToSource(GDALDatasetH hSrcDS, void *hCutline,
	char ***ppapszWarpOptions, char **papszTO_In)
{
	OGRGeometryH hMultiPolygon = OGR_G_Clone((OGRGeometryH)hCutline);
	char **papszTO = CSLDuplicate(papszTO_In);

	//检查源投影是一致的
	OGRSpatialReferenceH  hRasterSRS = NULL;
	const char *pszProjection = NULL;

	if (GDALGetProjectionRef(hSrcDS) != NULL
		&& strlen(GDALGetProjectionRef(hSrcDS)) > 0)
		pszProjection = GDALGetProjectionRef(hSrcDS);
	else if (GDALGetGCPProjection(hSrcDS) != NULL)
		pszProjection = GDALGetGCPProjection(hSrcDS);

	if (pszProjection != NULL)
	{
		hRasterSRS = OSRNewSpatialReference(NULL);
		if (OSRImportFromWkt(hRasterSRS, (char **)&pszProjection) != CE_None)
		{
			OSRDestroySpatialReference(hRasterSRS);
			hRasterSRS = NULL;
		}
	}

	OGRSpatialReferenceH hSrcSRS = OGR_G_GetSpatialReference(hMultiPolygon);
	if (hRasterSRS != NULL && hSrcSRS != NULL)
	{
		/* ok, we will reproject */
	}
	else if (hRasterSRS != NULL && hSrcSRS == NULL)
	{
		fprintf(stderr,
			"Warning : the source raster dataset has a SRS, but the input vector layer\n"
			"not.  Cutline results may be incorrect.\n");
	}
	else if (hRasterSRS == NULL && hSrcSRS != NULL)
	{
		fprintf(stderr,
			"Warning : the input vector layer has a SRS, but the source raster dataset does not.\n"
			"Cutline results may be incorrect.\n");
	}

	if (hRasterSRS != NULL)
		OSRDestroySpatialReference(hRasterSRS);

	/* -------------------------------------------------------------------- */
	/*      Extract the cutline SRS WKT.                                    */
	/* -------------------------------------------------------------------- */
	if (hSrcSRS != NULL)
	{
		char *pszCutlineSRS_WKT = NULL;

		OSRExportToWkt(hSrcSRS, &pszCutlineSRS_WKT);
		papszTO = CSLSetNameValue(papszTO, "DST_SRS", pszCutlineSRS_WKT);
		CPLFree(pszCutlineSRS_WKT);
	}
	else
	{
		int iDstSRS = CSLFindString(papszTO, "DST_SRS");
		if (iDstSRS >= 0)
			papszTO = CSLRemoveStrings(papszTO, iDstSRS, 1, NULL);
	}

	//地理坐标到影像行列的转换
	CutlineTransformer oTransformer;

	oTransformer.hSrcImageTransformer =
		GDALCreateGenImgProjTransformer2(hSrcDS, NULL, papszTO);

	CSLDestroy(papszTO);

	if (oTransformer.hSrcImageTransformer == NULL)
		exit(1);

	OGR_G_Transform(hMultiPolygon,
		(OGRCoordinateTransformationH)&oTransformer);

	GDALDestroyGenImgProjTransformer(oTransformer.hSrcImageTransformer);

	/* -------------------------------------------------------------------- */
	/*      Convert aggregate geometry into WKT.                            */
	/* -------------------------------------------------------------------- */
	char *pszWKT = NULL;

	OGR_G_ExportToWkt(hMultiPolygon, &pszWKT);
	OGR_G_DestroyGeometry(hMultiPolygon);

	*ppapszWarpOptions = CSLSetNameValue(*ppapszWarpOptions,
		"CUTLINE", pszWKT);
	CPLFree(pszWKT);
}
long GeoMosaic::GDALTool_LoadCutline(const char *pszCutlineDSName, const char *pszCLayer,
	const char *pszCWHERE, const char *pszCSQL, void **phCutlineRet)
{
	OGRRegisterAll();

	//打开矢量数据集
	OGRDataSourceH hSrcDS;

	hSrcDS = OGROpen(pszCutlineDSName, FALSE, NULL);
	if (hSrcDS == NULL)
		return -1;

	//获取矢量图层
	OGRLayerH hLayer = NULL;

	if (pszCSQL != NULL)
		hLayer = OGR_DS_ExecuteSQL(hSrcDS, pszCSQL, NULL, NULL);
	else if (pszCLayer != NULL)
		hLayer = OGR_DS_GetLayerByName(hSrcDS, pszCLayer);
	else
		hLayer = OGR_DS_GetLayer(hSrcDS, 0);

	if (hLayer == NULL)
	{
		OGR_DS_Destroy(hSrcDS);
		return -2;
	}

	/* -------------------------------------------------------------------- */
	/*      Apply WHERE clause if there is one.                             */
	/* -------------------------------------------------------------------- */
	if (pszCWHERE != NULL)
		OGR_L_SetAttributeFilter(hLayer, pszCWHERE);

	/* -------------------------------------------------------------------- */
	/*      Collect the geometries from this layer, and build list of       */
	/*      burn values.                                                    */
	/* -------------------------------------------------------------------- */
	OGRFeatureH hFeat;
	OGRGeometryH hMultiPolygon = OGR_G_CreateGeometry(wkbMultiPolygon);

	OGR_L_ResetReading(hLayer);

	while ((hFeat = OGR_L_GetNextFeature(hLayer)) != NULL)
	{
		OGRGeometryH hGeom = OGR_F_GetGeometryRef(hFeat);

		if (hGeom == NULL)
		{
			OGR_DS_Destroy(hSrcDS);
			return -2;
		}

		OGRwkbGeometryType eType = wkbFlatten(OGR_G_GetGeometryType(hGeom));

		if (eType == wkbPolygon)
			OGR_G_AddGeometry(hMultiPolygon, hGeom);
		else if (eType == wkbMultiPolygon)
		{
			int iGeom;

			for (iGeom = 0; iGeom < OGR_G_GetGeometryCount(hGeom); iGeom++)
			{
				OGR_G_AddGeometry(hMultiPolygon,
					OGR_G_GetGeometryRef(hGeom, iGeom));
			}
		}
		else
		{
			OGR_DS_Destroy(hSrcDS);
			return -2;
		}
		OGR_F_Destroy(hFeat);
	}

	if (OGR_G_GetGeometryCount(hMultiPolygon) == 0)
	{
		return -2;
	}

	/* -------------------------------------------------------------------- */
	/*      Ensure the coordinate system gets set on the geometry.          */
	/* -------------------------------------------------------------------- */
	OGR_G_AssignSpatialReference(hMultiPolygon, OGR_L_GetSpatialRef(hLayer));

	*phCutlineRet = (void *)hMultiPolygon;

	//清除指针
	if (pszCSQL != NULL)
		OGR_DS_ReleaseResultSet(hSrcDS, hLayer);

	OGR_DS_Destroy(hSrcDS);
	return 0;
}
long GeoMosaic::GDALTool_ImageMosaicing(vector<string> vStrSrcFiles, const char* pszCutLineFile, const char* pszOutFile,
	GDALResampleAlg eResampleMethod, const char *pszFormat)
{
	//输入输出文件
	GDALDatasetH		hDstDS;
	char              **papszSrcFiles = NULL;
	const char         *pszDstFilename = NULL;
	int                 bCreateOutput = FALSE;
	//坐标转换对象
	void               *hTransformArg, *hGenImgProjArg = NULL, *hApproxArg = NULL;
	char              **papszWarpOptions = NULL;
	double              dfErrorThreshold = 0.125;
	GDALTransformerFunc pfnTransformer = NULL;
	//输出文件选项
	char              **papszCreateOptions = NULL;
	GDALDataType        eOutputType = GDT_Unknown, eWorkingType = GDT_Unknown;

	GDALResampleAlg     eResampleAlg = (GDALResampleAlg)eResampleMethod;
	//NODATA设置
	const char         *pszSrcNodata = NULL;
	const char         *pszDstNodata = NULL;
	int                 bMulti = FALSE;
	char              **papszTO = NULL;
	//镶嵌线文件
	const char         *pszCutlineDSName = NULL;

	if (vStrSrcFiles.empty())
		return -1;

	GDALAllRegister();
	OGRRegisterAll();

	pszSrcNodata = "0 0 0";
	pszDstNodata = "0 0 0";
	dfXRes = 0.0;
	dfYRes = 0.0;
	bCreateOutput = TRUE;
	eResampleAlg = GRA_Bilinear;
	pszCutlineDSName = pszCutLineFile;

	for (size_t i = 0; i<vStrSrcFiles.size(); i++)
	{
		string strFile = vStrSrcFiles[i];
		papszSrcFiles = CSLAddString(papszSrcFiles, strFile.c_str());
	}

	pszDstFilename = pszOutFile;
	if (pszDstFilename == NULL)
		return -2;

	/* -------------------------------------------------------------------- */
	/*      输出文件是否已经存在？                                          */
	/* -------------------------------------------------------------------- */
	CPLPushErrorHandler(CPLQuietErrorHandler);
	hDstDS = GDALOpen(pszDstFilename, GA_Update);
	CPLPopErrorHandler();

	//避免重写已经存在的文件
	if (hDstDS == NULL)
	{
		CPLPushErrorHandler(CPLQuietErrorHandler);
		hDstDS = GDALOpen(pszDstFilename, GA_ReadOnly);
		CPLPopErrorHandler();

		if (hDstDS)
		{
			fprintf(stderr, "输出文件%s存在，但是不能写入\n", pszDstFilename);
			GDALClose(hDstDS);
			return -2;
		}
	}

	/* -------------------------------------------------------------------- */
	/*      创建输出文件                                                    */
	/* -------------------------------------------------------------------- */
	int   bInitDestSetForFirst = FALSE;

	if (hDstDS == NULL)
	{
		int iResult = 0;
		hDstDS = GDALTool_GDALWarpCreateOutput(papszSrcFiles, pszDstFilename, pszFormat,
			papszTO, &papszCreateOptions, eOutputType);

		if (iResult != 0)
			return iResult;

		bCreateOutput = TRUE;

		if (CSLFetchNameValue(papszWarpOptions, "INIT_DEST") == NULL &&
			pszDstNodata == NULL)
		{
			papszWarpOptions = CSLSetNameValue(papszWarpOptions,
				"INIT_DEST", "0");
			bInitDestSetForFirst = TRUE;
		}
		else if (CSLFetchNameValue(papszWarpOptions, "INIT_DEST") == NULL)
		{
			papszWarpOptions = CSLSetNameValue(papszWarpOptions,
				"INIT_DEST", "NO_DATA");
			bInitDestSetForFirst = TRUE;
		}

		CSLDestroy(papszCreateOptions);
		papszCreateOptions = NULL;
	}

	if (hDstDS == NULL)
		return -2;

	/* -------------------------------------------------------------------- */
	/*      遍历所有的输入文件，并将其写入输出文件                          */
	/* -------------------------------------------------------------------- */
	int iRev = 0;
	for (int iSrc = 0; papszSrcFiles[iSrc] != NULL; iSrc++)
	{
		GDALDatasetH hSrcDS;

		/* -------------------------------------------------------------------- */
		/*      打开文件                                                        */
		/* -------------------------------------------------------------------- */
		hSrcDS = GDALOpen(papszSrcFiles[iSrc], GA_ReadOnly);

		if (hSrcDS == NULL)
		{
			iRev = -2;
			goto CLEAN;
		}

		/* -------------------------------------------------------------------- */
		/*      检查输入文件是否不存在波段                                      */
		/* -------------------------------------------------------------------- */
		if (GDALGetRasterCount(hSrcDS) == 0)
		{
			fprintf(stderr, "输入文件 %s 不存在波段。\n", papszSrcFiles[iSrc]);
			iRev = -2;
			goto CLEAN;
		}

		/* -------------------------------------------------------------------- */
		/*      处理alpha波段                                                   */
		/* -------------------------------------------------------------------- */
		GDALColorInterp ci = GDALGetRasterColorInterpretation(
			GDALGetRasterBand(hSrcDS, GDALGetRasterCount(hSrcDS)));

		if (ci == GCI_AlphaBand && !bEnableSrcAlpha)
			bEnableSrcAlpha = TRUE;

		/* -------------------------------------------------------------------- */
		/*      创建转换参数从源坐标到目标坐标系                                */
		/* -------------------------------------------------------------------- */
		hTransformArg = hGenImgProjArg =
			GDALCreateGenImgProjTransformer2(hSrcDS, hDstDS, papszTO);

		if (hTransformArg == NULL)
		{
			iRev = -2;
			goto CLEAN;
		}

		pfnTransformer = GDALGenImgProjTransform;

		/* -------------------------------------------------------------------- */
		/*      Warp the transformer with a linear approximator unless the      */
		/*      acceptable error is zero.                                       */
		/* -------------------------------------------------------------------- */
		if (dfErrorThreshold != 0.0)
		{
			hTransformArg = hApproxArg =
				GDALCreateApproxTransformer(GDALGenImgProjTransform,
					hGenImgProjArg, dfErrorThreshold);
			pfnTransformer = GDALApproxTransform;
		}

		/* -------------------------------------------------------------------- */
		/*      Clear temporary INIT_DEST settings after the first image.       */
		/* -------------------------------------------------------------------- */
		if (bInitDestSetForFirst && iSrc == 1)
			papszWarpOptions = CSLSetNameValue(papszWarpOptions,
				"INIT_DEST", NULL);

		/* -------------------------------------------------------------------- */
		/*      创建warp选项                                                    */
		/* -------------------------------------------------------------------- */
		GDALWarpOptions *psWO = GDALCreateWarpOptions();

		psWO->papszWarpOptions = CSLDuplicate(papszWarpOptions);
		psWO->eWorkingDataType = eWorkingType;
		psWO->eResampleAlg = eResampleAlg;

		psWO->hSrcDS = hSrcDS;
		psWO->hDstDS = hDstDS;

		psWO->pfnTransformer = pfnTransformer;
		psWO->pTransformerArg = hTransformArg;
		psWO->dfWarpMemoryLimit = 52428800;		//使用50M的内存

												/* -------------------------------------------------------------------- */
												/*      创建波段映射关系                                                */
												/* -------------------------------------------------------------------- */
		if (bEnableSrcAlpha)
			psWO->nBandCount = GDALGetRasterCount(hSrcDS) - 1;
		else
			psWO->nBandCount = GDALGetRasterCount(hSrcDS);

		psWO->panSrcBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
		psWO->panDstBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));

		for (int i = 0; i < psWO->nBandCount; i++)
		{
			psWO->panSrcBands[i] = i + 1;
			psWO->panDstBands[i] = i + 1;
		}

		/* -------------------------------------------------------------------- */
		/*      构建alpha波段                                                   */
		/* -------------------------------------------------------------------- */
		if (bEnableSrcAlpha)
			psWO->nSrcAlphaBand = GDALGetRasterCount(hSrcDS);

		if (!bEnableDstAlpha
			&& GDALGetRasterCount(hDstDS) == psWO->nBandCount + 1
			&& GDALGetRasterColorInterpretation(
				GDALGetRasterBand(hDstDS, GDALGetRasterCount(hDstDS)))
			== GCI_AlphaBand)
		{
			printf("Using band %d of destination image as alpha.\n",
				GDALGetRasterCount(hDstDS));

			bEnableDstAlpha = TRUE;
		}

		if (bEnableDstAlpha)
			psWO->nDstAlphaBand = GDALGetRasterCount(hDstDS);

		/* -------------------------------------------------------------------- */
		/*      创建  NODATA 选项                                               */
		/* -------------------------------------------------------------------- */
		if (pszSrcNodata != NULL && !EQUALN(pszSrcNodata, "n", 1))
		{
			char **papszTokens = CSLTokenizeString(pszSrcNodata);
			int  nTokenCount = CSLCount(papszTokens);

			psWO->padfSrcNoDataReal = (double *)
				CPLMalloc(psWO->nBandCount*sizeof(double));
			psWO->padfSrcNoDataImag = (double *)
				CPLMalloc(psWO->nBandCount*sizeof(double));

			for (int i = 0; i < psWO->nBandCount; i++)
			{
				if (i < nTokenCount)
				{
					CPLStringToComplex(papszTokens[i],
						psWO->padfSrcNoDataReal + i,
						psWO->padfSrcNoDataImag + i);
				}
				else
				{
					psWO->padfSrcNoDataReal[i] = psWO->padfSrcNoDataReal[i - 1];
					psWO->padfSrcNoDataImag[i] = psWO->padfSrcNoDataImag[i - 1];
				}
			}

			CSLDestroy(papszTokens);

			psWO->papszWarpOptions = CSLSetNameValue(psWO->papszWarpOptions,
				"UNIFIED_SRC_NODATA", "YES");
		}

		/* -------------------------------------------------------------------- */
		/*      没有指定NODATA值，从源文件中读取                                */
		/* -------------------------------------------------------------------- */
		if (pszSrcNodata == NULL)
		{
			int bHaveNodata = FALSE;
			double dfReal = 0.0;

			for (int i = 0; !bHaveNodata && i < psWO->nBandCount; i++)
			{
				GDALRasterBandH hBand = GDALGetRasterBand(hSrcDS, i + 1);
				dfReal = GDALGetRasterNoDataValue(hBand, &bHaveNodata);
			}

			if (bHaveNodata)
			{
				printf("Using internal nodata values (eg. %g) for image %s.\n",
					dfReal, papszSrcFiles[iSrc]);
				psWO->padfSrcNoDataReal = (double *)
					CPLMalloc(psWO->nBandCount*sizeof(double));
				psWO->padfSrcNoDataImag = (double *)
					CPLMalloc(psWO->nBandCount*sizeof(double));

				for (int i = 0; i < psWO->nBandCount; i++)
				{
					GDALRasterBandH hBand = GDALGetRasterBand(hSrcDS, i + 1);

					dfReal = GDALGetRasterNoDataValue(hBand, &bHaveNodata);

					if (bHaveNodata)
					{
						psWO->padfSrcNoDataReal[i] = dfReal;
						psWO->padfSrcNoDataImag[i] = 0.0;
					}
					else
					{
						psWO->padfSrcNoDataReal[i] = -123456.789;
						psWO->padfSrcNoDataImag[i] = 0.0;
					}
				}
			}
		}

		/* -------------------------------------------------------------------- */
		/*      设置输出文件的NODATA为最大值                                    */
		/* -------------------------------------------------------------------- */
		if (pszDstNodata != NULL)
		{
			char **papszTokens = CSLTokenizeString(pszDstNodata);
			int  nTokenCount = CSLCount(papszTokens);

			psWO->padfDstNoDataReal = (double *)
				CPLMalloc(psWO->nBandCount*sizeof(double));
			psWO->padfDstNoDataImag = (double *)
				CPLMalloc(psWO->nBandCount*sizeof(double));

			for (int i = 0; i < psWO->nBandCount; i++)
			{
				if (i < nTokenCount)
				{
					CPLStringToComplex(papszTokens[i],
						psWO->padfDstNoDataReal + i,
						psWO->padfDstNoDataImag + i);
				}
				else
				{
					psWO->padfDstNoDataReal[i] = psWO->padfDstNoDataReal[i - 1];
					psWO->padfDstNoDataImag[i] = psWO->padfDstNoDataImag[i - 1];
				}

				GDALRasterBandH hBand = GDALGetRasterBand(hDstDS, i + 1);
				int bClamped = FALSE, bRounded = FALSE;

#define CLAMP(val,type,minval,maxval) \
	do { if (val < minval) { bClamped = TRUE; val = minval; } \
		else if (val > maxval) { bClamped = TRUE; val = maxval; } \
		else if (val != (type)val) { bRounded = TRUE; val = (type)(val + 0.5); } } \
		while(0)

				switch (GDALGetRasterDataType(hBand))
				{
				case GDT_Byte:
					CLAMP(psWO->padfDstNoDataReal[i], GByte,
						0.0, 255.0);
					break;
				case GDT_Int16:
					CLAMP(psWO->padfDstNoDataReal[i], GInt16,
						-32768.0, 32767.0);
					break;
				case GDT_UInt16:
					CLAMP(psWO->padfDstNoDataReal[i], GUInt16,
						0.0, 65535.0);
					break;
				case GDT_Int32:
					CLAMP(psWO->padfDstNoDataReal[i], GInt32,
						-2147483648.0, 2147483647.0);
					break;
				case GDT_UInt32:
					CLAMP(psWO->padfDstNoDataReal[i], GUInt32,
						0.0, 4294967295.0);
					break;
				default:
					break;
				}

				if (bClamped)
				{
					printf("for band %d, destination nodata value has been clamped "
						"to %.0f, the original value being out of range.\n",
						i + 1, psWO->padfDstNoDataReal[i]);
				}
				else if (bRounded)
				{
					printf("for band %d, destination nodata value has been rounded "
						"to %.0f, %s being an integer datatype.\n",
						i + 1, psWO->padfDstNoDataReal[i],
						GDALGetDataTypeName(GDALGetRasterDataType(hBand)));
				}

				if (bCreateOutput)
				{
					GDALSetRasterNoDataValue(
						GDALGetRasterBand(hDstDS, psWO->panDstBands[i]),
						psWO->padfDstNoDataReal[i]);
				}
			}

			CSLDestroy(papszTokens);
		}

		/* -------------------------------------------------------------------- */
		/*      读取镶嵌线                                                      */
		/* -------------------------------------------------------------------- */
		void *hCutline = NULL;
		if (pszCutlineDSName != NULL)
		{
			string strFileName = GDALTool_GetFileName(papszSrcFiles[iSrc]);
			string strWhere = "影像路径=\"" + strFileName + "\"";
			GDALTool_LoadCutline(pszCutlineDSName, NULL, strWhere.c_str(), NULL, &hCutline);
		}

		if (hCutline != NULL)
		{
			GDALTool_TransformCutlineToSource(hSrcDS, hCutline, &(psWO->papszWarpOptions), papszTO);
		}

		/* -------------------------------------------------------------------- */
		/*      初始化执行warp                                                  */
		/* -------------------------------------------------------------------- */
		GDALWarpOperation oWO;
		CPLErr CE = CE_None;
		if (oWO.Initialize(psWO) == CE_None)
		{
			if (bMulti)
				CE = oWO.ChunkAndWarpMulti(0, 0, GDALGetRasterXSize(hDstDS), GDALGetRasterYSize(hDstDS));
			else
				CE = oWO.ChunkAndWarpImage(0, 0, GDALGetRasterXSize(hDstDS), GDALGetRasterYSize(hDstDS));
		}

		/* -------------------------------------------------------------------- */
		/*      清理资源                                                        */
		/* -------------------------------------------------------------------- */
		if (hApproxArg != NULL)
			GDALDestroyApproxTransformer(hApproxArg);

		if (hGenImgProjArg != NULL)
			GDALDestroyGenImgProjTransformer(hGenImgProjArg);

		GDALDestroyWarpOptions(psWO);

		if (hCutline != NULL)	//释放镶嵌块资源
		{
			OGR_G_DestroyGeometry((OGRGeometryH)hCutline);
			hCutline = NULL;
		}

		GDALClose(hSrcDS);

		if (CE != CE_None)
		{
			GDALClose(hDstDS);

			CSLDestroy(papszSrcFiles);
			CSLDestroy(papszWarpOptions);
			CSLDestroy(papszTO);

			GDALDumpOpenDatasets(stderr);
			OGRCleanupAll();
			return -1;
		}
	}

	/* -------------------------------------------------------------------- */
	/*      善后工作                                                        */
	/* -------------------------------------------------------------------- */
CLEAN:

	GDALClose(hDstDS);

	CSLDestroy(papszSrcFiles);
	CSLDestroy(papszWarpOptions);
	CSLDestroy(papszTO);

	//GDALDumpOpenDatasets( stderr );

	return 0;
}
long GeoMosaic::GDALTool_ImageMosaicing(char* path, int numBeg, int numEnd, const char* pszCutLineFile, const char* pszOutFile,
	GDALResampleAlg eResampleMethod, const char *pszFormat)
{
	vector<string> m_vStrSrcFile;

	//解析文件名
	char srcImgpathDir[_MAX_DIR];
	char srcImgextension[_MAX_EXT];
	char srcImgfileName[_MAX_FNAME];
	char srcImgDriver[_MAX_DRIVE];
	_splitpath_s(path, srcImgDriver, _MAX_DRIVE, srcImgpathDir, _MAX_DIR, srcImgfileName, _MAX_FNAME, srcImgextension, _MAX_EXT);
	char ss[10];
	char srcpath[_MAX_PATH];
	for (int i = numBeg; i <= numEnd; i++)
	{
		sprintf(ss, "IMG_%04d", i);
		_makepath_s(srcpath, _MAX_PATH, srcImgDriver, srcImgpathDir, ss, srcImgextension);
		string temp = srcpath;
		m_vStrSrcFile.push_back(temp);
	}

	return GDALTool_ImageMosaicing(m_vStrSrcFile, pszCutLineFile, pszOutFile, eResampleMethod, pszFormat);
}
//=========================================================================================================================================

/*
根据影像点文件生成shp格式的区域文件
//double x：地面x的坐标
//double y：地面y的坐标
*/
long CAreaFileGet::setPolygon(vector<GdPos> polygon)
{
	m_polygons.push_back(polygon);
	return 0;
}
/*************************点文件转换为区文件**************************/
//const char* projRef：投影信息
//const char* pathOut：影像输出路径
long CAreaFileGet::transTOpolygon(const char* projRef, const char* pathOut)
{
	if (m_polygons.size() <= 0)
		return -1;
	const char *pszDriverName = "ESRI Shapefile";
	GDALDriver *poDriver;

	OGRRegisterAll();

	//若没有投影信息则自定义投影信息
	OGRSpatialReference *poSpatialRef = new OGRSpatialReference(projRef);
	if (strlen(projRef) == 0)
	{
		poSpatialRef->SetProjCS("UTM 41(WGS84) in northern hemisphere.");
		poSpatialRef->SetWellKnownGeogCS("WGS84");
		poSpatialRef->SetUTM(41, TRUE);
	}


	//获取数据驱动
	poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	if (poDriver == NULL)
	{
		printf("%s driver not available.\n", pszDriverName);
		return -2;
	}

	//创建数据集
	// 	OGRDataSource *poDS;
	GDALDataset* poDS;
	poDS = poDriver->Create(pathOut, 0, 0, 0, GDT_Unknown, NULL);
	if (poDS == NULL)
	{
		printf("Creation of output file failed.\n");
		return -2;
	}

	//创建数据图层
	OGRLayer *poLayer;
	poLayer = poDS->CreateLayer("polyogon", poSpatialRef, wkbPolygon, NULL);
	if (poLayer == NULL)
	{
		printf("Layer creation failed.\n");
		return -2;
	}

	//数据字段信息
	OGRFieldDefn oField("Name", OFTString);
	oField.SetWidth(32);
	if (poLayer->CreateField(&oField) != OGRERR_NONE)
	{
		printf("Creating Name field failed.\n");
		return -2;
	}

	//数据字段输出
	for (int i = 0; i<m_polygons.size(); i++)
	{
		OGRFeature *poFeature = NULL;
		OGRLinearRing Ring;
		if (m_polygons[i].size() >= 3)
		{
			for (int j = 0; j<m_polygons[i].size(); j++)
				Ring.addPoint(m_polygons[i][j].x, m_polygons[i][j].y);
			poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
			Ring.closeRings();
			OGRPolygon polygon;
			polygon.addRing(&Ring);
			poFeature->SetGeometry(&polygon);
			if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
			{
				printf("Failed to create feature in shapefile.\n");
				return -2;
			}
		}
		OGRFeature::DestroyFeature(poFeature);
	}
	/*OGRDataSource::DestroyDataSource( poDS );*/
	GDALClose(poDS);
	m_polygons.clear();
	return 0;
}
//=========================================================================================================================================
/**
* @brief 图像校正坐标转换结构体
*/
struct TransformChain
{
	/*! GDAL坐标转换函数指针 */
	GDALTransformerFunc GDALTransformer;
	/*! GDAL坐标转换参数 */
	void *              GDALTransformerArg;
	/*! 输出图像6参数 */
	double              adfGeotransform[6];
	/*! 输出图像逆6参数 */
	double              adfInvGeotransform[6];
};
/**
* @brief 创建一个转换函数
*
* 该函数用于创建一个将输出图像投影坐标转到输出图像的行列号坐标
* @sa DestroyGeoToPixelTransform
* @returns 返回GDAL坐标转换回调函数的参数 \ref GeoToPixelTransform
*/
void *CreateGeoToPixelTransform(GDALTransformerFunc GDALTransformer, void *GDALTransformerArg, double *padfGeotransform)
{
	TransformChain *pChain = new TransformChain;
	pChain->GDALTransformer = GDALTransformer;
	pChain->GDALTransformerArg = GDALTransformerArg;
	memcpy(pChain->adfGeotransform, padfGeotransform, sizeof(double) * 6);

	if (!GDALInvGeoTransform(pChain->adfGeotransform, pChain->adfInvGeotransform))
	{
		// 如果输出图像六参数不能计算逆六参数，则不能进行变换处理
		delete pChain;
		return NULL;
	}
	return (void*)pChain;
}

/**
* @brief 析构转换参数
*/
void DestroyGeoToPixelTransform(void *GeoToPixelTransfomArg)
{
	delete static_cast<TransformChain *>(GeoToPixelTransfomArg);
}

/**
* @brief 坐标转换函数
* @sa CreateGeoToPixelTransform
*/
int GeoToPixelTransform(void *pTransformerArg, int bDstToSrc, int nPointCount, double *x, double *y, double *z, int *panSuccess)
{
	TransformChain *pChain = static_cast<TransformChain*>(pTransformerArg);
	if (pChain == NULL)
		return FALSE;

	if (!bDstToSrc)	//坐标正变换
	{
		// 先调用GDAL库中的坐标转换函数，将坐标转为输出图像的投影坐标
		if (!pChain->GDALTransformer(pChain->GDALTransformerArg, bDstToSrc, nPointCount, x, y, z, panSuccess))
			return FALSE;

		// 再从输出图像投影坐标系转到输出图像行列号
#pragma omp parallel for
		for (int i = 0; i < nPointCount; ++i)
		{
			if (!panSuccess[i])
				continue;

			double xP = x[i];
			double yP = y[i];
			GDALApplyGeoTransform(pChain->adfInvGeotransform, xP, yP, &x[i], &y[i]);
		}
	}
	else	//坐标逆变换
	{
		// 先从输出图像行列号转到输出图像投影坐标系
#pragma omp parallel for
		for (int i = 0; i < nPointCount; ++i)
		{
			double P = x[i];
			double L = y[i];
			GDALApplyGeoTransform(pChain->adfGeotransform, P, L, &x[i], &y[i]);
		}
		// 再调用GDAL库中坐标转换函数从输出图像投影坐标转换到原始坐标
		if (!pChain->GDALTransformer(pChain->GDALTransformerArg, bDstToSrc, nPointCount, x, y, z, panSuccess))
			return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long GeoGCPProcess::GeoProc_GCPWarpOrder(const char *pszSrcFilename, const char *pszDstFilename, int nReqOrder, int bReversed, const char *pszReferenceWKT, double *dGroundSize, double* GCPs, int gcpNum)
{
	printf("\rstarting geo correct by GCPs");
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
														// 打开原始图像并计算图像信息
	GDALDatasetH hSrcDS = GDALOpen(pszSrcFilename, GA_ReadOnly);
	GDALDataType eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS, 1));	//获取数据类型
	int iBandCount = GDALGetRasterCount(hSrcDS);

	// 创建几何多项式坐标转换关系
	GDAL_GCP* gcp_pairs = new GDAL_GCP[gcpNum];
	for (int i = 0; i<gcpNum; i++)
	{
		gcp_pairs[i].pszId = "";
		gcp_pairs[i].pszInfo = "";
		gcp_pairs[i].dfGCPPixel = GCPs[4 * i + 0];
		gcp_pairs[i].dfGCPLine = GCPs[4 * i + 1];
		gcp_pairs[i].dfGCPX = GCPs[4 * i + 2];
		gcp_pairs[i].dfGCPY = GCPs[4 * i + 3];
		gcp_pairs[i].dfGCPZ = 0.0;
	}
	void *hTransformArg = GDALCreateGCPTransformer(gcpNum, gcp_pairs, nReqOrder, FALSE);
	if (hTransformArg == NULL)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 使用SuggestedWarpOutput函数计算输出图像四至范围、大小、六参数等信息
	double adfGeoTransform[6];
	double adfExtent[4];
	int    nPixels, nLines;

	if (GDALSuggestedWarpOutput2(hSrcDS, GDALGCPTransform, hTransformArg,
		adfGeoTransform, &nPixels, &nLines, adfExtent, 0) != CE_None)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 下面开始根据用户指定的分辨率来反算输出图像的大小和六参数等信息
	double dResXSize = dGroundSize[0];
	double dResYSize = dGroundSize[1];

	//如果结果投影为投影坐标系统，则将其分辨率与原始影像一致
	if (dResXSize == 0.0 && dResYSize == 0.0)
	{
		double dGeoTrans[6] = { 0 };
		GDALGetGeoTransform(hSrcDS, dGeoTrans);
		dResXSize = ABS(dGeoTrans[1]);
		dResYSize = ABS(dGeoTrans[5]);
	}

	// 如果用户指定了输出图像的分辨率
	if (dResXSize != 0.0 || dResYSize != 0.0)
	{
		// 如果只指定了一个，使用自动计算的结果
		if (dResXSize == 0.0) dResXSize = adfGeoTransform[1];
		if (dResYSize == 0.0) dResYSize = adfGeoTransform[5];

		// 确保分辨率符号正确
		if (dResXSize < 0.0) dResXSize = -dResXSize;
		if (dResYSize > 0.0) dResYSize = -dResYSize;

		// 计算输出图像的范围
		double minX = adfGeoTransform[0];
		double maxX = adfGeoTransform[0] + adfGeoTransform[1] * nPixels;
		double maxY = adfGeoTransform[3];
		double minY = adfGeoTransform[3] + adfGeoTransform[5] * nLines;

		// 按照用户指定的分辨率来计算图像的输出大小以及范围
		nPixels = (int)(((maxX - minX) / dResXSize) + 0.5);
		nLines = (int)(((minY - maxY) / dResYSize) + 0.5);
		adfGeoTransform[0] = minX;
		adfGeoTransform[3] = maxY;
		adfGeoTransform[1] = dResXSize;
		adfGeoTransform[5] = dResYSize;
	}

	// 创建输出图像
	GDALDriverH hDriver = GDALGetDriverByName("GTiff");
	GDALDatasetH hDstDS = GDALCreate(hDriver, pszDstFilename, nPixels, nLines, iBandCount, eDT, NULL);
	GDALSetProjection(hDstDS, pszReferenceWKT);
	GDALSetGeoTransform(hDstDS, adfGeoTransform);

	// 构造GDALWarp的变换选项
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = eDT;
	psWO->eResampleAlg = GRA_NearestNeighbour;

	psWO->hSrcDS = hSrcDS;
	psWO->hDstDS = hDstDS;

	psWO->pfnTransformer = GeoToPixelTransform;
	psWO->pTransformerArg = CreateGeoToPixelTransform(GDALGCPTransform, hTransformArg, adfGeoTransform);

	psWO->nBandCount = iBandCount;
	psWO->panSrcBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	psWO->panDstBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	for (int i = 0; i < iBandCount; i++)
	{
		psWO->panSrcBands[i] = i + 1;
		psWO->panDstBands[i] = i + 1;
	}

	// 创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// 执行处理
	oWO.ChunkAndWarpImage(0, 0, nPixels, nLines);

	// 释放资源和关闭文件
	DestroyGeoToPixelTransform(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);
	GDALDestroyGCPTransformer(hTransformArg);

	GDALClose(hSrcDS);
	GDALClose(hDstDS);
	delete[]gcp_pairs;
	printf("-finish geo correct by GCPs\n");

	return 0;
}
long GeoGCPProcess::GeoProc_GCPWarpTPS(const char* pszSrcFilename, const char* pszDstFilename, const char* pszDstWkt, double* dGroundSize, GDALResampleAlg eResampleMethod, double* GCPs, int gcpNum)
{
	printf("\rstarting TPS correct by GCPs");
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
														// 打开原始图像并计算图像信息
	GDALDatasetH hSrcDS = GDALOpen(pszSrcFilename, GA_ReadOnly);
	GDALDataType eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS, 1));	//获取数据类型
	int iBandCount = GDALGetRasterCount(hSrcDS);

	// 创建TPS坐标转换关系
	GDAL_GCP* gcp_pairs = new GDAL_GCP[gcpNum];
	for (int i = 0; i<gcpNum; i++)
	{
		gcp_pairs[i].pszId = "";
		gcp_pairs[i].pszInfo = "";
		gcp_pairs[i].dfGCPPixel = GCPs[4 * i + 0];
		gcp_pairs[i].dfGCPLine = GCPs[4 * i + 1];
		gcp_pairs[i].dfGCPX = GCPs[4 * i + 2];
		gcp_pairs[i].dfGCPY = GCPs[4 * i + 3];
		gcp_pairs[i].dfGCPZ = 0.0;
	}

	void *hTransformArg = GDALCreateTPSTransformer(gcpNum, gcp_pairs, FALSE);
	if (hTransformArg == NULL)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 使用SuggestedWarpOutput函数计算输出图像四至范围、大小、六参数等信息
	double adfGeoTransform[6];
	double adfExtent[4];
	int    nPixels, nLines;

	if (GDALSuggestedWarpOutput2(hSrcDS, GDALTPSTransform, hTransformArg,
		adfGeoTransform, &nPixels, &nLines, adfExtent, 0) != CE_None)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 下面开始根据用户指定的分辨率来反算输出图像的大小和六参数等信息
	double dResXSize = dGroundSize[0];
	double dResYSize = dGroundSize[1];

	//如果结果投影为投影坐标系统，则将其分辨率与原始影像一致
	if (dResXSize == 0.0 && dResYSize == 0.0)
	{
		double dGeoTrans[6] = { 0 };
		GDALGetGeoTransform(hSrcDS, dGeoTrans);
		dResXSize = ABS(dGeoTrans[1]);
		dResYSize = ABS(dGeoTrans[5]);
	}

	// 如果用户指定了输出图像的分辨率
	if (dResXSize != 0.0 || dResYSize != 0.0)
	{
		// 如果只指定了一个，使用自动计算的结果
		if (dResXSize == 0.0) dResXSize = adfGeoTransform[1];
		if (dResYSize == 0.0) dResYSize = adfGeoTransform[5];

		// 确保分辨率符号正确
		if (dResXSize < 0.0) dResXSize = -dResXSize;
		if (dResYSize > 0.0) dResYSize = -dResYSize;

		// 计算输出图像的范围
		double minX = adfGeoTransform[0];
		double maxX = adfGeoTransform[0] + adfGeoTransform[1] * nPixels;
		double maxY = adfGeoTransform[3];
		double minY = adfGeoTransform[3] + adfGeoTransform[5] * nLines;

		// 按照用户指定的分辨率来计算图像的输出大小以及范围
		nPixels = (int)(((maxX - minX) / dResXSize) + 0.5);
		nLines = (int)(((minY - maxY) / dResYSize) + 0.5);
		adfGeoTransform[0] = minX;
		adfGeoTransform[3] = maxY;
		adfGeoTransform[1] = dResXSize;
		adfGeoTransform[5] = dResYSize;
	}

	// 创建输出图像
	GDALDriverH hDriver = GDALGetDriverByName("GTiff");
	GDALDatasetH hDstDS = GDALCreate(hDriver, pszDstFilename, nPixels, nLines, iBandCount, eDT, NULL);
	GDALSetProjection(hDstDS, pszDstWkt);
	GDALSetGeoTransform(hDstDS, adfGeoTransform);

	// 构造GDALWarp的变换选项
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = eDT;
	psWO->eResampleAlg = GRA_NearestNeighbour;

	psWO->hSrcDS = hSrcDS;
	psWO->hDstDS = hDstDS;

	psWO->pfnTransformer = GeoToPixelTransform;
	psWO->pTransformerArg = CreateGeoToPixelTransform(GDALTPSTransform, hTransformArg, adfGeoTransform);

	psWO->nBandCount = iBandCount;
	psWO->panSrcBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	psWO->panDstBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	for (int i = 0; i < iBandCount; i++)
	{
		psWO->panSrcBands[i] = i + 1;
		psWO->panDstBands[i] = i + 1;
	}

	// 创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// 执行处理
	oWO.ChunkAndWarpImage(0, 0, nPixels, nLines);

	// 释放资源和关闭文件
	DestroyGeoToPixelTransform(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);
	GDALDestroyTPSTransformer(hTransformArg);

	GDALClose(hSrcDS);
	GDALClose(hDstDS);
	delete[]gcp_pairs;

	printf("\rfinishing TPS correct by GCPs\n");
	return 0;
}
long GeoGCPProcess::GeoProc_GCPWarpRPC(const char * pszSrcFile, const char * pszDstFile, GDALRPCInfo *psRPC, double dfPixErrThreshold, char **papszOptions, GDALResampleAlg eResampleMethod, const char * pszFormat)
{
	printf("\rstarting RPC correct by GCPs");
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
														// 打开原始图像并计算图像信息
	GDALDatasetH hSrcDS = GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS, 1));	//获取数据类型
	int iBandCount = GDALGetRasterCount(hSrcDS);

	// 创建RPC坐标转换关系
	void *hTransformArg = GDALCreateRPCTransformer(psRPC, FALSE, dfPixErrThreshold, papszOptions);
	if (hTransformArg == NULL)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 使用SuggestedWarpOutput函数计算输出图像四至范围、大小、六参数等信息
	double adfGeoTransform[6];
	double adfExtent[4];
	int    nPixels, nLines;

	if (GDALSuggestedWarpOutput2(hSrcDS, GDALRPCTransform, hTransformArg,
		adfGeoTransform, &nPixels, &nLines, adfExtent, 0) != CE_None)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 创建输出图像
	GDALDriverH hDriver = GDALGetDriverByName(pszFormat);
	GDALDatasetH hDstDS = GDALCreate(hDriver, pszDstFile, nPixels, nLines, iBandCount, eDT, NULL);
	GDALSetProjection(hDstDS, SRS_WKT_WGS84);	//RPC校正结果是WGS84经纬度坐标系统
	GDALSetGeoTransform(hDstDS, adfGeoTransform);

	// 构造GDALWarp的变换选项
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = eDT;
	psWO->eResampleAlg = GRA_NearestNeighbour;

	psWO->hSrcDS = hSrcDS;
	psWO->hDstDS = hDstDS;

	psWO->pfnTransformer = GeoToPixelTransform;
	psWO->pTransformerArg = CreateGeoToPixelTransform(GDALRPCTransform, hTransformArg, adfGeoTransform);

	psWO->nBandCount = iBandCount;
	psWO->panSrcBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	psWO->panDstBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	for (int i = 0; i < iBandCount; i++)
	{
		psWO->panSrcBands[i] = i + 1;
		psWO->panDstBands[i] = i + 1;
	}

	// 创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// 执行处理
	oWO.ChunkAndWarpImage(0, 0, nPixels, nLines);

	// 释放资源和关闭文件
	DestroyGeoToPixelTransform(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);
	GDALDestroyRPCTransformer(hTransformArg);

	GDALClose(hSrcDS);
	GDALClose(hDstDS);
	printf("\rfinish RPC correct by GCPs\n");
	return 0;
}
long GeoGCPProcess::GeoProc_GCPWarpGeoLoc(const char * pszSrcFile, const char * pszDstFile, char **papszGeoLocOptions, GDALResampleAlg eResampleMethod, const char * pszFormat)
{
	printf("\rstarting GeoLoc correct by GCPs");
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
														// 打开原始图像并计算图像信息
	GDALDatasetH hSrcDS = GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS, 1));	//获取数据类型
	int iBandCount = GDALGetRasterCount(hSrcDS);

	// 创建Geoloc坐标转换关系
	void *hTransformArg = GDALCreateGeoLocTransformer(hSrcDS, papszGeoLocOptions, FALSE);
	if (hTransformArg == NULL)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 使用SuggestedWarpOutput函数计算输出图像四至范围、大小、六参数等信息
	double adfGeoTransform[6];
	double adfExtent[4];
	int    nPixels, nLines;

	if (GDALSuggestedWarpOutput2(hSrcDS, GDALGeoLocTransform, hTransformArg,
		adfGeoTransform, &nPixels, &nLines, adfExtent, 0) != CE_None)
	{
		GDALClose(hSrcDS);
		return -1;
	}

	// 创建输出图像
	GDALDriverH hDriver = GDALGetDriverByName(pszFormat);
	GDALDatasetH hDstDS = GDALCreate(hDriver, pszDstFile, nPixels, nLines, iBandCount, eDT, NULL);
	GDALSetProjection(hDstDS, SRS_WKT_WGS84);	//GeoLoc校正结果是WGS84经纬度坐标系统
	GDALSetGeoTransform(hDstDS, adfGeoTransform);

	// 构造GDALWarp的变换选项
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = eDT;
	psWO->eResampleAlg = GRA_NearestNeighbour;

	psWO->hSrcDS = hSrcDS;
	psWO->hDstDS = hDstDS;

	psWO->pfnTransformer = GeoToPixelTransform;
	psWO->pTransformerArg = CreateGeoToPixelTransform(GDALGeoLocTransform, hTransformArg, adfGeoTransform);

	psWO->nBandCount = iBandCount;
	psWO->panSrcBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	psWO->panDstBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	for (int i = 0; i < iBandCount; i++)
	{
		psWO->panSrcBands[i] = i + 1;
		psWO->panDstBands[i] = i + 1;
	}

	// 创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// 执行处理
	oWO.ChunkAndWarpImage(0, 0, nPixels, nLines);

	// 释放资源和关闭文件
	DestroyGeoToPixelTransform(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);
	GDALDestroyGeoLocTransformer(hTransformArg);

	GDALClose(hSrcDS);
	GDALClose(hDstDS);
	printf("\rfinishing GeoLoc correct by GCPs\n");
	return 0;
}
//#define ABS_FLOAT_0 0.0001 
//double GetTriangleSquar(double* pnt1, double* pnt2, double* pnt3)
//{
//	double tmp1, tmp2, tmp3, tmp4;
//	tmp1 = pnt2[0] - pnt1[0];
//	tmp2 = pnt2[1] - pnt1[1];
//	tmp3 = pnt3[0] - pnt2[0];
//	tmp4 = pnt3[1] - pnt2[1];
//	return fabs((tmp1 * tmp4 - tmp2 * tmp3)) / 2.0f;
//}
//bool  IsInTriangle(double* pnt1, double* pnt2, double* pnt3, double* pnt4)
//{
//	double SABC, SADB, SBDC, SADC;
//	SABC = GetTriangleSquar(pnt1, pnt2, pnt3);
//	SADB = GetTriangleSquar(pnt1, pnt4, pnt2);
//	SBDC = GetTriangleSquar(pnt2, pnt4, pnt3);
//	SADC = GetTriangleSquar(pnt1, pnt4, pnt3);
//
//	double SumSuqar = SADB + SBDC + SADC;
//
//	if ((-ABS_FLOAT_0 < (SABC - SumSuqar)) && ((SABC - SumSuqar) < ABS_FLOAT_0))
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//void  GetTransInvTran(Point2f *pnt1, Point2f* pnt2, int gcps, double *adfGeoTransform, double *adfGeoInvTransform)
//{
//	//解算仿射变换系数
//	Mat geotrans, invgeotrans;
//	geotrans = getAffineTransform(pnt1, pnt2);
//	invgeotrans = getAffineTransform(pnt2, pnt1);
//
//	adfGeoTransform[0] = geotrans.at<double>(0, 0);
//	adfGeoTransform[1] = geotrans.at<double>(0, 1);
//	adfGeoTransform[2] = geotrans.at<double>(0, 2);
//	adfGeoTransform[3] = geotrans.at<double>(1, 0);
//	adfGeoTransform[4] = geotrans.at<double>(1, 1);
//	adfGeoTransform[5] = geotrans.at<double>(1, 2);
//
//	adfGeoInvTransform[0] = invgeotrans.at<double>(0, 0);
//	adfGeoInvTransform[1] = invgeotrans.at<double>(0, 1);
//	adfGeoInvTransform[2] = invgeotrans.at<double>(0, 2);
//	adfGeoInvTransform[3] = invgeotrans.at<double>(1, 0);
//	adfGeoInvTransform[4] = invgeotrans.at<double>(1, 1);
//	adfGeoInvTransform[5] = invgeotrans.at<double>(1, 2);
//}
//long  GDALTool_TriangleDelaunay(double* points, int pointNumber, const char* pathDst)
//{
//	if (points == NULL || pathDst == NULL)
//		return Error_Empty_Data;
//
//	double *padfX = new double[pointNumber];
//	double *padfY = new double[pointNumber];
//	for (int i = 0; i<pointNumber; i++)
//	{
//		padfX[i] = points[2 * i + 0];
//		padfY[i] = points[2 * i + 1];
//	}
//
//	//构建三角网
//	if (GDALHasTriangulation() != TRUE)
//		return -1;
//	GDALTriangulation *pdfTriangle = GDALTriangulationCreateDelaunay(pointNumber, padfX, padfY);
//	GDALTriangulationComputeBarycentricCoefficients(pdfTriangle, padfX, padfY);
//	//保存到文件中将三角网
//	ofstream ofs(pathDst, ios_base::out);
//	if (!ofs.is_open())
//		return -1;
//
//	ofs << "POINTS\n";
//	ofs << "point numbers of Tin:\n" << pointNumber << endl;
//	for (int i = 0; i<pointNumber; i++)
//		ofs << i + 1 << setw(15) << padfX[i] << setw(15) << padfY[i] << endl;
//	ofs << "TRIANGLES\n";
//	ofs << "triangle number:\n" << pdfTriangle->nFacets << endl;
//	for (int i = 0; i<pdfTriangle->nFacets; i++)
//		ofs << pdfTriangle->pasFacets[i].anVertexIdx[0] << setw(5) << pdfTriangle->pasFacets[i].anVertexIdx[1] << setw(5) << pdfTriangle->pasFacets[i].anVertexIdx[2] << setw(5) <<
//		pdfTriangle->pasFacets[i].anNeighborIdx[0] << setw(5) << pdfTriangle->pasFacets[i].anNeighborIdx[1] << setw(5) << pdfTriangle->pasFacets[i].anNeighborIdx[2] << setw(15) <<
//		pdfTriangle->pasFacetCoefficients[i].dfCstX << setw(15) << pdfTriangle->pasFacetCoefficients[i].dfCstY << setw(15) <<
//		pdfTriangle->pasFacetCoefficients[i].dfMul1X << setw(15) << pdfTriangle->pasFacetCoefficients[i].dfMul1Y << setw(15) <<
//		pdfTriangle->pasFacetCoefficients[i].dfMul2X << setw(15) << pdfTriangle->pasFacetCoefficients[i].dfMul2Y << endl;
//	ofs << "END TIN";
//	ofs.close();
//
//	GDALTriangulationFree(pdfTriangle);
//	delete[]padfX;
//	delete[]padfY;
//	return 0;
//}
//GDALTriangulation* GDALTool_LoadTriangle(const char* pathTri, double* pdfTriVex, int &number)
//{
//	GDALTriangulation* m_triangle = NULL;
//	GDALTriFacet *m_facets = NULL;
//	GDALTriBarycentricCoefficients* m_barycentricCoefficients = NULL;
//
//	int triNumber, nfaces;
//	char cnstTmpchar[2048];
//	ifstream ifs(pathTri, ios_base::in);
//	ifs.getline(cnstTmpchar, 2048);
//	ifs.getline(cnstTmpchar, 2048);
//	ifs.getline(cnstTmpchar, 2048);
//	triNumber = atoi(cnstTmpchar);
//	number = triNumber;
//
//	for (int i = 0; i<triNumber; i++)
//	{
//		ifs.getline(cnstTmpchar, 2048);
//		int indexnum;
//		sscanf(cnstTmpchar, "%d%lf%lf", &indexnum, &pdfTriVex[2 * i + 0], &pdfTriVex[2 * i + 1]);
//	}
//
//	ifs.getline(cnstTmpchar, 2048);
//	ifs.getline(cnstTmpchar, 2048);
//	ifs.getline(cnstTmpchar, 2048);
//	nfaces = atoi(cnstTmpchar);
//
//	m_triangle = (GDALTriangulation*)CPLCalloc(1, sizeof(GDALTriangulation));
//	m_facets = (GDALTriFacet*)VSIMalloc2(nfaces, sizeof(GDALTriFacet));
//	m_barycentricCoefficients = (GDALTriBarycentricCoefficients*)VSIMalloc2(nfaces, sizeof(GDALTriBarycentricCoefficients));
//
//	m_triangle->nFacets = nfaces;
//	m_triangle->pasFacetCoefficients = m_barycentricCoefficients;
//	m_triangle->pasFacets = m_facets;
//
//	for (int i = 0; i<nfaces; i++)
//	{
//		ifs.getline(cnstTmpchar, 2048);
//		sscanf(cnstTmpchar, "%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf", &m_triangle->pasFacets[i].anVertexIdx[0], &m_triangle->pasFacets[i].anVertexIdx[1], &m_triangle->pasFacets[i].anVertexIdx[2],
//			&m_triangle->pasFacets[i].anNeighborIdx[0], &m_triangle->pasFacets[i].anNeighborIdx[1], &m_triangle->pasFacets[i].anNeighborIdx[2],
//			&m_triangle->pasFacetCoefficients->dfCstX, &m_triangle->pasFacetCoefficients->dfCstY, &m_triangle->pasFacetCoefficients->dfMul1X, &m_triangle->pasFacetCoefficients->dfMul1Y,
//			&m_triangle->pasFacetCoefficients->dfMul2X, &m_triangle->pasFacetCoefficients->dfMul2Y);
//
//	}
//	ifs.close();
//	return m_triangle;
//}
////重采样函数-双线性插值
//long  ReSampling2(float *pImgBuffer, DPOINT *pPositions, int nImgWidth, int nLines, int nReImgWidth, int nReLines, float *pRegBuffer, float *fDGrey, float *fDItem)
//{
//	memset(fDGrey, 0, nReImgWidth * nReLines*sizeof(float));
//	memset(fDItem, 0, nReImgWidth * nReLines*sizeof(float));
//
//	for (long ns = 0; ns<nReImgWidth*nReLines; ns++)
//		pRegBuffer[ns] = 0.0;
//
//	int i = 0, j = 0;
//	int nC = 0, nY = 0;
//	double fDX = 0, fDY = 0;
//
//	unsigned long nOffset = 0;
//
//	for (i = 0; i<nLines; i++)
//	{
//		for (j = 0; j<nImgWidth; j++)
//		{
//			nC = (int)pPositions[i*nImgWidth + j].dX;
//			nY = (int)pPositions[i*nImgWidth + j].dY;       //dddd
//
//			fDX = pPositions[i*nImgWidth + j].dX - nC;
//			fDY = pPositions[i*nImgWidth + j].dY - nY;
//
//			float fDN = pImgBuffer[i*nImgWidth + j];
//
//			float fTempGrey[4];
//			memset(fTempGrey, 0, 4 * sizeof(float));
//			fTempGrey[0] = float((1 - fDX)*(1 - fDY)*fDN);
//			fTempGrey[1] = float(fDX*(1 - fDY)*fDN);
//			fTempGrey[2] = float((1 - fDX)*fDY*fDN);
//			fTempGrey[3] = float(fDX*fDY*fDN);
//
//			if (nC >= 0 && nC<nReImgWidth && nY >= 0 && nY<nReLines)
//			{
//				nOffset = nY*nReImgWidth + nC;
//				fDGrey[nOffset] += fTempGrey[0];
//				fDItem[nOffset] += float((1 - fDX)*(1 - fDY));
//				if (nC < nReImgWidth - 1)
//				{
//					fDGrey[nOffset + 1] += fTempGrey[1];
//					fDItem[nOffset + 1] += float(fDX*(1 - fDY));
//				}
//				if (nY < nReLines - 1)
//				{
//					fDGrey[nOffset + nReImgWidth] += fTempGrey[2];
//					fDItem[nOffset + nReImgWidth] += float((1 - fDX)*fDY);
//				}
//				if (nC<nReImgWidth - 1 && nY<nReLines - 1)
//				{
//					fDGrey[nOffset + nReImgWidth + 1] += fTempGrey[3];
//					fDItem[nOffset + nReImgWidth + 1] += float(fDX*fDY);
//				}
//			}
//		}
//	}
//
//	float fSumValues = 0;
//	float fSumDItems = 0;
//	int   nCount = 0;
//	int sss = 0;
//
//	for (i = 0; i<nReLines; i++)
//	{
//		for (j = 0; j<nReImgWidth; j++)
//		{
//			nOffset = i*nReImgWidth + j;
//			if (fDItem[nOffset] != 0)
//			{
//				pRegBuffer[nOffset] = fDGrey[nOffset] / fDItem[nOffset];
//			}
//			else
//			{
//				if (i>0 && i<nReLines - 1 && j>0 && j<nReImgWidth - 1)
//				{
//					fSumValues = 0;
//					fSumDItems = 0;
//					nCount = 0;
//
//					if (fDItem[nOffset - nReImgWidth - 1] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset - nReImgWidth - 1] / fDItem[nOffset - nReImgWidth - 1];//
//						fSumDItems += fDItem[nOffset - nReImgWidth - 1];
//					}
//					if (fDItem[nOffset - nReImgWidth] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset - nReImgWidth] / fDItem[nOffset - nReImgWidth];
//						fSumDItems += fDItem[nOffset - nReImgWidth];
//					}
//					if (fDItem[nOffset - nReImgWidth + 1] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset - nReImgWidth + 1] / fDItem[nOffset - nReImgWidth + 1];
//						fSumDItems += fDItem[nOffset - nReImgWidth + 1];
//					}
//					if (fDItem[nOffset - 1] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset - 1] / fDItem[nOffset - 1];
//						fSumDItems += fDItem[nOffset - 1];
//					}
//					if (fDItem[nOffset + 1] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset + 1] / fDItem[nOffset + 1];
//						fSumDItems += fDItem[nOffset + 1];
//					}
//					if (fDItem[nOffset + nReImgWidth - 1] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset + nReImgWidth - 1] / fDItem[nOffset + nReImgWidth - 1];
//						fSumDItems += fDItem[nOffset + nReImgWidth - 1];
//					}
//					if (fDItem[nOffset + nReImgWidth] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset + nReImgWidth] / fDItem[nOffset + nReImgWidth];
//						fSumDItems += fDItem[nOffset + nReImgWidth];
//					}
//					if (fDItem[nOffset + nReImgWidth + 1] != 0)
//					{
//						nCount++;
//						fSumValues += fDGrey[nOffset + nReImgWidth + 1] / fDItem[nOffset + nReImgWidth + 1];
//						fSumDItems += fDItem[nOffset + nReImgWidth + 1];
//					}
//
//					if (nCount >= 1)
//					{
//						//	if( fSumDItems>0.5 )
//						pRegBuffer[nOffset] = fSumValues / nCount;
//						sss++;
//					}
//
//				}
//			}
//		}
//	}
//
//
//	return 0;
//}
////RST重采样函数
//long  GeoGCPProcess::GeoProc_GCPTriangle(char* pathSrcImg, char* pathDstImg, char* pathTri, double* GCPs, int gcpNum, int UTMZone, double GroundSize)
//{
//	long			IError = 0;
//	GDALDatasetH	m_datasetDst, m_datasetSrc;
//	float			*pdfDstImgData = NULL, *pdfSrcImgData = NULL, *fDGrey = NULL, *fDItem = NULL;
//	double			**facetadfGeoTrans = NULL, **facetadfGeoInvTrans = NULL, *padfXY = NULL, *padGeoXY = NULL;
//	double			maxx, minx, maxy, miny;
//	double			maxxGeo, minxGeo, maxyGeo, minyGeo;
//	double			pntGeoLT[2], pntGeoLD[2], pntGeoRT[2], pntGeoRD[2];
//	double			adfTrans[6];
//	int				*pdfTriangleImg = NULL;
//	char			*temp_tri = "~tri_temp.tri";
//	int				bands, xsize, ysize, xsizetrans, ysizetrans, RealGCPNum = 0;
//	char			*pszDstWkt;
//	DPOINT			*pPositions = NULL;
//	OGRSpatialReference oSRS;
//	GDALTriangulation	*m_triangle;
//
//	//1-打开影像,判断控制点是否正确//////////////////////////////////////////////////////
//	//a-打开影像
//	if ((_access(pathDstImg, 0)) == 0)
//		remove(pathDstImg);
//	if (gcpNum<3)
//	{
//		cout << "输入控制点数量少于3个";
//		IError = 1;
//		goto ErrEnd;
//	}
//	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//中文路径
//	GDALAllRegister();
//	m_datasetSrc = GDALOpen(pathSrcImg, GA_ReadOnly);
//	if (m_datasetSrc == NULL)
//	{
//		cout << "待校正影像打开错误";
//		IError = 2;
//		goto ErrEnd;
//	}
//	//b-读取数据信息
//	bands = GDALGetRasterCount(m_datasetSrc);
//	xsize = GDALGetRasterXSize(m_datasetSrc);
//	ysize = GDALGetRasterYSize(m_datasetSrc);
//	//pszSrcWKT = GDALGetProjectionRef(m_datasetSrc);
//	oSRS.SetUTM(UTMZone, TRUE);
//	oSRS.SetWellKnownGeogCS("WGS84");
//	oSRS.exportToWkt(&pszDstWkt);
//
//	//c-获取控制点在影像上的范围(采用影像坐标)
//	padfXY = new double[2 * gcpNum];
//	padGeoXY = new double[2 * gcpNum];
//	for (int i = 0; i<gcpNum; i++)
//	{
//		if ((GCPs[4 * i + 0]<0) || (GCPs[4 * i + 0] >= xsize))
//			continue;
//		if ((GCPs[4 * i + 1]<0) || (GCPs[4 * i + 1] >= ysize))
//			continue;
//		padfXY[2 * RealGCPNum + 0] = GCPs[4 * i + 0];
//		padfXY[2 * RealGCPNum + 1] = GCPs[4 * i + 1];
//		padGeoXY[2 * RealGCPNum + 0] = GCPs[4 * i + 2];
//		padGeoXY[2 * RealGCPNum + 1] = GCPs[4 * i + 3];
//		RealGCPNum++;
//
//	}
//	maxx = padfXY[0], minx = padfXY[0], maxy = padfXY[1], miny = padfXY[1];
//	maxxGeo = padGeoXY[0], minxGeo = padGeoXY[0], maxyGeo = padGeoXY[1], minyGeo = padGeoXY[1];
//	for (int i = 0; i<gcpNum; i++)
//	{
//		maxx = max(maxx, padfXY[2 * i + 0]);
//		minx = min(minx, padfXY[2 * i + 0]);
//		maxy = max(maxy, padfXY[2 * i + 1]);
//		miny = min(miny, padfXY[2 * i + 1]);
//
//		maxxGeo = max(maxxGeo, padGeoXY[2 * i + 0]);
//		minxGeo = min(maxxGeo, padGeoXY[2 * i + 0]);
//		maxyGeo = max(maxyGeo, padGeoXY[2 * i + 1]);
//		minyGeo = min(minyGeo, padGeoXY[2 * i + 1]);
//	}
//	if (RealGCPNum<3)
//	{
//		cout << "有效控制点数量少于3个";
//		IError = 3;
//		goto ErrEnd;
//	}
//	//xsize=(int)maxx-(int)minx;
//	//ysize=(int)maxy-(int)miny;
//
//	//2-建立三角网///////////////////////////////////////////////////////////////////////
//	GDALTool_TriangleDelaunay(padfXY, RealGCPNum, temp_tri);
//	m_triangle = GDALTool_LoadTriangle(temp_tri, padfXY, RealGCPNum);
//	if (m_triangle->nFacets<1)
//	{
//		cout << "构网错误";
//		IError = 4;
//		goto ErrEnd;
//	}
//
//	//3-控制点在影像上的位置////////////////////////////////////////////////////////////
//	//a-首先获取仿射变换参数
//	Point2f *pnt1 = new Point2f[RealGCPNum];
//	Point2f *pnt2 = new Point2f[RealGCPNum];
//	double adfGeoTrans[6], adfGeoInvTrans[6];
//	for (int i = 0; i<RealGCPNum; i++)
//	{
//		pnt1[i].x = float(GCPs[4 * i + 0])/*-minx*/;
//		pnt1[i].y = float(GCPs[4 * i + 1])/*-miny*/;
//		pnt2[i].x = float(GCPs[4 * i + 2]);
//		pnt2[i].y = float(GCPs[4 * i + 3]);
//	}
//	GetTransInvTran(pnt1, pnt2, RealGCPNum, adfGeoTrans, adfGeoInvTrans);
//	delete[]pnt1;
//	delete[]pnt2;
//
//	//b-影像四个脚点的坐标
//	pntGeoLT[0] = adfGeoTrans[2];
//	pntGeoLT[1] = adfGeoTrans[5];
//	pntGeoLD[0] = ysize*adfGeoTrans[1] + adfGeoTrans[2];
//	pntGeoLD[1] = ysize*adfGeoTrans[4] + adfGeoTrans[5];
//	pntGeoRT[0] = xsize*adfGeoTrans[0] + adfGeoTrans[2];
//	pntGeoRT[1] = xsize*adfGeoTrans[3] + adfGeoTrans[5];
//	pntGeoRD[0] = xsize*adfGeoTrans[0] + ysize*adfGeoTrans[1] + adfGeoTrans[2];
//	pntGeoRD[1] = xsize*adfGeoTrans[3] + ysize*adfGeoTrans[4] + adfGeoTrans[5];
//	xsizetrans = abs(max(max(pntGeoLT[0], pntGeoRD[0]), max(pntGeoLD[0], pntGeoRT[0])) - min(min(pntGeoLT[0], pntGeoRD[0]), min(pntGeoLD[0], pntGeoRT[0]))) / abs(GroundSize);
//	ysizetrans = abs(max(max(pntGeoLT[1], pntGeoRD[1]), max(pntGeoLD[1], pntGeoRT[1])) - min(min(pntGeoLT[1], pntGeoRD[1]), min(pntGeoLD[1], pntGeoRT[1]))) / abs(GroundSize);
//
//	//c-投影系数
//	adfTrans[0] = min(min(pntGeoLT[0], pntGeoRD[0]), min(pntGeoLD[0], pntGeoRT[0]));
//	adfTrans[1] = GroundSize;
//	adfTrans[2] = 0;
//	adfTrans[3] = max(max(pntGeoLT[1], pntGeoRD[1]), max(pntGeoLD[1], pntGeoRT[1]));
//	adfTrans[4] = 0;
//	adfTrans[5] = -GroundSize;
//
//	//d-三角剖分影像
//	pdfTriangleImg = new int[xsize*ysize];
//	memset(pdfTriangleImg, 0, sizeof(int)*xsize*ysize);
//	for (int i = 0; i<xsize; i++)
//	{
//		for (int j = 0; j<ysize; j++)
//		{
//			double curPnt[2] = { i,j };
//			for (int k = 0; k<m_triangle->nFacets; k++)
//			{
//				if (IsInTriangle(&padfXY[2 * m_triangle->pasFacets[k].anVertexIdx[0]],
//					&padfXY[2 * m_triangle->pasFacets[k].anVertexIdx[1]],
//					&padfXY[2 * m_triangle->pasFacets[k].anVertexIdx[2]], curPnt))
//				{
//					pdfTriangleImg[j*xsize + i] = k + 1;
//					break;
//				}
//			}
//		}
//	}
//
//	//e-每一个三角网内的仿射变换系数
//	facetadfGeoTrans = new double *[m_triangle->nFacets];
//	facetadfGeoInvTrans = new double *[m_triangle->nFacets];
//	Point2f pnts1[3], pnts2[3];
//	for (int i = 0; i<m_triangle->nFacets; i++)
//	{
//		facetadfGeoTrans[i] = new double[6];
//		facetadfGeoInvTrans[i] = new double[6];
//		for (int j = 0; j<3; j++)
//		{
//			pnts1[j].x = padfXY[2 * m_triangle->pasFacets[i].anVertexIdx[j] + 0];
//			pnts1[j].y = padfXY[2 * m_triangle->pasFacets[i].anVertexIdx[j] + 1];
//
//			pnts2[j].x = padGeoXY[2 * m_triangle->pasFacets[i].anVertexIdx[j] + 0];
//			pnts2[j].y = padGeoXY[2 * m_triangle->pasFacets[i].anVertexIdx[j] + 1];
//		}
//		GetTransInvTran(pnts1, pnts2, 3, facetadfGeoTrans[i], facetadfGeoInvTrans[i]);
//	}
//
//	//f-三角剖分影像输出
//	if (strlen(pathTri)>5)
//	{
//		GDALDatasetH m_data_tri = GDALCreate(GDALGetDriverByName("GTiff"), pathTri, xsize, ysize, 1, GDT_Int32, NULL);//三角网的位置
//		GDALRasterIO(GDALGetRasterBand(m_data_tri, 1), GF_Write, 0, 0, xsize, ysize, pdfTriangleImg, xsize, ysize, GDT_Int32, 0, 0);
//		GDALClose(m_data_tri);
//	}
//
//	//g-计算原始图像到结果图像的像元映射
//	pPositions = new DPOINT[xsize*ysize];
//	double imgPnt[2];
//	for (int i = 0; i<xsize; i++)
//	{
//		for (int j = 0; j<ysize; j++)
//		{
//			int triIndex = pdfTriangleImg[j*xsize + i] - 1;
//			if (triIndex<0)
//				continue;
//
//			imgPnt[0] = facetadfGeoTrans[triIndex][0] * i + facetadfGeoTrans[triIndex][1] * j + facetadfGeoTrans[triIndex][2];
//			imgPnt[1] = facetadfGeoTrans[triIndex][3] * i + facetadfGeoTrans[triIndex][4] * j + facetadfGeoTrans[triIndex][5];
//
//			imgPnt[0] = (imgPnt[0] - adfTrans[0]) / adfTrans[1];
//			imgPnt[1] = (imgPnt[1] - adfTrans[3]) / adfTrans[5];
//			if (imgPnt[0]<0 || imgPnt[0] >= xsizetrans || imgPnt[1] >= ysizetrans || imgPnt[1]<0)
//			{
//				pPositions[j*xsize + i].dX = -3;
//				pPositions[j*xsize + i].dX = -3;
//			}
//			else
//			{
//				pPositions[j*xsize + i].dX = imgPnt[0];
//				pPositions[j*xsize + i].dY = imgPnt[1];
//			}
//		}
//	}
//
//	//4-重采样影像////////////////////////////////////////////////////////////////////
//	pdfSrcImgData = new float[xsize*ysize];
//	pdfDstImgData = new float[xsizetrans*ysizetrans];
//	fDGrey = new float[xsizetrans*ysizetrans];
//	fDItem = new float[xsizetrans*ysizetrans];
//	m_datasetDst = GDALCreate(GDALGetDriverByName("GTiff"), pathDstImg, xsizetrans, ysizetrans, bands, GDT_Float32, NULL);
//	for (int bn = 1; bn <= bands; bn++)
//	{
//		GDALRasterIO(GDALGetRasterBand(m_datasetSrc, bn), GF_Read, 0, 0, xsize, ysize, pdfSrcImgData, xsize, ysize, GDT_Float32, 0, 0);
//		ReSampling2(pdfSrcImgData, pPositions, xsize, ysize, xsizetrans, ysizetrans, pdfDstImgData, fDGrey, fDItem);
//		GDALRasterIO(GDALGetRasterBand(m_datasetDst, bn), GF_Write, 0, 0, xsizetrans, ysizetrans, pdfDstImgData, xsizetrans, ysizetrans, GDT_Float32, 0, 0);
//	}
//	GDALSetGeoTransform(m_datasetDst, adfTrans);
//	GDALSetProjection(m_datasetDst, pszDstWkt);
//
//ErrEnd:
//	if (m_datasetDst)	GDALClose(m_datasetDst);
//	if (m_datasetSrc)	GDALClose(m_datasetSrc);
//	if (pdfDstImgData)	delete[]pdfDstImgData;
//	if (pdfSrcImgData)	delete[]pdfSrcImgData;
//	if (padfXY)			delete[]padfXY;
//	if (padGeoXY)		delete[]padGeoXY;
//	if (facetadfGeoInvTrans)
//	{
//		for (int i = 0; i<m_triangle->nFacets; i++)
//			delete[]facetadfGeoInvTrans[i];
//		delete[]facetadfGeoInvTrans;
//	}
//	if (facetadfGeoTrans)
//	{
//		for (int i = 0; i<m_triangle->nFacets; i++)
//			delete[]facetadfGeoTrans[i];;
//		delete[]facetadfGeoTrans;
//	}
//	if ((_access(temp_tri, 0)) == 0)
//		remove(temp_tri);
//
//	if (pdfTriangleImg)	delete[]pdfTriangleImg;
//	if (m_triangle)		GDALTriangulationFree(m_triangle);
//	if (fDGrey)			delete[]fDGrey;
//	if (fDItem)			delete[]fDItem;
//	if (pPositions)		delete[]pPositions;
//
//	return IError;
//}
long  GeoGCPProcess::GeoProc_GetENVIGcp(const char* pathGCP, double *gcps, int num, int headline/* =0 */)
{
	ifstream ifs(pathGCP, ios_base::in);
	if (!ifs.is_open())
		return -1;

	char tmpchr[2048];
	for (int i = 0; i<headline; ++i)
		ifs.getline(tmpchr, 2048); //eat head lines
	int iteratornum = 0;
	do
	{
		ifs.getline(tmpchr, 2048);
		if (tmpchr == "")
			continue;
		sscanf(tmpchr, "%lf%lf%lf%lf", &gcps[4 * iteratornum + 2], &gcps[4 * iteratornum + 3], &gcps[4 * iteratornum + 0], &gcps[4 * iteratornum + 1]);
		iteratornum++;
	} while (!ifs.eof());
	return 0;
}
