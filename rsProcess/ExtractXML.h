#pragma once

#include"rapidxml\rapidxml.hpp"
#include"rapidxml\rapidxml_iterators.hpp"
#include"rapidxml\rapidxml_print.hpp"
#include"rapidxml\rapidxml_utils.hpp"

#include<vector>
#include<string>
using namespace std;
using namespace rapidxml;

//订单文件解析
/*
	订单文件解析，通过解析不同的订单文件
	1.数据预处理订单文件解析
	2.预处理数据元数据解析
	3.0级数据产品订单文件解析
	4.0级数据元数据解析
	5.1级数据产品订单文件解析
	6.2级数据产品订单文件解析
*/
class ExtractQPDProduct 
{
	/*****************************************************************************
	* @brief : 全谱段数据处理数据预处理订单文件解析函数
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.1
	* @inparam : pCatalogOF	订单文件
	//            pRAWData		原始图像数据
	//			  pDarkData     暗电流数据
	//			  pD0Data		0级数据
	//			  pD0XML		XML元文件
	//			  pD0JPG		0级快视图
	//			  bSeg			是否分景
	//			  nSLine		分景起始行
	//			  nELine		分景终止行
	//			  nALine		自动分景行数
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	*****************************************************************************/
	long ExtractLoadPreProductXML(char *pCatalogOF, vector<string> &pRawData, vector<string> &pDarkData, vector<string> &pD0Data, 
							  vector<string> &pD0Dark, vector<string> &pD0XML, vector<string> &pD0JPG,bool &bSeg, int &nSLine, 
							  int &nELine, int &nALine, bool &bQuickView, bool &bGrey, float &fScale, int nUVBand[], int nVNIRBand[],
							  int nSWIRBand[], int nTIHBand[], string &pProFile);

	/*****************************************************************************
	* @brief : 全谱段数据处理数据预处理订单文件解析函数
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.1
	* @inparam : pCatalogOF	订单文件
	//            pRAWData		原始图像数据
	//			  pDarkData     暗电流数据
	//			  pD0Data		0级数据
	//			  pD0XML		XML元文件
	//			  pD0JPG		0级快视图
	//			  bSeg			是否分景
	//			  nSLine		分景起始行
	//			  nELine		分景终止行
	//			  nALine		自动分景行数
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	*****************************************************************************/
	long ExtractSavePreProductXML(char *pCatalogOF, vector<string> &pRawData, vector<string> &pDarkData, vector<string> &pD0Data, vector<string> &pD0Dark, 
								  vector<string> &pD0XML, vector<string> &pD0JPG,bool &bSeg, int &nSLine, int &nELine, int &nALine, bool &bQuickView, 
								  bool &bGrey, float &fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile);

	/*****************************************************************************
	* @brief : 全谱段数据处理原始数据元数据读取
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.0
	* @inparam : pP0MetaFile	0级数据元文件
	//            nSensorType	传感器类型
	//			  nSensorOrder  传感器顺序
	//			  nSamples		图像宽度
	//			  nLines		行数
	//			  nBands		波段数
	//			  nHeadOffset	头偏移
	*****************************************************************************/
	long ExtractLoadPreProductMeta(char *pP0MetaFile, unsigned int &nSensorType, unsigned int &nSensorOrder, unsigned int &nSamples, unsigned int &nLines, unsigned int &nBands, __int64 &nHeadOffset);

	/*****************************************************************************
	* @brief : 全谱段数据处理原始数据元数据写入
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.1
	* @inparam : pP0MetaFile	0级产品元文件
	//            nSensorType	传感器类型
	//			  nSensorOrder  传感器顺序
	//			  nSamples		图像宽度
	//			  nLines		行数
	//			  nBands		波段数
	//			  nHeadOffset	头偏移
	*****************************************************************************/
	long ExtractSavePreProductMeta(char *pD0MetaFile, int nSensorType, int	nSensorOrder, int nWidths, int nSamples, int nLines, int nBands, int nYear, 
														int nMonth,int	nDay, int nSecond, __int64 nHeadOffset, __int64 nEofOffset, bool bDark, int nDarkLine);
	//======================================================================================================================================================================================================

	/*****************************************************************************
	* @brief : 读取0级数据处理订单文件
	* @author : W.W.Frank
	* @date : 2016/3/18 19:43
	* @version : version 1.0
	* @inparam : pD0Data		0级数据
	//			  pD0Dark       暗电流数据
	//			  pD0XML		0级数据元文件
	//			  pP0Data		0级产品
	//			  pP0XML		0级产品元文件
	//			  pP0JPG		0级产品快视图
	//			  pEvent		Event文件
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	*****************************************************************************/
	long ExtractLoadP0ProductXML(char *pP0OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
								vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pEvent,bool bQuickView, 
								bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile);

	/*****************************************************************************
	* @brief : 将0级数据处理订单写入
	* @author : W.W.Frank
	* @date : 2016/3/18 19:48
	* @version : version 1.0
	* @inparam : pP0OF	订单文件
	//			  pD0Data		0级数据
	//			  pD0Dark       暗电流数据
	//			  pD0XML		0级数据元文件
	//			  pP0Data		0级产品
	//			  pP0XML		0级产品元文件
	//			  pP0JPG		0级产品快视图
	//			  pEvent		Event文件
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	* @outparam :
	*****************************************************************************/
	long ExtractSaveP0ProductXML(char *pP0OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
								vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pEvent,bool bQuickView, 
								bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile);

	/*****************************************************************************
	* @brief : 读取0级产品元数据
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.1
	* @inparam : pP0MetaFile	0级数据元文件
	//            nSensorType	传感器类型
	//			  nSensorOrder  传感器顺序
	//			  nSamples		图像宽度
	//			  nLines		行数
	//			  nBands		波段数
	//			  nHeadOffset	头偏移
	*****************************************************************************/
	long ExtractLoadP0ProductMeta(char *pP0MetaFile, unsigned int &nSensorType, unsigned int &nSensorOrder, unsigned int &nSamples, unsigned int &nLines, unsigned int &nBands, __int64 &nHeadOffset);

	/*****************************************************************************
	* @brief : 全谱段数据处理0级产品订单文件解析
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.1
	* @inparam : pP0MetaFile	0级产品元文件
	//            nSensorType	传感器类型
	//			  nSensorOrder  传感器顺序
	//			  nSamples		图像宽度
	//			  nLines		行数
	//			  nBands		波段数
	//			  nHeadOffset	头偏移
	* @outparam :
	*****************************************************************************/
	long ExtractSaveP0ProductMeta(char *pP0MetaFile, int nSensorType, int nSensorOrder, int nSamples, int nLines, int nBands, __int64 nHeadOffset);

	//======================================================================================================================================================================================================
	/*****************************************************************************
	* @brief : 全谱段影像处理1级数据产品订单文件解析
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.0
	* @inparam : pP1OF			订单文件
	//			  pD0Data		0级数据
	//			  pD0Dark       暗电流数据
	//			  pD0XML		0级数据元文件
	//			  nP0			0级产品生产类型  0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1A			1A级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1B			1B级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1C			1C级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1D			1D级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1E			1E级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  pP0Data		0级产品
	//			  pP0XML		0级产品元文件
	//			  pP0JPG		0级产品快视图
	//			  pP1AData		0级产品
	//			  pP1AXML		0级产品元文件
	//			  pP1AJPG		0级产品快视图
	//			  pP1BData		0级产品
	//			  pP1BXML		0级产品元文件
	//			  pP1BJPG		0级产品快视图
	//			  pP1CData		0级产品
	//			  pP1CXML		0级产品元文件
	//			  pP1CJPG		0级产品快视图
	//			  pP1DData		0级产品
	//			  pP1DXML		0级产品元文件
	//			  pP1DJPG		0级产品快视图
	//			  pP1EData		0级产品
	//			  pP1EXML		0级产品元文件
	//			  pP1EJPG		0级产品快视图
	//			  pReleCof		相对辐射校正系数
	//			  pAbCof		绝对辐射校正系数
	//			  pModtran		辐亮度文件
	//			  pWaveLen		实验室辐射定标结果
	//			  pEvent		Event文件
	//			  fFOLapX		视场重叠X,8×1
	//			  fFOLapY		视场重叠Y,8×1
	//			  fSOLapX		谱段重叠X,3×1
	//			  fSOLapY		谱段重叠Y,3×1
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	* @outparam :
	*****************************************************************************/
	long ExtractLoadP1ProductXML(char *pP1OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
								short &nP0, short &nP1A, short &nP1B, short &nP1C, short &nP1D, short &nP1E,
								vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pP1AData, vector<string> &pP1AXML, vector<string> &pP1AJPG,
								vector<string> &pP1BData, vector<string> &pP1BXML, vector<string> &pP1BJPG, vector<string> &pP1CData, vector<string> &pP1CXML, vector<string> &pP1CJPG,
								vector<string> &pP1DData, vector<string> &pP1DXML, vector<string> &pP1DJPG, vector<string> &pP1EData, vector<string> &pP1EXML, vector<string> &pP1EJPG,
								vector<string> &pReleCof, vector<string> &pAbCof, string &pModtran, vector<string> &pWaveLen, vector<string> &pEvent,
								float fFOLapX[], float fFOLapY[], float fSOLapX[], float fSOLapY[],
								bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile);

	/*****************************************************************************
	* @brief : 全谱段数据处理1级数据产品处理订单文件写入
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.0
	* @inparam : pP1OF			订单文件
	//			  pD0Data		0级数据
	//			  pD0Dark       暗电流数据
	//			  pD0XML		0级数据元文件
	//			  nP0			0级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1A			1A级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1B			1B级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1C			1C级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1D			1D级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1E			1E级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  pP0Data		0级产品
	//			  pP0XML		0级产品元文件
	//			  pP0JPG		0级产品快视图
	//			  pP1AData		0级产品
	//			  pP1AXML		0级产品元文件
	//			  pP1AJPG		0级产品快视图
	//			  pP1BData		0级产品
	//			  pP1BXML		0级产品元文件
	//			  pP1BJPG		0级产品快视图
	//			  pP1CData		0级产品
	//			  pP1CXML		0级产品元文件
	//			  pP1CJPG		0级产品快视图
	//			  pP1DData		0级产品
	//			  pP1DXML		0级产品元文件
	//			  pP1DJPG		0级产品快视图
	//			  pP1EData		0级产品
	//			  pP1EXML		0级产品元文件
	//			  pP1EJPG		0级产品快视图
	//			  pReleCof		相对辐射校正系数
	//			  pAbCof		绝对辐射校正系数
	//			  pModtran		辐亮度文件
	//			  pWaveLen		实验室辐射定标结果
	//			  pEvent		Event文件
	//			  fFOLapX		视场重叠X,8×1
	//			  fFOLapY		视场重叠Y,8×1
	//			  fSOLapX		谱段重叠X,3×1
	//			  fSOLapY		谱段重叠Y,3×1
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	*****************************************************************************/
	long ExtractSaveP1ProductXML(char *pP1OF, vector<string> pD0Data, vector<string> pD0XML, vector<string> pD0Dark,
								short nP0, short nP1A, short nP1B, short nP1C, short nP1D, short nP1E,
								vector<string> pP0Data, vector<string> pP0XML, vector<string> pP0JPG, vector<string> pP1AData, vector<string> pP1AXML, vector<string> pP1AJPG,
								vector<string> pP1BData, vector<string> pP1BXML, vector<string> pP1BJPG, vector<string> pP1CData, vector<string> pP1CXML, vector<string> pP1CJPG,
								vector<string> pP1DData, vector<string> pP1DXML, vector<string> pP1DJPG, vector<string> pP1EData, vector<string> pP1EXML, vector<string> pP1EJPG,
								vector<string> pReleCof, vector<string> pAbCof, string pModtran, vector<string> pWaveLen, vector<string> pEvent,
								float fFOLapX[], float fFOLapY[], float fSOLapX[], float fSOLapY[],
								bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string pProFile);

	//======================================================================================================================================================================================================
	/*****************************************************************************
	* @brief : 全谱段数据处理2级数据产品订单文件解析
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.1
	* @inparam : pP1OF			订单文件
	//			  pD0Data		0级数据
	//			  pD0Dark       暗电流数据
	//			  pD0XML		0级数据元文件
	//			  nP0			0级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1A			1A级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1B			1B级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1C			1C级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1D			1D级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1E			1E级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP2A			2A级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP2B			2B级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP2C			2C级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  pP0Data		0级产品
	//			  pP0XML		0级产品元文件
	//			  pP0JPG		0级产品快视图
	//			  pP1AData		1A级产品
	//			  pP1AXML		1A级产品元文件
	//			  pP1AJPG		1A级产品快视图
	//			  pP1BData		1B级产品
	//			  pP1BXML		1B级产品元文件
	//			  pP1BJPG		1B级产品快视图
	//			  pP1CData		1C级产品
	//			  pP1CXML		1C级产品元文件
	//			  pP1CJPG		1C级产品快视图
	//			  pP1DData		1D级产品
	//			  pP1DXML		1D级产品元文件
	//			  pP1DJPG		1D级产品快视图
	//			  pP1EData		1E级产品
	//			  pP1EXML		1E级产品元文件
	//			  pP1EJPG		1E级产品快视图
	//			  pP2AData		2A级产品
	//			  pP2AXML		2A级产品元文件
	//			  pP2AJPG		2A级产品快视图
	//			  pP2BData		2B级产品
	//			  pP2BXML		2B级产品元文件
	//			  pP2BJPG		2B级产品快视图
	//			  pP2CData		2C级产品
	//			  pP2CXML		2C级产品元文件
	//			  pP2CJPG		2C级产品快视图
	//			  pReleCof		相对辐射校正系数
	//			  pAbCof		绝对辐射校正系数
	//			  pModtran		辐亮度文件
	//			  pWaveLen		实验室辐射定标结果
	//			  pEvent		Event文件
	//			  pSbet			SBET文件
	//			  pPOS			POS文件
	//			  pEOF			EOF文件
	//			  pDEM			DEM文件
	//			  fFOLapX		视场重叠X,8×1
	//			  fFOLapY		视场重叠Y,8×1
	//			  fSOLapX		谱段重叠X,3×1
	//			  fSOLapY		谱段重叠Y,3×1
	//			  fFov			总视场,4×1
	//			  fIFov			瞬时视场,8×1
	//			  fFocalLen		焦距,4×1
	//			  dBoresightMis	视准轴偏差,3×1
	//			  dGNSSOffset	位置偏差,3×1
	//			  dXYZOffset	绝对位置偏差,3×1
	//			  nBand			重采样的波段数,4×1
	//			  nSamples		重采样的谱段数,4×1
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	*****************************************************************************/
	long ExtractLoadP2ProductXML(char *pP2OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
								short &nP0, short &nP1A, short &nP1B, short &nP1C, short &nP1D, short &nP1E, short &nP2A, short &nP2B, short &nP2C,
								vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pP1AData, vector<string> &pP1AXML, vector<string> &pP1AJPG,
								vector<string> &pP1BData, vector<string> &pP1BXML, vector<string> &pP1BJPG, vector<string> &pP1CData, vector<string> &pP1CXML, vector<string> &pP1CJPG,
								vector<string> &pP1DData, vector<string> &pP1DXML, vector<string> &pP1DJPG, vector<string> &pP1EData, vector<string> &pP1EXML, vector<string> &pP1EJPG,
								vector<string> &pP2AData, vector<string> &pP2AXML, vector<string> &pP2AJPG,
								vector<string> &pP2BData, vector<string> &pP2BXML, vector<string> &pP2BJPG, vector<string> &pP2CData, vector<string> &pP2CXML, vector<string> &pP2CJPG,
								vector<string> &pReleCof, vector<string> &pAbCof, string &pModtran, vector<string> &pWaveLen, vector<string> &pEvent,
								string &pSbet, vector<string> &pPOS, vector<string> &pEOF, string &pDEM,
								float fFOLapX[], float fFOLapY[], float fSOLapX[], float fSOLapY[],
								float fFov[], float fIFov[], float fFocalLen[],
								double dBoresightMis[], double dGNSSOffset[], double dXYZOffset[], int nBand[], int nSamples[],
								bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile);

	/*****************************************************************************
	* @brief : 全谱段数据处理2级数据产品订单文件保存
	* @author : W.W.Frank
	* @date : 2016/3/30
	* @version : version 1.1
	* @inparam : pP1OF			订单文件
	//			  pD0Data		0级数据
	//			  pD0Dark       暗电流数据
	//			  pD0XML		0级数据元文件
	//			  nP0			0级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1A			1A级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1B			1B级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1C			1C级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1D			1D级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP1E			1E级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP2A			2A级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP2B			2B级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  nP2C			2C级产品生产类型 0：不进行处理，1：进行处理不保留结果，2：处理并保留结果,3：处理，保留并生成快视图
	//			  pP0Data		0级产品
	//			  pP0XML		0级产品元文件
	//			  pP0JPG		0级产品快视图
	//			  pP1AData		1A级产品
	//			  pP1AXML		1A级产品元文件
	//			  pP1AJPG		1A级产品快视图
	//			  pP1BData		1B级产品
	//			  pP1BXML		1B级产品元文件
	//			  pP1BJPG		1B级产品快视图
	//			  pP1CData		1C级产品
	//			  pP1CXML		1C级产品元文件
	//			  pP1CJPG		1C级产品快视图
	//			  pP1DData		1D级产品
	//			  pP1DXML		1D级产品元文件
	//			  pP1DJPG		1D级产品快视图
	//			  pP1EData		1E级产品
	//			  pP1EXML		1E级产品元文件
	//			  pP1EJPG		1E级产品快视图
	//			  pP2AData		2A级产品
	//			  pP2AXML		2A级产品元文件
	//			  pP2AJPG		2A级产品快视图
	//			  pP2BData		2B级产品
	//			  pP2BXML		2B级产品元文件
	//			  pP2BJPG		2B级产品快视图
	//			  pP2CData		2C级产品
	//			  pP2CXML		2C级产品元文件
	//			  pP2CJPG		2C级产品快视图
	//			  pReleCof		相对辐射校正系数
	//			  pAbCof		绝对辐射校正系数
	//			  pModtran		辐亮度文件
	//			  pWaveLen		实验室辐射定标结果
	//			  pEvent		Event文件
	//			  pSbet			SBET文件
	//			  pPOS			POS文件
	//			  pEOF			EOF文件
	//			  pDEM			DEM文件
	//			  fFOLapX		视场重叠X,8×1
	//			  fFOLapY		视场重叠Y,8×1
	//			  fSOLapX		谱段重叠X,3×1
	//			  fSOLapY		谱段重叠Y,3×1
	//			  fFov			总视场,4×1
	//			  fIFov			瞬时视场,8×1
	//			  fFocalLen		焦距,4×1
	//			  dBoresightMis	视准轴偏差,3×1
	//			  dGNSSOffset	位置偏差,3×1
	//			  dXYZOffset	绝对位置偏差,3×1
	//			  nBand			重采样的波段数,4×1
	//			  nSamples		重采样的谱段数,4×1
	//			  bQuickView	是否快视图
	//			  bGrey			是否是灰度图
	//			  fScale		缩放因子
	//			  nUVBand		紫外快视波段
	//			  nVNIRBand		紫外快视波段
	//			  nSWIRBand		紫外快视波段
	//			  nTIHBand		紫外快视波段
	//			  pProFile		处理进度文件
	*****************************************************************************/
	long ExtractSaveP2ProductXML(char *pP2OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
		short &nP0, short &nP1A, short &nP1B, short &nP1C, short &nP1D, short &nP1E, short &nP2A, short &nP2B, short &nP2C,
		vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pP1AData, vector<string> &pP1AXML, vector<string> &pP1AJPG,
		vector<string> &pP1BData, vector<string> &pP1BXML, vector<string> &pP1BJPG, vector<string> &pP1CData, vector<string> &pP1CXML, vector<string> &pP1CJPG,
		vector<string> &pP1DData, vector<string> &pP1DXML, vector<string> &pP1DJPG, vector<string> &pP1EData, vector<string> &pP1EXML, vector<string> &pP1EJPG,
		vector<string> &pP2AData, vector<string> &pP2AXML, vector<string> &pP2AJPG,
		vector<string> &pP2BData, vector<string> &pP2BXML, vector<string> &pP2BJPG, vector<string> &pP2CData, vector<string> &pP2CXML, vector<string> &pP2CJPG,
		vector<string> &pReleCof, vector<string> &pAbCof, string &pModtran, vector<string> &pWaveLen, vector<string> &pEvent,
		string &pSbet, vector<string> &pPOS, vector<string> &pEOF, string &pDEM,
		float fFOLapX[], float fFOLapY[], float fSOLapX[], float fSOLapY[],
		float fFov[], float fIFov[], float fFocalLen[],
		double dBoresightMis[], double dGNSSOffset[], double dXYZOffset[], int nBand[], int nSamples[],
		bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile);
};