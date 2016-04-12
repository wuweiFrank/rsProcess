#pragma once

//定义进度条类获取处理进度
#ifndef _LAS_PROGRESS_
#define _LAS_PROGRESS_
#include"AuxiliaryFunction.h"
#include <string>
using namespace std;
/**
* @brief 进度条基类
*
* 提供进度条基类接口，来反映当前算法的进度值 模仿GDAL进度条
*/
class  GDALProcessBase
{
public:
	/**
	* @brief 构造函数
	*/
	GDALProcessBase()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

	/**
	* @brief 析构函数
	*/
	virtual ~GDALProcessBase() {}

	/**
	* @brief 设置进度信息
	* @param pszMsg			进度信息
	*/
	virtual void SetMessage(const char* pszMsg) = 0;

	/**
	* @brief 设置进度值
	* @param dPosition		进度值
	* @return 返回是否取消的状态，true为不取消，false为取消
	*/
	virtual bool SetPosition(double dPosition) = 0;

	/**
	* @brief 进度条前进一步，返回true表示继续，false表示取消
	* @return 返回是否取消的状态，true为不取消，false为取消
	*/
	virtual bool StepIt() = 0;

	/**
	* @brief 设置进度个数
	* @param iStepCount		进度个数
	*/
	virtual void SetStepCount(int iStepCount)
	{
		ReSetProcess();
		m_iStepCount = iStepCount;
	}

	/**
	* @brief 获取进度信息
	* @return 返回当前进度信息
	*/
	string GetMessage()
	{
		return m_strMessage;
	}

	/**
	* @brief 获取进度值
	* @return 返回当前进度值
	*/
	double GetPosition()
	{
		return m_dPosition;
	}

	/**
	* @brief 重置进度条
	*/
	void ReSetProcess()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

	/*! 进度信息 */
	string m_strMessage;
	/*! 进度值 */
	double m_dPosition;
	/*! 进度个数 */
	int m_iStepCount;
	/*! 进度当前个数 */
	int m_iCurStep;
	/*! 是否取消，值为false时表示计算取消 */
	bool m_bIsContinue;
};

/**
* @brief 控制台进度条类
*
* 提供控制台程序的进度条类接口，来反映当前算法的进度值
*/
class  GDALConsoleProcess : public GDALProcessBase
{
public:
	/**
	* @brief 构造函数
	*/
	GDALConsoleProcess()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
	};

	/**
	* @brief 析构函数
	*/
	~GDALConsoleProcess()
	{
		//remove(m_pszFile);
	};

	/**
	* @brief 设置进度信息
	* @param pszMsg			进度信息
	*/
	void SetMessage(const char* pszMsg)
	{
		m_strMessage = pszMsg;
		printf("%s\n", pszMsg);
	}

	/**
	* @brief 设置进度值
	* @param dPosition		进度值
	* @return 返回是否取消的状态，true为不取消，false为取消
	*/
	bool SetPosition(double dPosition)
	{
		m_dPosition = dPosition;
		TermProgress(m_dPosition);
		m_bIsContinue = true;
		return true;
	}

	/**
	* @brief 进度条前进一步
	* @return 返回是否取消的状态，true为不取消，false为取消
	*/
	bool StepIt()
	{
		m_iCurStep++;
		m_dPosition = m_iCurStep*1.0 / m_iStepCount;

		TermProgress(m_dPosition);
		m_bIsContinue = true;
		return true;
	}

private:
	void TermProgress(double dfComplete)
	{
		static int nLastTick = -1;
		int nThisTick = (int)(dfComplete * 40.0);

		nThisTick = min(40, max(0, nThisTick));

		// Have we started a new progress run?  
		if (nThisTick < nLastTick && nLastTick >= 39)
			nLastTick = -1;

		if (nThisTick <= nLastTick)
			return;

		while (nThisTick > nLastTick)
		{
			nLastTick++;
			if (nLastTick % 4 == 0)
				fprintf(stdout, "%d", (nLastTick / 4) * 10);
			else
				fprintf(stdout, ".");
		}

		if (nThisTick == 40)
			fprintf(stdout, " - done.\n");
		else
			fflush(stdout);
	}
};
#ifndef STD_API
#define STD_API __stdcall
#endif

/*****************************************************************************
* @brief : 进度条函数将GDAL算法中的进度导入进度条
* @author : W.W.Frank
* @date : 2015/12/3 21:29
* @version : version 1.0
* @inparam :
* @outparam :
*****************************************************************************/
int STD_API ALGTremProgress(double dfComplete, const char* pszMessage, void* pProgress);

#endif