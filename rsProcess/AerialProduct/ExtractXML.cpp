#include"ExtractXML.h"
#include<Windows.h>

long ExtractQPDProduct::ExtractLoadPreProductXML(char *pCatalogOF, vector<string> &pRawData, vector<string> &pDarkData, vector<string> &pD0Data,
												vector<string> &pD0Dark, vector<string> &pD0XML, vector<string> &pD0JPG, bool &bSeg, int &nSLine,
												int &nELine, int &nALine, bool &bQuickView, bool &bGrey, float &fScale, int nUVBand[], int nVNIRBand[],
												int nSWIRBand[], int nTIHBand[], string &pProFile)
{
	long lError = 0;
	char *cSeg = "";
	char *cAuto = "";
	char *cLine = "";
	char *cQuickView = "";
	char *cBand = "";
	char *buf = "";
	char *temp = "";
	char *cScale = "";
	int i = 0;

	std::locale::global(std::locale(""));
	file<> fdoc(pCatalogOF);

	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<>* root = doc.first_node();
	if (root == NULL)
	{
		lError = 1;
		return lError;
	}

	//==============================获取输入数据===============================================
	xml_node<> *inputnode = root->first_node("InputFilelist");
	if (inputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		//获取原始图像数据
		xml_node<>* RawData;
		RawData = inputnode->first_node("UVRawData1");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("UVRawData2");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("UVRawData3");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("VNIRRawData1");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("VNIRRawData2");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("VNIRRawData3");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("SWIRRawData1");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("SWIRRawData2");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("SWIRRawData3");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("TIHRawData1");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("TIHRawData2");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());
		RawData = inputnode->first_node("TIHRawData3");
		if (RawData == NULL)
		{
			lError = 1;
			return lError;
		}
		pRawData.push_back(RawData->value());

		//获取暗电流数据
		xml_node<>* DarkData;
		DarkData = inputnode->first_node("UVDarkData1");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("UVDarkData2");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("UVDarkData3");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("VNIRDarkData1");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("VNIRDarkData2");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("VNIRDarkData3");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("SWIRDarkData1");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("SWIRDarkData2");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("SWIRDarkData3");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("TIHDarkData1");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("TIHDarkData2");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());
		DarkData = inputnode->first_node("TIHDarkData3");
		if (DarkData == NULL)
		{
			lError = 1;
			return lError;
		}
		pDarkData.push_back(DarkData->value());

	}


	//==============================获取输出数据===============================================
	xml_node<> *outputnode = root->first_node("OutputFilelist");
	if (outputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		//获取0级数据
		xml_node<>* D0Data;
		D0Data = outputnode->first_node("UVD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("UVD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("UVD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("VNIRD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("VNIRD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("VNIRD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("SWIRD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("SWIRD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("SWIRD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("TIHD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("TIHD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = outputnode->first_node("TIHD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());

		//获取0级数据暗电流数据
		xml_node<>* D0Dark;
		D0Dark = outputnode->first_node("UVD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("UVD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("UVD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("VNIRD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("VNIRD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("VNIRD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("SWIRD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("SWIRD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("SWIRD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("TIHD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("TIHD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = outputnode->first_node("TIHD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());

		//获取元数据文件
		xml_node<>* D0XML;
		D0XML = outputnode->first_node("UVD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("UVD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("UVD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("VNIRD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("VNIRD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("VNIRD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("SWIRD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("SWIRD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("SWIRD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("TIHD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("TIHD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = outputnode->first_node("TIHD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());

		//获取快视图
		xml_node<>* D0JPG;
		D0JPG = outputnode->first_node("UVD0JPG1");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("UVD0JPG2");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("UVD0JPG3");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("VNIRD0JPG1");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("VNIRD0JPG2");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("VNIRD0JPG3");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("SWIRD0JPG1");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("SWIRD0JPG2");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("SWIRD0JPG3");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("TIHD0JPG1");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("TIHD0JPG2");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
		D0JPG = outputnode->first_node("TIHD0JPG3");
		if (D0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0JPG.push_back(D0JPG->value());
	}


	//==============================获取参数===============================================
	xml_node<> *paranode = root->first_node("Parament");
	if (paranode == NULL)
	{
		lError = 1;
		return lError;
	}
	//获取分割参数
	xml_node<>* segnode;
	segnode = paranode->first_node("Segment");
	if (segnode == NULL)
	{
		lError = 1;
		return lError;
	}
	xml_node<>* segpara;
	segpara = segnode->first_node("WhetherSeg");
	if (segpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cSeg = segpara->value();
	if (cSeg[0] == '0')
	{
		bSeg = 0;
	}
	else
	{
		bSeg = 1;
	}
	segpara = segnode->first_node("WhertherAuto");
	if (segpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cAuto = segpara->value();

	segpara = segnode->first_node("SegLine");
	if (segpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cLine = segpara->value();
	if (cAuto[0] == '0')
	{
		nALine = 0;
		temp = strtok_s(cLine, ",", &buf);
		if (temp)
		{
			nSLine = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			nELine = atoi(temp);
		}
		else
		{
			return lError;
		}
	}
	else
	{
		nALine = atoi(cLine);
		nSLine = 0;
		nELine = 0;
	}
	//获取快视图参数
	xml_node<>* jpgnode;
	jpgnode = paranode->first_node("QuickView");
	if (jpgnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* jpgpara;
		jpgpara = jpgnode->first_node("WhetherQuickView");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cQuickView = jpgpara->value();
		if (cQuickView[0] == '0')
		{
			bQuickView = 0;
		}
		else
		{
			bQuickView = 1;
		}
		jpgpara = jpgnode->first_node("QuickViewScale");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cScale = jpgpara->value();
		fScale = (float)atof(cScale);

		jpgpara = jpgnode->first_node("UVQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nUVBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("VNIRQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nVNIRBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("SWIRQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nSWIRBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("TIHQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nTIHBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
	}

	//获取处理进度文件
	xml_node<>* repnode = NULL;
	repnode = paranode->first_node("ReportFile");
	if (repnode == NULL)
	{
		lError = 1;
		return lError;
	}
	pProFile = repnode->value();
	return lError;
}

long ExtractQPDProduct::ExtractSavePreProductXML(char *pCatalogOF, vector<string> &pRawData, vector<string> &pDarkData, vector<string> &pD0Data, vector<string> &pD0Dark,
												vector<string> &pD0XML, vector<string> &pD0JPG, bool &bSeg, int &nSLine, int &nELine, int &nALine, bool &bQuickView,
												bool &bGrey, float &fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile)
{
	long lError = 0;
	char cSegLine[50];
	char cUVBand[50], cVNIRBand[50], cSWIRBand[50], cTIHBand[50];
	char cScale[50];

	string text;
	ofstream out;

	std::locale::global(std::locale(""));
	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_pi, doc.allocate_string("xml version='1.0'encoding='utf-8'"));
	doc.append_node(root);

	xml_node<>* node = doc.allocate_node(node_element, "Task", NULL);
	doc.append_node(node);

	xml_node<>* inputnode = doc.allocate_node(node_element, "InputFilelist"); //num = '24', "36"
	inputnode->append_node(doc.allocate_node(node_element, "UVRawData1", pRawData[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVRawData2", pRawData[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVRawData3", pRawData[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRRawData1", pRawData[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRRawData2", pRawData[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRRawData3", pRawData[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRRawData1", pRawData[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRRawData2", pRawData[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRRawData3", pRawData[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHRawData1", pRawData[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHRawData2", pRawData[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHRawData3", pRawData[11].c_str()));

	inputnode->append_node(doc.allocate_node(node_element, "UVDarkData1", pDarkData[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVDarkData2", pDarkData[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVDarkData3", pDarkData[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRDarkData1", pDarkData[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRDarkData2", pDarkData[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRDarkData3", pDarkData[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRDarkData1", pDarkData[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRDarkData2", pDarkData[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRDarkData3", pDarkData[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHDarkData1", pDarkData[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHDarkData2", pDarkData[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHDarkData3", pDarkData[11].c_str()));
	node->append_node(inputnode);

	xml_node<>* outputnode = doc.allocate_node(node_element, "OutputFilelist");// num = '24'
	outputnode->append_node(doc.allocate_node(node_element, "UVD0Data1", pD0Data[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0Data2", pD0Data[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0Data3", pD0Data[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data1", pD0Data[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data2", pD0Data[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data3", pD0Data[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data1", pD0Data[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data2", pD0Data[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data3", pD0Data[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0Data1", pD0Data[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0Data2", pD0Data[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0Data3", pD0Data[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVD0Dark1", pD0Dark[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0Dark2", pD0Dark[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0Dark3", pD0Dark[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark1", pD0Dark[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark2", pD0Dark[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark3", pD0Dark[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark1", pD0Dark[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark2", pD0Dark[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark3", pD0Dark[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark1", pD0Dark[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark2", pD0Dark[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark3", pD0Dark[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVD0XML1", pD0XML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0XML2", pD0XML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0XML3", pD0XML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML1", pD0XML[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML2", pD0XML[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML3", pD0XML[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML1", pD0XML[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML2", pD0XML[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML3", pD0XML[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0XML1", pD0XML[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0XML2", pD0XML[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0XML3", pD0XML[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVD0JPG1", pD0JPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0JPG2", pD0JPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVD0JPG3", pD0JPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0JPG1", pD0JPG[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0JPG2", pD0JPG[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRD0JPG3", pD0JPG[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0JPG1", pD0JPG[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0JPG2", pD0JPG[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRD0JPG3", pD0JPG[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0JPG1", pD0JPG[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0JPG2", pD0JPG[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHD0JPG3", pD0JPG[11].c_str()));
	node->append_node(outputnode);

	xml_node<>* paranode = doc.allocate_node(node_element, "Parament");
	node->append_node(paranode);

	xml_node<>* segnode = doc.allocate_node(node_element, "Segment");
	if (bSeg == 0)
	{
		segnode->append_node(doc.allocate_node(node_element, "WhetherSeg", "0"));
		segnode->append_node(doc.allocate_node(node_element, "WhertherAuto", "0"));
		segnode->append_node(doc.allocate_node(node_element, "SegLine", "0"));
	}
	else
	{
		segnode->append_node(doc.allocate_node(node_element, "WhetherSeg", "1"));
		if (nALine == 0)
		{
			segnode->append_node(doc.allocate_node(node_element, "WhertherAuto", "0"));
			sprintf_s(cSegLine, sizeof(cSegLine), "%d,%d", nSLine, nELine);
			segnode->append_node(doc.allocate_node(node_element, "SegLine", cSegLine));
		}
		else
		{
			segnode->append_node(doc.allocate_node(node_element, "WhertherAuto", "1"));
			sprintf_s(cSegLine, sizeof(cSegLine), "%d", nALine);
			segnode->append_node(doc.allocate_node(node_element, "SegLine", cSegLine));
		}
	}
	paranode->append_node(segnode);

	xml_node<>* jpgnode = doc.allocate_node(node_element, "QuickView");
	if (bQuickView == 0)
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", "-1"));
	}
	else
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "1"));
		sprintf_s(cScale, sizeof(cScale), "%f", fScale);
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", cScale));
		sprintf_s(cUVBand, sizeof(cUVBand), "%d,%d,%d", nUVBand[0], nUVBand[1], nUVBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", cUVBand));
		sprintf_s(cVNIRBand, sizeof(cVNIRBand), "%d,%d,%d", nVNIRBand[0], nVNIRBand[1], nVNIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", cVNIRBand));
		sprintf_s(cSWIRBand, sizeof(cSWIRBand), "%d,%d,%d", nSWIRBand[0], nSWIRBand[1], nSWIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", cSWIRBand));
		sprintf_s(cTIHBand, sizeof(cTIHBand), "%d,%d,%d", nTIHBand[0], nTIHBand[1], nTIHBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", cTIHBand));
	}
	paranode->append_node(jpgnode);

	xml_node<>* repnode = doc.allocate_node(node_element, "ReportFile", pProFile.c_str());
	paranode->append_node(repnode);


	string str;
	rapidxml::print(std::back_inserter(str), doc, 0);
	ofstream out1(pCatalogOF);
	out1 << doc;
	out1.close();

	return lError;
}

long ExtractQPDProduct::ExtractLoadPreProductMeta(char *pP0MetaFile, unsigned int &nSensorType, unsigned int &nSensorOrder, unsigned int &nSamples, unsigned int &nLines, unsigned int &nBands, __int64 &nHeadOffset)
{
	long lError = 0;
	char *cSensorType, *cSensorOrder;
	char *cSamples, *cLines, *cBands;
	char *cHeadOffset;
	xml_node<>* tempnode;

	std::locale::global(std::locale(""));
	file<> fdoc(pP0MetaFile);

	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<>* root = doc.first_node();
	if (root == NULL)
	{
		lError = 1;
		return lError;
	}

	tempnode = root->first_node("Sensor");
	if (tempnode == NULL)
	{
		lError = 1;
		return lError;
	}

	tempnode = root->first_node("Producer");
	if (tempnode == NULL)
	{
		lError = 1;
		return lError;
	}

	tempnode = root->first_node("ProduceTime");
	if (tempnode == NULL)
	{
		lError = 1;
		return lError;
	}

	xml_node<> *datainfonode = root->first_node("DataInfo");
	if (datainfonode == NULL)
	{
		lError = 1;
		return lError;
	}

	xml_node<> *infonode;
	infonode = datainfonode->first_node("Format");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}

	infonode = datainfonode->first_node("Level");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}

	infonode = datainfonode->first_node("SensorType");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cSensorType = infonode->value();

	infonode = datainfonode->first_node("SensorOrder");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cSensorOrder = infonode->value();

	infonode = datainfonode->first_node("Samples");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cSamples = infonode->value();

	infonode = datainfonode->first_node("Lines");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cLines = infonode->value();

	infonode = datainfonode->first_node("Bands");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cBands = infonode->value();

	infonode = datainfonode->first_node("HeadOffset");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cHeadOffset = infonode->value();

	nSensorType = (unsigned int)atoi(cSensorType);
	nSensorOrder = (unsigned int)atoi(cSensorOrder);
	nSamples = (unsigned int)atoi(cSamples);
	nLines = (unsigned int)atoi(cLines);
	nBands = (unsigned int)atoi(cBands);
	nHeadOffset = _atoi64(cHeadOffset);

	return lError;
}

long ExtractQPDProduct::ExtractSavePreProductMeta(char *pD0MetaFile, int nSensorType, int	nSensorOrder, int nWidths, int nSamples, int nLines, int nBands, int nYear,
													int nMonth, int	nDay, int nSecond, __int64 nHeadOffset, __int64 nEofOffset, bool bDark, int nDarkLine)
{
	long lError = 0;
	char cSensorType[50], cSensorOrder[50];
	char cWidths[50], cSamples[50], cLines[50], cBands[50];
	char cCTime[100], cPTime[100];
	char cHeadOffset[50];
	char cDark[50], cDarkLine[50];

	_itoa_s(nSensorType, cSensorType, 10);
	_itoa_s(nSensorOrder, cSensorOrder, 10);
	_itoa_s(nWidths, cWidths, 10);
	_itoa_s(nSamples, cSamples, 10);
	_itoa_s(nLines, cLines, 10);
	_itoa_s(nBands, cBands, 10);
	_itoa_s((int)nHeadOffset, cHeadOffset, 10);
	_itoa_s(bDark, cDark, 10);
	_itoa_s(nDarkLine, cDarkLine, 10);

	int nPYear = 0, nPMonth = 0, nPDay = 0;
	SYSTEMTIME ct;
	GetLocalTime(&ct);
	nPYear = ct.wYear;
	nPMonth = ct.wMonth;
	nPDay = ct.wDay;
	_itoa_s(nYear * 10000 + nMonth * 100 + nDay, cCTime, 10);
	_itoa_s(nPYear * 10000 + nPMonth * 100 + nPDay, cPTime, 10);

	string text;
	ofstream out;

	std::locale::global(std::locale(""));
	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_pi, doc.allocate_string("xml version='1.0'encoding='utf-8'"));
	doc.append_node(root);

	xml_node<>* node = doc.allocate_node(node_element, "D0Meta", NULL);
	doc.append_node(node);

	xml_node<>* sensornode = doc.allocate_node(node_element, "Sensor", "HKGF-3");
	node->append_node(sensornode);

	xml_node<>* producernode = doc.allocate_node(node_element, "Producer", "SITP");
	node->append_node(producernode);

	xml_node<>* ctimernode = doc.allocate_node(node_element, "CollectTime", cCTime);
	node->append_node(ctimernode);

	xml_node<>* ptimernode = doc.allocate_node(node_element, "ProduceTime", cPTime);
	node->append_node(ptimernode);

	xml_node<>* datainfonode = doc.allocate_node(node_element, "DataInfo");

	datainfonode->append_node(doc.allocate_node(node_element, "Format", "RAW"));
	datainfonode->append_node(doc.allocate_node(node_element, "SensorType", cSensorType));
	datainfonode->append_node(doc.allocate_node(node_element, "SensorOrder", cSensorOrder));
	datainfonode->append_node(doc.allocate_node(node_element, "Widths", cWidths));
	datainfonode->append_node(doc.allocate_node(node_element, "Samples", cSamples));
	datainfonode->append_node(doc.allocate_node(node_element, "Lines", cLines));
	datainfonode->append_node(doc.allocate_node(node_element, "Bands", cBands));
	datainfonode->append_node(doc.allocate_node(node_element, "HeadOffset", cHeadOffset));
	node->append_node(datainfonode);

	xml_node<>* darkinfonode = doc.allocate_node(node_element, "DarkInfo");
	darkinfonode->append_node(doc.allocate_node(node_element, "WhetherDark", cDark));
	darkinfonode->append_node(doc.allocate_node(node_element, "DarkLine", cDarkLine));
	node->append_node(darkinfonode);


	string str;
	rapidxml::print(std::back_inserter(str), doc, 0);
	ofstream out1(pD0MetaFile);
	out1 << doc;
	out1.close();

	return lError;
}


long ExtractQPDProduct::ExtractLoadP0ProductXML(char *pP0OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
	vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pEvent, bool bQuickView,
	bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile)
{
	long lError = 0;
	char *cSeg = "";
	char *cAuto = "";
	char *cLine = "";
	char *cQuickView = "";
	char *cBand = "";
	char *buf = "";
	char *temp = "";
	char *cScale = "";
	int i = 0;

	std::locale::global(std::locale(""));
	file<> fdoc(pP0OF);

	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<>* root = doc.first_node();
	if (root == NULL)
	{
		lError = 1;
		return lError;
	}

	//==============================获取输入数据===============================================
	xml_node<> *inputnode = root->first_node("InputFilelist");
	if (inputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	//获取原始图像数据
	xml_node<>* D0Data;
	D0Data = inputnode->first_node("UVD0Data1");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("UVD0Data2");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("UVD0Data3");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("VNIRD0Data1");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("VNIRD0Data2");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("VNIRD0Data3");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("SWIRD0Data1");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("SWIRD0Data2");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("SWIRD0Data3");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("TIHD0Data1");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("TIHD0Data2");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());
	D0Data = inputnode->first_node("TIHD0Data3");
	if (D0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Data.push_back(D0Data->value());

	//获取暗电流数据
	xml_node<>* D0Dark;
	D0Dark = inputnode->first_node("UVD0Dark1");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("UVD0Dark2");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("UVD0Dark3");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("VNIRD0Dark1");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("VNIRD0Dark2");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("VNIRD0Dark3");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("SWIRD0Dark1");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("SWIRD0Dark2");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("SWIRD0Dark3");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("TIHD0Dark1");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("TIHD0Dark2");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());
	D0Dark = inputnode->first_node("TIHD0Dark3");
	if (D0Dark == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0Dark.push_back(D0Dark->value());

	//获取0级产品元数据文件
	xml_node<>* D0XML;
	D0XML = inputnode->first_node("UVD0XML1");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("UVD0XML2");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("UVD0XML3");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("VNIRD0XML1");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("VNIRD0XML2");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("VNIRD0XML3");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("SWIRD0XML1");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("SWIRD0XML2");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("SWIRD0XML3");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("TIHD0XML1");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("TIHD0XML2");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());
	D0XML = inputnode->first_node("TIHD0XML3");
	if (D0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pD0XML.push_back(D0XML->value());

	//==============================获取输出数据===============================================
	xml_node<> *outputnode = root->first_node("OutputFilelist");
	if (outputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	//获取0级产品
	xml_node<>* P0Data;
	P0Data = outputnode->first_node("UVP0Data1");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("UVP0Data2");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("UVP0Data3");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("VNIRP0Data1");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("VNIRP0Data2");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("VNIRP0Data3");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("SWIRP0Data1");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("SWIRP0Data2");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("SWIRP0Data3");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("TIHP0Data1");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("TIHP0Data2");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());
	P0Data = outputnode->first_node("TIHP0Data3");
	if (P0Data == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0Data.push_back(P0Data->value());


	//获取0级产品元数据文件
	xml_node<>* P0XML;
	P0XML = outputnode->first_node("UVP0XML1");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("UVP0XML2");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("UVP0XML3");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("VNIRP0XML1");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("VNIRP0XML2");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("VNIRP0XML3");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("SWIRP0XML1");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("SWIRP0XML2");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("SWIRP0XML3");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("TIHP0XML1");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("TIHP0XML2");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());
	P0XML = outputnode->first_node("TIHP0XML3");
	if (P0XML == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0XML.push_back(P0XML->value());

	//获取快视图
	xml_node<>* P0JPG;
	P0JPG = outputnode->first_node("UVP0JPG1");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("UVP0JPG2");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("UVP0JPG3");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("VNIRP0JPG1");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("VNIRP0JPG2");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("VNIRP0JPG3");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("SWIRP0JPG1");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("SWIRP0JPG2");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("SWIRP0JPG3");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("TIHP0JPG1");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("TIHP0JPG2");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());
	P0JPG = outputnode->first_node("TIHP0JPG3");
	if (P0JPG == NULL)
	{
		lError = 1;
		return lError;
	}
	pP0JPG.push_back(P0JPG->value());

	//获取EVENT
	xml_node<>* Event;
	Event = outputnode->first_node("UVEvent1");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("UVEvent2");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("UVEvent3");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("VNIREvent1");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("VNIREvent2");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("VNIREvent3");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("SWIREvent1");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("SWIREvent2");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("SWIREvent3");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("TIHEvent1");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("TIHEvent2");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	Event = outputnode->first_node("TIHEvent3");
	if (Event == NULL)
	{
		lError = 1;
		return lError;
	}
	pEvent.push_back(Event->value());
	//==============================获取参数===============================================
	xml_node<> *paranode = root->first_node("Parament");
	if (paranode == NULL)
	{
		lError = 1;
		return lError;
	}
	//获取快视图参数
	xml_node<>* jpgnode;
	jpgnode = paranode->first_node("QuickView");
	if (jpgnode == NULL)
	{
		lError = 1;
		return lError;
	}
	xml_node<>* jpgpara;
	jpgpara = jpgnode->first_node("WhetherQuickView");
	if (jpgpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cQuickView = jpgpara->value();
	if (cQuickView[0] == '0')
	{
		bQuickView = 0;
	}
	else
	{
		bQuickView = 1;
	}
	jpgpara = jpgnode->first_node("QuickViewScale");
	if (jpgpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cScale = jpgpara->value();
	fScale = (float)atof(cScale);

	jpgpara = jpgnode->first_node("UVQuickBand");
	if (jpgpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cBand = jpgpara->value();
	i = 0;
	temp = strtok_s(cBand, ",", &buf);
	while (temp && i<3)
	{
		nUVBand[i] = atoi(temp);
		temp = strtok_s(NULL, ",", &buf);
		i++;
	}
	jpgpara = jpgnode->first_node("VNIRQuickBand");
	if (jpgpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cBand = jpgpara->value();
	i = 0;
	temp = strtok_s(cBand, ",", &buf);
	while (temp && i<3)
	{
		nVNIRBand[i] = atoi(temp);
		temp = strtok_s(NULL, ",", &buf);
		i++;
	}
	jpgpara = jpgnode->first_node("SWIRQuickBand");
	if (jpgpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cBand = jpgpara->value();
	i = 0;
	temp = strtok_s(cBand, ",", &buf);
	while (temp && i<3)
	{
		nSWIRBand[i] = atoi(temp);
		temp = strtok_s(NULL, ",", &buf);
		i++;
	}
	jpgpara = jpgnode->first_node("TIHQuickBand");
	if (jpgpara == NULL)
	{
		lError = 1;
		return lError;
	}
	cBand = jpgpara->value();
	i = 0;
	temp = strtok_s(cBand, ",", &buf);
	while (temp && i<3)
	{
		nTIHBand[i] = atoi(temp);
		temp = strtok_s(NULL, ",", &buf);
		i++;
	}
	//获取处理进度文件
	xml_node<>* repnode = NULL;
	repnode = paranode->first_node("ReportFile");
	if (repnode == NULL)
	{
		lError = 1;
		return lError;
	}
	pProFile = repnode->value();
	return lError;
}

long ExtractQPDProduct::ExtractSaveP0ProductXML(char *pP0OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
	vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pEvent, bool bQuickView,
	bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile)
{
	long lError = 0;
	char cUVBand[50], cVNIRBand[50], cSWIRBand[50], cTIHBand[50];
	char cScale[50];

	string text;
	ofstream out;

	std::locale::global(std::locale(""));
	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_pi, doc.allocate_string("xml version='1.0'encoding='utf-8'"));
	doc.append_node(root);

	xml_node<>* node = doc.allocate_node(node_element, "P0Task", NULL);
	doc.append_node(node);

	xml_node<>* inputnode = doc.allocate_node(node_element, "InputFilelist"); //num = '24', "36"
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Data1", pD0Data[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Data2", pD0Data[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Data3", pD0Data[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data1", pD0Data[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data2", pD0Data[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data3", pD0Data[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data1", pD0Data[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data2", pD0Data[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data3", pD0Data[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data1", pD0Data[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data2", pD0Data[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data3", pD0Data[11].c_str()));

	inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark1", pD0Dark[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark2", pD0Dark[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark3", pD0Dark[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark1", pD0Dark[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark2", pD0Dark[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark3", pD0Dark[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark1", pD0Dark[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark2", pD0Dark[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark3", pD0Dark[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark1", pD0Dark[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark2", pD0Dark[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark3", pD0Dark[11].c_str()));

	inputnode->append_node(doc.allocate_node(node_element, "UVD0XML1", pD0XML[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0XML2", pD0XML[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0XML3", pD0XML[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML1", pD0XML[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML2", pD0XML[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML3", pD0XML[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML1", pD0XML[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML2", pD0XML[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML3", pD0XML[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML1", pD0XML[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML2", pD0XML[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML3", pD0XML[11].c_str()));

	node->append_node(inputnode);

	xml_node<>* outputnode = doc.allocate_node(node_element, "OutputFilelist");// num = '24'
	outputnode->append_node(doc.allocate_node(node_element, "UVP0Data1", pP0Data[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0Data2", pP0Data[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0Data3", pP0Data[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data1", pP0Data[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data2", pP0Data[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data3", pP0Data[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data1", pP0Data[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data2", pP0Data[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data3", pP0Data[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data1", pP0Data[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data2", pP0Data[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data3", pP0Data[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP0XML1", pP0XML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0XML2", pP0XML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0XML3", pP0XML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML1", pP0XML[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML2", pP0XML[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML3", pP0XML[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML1", pP0XML[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML2", pP0XML[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML3", pP0XML[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML1", pP0XML[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML2", pP0XML[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML3", pP0XML[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG1", pP0JPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG2", pP0JPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG3", pP0JPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG1", pP0JPG[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG2", pP0JPG[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG3", pP0JPG[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG1", pP0JPG[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG2", pP0JPG[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG3", pP0JPG[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG1", pP0JPG[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG2", pP0JPG[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG3", pP0JPG[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVEvent1", pEvent[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVEvent2", pEvent[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVEvent3", pEvent[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIREvent1", pEvent[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIREvent2", pEvent[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIREvent3", pEvent[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIREvent1", pEvent[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIREvent2", pEvent[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIREvent3", pEvent[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHEvent1", pEvent[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHEvent2", pEvent[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHEvent3", pEvent[11].c_str()));

	node->append_node(outputnode);

	xml_node<>* paranode = doc.allocate_node(node_element, "Parament");
	node->append_node(paranode);


	xml_node<>* jpgnode = doc.allocate_node(node_element, "QuickView");
	if (bQuickView == 0)
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", "-1"));
	}
	else
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "1"));
		sprintf_s(cScale, sizeof(cScale), "%f", fScale);
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", cScale));
		sprintf_s(cUVBand, sizeof(cUVBand), "%d,%d,%d", nUVBand[0], nUVBand[1], nUVBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", cUVBand));
		sprintf_s(cVNIRBand, sizeof(cVNIRBand), "%d,%d,%d", nVNIRBand[0], nVNIRBand[1], nVNIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", cVNIRBand));
		sprintf_s(cSWIRBand, sizeof(cSWIRBand), "%d,%d,%d", nSWIRBand[0], nSWIRBand[1], nSWIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", cSWIRBand));
		sprintf_s(cTIHBand, sizeof(cTIHBand), "%d,%d,%d", nTIHBand[0], nTIHBand[1], nTIHBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", cTIHBand));
	}
	paranode->append_node(jpgnode);

	xml_node<>* repnode = doc.allocate_node(node_element, "ReportFile", pProFile.c_str());
	paranode->append_node(repnode);


	string str;
	rapidxml::print(std::back_inserter(str), doc, 0);
	ofstream out1(pP0OF);
	out1 << doc;
	out1.close();

	return lError;
}

long ExtractQPDProduct::ExtractLoadP0ProductMeta(char *pP0MetaFile, unsigned int &nSensorType, unsigned int &nSensorOrder, unsigned int &nSamples, unsigned int &nLines, unsigned int &nBands, __int64 &nHeadOffset)
{
	long lError = 0;
	char *cSensorType, *cSensorOrder;
	char *cSamples, *cLines, *cBands;
	char *cHeadOffset;
	xml_node<>* tempnode;

	std::locale::global(std::locale(""));
	file<> fdoc(pP0MetaFile);

	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<>* root = doc.first_node();
	if (root == NULL)
	{
		lError = 1;
		return lError;
	}

	tempnode = root->first_node("Sensor");
	if (tempnode == NULL)
	{
		lError = 1;
		return lError;
	}

	tempnode = root->first_node("Producer");
	if (tempnode == NULL)
	{
		lError = 1;
		return lError;
	}

	tempnode = root->first_node("ProduceTime");
	if (tempnode == NULL)
	{
		lError = 1;
		return lError;
	}

	xml_node<> *datainfonode = root->first_node("DataInfo");
	if (datainfonode == NULL)
	{
		lError = 1;
		return lError;
	}

	xml_node<> *infonode;
	infonode = datainfonode->first_node("Format");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}

	infonode = datainfonode->first_node("Level");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}

	infonode = datainfonode->first_node("SensorType");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cSensorType = infonode->value();

	infonode = datainfonode->first_node("SensorOrder");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cSensorOrder = infonode->value();

	infonode = datainfonode->first_node("Samples");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cSamples = infonode->value();

	infonode = datainfonode->first_node("Lines");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cLines = infonode->value();

	infonode = datainfonode->first_node("Bands");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cBands = infonode->value();

	infonode = datainfonode->first_node("HeadOffset");
	if (infonode == NULL)
	{
		lError = 1;
		return lError;
	}
	cHeadOffset = infonode->value();

	nSensorType = (unsigned int)atoi(cSensorType);
	nSensorOrder = (unsigned int)atoi(cSensorOrder);
	nSamples = (unsigned int)atoi(cSamples);
	nLines = (unsigned int)atoi(cLines);
	nBands = (unsigned int)atoi(cBands);
	nHeadOffset = _atoi64(cHeadOffset);

	return lError;
}

long ExtractQPDProduct::ExtractSaveP0ProductMeta(char *pP0MetaFile, int nSensorType, int nSensorOrder, int nSamples, int nLines, int nBands, __int64 nHeadOffset)
{
	long lError = 0;
	char cSensorType[50], cSensorOrder[50];
	char cSamples[50], cLines[50], cBands[50];
	char  cPTime[100];
	char cHeadOffset[50];

	_itoa_s(nSensorType, cSensorType, 10);
	_itoa_s(nSensorOrder, cSensorOrder, 10);
	_itoa_s(nSamples, cSamples, 10);
	_itoa_s(nLines, cLines, 10);
	_itoa_s(nBands, cBands, 10);
	_itoa_s((int)nHeadOffset, cHeadOffset, 10);

	string text;
	ofstream out;

	std::locale::global(std::locale(""));
	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_pi, doc.allocate_string("xml version='1.0'encoding='utf-8'"));
	doc.append_node(root);

	xml_node<>* node = doc.allocate_node(node_element, "P0Meta", NULL);
	doc.append_node(node);

	xml_node<>* sensornode = doc.allocate_node(node_element, "Sensor", "HKGF-3");
	node->append_node(sensornode);

	xml_node<>* producernode = doc.allocate_node(node_element, "Producer", "SITP");
	node->append_node(producernode);

	xml_node<>* ptimernode = doc.allocate_node(node_element, "ProduceTime", cPTime);
	node->append_node(ptimernode);

	xml_node<>* datainfonode = doc.allocate_node(node_element, "DataInfo");
	datainfonode->append_node(doc.allocate_node(node_element, "Format", "TIFF"));
	datainfonode->append_node(doc.allocate_node(node_element, "Level", "P0"));
	datainfonode->append_node(doc.allocate_node(node_element, "SensorType", cSensorType));
	datainfonode->append_node(doc.allocate_node(node_element, "SensorOrder", cSensorOrder));
	datainfonode->append_node(doc.allocate_node(node_element, "Samples", cSamples));
	datainfonode->append_node(doc.allocate_node(node_element, "Lines", cLines));
	datainfonode->append_node(doc.allocate_node(node_element, "Bands", cBands));
	datainfonode->append_node(doc.allocate_node(node_element, "HeadOffset", cHeadOffset));
	node->append_node(datainfonode);

	string str;
	rapidxml::print(std::back_inserter(str), doc, 0);
	ofstream out1(pP0MetaFile);
	out1 << doc;
	out1.close();

	return lError;
}


long ExtractQPDProduct::ExtractLoadP1ProductXML(char *pP1OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
	short &nP0, short &nP1A, short &nP1B, short &nP1C, short &nP1D, short &nP1E,
	vector<string> &pP0Data, vector<string> &pP0XML, vector<string> &pP0JPG, vector<string> &pP1AData, vector<string> &pP1AXML, vector<string> &pP1AJPG,
	vector<string> &pP1BData, vector<string> &pP1BXML, vector<string> &pP1BJPG, vector<string> &pP1CData, vector<string> &pP1CXML, vector<string> &pP1CJPG,
	vector<string> &pP1DData, vector<string> &pP1DXML, vector<string> &pP1DJPG, vector<string> &pP1EData, vector<string> &pP1EXML, vector<string> &pP1EJPG,
	vector<string> &pReleCof, vector<string> &pAbCof, string &pModtran, vector<string> &pWaveLen, vector<string> &pEvent,
	float fFOLapX[], float fFOLapY[], float fSOLapX[], float fSOLapY[],
	bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile)
{
	long lError = 0;
	char *cQuickView = "";
	char *cBand = "";
	char *buf = "";
	char *temp = "";
	char *cScale = "";
	int i = 0, j = 0;

	std::locale::global(std::locale(""));
	file<> fdoc(pP1OF);

	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<>* root = doc.first_node();
	if (root == NULL)
	{
		lError = 1;
		return lError;
	}

	//==============================获取输入数据===============================================
	xml_node<> *inputnode = root->first_node("InputFilelist");
	if (inputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		//获取原始图像数据
		xml_node<>* D0Data;
		D0Data = inputnode->first_node("UVD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("UVD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("UVD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("VNIRD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("VNIRD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("VNIRD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("SWIRD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("SWIRD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("SWIRD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("TIHD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("TIHD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("TIHD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());

		//获取暗电流数据
		xml_node<>* D0Dark;
		D0Dark = inputnode->first_node("UVD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("UVD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("UVD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("VNIRD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("VNIRD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("VNIRD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("SWIRD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("SWIRD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("SWIRD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("TIHD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("TIHD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("TIHD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());

		//获取0级产品元数据文件
		xml_node<>* D0XML;
		D0XML = inputnode->first_node("UVD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("UVD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("UVD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("VNIRD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("VNIRD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("VNIRD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("SWIRD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("SWIRD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("SWIRD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("TIHD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("TIHD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("TIHD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
	}


	//==============================获取输出数据===============================================
	xml_node<> *outputnode = root->first_node("OutputFilelist");
	if (outputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		//获取0级产品=================================
		xml_node<>* P0Data;
		P0Data = outputnode->first_node("UVP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("UVP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("UVP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("VNIRP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("VNIRP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("VNIRP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("SWIRP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("SWIRP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("SWIRP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("TIHP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("TIHP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("TIHP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());

		//获取0级产品元数据文件
		xml_node<>* P0XML;
		P0XML = outputnode->first_node("UVP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("UVP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("UVP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("VNIRP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("VNIRP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("VNIRP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("SWIRP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("SWIRP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("SWIRP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("TIHP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("TIHP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("TIHP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());

		//获取0级快视图
		xml_node<>* P0JPG;
		P0JPG = outputnode->first_node("UVP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("UVP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("UVP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("VNIRP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("VNIRP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("VNIRP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("SWIRP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("SWIRP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("SWIRP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("TIHP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("TIHP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("TIHP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());

		//获取EVENT
		xml_node<>* Event;
		Event = outputnode->first_node("UVEvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("UVEvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("UVEvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("VNIREvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("VNIREvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("VNIREvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("SWIREvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("SWIREvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("SWIREvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("TIHEvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("TIHEvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("TIHEvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());

		//获取P1A级产品====================================
		xml_node<>* P1AData;
		P1AData = outputnode->first_node("UVP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("UVP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("UVP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("VNIRP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("VNIRP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("VNIRP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("SWIRP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("SWIRP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("SWIRP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("TIHP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("TIHP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("TIHP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());


		//获取0级产品元数据文件
		xml_node<>* P1AXML;
		P1AXML = outputnode->first_node("UVP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("UVP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("UVP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("VNIRP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("VNIRP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("VNIRP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("SWIRP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("SWIRP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("SWIRP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("TIHP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("TIHP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("TIHP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());

		//获取P1A快视图
		xml_node<>* P1AJPG;
		P1AJPG = outputnode->first_node("UVP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("UVP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("UVP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("VNIRP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("VNIRP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("VNIRP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("SWIRP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("SWIRP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("SWIRP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("TIHP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("TIHP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("TIHP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());

		//获取P1B级产品
		xml_node<>* P1BData;
		P1BData = outputnode->first_node("UVP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("UVP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("UVP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("VNIRP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("VNIRP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("VNIRP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("SWIRP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("SWIRP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("SWIRP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("TIHP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("TIHP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("TIHP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());


		//获取P1B级产品元数据文件
		xml_node<>* P1BXML;
		P1BXML = outputnode->first_node("UVP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("UVP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("UVP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("VNIRP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("VNIRP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("VNIRP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("SWIRP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("SWIRP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("SWIRP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("TIHP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("TIHP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("TIHP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());

		//获取P1B快视图
		xml_node<>* P1BJPG;
		P1BJPG = outputnode->first_node("UVP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("UVP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("UVP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("VNIRP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("VNIRP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("VNIRP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("SWIRP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("SWIRP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("SWIRP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("TIHP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("TIHP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("TIHP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());

		//获取P1C级产品
		xml_node<>* P1CData;
		P1CData = outputnode->first_node("UVP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("UVP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("UVP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("VNIRP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("VNIRP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("VNIRP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("SWIRP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("SWIRP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("SWIRP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("TIHP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("TIHP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("TIHP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());


		//获取P1C级产品元数据文件
		xml_node<>* P1CXML;
		P1CXML = outputnode->first_node("UVP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("UVP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("UVP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("VNIRP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("VNIRP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("VNIRP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("SWIRP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("SWIRP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("SWIRP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("TIHP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("TIHP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("TIHP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());

		//获取P1C快视图
		xml_node<>* P1CJPG;
		P1CJPG = outputnode->first_node("UVP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("UVP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("UVP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("VNIRP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("VNIRP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("VNIRP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("SWIRP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("SWIRP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("SWIRP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("TIHP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("TIHP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("TIHP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());

		//获取P1D级产品
		xml_node<>* P1DData;
		P1DData = outputnode->first_node("UVP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());
		P1DData = outputnode->first_node("VNIRP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());
		P1DData = outputnode->first_node("SWIRP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());
		P1DData = outputnode->first_node("TIHP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());

		//获取P1D级产品元数据文件
		xml_node<>* P1DXML;
		P1DXML = outputnode->first_node("UVP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());
		P1DXML = outputnode->first_node("VNIRP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());
		P1DXML = outputnode->first_node("SWIRP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());
		P1DXML = outputnode->first_node("TIHP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());

		//获取P1D快视图
		xml_node<>* P1DJPG;
		P1DJPG = outputnode->first_node("UVP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());
		P1DJPG = outputnode->first_node("VNIRP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());
		P1DJPG = outputnode->first_node("SWIRP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());
		P1DJPG = outputnode->first_node("TIHP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());

		//获取P1E级产品
		xml_node<>* P1EData;
		P1EData = outputnode->first_node("UVP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());
		P1EData = outputnode->first_node("VNIRP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());
		P1EData = outputnode->first_node("SWIRP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());
		P1EData = outputnode->first_node("TIHP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());

		//获取P1E级产品元数据文件
		xml_node<>* P1EXML;
		P1EXML = outputnode->first_node("UVP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());
		P1EXML = outputnode->first_node("VNIRP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());
		P1EXML = outputnode->first_node("SWIRP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());
		P1EXML = outputnode->first_node("TIHP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());

		//获取P1E快视图
		xml_node<>* P1EJPG;
		P1EJPG = outputnode->first_node("UVP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());
		P1EJPG = outputnode->first_node("VNIRP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());
		P1EJPG = outputnode->first_node("SWIRP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());
		P1EJPG = outputnode->first_node("TIHP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());

	}

	//==============================获取参数===============================================
	xml_node<> *paranode = root->first_node("Parament");
	if (paranode == NULL)
	{
		lError = 1;
		return lError;
	}
	//获取相对辐射校正系数
	xml_node<> *recofnode = paranode->first_node("RadiateReleCof");
	if (recofnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* ReleCof;
		ReleCof = recofnode->first_node("UVReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("UVReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("UVReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("VNIRReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("VNIRReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("VNIRReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("SWIRReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("SWIRReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("SWIRReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("TIHReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("TIHReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("TIHReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
	}
	//获取绝对辐射校正系数
	xml_node<> *abcofnode = paranode->first_node("RadiateAbCof");
	if (abcofnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* AbCof;
		AbCof = abcofnode->first_node("UVAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("UVAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("UVAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("VNIRAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("VNIRAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("VNIRAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("SWIRAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("SWIRAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("SWIRAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("TIHAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("TIHAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("TIHAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
	}
	//获取中心波长半波宽信息
	xml_node<> *splnode = paranode->first_node("WaveLength");
	if (splnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* WaveLen;
		WaveLen = splnode->first_node("Modtran");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pModtran = WaveLen->value();
		WaveLen = splnode->first_node("UVWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("UVWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("UVWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("VNIRWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("VNIRWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("VNIRWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("SWIRWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("SWIRWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("SWIRWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("TIHWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("TIHWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("TIHWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
	}

	//获取视场拼接参数
	xml_node<>* folapnode;
	folapnode = paranode->first_node("FieldOverLap");
	if (folapnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* FOLP;
		char *cFOLap;
		i = 0, j = 0;
		FOLP = folapnode->first_node("VNIRFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		FOLP = folapnode->first_node("SWIRFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		FOLP = folapnode->first_node("TIHFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i + 4] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j + 4] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		FOLP = folapnode->first_node("UVFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i + 6] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j + 6] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}


	}

	//获取谱段拼接参数
	xml_node<>* solapnode;
	solapnode = paranode->first_node("SpectrumOverLap");
	if (solapnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* SOLP;
		char *cFOLap;
		i = 0, j = 0;
		SOLP = solapnode->first_node("UVSpectrumOverLap");
		if (SOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = SOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<1)
			{
				fSOLapX[i] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<1)
			{
				fSOLapY[j] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		SOLP = solapnode->first_node("SWIRSpectrumOverLap");
		if (SOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = SOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<1)
			{
				fSOLapX[i + 1] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<1)
			{
				fSOLapY[j + 1] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		SOLP = solapnode->first_node("TIHSpectrumOverLap");
		if (SOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = SOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<1)
			{
				fSOLapX[i + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<1)
			{
				fSOLapY[j + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
	}


	//获取快视图参数
	xml_node<>* jpgnode;
	jpgnode = paranode->first_node("QuickView");
	if (jpgnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* jpgpara;
		jpgpara = jpgnode->first_node("WhetherQuickView");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cQuickView = jpgpara->value();
		if (cQuickView[0] == '0')
		{
			bQuickView = 0;
		}
		else
		{
			bQuickView = 1;
		}
		jpgpara = jpgnode->first_node("QuickViewScale");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cScale = jpgpara->value();
		fScale = (float)atof(cScale);

		jpgpara = jpgnode->first_node("UVQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nUVBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("VNIRQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nVNIRBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("SWIRQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nSWIRBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("TIHQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nTIHBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
	}

	//获取处理进度文件
	xml_node<>* repnode = NULL;
	repnode = paranode->first_node("ReportFile");
	if (repnode == NULL)
	{
		lError = 1;
		return lError;
	}
	pProFile = repnode->value();
	return lError;
}

long ExtractQPDProduct::ExtractSaveP1ProductXML(char *pP1OF, vector<string> pD0Data, vector<string> pD0XML, vector<string> pD0Dark,
	short nP0, short nP1A, short nP1B, short nP1C, short nP1D, short nP1E,
	vector<string> pP0Data, vector<string> pP0XML, vector<string> pP0JPG, vector<string> pP1AData, vector<string> pP1AXML, vector<string> pP1AJPG,
	vector<string> pP1BData, vector<string> pP1BXML, vector<string> pP1BJPG, vector<string> pP1CData, vector<string> pP1CXML, vector<string> pP1CJPG,
	vector<string> pP1DData, vector<string> pP1DXML, vector<string> pP1DJPG, vector<string> pP1EData, vector<string> pP1EXML, vector<string> pP1EJPG,
	vector<string> pReleCof, vector<string> pAbCof, string pModtran, vector<string> pWaveLen, vector<string> pEvent,
	float fFOLapX[], float fFOLapY[], float fSOLapX[], float fSOLapY[],
	bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string pProFile)
{
	long lError = 0;
	char cUVBand[50], cVNIRBand[50], cSWIRBand[50], cTIHBand[50];
	char cScale[50];

	string text;
	ofstream out;

	std::locale::global(std::locale(""));
	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_pi, doc.allocate_string("xml version='1.0'encoding='utf-8'"));
	doc.append_node(root);

	xml_node<>* node = doc.allocate_node(node_element, "P1Task", NULL);
	doc.append_node(node);

	xml_node<>* inputnode = doc.allocate_node(node_element, "InputFilelist"); //num = '24', "36"
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Data1", pD0Data[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Data2", pD0Data[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Data3", pD0Data[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data1", pD0Data[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data2", pD0Data[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data3", pD0Data[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data1", pD0Data[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data2", pD0Data[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data3", pD0Data[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data1", pD0Data[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data2", pD0Data[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data3", pD0Data[11].c_str()));

	inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark1", pD0Dark[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark2", pD0Dark[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark3", pD0Dark[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark1", pD0Dark[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark2", pD0Dark[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark3", pD0Dark[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark1", pD0Dark[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark2", pD0Dark[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark3", pD0Dark[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark1", pD0Dark[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark2", pD0Dark[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark3", pD0Dark[11].c_str()));

	inputnode->append_node(doc.allocate_node(node_element, "UVD0XML1", pD0XML[0].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0XML2", pD0XML[1].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "UVD0XML3", pD0XML[2].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML1", pD0XML[3].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML2", pD0XML[4].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML3", pD0XML[5].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML1", pD0XML[6].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML2", pD0XML[7].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML3", pD0XML[8].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML1", pD0XML[9].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML2", pD0XML[10].c_str()));
	inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML3", pD0XML[11].c_str()));

	node->append_node(inputnode);

	xml_node<>* outputnode = doc.allocate_node(node_element, "OutputFilelist");
	outputnode->append_node(doc.allocate_node(node_element, "UVP0Data1", pP0Data[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0Data2", pP0Data[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0Data3", pP0Data[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data1", pP0Data[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data2", pP0Data[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data3", pP0Data[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data1", pP0Data[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data2", pP0Data[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data3", pP0Data[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data1", pP0Data[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data2", pP0Data[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data3", pP0Data[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP0XML1", pP0XML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0XML2", pP0XML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0XML3", pP0XML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML1", pP0XML[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML2", pP0XML[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML3", pP0XML[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML1", pP0XML[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML2", pP0XML[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML3", pP0XML[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML1", pP0XML[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML2", pP0XML[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML3", pP0XML[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG1", pP0JPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG2", pP0JPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG3", pP0JPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG1", pP0JPG[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG2", pP0JPG[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG3", pP0JPG[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG1", pP0JPG[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG2", pP0JPG[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG3", pP0JPG[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG1", pP0JPG[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG2", pP0JPG[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG3", pP0JPG[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVEvent1", pEvent[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVEvent2", pEvent[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVEvent3", pEvent[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIREvent1", pEvent[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIREvent2", pEvent[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIREvent3", pEvent[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIREvent1", pEvent[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIREvent2", pEvent[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIREvent3", pEvent[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHEvent1", pEvent[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHEvent2", pEvent[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHEvent3", pEvent[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1AData1", pP1AData[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1AData2", pP1AData[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1AData3", pP1AData[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AData1", pP1AData[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AData2", pP1AData[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AData3", pP1AData[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AData1", pP1AData[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AData2", pP1AData[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AData3", pP1AData[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AData1", pP1AData[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AData2", pP1AData[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AData3", pP1AData[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1AXML1", pP1AXML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1AXML2", pP1AXML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1AXML3", pP1AXML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AXML1", pP1AXML[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AXML2", pP1AXML[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AXML3", pP1AXML[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AXML1", pP1AXML[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AXML2", pP1AXML[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AXML3", pP1AXML[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AXML1", pP1AXML[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AXML2", pP1AXML[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AXML3", pP1AXML[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1AJPG1", pP1AJPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1AJPG2", pP1AJPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1AJPG3", pP1AJPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AJPG1", pP1AJPG[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AJPG2", pP1AJPG[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AJPG3", pP1AJPG[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AJPG1", pP1AJPG[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AJPG2", pP1AJPG[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AJPG3", pP1AJPG[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AJPG1", pP1AJPG[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AJPG2", pP1AJPG[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1AJPG3", pP1AJPG[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1BData1", pP1BData[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1BData2", pP1BData[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1BData3", pP1BData[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BData1", pP1BData[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BData2", pP1BData[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BData3", pP1BData[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BData1", pP1BData[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BData2", pP1BData[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BData3", pP1BData[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BData1", pP1BData[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BData2", pP1BData[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BData3", pP1BData[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1BXML1", pP1BXML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1BXML2", pP1BXML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1BXML3", pP1BXML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BXML1", pP1BXML[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BXML2", pP1BXML[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BXML3", pP1BXML[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BXML1", pP1BXML[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BXML2", pP1BXML[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BXML3", pP1BXML[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BXML1", pP1BXML[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BXML2", pP1BXML[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BXML3", pP1BXML[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1BJPG1", pP1BJPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1BJPG2", pP1BJPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1BJPG3", pP1BJPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BJPG1", pP1BJPG[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BJPG2", pP1BJPG[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BJPG3", pP1BJPG[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BJPG1", pP1BJPG[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BJPG2", pP1BJPG[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BJPG3", pP1BJPG[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BJPG1", pP1BJPG[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BJPG2", pP1BJPG[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1BJPG3", pP1BJPG[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1CData1", pP1CData[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1CData2", pP1CData[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1CData3", pP1CData[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CData1", pP1CData[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CData2", pP1CData[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CData3", pP1CData[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CData1", pP1CData[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CData2", pP1CData[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CData3", pP1CData[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CData1", pP1CData[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CData2", pP1CData[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CData3", pP1CData[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1CXML1", pP1CXML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1CXML2", pP1CXML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1CXML3", pP1CXML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CXML1", pP1CXML[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CXML2", pP1CXML[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CXML3", pP1CXML[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CXML1", pP1CXML[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CXML2", pP1CXML[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CXML3", pP1CXML[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CXML1", pP1CXML[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CXML2", pP1CXML[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CXML3", pP1CXML[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1CJPG1", pP1CJPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1CJPG2", pP1CJPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "UVP1CJPG3", pP1CJPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CJPG1", pP1CJPG[3].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CJPG2", pP1CJPG[4].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CJPG3", pP1CJPG[5].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CJPG1", pP1CJPG[6].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CJPG2", pP1CJPG[7].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CJPG3", pP1CJPG[8].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CJPG1", pP1CJPG[9].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CJPG2", pP1CJPG[10].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1CJPG3", pP1CJPG[11].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1DData", pP1DData[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1DData", pP1DData[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1DData", pP1DData[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1DData", pP1DData[3].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1DXML", pP1DXML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1DXML", pP1DXML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1DXML", pP1DXML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1DXML", pP1DXML[3].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1DJPG", pP1DJPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1DJPG", pP1DJPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1DJPG", pP1DJPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1DJPG", pP1DJPG[3].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1EData", pP1EData[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1EData", pP1EData[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1EData", pP1EData[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1EData", pP1EData[3].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1EXML", pP1EXML[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1EXML", pP1EXML[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1EXML", pP1EXML[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1EXML", pP1EXML[3].c_str()));

	outputnode->append_node(doc.allocate_node(node_element, "UVP1EJPG", pP1EJPG[0].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "VNIRP1EJPG", pP1EJPG[1].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "SWIRP1EJPG", pP1EJPG[2].c_str()));
	outputnode->append_node(doc.allocate_node(node_element, "TIHP1EJPG", pP1EJPG[3].c_str()));

	node->append_node(outputnode);

	xml_node<>* paranode = doc.allocate_node(node_element, "Parament");
	node->append_node(paranode);

	xml_node<>* recofnode = doc.allocate_node(node_element, "RadiateReleCof");
	recofnode->append_node(doc.allocate_node(node_element, "UVReleCof1", pReleCof[0].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "UVReleCof2", pReleCof[1].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "UVReleCof3", pReleCof[2].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "VNIRReleCof1", pReleCof[3].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "VNIRReleCof2", pReleCof[4].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "VNIRReleCof3", pReleCof[5].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "SWIRReleCof1", pReleCof[6].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "SWIRReleCof2", pReleCof[7].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "SWIRReleCof3", pReleCof[8].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "TIHReleCof1", pReleCof[9].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "TIHReleCof2", pReleCof[10].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "TIHReleCof3", pReleCof[11].c_str()));
	paranode->append_node(recofnode);

	xml_node<>* abcofnode = doc.allocate_node(node_element, "RadiateAbCof");
	abcofnode->append_node(doc.allocate_node(node_element, "UVAbCof1", pAbCof[0].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "UVAbCof2", pAbCof[1].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "UVAbCof3", pAbCof[2].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "VNIRAbCof1", pAbCof[3].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "VNIRAbCof2", pAbCof[4].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "VNIRAbCof3", pAbCof[5].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "SWIRAbCof1", pAbCof[6].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "SWIRAbCof2", pAbCof[7].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "SWIRAbCof3", pAbCof[8].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "TIHAbCof1", pAbCof[9].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "TIHAbCof2", pAbCof[10].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "TIHAbCof3", pAbCof[11].c_str()));
	paranode->append_node(abcofnode);

	xml_node<>* splnode = doc.allocate_node(node_element, "WaveLength");
	splnode->append_node(doc.allocate_node(node_element, "Modtran", pModtran.c_str()));
	splnode->append_node(doc.allocate_node(node_element, "UVWaveLen1", pWaveLen[0].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "UVWaveLen2", pWaveLen[1].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "UVWaveLen3", pWaveLen[2].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "VNIRWaveLen1", pWaveLen[3].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "VNIRWaveLen2", pWaveLen[4].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "VNIRWaveLen3", pWaveLen[5].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "SWIRWaveLen1", pWaveLen[6].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "SWIRWaveLen2", pWaveLen[7].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "SWIRWaveLen3", pWaveLen[8].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "TIHWaveLen1", pWaveLen[9].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "TIHWaveLen2", pWaveLen[10].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "TIHWaveLen3", pWaveLen[11].c_str()));
	paranode->append_node(splnode);

	char cUVFOLap[200], cVNIRFOLap[200], cSWIRFOLap[200], cTIHFOLap[200];
	xml_node<>* folapnode = doc.allocate_node(node_element, "FieldOverLap");
	sprintf_s(cVNIRFOLap, sizeof(cVNIRFOLap), "%f,%f,%f,%f", fFOLapX[0], fFOLapX[1], fFOLapY[0], fFOLapY[1]);
	folapnode->append_node(doc.allocate_node(node_element, "VNIRFieldOverLap", cVNIRFOLap));
	sprintf_s(cSWIRFOLap, sizeof(cSWIRFOLap), "%f,%f,%f,%f", fFOLapX[2], fFOLapX[3], fFOLapY[2], fFOLapY[3]);
	folapnode->append_node(doc.allocate_node(node_element, "SWIRFieldOverLap", cSWIRFOLap));
	sprintf_s(cTIHFOLap, sizeof(cTIHFOLap), "%f,%f,%f,%f", fFOLapX[4], fFOLapX[5], fFOLapY[4], fFOLapY[5]);
	folapnode->append_node(doc.allocate_node(node_element, "TIHFieldOverLap", cTIHFOLap));
	sprintf_s(cUVFOLap, sizeof(cUVFOLap), "%f,%f,%f,%f", fFOLapX[6], fFOLapX[7], fFOLapY[6], fFOLapY[7]);
	folapnode->append_node(doc.allocate_node(node_element, "UVFieldOverLap", cUVFOLap));
	paranode->append_node(folapnode);

	char cUVSOLap[100], cSWIRSOLap[100], cTIHSOLap[100];
	xml_node<>* solapnode = doc.allocate_node(node_element, "SpectrumOverLap");
	sprintf_s(cUVSOLap, sizeof(cUVSOLap), "%f,%f", fSOLapX[0], fSOLapY[0]);
	solapnode->append_node(doc.allocate_node(node_element, "UVSpectrumOverLap", cUVSOLap));
	sprintf_s(cSWIRSOLap, sizeof(cSWIRSOLap), "%f,%f", fSOLapX[1], fSOLapY[1]);
	solapnode->append_node(doc.allocate_node(node_element, "SWIRSpectrumOverLap", cSWIRSOLap));
	sprintf_s(cTIHSOLap, sizeof(cTIHSOLap), "%f,%f", fSOLapX[2], fSOLapY[2]);
	solapnode->append_node(doc.allocate_node(node_element, "TIHSpectrumOverLap", cTIHSOLap));
	paranode->append_node(solapnode);

	xml_node<>* jpgnode = doc.allocate_node(node_element, "QuickView");
	if (bQuickView == 0)
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", "-1"));
	}
	else
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "1"));
		sprintf_s(cScale, sizeof(cScale), "%f", fScale);
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", cScale));
		sprintf_s(cUVBand, sizeof(cUVBand), "%d,%d,%d", nUVBand[0], nUVBand[1], nUVBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", cUVBand));
		sprintf_s(cVNIRBand, sizeof(cVNIRBand), "%d,%d,%d", nVNIRBand[0], nVNIRBand[1], nVNIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", cVNIRBand));
		sprintf_s(cSWIRBand, sizeof(cSWIRBand), "%d,%d,%d", nSWIRBand[0], nSWIRBand[1], nSWIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", cSWIRBand));
		sprintf_s(cTIHBand, sizeof(cTIHBand), "%d,%d,%d", nTIHBand[0], nTIHBand[1], nTIHBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", cTIHBand));
	}
	paranode->append_node(jpgnode);

	xml_node<>* repnode = doc.allocate_node(node_element, "ReportFile", pProFile.c_str());
	paranode->append_node(repnode);

	string str;
	rapidxml::print(std::back_inserter(str), doc, 0);
	ofstream out1(pP1OF);
	out1 << doc;
	out1.close();

	return lError;
}


long ExtractQPDProduct::ExtractLoadP2ProductXML(char *pP2OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
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
	bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile)
{
	long lError = 0;
	char *cQuickView = "";
	char *cBand = "";
	char *buf = "";
	char *temp = "";
	char *cScale = "";
	int i = 0, j = 0;

	std::locale::global(std::locale(""));
	file<> fdoc(pP2OF);

	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<>* root = doc.first_node();
	if (root == NULL)
	{
		lError = 1;
		return lError;
	}

	//==============================获取输入数据===============================================
	xml_node<> *inputnode = root->first_node("InputFilelist");
	if (inputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		//获取原始图像数据
		xml_node<>* D0Data;
		D0Data = inputnode->first_node("UVD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("UVD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("UVD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("VNIRD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("VNIRD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("VNIRD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("SWIRD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("SWIRD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("SWIRD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("TIHD0Data1");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("TIHD0Data2");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());
		D0Data = inputnode->first_node("TIHD0Data3");
		if (D0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Data.push_back(D0Data->value());

		//获取暗电流数据
		xml_node<>* D0Dark;
		D0Dark = inputnode->first_node("UVD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("UVD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("UVD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("VNIRD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("VNIRD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("VNIRD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("SWIRD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("SWIRD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("SWIRD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("TIHD0Dark1");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("TIHD0Dark2");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());
		D0Dark = inputnode->first_node("TIHD0Dark3");
		if (D0Dark == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0Dark.push_back(D0Dark->value());

		//获取0级产品元数据文件
		xml_node<>* D0XML;
		D0XML = inputnode->first_node("UVD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("UVD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("UVD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("VNIRD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("VNIRD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("VNIRD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("SWIRD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("SWIRD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("SWIRD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("TIHD0XML1");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("TIHD0XML2");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
		D0XML = inputnode->first_node("TIHD0XML3");
		if (D0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pD0XML.push_back(D0XML->value());
	}


	//==============================获取输出数据===============================================
	xml_node<> *outputnode = root->first_node("OutputFilelist");
	if (outputnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		//获取0级产品=================================
		xml_node<>* P0Data;
		P0Data = outputnode->first_node("UVP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("UVP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("UVP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("VNIRP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("VNIRP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("VNIRP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("SWIRP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("SWIRP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("SWIRP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("TIHP0Data1");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("TIHP0Data2");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());
		P0Data = outputnode->first_node("TIHP0Data3");
		if (P0Data == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0Data.push_back(P0Data->value());

		//获取0级产品元数据文件
		xml_node<>* P0XML;
		P0XML = outputnode->first_node("UVP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("UVP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("UVP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("VNIRP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("VNIRP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("VNIRP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("SWIRP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("SWIRP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("SWIRP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("TIHP0XML1");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("TIHP0XML2");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());
		P0XML = outputnode->first_node("TIHP0XML3");
		if (P0XML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0XML.push_back(P0XML->value());

		//获取0级快视图
		xml_node<>* P0JPG;
		P0JPG = outputnode->first_node("UVP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("UVP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("UVP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("VNIRP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("VNIRP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("VNIRP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("SWIRP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("SWIRP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("SWIRP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("TIHP0JPG1");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("TIHP0JPG2");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());
		P0JPG = outputnode->first_node("TIHP0JPG3");
		if (P0JPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP0JPG.push_back(P0JPG->value());

		//获取EVENT
		xml_node<>* Event;
		Event = outputnode->first_node("UVEvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("UVEvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("UVEvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("VNIREvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("VNIREvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("VNIREvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("SWIREvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("SWIREvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("SWIREvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("TIHEvent1");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("TIHEvent2");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());
		Event = outputnode->first_node("TIHEvent3");
		if (Event == NULL)
		{
			lError = 1;
			return lError;
		}
		pEvent.push_back(Event->value());

		//获取P1A级产品====================================
		xml_node<>* P1AData;
		P1AData = outputnode->first_node("UVP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("UVP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("UVP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("VNIRP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("VNIRP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("VNIRP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("SWIRP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("SWIRP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("SWIRP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("TIHP1AData1");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("TIHP1AData2");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());
		P1AData = outputnode->first_node("TIHP1AData3");
		if (P1AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AData.push_back(P1AData->value());


		//获取0级产品元数据文件
		xml_node<>* P1AXML;
		P1AXML = outputnode->first_node("UVP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("UVP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("UVP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("VNIRP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("VNIRP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("VNIRP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("SWIRP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("SWIRP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("SWIRP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("TIHP1AXML1");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("TIHP1AXML2");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());
		P1AXML = outputnode->first_node("TIHP1AXML3");
		if (P1AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AXML.push_back(P1AXML->value());

		//获取P1A快视图
		xml_node<>* P1AJPG;
		P1AJPG = outputnode->first_node("UVP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("UVP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("UVP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("VNIRP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("VNIRP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("VNIRP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("SWIRP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("SWIRP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("SWIRP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("TIHP1AJPG1");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("TIHP1AJPG2");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());
		P1AJPG = outputnode->first_node("TIHP1AJPG3");
		if (P1AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1AJPG.push_back(P1AJPG->value());

		//获取P1B级产品
		xml_node<>* P1BData;
		P1BData = outputnode->first_node("UVP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("UVP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("UVP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("VNIRP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("VNIRP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("VNIRP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("SWIRP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("SWIRP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("SWIRP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("TIHP1BData1");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("TIHP1BData2");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());
		P1BData = outputnode->first_node("TIHP1BData3");
		if (P1BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BData.push_back(P1BData->value());


		//获取P1B级产品元数据文件
		xml_node<>* P1BXML;
		P1BXML = outputnode->first_node("UVP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("UVP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("UVP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("VNIRP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("VNIRP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("VNIRP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("SWIRP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("SWIRP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("SWIRP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("TIHP1BXML1");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("TIHP1BXML2");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());
		P1BXML = outputnode->first_node("TIHP1BXML3");
		if (P1BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BXML.push_back(P1BXML->value());

		//获取P1B快视图
		xml_node<>* P1BJPG;
		P1BJPG = outputnode->first_node("UVP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("UVP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("UVP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("VNIRP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("VNIRP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("VNIRP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("SWIRP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("SWIRP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("SWIRP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("TIHP1BJPG1");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("TIHP1BJPG2");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());
		P1BJPG = outputnode->first_node("TIHP1BJPG3");
		if (P1BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1BJPG.push_back(P1BJPG->value());

		//获取P1C级产品
		xml_node<>* P1CData;
		P1CData = outputnode->first_node("UVP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("UVP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("UVP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("VNIRP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("VNIRP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("VNIRP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("SWIRP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("SWIRP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("SWIRP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("TIHP1CData1");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("TIHP1CData2");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());
		P1CData = outputnode->first_node("TIHP1CData3");
		if (P1CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CData.push_back(P1CData->value());


		//获取P1C级产品元数据文件
		xml_node<>* P1CXML;
		P1CXML = outputnode->first_node("UVP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("UVP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("UVP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("VNIRP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("VNIRP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("VNIRP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("SWIRP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("SWIRP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("SWIRP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("TIHP1CXML1");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("TIHP1CXML2");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());
		P1CXML = outputnode->first_node("TIHP1CXML3");
		if (P1CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CXML.push_back(P1CXML->value());

		//获取P1C快视图
		xml_node<>* P1CJPG;
		P1CJPG = outputnode->first_node("UVP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("UVP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("UVP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("VNIRP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("VNIRP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("VNIRP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("SWIRP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("SWIRP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("SWIRP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("TIHP1CJPG1");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("TIHP1CJPG2");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());
		P1CJPG = outputnode->first_node("TIHP1CJPG3");
		if (P1CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1CJPG.push_back(P1CJPG->value());

		//获取P1D级产品
		xml_node<>* P1DData;
		P1DData = outputnode->first_node("UVP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());
		P1DData = outputnode->first_node("VNIRP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());
		P1DData = outputnode->first_node("SWIRP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());
		P1DData = outputnode->first_node("TIHP1DData");
		if (P1DData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DData.push_back(P1DData->value());

		//获取P1D级产品元数据文件
		xml_node<>* P1DXML;
		P1DXML = outputnode->first_node("UVP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());
		P1DXML = outputnode->first_node("VNIRP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());
		P1DXML = outputnode->first_node("SWIRP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());
		P1DXML = outputnode->first_node("TIHP1DXML");
		if (P1DXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DXML.push_back(P1DXML->value());

		//获取P1D快视图
		xml_node<>* P1DJPG;
		P1DJPG = outputnode->first_node("UVP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());
		P1DJPG = outputnode->first_node("VNIRP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());
		P1DJPG = outputnode->first_node("SWIRP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());
		P1DJPG = outputnode->first_node("TIHP1DJPG");
		if (P1DJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1DJPG.push_back(P1DJPG->value());

		//获取P1E级产品
		xml_node<>* P1EData;
		P1EData = outputnode->first_node("UVP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());
		P1EData = outputnode->first_node("VNIRP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());
		P1EData = outputnode->first_node("SWIRP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());
		P1EData = outputnode->first_node("TIHP1EData");
		if (P1EData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EData.push_back(P1EData->value());

		//获取P1E级产品元数据文件
		xml_node<>* P1EXML;
		P1EXML = outputnode->first_node("UVP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());
		P1EXML = outputnode->first_node("VNIRP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());
		P1EXML = outputnode->first_node("SWIRP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());
		P1EXML = outputnode->first_node("TIHP1EXML");
		if (P1EXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EXML.push_back(P1EXML->value());

		//获取P1E快视图
		xml_node<>* P1EJPG;
		P1EJPG = outputnode->first_node("UVP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());
		P1EJPG = outputnode->first_node("VNIRP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());
		P1EJPG = outputnode->first_node("SWIRP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());
		P1EJPG = outputnode->first_node("TIHP1EJPG");
		if (P1EJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP1EJPG.push_back(P1EJPG->value());

		//获取P2A级产品
		xml_node<>* P2AData;
		P2AData = outputnode->first_node("UVP2AData");
		if (P2AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AData.push_back(P2AData->value());
		P2AData = outputnode->first_node("VNIRP2AData");
		if (P2AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AData.push_back(P2AData->value());
		P2AData = outputnode->first_node("SWIRP2AData");
		if (P2AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AData.push_back(P2AData->value());
		P2AData = outputnode->first_node("TIHP2AData");
		if (P2AData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AData.push_back(P2AData->value());

		//获取P2A级产品元数据文件
		xml_node<>* P2AXML;
		P2AXML = outputnode->first_node("UVP2AXML");
		if (P2AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AXML.push_back(P2AXML->value());
		P2AXML = outputnode->first_node("VNIRP2AXML");
		if (P2AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AXML.push_back(P2AXML->value());
		P2AXML = outputnode->first_node("SWIRP2AXML");
		if (P2AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AXML.push_back(P2AXML->value());
		P2AXML = outputnode->first_node("TIHP2AXML");
		if (P2AXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AXML.push_back(P2AXML->value());

		//获取P2A快视图
		xml_node<>* P2AJPG;
		P2AJPG = outputnode->first_node("UVP2AJPG");
		if (P2AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AJPG.push_back(P2AJPG->value());
		P2AJPG = outputnode->first_node("VNIRP2AJPG");
		if (P2AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AJPG.push_back(P2AJPG->value());
		P2AJPG = outputnode->first_node("SWIRP2AJPG");
		if (P2AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AJPG.push_back(P2AJPG->value());
		P2AJPG = outputnode->first_node("TIHP2AJPG");
		if (P2AJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2AJPG.push_back(P2AJPG->value());

		//获取P2B级产品
		xml_node<>* P2BData;
		P2BData = outputnode->first_node("UVP2BData");
		if (P2BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BData.push_back(P2BData->value());
		P2BData = outputnode->first_node("VNIRP2BData");
		if (P2BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BData.push_back(P2BData->value());
		P2BData = outputnode->first_node("SWIRP2BData");
		if (P2BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BData.push_back(P2BData->value());
		P2BData = outputnode->first_node("TIHP2BData");
		if (P2BData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BData.push_back(P2BData->value());

		//获取P2B级产品元数据文件
		xml_node<>* P2BXML;
		P2BXML = outputnode->first_node("UVP2BXML");
		if (P2BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BXML.push_back(P2BXML->value());
		P2BXML = outputnode->first_node("VNIRP2BXML");
		if (P2BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BXML.push_back(P2BXML->value());
		P2BXML = outputnode->first_node("SWIRP2BXML");
		if (P2BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BXML.push_back(P2BXML->value());
		P2BXML = outputnode->first_node("TIHP2BXML");
		if (P2BXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BXML.push_back(P2BXML->value());

		//获取P2B快视图
		xml_node<>* P2BJPG;
		P2BJPG = outputnode->first_node("UVP2BJPG");
		if (P2BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BJPG.push_back(P2BJPG->value());
		P2BJPG = outputnode->first_node("VNIRP2BJPG");
		if (P2BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BJPG.push_back(P2BJPG->value());
		P2BJPG = outputnode->first_node("SWIRP2BJPG");
		if (P2BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BJPG.push_back(P2BJPG->value());
		P2BJPG = outputnode->first_node("TIHP2BJPG");
		if (P2BJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2BJPG.push_back(P2BJPG->value());
		//获取P2C级产品
		xml_node<>* P2CData;
		P2CData = outputnode->first_node("UVP2CData");
		if (P2CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CData.push_back(P2CData->value());
		P2CData = outputnode->first_node("VNIRP2CData");
		if (P2CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CData.push_back(P2CData->value());
		P2CData = outputnode->first_node("SWIRP2CData");
		if (P2CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CData.push_back(P2CData->value());
		P2CData = outputnode->first_node("TIHP2CData");
		if (P2CData == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CData.push_back(P2CData->value());

		//获取P2C级产品元数据文件
		xml_node<>* P2CXML;
		P2CXML = outputnode->first_node("UVP2CXML");
		if (P2CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CXML.push_back(P2CXML->value());
		P2CXML = outputnode->first_node("VNIRP2CXML");
		if (P2CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CXML.push_back(P2CXML->value());
		P2CXML = outputnode->first_node("SWIRP2CXML");
		if (P2CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CXML.push_back(P2CXML->value());
		P2CXML = outputnode->first_node("TIHP2CXML");
		if (P2CXML == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CXML.push_back(P2CXML->value());

		//获取P2C快视图
		xml_node<>* P2CJPG;
		P2CJPG = outputnode->first_node("UVP2CJPG");
		if (P2CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CJPG.push_back(P2CJPG->value());
		P2CJPG = outputnode->first_node("VNIRP2CJPG");
		if (P2CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CJPG.push_back(P2CJPG->value());
		P2CJPG = outputnode->first_node("SWIRP2CJPG");
		if (P2CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CJPG.push_back(P2CJPG->value());
		P2CJPG = outputnode->first_node("TIHP2CJPG");
		if (P2CJPG == NULL)
		{
			lError = 1;
			return lError;
		}
		pP2CJPG.push_back(P2CJPG->value());

		//获取pos
		xml_node<>* POS;
		POS = outputnode->first_node("UVPOS1");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("UVPOS2");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("UVPOS3");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("VNIRPOS1");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("VNIRPOS2");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("VNIRPOS3");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("SWIRPOS1");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("SWIRPOS2");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("SWIRPOS3");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("TIHPOS1");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("TIHPOS2");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());
		POS = outputnode->first_node("TIHPOS3");
		if (POS == NULL)
		{
			lError = 1;
			return lError;
		}
		pPOS.push_back(POS->value());

		//获取Eof
		xml_node<>* Eof;
		Eof = outputnode->first_node("UVEOF1");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("UVEOF2");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("UVEOF3");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("VNIREOF1");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("VNIREOF2");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("VNIREOF3");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("SWIREOF1");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("SWIREOF2");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("SWIREOF3");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("TIHEOF1");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("TIHEOF2");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
		Eof = outputnode->first_node("TIHEOF3");
		if (Eof == NULL)
		{
			lError = 1;
			return lError;
		}
		pEOF.push_back(Eof->value());
	}



	//==============================获取参数===============================================
	xml_node<> *paranode = root->first_node("Parament");
	if (paranode == NULL)
	{
		lError = 1;
		return lError;
	}
	//获取相对辐射校正系数
	xml_node<> *recofnode = paranode->first_node("RadiateReleCof");
	if (recofnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* ReleCof;
		ReleCof = recofnode->first_node("UVReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("UVReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("UVReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("VNIRReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("VNIRReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("VNIRReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("SWIRReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("SWIRReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("SWIRReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("TIHReleCof1");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("TIHReleCof2");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
		ReleCof = recofnode->first_node("TIHReleCof3");
		if (ReleCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pReleCof.push_back(ReleCof->value());
	}

	//获取绝对辐射校正系数
	xml_node<> *abcofnode = paranode->first_node("RadiateAbCof");
	if (abcofnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* AbCof;
		AbCof = abcofnode->first_node("UVAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("UVAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("UVAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("VNIRAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("VNIRAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("VNIRAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("SWIRAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("SWIRAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("SWIRAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("TIHAbCof1");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("TIHAbCof2");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
		AbCof = abcofnode->first_node("TIHAbCof3");
		if (AbCof == NULL)
		{
			lError = 1;
			return lError;
		}
		pAbCof.push_back(AbCof->value());
	}

	//获取中心波长半波宽信息
	xml_node<> *splnode = paranode->first_node("WaveLength");
	if (splnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* WaveLen;
		WaveLen = splnode->first_node("Modtran");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pModtran = WaveLen->value();
		WaveLen = splnode->first_node("UVWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("UVWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("UVWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("VNIRWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("VNIRWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("VNIRWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("SWIRWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("SWIRWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("SWIRWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("TIHWaveLen1");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("TIHWaveLen2");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
		WaveLen = splnode->first_node("TIHWaveLen3");
		if (WaveLen == NULL)
		{
			lError = 1;
			return lError;
		}
		pWaveLen.push_back(WaveLen->value());
	}

	//获取SBET文件
	xml_node<>* sbetnode = NULL;
	sbetnode = paranode->first_node("Sbet");
	if (sbetnode == NULL)
	{
		lError = 1;
		return lError;
	}
	pSbet = sbetnode->value();

	//获取DEM文件
	xml_node<>* demnode = NULL;
	demnode = paranode->first_node("DEM");
	if (demnode == NULL)
	{
		lError = 1;
		return lError;
	}
	pDEM = demnode->value();

	//获取视场拼接参数
	xml_node<>* folapnode;
	folapnode = paranode->first_node("FieldOverLap");
	if (folapnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* FOLP;
		char *cFOLap;
		i = 0, j = 0;
		FOLP = folapnode->first_node("VNIRFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		FOLP = folapnode->first_node("SWIRFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		FOLP = folapnode->first_node("TIHFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i + 4] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j + 4] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		FOLP = folapnode->first_node("UVFieldOverLap");
		if (FOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = FOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<2)
			{
				fFOLapX[i + 6] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<2)
			{
				fFOLapY[j + 6] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}


	}

	//获取谱段拼接参数
	xml_node<>* solapnode;
	solapnode = paranode->first_node("SpectrumOverLap");
	if (solapnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* SOLP;
		char *cFOLap;
		i = 0, j = 0;
		SOLP = solapnode->first_node("UVSpectrumOverLap");
		if (SOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = SOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<1)
			{
				fSOLapX[i] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<1)
			{
				fSOLapY[j] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		SOLP = solapnode->first_node("SWIRSpectrumOverLap");
		if (SOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = SOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<1)
			{
				fSOLapX[i + 1] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<1)
			{
				fSOLapY[j + 1] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
		i = 0, j = 0;
		SOLP = solapnode->first_node("TIHSpectrumOverLap");
		if (SOLP == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFOLap = SOLP->value();
			temp = strtok_s(cFOLap, ",", &buf);
			while (temp && i<1)
			{
				fSOLapX[i + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
			while (temp && j<1)
			{
				fSOLapY[j + 2] = (float)atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				j++;
			}
		}
	}

	//获取视场角、瞬时视场角、焦距
	char *cFov, *cIFov, *cFocalLen;
	xml_node<>* geonode;
	geonode = paranode->first_node("GeoCof");
	if (geonode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* geocof;
		i = 0;
		geocof = geonode->first_node("FOV");
		if (geocof == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFov = geocof->value();
			temp = strtok_s(cFov, ",", &buf);
			while (temp && i<4)
			{
				fFov[i] = atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}

		i = 0;
		geocof = geonode->first_node("IFOV");
		if (geocof == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cIFov = geocof->value();
			temp = strtok_s(cIFov, ",", &buf);
			while (temp && i<8)
			{
				fIFov[i] = atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}

		i = 0;
		geocof = geonode->first_node("FocalLen");
		if (geocof == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cFocalLen = geocof->value();
			temp = strtok_s(cFocalLen, ",", &buf);
			while (temp && i<4)
			{
				fFocalLen[i] = atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}
	}

	char *cBoresightMis, *cGNSSOffset, *cXYZOffset, *cBands, *cSamples;
	//获取视准轴偏差和偏移距离参数
	xml_node<>* misnode;
	misnode = paranode->first_node("POSMiss");
	if (misnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* posmiss;
		i = 0;
		posmiss = misnode->first_node("BoresightMis");
		if (posmiss == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cBoresightMis = posmiss->value();
			temp = strtok_s(cBoresightMis, ",", &buf);
			while (temp && i<3)
			{
				dBoresightMis[i] = atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}
		i = 0;
		posmiss = misnode->first_node("GNSSOffset");
		if (posmiss == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cGNSSOffset = posmiss->value();
			temp = strtok_s(cGNSSOffset, ",", &buf);
			while (temp && i<3)
			{
				dGNSSOffset[i] = atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}
		i = 0;
		posmiss = misnode->first_node("XYZOffset");
		if (posmiss == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cXYZOffset = posmiss->value();
			temp = strtok_s(cXYZOffset, ",", &buf);
			while (temp && i<3)
			{
				dXYZOffset[i] = atof(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}
	}

	//获取数据压缩后的波段和像元数目
	xml_node<>* mergenode;
	mergenode = paranode->first_node("Merge");
	if (misnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* mix;
		i = 0;
		mix = mergenode->first_node("Band");
		if (mix == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cBands = mix->value();
			temp = strtok_s(cBands, ",", &buf);
			while (temp && i < 4)
			{
				nBand[i] = atoi(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}
		i = 0;
		mix = mergenode->first_node("Samples");
		if (mix == NULL)
		{
			lError = 1;
			return lError;
		}
		else
		{
			cSamples = mix->value();
			temp = strtok_s(cSamples, ",", &buf);
			while (temp && i < 4)
			{
				nSamples[i] = atoi(temp);
				temp = strtok_s(NULL, ",", &buf);
				i++;
			}
		}
	}
	//获取快视图参数
	xml_node<>* jpgnode;
	jpgnode = paranode->first_node("QuickView");
	if (jpgnode == NULL)
	{
		lError = 1;
		return lError;
	}
	else
	{
		xml_node<>* jpgpara;
		jpgpara = jpgnode->first_node("WhetherQuickView");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cQuickView = jpgpara->value();
		if (cQuickView[0] == '0')
		{
			bQuickView = 0;
		}
		else
		{
			bQuickView = 1;
		}
		jpgpara = jpgnode->first_node("QuickViewScale");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cScale = jpgpara->value();
		fScale = (float)atof(cScale);

		jpgpara = jpgnode->first_node("UVQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nUVBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("VNIRQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nVNIRBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("SWIRQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nSWIRBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
		jpgpara = jpgnode->first_node("TIHQuickBand");
		if (jpgpara == NULL)
		{
			lError = 1;
			return lError;
		}
		cBand = jpgpara->value();
		i = 0;
		temp = strtok_s(cBand, ",", &buf);
		while (temp && i<3)
		{
			nTIHBand[i] = atoi(temp);
			temp = strtok_s(NULL, ",", &buf);
			i++;
		}
	}

	//获取处理进度文件
	xml_node<>* repnode = NULL;
	repnode = paranode->first_node("ReportFile");
	if (repnode == NULL)
	{
		lError = 1;
		return lError;
	}
	pProFile = repnode->value();
	return lError;
}

long ExtractQPDProduct::ExtractSaveP2ProductXML(char *pP2OF, vector<string> &pD0Data, vector<string> &pD0XML, vector<string> &pD0Dark,
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
	bool bQuickView, bool bGrey, float fScale, int nUVBand[], int nVNIRBand[], int nSWIRBand[], int nTIHBand[], string &pProFile)
{
	long lError = 0;

	string text;
	ofstream out;

	std::locale::global(std::locale(""));
	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_pi, doc.allocate_string("xml version='1.0'encoding='utf-8'"));
	doc.append_node(root);

	xml_node<>* node = doc.allocate_node(node_element, "P2Task", NULL);
	doc.append_node(node);

	xml_node<>* inputnode = doc.allocate_node(node_element, "InputFilelist");
	if (inputnode != 0)
	{
		inputnode->append_node(doc.allocate_node(node_element, "UVD0Data1", pD0Data[0].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "UVD0Data2", pD0Data[1].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "UVD0Data3", pD0Data[2].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data1", pD0Data[3].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data2", pD0Data[4].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Data3", pD0Data[5].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data1", pD0Data[6].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data2", pD0Data[7].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Data3", pD0Data[8].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data1", pD0Data[9].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data2", pD0Data[10].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0Data3", pD0Data[11].c_str()));

		inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark1", pD0Dark[0].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark2", pD0Dark[1].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "UVD0Dark3", pD0Dark[2].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark1", pD0Dark[3].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark2", pD0Dark[4].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0Dark3", pD0Dark[5].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark1", pD0Dark[6].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark2", pD0Dark[7].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0Dark3", pD0Dark[8].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark1", pD0Dark[9].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark2", pD0Dark[10].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0Dark3", pD0Dark[11].c_str()));

		inputnode->append_node(doc.allocate_node(node_element, "UVD0XML1", pD0XML[0].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "UVD0XML2", pD0XML[1].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "UVD0XML3", pD0XML[2].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML1", pD0XML[3].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML2", pD0XML[4].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "VNIRD0XML3", pD0XML[5].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML1", pD0XML[6].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML2", pD0XML[7].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "SWIRD0XML3", pD0XML[8].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML1", pD0XML[9].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML2", pD0XML[10].c_str()));
		inputnode->append_node(doc.allocate_node(node_element, "TIHD0XML3", pD0XML[11].c_str()));

		node->append_node(inputnode);
	}

	xml_node<>* outputnode = doc.allocate_node(node_element, "OutputFilelist");
	if (outputnode != 0)
	{
		outputnode->append_node(doc.allocate_node(node_element, "UVP0Data1", pP0Data[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP0Data2", pP0Data[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP0Data3", pP0Data[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data1", pP0Data[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data2", pP0Data[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0Data3", pP0Data[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data1", pP0Data[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data2", pP0Data[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0Data3", pP0Data[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data1", pP0Data[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data2", pP0Data[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0Data3", pP0Data[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP0XML1", pP0XML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP0XML2", pP0XML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP0XML3", pP0XML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML1", pP0XML[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML2", pP0XML[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0XML3", pP0XML[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML1", pP0XML[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML2", pP0XML[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0XML3", pP0XML[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML1", pP0XML[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML2", pP0XML[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0XML3", pP0XML[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG1", pP0JPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG2", pP0JPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP0JPG3", pP0JPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG1", pP0JPG[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG2", pP0JPG[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP0JPG3", pP0JPG[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG1", pP0JPG[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG2", pP0JPG[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP0JPG3", pP0JPG[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG1", pP0JPG[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG2", pP0JPG[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP0JPG3", pP0JPG[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVEvent1", pEvent[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVEvent2", pEvent[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVEvent3", pEvent[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIREvent1", pEvent[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIREvent2", pEvent[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIREvent3", pEvent[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIREvent1", pEvent[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIREvent2", pEvent[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIREvent3", pEvent[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHEvent1", pEvent[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHEvent2", pEvent[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHEvent3", pEvent[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1AData1", pP1AData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1AData2", pP1AData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1AData3", pP1AData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AData1", pP1AData[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AData2", pP1AData[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AData3", pP1AData[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AData1", pP1AData[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AData2", pP1AData[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AData3", pP1AData[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AData1", pP1AData[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AData2", pP1AData[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AData3", pP1AData[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1AXML1", pP1AXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1AXML2", pP1AXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1AXML3", pP1AXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AXML1", pP1AXML[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AXML2", pP1AXML[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AXML3", pP1AXML[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AXML1", pP1AXML[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AXML2", pP1AXML[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AXML3", pP1AXML[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AXML1", pP1AXML[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AXML2", pP1AXML[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AXML3", pP1AXML[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1AJPG1", pP1AJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1AJPG2", pP1AJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1AJPG3", pP1AJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AJPG1", pP1AJPG[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AJPG2", pP1AJPG[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1AJPG3", pP1AJPG[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AJPG1", pP1AJPG[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AJPG2", pP1AJPG[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1AJPG3", pP1AJPG[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AJPG1", pP1AJPG[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AJPG2", pP1AJPG[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1AJPG3", pP1AJPG[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1BData1", pP1BData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1BData2", pP1BData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1BData3", pP1BData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BData1", pP1BData[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BData2", pP1BData[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BData3", pP1BData[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BData1", pP1BData[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BData2", pP1BData[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BData3", pP1BData[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BData1", pP1BData[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BData2", pP1BData[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BData3", pP1BData[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1BXML1", pP1BXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1BXML2", pP1BXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1BXML3", pP1BXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BXML1", pP1BXML[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BXML2", pP1BXML[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BXML3", pP1BXML[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BXML1", pP1BXML[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BXML2", pP1BXML[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BXML3", pP1BXML[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BXML1", pP1BXML[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BXML2", pP1BXML[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BXML3", pP1BXML[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1BJPG1", pP1BJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1BJPG2", pP1BJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1BJPG3", pP1BJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BJPG1", pP1BJPG[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BJPG2", pP1BJPG[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1BJPG3", pP1BJPG[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BJPG1", pP1BJPG[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BJPG2", pP1BJPG[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1BJPG3", pP1BJPG[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BJPG1", pP1BJPG[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BJPG2", pP1BJPG[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1BJPG3", pP1BJPG[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1CData1", pP1CData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1CData2", pP1CData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1CData3", pP1CData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CData1", pP1CData[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CData2", pP1CData[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CData3", pP1CData[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CData1", pP1CData[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CData2", pP1CData[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CData3", pP1CData[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CData1", pP1CData[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CData2", pP1CData[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CData3", pP1CData[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1CXML1", pP1CXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1CXML2", pP1CXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1CXML3", pP1CXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CXML1", pP1CXML[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CXML2", pP1CXML[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CXML3", pP1CXML[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CXML1", pP1CXML[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CXML2", pP1CXML[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CXML3", pP1CXML[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CXML1", pP1CXML[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CXML2", pP1CXML[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CXML3", pP1CXML[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1CJPG1", pP1CJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1CJPG2", pP1CJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVP1CJPG3", pP1CJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CJPG1", pP1CJPG[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CJPG2", pP1CJPG[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1CJPG3", pP1CJPG[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CJPG1", pP1CJPG[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CJPG2", pP1CJPG[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1CJPG3", pP1CJPG[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CJPG1", pP1CJPG[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CJPG2", pP1CJPG[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1CJPG3", pP1CJPG[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1DData", pP1DData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1DData", pP1DData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1DData", pP1DData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1DData", pP1DData[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1DXML", pP1DXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1DXML", pP1DXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1DXML", pP1DXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1DXML", pP1DXML[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1DJPG", pP1DJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1DJPG", pP1DJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1DJPG", pP1DJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1DJPG", pP1DJPG[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1EData", pP1EData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1EData", pP1EData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1EData", pP1EData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1EData", pP1EData[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1EXML", pP1EXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1EXML", pP1EXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1EXML", pP1EXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1EXML", pP1EXML[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP1EJPG", pP1EJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP1EJPG", pP1EJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP1EJPG", pP1EJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP1EJPG", pP1EJPG[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2AData", pP2AData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2AData", pP2AData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2AData", pP2AData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2AData", pP2AData[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2AXML", pP2AXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2AXML", pP2AXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2AXML", pP2AXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2AXML", pP2AXML[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2AJPG", pP2AJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2AJPG", pP2AJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2AJPG", pP2AJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2AJPG", pP2AJPG[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2BData", pP2BData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2BData", pP2BData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2BData", pP2BData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2BData", pP2BData[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2BXML", pP2BXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2BXML", pP2BXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2BXML", pP2BXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2BXML", pP2BXML[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2BJPG", pP2BJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2BJPG", pP2BJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2BJPG", pP2BJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2BJPG", pP2BJPG[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2CData", pP2CData[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2CData", pP2CData[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2CData", pP2CData[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2CData", pP2CData[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2CXML", pP2CXML[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2CXML", pP2CXML[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2CXML", pP2CXML[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2CXML", pP2CXML[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVP2CJPG", pP2CJPG[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRP2CJPG", pP2CJPG[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRP2CJPG", pP2CJPG[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHP2CJPG", pP2CJPG[3].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVPOS1", pPOS[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVPOS2", pPOS[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVPOS3", pPOS[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRPOS1", pPOS[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRPOS2", pPOS[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIRPOS3", pPOS[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRPOS1", pPOS[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRPOS2", pPOS[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIRPOS3", pPOS[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHPOS1", pPOS[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHPOS2", pPOS[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHPOS3", pPOS[11].c_str()));

		outputnode->append_node(doc.allocate_node(node_element, "UVEOF1", pEOF[0].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVEOF2", pEOF[1].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "UVEOF3", pEOF[2].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIREOF1", pEOF[3].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIREOF2", pEOF[4].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "VNIREOF3", pEOF[5].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIREOF1", pEOF[6].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIREOF2", pEOF[7].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "SWIREOF3", pEOF[8].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHEOF1", pEOF[9].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHEOF2", pEOF[10].c_str()));
		outputnode->append_node(doc.allocate_node(node_element, "TIHEOF3", pEOF[11].c_str()));

		node->append_node(outputnode);
	}

	xml_node<>* paranode = doc.allocate_node(node_element, "Parament");
	node->append_node(paranode);

	xml_node<>* recofnode = doc.allocate_node(node_element, "RadiateReleCof");
	recofnode->append_node(doc.allocate_node(node_element, "UVReleCof1", pReleCof[0].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "UVReleCof2", pReleCof[1].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "UVReleCof3", pReleCof[2].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "VNIRReleCof1", pReleCof[3].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "VNIRReleCof2", pReleCof[4].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "VNIRReleCof3", pReleCof[5].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "SWIRReleCof1", pReleCof[6].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "SWIRReleCof2", pReleCof[7].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "SWIRReleCof3", pReleCof[8].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "TIHReleCof1", pReleCof[9].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "TIHReleCof2", pReleCof[10].c_str()));
	recofnode->append_node(doc.allocate_node(node_element, "TIHReleCof3", pReleCof[11].c_str()));
	paranode->append_node(recofnode);

	xml_node<>* abcofnode = doc.allocate_node(node_element, "RadiateAbCof");
	abcofnode->append_node(doc.allocate_node(node_element, "UVAbCof1", pAbCof[0].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "UVAbCof2", pAbCof[1].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "UVAbCof3", pAbCof[2].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "VNIRAbCof1", pAbCof[3].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "VNIRAbCof2", pAbCof[4].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "VNIRAbCof3", pAbCof[5].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "SWIRAbCof1", pAbCof[6].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "SWIRAbCof2", pAbCof[7].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "SWIRAbCof3", pAbCof[8].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "TIHAbCof1", pAbCof[9].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "TIHAbCof2", pAbCof[10].c_str()));
	abcofnode->append_node(doc.allocate_node(node_element, "TIHAbCof3", pAbCof[11].c_str()));
	paranode->append_node(abcofnode);

	xml_node<>* splnode = doc.allocate_node(node_element, "WaveLength");
	splnode->append_node(doc.allocate_node(node_element, "Modtran", pModtran.c_str()));
	splnode->append_node(doc.allocate_node(node_element, "UVWaveLen1", pWaveLen[0].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "UVWaveLen2", pWaveLen[1].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "UVWaveLen3", pWaveLen[2].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "VNIRWaveLen1", pWaveLen[3].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "VNIRWaveLen2", pWaveLen[4].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "VNIRWaveLen3", pWaveLen[5].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "SWIRWaveLen1", pWaveLen[6].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "SWIRWaveLen2", pWaveLen[7].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "SWIRWaveLen3", pWaveLen[8].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "TIHWaveLen1", pWaveLen[9].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "TIHWaveLen2", pWaveLen[10].c_str()));
	splnode->append_node(doc.allocate_node(node_element, "TIHWaveLen3", pWaveLen[11].c_str()));
	paranode->append_node(splnode);

	xml_node<>* sbetnode = doc.allocate_node(node_element, "Sbet", pSbet.c_str());
	paranode->append_node(sbetnode);

	xml_node<>* demnode = doc.allocate_node(node_element, "DEM", pDEM.c_str());
	paranode->append_node(demnode);

	char cUVFOLap[200], cVNIRFOLap[200], cSWIRFOLap[200], cTIHFOLap[200];
	xml_node<>* folapnode = doc.allocate_node(node_element, "FieldOverLap");
	sprintf_s(cVNIRFOLap, sizeof(cVNIRFOLap), "%f,%f,%f,%f", fFOLapX[0], fFOLapX[1], fFOLapY[0], fFOLapY[1]);
	folapnode->append_node(doc.allocate_node(node_element, "VNIRFieldOverLap", cVNIRFOLap));
	sprintf_s(cSWIRFOLap, sizeof(cSWIRFOLap), "%f,%f,%f,%f", fFOLapX[2], fFOLapX[3], fFOLapY[2], fFOLapY[3]);
	folapnode->append_node(doc.allocate_node(node_element, "SWIRFieldOverLap", cSWIRFOLap));
	sprintf_s(cTIHFOLap, sizeof(cTIHFOLap), "%f,%f,%f,%f", fFOLapX[4], fFOLapX[5], fFOLapY[4], fFOLapY[5]);
	folapnode->append_node(doc.allocate_node(node_element, "TIHFieldOverLap", cTIHFOLap));
	sprintf_s(cUVFOLap, sizeof(cUVFOLap), "%f,%f,%f,%f", fFOLapX[6], fFOLapX[7], fFOLapY[6], fFOLapY[7]);
	folapnode->append_node(doc.allocate_node(node_element, "UVFieldOverLap", cUVFOLap));
	paranode->append_node(folapnode);

	char cUVSOLap[100], cSWIRSOLap[100], cTIHSOLap[100];
	xml_node<>* solapnode = doc.allocate_node(node_element, "SpectrumOverLap");
	sprintf_s(cUVSOLap, sizeof(cUVSOLap), "%f,%f", fSOLapX[0], fSOLapY[0]);
	solapnode->append_node(doc.allocate_node(node_element, "UVSpectrumOverLap", cUVSOLap));
	sprintf_s(cSWIRSOLap, sizeof(cSWIRSOLap), "%f,%f", fSOLapX[1], fSOLapY[1]);
	solapnode->append_node(doc.allocate_node(node_element, "SWIRSpectrumOverLap", cSWIRSOLap));
	sprintf_s(cTIHSOLap, sizeof(cTIHSOLap), "%f,%f", fSOLapX[2], fSOLapY[2]);
	solapnode->append_node(doc.allocate_node(node_element, "TIHSpectrumOverLap", cTIHSOLap));
	paranode->append_node(solapnode);

	char cFov[200], cIFov[200], cFocalLen[200];
	xml_node<>* gemnode = doc.allocate_node(node_element, "GeoCof");
	sprintf_s(cFov, sizeof(cFov), "%f,%f,%f,%f", fFov[0], fFov[1], fFov[2], fFov[3]);
	gemnode->append_node(doc.allocate_node(node_element, "FOV", cFov));
	sprintf_s(cIFov, sizeof(cIFov), "%f,%f,%f,%f,%f,%f,%f,%f", fIFov[0], fIFov[1], fIFov[2], fIFov[3], fIFov[4], fIFov[5], fIFov[6], fIFov[7]);
	gemnode->append_node(doc.allocate_node(node_element, "IFOV", cIFov));
	sprintf_s(cFocalLen, sizeof(cFocalLen), "%f,%f,%f,%f", fFocalLen[0], fFocalLen[1], fFocalLen[2], fFocalLen[2]);
	gemnode->append_node(doc.allocate_node(node_element, "FocalLen", cFocalLen));
	paranode->append_node(gemnode);

	char cBoresightMis[200], cGNSSOffset[200], cXYZOffset[200], cBand[50], cSamples[50];
	xml_node<>* misnode = doc.allocate_node(node_element, "POSMiss");
	sprintf_s(cBoresightMis, sizeof(cBoresightMis), "%lf,%lf,%lf", dBoresightMis[0], dBoresightMis[1], dBoresightMis[2]);
	misnode->append_node(doc.allocate_node(node_element, "BoresightMis", cBoresightMis));
	sprintf_s(cGNSSOffset, sizeof(cGNSSOffset), "%lf,%lf,%lf", dGNSSOffset[0], dGNSSOffset[1], dGNSSOffset[2]);
	misnode->append_node(doc.allocate_node(node_element, "GNSSOffset", cGNSSOffset));
	sprintf_s(cXYZOffset, sizeof(cXYZOffset), "%lf,%lf,%lf", dXYZOffset[0], dXYZOffset[1], dXYZOffset[2]);
	misnode->append_node(doc.allocate_node(node_element, "XYZOffset", cXYZOffset));
	paranode->append_node(misnode);

	xml_node<>* mergenode = doc.allocate_node(node_element, "Merge");
	sprintf_s(cBand, sizeof(cBand), "%d,%d,%d,%d", nBand[0], nBand[1], nBand[2], nBand[3]);
	mergenode->append_node(doc.allocate_node(node_element, "Band", cBand));
	sprintf_s(cSamples, sizeof(cSamples), "%d,%d,%d,%d", nSamples[0], nSamples[1], nSamples[2], nSamples[3]);
	mergenode->append_node(doc.allocate_node(node_element, "Samples", cSamples));
	paranode->append_node(mergenode);

	char cUVBand[50], cVNIRBand[50], cSWIRBand[50], cTIHBand[50];
	char cScale[50];
	xml_node<>* jpgnode = doc.allocate_node(node_element, "QuickView");
	if (bQuickView == 0)
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", "0"));
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", "-1"));
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", "-1"));
	}
	else
	{
		jpgnode->append_node(doc.allocate_node(node_element, "WhetherQuickView", "1"));
		sprintf_s(cScale, sizeof(cScale), "%f", fScale);
		jpgnode->append_node(doc.allocate_node(node_element, "QuickViewScale", cScale));
		sprintf_s(cUVBand, sizeof(cUVBand), "%d,%d,%d", nUVBand[0], nUVBand[1], nUVBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "UVQuickBand", cUVBand));
		sprintf_s(cVNIRBand, sizeof(cVNIRBand), "%d,%d,%d", nVNIRBand[0], nVNIRBand[1], nVNIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "VNIRQuickBand", cVNIRBand));
		sprintf_s(cSWIRBand, sizeof(cSWIRBand), "%d,%d,%d", nSWIRBand[0], nSWIRBand[1], nSWIRBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "SWIRQuickBand", cSWIRBand));
		sprintf_s(cTIHBand, sizeof(cTIHBand), "%d,%d,%d", nTIHBand[0], nTIHBand[1], nTIHBand[2]);
		jpgnode->append_node(doc.allocate_node(node_element, "TIHQuickBand", cTIHBand));
	}
	paranode->append_node(jpgnode);

	xml_node<>* repnode = doc.allocate_node(node_element, "ReportFile", pProFile.c_str());
	paranode->append_node(repnode);

	string str;
	rapidxml::print(std::back_inserter(str), doc, 0);
	ofstream out1(pP2OF);
	out1 << doc;
	out1.close();

	return lError;
}