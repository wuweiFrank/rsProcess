#include"stdafx.h"
#include"PreProcess.h"
#include"AuxiliaryFunction.h"
#include"Global.h"

long QPDPreProcess::PreProc_GetHeadInfo(FILE *fRAW, DINFO &mDataHeader, vector<short> &nLeakFrameType, vector<int> &nLeakFrameSize, int &nLeakFrameCount)
{
	unsigned char cTemp = 0;
	unsigned short nTemp = 0;
	unsigned int lTemp = 0;
	unsigned char cPacktype = 0;
	unsigned short SOF = 0, nSamples = 0, nWidths = 0, nBands = 0, nLines = 0;
	unsigned short nfline = 0, nlline = 0, ndif = 0;
	unsigned long SyncHeader = 0;
	long lError = 0;
	__int64 nOffset = 0, nOffset2 = 0;
	__int64 nStartPos = 0, nEndPos = 0, nHeadOffset = 0, nFileSize = 0, nEofOffset = 0;
	const __int64 nPacktypeOffset = 9;	//包类型字节偏移
	__int64 nMisByteCount = 0;
	int i = 0, j = 0, k = 0;
	unsigned int nFramesize = 0, nWidthsize = 0;

	//获取包长度（行像元数），第一个包起始位置偏移
	memset(&mDataHeader, 0, sizeof(DINFO));
	nOffset = 0;

	//找到第一个完整的数据包的起始位置的偏移，因为第一帧也有可能出现漏帧的现象
	do
	{
		_fseeki64(fRAW, nOffset, SEEK_SET);
		fread_s(&SOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
		fread_s(&SyncHeader, sizeof(unsigned long), sizeof(unsigned long), 1, fRAW);
		if (SOF == 0xFDFD && SyncHeader == 0x7F7F7F7F)
		{
			fread_s(&nSamples, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
			if (nSamples == 320 || nSamples == 640 || nSamples == 1024 || nSamples == 2048)
			{
				mDataHeader.nSamples = nSamples;
				nWidths = nSamples + 8;
				mDataHeader.nWidths = nWidths;
				nWidthsize = nWidths*sizeof(unsigned short);
				nStartPos = nOffset;
				break;
			}
		}
		nOffset++;
	} while (!feof(fRAW));

	//获取最后一个包结尾位置，包结尾标志位0xFBFB
	_fseeki64(fRAW, 0, SEEK_END);
	nFileSize = _ftelli64(fRAW);
	nOffset = nFileSize - 2;
	do
	{
		_fseeki64(fRAW, nOffset, SEEK_SET);
		fread_s(&nTemp, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
		if (nTemp == 0xFBFB)
		{
			nEndPos = nOffset + 2;
			break;
		}
		nOffset--;
	} while (nOffset>0);

	//获取首帧图像的位置偏移||方法为找到包类型为0xAA的位置减掉（包类型位置偏移+第一个包起始位置偏移+漏非整数行大小）
	nOffset = 0;
	i = 0;
	do
	{
		//假设第一行不存在漏行的情况
		nOffset = i*nWidthsize + nPacktypeOffset + nMisByteCount + nStartPos;
		_fseeki64(fRAW, nOffset, SEEK_SET);
		fread_s(&cPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
		//判断
		if (cPacktype == 0xAA)
		{
			break;
		}
		else if (cPacktype == 0x55)
		{
			i++;
		}
		else
		{
			//出现首帧前漏非整数行
			j = 0;
			do
			{
				j++;
				nOffset = i*nWidthsize + nMisByteCount + nStartPos + j;//////////向下搜索
				_fseeki64(fRAW, nOffset, SEEK_SET);
				fread_s(&SOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
				if (SOF == 0xFDFD)
				{
					nOffset = i*nWidthsize + nPacktypeOffset + nMisByteCount + nStartPos + j;
					_fseeki64(fRAW, nOffset, SEEK_SET);
					fread_s(&cPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
					if (cPacktype == 0xAA)
					{
						//判定为辅助数据行
						nMisByteCount = nMisByteCount + j;
						break;
					}
					else if (cPacktype == 0x55)
					{
						//判定为图像数据行
						nMisByteCount = nMisByteCount + j;
						i++;
						break;
					}
				}
			} while (nOffset < nEndPos);
		}
	} while (!feof(fRAW) && cPacktype != 0xAA);
	nHeadOffset = nOffset - nPacktypeOffset;
	mDataHeader.nHeadOffset = nHeadOffset;

	//获取包信息,包括：传感器类型，相机序号，数据获取的年月日，波段数和成像模式
	_fseeki64(fRAW, nHeadOffset + 8, SEEK_SET);
	fread_s(&cTemp, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
	mDataHeader.nSensorOrder = cTemp % 16;
	mDataHeader.nSensorType = cTemp / 16;

	_fseeki64(fRAW, nHeadOffset + 14, SEEK_SET);
	fread_s(&nTemp, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
	mDataHeader.nYear = nTemp;
	fread_s(&cTemp, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
	mDataHeader.nDay = cTemp;
	fread_s(&cTemp, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
	mDataHeader.nMonth = cTemp;

	_fseeki64(fRAW, nHeadOffset + 24, SEEK_SET);
	fread_s(&nBands, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
	mDataHeader.nBands = nBands;

	_fseeki64(fRAW, nHeadOffset + 33, SEEK_SET);
	fread_s(&cTemp, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
	mDataHeader.nMode = cTemp;

	nFramesize = nWidthsize*(nBands + 1);
	//获取图像尾的位置||方法找到行号计数为总波段的行位置
	nMisByteCount = 0;
	nOffset = 0;
	i = 1;
	do
	{
		nOffset = nEndPos - i*nWidthsize + nMisByteCount;
		_fseeki64(fRAW, nOffset, SEEK_SET);
		fread_s(&SOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
		if (SOF == 0xFDFD)
		{
			nOffset = nEndPos - i*nWidthsize - nMisByteCount + 12;
			_fseeki64(fRAW, nOffset, SEEK_SET);
			fread_s(&nTemp, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
			if (nTemp == nBands)
			{
				nEofOffset = nEndPos - (i - 1)*nWidthsize + nMisByteCount;
				mDataHeader.nEofOffset = nEofOffset;
				break;
			}
			else
			{
				i++;
				i += nTemp;		//待验证
			}
		}
		else
		{
			nMisByteCount++;
		}
	} while (nOffset > 0);
	nEofOffset = nEndPos - (i - 1)*nWidthsize + nMisByteCount;
	return lError;
}

long QPDPreProcess::PreProc_LeakLineCheck(const char *pRAWData,  DINFO &mDataHeader, vector<short> &nLeakFrameType, vector<int> &nLeakFrameSize, int &nLeakFrameCount)
{
	errno_t err = 0;
	long lError = 0;
	unsigned short nSamples = 0, nWidths = 0, nBands = 0, nLines = 0, nHeight = 0;
	__int64 nHeadOffset = 0, nEofOffset = 0;
	__int64 nFramesize = 0, nWidthsize = 0;
	unsigned short nSOF = 0, ntmpSOF = 0;
	unsigned short nEOF = 0, ntmpEOF = 0;
	unsigned char cPacktype = 0, ctmpPacktype = 0;
	unsigned short nLineID = 0, ntmpLineID = 0;
	int ndif = 0;
	__int64 nOffset = 0;
	__int64 nMisByteCount = 0;

	const int nPacktypeOffset = 9;	//包类型字节偏移
	const int nLineIDOffset = 10;	//帧计数字节偏移
	int i = 0, j = 0, k = 0;
	int m = 0, n = 0;
	vector<short> btmpMiss;
	vector<short> nFrameID, nBandID, nMissbyte;
	bool bAssistLeak = 0;
	FILE *fRAW = NULL;

	//获取数据信息头
	//DINFO mDataHeader;
	//vector<short> nLeakFrameType;
	//vector<int> nLeakFrameSize;
	//int nLeakFrameCount;

	err = fopen_s(&fRAW, pRAWData, "rb");
	if (err != 0)
		exit(-1);	//此处应有日志记录
	lError=PreProc_GetHeadInfo(fRAW, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);
	if (lError != 0)
		exit(-1);//错误提示，日志

	//对漏行数据进行检测
	nSamples = mDataHeader.nSamples;
	nWidths = mDataHeader.nWidths;
	nBands = mDataHeader.nBands;
	nHeadOffset = mDataHeader.nHeadOffset;
	nEofOffset = mDataHeader.nEofOffset;
	nHeight = nBands + 1;
	nWidthsize = nWidths*sizeof(unsigned short);
	nFramesize = nWidthsize*nHeight;

	nLeakFrameCount = 0;
	//得到第一帧的SOF 包类型 帧计数 EOF
	nOffset = nHeadOffset;
	_fseeki64(fRAW, nOffset, SEEK_SET);
	fread_s(&ntmpSOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
	_fseeki64(fRAW, nOffset + nPacktypeOffset, SEEK_SET);
	fread_s(&ctmpPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
	_fseeki64(fRAW, nOffset + nLineIDOffset, SEEK_SET);
	fread_s(&ntmpLineID, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);

	i = 1;
	nOffset = nHeadOffset + nFramesize;
	do
	{
		_fseeki64(fRAW, nOffset, SEEK_SET);
		fread_s(&nSOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
		_fseeki64(fRAW, nOffset + nPacktypeOffset, SEEK_SET);
		fread_s(&cPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
		if (ntmpSOF == 0xFDFD && ctmpPacktype == 0xAA && nSOF == 0xFDFD && cPacktype == 0xAA)
		{
			//无漏行或者漏整数帧ene
			_fseeki64(fRAW, nOffset + nLineIDOffset, SEEK_SET);
			fread_s(&nLineID, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
			ndif = abs(nLineID - ntmpLineID - 1) % 65535;
			nLeakFrameType.push_back(0);
			nLeakFrameSize.push_back(0);
			nLines++;
			if (ndif != 0)
			{
				//漏整数帧
				nLines += ndif;
				nLeakFrameCount += ndif;
				for (j = 0; j < ndif; j++)
				{
					nLeakFrameType.push_back(1);
					nLeakFrameSize.push_back((int)nFramesize);
				}
			}
		}
		else if (ntmpSOF == 0xFDFD && ctmpPacktype == 0xAA && nSOF == 0xFDFD && cPacktype == 0x55)
		{
			//漏行
			bAssistLeak = 1;
			m++;
			for (j = 1; j < nBands; j++)
			{
				nOffset = nHeadOffset + __int64(i*nFramesize) - nMisByteCount - j*nWidthsize;
				_fseeki64(fRAW, nOffset, SEEK_SET);
				fread_s(&nSOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
				_fseeki64(fRAW, nOffset + nPacktypeOffset, SEEK_SET);
				fread_s(&cPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);

				if (nSOF == 0xFDFD && cPacktype == 0xAA)
				{
					bAssistLeak = 0;
					_fseeki64(fRAW, nOffset + nLineIDOffset, SEEK_SET);
					fread_s(&nLineID, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
					ndif = abs(nLineID - ntmpLineID - 1) % 65535;

					nLeakFrameCount++;
					nMisByteCount += j*nWidthsize;
					nLeakFrameType.push_back(2);
					nLeakFrameSize.push_back(j);
					nLines++;

					if (ndif != 0)
					{//漏行又漏整数帧
						nLines += ndif;
						nLeakFrameCount += ndif;
						for (j = 0; j < ndif; j++)
						{
							nLeakFrameType.push_back(1);
							nLeakFrameSize.push_back((int)nFramesize);
						}
					}
					break;
				}
			}
			if (bAssistLeak == 1)
			{
				//漏行,下一帧遗漏帧头
				for (j = 1; j < nBands; j++)
				{
					nOffset = nHeadOffset + __int64(i*nFramesize) - nMisByteCount + j*nWidthsize;
					_fseeki64(fRAW, nOffset, SEEK_SET);
					fread_s(&nSOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
					_fseeki64(fRAW, nOffset + nPacktypeOffset, SEEK_SET);
					fread_s(&cPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);

					if (nSOF == 0xFDFD && cPacktype == 0xAA)
					{
						bAssistLeak = 0;
						_fseeki64(fRAW, nOffset + nLineIDOffset, SEEK_SET);
						fread_s(&nLineID, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
						ndif = abs(nLineID - ntmpLineID - 2) % 65535;

						nMisByteCount += (nHeight - j)*nWidthsize;

						nLeakFrameCount++;
						nLeakFrameType.push_back(3);
						nLeakFrameSize.push_back((nHeight - j - 1)*nWidthsize);
						nLines++;

						nLeakFrameCount++;
						nLeakFrameType.push_back(3);
						nLeakFrameSize.push_back(nWidthsize);
						nLines++;

						i++;
						if (ndif != 0)
						{//漏行又漏整数帧
							nLines += ndif;
							nLeakFrameCount += ndif;
							for (j = 0; j < ndif; j++)
							{
								nLeakFrameType.push_back(1);
								nLeakFrameSize.push_back((int)nFramesize);
							}
						}
						break;
					}
				}
			}
		}
		else
		{
			//漏非整数行
			n++;
			bAssistLeak = 1;
			for (j = 1; j < nFramesize; j++)
			{
				nOffset = nHeadOffset + __int64(i*nFramesize) - nMisByteCount - j;
				_fseeki64(fRAW, nOffset, SEEK_SET);
				fread_s(&nSOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
				_fseeki64(fRAW, nOffset + nPacktypeOffset, SEEK_SET);
				fread_s(&cPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
				if (nSOF == 0xFDFD && cPacktype == 0xAA)
				{
					bAssistLeak = 0;
					_fseeki64(fRAW, nOffset + nLineIDOffset, SEEK_SET);
					fread_s(&nLineID, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
					ndif = abs(nLineID - ntmpLineID - 1) % 65535;

					nLeakFrameType.push_back(3);
					nLeakFrameSize.push_back(j);
					nLeakFrameCount++;
					nMisByteCount += j;
					nLines++;

					if (ndif != 0)
					{//漏字节又漏整数帧
						nLines += ndif;
						nLeakFrameCount += ndif;
						for (j = 0; j < ndif; j++)
						{
							nLeakFrameType.push_back(1);
							nLeakFrameSize.push_back((int)nFramesize);
						}
					}
					break;
				}
			}
			if (bAssistLeak == 1)
			{//漏行,下一帧遗漏帧头
				for (j = 1; j < nFramesize; j++)
				{
					nOffset = nHeadOffset + __int64(i*nFramesize) - nMisByteCount + j;
					_fseeki64(fRAW, nOffset, SEEK_SET);
					fread_s(&nSOF, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
					_fseeki64(fRAW, nOffset + nPacktypeOffset, SEEK_SET);
					fread_s(&cPacktype, sizeof(unsigned char), sizeof(unsigned char), 1, fRAW);
					if (nSOF == 0xFDFD && cPacktype == 0xAA)
					{
						bAssistLeak = 0;
						_fseeki64(fRAW, nOffset + nLineIDOffset, SEEK_SET);
						fread_s(&nLineID, sizeof(unsigned short), sizeof(unsigned short), 1, fRAW);
						ndif = abs(nLineID - ntmpLineID - 2) % 65535;


						nMisByteCount += nFramesize - j;

						nLeakFrameCount++;
						nLeakFrameType.push_back(3);
						nLeakFrameSize.push_back(j - 1);
						nLines++;

						nLeakFrameCount++;
						nLeakFrameType.push_back(3);
						nLeakFrameSize.push_back(1);
						nLines++;

						i++;
						if (ndif != 0)
						{//漏字节又漏整数帧
							nLines += ndif;
							nLeakFrameCount += ndif;
							for (j = 0; j < ndif; j++)
							{
								nLeakFrameType.push_back(1);
								nLeakFrameSize.push_back((int)nFramesize);
							}
						}
						break;
					}
				}
			}
		}

		ntmpSOF = nSOF;
		ctmpPacktype = cPacktype;
		ntmpLineID = nLineID;
		i++;
		nOffset = nHeadOffset + __int64(i*nFramesize) - nMisByteCount;
		printf("proc : %d \r", i);
	} while (nOffset < nEofOffset);
	printf("\n");

	//判断最后一帧是否存在漏帧
	ndif = int(nOffset - nEofOffset);
	if (ndif == 0)
	{
		nLeakFrameType.push_back(0);
		nLeakFrameSize.push_back(0);
		nLines++;
	}
	else if (ndif%nWidthsize == 0)
	{
		nLeakFrameType.push_back(2);
		nLeakFrameSize.push_back(int(ndif / nWidthsize));
		nLines++;
	}
	else
	{
		nLeakFrameType.push_back(3);
		nLeakFrameSize.push_back(ndif - 1);
		nLines++;
	}
	mDataHeader.nLines = nLeakFrameType.size();

	return lError;

}

long QPDPreProcess::PreProc_LeakLineInterpolate(FILE *fRAW, unsigned short *pRepairBuffer, DINFO mDataHeader, vector<short> nLeakFrameType, vector<int> nLeakFrameSize,
																	  int nLoc, __int64 nOffset, unsigned short *pfBuffer, unsigned short *plBuffer, unsigned short *pLeakBuffer)
{
	//==========变量定义=============
	long lError = 0;
	int i = 0, j = 0, k = 0, m = 0;
	unsigned int nFramesize = 0, nWidthsize = 0;
	__int64 nLineOffset = 0, nMisByteCount = 0;

	int nAcBands = 0;
	unsigned short nBandID = 0;
	int nfValid = 0, nlValid = 0;
	float ffWeight = 0, flWeight = 0;

	const int nFrameIDOffset = 5;
	const int nBandIDOffset = 6;

	int nSamples = 0, nWidths = 0, nHeight = 0, nLines = 0, nFrame = 0;

	unsigned short  SOF = 0, SyncHeader1 = 0, SyncHeader2 = 0, nFrameID = 0, nPacktype = 0;

	//=================================
	nSamples = mDataHeader.nSamples;
	nWidths = mDataHeader.nWidths;
	nLines = mDataHeader.nLines;
	nHeight = mDataHeader.nBands + 1;
	nFrame = nHeight*nWidths;

	nWidthsize = nWidths*sizeof(unsigned short);
	nFramesize = nHeight*nWidthsize;

	memset(pRepairBuffer, 0, nFramesize);
	memset(pfBuffer, 0, nWidthsize);
	memset(plBuffer, 0, nWidthsize);
	memset(pLeakBuffer, 0, nFramesize);

	//漏整数帧
	if (nLeakFrameType[nLoc] == 1)
	{
		nAcBands = 0;

		nMisByteCount = 0;
		for (k = nLoc - 1; k >= 0; k--)
		{//找到左侧差值行
			if (nLeakFrameType[k] == 0)
			{
				nfValid = k;
				nLineOffset = nOffset - (nLoc - k)*nFramesize + nMisByteCount;
				_fseeki64(fRAW, nLineOffset, SEEK_SET);
				fread_s(pfBuffer, nFramesize, sizeof(unsigned short), nFrame, fRAW);
				break;
			}
			else if (nLeakFrameType[k] == 1)
			{
				nMisByteCount += nLeakFrameSize[k];
			}
			else if (nLeakFrameType[k] == 2)
			{
				nMisByteCount += nLeakFrameSize[k] * nWidthsize;
			}
			else
			{
				nMisByteCount += nLeakFrameSize[k];
			}
		}
		nMisByteCount = nLeakFrameSize[nLoc];
		for (k = nLoc + 1; k < nLines; k++)
		{
			//找到右侧差值行
			if (nLeakFrameType[k] == 0)
			{
				nlValid = k;
				nLineOffset = nOffset + (k - nLoc)*nFramesize - nMisByteCount;
				_fseeki64(fRAW, nLineOffset, SEEK_SET);
				fread_s(plBuffer, nFramesize, sizeof(unsigned short), nFrame, fRAW);
				break;
			}
			else if (nLeakFrameType[k] == 1)
			{
				nMisByteCount += nLeakFrameSize[k];
			}
			else if (nLeakFrameType[k] == 2)
			{
				nMisByteCount += nLeakFrameSize[k] * nWidthsize;
			}
			else
			{
				nMisByteCount += nLeakFrameSize[k];
			}
		}
		ffWeight = float(nLoc - nfValid) / float(nlValid - nfValid);
		flWeight = 1 - ffWeight;

		for (k = 0; k < nFrame; k++)
		{
			pRepairBuffer[k] = unsigned short(ffWeight*pfBuffer[k] + flWeight*plBuffer[k]);
		}
		SOF = pfBuffer[0];
		SyncHeader1 = pfBuffer[1];
		SyncHeader2 = pfBuffer[2];
		nPacktype = pfBuffer[4 + nWidths];
		nFrameID = (pfBuffer[nFrameIDOffset] + nLoc - nfValid) % 65535;


		for (k = 0; k < nHeight; k++)
		{
			pRepairBuffer[k*nWidths] = SOF;
			pRepairBuffer[k*nWidths + 1] = SyncHeader1;
			pRepairBuffer[k*nWidths + 2] = SyncHeader2;
			pRepairBuffer[k*nWidths + 3] = nSamples;
			pRepairBuffer[k*nWidths + 4] = nPacktype;
			pRepairBuffer[k*nWidths + 5] = nFrameID;
			pRepairBuffer[k*nWidths + 6] = k;
			pRepairBuffer[k*nWidths + nWidths - 1] = 0xFBFB;
		}
		for (k = 0; k < nWidths; k++)
		{
			pRepairBuffer[k] = pfBuffer[k];
		}
		pRepairBuffer[5] = nFrameID;
		pRepairBuffer[9] = unsigned short(ffWeight*pfBuffer[9] + flWeight*plBuffer[9]);
		pRepairBuffer[10] = unsigned short(ffWeight*pfBuffer[10] + flWeight*plBuffer[10]);
		pRepairBuffer[11] = unsigned short(ffWeight*pfBuffer[11] + flWeight*plBuffer[11]);
	}
	//漏整数行修复
	else if (nLeakFrameType[nLoc] == 2)
	{
		nAcBands = nHeight - nLeakFrameSize[nLoc];
		_fseeki64(fRAW, nOffset, SEEK_SET);
		fread_s(pLeakBuffer, nWidths*nAcBands*sizeof(unsigned short), sizeof(unsigned short), nWidths*nAcBands, fRAW);

		for (i = 0, j = 0; i < nHeight; i++)
		{
			if (pLeakBuffer[j*nWidths + nBandIDOffset] == i)
			{
				memcpy(pRepairBuffer + i*nWidths, pLeakBuffer + j*nWidths, nWidthsize);
				j++;
			}
			else
			{//存在漏行
				nMisByteCount = 0;
				for (k = nLoc - 1; k >= 0; k--)
				{//找到左侧差值行
					if (nLeakFrameType[k] == 0)
					{
						nfValid = k;
						nLineOffset = nOffset + i*nWidthsize - (nLoc - k)*nFramesize + nMisByteCount;
						_fseeki64(fRAW, nLineOffset, SEEK_SET);
						fread_s(pfBuffer, nWidthsize, sizeof(unsigned short), nWidths, fRAW);
						break;
					}
					else if (nLeakFrameType[k] == 1)
					{
						nMisByteCount += nLeakFrameSize[k];
					}
					else if (nLeakFrameType[k] == 2)
					{
						nMisByteCount += nLeakFrameSize[k] * nWidthsize;
						for (m = 1; m <= i; m++)
						{
							nLineOffset = nOffset + m*nWidthsize - (nLoc - k)*nFramesize + nMisByteCount;
							_fseeki64(fRAW, nLineOffset, SEEK_SET);
							fread_s(pfBuffer, nWidthsize, sizeof(unsigned short), nWidths, fRAW);
							if (pfBuffer[nBandIDOffset] == i)
							{
								nfValid = k;
								break;
							}
						}
					}
					else
					{
						nMisByteCount += nLeakFrameSize[k];
					}
				}
				nMisByteCount = nLeakFrameSize[nLoc] * nWidthsize;
				for (k = nLoc + 1; k < nLines; k++)
				{//找到右侧差值行
					if (nLeakFrameType[k] == 0)
					{
						nlValid = k;
						nLineOffset = nOffset + i*nWidthsize + (k - nLoc)*nFramesize - nMisByteCount;
						_fseeki64(fRAW, nLineOffset, SEEK_SET);
						fread_s(plBuffer, nWidthsize, sizeof(unsigned short), nWidths, fRAW);
						break;
					}
					else if (nLeakFrameType[k] == 1)
					{
						nMisByteCount += nLeakFrameSize[k];
					}
					else if (nLeakFrameType[k] == 2)
					{
						nMisByteCount += nLeakFrameSize[k] * nWidthsize;
						for (m = 1; m <= i; m++)
						{
							nLineOffset = nOffset + m*nWidthsize + (k - nLoc)*nFramesize - nMisByteCount;
							_fseeki64(fRAW, nLineOffset, SEEK_SET);
							fread_s(plBuffer, nWidthsize, sizeof(unsigned short), nWidths, fRAW);
							if (plBuffer[nBandIDOffset] == i)
							{
								nlValid = k;
								break;
							}
						}
					}
					else
					{
						nMisByteCount += nLeakFrameSize[k];
					}
				}
				ffWeight = float(nLoc - nfValid) / float(nlValid - nfValid);
				flWeight = 1 - ffWeight;
				for (k = 0; k < nWidths; k++)
				{
					pRepairBuffer[k + i*nWidths] = unsigned short(ffWeight*pfBuffer[k] + flWeight*plBuffer[k]);
				}
				SOF = pfBuffer[0];
				SyncHeader1 = pfBuffer[1];
				SyncHeader2 = pfBuffer[2];
				nPacktype = pfBuffer[4 + nWidths];
				nFrameID = (pfBuffer[nFrameIDOffset] + nLoc - nfValid) % 65535;

				pRepairBuffer[i*nWidths] = SOF;
				pRepairBuffer[i*nWidths + 1] = SyncHeader1;
				pRepairBuffer[i*nWidths + 2] = SyncHeader2;
				pRepairBuffer[i*nWidths + 3] = nSamples;
				pRepairBuffer[i*nWidths + 4] = nPacktype;
				pRepairBuffer[i*nWidths + 5] = nFrameID;
				pRepairBuffer[i*nWidths + 6] = i;
				pRepairBuffer[i*nWidths + nWidths - 1] = 0xFBFB;
			}
		}
	}
	//漏非整数行修复
	else if (nLeakFrameType[nLoc] == 3)
	{
		nAcBands = 0;
		nMisByteCount = 0;
		for (k = nLoc - 1; k >= 0; k--)
		{//找到左侧差值行
			if (nLeakFrameType[k] == 0)
			{
				nfValid = k;
				nLineOffset = nOffset - (nLoc - k)*nFramesize + nMisByteCount;
				_fseeki64(fRAW, nLineOffset, SEEK_SET);
				fread_s(pfBuffer, nFramesize, sizeof(unsigned short), nFrame, fRAW);
				break;
			}
			else if (nLeakFrameType[k] == 1)
			{
				nMisByteCount += nLeakFrameSize[k];
			}
			else if (nLeakFrameType[k] == 2)
			{
				nMisByteCount += nLeakFrameSize[k] * nWidthsize;
			}
			else
			{
				nMisByteCount += nLeakFrameSize[k];
			}
		}
		nMisByteCount = nLeakFrameSize[nLoc];
		for (k = nLoc + 1; k < nLines; k++)
		{//找到右侧差值行
			if (nLeakFrameType[k] == 0)
			{
				nlValid = k;
				nLineOffset = nOffset + (k - nLoc)*nFramesize - nMisByteCount;
				_fseeki64(fRAW, nLineOffset, SEEK_SET);
				fread_s(plBuffer, nFramesize, sizeof(unsigned short), nFrame, fRAW);
				break;
			}
			else if (nLeakFrameType[k] == 1)
			{
				nMisByteCount += nLeakFrameSize[k];
			}
			else if (nLeakFrameType[k] == 2)
			{
				nMisByteCount += nLeakFrameSize[k] * nWidthsize;
			}
			else
			{
				nMisByteCount += nLeakFrameSize[k];
			}
		}
		ffWeight = float(nLoc - nfValid) / float(nlValid - nfValid);
		flWeight = 1 - ffWeight;

		for (k = 0; k < nFrame; k++)
		{
			pRepairBuffer[k] = unsigned short(ffWeight*pfBuffer[k] + flWeight*plBuffer[k]);
		}
		SOF = pfBuffer[0];
		SyncHeader1 = pfBuffer[1];
		SyncHeader2 = pfBuffer[2];
		nPacktype = pfBuffer[4 + nWidths];
		nFrameID = (pfBuffer[nFrameIDOffset] + nLoc - nfValid) % 65535;
		for (k = 0; k < nHeight; k++)
		{
			pRepairBuffer[k*nWidths] = SOF;
			pRepairBuffer[k*nWidths + 1] = SyncHeader1;
			pRepairBuffer[k*nWidths + 2] = SyncHeader2;
			pRepairBuffer[k*nWidths + 3] = nSamples;
			pRepairBuffer[k*nWidths + 4] = nPacktype;
			pRepairBuffer[k*nWidths + 5] = nFrameID;
			pRepairBuffer[k*nWidths + 6] = k;
			pRepairBuffer[k*nWidths + nWidths - 1] = 0xFBFB;
		}
		for (k = 0; k < nWidths; k++)
		{
			pRepairBuffer[k] = pfBuffer[k];
		}
		pRepairBuffer[5] = nFrameID;
		pRepairBuffer[9] = unsigned short(ffWeight*pfBuffer[9] + flWeight*plBuffer[9]);
		pRepairBuffer[10] = unsigned short(ffWeight*pfBuffer[10] + flWeight*plBuffer[10]);
		pRepairBuffer[11] = unsigned short(ffWeight*pfBuffer[11] + flWeight*plBuffer[11]);
	}

	return lError;
}

long QPDPreProcess::PreProc_GenerateD0Data(const char *pRAWData,const char *pData, DINFO mDataHeader, vector<short> nLeakFrameType, vector<int> nLeakFrameSize, int nLeakFrameCount, const int nFixLines)
{
	//==========变量定义=============
	long lError = 0;
	int i = 0, j = 0, k = 0;
	int nLines = 0, nBands = 0, nWidths = 0, nHeight = 0;
	__int64 nOffset = 0, nHeadOffset = 0;
	__int64 nFrame = 0, nFramesize = 0, nWidthsize = 0, nBufFramesize = 0;

	FILE *fRAW = NULL, *fData = NULL;
	errno_t err = 0;
	int nLoops = 0, nReadLines = 0, nLeftLines = 0;
	unsigned short *pBuffer = NULL;
	unsigned short *pRepairBuffer = NULL;
	unsigned short *pfBuffer = NULL, *plBuffer = NULL, *pLeakBuffer = NULL;
	//===========处理===============
	//漏行修复
	nWidths = mDataHeader.nWidths;
	nLines = mDataHeader.nLines;
	nBands = mDataHeader.nBands;
	nHeadOffset = mDataHeader.nHeadOffset;
	nHeight = nBands + 1;

	nFrame = nWidths*nHeight;
	nWidthsize = nWidths*sizeof(unsigned short);
	nFramesize = nFrame*sizeof(unsigned short);
	nBufFramesize = nFrame*sizeof(unsigned short)*nFixLines;

	ENVIHeader mENVIHeader;
	char drive[_MAX_DRIVE]; char dir[_MAX_DIR]; char filename[_MAX_FNAME]; char ext[_MAX_EXT];
	char path[_MAX_PATH];
	_splitpath_s(pData, drive, dir, filename, ext);
	_makepath_s(path, drive, dir, filename, "hdr");
	memset(&mENVIHeader, 0, sizeof(ENVIHeader));

	err = fopen_s(&fRAW, pRAWData, "rb");
	if (err)
	{
		lError = 1;
		goto ErrEnd;
	}

	err = fopen_s(&fData, pData, "wb");
	if (err)
	{
		lError = 1;
		goto ErrEnd;
	}

	//写ENVI头文件
	mENVIHeader.datatype = 12;
	mENVIHeader.imgWidth = nWidths;
	mENVIHeader.imgHeight = nLines;
	mENVIHeader.imgBands = nHeight;
	mENVIHeader.interleave = "BIL";
	WriteENVIHeader(path, mENVIHeader);

	//生成0级数据
	try
	{
		pBuffer = new unsigned short[nFrame*nFixLines];
	}
	catch (bad_alloc&)
	{
		lError = 1;
		goto ErrEnd;
	}
	memset(pBuffer, 0, nBufFramesize);
	//进行漏行检测
	lError = PreProc_LeakLineCheck(pRAWData, mDataHeader, nLeakFrameType, nLeakFrameSize, nLeakFrameCount);
	if (lError != 0)
		goto ErrEnd;

	if (nLeakFrameCount == 0)
	{
		//无漏行，直接分buffer读取和写入，去除首尾多余部分	
		nOffset = nHeadOffset;
		if (nLines%nFixLines == 0)
		{
			nLoops = nLines / nFixLines;
		}
		else
		{
			nLoops = nLines / nFixLines + 1;
		}
		for (i = 0; i<nLoops; i++)
		{
			nLeftLines = nLines - i*nFixLines;
			if (nLeftLines>nFixLines)
			{
				nReadLines = nFixLines;
			}
			else
			{
				nReadLines = nLeftLines;
			}
			nOffset = i*nBufFramesize + nHeadOffset;
			_fseeki64(fRAW, nOffset, SEEK_SET);
			fread_s(pBuffer, nBufFramesize, sizeof(unsigned short), nFrame*nReadLines, fRAW);
			fwrite(pBuffer, sizeof(unsigned short), nFrame*nReadLines, fData);
		}
	}
	else
	{
		//存在漏行，进行差值修复
		try
		{
			pRepairBuffer = new unsigned short[nFrame];
		}
		catch (bad_alloc&)
		{
			lError = 1;
			goto ErrEnd;
		}
		try
		{
			pfBuffer = new unsigned short[nFrame];
		}
		catch (bad_alloc)
		{
			lError = 1;
			goto ErrEnd;
		}
		try
		{
			plBuffer = new unsigned short[nFrame];
		}
		catch (bad_alloc&)
		{
			lError = 1;
			goto ErrEnd;
		}
		try
		{
			pLeakBuffer = new unsigned short[nFrame];
		}
		catch (bad_alloc)
		{
			lError = 1;
			goto ErrEnd;
		}

		memset(pRepairBuffer, 0, nFrame*sizeof(unsigned short));
		memset(pfBuffer, 0, nFrame*sizeof(unsigned short));
		memset(plBuffer, 0, nFrame*sizeof(unsigned short));
		memset(pLeakBuffer, 0, nFrame*sizeof(unsigned short));

		nOffset = nHeadOffset;
		nReadLines = 0;
		_fseeki64(fRAW, nOffset, SEEK_SET);

		for (i = 0; i < nLines; i++)
		{
			if (nLeakFrameSize[i] == 0)
			{
				nReadLines++;
				if (nReadLines >= nFixLines || i == nLines - 1)
				{
					_fseeki64(fRAW, nOffset, SEEK_SET);
					fread_s(pBuffer, nBufFramesize, sizeof(unsigned short), nFrame*nReadLines, fRAW);
					fwrite(pBuffer, sizeof(unsigned short), nFrame*nReadLines, fData);
					nOffset += sizeof(unsigned short)*nFrame*nReadLines;
					nReadLines = 0;
				}
			}
			else
			{
				if (nReadLines > 0)
				{
					//将前序行写入
					_fseeki64(fRAW, nOffset, SEEK_SET);
					fread_s(pBuffer, nBufFramesize, sizeof(unsigned short), nFrame*nReadLines, fRAW);
					fwrite(pBuffer, sizeof(unsigned short), nFrame*nReadLines, fData);
					nOffset += sizeof(unsigned short)*nFrame*nReadLines;
					nReadLines = 0;
				}
				//漏帧位置修复，生成新的帧，存储在pRepairBuffer
				lError = PreProc_LeakLineInterpolate(fRAW, pRepairBuffer, mDataHeader, nLeakFrameType, nLeakFrameSize, i, nOffset, pfBuffer, plBuffer, pLeakBuffer);
				if (lError)
				{
					lError = 1;
					goto ErrEnd;
				}
				else
				{
					fwrite(pRepairBuffer, sizeof(unsigned short), nFrame, fData);
					switch (nLeakFrameType[i])
					{
					case 1:
						nOffset += 0;
						continue;
					case 2:
						nOffset = nOffset + nFramesize - nLeakFrameSize[i] * nWidthsize;
						continue;
					case 3:
						nOffset = nOffset + nFramesize - nLeakFrameSize[i];
						continue;
					default:
						break;
					}
				}
			}
		}
	}

ErrEnd:
	if (pBuffer)
	{
		delete[] pBuffer;
		pBuffer = NULL;
	}
	if (pRepairBuffer)
	{
		delete[] pRepairBuffer;
		pRepairBuffer = NULL;
	}
	if (pfBuffer)
	{
		delete[] pfBuffer;
		pfBuffer = NULL;
	}
	if (plBuffer)
	{
		delete[] plBuffer;
		plBuffer = NULL;
	}
	if (pLeakBuffer)
	{
		delete[] pLeakBuffer;
		pLeakBuffer = NULL;
	}
	fclose(fRAW);
	fclose(fData);
	return lError;
}