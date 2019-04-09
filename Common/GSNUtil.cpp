

#include "GSNUtil.h"
#include "String/NSString.h"
#include <sddl.h>
#include <Tlhelp32.h>
#include <winsvc.h>


#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma warning(disable: 4996)


static char s_HexTable[17] = "0123456789ABCDEF";

/*
 *	Function:		DataToHex
 *	Explanation:	内存数据转16进制
 *	Input:			pMem		内存数据
					ulMemSize	pMem大小
					ulHexSize	pHexStr内存的大小
 *  Output:			pHexStr		16进制字符串大小
 *	Return:			实际转换后16进制字符串大小
 */
unsigned long GSNUtil::DataToHex(void *pMem, unsigned long ulMemSize, char* pHexStr, unsigned long ulHexSize)
{
	unsigned long		ulCount					= 0;
	unsigned long		ulIndex					= 0;
	unsigned char*		pByte					= (unsigned char *)pMem;
	int					nByteHi;
	int					nByteLo;

	if (ulHexSize < ulMemSize * 2 + 1)
	{
		return 0;
	}

	for (; ulIndex < ulMemSize; ulIndex++)
	{
		nByteHi = pByte[ulIndex] >> 4;
		nByteLo = pByte[ulIndex] & 0x0F;
		pHexStr[ulCount] = s_HexTable[nByteHi];
		ulCount++;
		pHexStr[ulCount] = s_HexTable[nByteLo];
		ulCount++;
	}

	pHexStr[ulCount] = 0;

	return ulCount;
}

/**
 * 函数功能说明：内存转16进制
 * @param [in]  strData		内存数据
 * @return 16进制
 */
std::string GSNUtil::DataToHex(std::string strData)
{
	char*				pData					= NULL;
	std::string			strReturn				= "";
	unsigned long		ulDataSize				= strData.size() * 2 + 2;
	unsigned long		ulHexSize				= 0;

	if (strData.empty())
	{
		return strReturn;
	}

	pData = (char *)malloc(ulDataSize);
	
	if (NULL == pData)
	{
		return strReturn;
	}

	memset(pData, 0, ulDataSize);
	ulHexSize = DataToHex((void *)strData.c_str(), strData.size(), pData, ulDataSize);
	strReturn.append(pData, ulHexSize);
	free(pData);

	return strReturn;
}

static unsigned char s_Hex2ValTable[256] =
{
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/*
 *	Function:		HexToData
 *	Explanation:	16进制转内存数据
 *	Input:			pHexStr		16进制字符串
					ulHexSize	pHexStr的大小
					ulMemSize	pMem可存储的大小
 *  Output:			pMem		内存数据
 *	Return:			实际转换后内存数据的大小
 */
unsigned long GSNUtil::HexToData(char *pHexStr, unsigned long ulHexSize, void *pMem, unsigned long ulMemSize)
{
	unsigned long		ulIndex					= 0;
	unsigned long		ulCount					= 0;
	unsigned char*		pByte					= NULL;
	unsigned char		ucByteHi;
	unsigned char		ucByteLo;

	if (ulMemSize < (ulHexSize / 2))
	{
		return 0;
	}

	pByte = (unsigned char*)pMem;

	for (;ulCount < ulMemSize; ulIndex++)
	{
		ucByteHi = (unsigned char)pHexStr[ulIndex];

		if (0 == ucByteHi)
		{
			break;
		}

		ucByteHi = s_Hex2ValTable[ucByteHi];

		if (0x0F < ucByteHi)
		{
			continue;
		}

		ulIndex++;
		ucByteLo = (unsigned char)pHexStr[ulIndex];

		if (0 == ucByteLo)
		{
			break;
		}

		ucByteLo = s_Hex2ValTable[ucByteLo];

		if (0x0F < ucByteLo)
		{
			continue;
		}

		pByte[ulCount] = (ucByteHi << 4) | ucByteLo;
		ulCount++;
	}


	return ulCount;
}

/**
 * 函数功能说明：16进制转内存
 * @param [in]  strHex			16进制
 * @return 内存
 * by Maga 2016-8-30
 */
std::string GSNUtil::HexToData(std::string strHex)
{
	char*				pData						= NULL;
	std::string			strReturn					= "";
	unsigned long		ulDataSize					= strHex.size() / 2 + 2;
	unsigned long		ulHexSize = 0;

	if (strHex.empty())
	{
		return strReturn;
	}

	pData = (char *)malloc(ulDataSize);

	if (NULL == pData)
	{
		return strReturn;
	}

	memset(pData, 0, ulDataSize);
	ulHexSize = HexToData((char *)strHex.c_str(), strHex.size(), (void *)pData, ulDataSize);
	strReturn.append(pData, ulHexSize);

	return strReturn;
}

/*
 *	Function:		GetOsMajorVersion
 *	Explanation:	获取系统主版本号
 *	Return:			系统主版本号
 */
int GSNUtil::GetOsMajorVersion()
{
	OSVERSIONINFOEXA		OsInfoEx				= { 0 };
	
	OsInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

	if (0 == GetVersionExA((OSVERSIONINFOA *)&OsInfoEx))
	{
		OsInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		if (0 == GetVersionExA((OSVERSIONINFOA *)&OsInfoEx))
		{
			return -1;
		}
	}

	return (int)OsInfoEx.dwMajorVersion;
}

//************************************
// Method:    获取系统次版本号
// FullName:  GSNUtil::GetOsMinorVersion
// Access:    public 
// Returns:   int
// Qualifier:
// by JJHUANG 2017-01-18
//************************************
int GSNUtil::GetOsMinorVersion()
{
	OSVERSIONINFOEXA		OsInfoEx = { 0 };

	OsInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

	if (0 == GetVersionExA((OSVERSIONINFOA *)&OsInfoEx))
	{
		OsInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		if (0 == GetVersionExA((OSVERSIONINFOA *)&OsInfoEx))
		{
			return -1;
		}
	}

	return (int)OsInfoEx.dwMinorVersion;
}

//************************************
// Method:    获取系统版本
// FullName:  GSNUtil::GetOsVersion
// Access:    public 
// Returns:   std::string
// Qualifier:
// by JJHUANG 2017-01-18
//************************************
std::string GSNUtil::GetOsVersion()
{
	/************************************************************************/
	/*https://msdn.microsoft.com/en-us/library/windows/desktop/ms724834(v=vs.85).aspx */
	/************************************************************************/
	
	OSVERSIONINFOEXA		OsInfoEx = { 0 };

	OsInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

	if (0 == GetVersionExA((OSVERSIONINFOA *)&OsInfoEx))
	{
		OsInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		if (0 == GetVersionExA((OSVERSIONINFOA *)&OsInfoEx))
		{
			return "";
		}
	}

	std::string	strVersion;
	int			nMajorVerion = (int)OsInfoEx.dwMajorVersion;
	int			nMinorVerion = (int)OsInfoEx.dwMinorVersion;

	switch (nMajorVerion)
	{
	case 5:
		switch (nMinorVerion)
		{
		case 0:
			strVersion = "Windows 2000";
			break;

		case 1:
			strVersion = "Windows XP";
			break;

		case 2:
			if (OsInfoEx.wProductType == VER_NT_WORKSTATION)
				strVersion = "Windows Server 2003";
			else
				strVersion = "Windows Server 2003 R2";

			break;
		}

		break;

	case 6:
		switch (nMinorVerion)
		{
		case 0:
			if (OsInfoEx.wProductType == VER_NT_WORKSTATION)
				strVersion = "Windows Vista";
			else
				strVersion = "Microsoft Windows Server 2008";

			break;

		case 1:
			if (OsInfoEx.wProductType == VER_NT_WORKSTATION)
				strVersion = "Windows 7";
			else
				strVersion = "Microsoft Windows Server 2008 R2";

			break;

		case 2:
			if (OsInfoEx.wProductType == VER_NT_WORKSTATION)
				strVersion = "Windows 8";
			else
				strVersion = "Windows Server 2012";

			break;

		case 3:
			if (OsInfoEx.wProductType == VER_NT_WORKSTATION)
				strVersion = "Windows 8.1";
			else
				strVersion = "Windows Server 2012 R2";

			break;
		}

		break;

	case 10:
		if (OsInfoEx.wProductType == VER_NT_WORKSTATION)
			strVersion = "Windows 10";
		else
			strVersion = "Windows Server 2016";

		break;

	default:
		break;
	}
	
	std::string strVersionNumber = NStr::FormatStr("%d.%d.%d", nMajorVerion, nMinorVerion, OsInfoEx.dwBuildNumber);

	return strVersion + " " + strVersionNumber;
}
/*
 *	Function:		SetSecurityDescriptorByString
 *	Explanation:	设置访问控制权限
 *	Input:			输入
 *  Output:			输出
 *	Return:			TRUE 成功 FALSE 失败
 */
bool GSNUtil::SetSecurityDescriptorByString(SECURITY_DESCRIPTOR *pSd, std::string strSdString /* = "" */)
{
	PSECURITY_DESCRIPTOR 		pNewSd;
	PACL						pAcl					= NULL;
	PACL						pDcl					= NULL;
	BOOL						bSaclPresent			= FALSE;
	BOOL						bSaclDefaulted			= FALSE;
	BOOL						bRetVal;

	bRetVal = ConvertStringSecurityDescriptorToSecurityDescriptorA((char *)strSdString.c_str(), SDDL_REVISION_1, &pNewSd, NULL);

	if (bRetVal == FALSE)
	{
		return FALSE;
	}

	GetSecurityDescriptorSacl(pNewSd, &bSaclPresent, &pAcl, &bSaclDefaulted);
	GetSecurityDescriptorDacl(pNewSd, &bSaclPresent, &pDcl, &bSaclDefaulted);

	if (pAcl)
	{
		SetSecurityDescriptorSacl(pSd, TRUE, pAcl, FALSE);
	}
		
	if (pDcl)
	{
		SetSecurityDescriptorDacl(pSd, TRUE, pDcl, FALSE);
	}
		
	return TRUE;
}

/*
 *	Function:		RegisterFile
 *	Explanation:	注册文件
 *	Input:			strFile			文件路径
					nWait			等待时间
 *	Return:			true 成功 false 失败
 */
bool GSNUtil::RegisterFile(std::string strFile, int nWait /* = INFINITE */)
{
	STARTUPINFOA			si				= { 0 };
	PROCESS_INFORMATION		pi				= { 0 };
	std::string				strCmd;
	int						nRet;

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	strCmd = NStr::FormatStr("regsvr32.exe /s \"%s\"", (char *)strFile.c_str());
	nRet = CreateProcessA(NULL, (char *)strCmd.c_str(), NULL, NULL,
								FALSE, 0, NULL, NULL, &si, &pi);

	if (0 == nRet)
	{
		return false;
	}

	::WaitForSingleObject(pi.hProcess, nWait);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}

bool GSNUtil::RegisterFile(std::wstring strFile, int nWait /* = INFINITE */)
{
	STARTUPINFOW			si				= { 0 };
	PROCESS_INFORMATION		pi				= { 0 };
	std::wstring				strCmd;
	int						nRet;

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	strCmd = NStr::FormatWStr(L"regsvr32.exe /s \"%s\"", (wchar_t *)strFile.c_str());
	nRet = CreateProcessW(NULL, (wchar_t *)strCmd.c_str(), NULL, NULL,
		FALSE, 0, NULL, NULL, &si, &pi);

	if (0 == nRet)
	{
		return false;
	}

	::WaitForSingleObject(pi.hProcess, nWait);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}

/*
 *	Function:		UnRegisterFile
 *	Explanation:	反注册文件
 *	Input:			strFile			文件路径
					nWait			等待时间
 *	Return:			true 成功 false 失败
 */
bool GSNUtil::UnRegisterFile(std::string strFile, int nWait /* = INFINITE */)
{
	STARTUPINFOA			si				= { 0 };
	PROCESS_INFORMATION		pi				= { 0 };
	std::string				strCmd;
	int						nRet;

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	strCmd = NStr::FormatStr("regsvr32.exe /u /s \"%s\"", (char *)strFile.c_str());
	nRet = CreateProcessA(NULL, (char *)strCmd.c_str(), NULL, NULL,
		FALSE, 0, NULL, NULL, &si, &pi);

	if (0 == nRet)
	{
		return false;
	}

	::WaitForSingleObject(pi.hProcess, nWait);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}

bool GSNUtil::UnRegisterFile(std::wstring strFile, int nWait /* = INFINITE */)
{
	STARTUPINFOW			si				= { 0 };
	PROCESS_INFORMATION		pi				= { 0 };
	std::wstring				strCmd;
	int						nRet;

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	strCmd = NStr::FormatWStr(L"regsvr32.exe /u /s \"%s\"", (wchar_t *)strFile.c_str());
	nRet = CreateProcessW(NULL, (wchar_t *)strCmd.c_str(), NULL, NULL,
		FALSE, 0, NULL, NULL, &si, &pi);

	if (0 == nRet)
	{
		return false;
	}

	::WaitForSingleObject(pi.hProcess, nWait);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}

/**
 * 函数功能说明：计算Hash
 * @param [in]  strBuf		字符串
 * @return 哈希值
 * by Maga 2016-7-14
 */
unsigned int GSNUtil::BKDRHash(std::string strBuf)
{
	unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < strBuf.length(); i++)
	{
		hash = (hash * seed) + strBuf[i];
	}

	return hash;
}

/**
 * 函数功能说明：启动进程
 * @param [in]  strExe		程序运行路径
 * @param [in]  strCmd		命令行参数
 * @param [out] pSi			启动信息
 * @param [out] pPi			进程信息
 * @return 进程句柄
 * by Maga 2016-8-16
 */
HANDLE GSNUtil::RunProcess(std::string strExe, std::string strCmd, STARTUPINFOA *pSi /* = NULL */, PROCESS_INFORMATION *pPi /* = NULL */)
{
	std::string				strCmdLine;
	STARTUPINFOA			Si						= { 0 };
	PROCESS_INFORMATION		Pi						= { 0 };
	int						nRet;
	
	if (strExe.empty() && strCmd.empty())
	{
		return NULL;
	}

	if (!strCmd.empty())
	{
		if (strExe.empty())
		{
			strCmdLine = strCmd;
		}
		else
		{
			if ('\"' == strExe[0])
			{
				strCmdLine = NStr::FormatStr("%s %s", strExe.c_str(), strCmd.c_str());
			}
			else
			{
				strCmdLine = NStr::FormatStr("\"%s\" %s", strExe.c_str(), strCmd.c_str());
			}
			
		}
	}
	else
	{
		strCmdLine = strExe;
	}

	if (strCmdLine.empty())
	{
		return NULL;
	}

	if (NULL == pSi)
	{
		pSi = &Si;
	}

	if (NULL == pPi)
	{
		pPi = &Pi;
	}

	nRet = CreateProcessA(NULL, (char *)strCmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, pSi, pPi);

	if (1 != nRet)
	{
		return NULL;
	}

	if (pPi == &Pi)
	{
		CloseHandle(pPi->hThread);
	}

	return pPi->hProcess;
}

/**
* 函数功能说明：启动进程
* @param [in]  strExe		程序运行路径
* @param [in]  strCmd		命令行参数
* @param [in]  bHide		是否隐藏
* @param [out] pSi			启动信息
* @param [out] pPi			进程信息
* @return 进程ID
* by Maga 2016-8-16
*/
unsigned long GSNUtil::RunProcess2(std::string strExe, std::string strCmd /* = "" */, bool bHide /* = false */, STARTUPINFOA *pSi /* = NULL */, PROCESS_INFORMATION *pPi /* = NULL */)
{
	std::string				strCmdLine;
	STARTUPINFOA			Si						= { 0 };
	PROCESS_INFORMATION		Pi						= { 0 };
	int						nRet;

	if (strExe.empty() && strCmd.empty())
	{
		return -1;
	}

	if (!strCmd.empty())
	{
		if (strExe.empty())
		{
			strCmdLine = strCmd;
		}
		else
		{
			if ('\"' == strExe[0])
			{
				strCmdLine = NStr::FormatStr("%s %s", strExe.c_str(), strCmd.c_str());
			}
			else
			{
				strCmdLine = NStr::FormatStr("\"%s\" %s", strExe.c_str(), strCmd.c_str());
			}

		}
	}
	else
	{
		strCmdLine = strExe;
	}

	if (strCmdLine.empty())
	{
		return -1;
	}

	if (NULL == pSi)
	{
		pSi = &Si;
	}

	if (NULL == pPi)
	{
		pPi = &Pi;
	}

	if (bHide)
	{
		pSi->wShowWindow = SW_HIDE;
		pSi->dwFlags = STARTF_USESHOWWINDOW;
	}

	nRet = CreateProcessA(NULL, (char *)strCmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, pSi, pPi);

	if (1 != nRet)
	{
		return -1;
	}

	if (pPi == &Pi)
	{
		CloseHandle(pPi->hThread);
		CloseHandle(pPi->hProcess);
	}

	return pPi->dwProcessId;
}

/**
 * 函数功能说明：根据进程ID判断进程是否存在
 * @param [in]  ulProcId		进程ID
 * @return TRUE 存在 FALSE 不存在
 * by Maga 2016-8-16
 */
bool GSNUtil::IsProcExist(unsigned long ulProcId)
{
	HANDLE			hProc			= NULL;


	hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ulProcId);

	if (INVALID_HANDLE_VALUE == hProc)
	{
		return false;
	}

	if (false == IsProcExistFromHandle(hProc))
	{
		CloseHandle(hProc);
		return false;
	}

	CloseHandle(hProc);
	return true;
}

/**
* 函数功能说明：根据进程ID判断进程是否存在
* @param [in]  hProc		进程句柄
* @return TRUE 存在 FALSE 不存在
* by Maga 2016-8-16
*/
bool GSNUtil::IsProcExistFromHandle(HANDLE hProc)
{
	unsigned long	ulExit;
	int				nRet;

	nRet = ::GetExitCodeProcess(hProc, &ulExit);

	if (1 != nRet)
	{
		return false;
	}

	if (STILL_ACTIVE == ulExit)
	{
		return true;
	}

	return false;
}

/**
 * 函数功能说明： 根据进程名判断进程是否存在
 * @param [in]  strProcName		进程名
 * @return TRUE 存在 FALSE 不存在
 * by Maga 2016-11-11
 */
bool GSNUtil::IsProcExist(std::string strProcName)
{
	std::vector<unsigned long>		vecProcIdList;

	GetProcIdFromName(strProcName, vecProcIdList);

	if (0 >= vecProcIdList.size())
	{
		return false;
	}

	return true;
}


/**
 * 函数功能说明：退出进程
 * @param [in]  ulProcId		进程ID
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-8-16
 */
bool GSNUtil::KillProc(unsigned long ulProcId)
{
	HANDLE				hProc				= NULL;

	hProc = ::OpenProcess(PROCESS_TERMINATE, FALSE, ulProcId);

	if (NULL == hProc)
	{
		return false;
	}

	TerminateProcess(hProc, 0);
	CloseHandle(hProc);

	return true;
}

/**
 * 函数功能说明：退出进程
 * @param [in]  strProcName		进程名称
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-9-24
 */
bool GSNUtil::KillProc(std::string strProcName)
{
	std::vector<unsigned long>			vecProcIdList;

	GetProcIdFromName(strProcName, vecProcIdList);

	for (int i = 0; i < vecProcIdList.size(); i++)
	{
		KillProc(vecProcIdList[i]);
	}

	return true;
}

/**
 * 函数功能说明：运行程序
 * @param [in]  strExe		程序路径
 * @param [in]  strCmd		命令行参数
 * @param [in]  bWait		是否等待
 * @param [in]  bHide		是否隐藏
 * @param [out] pulExitCode	退出代码
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-8-18
 */
bool GSNUtil::RunProcess3(std::string strExe, std::string strCmd, bool bWait /* = false */, bool bHide /* = false */, unsigned long *pulExitCode /* = NULL */)
{
	std::string				strCmdLine;
	STARTUPINFOA			Si					= { 0 };
	PROCESS_INFORMATION		Pi					= { 0 };
	int						nRet;

	if (strExe.empty() && strCmd.empty())
	{
		return false;
	}

	if (!strCmd.empty())
	{
		if (strExe.empty())
		{
			strCmdLine = strCmd;
		}
		else
		{
			if ('\"' == strExe[0])
			{
				strCmdLine = NStr::FormatStr("%s %s", strExe.c_str(), strCmd.c_str());
			}
			else
			{
				strCmdLine = NStr::FormatStr("\"%s\" %s", strExe.c_str(), strCmd.c_str());
			}

		}
	}
	else
	{
		strCmdLine = strExe;
	}

	if (bHide)
	{
		Si.wShowWindow = SW_HIDE;
		Si.dwFlags = STARTF_USESHOWWINDOW;
	}

	nRet = CreateProcessA(NULL, (char *)strCmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi);

	if (0 == nRet)
	{
		return false;
	}

	if (bWait)
	{
		WaitForSingleObject(Pi.hProcess, INFINITE);
	}

	if (pulExitCode)
	{
		*pulExitCode = -1;
		nRet = GetExitCodeProcess(Pi.hProcess, pulExitCode);
	}

	CloseHandle(Pi.hThread);
	CloseHandle(Pi.hProcess);

	return true;
}

/**
* 函数功能说明：运行程序
* @param [in]  strExe		程序路径
* @param [in]  strCmd		命令行参数
* @param [in]  ulWaitTime	等待时间(0 不等待)
* @param [in]  bHide		是否隐藏
* @param [out] pulExitCode	退出代码
* @return TRUE 成功 FALSE 失败
* by Maga 2016-8-18
*/
bool GSNUtil::RunProcess5(std::string strExe, std::string strCmd /* = "" */, unsigned long ulWaitTime /* = 0 */, bool bHide /* = false */, unsigned long *pulExitCode /* = NULL */)
{
	std::string				strCmdLine;
	STARTUPINFOA			Si = { 0 };
	PROCESS_INFORMATION		Pi = { 0 };
	int						nRet;

	if (strExe.empty() && strCmd.empty())
	{
		return false;
	}

	if (!strCmd.empty())
	{
		if (strExe.empty())
		{
			strCmdLine = strCmd;
		}
		else
		{
			if ('\"' == strExe[0])
			{
				strCmdLine = NStr::FormatStr("%s %s", strExe.c_str(), strCmd.c_str());
			}
			else
			{
				strCmdLine = NStr::FormatStr("\"%s\" %s", strExe.c_str(), strCmd.c_str());
			}

		}
	}
	else
	{
		strCmdLine = strExe;
	}

	if (bHide)
	{
		Si.wShowWindow = SW_HIDE;
		Si.dwFlags = STARTF_USESHOWWINDOW;
	}

	nRet = CreateProcessA(NULL, (char *)strCmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi);

	if (0 == nRet)
	{
		return false;
	}

	if (0 != ulWaitTime)
	{
		WaitForSingleObject(Pi.hProcess, ulWaitTime);
	}

	if (pulExitCode)
	{
		*pulExitCode = -1;
		nRet = GetExitCodeProcess(Pi.hProcess, pulExitCode);
	}

	CloseHandle(Pi.hThread);
	CloseHandle(Pi.hProcess);

	return true;
}

/**
* 函数功能说明：运行程序
* @param [in]  strExe		程序路径
* @param [in]  strCmd		命令行参数
* @param [in]  strCurDir	当前工作目录
* @param [in]  bWait		是否等待
* @param [in]  bHide		是否隐藏
* @param [out] pulExitCode	退出代码
* @return TRUE 成功 FALSE 失败
* by Maga 2016-9-28
*/
bool GSNUtil::RunProcess4(std::string strExe, std::string strCmd /* = "" */, std::string strCurDir /* = "" */, 
							bool bWait /* = false */, bool bHide /* = false */, unsigned long *pulExitCode /* = NULL */)
{
	std::string				strCmdLine;
	STARTUPINFOA			Si = { 0 };
	PROCESS_INFORMATION		Pi = { 0 };
	int						nRet;

	if (strExe.empty() && strCmd.empty())
	{
		return false;
	}

	if (!strCmd.empty())
	{
		if (strExe.empty())
		{
			strCmdLine = strCmd;
		}
		else
		{
			if ('\"' == strExe[0])
			{
				strCmdLine = NStr::FormatStr("%s %s", strExe.c_str(), strCmd.c_str());
			}
			else
			{
				strCmdLine = NStr::FormatStr("\"%s\" %s", strExe.c_str(), strCmd.c_str());
			}

		}
	}
	else
	{
		strCmdLine = strExe;
	}

	if (bHide)
	{
		Si.wShowWindow = SW_HIDE;
		Si.dwFlags = STARTF_USESHOWWINDOW;
	}

	if (strCurDir.empty())
	{
		nRet = CreateProcessA(NULL, (char *)strCmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi);
	}
	else
	{
		nRet = CreateProcessA(NULL, (char *)strCmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, strCurDir.c_str(), &Si, &Pi);
	}
	

	if (0 == nRet)
	{
		return false;
	}

	if (bWait)
	{
		WaitForSingleObject(Pi.hProcess, INFINITE);
	}

	if (pulExitCode)
	{
		*pulExitCode = -1;
		nRet = GetExitCodeProcess(Pi.hProcess, pulExitCode);
	}

	CloseHandle(Pi.hThread);
	CloseHandle(Pi.hProcess);

	return true;
}

/**
 * 函数功能说明：后台运行程序
 * @param [in]  strCmdLine		运行命令行指令
 * @param [in]  ulTimeOut		等待时间
 * @param [out] pExitCode		退出代码
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-8-30
 */
bool GSNUtil::RunProcessBk(std::string strCmdLine, unsigned long *pExitCode, unsigned long ulTimeOut)
{
	STARTUPINFOA			Si					= { 0 };
	PROCESS_INFORMATION		Pi					= { 0 };
	int						nRet;
	unsigned long			ulTime;

	Si.cb = sizeof(Si);
	Si.wShowWindow = SW_HIDE;
	Si.dwFlags |= STARTF_USESHOWWINDOW;

	*pExitCode = -1;
	nRet = CreateProcessA(NULL, (char *)strCmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi);

	if (1 != nRet)
	{
		return false;
	}

	ulTime = ::WaitForSingleObject(Pi.hProcess, ulTimeOut);

	if (WAIT_FAILED == ulTime || WAIT_TIMEOUT == ulTime)
	{
		::TerminateProcess(Pi.hProcess, ERROR_PROCESS_ABORTED);
		*pExitCode = ERROR_PROCESS_ABORTED;
	}
	else
	{
		GetExitCodeProcess(Pi.hProcess, pExitCode);
	}

	CloseHandle(Pi.hProcess);
	CloseHandle(Pi.hThread);

	return true;
}

/**
 * 函数功能说明：根据进程名称获取进程ID
 * @param [in]  strProcName			进程名
 * @param [out] vecProcId			进程ID列表
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-9-5
 */
bool GSNUtil::GetProcIdFromName(std::string strProcName, std::vector<unsigned long> &vecProcId)
{
	int					nRet;
	HANDLE				hProcShot;
	PROCESSENTRY32W		ProcEntry;
	std::wstring			strExeFile;

	vecProcId.clear();
	strExeFile = NStr::StrToWStr(strProcName);
	hProcShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (INVALID_HANDLE_VALUE == hProcShot)
	{
		return false;
	}

	ProcEntry.dwSize = sizeof(ProcEntry);
	nRet = Process32FirstW(hProcShot, &ProcEntry);

	for (; 1 == nRet;)
	{
		if (0 == _wcsicmp(strExeFile.c_str(), ProcEntry.szExeFile))
		{
			vecProcId.push_back(ProcEntry.th32ProcessID);
		}

		nRet = Process32NextW(hProcShot, &ProcEntry);
	}

	CloseHandle(hProcShot);
	return true;
}

/**
 * 函数功能说明：根据进程名称获取进程ID
 * @param [in]  strProcName		进程名
 * @return 进程ID
 * by Maga 2016-9-5
 */
unsigned long GSNUtil::GetProcIdFromName(std::string strProcName)
{
	std::vector<unsigned long>			vecProcIdList;

	if (false == GetProcIdFromName(strProcName, vecProcIdList))
	{
		return -1;
	}

	if (0 >= vecProcIdList.size())
	{
		return -1;
	}

	return vecProcIdList[0];
}

/**
 * 函数功能说明：检测tcp端口是否被占用(true 被占用)
 * @param [in]  usPort		端口
 * @return TRUE 被占用 FALSE 未被占用
 * by Maga 2016-9-27
 */
bool GSNUtil::CheckPortTcp(unsigned short usPort)
{
	struct sockaddr_in			SocketServer;
	WSADATA						wsaData;
	SOCKET						nSocket;
	bool						bCheck = false;

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return false;
	}

	SocketServer.sin_family = AF_INET;
	SocketServer.sin_port = htons(usPort);
	SocketServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	nSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == nSocket)
	{
		WSACleanup();
		return bCheck;
	}

	if (SOCKET_ERROR != connect(nSocket, (struct sockaddr *)&SocketServer, sizeof(SocketServer)))
	{
		bCheck = true;
	}

	closesocket(nSocket);
	WSACleanup();
	return bCheck;
}

/**
 * 函数功能说明：检测udp端口是否被占用(true 被占用)
 * @param [in]  usPort		端口
 * @return TRUE 被占用 FALSE 未被占用
 * by Maga 2016-9-27
 */
bool GSNUtil::CheckPortUdp(unsigned short usPort)
{
	struct sockaddr_in			SocketServer;
	WSADATA						wsaData;
	SOCKET						nSocket;
	bool						bCheck						= false;

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return false;
	}

	SocketServer.sin_family = AF_INET;
	SocketServer.sin_port = htons(usPort);
	SocketServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	nSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (INVALID_SOCKET == nSocket)
	{
		WSACleanup();
		return bCheck;
	}

	if (SOCKET_ERROR == bind(nSocket, (SOCKADDR FAR *)&SocketServer, sizeof(SOCKADDR_IN)))
	{
		bCheck = true;
	}

	closesocket(nSocket);
	WSACleanup();
	return bCheck;
}

/**
 * 函数功能说明：创建服务
 * @param [in]  strPath			文件路径
 * @param [in]  strName			服务名称
 * @param [in]  ulServiceType	服务类型
 * @param [in]  ulStartType		启动状态
 * @param [in]  strDisplayName	显示名称
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-9-27
 */
bool GSNUtil::CreateOneService(std::string strPath, std::string strName,
							unsigned long ulServiceType /* = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS */, 
							unsigned long ulStartType /* = SERVICE_AUTO_START */, std::string strDisplayName /* = "" */)
{
	SC_HANDLE			hManager;
	SC_HANDLE			hService;

	if (strDisplayName.empty())
	{
		strDisplayName = strName;
	}

	hManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == hManager)
	{
		return false;
	}

	hService = CreateServiceA(hManager, strName.c_str(), strDisplayName.c_str(), SERVICE_ALL_ACCESS,
							ulServiceType, ulStartType, SERVICE_ERROR_NORMAL, strPath.c_str(),
							NULL, NULL, NULL, NULL, NULL);

	if (NULL == hService)
	{
		CloseServiceHandle(hManager);
		return false;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hManager);
	return true;
}

/**
 * 函数功能说明：检测服务是否存在
 * @param [in]  strName		服务名称
 * @return TRUE 存在 FALSE 不存在
 * by Maga 2016-9-27
 */
bool GSNUtil::CheckServiceExist(std::string strName)
{
	SC_HANDLE			hManager;
	SC_HANDLE			hService;

	hManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if (NULL == hManager)
	{
		return false;
	}

	hService = OpenServiceA(hManager, strName.c_str(), SERVICE_QUERY_STATUS);

	if (NULL == hService)
	{
		CloseServiceHandle(hManager);
		return false;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hManager);
	return true;
}

/**
 * 函数功能说明：
 * @param [in]  传入
 * @param [out] 传出
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-9-27
 */
bool GSNUtil::StartUpService(std::string strName, int nWait /* = 10000 */)
{
	SC_HANDLE			hManager				= NULL;
	SC_HANDLE			hService				= NULL;
	SERVICE_STATUS		ServiceStatus;
	int					nRet;
	bool				bReturn					= false;
	unsigned long		ulEndTime				= 0;
	unsigned long		ulTime					= 0;

	hManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if (NULL == hManager)
	{
		goto End;
	}

	hService = OpenServiceA(hManager, strName.c_str(), SERVICE_START | SERVICE_QUERY_STATUS);

	if (NULL == hService)
	{
		goto End;
	}

	nRet = QueryServiceStatus(hService, &ServiceStatus);

	if (1 != nRet)
	{
		goto End;
	}

	if (SERVICE_RUNNING == ServiceStatus.dwCurrentState)
	{
		bReturn = true;
		goto End;
	}

	nRet = StartServiceA(hService, 0, NULL);

	if (1 != nRet)
	{
		goto End;
	}

	ulEndTime = GetTickCount() + nWait;

	for (ulTime = GetTickCount(); ulTime < ulEndTime; ulTime = GetTickCount())
	{
		nRet = QueryServiceStatus(hService, &ServiceStatus);

		if (1 != nRet)
		{
			goto End;
		}

		if (SERVICE_RUNNING == ServiceStatus.dwCurrentState)
		{
			bReturn = true;
			break;
		}

		Sleep(100);
	}

End:

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hManager)
	{
		CloseServiceHandle(hManager);
	}

	return bReturn;
}

/**
 * 函数功能说明：暂停服务
 * @param [in]  strName		服务名称
 * @param [in]  bResume		是否恢复
 * @param [in]  nWait		等待时间
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-9-28
 */
bool GSNUtil::PauseService(std::string strName, bool bResume /* = false */, int nWait /* = 10000 */)
{
	SC_HANDLE			hManager				= NULL;
	SC_HANDLE			hService				= NULL;
	SERVICE_STATUS		ServiceStatus;
	int					nRet;
	bool				bReturn					= false;
	unsigned long		ulEndTime				= 0;
	unsigned long		ulTime					= 0;
	unsigned long		ulControl;

	hManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if (NULL == hManager)
	{
		goto End;
	}

	hService = OpenServiceA(hManager, strName.c_str(), SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS);

	if (NULL == hService)
	{
		goto End;
	}

	if (bResume)
	{
		ulControl = SERVICE_CONTROL_CONTINUE;
	}
	else
	{
		ulControl = SERVICE_CONTROL_PAUSE;
	}

	nRet = QueryServiceStatus(hService, &ServiceStatus);

	if (1 != nRet)
	{
		goto End;
	}

	nRet = ControlService(hService, ulControl, &ServiceStatus);

	if (1 != nRet)
	{
		goto End;
	}

	ulEndTime = GetTickCount() + nWait;

	for (ulTime = GetTickCount(); ulTime < ulEndTime; ulTime = GetTickCount())
	{
		nRet = QueryServiceStatus(hService, &ServiceStatus);

		if (1 != nRet)
		{
			goto End;
		}

		if (((SERVICE_PAUSED == ServiceStatus.dwCurrentState) && (bResume == false)) ||
			((SERVICE_RUNNING == ServiceStatus.dwCurrentState ) && (bResume != false))
			)
		{
			bReturn = true;
			break;
		}

		Sleep(100);
	}
	
End:

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hManager)
	{
		CloseServiceHandle(hManager);
	}

	return bReturn;
}

/**
 * 函数功能说明：停止服务
 * @param [in]  strName			服务名称
 * @param [in]  nWait			等待时间
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-9-28
 */
bool GSNUtil::StopService(std::string strName, int nWait /* = 10000 */)
{
	SC_HANDLE			hManager				= NULL;
	SC_HANDLE			hService				= NULL;
	SERVICE_STATUS		ServiceStatus;
	int					nRet;
	bool				bReturn					= false;
	unsigned long		ulEndTime				= 0;
	unsigned long		ulTime					= 0;

	hManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if (NULL == hManager)
	{
		goto End;
	}

	hService = OpenServiceA(hManager, strName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);

	if (NULL == hService)
	{
		goto End;
	}

	nRet = QueryServiceStatus(hService, &ServiceStatus);

	if (1 != nRet)
	{
		goto End;
	}

	nRet = ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);

	if (1 != nRet)
	{
		goto End;
	}

	ulEndTime = GetTickCount() + nWait;

	for (ulTime = GetTickCount(); ulTime < ulEndTime; ulTime = GetTickCount())
	{
		nRet = QueryServiceStatus(hService, &ServiceStatus);

		if (1 != nRet)
		{
			goto End;
		}

		if (SERVICE_STOPPED == ServiceStatus.dwCurrentState)
		{
			bReturn = true;
			break;
		}

		Sleep(100);
	}

End:

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hManager)
	{
		CloseServiceHandle(hManager);
	}

	return bReturn;
}

/**
 * 函数功能说明：删除服务
 * @param [in]  strName			服务名称
 * @param [in]  nWait			等待时间
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-9-28
 */
bool GSNUtil::DeleteService(std::string strName, int nWait /* = 10000 */)
{
	SC_HANDLE			hManager = NULL;
	SC_HANDLE			hService = NULL;
	SERVICE_STATUS		ServiceStatus;
	int					nRet;
	bool				bReturn = false;
	unsigned long		ulEndTime = 0;
	unsigned long		ulTime = 0;

	hManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if (NULL == hManager)
	{
		goto End;
	}

	hService = OpenServiceA(hManager, strName.c_str(), DELETE | SERVICE_QUERY_STATUS);

	if (NULL == hService)
	{
		goto End;
	}

	nRet = DeleteService(hService);

	if (1 != nRet)
	{
		goto End;
	}

	ulEndTime = GetTickCount() + nWait;

	for (ulTime = GetTickCount(); ulTime < ulEndTime; ulTime = GetTickCount())
	{
		nRet = QueryServiceStatus(hService, &ServiceStatus);

		if (1 != nRet)
		{
			goto End;
		}

		if (SERVICE_STOP_PENDING != ServiceStatus.dwCurrentState)
		{
			bReturn = true;
			break;
		}

		Sleep(100);
	}

End:

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hManager)
	{
		CloseServiceHandle(hManager);
	}

	return bReturn;
}

/**
 * 函数功能说明：检测服务是否运行
 * @param [in]  strName		服务名称
 * @return TRUE 运行 FALSE 未运行
 * by Maga 2016-12-22
 */
bool GSNUtil::CheckServiceRunning(std::string strName)
{
	SC_HANDLE			hManager				= NULL;
	SC_HANDLE			hService				= NULL;
	SERVICE_STATUS		ServiceStatus;
	int					nRet;
	bool				bReturn					= false;

	hManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if (NULL == hManager)
	{
		goto End;
	}

	hService = OpenServiceA(hManager, strName.c_str(), SERVICE_START | SERVICE_QUERY_STATUS);

	if (NULL == hService)
	{
		goto End;
	}

	nRet = QueryServiceStatus(hService, &ServiceStatus);

	if (1 != nRet)
	{
		goto End;
	}

	if (SERVICE_RUNNING == ServiceStatus.dwCurrentState)
	{
		bReturn = true;
		goto End;
	}

End:

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hManager)
	{
		CloseServiceHandle(hManager);
	}

	return bReturn;
}

/**
 * 函数功能说明：获取本机IP地址
 * @return IP地址
 * by Maga 2016-9-28
 */
std::string GSNUtil::GetLocalIp()
{
	int					nRet;
	char				szHostName[1024]			= { 0 };
	HOSTENT*			pHost;
	const char*			pIP;
	WSADATA				wsaData;

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return false;
	}

	nRet = gethostname(szHostName, 1023);

	if (SOCKET_ERROR == nRet || 0 == szHostName[0])
	{
		WSACleanup();
		return "";
	}

	pHost = gethostbyname(szHostName);

	if (NULL == pHost)
	{
		WSACleanup();
		return "";
	}

	pIP = inet_ntoa(*((struct in_addr *)pHost->h_addr_list[0]));
	WSACleanup();
	return pIP;
}

/**
 * 函数功能说明：创建一个工作对象
 * @return 句柄
 * by Maga 2016-9-29
 */
HANDLE GSNUtil::GetJobObj()
{
	HANDLE						hJobObj				= NULL;
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION        Limit;

	hJobObj = (HANDLE)CreateJobObjectA(NULL, NULL);

	if (NULL == hJobObj)
	{
		return hJobObj;
	}

	if (!QueryInformationJobObject(hJobObj, JobObjectExtendedLimitInformation,
		&Limit, sizeof(Limit), NULL))
	{
		CloseHandle(hJobObj);
		hJobObj = NULL;

		return hJobObj;
	}

	Limit.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

	if (!SetInformationJobObject(hJobObj, JobObjectExtendedLimitInformation,
		&Limit, sizeof(Limit)))
	{
		CloseHandle(hJobObj);
		hJobObj = NULL;

		return hJobObj;
	}

	return hJobObj;
}

/**
 * 函数功能说明：启动进程（加入工作集）
 * @param [in]  strPath			路径
 * @param [in]  strCmd			命令行参数
 * @param [in]  hJobObj			工作集句柄
 * @param [in]  bWait			是否等待
 * @param [in]  bHide			是否隐藏
 * @param [out] pExitCode		退出
 * @return 进程ID
 * by Maga 2016-9-29
 */
unsigned long GSNUtil::RunProcessEx(std::string strPath, std::string strCmd, HANDLE hJobObj, 
										bool bWait /* = false */, bool bHide /* = false */, int *pExitCode /* = NULL */)
{
	DWORD                   dwExitCode				= 0;
	std::string				strCmdLine;
	STARTUPINFOA            si						= { 0 };
	PROCESS_INFORMATION     pi						= { 0 };

	if (NULL == hJobObj)
	{
		return -1;
	}

	if (strPath.empty() && strCmd.empty())
	{
		return false;
	}

	if (!strCmd.empty())
	{
		if (strPath.empty())
		{
			strCmdLine = strCmd;
		}
		else
		{
			if ('\"' == strPath[0])
			{
				strCmdLine = NStr::FormatStr("%s %s", strPath.c_str(), strCmd.c_str());
			}
			else
			{
				strCmdLine = NStr::FormatStr("\"%s\" %s", strPath.c_str(), strCmd.c_str());
			}

		}
	}
	else
	{
		strCmdLine = strPath;
	}

	si.cb = sizeof(STARTUPINFOA);
	si.dwFlags = STARTF_USESHOWWINDOW;

	if (bHide)
	{
		si.wShowWindow = SW_HIDE;
	}

	if (!CreateProcessA(NULL, (char *)strCmdLine.c_str(),
		NULL, NULL,
		FALSE,
		CREATE_NO_WINDOW | CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB,
		NULL, NULL,
		&si, &pi))
	{
		return -1;
	}

	if (!AssignProcessToJobObject(hJobObj, pi.hProcess))
	{
		TerminateProcess(pi.hProcess, 1);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		return -1;
	}

	if (!ResumeThread(pi.hThread))
	{
		TerminateProcess(pi.hProcess, 1);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		return -1;
	}

	if (bWait)
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);
	}

	if (pExitCode)
	{
		if (GetExitCodeProcess(pi.hProcess, &dwExitCode))
		{
			*pExitCode = (int)dwExitCode;
		}
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return pi.dwProcessId;
}

/**
 * 函数功能说明：是否是64位系统
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-10-8
 */
bool GSNUtil::IsWow64()
{
#ifdef WIN64
	return true;
#endif

	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS			fnIsWow64Process				= NULL;
	BOOL						bIsWow64						= FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleA("kernel32"), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}

	if (bIsWow64)
	{
		return true;
	}

	return false;
}

/**
 * 函数功能说明：是否win7或更高
 * @return TRUE 是 FALSE 不是
 * by Maga 2016-12-22
 */
BOOL GSNUtil::IsWin7OrGreater()
{
	BOOL				bRetVal;
	OSVERSIONINFOEXW	OsVersionInfo;

	memset(&OsVersionInfo, 0, sizeof(OSVERSIONINFOEXW));
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

	bRetVal = GetVersionExW((OSVERSIONINFOW *)&OsVersionInfo);

	if (FALSE == bRetVal)
	{
		OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);

		bRetVal = GetVersionExW((OSVERSIONINFOW *)&OsVersionInfo);

		if (FALSE == bRetVal)
			return FALSE;
	}

	if (6 <= OsVersionInfo.dwMajorVersion)
		return TRUE;

	return FALSE;
}

/**
 * 函数功能说明：用户管理员权限运行
 * @param [in]  strFile			程序路径
 * @param [in]  strCmd			命令参数
 * @param [in]  bHide			是否隐藏
 * @return TRUE 成功 FALSE 失败
 * by Maga 2016-10-9
 */
bool GSNUtil::RunAsAdmin(std::string strFile, std::string strCmd, bool bHide /* = false */)
{
	SHELLEXECUTEINFOA   ExInfo;
	BOOL				bRetVal;

	ZeroMemory(&ExInfo, sizeof(ExInfo));

	ExInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
	ExInfo.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI;
	ExInfo.lpVerb = "runas";//_TEXT("runas");
	ExInfo.lpFile = strFile.c_str();
	ExInfo.lpParameters = strCmd.c_str();

	if (bHide)
	{
		ExInfo.nShow = SW_HIDE;
	}
	else
	{
		ExInfo.nShow = SW_SHOWNORMAL;
	}
	

	bRetVal = ShellExecuteExA(&ExInfo);

	return bRetVal;
}

