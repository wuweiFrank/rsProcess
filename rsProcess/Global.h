#pragma once

//目前所有错误都是1，正确结果都返回0，等整个程序完成再对错误代码进行调整

//日志输出路径
#define _log_path_ "c:\\process_log\\rsProcess.log";

//数据头信息
typedef struct stDataInfo
{
	unsigned int	nSensorType;
	unsigned int	nSensorOrder;
	unsigned int	nWidths;
	unsigned int	nSamples;
	unsigned int	nLines;
	unsigned int	nBands;
	unsigned int	nYear;
	unsigned int	nMonth;
	unsigned int	nDay;
	unsigned int	nSecond;
	unsigned int	nMode;
	__int64 nHeadOffset;
	__int64 nEofOffset;
}DINFO;