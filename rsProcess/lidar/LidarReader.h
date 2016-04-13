#pragma once
#include"LidarDefs.h"
#include"..\GDALProgress.h"
/*读写lidar文件基类*/
class LidarReader
{
public:
	LidarReader() { m_lasFile = NULL; m_lasvarHeader = NULL; m_isDatasetOpen = false; m_Progress = NULL; }
	~LidarReader() {
		if (m_lasFile != NULL)
		{
			fclose(m_lasFile);
			m_lasFile = NULL;
		}
		if (m_lasvarHeader != NULL)
		{
			delete[]m_lasvarHeader;
			m_lasvarHeader = NULL;
		}
		m_isDatasetOpen = false;
	}

	//1.打开las文件
	virtual long LidarReader_Open(const char* pathLidar) = 0;
	//2.读取las文件
	virtual long LidarReader_Read(bool inMemory,int sample) = 0;
	//3.将las文件写入
	virtual long LidarReader_Write(const char* pathLidar) = 0;
	//4.设置进度条
	virtual void LidarReader_SetProgress(GDALProcessBase* progress) { m_Progress = progress; }

public:
	LASHeader			m_lasHeader;
	LASVariableRecord*	m_lasvarHeader;
	LASVariable_header_geo_keys m_lasgeokeyHeader;
	LASVariable_header_key_entry m_lasgeoentryHeader;
	LASSet				m_lasDataset;
	//以上为las文件数据集
	FILE				*m_lasFile;
	bool                m_isDatasetOpen;
	GDALProcessBase		*m_Progress;
};

/*LAS格式文件读写基类*/
class LASLidarReader : public LidarReader
{
public:
	//1.打开las文件，读取las文件头文件获取头文件信息
	long LidarReader_Open(const char* pathLidar);
	//2.读取数据，判断是否读取数据至内存，不读入内存则只读取index，给定采样参数
	long LidarReader_Read(bool inMemory, int readSkip =1);
	//3.将las文件写入
	long LidarReader_Write(const char* pathLidar);
};

/*XYZ格式文件读写类*/
class XYZLidarReader : public LidarReader
{
public:
	//1.打开las文件，读取las文件头文件获取头文件信息
	long LidarReader_Open(const char* pathLidar);
	//2.读取数据，判断是否读取数据至内存，不读入内存则只读取index，给定采样参数
	long LidarReader_Read(bool inMemory, int readSkip = 1);
	//3.将xyz以las文件形式写入
	long LidarReader_Write(const char* pathLidar);
	//4.将xyz文件以xyz文件形式写入
	long LidarReader_WriteXYZ(const char* pathLidar);
};