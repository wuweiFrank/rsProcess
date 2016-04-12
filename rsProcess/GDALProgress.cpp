#include"GDALProgress.h"

int STD_API ALGTremProgress(double dfComplete, const char* pszMessage, void* pProgress)
{
	if (pProgress != NULL)
	{
		GDALProcessBase* pProcess = (GDALProcessBase*)pProgress;
		pProcess->m_bIsContinue = pProcess->SetPosition(dfComplete);
		if (pProcess->m_bIsContinue)
			return true;
		else
			return false;
	}
	else
		return true;
}

