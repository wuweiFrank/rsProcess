#pragma once
#include<time.h>
#include<vector>
using namespace std;
#include"Geometry.h"
#include"..\RTree.h"

//定义读取数据的最大内存量为512MB
#define LargestMemoryToRead 536870912
#define BlockPointNumbers 2048
//================================================las头文件定义=========================================================
// 地面类型
enum  LAS_CLASSIFICATION_TYPE
{
	CLASSIFICATION_NULL = 0,	// 未设置类别
	CLASSIFICATION_UNCLASSIFIED = 1,
	CLASSIFICATION_GROUND = 2,
	CLASSIFICATION_LOW_VEGETATION = 3,
	CLASSIFICATION_MEDIUM_VEGETATION = 4,
	CLASSIFICATION_HIGH_VEGETATION = 5,
	CLASSIFICATION_BUILDING = 6,
	CLASSIFICATION_LOW_POINT_NOISE = 7,
	CLASSIFICATION_MEDEL_KEYPOINT = 8,
	CLASSIFICATION_WATER = 9,
	CLASSIFICATION_OVERLAP_POINTS2 = 12,
	CLASSIFICATION_reserved						// 保留使用
};

#pragma pack(1)/*使得字节对齐*/
class LASHeader
{
public:

	// LASHeader在文件中的大小dpw
	// sizeof(LASHeader)计算的结果可能比这个大
	// 因为编译器可能会做一些优化(如边界对齐等) 
	// 加上颜色扩展
	enum { HEADER_SIZE = 227 };

public:												// 必须 说明		默认值	字节编号
	char file_signature[4];							// *	文件标志	"LASF"	1-4
													//unsigned short file_source_id;				// *	文件编号	0		
	/*
	* This data field is reserved and must be zero filled by generating generating_software.
	* 1.0版las文件为4字节；1.1版为2字节，与上面的File_Source_ID合用4个字节
	*/
	unsigned long  reserved;						// -	保存使用	5-8		5-8
	//unsigned short filesourceid;
	unsigned long  project_ID_GUID_data_1;			// -	4字节		0		9-12
	unsigned short project_ID_GUID_data_2;			// -	2字节		0		13-14
	unsigned short project_ID_GUID_data_3;			// -	2字节		0		15-16
	unsigned char project_ID_GUID_data_4[8];		// -	8字节		0		17-24
	unsigned char version_major;					// *	主版本号			25
	unsigned char version_minor;					// *	副版本号			26
	unsigned char system_id[32];					// *	系统标识			27-58
	char generating_software[32];					// *	生成软件			59-90
	unsigned short file_creation_day;				// -	创建时间			91-92
	unsigned short file_creation_year;				// -						93-94
	unsigned short header_size;						// *	Head大小			95-96
	unsigned int offset_to_point_data;				// *	数据地址			97-100
	unsigned int number_of_variable_length_records; // *	变长记录数目		101-104
	unsigned char point_data_format;				// *	点数据格式GPS		105
	unsigned short point_data_record_length;		// *	点数据长度			106-107
	unsigned long number_of_point_records;			// *	点的数目			108-111
	unsigned long number_of_points_by_return[5];	// *	回波返回数			112-131

	/*
	* Xcoordinate = (Xrecord * x_scale_factor) + x_offset
	* Ycoordinate = (Yrecord * y_scale_factor) + y_offset
	* Zcoordinate = (Zrecord * z_scale_factor) + z_offset
	*/
	double x_scale_factor;							// *	缩放系数			132-139
	double y_scale_factor;							// *	缩放系数			140-147
	double z_scale_factor;							// *	缩放系数			148-155

	double x_offset;								// *	相对偏移			156-163	
	double y_offset;								// *	相对偏移			170-177
	double z_offset;								// *	相对偏移			164-169

	double max_x;									// *	x最大值				178-195
	double min_x;									// *	x最小值				186-193
	double max_y;									// *						194-201			
	double min_y;									// *						202-209
	double max_z;									// *						210-217
	double min_z;									// *						218-225

public:
	/*
		构造和析构
	*/
	LASHeader();
	LASHeader(const LASHeader& header);
	LASHeader& operator=(const LASHeader& header);

	/*
		数据集ID
	*/
	unsigned short GetFile_Source_ID() { return (version_minor == 1 && version_minor == 1) ? reserved >> 16 : 0; }
	void SetFile_Source_ID(unsigned short id);

	/*
		数据集保留位
	*/
	unsigned int Getreserved() { return (version_minor == 1 && version_minor == 1) ? reserved &= 0x0000FFFF : reserved; }
	void Setreserved(unsigned int reservedi);

	bool HasPoint()		   const
	{
		if (point_data_record_length >= 12)
			return true;
		else
			return false;
	}
	bool HasIntensity()	   const
	{
		if (point_data_record_length >= 14)
			return true;
		else
			return false;
	}
	bool HasReturnNumber() const
	{
		if (point_data_record_length >= 15)
			return true;
		else
			return false;
	}
	bool HasNumberofReturn()const
	{
		if (point_data_record_length >= 15)
			return true;
		else
			return false;
	}
	bool HasScanDirectionFlag() const
	{
		if (point_data_record_length >= 15)
			return true;
		else
			return false;
	}
	bool HasEdgeofFlightLine()const
	{
		if (point_data_record_length >= 15)
			return true;
		else
			return false;
	}
	bool HasScanAgnleRank()	const
	{
		if (point_data_record_length >= 17)
			return true;
		else
			return false;
	}
	bool HasFlightLine()const
	{
		if (point_data_record_length >= 17)
			return true;
		else
			return false;
	}
	bool HasGPSTime()const;

	bool HasLASColorExt4() const;	//是否有颜色 四字节定义
	bool HasLASColorExt6() const;	//是否有颜色 六字节定义
	int  HasLASPointExt()  const;	//数据格式是否有扩展
	bool HasPointDataStartfile_signature() const { return version_minor == 0 && version_minor == 1; }
	tm*  GetDataTime() {
		time_t timer;
		time(&timer);
		return localtime(&timer);
	};				//获取时间

	void LASHeader_Read(FILE *fs);		//读写数据头文件
	void LASHeader_Write(FILE *fs) const;

	const static unsigned short Data_Record_Length_of_Format0; // 20，不扩展激光点结构时的0格式点标准长度
	const static unsigned short Data_Record_Length_of_Format1; // 28，不扩展激光点结构时的1格式点标准长度
	const static unsigned short Data_Record_Length_of_Format2; // 26，1.2不扩展激光点结构时的1格式点标准长度
	const static unsigned short Data_Record_Length_of_Format3; // 34，1.2不扩展激光点结构时的1格式点标准长度
	const static unsigned short HeaderSize_Def;				   // 227，标准头文件长度

	const static char ErrorPointCnt[];			 // "number of point is more than LasFileHead.Number_of_point_records";
	const static char ErrorVarLenRecordCnt[];	 // "number of variable Length Record is more than LasFileHead.Number_of_variable_length_records";
	const static char ErrorOffsettoData[];		 // "number of bytes of Variable Length Record is more than LasFileHead.Offset_to_data";
};

// 变长记录头部
#pragma pack(1)
#define min_las_record_after_header 54;
class  LASVariableRecord
{
public:											// 必须	描述	默认值
	LASVariableRecord() {
		reserved = 0xAABB;
		memset(user_id, 0, sizeof(char) * 16);
		record_id = 0;
		record_length_after_header = min_las_record_after_header;
		memset(description, 0, sizeof(char));
		record_buffer = NULL;
	}
	~LASVariableRecord()
	{
		if (record_buffer != NULL)
			delete[]record_buffer;
	}
	unsigned short reserved;					// -	保留	0
	char user_id[16];							// *			LASF_Specl/LASF_Projection
	unsigned short record_id;					// *	编号
	unsigned short record_length_after_header;	// *	后续记录大小
	char description[32];						// -	数据说明
	unsigned char* record_buffer;

	void LASVariableRecord_Read(FILE* fs);
	void LASVariableRecord_Write(FILE* fs) const;

	//Two bytes after the last variable length record, and before the point data
	const static unsigned short Min_Record_Length_After_Header;  // 54
	const static short Point_Data_Start_Signature;
};

// 官方定义的变长记录结构
// User ID: LASF_Projection
// Record ID: 34735
class  LASVariable_header_geo_keys
{
public:
	unsigned short key_directory_version;
	unsigned short key_revision;
	unsigned short minor_revision;
	unsigned short number_of_keys;

	LASVariable_header_geo_keys()
	{
		key_directory_version = 1;	// Always
		key_revision = 1;	// Always
		minor_revision = 0;	// Always
	}
};
class  LASVariable_header_key_entry
{
public:
	unsigned short key_id;
	unsigned short tiff_tag_location;
	unsigned short count;
	unsigned short value_offset;
};



//=======================================================las xyz点文件========================================================
#ifndef COLORREF
#define COLORREF long
#endif

#ifndef RGB
#define RGB(r,g,b)          ((COLORREF)(((unsigned char)(r)|((unsigned long)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))
#endif

typedef RTree<int, double, 2, double, 4>  LASBlockTree;

/*激光回波信号*/
enum  eLASEcho
{
	eLidarEchoOnly = 0,
	eLidarEchoFirst = 1,
	eLidarEchoMidian = 2,
	eLidarEchoLast = 3
};
/*点云的类别*/
#pragma pack(1)
enum  eLASClassification
{
	elcCreated = 0,	// 创建的，没有被分类的
	elcUnclassified = 1,	// 无类别的，或无法识别类别的点
	elcGround = 2,	// 地面点
	elcLowVegetation = 3,	// 矮的植被
	elcMediumVegetation = 4,	// 中等高度的植被
	elcHighVegetation = 5,	// 高的植被
	elcBuilding = 6,	// 建筑物
	elcLowPoint = 7,	// 低于地表的点（噪音）
	elcModelKeyPoint = 8,	// 控制点
	elcWater = 9,	// 水
	elcOverlapPoint = 12,	// 航带重叠点
	elcDeletedPoint = -1	// 已删除的点
};
#pragma pack()
static eLASClassification GetLidarClassification(unsigned char clsType)
{
	return (eLASClassification)clsType;
}
/*Las1.2颜色扩展*/
struct LASColorExt
{
	unsigned short Red;
	unsigned short Green;
	unsigned short Blue;
};

/*点云XYZ文件*/
#pragma pack(1)
struct LAS_XYZ
{
	double x;
	double y;
	double z;
};

//las点文件
#pragma pack(1)/*字节对齐*/
struct LASPoint
{
public:
	LAS_XYZ			m_vec3d;
	unsigned short  m_intensity;
	unsigned char   m_rnseByte;
	char			m_classify;
	char			m_scanAngle;
	unsigned char	m_userdata;
	unsigned short  m_flightID;
	double			m_gpsTime;
	LASColorExt		m_colorExt;
};

//las块文件，将点文件分块得到的数据
class LASRectangle
{
public:
	LASRectangle() { m_lasPoints = NULL; m_point_index = NULL; m_lasPoints_numbers = 0; }
	~LASRectangle(){
		if (m_lasPoints != NULL)
			delete[]m_lasPoints;
		m_lasPoints = NULL;
		m_lasPoints_numbers = 0;
	}

	//分配内存
	void LASRect_AllocateMemory(int lasPoints,bool inMemory,Rect rect);
public:
	Rect m_Rectangle;
	LASPoint* m_lasPoints;
	long long m_lasPoints_numbers;
	int *m_point_index;
};

//整体的las文件
class LASSet
{
public:
	LASSet() { m_lasRectangles = NULL; m_lasBlockTree.RemoveAll(); m_numRectangles = 0; }
	~LASSet(){
		printf("正在释放内存...\n");
		if (m_lasRectangles != NULL)
			delete[]m_lasRectangles;
		m_lasRectangles = NULL;
		m_lasBlockTree.RemoveAll();
	}

	//找到匹配的矩形的id，根据id获取在哪个矩形中
	bool LASRect_Search(int rectID, Rect searchRect, vector<int> &rects);
	bool LASRect_Search(int rectID, LAS_XYZ searchPnt, vector<int> &rects);

	//构建R树的过程
	long LASRect_BuildTree();

	//分配内存
	void LASRect_AllocateMemory(int lasRects, bool inMemory);

public:
	LASRectangle* m_lasRectangles;
	LASBlockTree  m_lasBlockTree;
	int			  m_numRectangles;
};