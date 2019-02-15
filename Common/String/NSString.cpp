// +----------------------------------------------------------------------
// |  C++ Basis Components
// +----------------------------------------------------------------------
// | std::wstring	¿í×Ö×Ö·û´®Àà
// | std::string 	¶à×Ö½Ú×Ö·û´®Àà 
// | NSNStr		×Ö·û´®³£ÓÃ²Ù×÷ÃüÃû¿Õ¼ä
// +----------------------------------------------------------------------
// +----------------------------------------------------------------------



#include "NSString.h"
#include <stdarg.h>
#include <assert.h>
#include <windows.h>


#ifdef GS_USE_CRYPT

#include "../GSNUtil.h"
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/des.h>

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

#endif


#define		__1K			1024
#define		__1M			1024 * __1K
#define		__1G			1024 * __1M
#define		__1T			1024 * __1G









/*
 *	Function:		StrToWStr
 *	Explanation:	¶à×Ö½Ú£¨std::string £©×ª³É¿í×Ö£¨std::wstring£©
 *	Input:			strBuf		¶à×Ö½Ú×Ö·û´®
					nCode		±àÂë
								£¨CP_ACP ±¾µØ±àÂëANSI 
								  CP_UTF8 utf8±àÂë£©
 *	Return:			Ê§°Ü·µ»Ø¿Õ×Ö·û´®
 */
std::wstring NStr::StrToWStr(std::string  strBuf, int nCode /* = CP_ACP */)
{
	std::wstring				wstrRet				= L"";
	wchar_t*				pBuf				= NULL;
	size_t					sBufSize			= 0;

	if (strBuf.empty())
	{
		return wstrRet;
	}

	sBufSize = ::MultiByteToWideChar(nCode, NULL, (char *)strBuf.c_str(), strBuf.size(), NULL, 0);

	if (0 >= sBufSize)
	{
		return wstrRet;
	}

	pBuf = new wchar_t[sBufSize + 1];

	if (sBufSize != ::MultiByteToWideChar(nCode, NULL, (char *)strBuf.c_str(), strBuf.size(), pBuf, sBufSize))
	{
		delete[]pBuf;
		
		return wstrRet;
	}

	pBuf[sBufSize] = '\0';
	wstrRet.append(pBuf, sBufSize);
	delete []pBuf;

	return wstrRet;
}

/*
 *	Function:		WStrToStr
 *	Explanation:	¿í×Ö£¨std::wstring£©×ª³É¶à×Ö½Ú£¨std::string £©
 *	Input:			wstrBuf		¿í×Ö×Ö·û´®
					nCode		±àÂë
								£¨CP_ACP ±¾µØ±àÂëANSI
								  CP_UTF8 utf8±àÂë£©
 *	Return:			Ê§°Ü·µ»Ø¿Õ×Ö·û´®
 */
std::string  NStr::WStrToStr(std::wstring wstrBuf, int nCode /* = CP_ACP */)
{
	std::string 				strRet				= "";
	char*					pBuf				= NULL;
	size_t					sBufSize			= 0;

	if (wstrBuf.empty())
	{
		return strRet;
	}

	sBufSize = ::WideCharToMultiByte(nCode, NULL, (wchar_t *)wstrBuf.c_str(), wstrBuf.size(), NULL, 0, NULL, NULL);

	if (0 >= sBufSize)
	{
		return strRet;
	}

	pBuf = new char[sBufSize + 1];

	if (sBufSize != ::WideCharToMultiByte(nCode, NULL, (wchar_t *)wstrBuf.c_str(), wstrBuf.size(), pBuf, sBufSize, NULL, NULL))
	{
		delete []pBuf;

		return strRet;
	}

	pBuf[sBufSize] = '\0';
	strRet.append(pBuf, sBufSize);
	delete []pBuf;

	return strRet;
}

/*
 *	Function:		GetUtf8
 *	Explanation:	±àÂë×ª»» ANSI => UTF8
 *	Input:			strBuf			ANSI×Ö·û´®
 *	Return:			UTF8×Ö·û´®
 */
std::string  NStr::GetUtf8(std::string  strBuf)
{
	std::wstring			wstrBuf					= L"";

	if (strBuf.empty())
	{
		return "";
	}

	wstrBuf = NStr::StrToWStr(strBuf);

	return NStr::WStrToStr(wstrBuf, CP_UTF8);
}

/*
*	Function:		GetAnsi
*	Explanation:	±àÂë×ª»» UTF8 => ANSI
*	Input:			strBuf			UTF8×Ö·û´®
*	Return:			ANSI×Ö·û´®
*/
std::string  NStr::GetAnsi(std::string  strBuf)
{
	std::wstring			wstrBuf = L"";

	if (strBuf.empty())
	{
		return "";
	}

	wstrBuf = NStr::StrToWStr(strBuf, CP_UTF8);

	return NStr::WStrToStr(wstrBuf);
}


std::string NStr::Unicode2ANSI(LPCWSTR lpszSrc)
{
	std::string sResult;
	if (lpszSrc != NULL)
	{
		int  nANSILen = WideCharToMultiByte(CP_ACP, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
		char* pANSI = new char[nANSILen + 1];
		if (pANSI != NULL)
		{
			ZeroMemory(pANSI, nANSILen + 1);
			WideCharToMultiByte(CP_ACP, 0, lpszSrc, -1, pANSI, nANSILen, NULL, NULL);
			sResult = pANSI;
			delete[] pANSI;
		}
	}
	return sResult;
}

std::string NStr::Unicode2UTF8(LPCWSTR lpszSrc)
{
	std::string sResult;
	if (lpszSrc != NULL)
	{
		int  nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
		char* pUTF8 = new char[nUTF8Len + 1];
		if (pUTF8 != NULL)
		{
			ZeroMemory(pUTF8, nUTF8Len + 1);
			WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, pUTF8, nUTF8Len, NULL, NULL);
			sResult = pUTF8;
			delete[] pUTF8;
		}
	}
	return sResult;
}

std::wstring NStr::ANSI2Unicode(LPCSTR lpszSrc)
{
	std::wstring sResult;
	if (lpszSrc != NULL)
	{
		int nUnicodeLen = MultiByteToWideChar(CP_ACP, 0, lpszSrc, -1, NULL, 0);
		LPWSTR pUnicode = new WCHAR[nUnicodeLen + 1];
		if (pUnicode != NULL)
		{
			ZeroMemory((void*)pUnicode, (nUnicodeLen + 1) * sizeof(WCHAR));
			MultiByteToWideChar(CP_ACP, 0, lpszSrc, -1, pUnicode, nUnicodeLen);
			sResult = pUnicode;
			delete[] pUnicode;
		}
	}
	return sResult;
}

_tstring NStr::ANSI2T(LPCSTR lpSrc)
{
#ifdef _UNICODE
	return ANSI2Unicode(lpSrc);
#else
	return lpSrc;
#endif
}
std::string NStr::T2ANSI(LPCTSTR lpSrc)
{
#ifdef _UNICODE
	return Unicode2ANSI(lpSrc);
#else
	return lpSrc;
#endif
}

std::string NStr::T2UTF8(LPCTSTR lpSrc)
{
#ifdef _UNICODE
	return Unicode2UTF8(lpSrc);
#else
	return lpSrc;
#endif
}

std::string NStr::utf16ToUtf8(LPCWSTR lpszSrc)
{
	std::string sResult;

	int  nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
	char* pUTF8 = new char[nUTF8Len + 1];
	ZeroMemory(pUTF8, nUTF8Len + 1);
	WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, pUTF8, nUTF8Len, NULL, NULL);
	sResult = pUTF8;
	delete[] pUTF8;

	return sResult;
}

std::wstring NStr::utf8ToUtf16(const std::string& utf8String)
{
	std::wstring sResult;
	int nUTF8Len = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, NULL, NULL);
	wchar_t* pUTF8 = new wchar_t[nUTF8Len + 1];

	ZeroMemory(pUTF8, nUTF8Len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, pUTF8, nUTF8Len);
	sResult = pUTF8;
	delete[] pUTF8;

	return sResult;
}


/*
 *	Function:		StrToInt
 *	Explanation:	×Ö·û´®×ª³Éint
 *	Input:			strBuf		×Ö·û´®
 *	Return:			intÊýÖµ
 */
int NStr::StrToInt(std::string  strBuf)
{
	if (strBuf.empty())
	{
		return 0;
	}

	return atoi((char *)strBuf.c_str());
}

int NStr::StrToInt(std::wstring strBuf)
{
	if (strBuf.empty())
	{
		return 0;
	}

	return _wtoi((wchar_t *)strBuf.c_str());
}

/*
 *	Function:		IntToStr
 *	Explanation:	int×ª×Ö·û´®
 *	Input:			nValue		ÊýÖµ
 *	Return:			×Ö·û´®
 */
std::string  NStr::IntToStr(int nValue)
{
	char				szBuf[1024]					= { 0 };

	_itoa_s(nValue, szBuf, 10);

	return szBuf;
}

std::wstring NStr::IntToWStr(int nValue)
{
	wchar_t				szBuf[1024]					= { 0 };

	_itow_s(nValue, szBuf, 10);

	return szBuf;
}

/*
 *	Function:		StrToFloat
 *	Explanation:	×Ö·û´®×ª³Éfloat
 *	Input:			strBuf		×Ö·û´®
 *	Return:			floatÊýÖµ
 */
float NStr::StrToFloat(std::string  strBuf)
{
	if (strBuf.empty())
	{
		return 0;
	}


	return (float)atof((char *)strBuf.c_str());
}

float NStr::StrToFloat(std::wstring strBuf)
{
	if (strBuf.empty())
	{
		return 0;
	}

	return (float)_wtof((wchar_t *)strBuf.c_str());
}

/*
 *	Function:		FloatToStr
 *	Explanation:	float×ª³É×Ö·û´®
 *	Input:			fValue		¸¡µãÊý
 *	Return:			×Ö·û´®
 */
std::string  NStr::FloatToStr(float fValue)
{
	char				szBuf[1024]					= { 0 };

	_gcvt_s(szBuf, fValue, 8);

	return szBuf;
}

std::wstring NStr::FloatToWStr(float fValue)
{
	std::string 			strValue					= NStr::FloatToStr(fValue);

	return NStr::StrToWStr(strValue);
}

/*
 *	Function:		StrToDouble
 *	Explanation:	×Ö·û´®×ª³Édouble
 *	Input:			strBuf		×Ö·û´®
 *	Return:			double
 */
double NStr::StrToDouble(std::string  strBuf)
{
	return strtod((char *)strBuf.c_str(), NULL);
}

double NStr::StrToDouble(std::wstring strBuf)
{
	std::string 			strValue					= NStr::WStrToStr(strBuf);

	return NStr::StrToDouble(strValue);
}

/*
 *	Function:		double×ª×Ö·û´®
 *	Explanation:	DoubleToStr
 *	Input:			dbValue		doubleÖµ
 *	Return:			×Ö·û´®
 */
std::string  NStr::DoubleToStr(double dbValue)
{
	char				szBuf[1024]					= { 0 };

	_gcvt_s(szBuf, dbValue, 8);

	return szBuf;
}

std::wstring NStr::DoubleToWStr(double dbValue)
{
	std::string 			strValue					= NStr::DoubleToStr(dbValue);

	return NStr::StrToWStr(strValue);
}

std::string  NStr::DoubleToStr(double dbValue, int nDigit)
{
	char				szBuf[1024]					= { 0 };
	char				szFormat[1024]				= { 0 };

	_snprintf(szFormat, 1023, "%%.%df", nDigit);
	_snprintf(szBuf, 1023, szFormat, dbValue);

	return szBuf;
}

std::wstring NStr::DoubleToWStr(double dbValue, int nDigit)
{
	std::string 			strValue					= NStr::DoubleToStr(dbValue, nDigit);

	return NStr::StrToWStr(strValue);
}

/*
 *	Function:		StringToLong
 *	Explanation:	×Ö·û´®×ªlong
 *	Input:			strBuf		×Ö·û´®
 *	Return:			long
 */
long NStr::StringToLong(std::string  strBuf)
{
	return atol((char *)strBuf.c_str());
}

long NStr::StringToLong(std::wstring strBuf)
{
	
	return _wtol((wchar_t *)strBuf.c_str());
}

/*
 *	Function:		LongToStr
 *	Explanation:	long×ª×Ö·û´®
 *	Input:			lValue		longÊýÖµ
 *	Return:			×Ö·û´®
 */
std::string  NStr::LongToStr(long lValue)
{
	char					szBuf[1024]				= { 0 };

	_ltoa_s(lValue, szBuf, 10);

	return szBuf;
}

std::wstring NStr::LongToWStr(long lValue)
{
	wchar_t					szBuf[1024]				= { 0 };

	_ltow_s(lValue, szBuf, 10);

	return szBuf;
}

/*
 *	Function:		StrToInt64
 *	Explanation:	×Ö·û´®×ª__int64
 *	Input:			strBuf		×Ö·û´®
 *	Return:			__int64ÊýÖµ
 */
__int64 NStr::StrToInt64(std::string  strBuf)
{
	return atoll((char *)strBuf.c_str());
}

__int64 NStr::StrToInt64(std::wstring strBuf)
{
	return _wtoll((wchar_t *)strBuf.c_str());
}

/*
 *	Function:		Int64ToStr
 *	Explanation:	__int64×ª×Ö·û´®
 *	Input:			nValue		ÊýÖµ
 *	Return:			×Ö·û´®
 */
std::string  NStr::Int64ToStr(__int64 nValue)
{
	char				szValue[1024]				= { 0 };

	_i64toa_s(nValue, szValue, 1023, 10);

	return szValue;
}

std::wstring NStr::Int64ToWStr(__int64 nValue)
{
	wchar_t				szValue[1024]				= { 0 };

	_i64tow_s(nValue, szValue, 1023, 10);

	return szValue;
}

/*
 *	Function:		SpliteStrByChar
 *	Explanation:	×Ö·û´®·Ö¸î(°´×Ö·ûc·Ö¸î)
 *	Input:			strBuf		×Ö·û´®	
					c			·Ö¸îµÄ×Ö·û
					bSkipEmpty	ÊÇ·ñÌø¹ý¿Õ´®
 *  Output:			vecList		·Ö¸îºóµÄ×Ö·û´®
 *	Return:			vecList´óÐ¡
 */
int NStr::SpliteStrByChar(std::string  strBuf,  std::vector<std::string > &vecList,
								char c /* = '|' */, bool bSkipEmpty /* = false */)
{
	size_t				sBufSize			= strBuf.size();
	char*				pStart				= (char *)strBuf.c_str();
	char*				pEnd				= pStart + sBufSize;

	vecList.clear();

	if (strBuf.empty())
	{
		return 0;
	}

	for (;;Sleep(1))
	{
		std::string 	strTemp		= "";
		char*		pFind		= strchr(pStart, c);

		if (NULL == pFind)
		{
			strTemp = pStart;

			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
			}
			
			break;
		}

		strTemp.append(pStart, pFind);

		if (!bSkipEmpty || !strTemp.empty())
		{
			vecList.push_back(strTemp);
		}

		pStart = pFind + 1;

		if (pStart >= pEnd)
		{
			break;
		}
	}

	return vecList.size();
}

int NStr::SpliteStrByChar(std::wstring strBuf,  std::vector<std::wstring> &vecList,
								char c /* = '|' */, bool bSkipEmpty /* = false */)
{
	size_t				sBufSize			= strBuf.size();
	wchar_t*			pStart				= (wchar_t *)strBuf.c_str();
	wchar_t*			pEnd				= pStart + sBufSize;

	vecList.clear();

	if (strBuf.empty())
	{
		return 0;
	}

	for (;; Sleep(1))
	{
		std::wstring	strTemp			= L"";
		wchar_t*	pFind			= wcschr(pStart, c);

		if (NULL == pFind)
		{
			strTemp = pStart;

			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
			}

			break;
		}

		strTemp.append(pStart, pFind);

		if (!bSkipEmpty || !strTemp.empty())
		{
			vecList.push_back(strTemp);
		}

		pStart = pFind + 1;

		if (pStart >= pEnd)
		{
			break;
		}
	}

	return vecList.size();
}

/*
 *	Function:		SpliteStrByChars
 *	Explanation:	×Ö·û´®·Ö¸î(ÒÔÔÚstrCharsÖÐ³öÏÖµÄ×Ö·û·Ö¸î)
 *	Input:			strBuf		×Ö·û´®
					strChars	·Ö¸î×Ö·û
					bSkipEmpty	ÊÇ·ñÌø¹ý¿Õ´®	
 *  Output:			Êä³ö
 *	Return:			TRUE ³É¹¦ FALSE Ê§°Ü
 */
int NStr::SpliteStrByChars(std::string  strBuf,  std::vector<std::string > &vecList,
								std::string  strChars /* = "|" */, bool bSkipEmpty /* = false */)
{
	size_t				sBufSize				= strBuf.size();
	char*				pStart					= (char *)strBuf.c_str();
	char*				pEnd					= pStart + sBufSize;
	std::string 			strTemp					= "";

	vecList.clear();

	if (strBuf.empty() || strChars.empty())
	{
		return 0;
	}

	for (;;Sleep(1))
	{
		char*		pFind		= NULL;

		pFind = strchr((char *)strChars.c_str(), *pStart);

		if (NULL == pFind)
		{
			strTemp.append(1, *pStart);
		}
		else
		{
			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
				strTemp.clear();
			}
		}

		pStart++;

		if (pStart >= pEnd)
		{
			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
				strTemp.clear();
			}
			break;
		}
	}

	return vecList.size();
}

int NStr::SpliteStrByChars(std::wstring strBuf,  std::vector<std::wstring> &vecList,
								std::wstring strChars /* = L"|" */, bool bSkipEmpty /* = false */)
{
	size_t				sBufSize			= strBuf.size();
	wchar_t*			pStart				= (wchar_t *)strBuf.c_str();
	wchar_t*			pEnd				= pStart + sBufSize;
	std::wstring			strTemp				= L"";

	vecList.clear();

	if (strBuf.empty() || strChars.empty())
	{
		return 0;
	}

	for (;; Sleep(1))
	{
		wchar_t*		pFind		= NULL;

		pFind = wcschr((wchar_t *)strChars.c_str(), *pStart);

		if (NULL == pFind)
		{
			strTemp.append(1, *pStart);
		}
		else
		{
			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
				strTemp.clear();
			}
		}

		pStart++;

		if (pStart >= pEnd)
		{
			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
				strTemp.clear();
			}
			break;
		}
	}

	return vecList.size();
}

/*
 *	Function:		SpliteStrByStr
 *	Explanation:	×Ö·û´®·Ö¸î£¨°´×Ö·û´®·Ö¸î£©
 *	Input:			strBuf		×Ö·û´®
					strSplite	·Ö¸î×Ö·û´®
					bSkipEmpty	ÊÇ·ñÌø¹ý¿Õ´®
 *  Output:			vecList		ÁÐ±í
 *	Return:			vecList´óÐ¡
 */
int NStr::SpliteStrByStr(std::string  strBuf,  std::vector<std::string > &vecList,
								std::string  strSplite /* = "|" */, bool bSkipEmpty /* = false */)
{
	size_t				sBufSize				= strBuf.size();
	char*				pStart					= (char *)strBuf.c_str();
	char*				pEnd					= pStart + sBufSize;

	vecList.clear();

	if (strBuf.empty())
	{
		return 0;
	}

	for (;; Sleep(1))
	{
		std::string 	strTemp			= "";
		char*		pFind			= strstr(pStart, (char *)strSplite.c_str());

		if (NULL == pFind)
		{
			strTemp = pStart;

			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
			}

			break;
		}

		strTemp.append(pStart, pFind);

		if (!bSkipEmpty || !strTemp.empty())
		{
			vecList.push_back(strTemp);
		}

		pStart = pFind + strSplite.size();

		if (pStart >= pEnd)
		{
			break;
		}
	}

	return vecList.size();
}

int NStr::SpliteStrByStr(std::wstring strBuf,  std::vector<std::wstring> &vecList,
								std::wstring strSplite /* = L"|" */, bool bSkipEmpty /* = false */)
{
	size_t				sBufSize				= strBuf.size();
	wchar_t*			pStart					= (wchar_t *)strBuf.c_str();
	wchar_t*			pEnd					= pStart + sBufSize;

	vecList.clear();

	if (strBuf.empty())
	{
		return 0;
	}

	for (;; Sleep(1))
	{
		std::wstring	strTemp			= L"";
		wchar_t*	pFind			= wcsstr(pStart, (wchar_t *)strSplite.c_str());

		if (NULL == pFind)
		{
			strTemp = pStart;

			if (!bSkipEmpty || !strTemp.empty())
			{
				vecList.push_back(strTemp);
			}

			break;
		}

		strTemp.append(pStart, pFind);

		if (!bSkipEmpty || !strTemp.empty())
		{
			vecList.push_back(strTemp);
		}

		pStart = pFind + strSplite.size();

		if (pStart >= pEnd)
		{
			break;
		}
	}

	return vecList.size();
}

/*
 *	Function:		MutilStrToArrays
 *	Explanation:	¶à¸ö×Ö·û´®·Ö¸î£¨ÒÔ\0½áÊø·û·Ö¸î£©
 *	Input:			pData			¶à¸ö×Ö·û´®
 *  Output:			vecList			×Ö·û´®ÁÐ±í
 *	Return:			×Ö·û´®¸öÊý(vecList.size())
 */
int NStr::MutilStrToArrays(char *pData,  std::vector<std::string > &vecList)
{
	std::string 			strBuf;

	for (;;)
	{
		strBuf = pData;
		vecList.push_back(strBuf);
		pData += strlen(pData) + 1;

		if (0 == pData[0])
		{
			break;
		}
	}

	return vecList.size();
}

int NStr::MutilStrToArrays(wchar_t *pData,  std::vector<std::wstring> &vecList)
{
	std::wstring			strBuf;

	for (;;)
	{
		strBuf = pData;
		vecList.push_back(strBuf);
		pData += wcslen(pData) + 1;

		if (0 == pData[0])
		{
			break;
		}
	}

	return vecList.size();
}

/*
 *	Function:		DelStrByChars
 *	Explanation:	É¾³ý×Ö·û´®ÖÐÖ¸¶¨µÄ×Ö·û(ÔÚstrCharsÖÐµÄ¶¼É¾³ý)
 *	Input:			strBuf		×Ö·û´®
					strDel		ÐèÒªÉ¾³ýµÄ×Ö·û
 *  Output:			strBuf		É¾³ýºóµÄ×Ö·û´®
 *	Return:			É¾³ýºó×Ö·û´®µÄ³¤¶È
 */
int NStr::DelStrByChars(std::string  &strBuf, std::string  strDel)
{
	size_t				sBufSize				= strBuf.size();
	char*				pStart					= (char *)strBuf.c_str();
	char*				pEnd					= pStart + sBufSize;
	std::string 			strReturn				= "";


	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (;; Sleep(1))
	{
		char*		pFind		= NULL;

		pFind = strchr((char *)strDel.c_str(), *pStart);

		if (NULL == pFind)
		{
			strReturn.append(1, *pStart);
		}
		
		pStart++;

		if (pStart >= pEnd)
		{
			break;
		}
	}

	strBuf = strReturn;

	return strBuf.size();
}

int NStr::DelStrByChars(std::wstring &strBuf, std::wstring strDel)
{
	size_t				sBufSize				= strBuf.size();
	wchar_t*			pStart					= (wchar_t *)strBuf.c_str();
	wchar_t*			pEnd					= pStart + sBufSize;
	std::wstring			strReturn				= L"";

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (;; Sleep(1))
	{
		wchar_t*		pFind = NULL;

		pFind = wcschr((wchar_t *)strDel.c_str(), *pStart);

		if (NULL == pFind)
		{
			strReturn.append(1, *pStart);
		}

		pStart++;

		if (pStart >= pEnd)
		{
			break;
		}
	}

	strBuf = strReturn;

	return strBuf.size();
}

/*
 *	Function:		DelStrByChar
 *	Explanation:	É¾³ý×Ö·û´®ÖÐÖ¸¶¨µÄ×Ö·û
 *	Input:			strBuf		×Ö·û´®
					c			ÐèÒªÉ¾³ýµÃ×Ö·û
 *  Output:			strBuf		É¾³ýºóµÄ×Ö·û´®
 *	Return:			É¾³ýºó×Ö·û´®³¤¶È
 */
int NStr::DelStrByStr(std::string  &strBuf, std::string  strDel)
{
	size_t				sBufSize				= strBuf.size();
	char*				pStart					= (char *)strBuf.c_str();
	char*				pEnd					= pStart + sBufSize;
	std::string 			strReturn;

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (;; Sleep(1))
	{
		char*		pFind			= strstr(pStart, (char *)strDel.c_str());

		if (NULL == pFind)
		{
			strReturn.append(pStart);
			break;
		}

		strReturn.append(pStart, pFind);
		pStart = pFind + strDel.size();

		if (pStart >= pEnd)
		{
			break;
		}
	}

	strBuf = strReturn;

	return strBuf.size();
}

int NStr::DelStrByStr(std::wstring &strBuf, std::wstring strDel)
{
	size_t				sBufSize				= strBuf.size();
	wchar_t*			pStart					= (wchar_t *)strBuf.c_str();
	wchar_t*			pEnd					= pStart + sBufSize;
	std::wstring			strReturn;

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (;; Sleep(1))
	{
		wchar_t*		pFind = wcsstr(pStart, (wchar_t *)strDel.c_str());

		if (NULL == pFind)
		{
			strReturn.append(pStart);
			break;
		}

		strReturn.append(pStart, pFind);
		pStart = pFind + strDel.size();

		if (pStart >= pEnd)
		{
			break;
		}
	}

	strBuf = strReturn;

	return strBuf.size();
}

/*
 *	Function:		ReplaceStr
 *	Explanation:	Ìæ»»×Ö·û´®ÖÐÖ¸¶¨µÄ×Ö·û´®
 *	Input:			strBuf		×Ö·û´®
					strSrc		ÐèÒªÌæ»»µÄ×Ö·û´®
					strDes		Ìæ»»µÄ×Ö·û´®
 *  Output:			strBuf		Ìæ»»ºóµÄ×Ö·û´®
 *	Return:			Ìæ»»´ÎÊý
 */
int NStr::ReplaceStr(std::string  &strBuf, std::string  strSrc, std::string  strDes)
{
	size_t				sBufSize				= strBuf.size();
	char*				pStart					= (char *)strBuf.c_str();
	char*				pEnd					= pStart + sBufSize;
	std::string 			strReturn;
	int					nCount					= 0;

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (;;)
	{
		char*		pFind = strstr(pStart, (char *)strSrc.c_str());

		if (NULL == pFind)
		{
			strReturn.append(pStart);
			break;
		}

		nCount++;
		strReturn.append(pStart, pFind);
		strReturn.append(strDes);
		pStart = pFind + strSrc.size();

		if (pStart >= pEnd)
		{
			break;
		}
	}

	strBuf = strReturn;

	return nCount;
}

int NStr::ReplaceStr(std::wstring &strBuf, std::wstring strSrc, std::wstring strDes)
{
	size_t				sBufSize				= strBuf.size();
	wchar_t*			pStart					= (wchar_t *)strBuf.c_str();
	wchar_t*			pEnd					= pStart + sBufSize;
	std::wstring			strReturn;
	int					nCount					= 0;

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (;; Sleep(1))
	{
		wchar_t*		pFind = wcsstr(pStart, (wchar_t *)strSrc.c_str());

		if (NULL == pFind)
		{
			strReturn.append(pStart);
			break;
		}

		nCount++;
		strReturn.append(pStart, pFind);
		strReturn.append(strDes);
		pStart = pFind + strSrc.size();

		if (pStart >= pEnd)
		{
			break;
		}
	}

	strBuf = strReturn;

	return nCount;
}

/*
 *	Function:		StrHeadCmp
 *	Explanation:	×Ö·û´®Í·²¿±È½Ï
 *	Input:			strBuf		×Ö·û´®(³¤×Ö·û´®)
					strHead		Í·²¿×Ö·û´®
					bNoCase		ÊÇ·ñÇø·Ö´óÐ¡Ð´
 *	Return:			0 ÏàµÈ  >0 ´ó <0 Ð¡
 */
int NStr::StrHeadCmp(std::string  strBuf, std::string  strHead, bool bNoCase /* = true */)
{
	if (strBuf.size() < strHead.size())
	{
		return -1;
	}

	if (bNoCase)
	{
		return _strnicmp((char *)strBuf.c_str(), (char *)strHead.c_str(), strHead.size());
	}

	return strncmp((char *)strBuf.c_str(), (char *)strHead.c_str(), strHead.size());
}

int NStr::StrHeadCmp(std::wstring strBuf, std::wstring strHead, bool bNoCase /* = true */)
{
	if (strBuf.size() < strHead.size())
	{
		return -1;
	}

	if (bNoCase)
	{
		return _wcsnicmp((wchar_t *)strBuf.c_str(), (wchar_t *)strHead.c_str(), strHead.size());
	}

	return wcsncmp((wchar_t *)strBuf.c_str(), (wchar_t *)strHead.c_str(), strHead.size());
}

/*
 *	Function:		StrTailCmp
 *	Explanation:	×Ö·û´®Î²²¿±È½Ï
 *	Input:			strBuf		×Ö·û´®£¨³¤×Ö·û´®£©
					strTail		Î²²¿×Ö·û´®
					bNoCase		ÊÇ·ñÇø·Ö´óÐ¡Ð´
 *	Return:			0 ÏàµÈ  >0 ´ó <0 Ð¡
 */
int NStr::StrTailCmp(std::string  strBuf, std::string  strTail, bool bNoCase /* = true */)
{
	int					nPos					= 0;
	char*				pStart					= NULL;

	nPos = strBuf.size() - strTail.size();

	if (0 > nPos)
	{
		return -1;
	}

	pStart = (char *)strBuf.c_str() + nPos;

	if (bNoCase)
	{
		return _strnicmp(pStart, (char *)strTail.c_str(), strTail.size());
	}

	return strncmp(pStart, (char *)strTail.c_str(), strTail.size());
}

int NStr::StrTailCmp(std::wstring strBuf, std::wstring strTail, bool bNoCase /* = true */)
{
	int					nPos				= 0;
	wchar_t*			pStart				= NULL;

	nPos = strBuf.size() - strTail.size();

	if (0 > nPos)
	{
		return -1;
	}

	pStart = (wchar_t *)strBuf.c_str() + nPos;

	if (bNoCase)
	{
		return _wcsnicmp(pStart, (wchar_t *)strTail.c_str(), strTail.size());
	}

	return wcsncmp(pStart, (wchar_t *)strTail.c_str(), strTail.size());
}

/*
 *	Function:		StrTrim
 *	Explanation:	È¥³ý×Ö·û´®Á½±ß³öÏÖµÄ×Ö·û(ÔÚstrCharsÖÐ³öÏÖµÄ¶¼É¾³ý)
 *	Input:			strBuf		×Ö·û´®
					strChars	ÐèÒªÈ¥³ýµÄ×Ö·û
 *	Output:			strBuf		È¥³ýºóµÄ×Ö·û´®
 *	Return:			È¥³ýºó×Ö·û´®µÄ´óÐ¡
 */
int NStr::StrTrim(std::string  &strBuf, std::string  strChars /* = " " */)
{
	std::string ::iterator				Iter;		// ÕýÏòµü´úÆ÷
	std::string ::reverse_iterator		reIter;		// ·´Ïòµü´úÆ÷

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		Iter = strBuf.begin();

		if (NULL != strchr((char *)strChars.c_str(), *Iter))
		{
			strBuf.erase(Iter);
			continue;
		}

		break;
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		reIter = strBuf.rbegin();

		if (NULL != strchr((char *)strChars.c_str(), *reIter))
		{
			strBuf.erase(strBuf.begin() + strBuf.size() - 1);
			continue;
		}

		break;
	}

	return strBuf.size();
}

int NStr::StrTrim(std::wstring &strBuf, std::wstring strChars /* = L" " */)
{
	std::wstring::iterator				Iter;		// ÕýÏòµü´úÆ÷
	std::wstring::reverse_iterator		reIter;		// ·´Ïòµü´úÆ÷

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		Iter = strBuf.begin();

		if (NULL != wcschr((wchar_t *)strChars.c_str(), *Iter))
		{
			strBuf.erase(Iter);
			continue;
		}

		break;
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		reIter = strBuf.rbegin();

		if (NULL != wcschr((wchar_t *)strChars.c_str(), *reIter))
		{
			strBuf.erase(strBuf.begin() + strBuf.size() - 1);
			continue;
		}

		break;
	}

	return strBuf.size();
}

/*
 *	Function:		StrTrimLeft
 *	Explanation:	È¥³ý×Ö·û´®×ó±ß³öÏÖµÄÖ¸¶¨×Ö·û(ÔÚstrCharsÖÐ³öÏÖµÄ¶¼É¾³ý)
 *	Input:			strBuf		×Ö·û´®
					strChars	ÐèÒªÈ¥³ýµÄ×Ö·û
 *	Output:			strBuf		È¥³ýºóµÄ×Ö·û´®
 *	Return:			È¥³ýºó×Ö·û´®µÄ´óÐ¡
 */
int NStr::StrTrimLeft(std::string  &strBuf, std::string  strChars /* = " " */)
{
	std::string ::iterator				Iter;		// ÕýÏòµü´úÆ÷

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		Iter = strBuf.begin();

		if (NULL != strchr((char *)strChars.c_str(), *Iter))
		{
			strBuf.erase(Iter);
			continue;
		}

		break;
	}

	return strBuf.size();
}

int NStr::StrTrimLeft(std::wstring &strBuf, std::wstring strChars /* = L" " */)
{
	std::wstring::iterator				Iter;		// ÕýÏòµü´úÆ÷

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		Iter = strBuf.begin();

		if (NULL != wcschr((wchar_t *)strChars.c_str(), *Iter))
		{
			strBuf.erase(Iter);
			continue;
		}

		break;
	}

	return strBuf.size();
}

/*
 *	Function:		StrTrimRight
 *	Explanation:	È¥³ý×Ö·û´®ÓÒ±ß³öÏÖµÄÖ¸¶¨×Ö·û(ÔÚstrCharsÖÐ³öÏÖµÄ¶¼É¾³ý)
 *	Input:			strBuf		×Ö·û´®
					strChars	ÐèÒªÈ¥³ýµÄ×Ö·û
 *	Output:			strBuf		È¥³ýºóµÄ×Ö·û´®
 *	Return:			È¥³ýºó×Ö·û´®µÄ´óÐ¡
 */
int NStr::StrTrimRight(std::string  &strBuf, std::string  strChars /* = " " */)
{
	std::string ::reverse_iterator		reIter;		// ·´Ïòµü´úÆ÷

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		reIter = strBuf.rbegin();

		if (NULL != strchr((char *)strChars.c_str(), *reIter))
		{
			strBuf.erase(strBuf.begin() + strBuf.size() - 1);
			continue;
		}

		break;
	}

	return strBuf.size();
}

int NStr::StrTrimRight(std::wstring &strBuf, std::wstring strChars /* = L" " */)
{
	std::wstring::reverse_iterator		reIter;		// ·´Ïòµü´úÆ÷

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (size_t sIndex = 0; sIndex < strBuf.size(); sIndex++)
	{
		reIter = strBuf.rbegin();

		if (NULL != wcschr((wchar_t *)strChars.c_str(), *reIter))
		{
			strBuf.erase(strBuf.begin() + strBuf.size() - 1);
			continue;
		}

		break;
	}

	return strBuf.size();
}

/*
 *	Function:		CreateRandomStr
 *	Explanation:	´´½¨Ëæ»ú×Ö·û´®
 *	Input:			nSize		×Ö·û´®³¤¶È	
					strSeed		Ëæ»úÖÖ×Ó
 *	Return:			Ëæ»ú×Ö·û´®
 */
std::string  NStr::CreateRandomStr(int nSize, std::string  strSeed /* = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" */)
{
	std::string 				strReturn;
	char*					pStr					= (char *)strSeed.c_str();
	size_t					sLength					= strSeed.size();
	static unsigned long	lThreadId				= 0;

	if (strSeed.empty())
	{
		return "";
	}

	if (GetCurrentThreadId() != lThreadId)
	{
		srand(GetTickCount());
		lThreadId = GetCurrentThreadId();
	}

	for (int i = 0; i < nSize; i++)
	{
		strReturn.append(1, pStr[rand() % sLength]);
	}

	return strReturn;
}

std::wstring NStr::CreateRandomWStr(int nSize, std::wstring strSeed /* = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" */)
{
	std::wstring				strReturn;
	wchar_t*				pStr				= (wchar_t *)strSeed.c_str();
	size_t					sLength				= strSeed.size();
	static unsigned long	lThreadId			= 0;

	if (strSeed.empty())
	{
		return L"";
	}

	if (GetCurrentThreadId() != lThreadId)
	{
		srand(GetTickCount());
		lThreadId = GetCurrentThreadId();
	}

	for (int i = 0; i < nSize; i++)
	{
		strReturn.append(1, pStr[rand() % sLength]);
	}

	return strReturn;
}

/*
 *	Function:		FormatStr
 *	Explanation:	¸ñÊ½»¯×Ö·û´®
 *	Input:			pFmt		¸ñÊ½»¯´®
 *	Return:			¸ñÊ½»¯ºóµÄ×Ö·û´®
 */
std::string  NStr::FormatStr(const char* pFmt, ...)
{
	std::string 			strBuf				= "";
	va_list				varList;
	char*				pData				= NULL;
	int					nRet				= 0;
	int					nSize				= 2048;
	bool				bSuccess			= false;

	pData = (char *)malloc(nSize);

	if (NULL == pData)
	{
		return strBuf;
	}

	va_start(varList, pFmt);

	for (; nSize < (10 * __1M); Sleep(1))
	{
		nRet = _vsnprintf(pData, nSize - 1, pFmt, varList);

		if (0 <= nRet)
		{
			bSuccess = true;
			break;
		}

		free(pData);
		nSize *= 2;
		pData = (char *)malloc(nSize);

		if (NULL == pData)
		{
			return strBuf;
		}
	}

	va_end(varList);

	if (bSuccess)
	{
		strBuf = pData;
	}

	free(pData);

	return strBuf;
}

/*
*	Function:		FormatStr
*	Explanation:	¸ñÊ½»¯×Ö·û´®
*	Input:			pFmt		¸ñÊ½»¯´®
*	Return:			¸ñÊ½»¯ºóµÄ×Ö·û´®
*/
std::wstring NStr::FormatWStr(const wchar_t* pFmt, ...)
{
	std::wstring			strBuf				= L"";
	va_list				varList;
	wchar_t*			pData				= NULL;
	int					nRet				= 0;
	int					nSize				= 2048;
	bool				bSuccess			= false;

	pData = (wchar_t *)malloc(nSize);

	if (NULL == pData)
	{
		return strBuf;
	}

	va_start(varList, pFmt);

	for (; nSize < (10 * __1M); Sleep(1))
	{
		nRet = _vsnwprintf(pData, nSize - 2, pFmt, varList);

		if (0 <= nRet)
		{
			bSuccess = true;
			break;
		}

		free(pData);
		nSize *= 2;
		pData = (wchar_t *)malloc(nSize);

		if (NULL == pData)
		{
			return strBuf;
		}
	}

	va_end(varList);

	if (bSuccess)
	{
		strBuf = pData;
	}

	free(pData);

	return strBuf;
}

/*
 *	Function:		StrToLwr
 *	Explanation:	×Ö·û´®×ªÐ¡Ð´
 *	Input:			strBuf		×Ö·û´®
 *	Return:			Ð¡Ð´×Ö·û´®
 */
std::string  NStr::StrToLwr(std::string  strBuf)
{
	if (strBuf.empty())
	{
		return "";
	}

	return _strlwr((char *)strBuf.c_str());
}

std::wstring NStr::StrToLwr(std::wstring strBuf)
{
	if (strBuf.empty())
	{
		return L"";
	}

	return _wcslwr((wchar_t *)strBuf.c_str());
}

/*
*	Function:		StrToLwr
*	Explanation:	×Ö·û´®×ª´óÐ´
*	Input:			strBuf		×Ö·û´®
*	Return:			´óÐ´×Ö·û´®
*/
std::string  NStr::StrToUpr(std::string  strBuf)
{
	if (strBuf.empty())
	{
		return "";
	}

	return _strupr((char *)strBuf.c_str());
}

std::wstring NStr::StrToUpr(std::wstring strBuf)
{
	if (strBuf.empty())
	{
		return L"";
	}

	return _wcsupr((wchar_t *)strBuf.c_str());
}

#define Ansi2Type_Number	0	// Êý×Ö
#define Ansi2Type_AlphUpr	1	// ´óÐ´×ÖÄ¸
#define Ansi2Type_AlphLwr	2	// Ð¡Ð´×ÖÄ¸
#define Ansi2Type_Operate	3	// ²Ù×÷·û

static byte s_Ansi_Table[256] =
{
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x03, 0xFF, 0x03, 0x03, 0x03, 0x03, 0xFF, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0x03, 0x03, 0x03,
	//        !     "     #     $     %     &     '      (     )     *     +     ,     -     .     /

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x03, 0x03, 0x03, 0x03,
	//  0     1     2     3     4     5     6     7      8     9     :     ;     <     =     >     ?

	0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	//  @     A     B     C     D     E     F     G      H     I     J     K     L     M     N     O

	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03,
	//  P     Q     R     S     T     U     V     W      X     Y     Z     [     \     ]     ^     _

	0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	//  `     a     b     c     d     e     f     g      h     i     j     k     l     m     n     o

	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0xFF,
	//  p     q     r     s     t     u     v     w      x     y     z     {     |     }     ~

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
 *	Function:		IsNumber
 *	Explanation:	ÅÐ¶Ï×Ö·ûÊÇ·ñÊÇÊý×Ö
 *	Input:			c		×Ö·û
 *	Return:			true ÊÇÊý×Ö false ²»ÊÇÊý×Ö
 */
bool NStr::IsNumber(char c)
{
	if (Ansi2Type_Number == s_Ansi_Table[(byte)c])
	{
		return true;
	}

	return false;
}

/*
*	Function:		IsAlphUpr
*	Explanation:	ÅÐ¶Ï×Ö·ûÊÇ·ñÊÇ´óÐ´×ÖÄ¸
*	Input:			c		×Ö·û
*	Return:			true ÊÇ false ²»ÊÇ
*/
bool NStr::IsAlphUpr(char c)
{
	if (Ansi2Type_AlphUpr == s_Ansi_Table[(byte)c])
	{
		return true;
	}

	return false;
}

/*
*	Function:		IsAplhLwr
*	Explanation:	ÅÐ¶Ï×Ö·ûÊÇ·ñÊÇÐ¡Ð´×ÖÄ¸
*	Input:			c		×Ö·û
*	Return:			true ÊÇ false ²»ÊÇ
*/
bool NStr::IsAplhLwr(char c)
{
	if (Ansi2Type_AlphLwr == s_Ansi_Table[(byte)c])
	{
		return true;
	}

	return false;
}

/*
*	Function:		IsAplh
*	Explanation:	ÅÐ¶Ï×Ö·ûÊÇ·ñÊÇ×ÖÄ¸
*	Input:			c		×Ö·û
*	Return:			true ÊÇ false ²»ÊÇ
*/
bool NStr::IsAplh(char c)
{
	if (Ansi2Type_AlphUpr == s_Ansi_Table[(byte)c]
		|| Ansi2Type_AlphLwr == s_Ansi_Table[(byte)c])
	{
		return true;
	}

	return false;
}

/*
*	Function:		IsOperate
*	Explanation:	ÅÐ¶Ï×Ö·ûÊÇ·ñÊÇ²Ù×÷·û
*	Input:			c		×Ö·û
*	Return:			true ÊÇ false ²»ÊÇ
*/
bool NStr::IsOperate(char c)
{
	if (Ansi2Type_Operate == s_Ansi_Table[(byte)c])
	{
		return true;
	}

	return false;
}

/**
 * º¯Êý¹¦ÄÜËµÃ÷£ºÅÐ¶Ï×Ö·û´®ÊÇ·ñ°üº¬ÖÐÎÄ
 * @param [in]  strBuf			×Ö·û´®
 * @return TRUE °üº¬ FALSE ²»°üº¬
 * by Maga 2016-9-22
 */
bool NStr::HasChinese(std::string  strBuf)
{
	for (int i = 0; i < strBuf.size(); i++)
	{
		char		c1;
		char		c2;

		c1 = strBuf[i];

		if ('\0' == c1)
		{
			break;
		}

		c2 = strBuf[i + 1];

		if ('\0' == c2)
		{
			break;
		}

		if ( (c1 & 0x80) && (c2 & 0x80) )
		{
			return true;
		}
	}

	return false;
}

/**
 * º¯Êý¹¦ÄÜËµÃ÷£º»ñÈ¡GUID
 * @return GUID
 * by Maga 2016-7-22
 */
std::string  NStr::GetGUIDStr()
{
	static char				buf[64]					= { 0 };
	GUID					guid;

	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf)
			, "%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
			);
	}

	return (char*)buf;
}

static const char s_Base64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
*	Function:		_GetBase64TableIndex
*	Explanation:	»ñÈ¡¶ÔÓ¦×Ö·ûÔÚs_Base64TableÖÐµÄË÷Òý
*	Input:			c		×Ö·û
*	Return:			Ë÷Òý£¬ Ê§°Ü·µ»Ø-1
*/
inline int _GetBase64TableIndex(char c)
{
	for (int i = 0; s_Base64Table[i]; i++)
	{
		if (c == s_Base64Table[i])
		{
			return i;
		}
	}

	return -1;
}

/*
 *	Function:		Base64Encode
 *	Explanation:	Base64¼ÓÃÜ
 *	Input:			strSrc		ÐèÒª¼ÓÃÜµÄ×Ö·û´®
 *	Return:			¼ÓÃÜºóµÄ×Ö·û´®£¬Ê§°Ü·µ»Ø""
 */
std::string  NStr::Base64Encode(std::string  strSrc)
{
	size_t				stSrcSize				= strSrc.size();
	unsigned char		cIn[3]					= { 0 };
	unsigned char		cOut;
	std::string 			strDes;

	for (size_t stIndex = 0; stIndex < stSrcSize;)
	{
		// 0
		cIn[0] = (unsigned char)strSrc[stIndex++];
		cOut = (cIn[0] >> 2) & 0x3F;
		strDes.append(1, s_Base64Table[cOut]);

		// 1
		if (stIndex < stSrcSize)
		{
			cIn[1] = (unsigned char)strSrc[stIndex++];
			cOut = ((cIn[0] << 4) & 0x30) + ((cIn[1] >> 4) & 0xF);
			strDes.append(1, s_Base64Table[cOut]);
		}
		else
		{
			cOut = (cIn[0] << 4) & 0x30;
			strDes.append(1, s_Base64Table[cOut]);
			strDes.append("==");
			break;
		}

		// 2
		if (stIndex < stSrcSize)
		{
			cIn[2] = (unsigned char)strSrc[stIndex++];
			cOut = ((cIn[1] << 2) & 0x3C) + ((cIn[2] >> 6) & 0x3);
			strDes.append(1, s_Base64Table[cOut]);
		}
		else
		{
			cOut = (cIn[1] << 2) & 0x3C;
			strDes.append(1, s_Base64Table[cOut]);
			strDes.append("=");
			break;
		}

		// 3
		cOut = cIn[2] & 0x3F;
		strDes.append(1, s_Base64Table[cOut]);
	}

	return strDes;
}

/*
 *	Function:		Base64Decode
 *	Explanation:	Base64½âÃÜ
 *	Input:			strSrc		ÐèÒª½âÃÜµÄ×Ö·û´®
 *	Return:			½âÃÜºóµÄ×Ö·û´®
 */
std::string  NStr::Base64Decode(std::string  strSrc)
{
	size_t				stTempSize				= strSrc.size();
	std::string 			strDes;
	std::string 			strTemp;
	unsigned char		cIn[4];
	unsigned char		cOut;

	// È¥³ý=
	strTemp = strSrc.substr(0, strSrc.find_first_of("=", strSrc.size() - 2));
	stTempSize = strTemp.size();

	for (size_t stIndex = 0; stIndex < stTempSize;)
	{
		// 0
		cIn[0] = _GetBase64TableIndex(strTemp[stIndex++]);

		// 1
		if (stIndex < stTempSize)
		{
			cIn[1] = _GetBase64TableIndex(strTemp[stIndex++]);
			cOut = (unsigned char)((cIn[0] << 2) & 0xFC) + (unsigned char)((cIn[1] >> 4) & 0x3);
			strDes.append(1, cOut);
		}
		else
		{
			break;
		}

		// 2
		if (stIndex < stTempSize)
		{
			cIn[2] = _GetBase64TableIndex(strTemp[stIndex++]);
			cOut = (unsigned char)((cIn[1] << 4) & 0xF0) + (unsigned char)((cIn[2] >> 2) & 0x0F);
			strDes.append(1, cOut);
		}
		else
		{
			break;
		}

		// 3
		if (stIndex < stTempSize)
		{
			cIn[3] = _GetBase64TableIndex(strTemp[stIndex++]);
			cOut = (unsigned char)((cIn[2] << 6) & 0xC0) + (unsigned char)(cIn[3] & 0x3F);
			strDes.append(1, cOut);
		}
		else
		{
			break;
		}
	}

	return strDes;
}


inline unsigned char _ToHex(unsigned char c)
{
	return c > 9 ? c + 55 : c + 48;
}

/*
 *	Function:		UrlEncode
 *	Explanation:	UrlEncode¼ÓÃÜ
 *	Input:			strBuf		ÐèÒª¼ÓÃÜµÄ×Ö·û´®
 *	Return:			¼ÓÃÜºó×Ö·û´®
 */
std::string  NStr::UrlEncode(std::string  strBuf)
{
	std::string 			strReturn;

	for (size_t stIndex = 0; stIndex < strBuf.size(); stIndex++)
	{
		if (isalnum((unsigned char)strBuf[stIndex])
			|| '-' == strBuf[stIndex]
			|| '_' == strBuf[stIndex]
			|| '.' == strBuf[stIndex]
			|| '~' == strBuf[stIndex])
		{
			strReturn += strBuf[stIndex];
		}
		else if (isspace((unsigned char)strBuf[stIndex]))
		{
			strReturn += "+";
		}
		else
		{
			strReturn += '%';
			strReturn += _ToHex((unsigned char)strBuf[stIndex] >> 4);
			strReturn += _ToHex((unsigned char)strBuf[stIndex] % 16);
		}
	}

	return strReturn;
}

inline unsigned char _FromHex(unsigned char c)
{
	unsigned char			cRet;

	if (c >= 'A' && c <= 'Z')
	{
		cRet = c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'z')
	{
		cRet = c - 'a' + 10;
	}
	else if (c >= '0' && c <= '9')
	{
		cRet = c - '0';
	}
	else
	{
		assert(0);
	}

	return cRet;
}

/*
*	Function:		UrlDecode
*	Explanation:	UrlDecode½âÃÜ
*	Input:			strBuf		ÐèÒª½âÃÜµÄ×Ö·û´®
*	Return:			½âÃÜºó×Ö·û´®
*/
std::string  NStr::UrlDecode(std::string  strBuf)
{
	std::string 			strReturn;

	for (size_t stIndex = 0; stIndex < strBuf.size(); stIndex++)
	{
		if ('+' == strBuf[stIndex])
		{
			strReturn += ' ';
		}
		else if ('%' == strBuf[stIndex])
		{
			unsigned char		cHigh;
			unsigned char		cLow;

			assert(stIndex + 2 < strBuf.size());
			cHigh = _FromHex((unsigned char)strBuf[++stIndex]);
			cLow = _FromHex((unsigned char)strBuf[++stIndex]);
			strReturn += cHigh * 16 + cLow;
		}
		else
		{
			strReturn += strBuf[stIndex];
		}
	}

	return strReturn;
}










#ifdef GS_USE_CRYPT

/*
 *	Function:		GetSha1
 *	Explanation:	Sha1¼ÓÃÜ
 *	Input:			strBuf		ÐèÒª¼ÓÃÜ×Ö·û´®
 *	Return:			¼ÓÃÜºó×Ö·û´®
 */
std::string  NStr::GetSha1(std::string  strBuf)
{
	SHA_CTX				Ctx;
	unsigned char		szMemory[21]				= { 0 };
	char				szSha1[41]					= { 0 };

	SHA1_Init(&Ctx);
	SHA1_Update(&Ctx, (void *)strBuf.c_str(), strBuf.size());
	SHA1_Final(szMemory, &Ctx);
	GSNUtil::DataToHex(szMemory, 20, szSha1, 41);

	return szSha1;
}

/*
 *	Function:		GetMD5
 *	Explanation:	MD5¼ÓÃÜ
 *	Input:			strBuf		ÐèÒª¼ÓÃÜ×Ö·û´®
 *	Return:			¼ÓÃÜºó×Ö·û´®
 */
std::string  NStr::GetMD5(std::string  strBuf)
{
	MD5_CTX				Ctx;
	unsigned char		szMemory[17]				= { 0 };
	char				szMD5[33]					= { 0 };

	MD5_Init(&Ctx);
	MD5_Update(&Ctx, (void *)strBuf.c_str(), strBuf.size());
	MD5_Final(szMemory, &Ctx);
	GSNUtil::DataToHex(szMemory, 16, szMD5, 33);

	return szMD5;
}

/*
 *	Function:		Aes128Encode
 *	Explanation:	Aes¼ÓÃÜ(·µ»ØµÄÊÇÄÚ´æÊý¾Ý)
 *	Input:			strPwd		ÃÜÂë
					strBuf		¼ÓÃÜÊý¾Ý
 *	Return:			¼ÓÃÜºóÊý¾Ý
 */
std::string  NStr::Aes128Encode(std::string  strPwd, std::string  strBuf)
{
	AES_KEY				AesKey;
	unsigned char		szOut[AES_BLOCK_SIZE]				= { 0 };
	int					nIndex								= 0;
	std::string 			strEncode;
	std::string 			strSub;

	// 128-bit
	if (0 > AES_set_encrypt_key((const unsigned char *)strPwd.c_str(), AES_BLOCK_SIZE * 8, &AesKey))
	{
		return "";
	}

	for (; nIndex < strBuf.size() / AES_BLOCK_SIZE; nIndex++)
	{
		memset(szOut, 0, sizeof(szOut));
		strSub = strBuf.substr(nIndex * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
		AES_encrypt((const unsigned char *)strSub.c_str(), szOut, &AesKey);
		strEncode += std::string ((char *)szOut, AES_BLOCK_SIZE);
	}

	if (0 < (strBuf.size() - nIndex * AES_BLOCK_SIZE))
	{
		memset(szOut, 0, sizeof(szOut));
		strSub = strBuf.substr(nIndex * AES_BLOCK_SIZE, strBuf.size() - nIndex * AES_BLOCK_SIZE);
		AES_encrypt((const unsigned char *)strSub.c_str(), szOut, &AesKey);
		strEncode += std::string ((char *)szOut, AES_BLOCK_SIZE);
	}

	return strEncode;
}

/*
 *	Function:		Aes128Decode
 *	Explanation:	Aes½âÃÜ
 *	Input:			strPwd		ÃÜÂë
					strBuf		½âÃÜÊý¾Ý
 *	Return:			½âÃÜºóÊý¾Ý
 */
std::string  NStr::Aes128Decode(std::string  strPwd, std::string  strBuf)
{
	AES_KEY				AesKey;
	unsigned char		szOut[AES_BLOCK_SIZE]				= { 0 };
	int					nIndex								= 0;
	std::string 			strDecode;
	std::string 			strSub;

	if (0 > AES_set_decrypt_key((const unsigned char *)strPwd.c_str(), AES_BLOCK_SIZE * 8, &AesKey))
	{
		return "";
	}

	for (; nIndex < strBuf.size() / AES_BLOCK_SIZE; nIndex++)
	{
		memset(szOut, 0, sizeof(szOut));
		strSub = strBuf.substr(nIndex * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
		AES_decrypt((const unsigned char *)strSub.c_str(), szOut, &AesKey);
		strDecode += std::string ((char *)szOut, AES_BLOCK_SIZE);
	}

	if (0 < (strBuf.size() - nIndex * AES_BLOCK_SIZE))
	{
		memset(szOut, 0, sizeof(szOut));
		strSub = strBuf.substr(nIndex * AES_BLOCK_SIZE, strBuf.size() - nIndex * AES_BLOCK_SIZE);
		AES_decrypt((const unsigned char *)strSub.c_str(), szOut, &AesKey);
		strDecode += std::string ((char *)szOut, AES_BLOCK_SIZE);
	}

	return strDecode;
}

/*
 *	Function:		RsaPubEncode
 *	Explanation:	Rsa¹«Ô¿¼ÓÃÜ(·µ»ØµÄÊÇÄÚ´æÊý¾Ý)
 *	Input:			strKey		ÃÜÔ¿
					strBuf		ÐèÒª¼ÓÃÜµÄÊý¾Ý
 *	Return:			¼ÓÃÜºóµÄÊý¾Ý
 */
std::string  NStr::RsaPubEncode(std::string  strKey, std::string  strBuf)
{
	BIO*				pBio;
	RSA*				pRsa;
	int					nBlock;
	int					nBlockReal;
	char*				pData					= NULL;
	int					nDataSize;
	int					nCount;
	int					nLen					= 0;
	std::string 			strReturn;

	pBio = BIO_new_mem_buf((void *)strKey.c_str(), -1);

	if (NULL == pBio)
	{
		return "";
	}

	pRsa = PEM_read_bio_RSA_PUBKEY(pBio, NULL, NULL, NULL);

	if (NULL == pRsa)
	{
		BIO_free(pBio);
		return "";
	}

	nBlock = RSA_size(pRsa);
	nBlockReal = nBlock - 11;
	nCount = (strBuf.size() + nBlockReal - 1) / nBlockReal;
	nDataSize = nCount * nBlock;
	pData = (char *)malloc(nDataSize);

	if (NULL == pData)
	{
		RSA_free(pRsa);
		BIO_free(pBio);
		return "";
	}

	memset(pData, 0, nDataSize);

	for (int i = 0; i < nCount; i++)
	{
		int			nRet			= 0;
		int			nSize			= min(strBuf.size() - i * nBlockReal, nBlockReal);

		nRet = RSA_public_encrypt(nSize,
								(const unsigned char *)strBuf.c_str() + i * nBlockReal,
								(unsigned char *)pData + nLen,
								pRsa, RSA_PKCS1_PADDING);

		if (0 > nRet)
		{
			nLen = -1;
			break;
		}

		nLen += nRet;
	}

	if (-1 != nLen)
	{
		strReturn = std::string (pData, nLen);
	}

	free(pData);
	RSA_free(pRsa);
	BIO_free(pBio);

	return strReturn;
}

/*
 *	Function:		RsaPubDecode
 *	Explanation:	Rsa¹«Ô¿½âÃÜ
 *	Input:			strKey		ÃÜÔ¿
					strBuf		ÐèÒª½âÃÜÊý¾Ý
 *	Return:			½âÃÜÊý¾Ý
 */
std::string  NStr::RsaPubDecode(std::string  strKey, std::string  strBuf)
{
	BIO*				pBio;
	RSA*				pRsa;
	int					nBlock;
	int					nBlockReal;
	char*				pData					= NULL;
	int					nDataSize;
	int					nCount;
	int					nLen					= 0;
	std::string 			strReturn;

	pBio = BIO_new_mem_buf((void *)strKey.c_str(), -1);

	if (NULL == pBio)
	{
		return "";
	}

	pRsa = PEM_read_bio_RSA_PUBKEY(pBio, NULL, NULL, NULL);

	if (NULL == pRsa)
	{
		BIO_free(pBio);
		return "";
	}

	nBlock = RSA_size(pRsa);
	nBlockReal = nBlock - 11;

	if (0 != (strBuf.size() % nBlock))
	{
		RSA_free(pRsa);
		BIO_free(pBio);
		return "";
	}

	nCount = strBuf.size() / nBlock;
	nDataSize = (strBuf.size() + nBlock - 1) / nBlock * nBlockReal + 1;
	pData = (char *)malloc(nDataSize);

	if (NULL == pData)
	{
		RSA_free(pRsa);
		BIO_free(pBio);
		return "";
	}

	memset(pData, 0, nDataSize);

	for (int i = 0; i < nCount; i++)
	{
		int			nRet			= 0;

		nRet = RSA_public_decrypt(nBlock,
									(unsigned char *)strBuf.c_str() + i * nBlock,
									(unsigned char *)pData + nLen,
									pRsa,
									RSA_PKCS1_PADDING);

		if (0 > nRet)
		{
			nLen = -1;
			break;
		}

		nLen += nRet;
	}

	if (-1 != nLen)
	{
		strReturn = std::string (pData, nLen);
	}

	free(pData);
	RSA_free(pRsa);
	BIO_free(pBio);

	return strReturn;
}

/*
*	Function:		RsaPriEncode
*	Explanation:	RsaË½Ô¿¼ÓÃÜ(·µ»ØµÄÊÇÄÚ´æÊý¾Ý)
*	Input:			strKey		ÃÜÔ¿
					strBuf		ÐèÒª¼ÓÃÜµÄÊý¾Ý
*	Return:			¼ÓÃÜºóµÄÊý¾Ý
*/
std::string  NStr::RsaPriEncode(std::string  strKey, std::string  strBuf)
{
	BIO*				pBio;
	RSA*				pRsa;
	int					nBlock;
	int					nBlockReal;
	char*				pData				= NULL;
	int					nDataSize;
	int					nCount;
	int					nLen				= 0;
	std::string 			strReturn;

	pBio = BIO_new_mem_buf((void *)strKey.c_str(), -1);

	if (NULL == pBio)
	{
		return "";
	}

	pRsa = PEM_read_bio_RSAPrivateKey(pBio, NULL, NULL, NULL);

	if (NULL == pRsa)
	{
		BIO_free(pBio);
		return "";
	}

	nBlock = RSA_size(pRsa);
	nBlockReal = nBlock - 11;
	nCount = (strBuf.size() + nBlockReal - 1) / nBlockReal;
	nDataSize = nCount * nBlock;
	pData = (char *)malloc(nDataSize);

	if (NULL == pData)
	{
		RSA_free(pRsa);
		BIO_free(pBio);
		return "";
	}

	memset(pData, 0, nDataSize);

	for (int i = 0; i < nCount; i++)
	{
		int			nRet = 0;
		int			nSize = min(strBuf.size() - i * nBlockReal, nBlockReal);

		nRet = RSA_private_encrypt(nSize,
									(const unsigned char *)strBuf.c_str() + i * nBlockReal,
									(unsigned char *)pData + nLen,
									pRsa, RSA_PKCS1_PADDING);

		if (0 > nRet)
		{
			nLen = -1;
			break;
		}

		nLen += nRet;
	}

	if (-1 != nLen)
	{
		strReturn = std::string (pData, nLen);
	}

	free(pData);
	RSA_free(pRsa);
	BIO_free(pBio);

	return strReturn;
}

/*
*	Function:		RsaPriDecode
*	Explanation:	RsaË½Ô¿½âÃÜ
*	Input:			strKey		ÃÜÔ¿
					strBuf		ÐèÒª½âÃÜÊý¾Ý
*	Return:			½âÃÜÊý¾Ý
*/
std::string  NStr::RsaPriDecode(std::string  strKey, std::string  strBuf)
{
	BIO*				pBio;
	RSA*				pRsa;
	int					nBlock;
	int					nBlockReal;
	char*				pData					= NULL;
	int					nDataSize;
	int					nCount;
	int					nLen					= 0;
	std::string 			strReturn;

	pBio = BIO_new_mem_buf((void *)strKey.c_str(), -1);

	if (NULL == pBio)
	{
		return "";
	}

	pRsa = PEM_read_bio_RSAPrivateKey(pBio, NULL, NULL, NULL);

	if (NULL == pRsa)
	{
		BIO_free(pBio);
		return "";
	}

	nBlock = RSA_size(pRsa);
	nBlockReal = nBlock - 11;

	if (0 != (strBuf.size() % nBlock))
	{
		RSA_free(pRsa);
		BIO_free(pBio);
		return "";
	}

	nCount = strBuf.size() / nBlock;
	nDataSize = (strBuf.size() + nBlock - 1) / nBlock * nBlockReal + 1;
	pData = (char *)malloc(nDataSize);

	if (NULL == pData)
	{
		RSA_free(pRsa);
		BIO_free(pBio);
		return "";
	}

	memset(pData, 0, nDataSize);

	for (int i = 0; i < nCount; i++)
	{
		int			nRet = 0;

		nRet = RSA_private_decrypt(nBlock,
			(unsigned char *)strBuf.c_str() + i * nBlock,
			(unsigned char *)pData + nLen,
			pRsa,
			RSA_PKCS1_PADDING);

		if (0 > nRet)
		{
			nLen = -1;
			break;
		}

		nLen += nRet;
	}

	if (-1 != nLen)
	{
		strReturn = std::string (pData, nLen);
	}

	free(pData);
	RSA_free(pRsa);
	BIO_free(pBio);

	return strReturn;
}

/*
 *	Function:		GetRsaKey
 *	Explanation:	»ñÈ¡Rsa¹«Ô¿¡¢Ë½Ô¿¶Ô
 *	Input:			ÊäÈë
 *  Output:			strPubKey		¹«Ô¿
					strPriKey		Ë½Ô¿
 *	Return:			true ³É¹¦ false Ê§°Ü
 */
bool NStr::GetRsaKey(std::string  &strPubKey, std::string  &strPriKey)
{
	RSA*				pRsa;
	int					nLen;
	char				szPubKey[1024]				= { 0 };
	char				szPriKey[1024]				= { 0 };

	pRsa = RSA_new();

	if (NULL == pRsa)
	{
		return false;
	}

	pRsa = RSA_generate_key(1024, 0x10001, NULL, NULL);
	nLen = i2d_RSAPublicKey(pRsa, (unsigned char **)&szPubKey);

	if (0 > nLen)
	{
		RSA_free(pRsa);
		return false;
	}

	strPubKey = std::string (szPubKey, nLen);
	nLen = i2d_RSAPrivateKey(pRsa, (unsigned char **)&szPriKey);

	if (0 > nLen)
	{
		RSA_free(pRsa);
		return false;
	}

	strPriKey = std::string (szPriKey, nLen);
	RSA_free(pRsa);

	return true;
}

/*
 *	Function:		DesEncode
 *	Explanation:	Des¼ÓÃÜ PKCS7
 *	Input:			strKey		ÃÜÔ¿
					strBuf		¼ÓÃÜÊý¾Ý
					bEcb		ÊÇ·ñÊÇECBÄ£Ê½
 *	Return:			¼ÓÃÜºóÊý¾Ý
 */
std::string  NStr::DesEncode(std::string  strKey, std::string  strBuf, bool bEcb /* = true */)
{
	int					nPkLen				= 0;
	DES_key_schedule	DesKs;
	std::string 			strReturn;
	unsigned char		szOut[9]			= { 0 };
	DES_cblock			DesCB;

	// ÃÜÔ¿Ìî³ä/½Ø¶Ï
	if (8 < strKey.size())
	{
		strKey = strKey.substr(0, 8);
	}
	else
	{
		strKey.append(8 - strKey.size(), 0);
	}

	// Êý¾ÝÌî³ä
	nPkLen = strBuf.size() % 8;
	strBuf.append(8 - nPkLen, 8 - nPkLen);
	

	DES_set_key_unchecked((const_DES_cblock *)strKey.c_str(), &DesKs);

	for (int i = 0; i < (strBuf.size() / 8); i++)
	{
		memset(szOut, 0, sizeof(szOut));

		if (bEcb)
		{
			DES_ecb_encrypt((DES_cblock *)(strBuf.c_str() + i * 8), (DES_cblock *)&szOut, &DesKs, DES_ENCRYPT);
		}
		else
		{
			DES_ncbc_encrypt((const unsigned char *)(strBuf.c_str() + i * 8), szOut, 8, &DesKs, &DesCB, DES_ENCRYPT);
		}

		strReturn += std::string ((char *)szOut, 8);
	}

	return strReturn;
}

/*
*	Function:		DesDecode
*	Explanation:	Des½âÃÜ PKCS7
*	Input:			strKey		ÃÜÔ¿
					strBuf		½âÃÜÊý¾Ý
					bEcb		ÊÇ·ñÊÇECBÄ£Ê½
*	Return:			½âÃÜºóÊý¾Ý
*/
std::string  NStr::DesDecode(std::string  strKey, std::string  strBuf, bool bEcb /* = true */)
{
	int					nPkLen					= 0;
	DES_key_schedule	DesKs;
	std::string 			strReturn;
	unsigned char		szOut[9]				= { 0 };
	DES_cblock			DesCB;
	int					nDel;

	// ÃÜÔ¿Ìî³ä/½Ø¶Ï
	if (8 < strKey.size())
	{
		strKey = strKey.substr(0, 8);
	}
	else
	{
		strKey.append(8 - strKey.size(), 0);
	}

	if (0 != (strBuf.size() % 8))
	{
		return "";
	}

	DES_set_key_unchecked((const_DES_cblock *)strKey.c_str(), &DesKs);

	for (int i = 0; i < (strBuf.size() / 8); i++)
	{
		memset(szOut, 0, sizeof(szOut));

		if (bEcb)
		{
			DES_ecb_encrypt((DES_cblock *)(strBuf.c_str() + i * 8), (DES_cblock *)&szOut, &DesKs, DES_DECRYPT);
		}
		else
		{
			DES_ncbc_encrypt((const unsigned char *)(strBuf.c_str() + i * 8), szOut, 8, &DesKs, &DesCB, DES_DECRYPT);
		}

		strReturn += std::string ((char *)szOut, 8);
	}

	// È¥³ýÌî³ä
	nDel = strReturn[strReturn.size() - 1];
	strReturn = strReturn.substr(0, strReturn.size() - nDel);

	return strReturn;
}

/*
 *	Function:		Des3Encode
 *	Explanation:	3Des¼ÓÃÜ PKCS7 ECB
 *	Input:			strKey		ÃÜÔ¿
					strBuf		¼ÓÃÜÊý¾Ý
 *	Return:			¼ÓÃÜºóÊý¾Ý
 */
std::string  NStr::Des3Encode(std::string  strKey, std::string  strBuf)
{
	int					nPkLen				= 0;
	DES_key_schedule	DesKs;
	DES_key_schedule	DesKs2;
	DES_key_schedule	DesKs3;
	std::string 			strReturn;
	unsigned char		szOut[9]			= { 0 };
	DES_cblock			BlockKey;

	// ÃÜÔ¿Ìî³ä/½Ø¶Ï
	if (24 < strKey.size())
	{
		strKey = strKey.substr(0, 24);
	}
	else
	{
		strKey.append(24 - strKey.size(), 0);
	}

	// Êý¾ÝÌî³ä
	nPkLen = strBuf.size() % 8;
	strBuf.append(8 - nPkLen, 8 - nPkLen);

	memcpy(&BlockKey, (void *)(strKey.c_str()), 8);
	DES_set_key_unchecked((const_DES_cblock *)BlockKey, &DesKs);
	memcpy(&BlockKey, (void *)(strKey.c_str() + 8), 8);
	DES_set_key_unchecked((const_DES_cblock *)BlockKey, &DesKs2);
	memcpy(&BlockKey, (void *)(strKey.c_str() + 16), 8);
	DES_set_key_unchecked((const_DES_cblock *)BlockKey, &DesKs3);

	for (int i = 0; i < (strBuf.size() / 8); i++)
	{
		memset(szOut, 0, sizeof(szOut));
		DES_ecb3_encrypt((DES_cblock *)(strBuf.c_str() + i * 8), (DES_cblock *)&szOut, &DesKs, &DesKs2, &DesKs3, DES_ENCRYPT);
		strReturn += std::string ((char *)szOut, 8);
	}

	return strReturn;
}

/*
*	Function:		Des3Decode
*	Explanation:	3Des½âÃÜ PKCS7 ECB
*	Input:			strKey		ÃÜÔ¿
					strBuf		½âÃÜÊý¾Ý
*	Return:			½âÃÜºóÊý¾Ý
*/
std::string  NStr::Des3Decode(std::string  strKey, std::string  strBuf)
{
	int					nPkLen				= 0;
	DES_key_schedule	DesKs;
	DES_key_schedule	DesKs2;
	DES_key_schedule	DesKs3;
	std::string 			strReturn;
	unsigned char		szOut[9]			= { 0 };
	DES_cblock			BlockKey;
	int					nDel;

	// ÃÜÔ¿Ìî³ä/½Ø¶Ï
	if (24 < strKey.size())
	{
		strKey = strKey.substr(0, 24);
	}
	else
	{
		strKey.append(24 - strKey.size(), 0);
	}

	if (0 != (strBuf.size() % 8))
	{
		return "";
	}

	memcpy(&BlockKey, (void *)(strKey.c_str()), 8);
	DES_set_key_unchecked((const_DES_cblock *)BlockKey, &DesKs);
	memcpy(&BlockKey, (void *)(strKey.c_str() + 8), 8);
	DES_set_key_unchecked((const_DES_cblock *)BlockKey, &DesKs2);
	memcpy(&BlockKey, (void *)(strKey.c_str() + 16), 8);
	DES_set_key_unchecked((const_DES_cblock *)BlockKey, &DesKs3);

	for (int i = 0; i < (strBuf.size() / 8); i++)
	{
		memset(szOut, 0, sizeof(szOut));
		DES_ecb3_encrypt((DES_cblock *)(strBuf.c_str() + i * 8), (DES_cblock *)&szOut, &DesKs, &DesKs2, &DesKs3, DES_DECRYPT);
		strReturn += std::string ((char *)szOut, 8);
	}

	// È¥³ýÌî³ä
	nDel = strReturn[strReturn.size() - 1];
	strReturn = strReturn.substr(0, strReturn.size() - nDel);

	return strReturn;
}

BOOL NStr::CheckUserPass(const char * szcontent)
{
	BOOL result = false;
	BOOL bfindsz = FALSE;	//ÊÇ·ñ°üº¬×Ö·û
	BOOL bfindnum = FALSE;	//ÊÇ·ñ°üº¬Êý×Ö
	BOOL bfinfch = FALSE;	//ÊÇ·ñ°üº¬ºº×Ö

	if ((strlen(szcontent) > 16) || strlen(szcontent) < 6)
		return result;//³¤¶È²»ÄÜ³¬¹ý16¸ö×Ö·û

	while (*szcontent)
	{
		if ((*szcontent) & 0x80){
			//ÊÇºº×Ö
			bfinfch = TRUE;
			szcontent++;//ÖªµÀÊÇºº×ÖµÄ»°Ìø¹ýÒ»¸ö×Ö½Ú¼ì²â
			break;
		}
		else if ((*szcontent >= 'a' && *szcontent <= 'z' || *szcontent >= 'A' && *szcontent <= 'Z'))
		{
			bfindsz = true;
		}
		else if (((*szcontent) >= '0' && (*szcontent) <= '9'))
		{
			bfindnum = true;
		}
		else{
			return false;
		}
		szcontent++;
	}

	if (bfinfch)
		return result;

	if (bfindsz && bfindnum)			// ÐèÒªÍ¬Ê±´æÔÚÊý×ÖºÍ×ÖÄ¸
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * º¯Êý¹¦ÄÜËµÃ÷£º¼ì²âÊÇ·ñ°üº¬ºº×Ó
 * @param [in]  strData		×Ö·û´®
 * @return TRUE °üº¬ FALSE ²»°üº¬
 * by Maga 2016-12-2
 */
BOOL NStr::CheckHasChina(std::string  strData)
{
	for (int i = 0; i < strData.size(); i++)
	{
		if (0x80 & strData[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ÎÄ¼þ´óÐ¡×ª»»Îª×Ö·û´®
std::string 	NStr::FileSizeToStr(__int64 ulSize)
{
	if (ulSize > 1024 * 1024 * 1024)
	{
		int nGb = (ulSize / (1024 * 1024 * 1024));
		int nMb = ((ulSize % (1024 * 1024 * 1024)) / (1000 * 1000 * 100));
		return NStr::FormatStr("%d.%01dG", nGb, nMb >= 9 ? 9 : nMb);
	}
	else if (ulSize > 1024 * 1024)
	{
		int nMb = ulSize / (1024 * 1024);
		int nKb = (ulSize % (1024 * 1024)) / (1000 * 100);
		return NStr::FormatStr("%d.%01dM", nMb, nKb >= 9 ? 9 : nKb);
	}
	else
	{
		int nKb = ulSize / 1024;
		int nb = ulSize % 1024 / 100;
		return NStr::FormatStr("%d.%01dKB", nKb, nb >= 9 ? 9 : nb);
	}
}

// ÎÄ¼þ´óÐ¡×ª»»Îª×Ö·û´®
std::wstring	NStr::FileSizeToWStr(__int64 ulSize)
{
	if (ulSize > 1024 * 1024 * 1024)
	{
		int nGb = (ulSize / (1024 * 1024 * 1024));
		int nMb = ((ulSize % (1024 * 1024 * 1024)) / (1000 * 1000 * 100));
		return NStr::FormatWStr(L"%d.%01dG", nGb, nMb >= 9 ? 9 : nMb);
	}
	else if(ulSize > 1024 * 1024)
	{
		int nMb = ulSize / (1024 * 1024);
		int nKb = (ulSize % (1024 * 1024)) / (1000 * 100);
		return NStr::FormatWStr(L"%d.%01dM", nMb, nKb >= 9 ? 9 : nKb);
	}
	else
	{
		int nKb = ulSize / 1024;
		int nb = ulSize % 1024 / 100;
		return NStr::FormatWStr(L"%d.%01dKB", nKb, nb >= 9 ? 9 : nb);
	}
}

// Êý×Ö×ª×Ö·û£¨ÓÃÒÔ´óÐ´×ª»»1£©
std::string  NStr::IntToStrForCapital(int nValue)
{
	if (nValue > 999)
	{
		nValue = 999;
	}
	const char* sNum[] = { "Áã", "Ò»", "¶þ", "Èý", "ËÄ", "Îå", "Áù", "Æß", "°Ë", "¾Å" };
	const char* sBit[] = { "", "Ê®", "°Ù", "Ç§" };
	const char* sTep[] = { "", "Íò", "ÒÚ", "Íò", "ÒÚ", "Íò", "ÒÚ" };

	int nHun = nValue / 10 / 10;
	int nTen = nValue / 10 % 10;
	int nUnit = nValue % 10;

	std::string  strOut = "";
	if (nHun != 0)
	{
		strOut = sNum[nHun];
		strOut += sBit[2];
	}

	if (nTen == 0)
	{
		strOut += nHun == 0 ? "" : sNum[0];
		strOut += sNum[nUnit];
	}
	else if (nTen == 1)
	{
		strOut += nHun == 0 ? "" : sNum[nTen];
		strOut += sBit[1];
		strOut += nUnit == 0 ? "" : sNum[nUnit];
	}
	else
	{
		strOut += sNum[nTen];
		strOut += sBit[1];
		strOut += nUnit == 0 ? "" : sNum[nUnit];
	}

	return strOut;
}

// Êý×Ö×ª×Ö·û£¨ÓÃÒÔ´óÐ´×ª»»1£©
std::wstring NStr::IntToWStrForCapital(int nValue)
{
	if (nValue > 999)
	{
		nValue = 999;
	}
	const wchar_t* sNum[] = { L"Áã", L"Ò»", L"¶þ", L"Èý", L"ËÄ", L"Îå", L"Áù", L"Æß", L"°Ë", L"¾Å" };
	const wchar_t* sBit[] = { L"", L"Ê®", L"°Ù", L"Ç§" };
	const wchar_t* sTep[] = { L"", L"Íò", L"ÒÚ", L"Íò", L"ÒÚ", L"Íò", L"ÒÚ" };

	int nHun = nValue / 10 / 10;
	int nTen = nValue / 10 % 10;
	int nUnit = nValue % 10;

	std::wstring strOut = L"";
	if (nHun != 0)
	{
		strOut = sNum[nHun];
		strOut += sBit[2];
	}

	if (nTen == 0)
	{
		strOut += nHun == 0 ? L"" : sNum[0];
		strOut += sNum[nUnit];
	}
	else if (nTen == 1)
	{
		strOut += nHun == 0 ? L"" : sNum[nTen];
		strOut += sBit[1];
		strOut += nUnit == 0 ? L"" : sNum[nUnit];
	}
	else
	{
		strOut += sNum[nTen];
		strOut += sBit[1];
		strOut += nUnit == 0 ? L"" : sNum[nUnit];
	}

	return strOut;
}


//************************************
// Method:      VersionCompare
// Description: °æ±¾±È½Ï£¨·µ»ØÊÇ·ñÓÐÐÂ°æ±¾£© a.b.c
// Returns:     bool
// Parameter:   std::string  strOldVer		¾É°æ±¾ºÅ
// Parameter:   std::string  strNewVer		ÐÂ°æ±¾ºÅ
// Parameter:   std::string  strSplite		°æ±¾ºÅ¼ä¸ô·û£¬Ä¬ÈÏÊ¹ÓÃµã"."
// Author:	    JJHUANG
// Date:        2017-6-23
//************************************
bool NStr::VersionCompare(std::string  strOldVer, std::string  strNewVer, std::string  strSplite /* = "." */)
{
	if (strOldVer.empty() || strNewVer.empty())
		return false;

	bool bRet = false;
	 std::vector<std::string > vectOldVer;
	 std::vector<std::string > vectNewVer;
	NStr::SpliteStrByChar(strOldVer, vectOldVer, '.', true);
	NStr::SpliteStrByChar(strNewVer, vectNewVer, '.', true);

	if (vectOldVer.empty() || vectNewVer.empty() || vectOldVer.size() != vectNewVer.size())
		return false;

	int nOldVer = 0;
	int nNewVer = 0;
	//a.b.c -> a*100*100 + b*100 + c
	for (int i=0; i<vectOldVer.size(); ++i)
	{
		int nVer = NStr::StrToInt(vectOldVer[i]);
		nOldVer += nVer * pow(100, 2 - i);
	}
	for (int i=0; i<vectNewVer.size(); ++i)
	{
		int nVer = NStr::StrToInt(vectNewVer[i]);
		nNewVer += nVer * pow(100, 2 - i);
	}

	return nOldVer < nNewVer;
}

//////////////////////////////////////////////////////////////////////////
typedef struct h2p {
	char *py;
	char *py_shengdiao;
	unsigned shengdiao;
	char *hz;
} pyhz_tab;

pyhz_tab p2h[] = {
	{ "a", "a1", 1, "°¢°¡ºÇëçß¹ï¹" },
	{ "a", "a2", 2, "°¡ºÇàÄ" },
	{ "a", "a3", 3, "°¡ºÇ" },
	{ "a", "a4", 4, "°¡ºÇ" },
	{ "a", "a5", 5, "°¢°¡ºÇ" },
	{ "ai", "ai1", 1, "°§°¤°£°¦°¥ÞßïÍ" },
	{ "ai", "ai2", 2, "´ô°¤°©°¨Þß" },
	{ "ai", "ai3", 3, "°«°¥°ªö°àÈ" },
	{ "ai", "ai4", 4, "°®°­°¬°¦°¥°¯êÓàÈè¨àÉæÈíÁ" },
	{ "an", "an1", 1, "°²ÚÏ°°°±âÖèñðÆ¹ã³§" },
	{ "an", "an3", 3, "°³ï§ÞîÛû" },
	{ "an", "an4", 4, "°¸°´°µ°¶÷ö°·áí" },
	{ "ang", "ang1", 1, "°¹" },
	{ "ang", "ang2", 2, "°º" },
	{ "ang", "ang4", 4, "°»" },
	{ "ao", "ao1", 1, "°¾°¼" },
	{ "ao", "ao2", 2, "°¾°½Ïùà»÷é÷¡°¿éáñúòüâÚåÛ" },
	{ "ao", "ao3", 3, "°ÀÞÖæÁ" },
	{ "ao", "ao4", 4, "°Â°Ä°Á°ÃÛêÞÖæñá®öË" },
	{ "ba", "ba1", 1, "°Ë°É°Í°È°Å°Ç°Ì°ÊôÎá±°Æ" },
	{ "ba", "ba2", 2, "°Ë°Î°ÏÜØÝÃ÷É" },
	{ "ba", "ba3", 3, "°Ñ°ÐîÙ" },
	{ "ba", "ba4", 4, "°Ñ°Ö°Õ°Ô°Ó°Òå±öÑ" },
	{ "ba", "ba5", 5, "°É°Õ" },
	{ "bai", "bai1", 1, "êþ" },
	{ "bai", "bai2", 2, "°×" },
	{ "bai", "bai3", 3, "°Ù°Ú²®°Ø°ÛÞã" },
	{ "bai", "bai4", 4, "°Ü°ÝßÂ°Þ" },
	{ "ban", "ban1", 1, "°ã°à°á°ß°ä°âñ£ñ­" },
	{ "ban", "ban3", 3, "°æ°åÚæÛàîÓô²" },
	{ "ban", "ban4", 4, "°ì°ë°é°ç°ê°è°í" },
	{ "bang", "bang1", 1, "°ï°îäº°ð" },
	{ "bang", "bang3", 3, "°ò°ñ°ó" },
	{ "bang", "bang4", 4, "°ô°ò°ø°õ°ù°÷°öÝò" },
	{ "bao", "bao1", 1, "°ü°ûÅÚ°þ°ý°úæßìÒöµ" },
	{ "bao", "bao2", 2, "±¡±¢" },
	{ "bao", "bao3", 3, "±£±¦±¥±¤ÝáñÙð±" },
	{ "bao", "bao4", 4, "±¨±©±§±¬±«ÆØÅÙÆÙ±ªõÀ" },
	{ "bei", "bei1", 1, "±³±¯±­±®±°ÚéÛýÝÉðÇ" },
	{ "bei", "bei3", 3, "±±" },
	{ "bei", "bei4", 4, "±»±¸±³±²±¶±´Ýí±¹ã£±·±ºÚý±µØÃíÕñØöÍ÷¹" },
	{ "bei", "bei5", 5, "±ÛßÂ" },
	{ "ben", "ben1", 1, "±¼êÚï¼" },
	{ "ben", "ben3", 3, "±¾±½ÛÎ" },
	{ "ben", "ben4", 4, "±¼±¿º»ÛÐ" },
	{ "beng", "beng1", 1, "±À±ÁàÔ" },
	{ "beng", "beng2", 2, "±Â" },
	{ "beng", "beng3", 3, "±Á" },
	{ "beng", "beng4", 4, "±Á±Ä±Åê´±Ã°ö" },
	{ "bi", "bi1", 1, "±Æ" },
	{ "bi", "bi2", 2, "±ÇÝ©" },
	{ "bi", "bi3", 3, "±È±Ê±Ë±ÉØ°ÙÂåþßÁïõô°" },
	{ "bi", "bi4", 4, "±Ø±Ï±ÒÃØ±Ü±Õ±Ú±Û±×±Ù±Ì·÷±Ð±Î±Óèµ±ÖÃÚ±Ýåöó÷æ¾ã¹±ÔîéñÔå¨÷ÂâØ±Ñää±ÍÛýÜÅæÔÜêêÚî¯ÝÉÞµóÙóëßÙôÅõÏáù" },
	{ "bian", "bian1", 1, "±à±ß±Þí¾ìÔòùóÖöý" },
	{ "bian", "bian3", 3, "±á±âØÒíÜñ¹ñÛ" },
	{ "bian", "bian4", 4, "±ã±ä±é±ç±æ±è±åÜÐãêâíÛÍçÂ" },
	{ "bian", "bian5", 5, "±ß" },
	{ "biao", "biao1", 1, "±ê±ëÉ×ïÚ±ìæôïðè¼ì®ì©ì­ñ¦÷Ô" },
	{ "biao", "biao3", 3, "±íñÑæ»" },
	{ "biao", "biao4", 4, "÷§" },
	{ "bie", "bie1", 1, "±ï±ñ±î" },
	{ "bie", "bie2", 2, "±ðõ¿" },
	{ "bie", "bie3", 3, "±ñ" },
	{ "bie", "bie4", 4, "±ð" },
	{ "bin", "bin1", 1, "±ö±õ±ò±óçÍ±ôéÄÙÏçãáÙïÙ" },
	{ "bin", "bin4", 4, "÷Þéë±÷ë÷÷Æ" },
	{ "bing", "bing1", 1, "²¢±ø±ùéÄ" },
	{ "bing", "bing3", 3, "±ýÆÁ±û±ú±ü±þÙ÷Úû" },
	{ "bing", "bing4", 4, "²¢²¡Þð" },
	{ "bo", "bo1", 1, "°ã²¨²¥²¦°þ²£âÄ²¤²§õÀ" },
	{ "bo", "bo2", 2, "°Ù²©²®²ª±¡²´°Ø²µÆÇ²±²«²²²°íç²¯²¬²­²³îàØÃÙñð¾õÛ" },
	{ "bo", "bo3", 3, "ô¤õË" },
	{ "bo", "bo4", 4, "±¡°Øô¤êþë¢éÞ" },
	{ "bo", "bo5", 5, "²·à£" },
	{ "bu", "bu1", 1, "åÍêÎîß" },
	{ "bu", "bu2", 2, "²»õ³" },
	{ "bu", "bu3", 3, "²¹²¶±¤²·²¸ß²" },
	{ "bu", "bu4", 4, "²»²¿²¼²½²À²¾ÆÒ²ºê³îÐ" },
	{ "ca", "ca1", 1, "²Á²ðàê" },
	{ "ca", "ca3", 3, "íå" },
	{ "cai", "cai1", 1, "²Â" },
	{ "cai", "cai2", 2, "²Å²Æ²Ä²Ã" },
	{ "cai", "cai3", 3, "²É²Ê²È²Ç" },
	{ "cai", "cai4", 4, "²É²Ë²Ì" },
	{ "can", "can1", 1, "²Î²Íæî" },
	{ "can", "can2", 2, "²Ð²Ñ²Ï" },
	{ "can", "can3", 3, "²Ò÷õ" },
	{ "can", "can4", 4, "²Ò²Ó²ôè²åîôÓ" },
	{ "cang", "cang1", 1, "²Ô²Ö²×²ÕØ÷" },
	{ "cang", "cang2", 2, "²Ø" },
	{ "cao", "cao1", 1, "²Ù²Ú" },
	{ "cao", "cao2", 2, "²Ü²ÛàÐäîó©ô½" },
	{ "cao", "cao3", 3, "²Ý" },
	{ "ce", "ce4", 4, "²ß²â²à²Þ²áâü" },
	{ "cen", "cen1", 1, "²Î" },
	{ "cen", "cen2", 2, "á¯ä¹" },
	{ "ceng", "ceng1", 1, "àá" },
	{ "ceng", "ceng2", 2, "Ôø²ã" },
	{ "ceng", "ceng4", 4, "²ä" },
	{ "cha", "cha1", 1, "²î²å²æ²êÔûàêè¾âÇïÊ" },
	{ "cha", "cha2", 2, "²é²ì²è²æ²ç²êé«âª²ëé¶éß" },
	{ "cha", "cha3", 3, "²æñÃïï" },
	{ "cha", "cha4", 4, "²îÉ²²æ²ï²íñÃè¾ãâæ±" },
	{ "chai", "chai1", 1, "²î²ðîÎ" },
	{ "chai", "chai2", 2, "²ñ²òÙ­" },
	{ "chai", "chai4", 4, "ò²ðû" },
	{ "chan", "chan1", 1, "²ó²ôêè" },
	{ "chan", "chan2", 2, "µ¥²øìø²õ²öäýó¸æ¿²÷âÜåîïâå¤õð" },
	{ "chan", "chan3", 3, "²ú²ù²ûÚÆÙæÝÛæö" },
	{ "chan", "chan4", 4, "²üâãåñ" },
	{ "chang", "chang1", 1, "²ýæ½²þØöãÑÝÅöð" },
	{ "chang", "chang2", 2, "³¤³¡³£³¢³¦³¥ÌÈÉÑæÏáäÜÉ" },
	{ "chang", "chang3", 3, "³¡³§³¨ë©êÆã®" },
	{ "chang", "chang4", 4, "³ª³©³«âêÛË" },
	{ "chao", "chao1", 1, "³¬³­³³³®´Â½ËìÌâ÷" },
	{ "chao", "chao2", 2, "³¯³±³°³²êË" },
	{ "chao", "chao3", 3, "³´³³" },
	{ "chao", "chao4", 4, "ñé" },
	{ "che", "che1", 1, "³µíº" },
	{ "che", "che3", 3, "³ß³¶" },
	{ "che", "che4", 4, "³¹³·³º³¸Ûå" },
	{ "chen", "chen1", 1, "³»è¡àÁÞÓ" },
	{ "chen", "chen2", 2, "³Â³Á³¿Éò³¾³¼³½³È³ÀÚÈå·" },
	{ "chen", "chen3", 3, "í×" },
	{ "chen", "chen4", 4, "³Æ³Ã³Ä³ÓÚßé´ö³" },
	{ "chen", "chen5", 5, "Ø÷" },
	{ "cheng", "cheng1", 1, "³Æ³Å³ÓîªàáîõèßòÉ" },
	{ "cheng", "cheng2", 2, "³É³Ç³Ì³Ð³ÏÊ¢³Ë³Ê³Í³Î³ÈØ©ÛôèÇëóîññÎõ¨" },
	{ "cheng", "cheng3", 3, "³Ñ³ÒñÎ" },
	{ "cheng", "cheng4", 4, "³Æ³Ó" },
	{ "chi", "chi1", 1, "³Ô³ÕßêàÍò¿ó×ð·æÊó¤í÷÷Î" },
	{ "chi", "chi2", 2, "³Ö³Ù³Ø³Û³×³ÚõØÜ¯ÜÝóøÛæ" },
	{ "chi", "chi3", 3, "³ß³Ý³Ü³ÞñÝôù" },
	{ "chi", "chi4", 4, "³à³â³áà´³ãë·ß³âÁÙÑáÜñ¡" },
	{ "chong", "chong1", 1, "³å³äÓ¿ã¿âçô¾ô©Üû" },
	{ "chong", "chong2", 2, "ÖÖÖØ³ç³æ" },
	{ "chong", "chong3", 3, "³è" },
	{ "chong", "chong4", 4, "³åï¥" },
	{ "chou", "chou1", 1, "³éñ¬" },
	{ "chou", "chou2", 2, "³î³ð³ï³ê³ñ³ìã°³ë³íàüÙ±öÅ" },
	{ "chou", "chou3", 3, "³ó³ò" },
	{ "chou", "chou4", 4, "³ô" },
	{ "chu", "chu1", 1, "³ö³õéË" },
	{ "chu", "chu2", 2, "³ý³ø³ù³÷³û³úòÜÛ»³üõé" },
	{ "chu", "chu3", 3, "´¦³þ´¢´¡èÆñÒèú" },
	{ "chu", "chu4", 4, "´¦´¥Ðó´£âð´¤ç©÷íØ¡ãÀ" },
	{ "chuai", "chuai1", 1, "´§Þõ" },
	{ "chuai", "chuai3", 3, "´§" },
	{ "chuai", "chuai4", 4, "´§à¨õßàÜëú" },
	{ "chuan", "chuan1", 1, "´©´¨çÝë°" },
	{ "chuan", "chuan2", 2, "´«´¬å×´ªô­" },
	{ "chuan", "chuan3", 3, "´­â¶" },
	{ "chuan", "chuan4", 4, "´®îË" },
	{ "chuang", "chuang1", 1, "´´´°´¯" },
	{ "chuang", "chuang2", 2, "´²´±" },
	{ "chuang", "chuang3", 3, "´³" },
	{ "chuang", "chuang4", 4, "´´âë" },
	{ "chui", "chui1", 1, "´µ´¶" },
	{ "chui", "chui2", 2, "´¹´¸´·Úï×µé³é¢" },
	{ "chun", "chun1", 1, "´º´»òí" },
	{ "chun", "chun2", 2, "´¿´½´¼´¾ðÈÝ»" },
	{ "chun", "chun3", 3, "´À" },
	{ "chuo", "chuo1", 1, "´ÁõÖ" },
	{ "chuo", "chuo4", 4, "´Âà¨ê¡öº" },
	{ "ci", "ci1", 1, "²î´Ì´ÃßÚ" },
	{ "ci", "ci2", 2, "´Ê´Ç´È´Å´É×È´Ä´ÆìôÜëðËôÙ" },
	{ "ci", "ci3", 3, "´Ë" },
	{ "ci", "ci4", 4, "´Î´Ì´ÍËÅ" },
	{ "cong", "cong1", 1, "´Ó´Ò´Ï´Ð´ÑÜÊæõè®èÈ" },
	{ "cong", "cong2", 2, "´Ó´ÔçýäÈ" },
	{ "cou", "cou4", 4, "´Õé¨ê£ëí" },
	{ "cu", "cu1", 1, "´Ö" },
	{ "cu", "cu2", 2, "áÞéã" },
	{ "cu", "cu4", 4, "´Ù´Ø´××äâ§õíõ¾Ýýõ¡" },
	{ "cuan", "cuan1", 1, "´Úß¥Ùàïé" },
	{ "cuan", "cuan2", 2, "ÔÜ" },
	{ "cuan", "cuan4", 4, "´Ü´Ûìà" },
	{ "cui", "cui1", 1, "Ë¥´ß´Ý´Þö¿éÁ" },
	{ "cui", "cui3", 3, "è­" },
	{ "cui", "cui4", 4, "´à´âÝÍ´ä´áã²´ãë¥ßý" },
	{ "cun", "cun1", 1, "´åñå" },
	{ "cun", "cun2", 2, "´æ¶×" },
	{ "cun", "cun3", 3, "ââ" },
	{ "cun", "cun4", 4, "´ç" },
	{ "cuo", "cuo1", 1, "´ê´é´èõã" },
	{ "cuo", "cuo2", 2, "áÏïóðîðûõº" },
	{ "cuo", "cuo3", 3, "´éëâ" },
	{ "cuo", "cuo4", 4, "´í´ë´ìØÈï±" },
	{ "da", "da1", 1, "´ð´îàªÞÇñ×ßÕ" },
	{ "da", "da2", 2, "´ò´ï´ð´ñí³÷²âòóÎ÷°æ§" },
	{ "da", "da3", 3, "´ò" },
	{ "da", "da4", 4, "´ó" },
	{ "da", "da5", 5, "Ëþðã" },
	{ "dai", "dai1", 1, "´ý´ôß¾" },
	{ "dai", "dai3", 3, "´þ´õ´ö" },
	{ "dai", "dai4", 4, "´ó´ú´ø´ý´÷´ü´û´þ´ù÷ìµ¡çéá·åÊææçªÜ¤ß°" },
	{ "dan", "dan1", 1, "µ¥µ£µ¤µ¢íñééóìÙÙð÷ñõµ¦" },
	{ "dan", "dan3", 3, "µ£µ¨µ§êæðãð÷" },
	{ "dan", "dan4", 4, "µ«µ£Ê¯µ¯µ­µ©µ°µ®µ¬à¢å£µªÝÌð÷" },
	{ "dang", "dang1", 1, "µ±ñÉîõ" },
	{ "dang", "dang3", 3, "µ³µ²ÚÔ" },
	{ "dang", "dang4", 4, "µ±µ´µµµ²å´ÝÐÛÊí¸" },
	{ "dao", "dao1", 1, "µ¶ß¶âáë®" },
	{ "dao", "dao2", 2, "ß¶" },
	{ "dao", "dao3", 3, "µ¼µ¹µºµ¸µ·µ»" },
	{ "dao", "dao4", 4, "µ½µÀµ¹µ¿µÁµ¾ìâàüôî" },
	{ "de", "de2", 2, "µÃµÂï½" },
	{ "de", "de5", 5, "µÄµØµÃµ×" },
	{ "dei", "dei3", 3, "µÃ" },
	{ "deng", "deng1", 1, "µÇµÆµÅàâô£" },
	{ "deng", "deng3", 3, "µÈê­" },
	{ "deng", "deng4", 4, "µËµÊµÉ³ÎµÅíãïëáØ" },
	{ "di", "di1", 1, "ÌáµÍµÎµÌàÖØµïáôÆ" },
	{ "di", "di2", 2, "µÄµÐµÏµÑµÓàÖµÒµÕµÔÝ¶Ùáêëïá" },
	{ "di", "di3", 3, "µ×µÖÚ®Û¡íÆÛæèÜØµ÷¾" },
	{ "di", "di4", 4, "µÄµØµÚµÛµÜµÝµÙµÞÚÐíûé¦æ·íÚç°" },
	{ "dia", "dia3", 3, "àÇ" },
	{ "dian", "dian1", 1, "µßµáµàñ²áÛ" },
	{ "dian", "dian3", 3, "µãµäµâõÚØ¼" },
	{ "dian", "dian4", 4, "µçµêµéµíµæµîµìµëµèçèô¡Ûãµåîäñ°Úç" },
	{ "diao", "diao1", 1, "µñµóµòµðõõµïöô" },
	{ "diao", "diao3", 3, "Äñ" },
	{ "diao", "diao4", 4, "µ÷µôµõµöï¢îö" },
	{ "die", "die1", 1, "µùµøõÚ" },
	{ "die", "die2", 2, "µþµüµúµýµûà©ØýëºñóõÞÜ¦ð¬ÞéÛìöø" },
	{ "ding", "ding1", 1, "¶¡¶¢¶¤¶£î®ôúðÛØêñôçà" },
	{ "ding", "ding3", 3, "¶¥¶¦ôú" },
	{ "ding", "ding4", 4, "¶¨¶©¶¤îúëë¶§íÖà¤" },
	{ "diu", "diu1", 1, "¶ªîû" },
	{ "dong", "dong1", 1, "¶«¶¬ßËá´ë±ð´" },
	{ "dong", "dong3", 3, "¶®¶­íÏ" },
	{ "dong", "dong4", 4, "¶¯¶´¶³¶°¶²¶±Ûíá¼ëËëØíÏ" },
	{ "dou", "dou1", 1, "¶¼¶µÝúóû" },
	{ "dou", "dou3", 3, "¶·¶¶¶¸ò½" },
	{ "dou", "dou4", 4, "¶Á¶·¶¹¶ºñ¼¶»" },
	{ "du", "du1", 1, "¶¼¶½à½" },
	{ "du", "du2", 2, "¶Á¶À¶Ù¶¾äÂë¹¶¿÷ò÷Çèü" },
	{ "du", "du3", 3, "¶Ç¶Ã¶Â¶ÄóÆ" },
	{ "du", "du4", 4, "¶È¶É¶Ç¶Å¶Ê¶ÆÜ¶ó¼" },
	{ "duan", "duan1", 1, "¶Ë" },
	{ "duan", "duan3", 3, "¶Ì" },
	{ "duan", "duan4", 4, "¶Ï¶Î¶Í¶ÐìÑé²óý" },
	{ "dui", "dui1", 1, "¶Ñ" },
	{ "dui", "dui4", 4, "¶Ô¶Ó¶Ò¶ØíÔí­í¡ïæ" },
	{ "dun", "dun1", 1, "¶Ö¶Ø¶×¶Õíâïæ" },
	{ "dun", "dun3", 3, "íïõ»" },
	{ "dun", "dun4", 4, "¶Ù¶Ü¶ÛìÀ¶Ýãç¶Úí»" },
	{ "duo", "duo1", 1, "¶àßÍ¶ß¶ÞñÖ" },
	{ "duo", "duo2", 2, "¶È¶áõâîì" },
	{ "duo", "duo3", 3, "¶ä¶ã¶âßáç¶" },
	{ "duo", "duo4", 4, "¶æ¶é¶å¶ç¶è¶âÍÔãõèÞ" },
	{ "e", "e1", 1, "°¢æ¹åí" },
	{ "e", "e2", 2, "¶î¶íÅ¶¶ì¶ð¶ë¶ê¶ïÝ­ï°" },
	{ "e", "e3", 3, "¶ñ" },
	{ "e", "e4", 4, "¶ñ¶ö¶óãµ¶ôØ¬ßÀ¶ò¶õéîò¦öùÚÌïÉÝàëñÛÑðÊÜÃãÕ" },
	{ "e", "e5", 5, "ßÀ" },
	{ "ei", "ei2", 2, "ÚÀ" },
	{ "ei", "ei3", 3, "ÚÀ" },
	{ "ei", "ei4", 4, "ÚÀ" },
	{ "en", "en1", 1, "¶÷Ýì" },
	{ "en", "en4", 4, "Þô" },
	{ "er", "er2", 2, "¶ø¶ùð¹öÜ" },
	{ "er", "er3", 3, "¶û¶úåÇ¶ü¶ýçíîï" },
	{ "er", "er4", 4, "¶þ·¡Ù¦" },
	{ "fa", "fa1", 1, "·¢" },
	{ "fa", "fa2", 2, "·£·¦·¥·§·¤ÛÒ" },
	{ "fa", "fa3", 3, "·¨íÀ" },
	{ "fa", "fa4", 4, "·¢·©" },
	{ "fan", "fan1", 1, "·­·¬·«·ªá¦Þ¬" },
	{ "fan", "fan2", 2, "·²·³·±·º·®Þ¬ìÜ·¯ÞÀ·°õì" },
	{ "fan", "fan3", 3, "·´·µ" },
	{ "fan", "fan4", 4, "·¹·¸·¶···ºèóî²" },
	{ "fang", "fang1", 1, "·½·¼·Á·»ÚúèÊîÕ" },
	{ "fang", "fang2", 2, "·¿·À·Á·»·¾öÐ" },
	{ "fang", "fang3", 3, "·Ã·Â·ÄáÝô³" },
	{ "fang", "fang4", 4, "·Å" },
	{ "fei", "fei1", 1, "·Ç·É·È·Æìéö­åúç³òãöî" },
	{ "fei", "fei2", 2, "·ÊëèäÇ" },
	{ "fei", "fei3", 3, "·Æ·Ë·Ìì³òãôäã­óõé¼" },
	{ "fei", "fei4", 4, "·Ñ·Ï·Ð·Î·ÍðòáôïÐÜÀ" },
	{ "fen", "fen1", 1, "·Ö·×·Õ·Ò·Ô·Óçã" },
	{ "fen", "fen2", 2, "·Ø·Ù·Úèû÷÷" },
	{ "fen", "fen3", 3, "·Û" },
	{ "fen", "fen4", 4, "·Ö·Ý·Ü·ß·à·ÞÙÇå¯ö÷" },
	{ "feng", "feng1", 1, "·ç·â·á·å·è·æ·ä·ã·éÛºÝ×ããí¿" },
	{ "feng", "feng2", 2, "·ê·ì·ë" },
	{ "feng", "feng3", 3, "·íßô" },
	{ "feng", "feng4", 4, "·î·ì·ïÙºÝ×" },
	{ "fo", "fo2", 2, "·ð" },
	{ "fou", "fou3", 3, "·ñó¾" },
	{ "fu", "fu1", 1, "·ò·ô·ó·õß»ïûôïõÃõÆ" },
	{ "fu", "fu2", 2, "·ò·þ¸£·ð·ù·ü·û¸¡·ö¸¥·÷¸¤·ýÜ½æÚÙë·ø¸¢·úèõòÝÜÞÜòÝ³ÝÊá¥âöåõÛ®ç¦ç¨ÙììðíÉíêî·ïûò¶ÜÀòð" },
	{ "fu", "fu3", 3, "¸®¸¸¸¯¸§¸¨¸¦¸©¸«¸¬¸ª¸­ÞÔäæíë" },
	{ "fu", "fu4", 4, "·þ¸´¸¸¸º¸±¸»¸¶¸¾¸½¸°¸¹¸²¸³¸µ¸¿¸À¸·¸¼æâêçð¥òóöÖöû" },
	{ "fu", "fu5", 5, "¸À" },
	{ "ga", "ga1", 1, "¼Ð¿§¸Â¸ìÙ¤ê¸" },
	{ "ga", "ga2", 2, "¸Â¸ÁÔþæÙîÅ" },
	{ "ga", "ga3", 3, "¸ÂæØ" },
	{ "ga", "ga4", 4, "ÞÎ" },
	{ "gai", "gai1", 1, "¸ÃêàÛòÚë" },
	{ "gai", "gai3", 3, "¸Ä" },
	{ "gai", "gai4", 4, "¸Å¸ÇØ¤¸Æ½æ¸Èê®" },
	{ "gan", "gan1", 1, "¸É¸Ê¸Î¸ËÞÏÇ¬¸ÍÛáÜÕ¸Ìãïí·ðáôû" },
	{ "gan", "gan3", 3, "¸Ð¸Ò¸Ï¸ËéÏ¸Ñß¦ä÷" },
	{ "gan", "gan4", 4, "¸É¸ÓäÆç¤êº" },
	{ "gang", "gang1", 1, "¸Õ¸Ö¸Ù¸×¿¸¸Ü¸Ô¸Øî¸" },
	{ "gang", "gang3", 3, "¸Û¸Ú" },
	{ "gang", "gang4", 4, "¸Ö¸Üí°óà" },
	{ "gao", "gao1", 1, "¸ß¸â¸à¸Þ¸áØº¸ÝéÀ" },
	{ "gao", "gao3", 3, "¸å¸ãÞ»éÂçÉ¸äê½" },
	{ "gao", "gao4", 4, "¸æ¸àÚ¾Û¬ï¯" },
	{ "ge", "ge1", 1, "¸è¸ñ¸ç¸ê¸î¸ì¸é¸í¿©¸ëÒÙØîÛÙæüñË" },
	{ "ge", "ge2", 2, "¸ï¸ñ¸ô¸ð¸ó¸ì¸é¸òàÃ÷Àò¢ë¡ëõïÓÜªØª" },
	{ "ge", "ge3", 3, "¸ö¸÷ºÏ¸Ç¸ðÛÁô´" },
	{ "ge", "ge4", 4, "¸ö¸÷¸õíÑò´" },
	{ "gei", "gei3", 3, "¸ø" },
	{ "gen", "gen1", 1, "¸ù¸ú" },
	{ "gen", "gen2", 2, "ßç" },
	{ "gen", "gen3", 3, "ôÞ" },
	{ "gen", "gen4", 4, "Ø¨ôÞÝ¢" },
	{ "geng", "geng1", 1, "¸ü¸û¸ý¸þâÙ" },
	{ "geng", "geng3", 3, "¹¢¾±¹£ßìöá¹¡ç®" },
	{ "geng", "geng4", 4, "¸ü" },
	{ "gong", "gong1", 1, "¹¤¹«¹²ºì¹©¹¦¹¥¹¬¹§¹ª¹¨¹­ëÅò¼ö¡" },
	{ "gong", "gong3", 3, "¹®¹°¹¯çî" },
	{ "gong", "gong4", 4, "¹²¹©¹±" },
	{ "gou", "gou1", 1, "¾ä¹µ¹´¹³óôØþèÛçÃ÷¸" },
	{ "gou", "gou3", 3, "¹·¹¶á¸èÛóÑ" },
	{ "gou", "gou4", 4, "¹»¹º¹¹¹´êí¹¸Ú¸æÅåÜì°" },
	{ "gu", "gu1", 1, "¹Ã¹Ç¹Â¹À¹¼¹¾ßÉ¹¿¹Á¹½éïð³ì±ÝÔòÁôþõý" },
	{ "gu", "gu2", 2, "¹Ç" },
	{ "gu", "gu3", 3, "¹Å¹É¹Ä¹Ç¹È¼Öãé¹Æì±ðÀêôëûÚ¬î­î¹îÜØÅòÁ÷½" },
	{ "gu", "gu4", 4, "¹Ê¹Ë¹Ì¹À¹ÍïÀèôêöáÄðóöñ" },
	{ "gua", "gua1", 1, "À¨¹Ï¹ÎßÉèéëÒð»" },
	{ "gua", "gua3", 3, "¹ÑßÉ¹Ð" },
	{ "gua", "gua4", 4, "¹Ò¹ÓØÔÚ´" },
	{ "guai", "guai1", 1, "¹ÔÞâ" },
	{ "guai", "guai3", 3, "¹Õ" },
	{ "guai", "guai4", 4, "¹Ö" },
	{ "guan", "guan1", 1, "¹Ø¹Û¹Ù¹Ú¹×ñæÝ¸ÙÄÂÚ÷¤" },
	{ "guan", "guan3", 3, "¹Ü¹ÝÝ¸" },
	{ "guan", "guan4", 4, "¹Û¹ß¹Ú¹á¹Þ¹àÞèîÂäÊðÙ" },
	{ "guang", "guang1", 1, "¹âßÛë×èæ" },
	{ "guang", "guang3", 3, "¹ãáî" },
	{ "guang", "guang4", 4, "¹äèæ" },
	{ "gui", "gui1", 1, "¹æ¹é¹å¹ê¹è¹ëð§¿þ¹çæ£öÙ" },
	{ "gui", "gui3", 3, "¹í¹ì¹î¹ïØÐâÑå³êÐóþ" },
	{ "gui", "gui4", 4, "¹ó¹ð¹ò¹ñ¹ôÈ²ØÛèíêÁ÷¬" },
	{ "gun", "gun3", 3, "¹ööçÙòçµíÞ¹õ" },
	{ "gun", "gun4", 4, "¹÷" },
	{ "guo", "guo1", 1, "¹ý¹ø¹ùÎÐñøòåáÆÛößÃ" },
	{ "guo", "guo2", 2, "¹úàþÞâÙåë½" },
	{ "guo", "guo3", 3, "¹û¹üâ£é¤òä" },
	{ "guo", "guo4", 4, "¹ý" },
	{ "ha", "ha1", 1, "¹þîþ" },
	{ "ha", "ha2", 2, "Ïº¸ò" },
	{ "ha", "ha3", 3, "¹þ" },
	{ "ha", "ha4", 4, "¹þ" },
	{ "hai", "hai1", 1, "ºÙ¿ÈàË" },
	{ "hai", "hai2", 2, "»¹º¢º¡" },
	{ "hai", "hai3", 3, "º£ëÜõ°" },
	{ "hai", "hai4", 4, "º¦º¥º§º¤" },
	{ "han", "han1", 1, "º¨º©ñü÷ýòÀ" },
	{ "han", "han2", 2, "º¬º®º¹º«º­º¯êÏìÊºªÚõ" },
	{ "han", "han3", 3, "º°º±ãÛ" },
	{ "han", "han4", 4, "ººº¹º¶º²º³ºµº´º·å«º¸ò¥ÝÕÞþ" },
	{ "hang", "hang1", 1, "º»" },
	{ "hang", "hang2", 2, "ÐÐº½¿Ôº¼ç¬çññþ" },
	{ "hang", "hang4", 4, "ÐÐÏïãì" },
	{ "hao", "hao1", 1, "ÝïÞ¶àã" },
	{ "hao", "hao2", 2, "ºÅºÁºÀº¿º¾ºÑàÆå©òº" },
	{ "hao", "hao3", 3, "ºÃºÂ" },
	{ "hao", "hao4", 4, "ºÃºÅºÆºÄð©ê»å°¸äò«" },
	{ "he", "he1", 1, "ºÈºÇÚ­àÀ" },
	{ "he", "he2", 2, "ºÍºÎºÏºÓºËºÐºÌºÉºÒºÔãØºÑêÂò¢ÛÀºÊîÁæüòÂôç" },
	{ "he", "he4", 4, "ºÍºÎºÈºÕÏÅºØºÉº×ÛÖºÖ" },
	{ "hei", "hei1", 1, "ºÚºÙàË" },
	{ "hen", "hen2", 2, "ºÛ" },
	{ "hen", "hen3", 3, "ºÜºÝ" },
	{ "hen", "hen4", 4, "ºÞ" },
	{ "heng", "heng1", 1, "ºßºà" },
	{ "heng", "heng2", 2, "ÐÐºáºâºãÞ¿çñèì" },
	{ "heng", "heng4", 4, "ºá" },
	{ "hng", "hng5", 5, "ºß" },
	{ "hong", "hong1", 1, "ºäºåºæÞ°Ùê" },
	{ "hong", "hong2", 2, "ºìºéºèºêºçºëãüãÈÞ®ÙäÝ¦" },
	{ "hong", "hong3", 3, "ºå" },
	{ "hong", "hong4", 4, "ºåÚ§Þ®" },
	{ "hou", "hou2", 2, "ºîºíºïðúóóô×÷¿" },
	{ "hou", "hou3", 3, "ºð" },
	{ "hou", "hou4", 4, "ºóºòááºñºîåËÜ©ö×" },
	{ "hu", "hu1", 1, "ºõºôÏ·ºöºýã±ßüäïéõìÃ" },
	{ "hu", "hu2", 2, "ºÍºúºþºýºËºøºüºù»¡ºûàñº÷õúðÀõ­â©éÎðÉì²ìÎ÷½" },
	{ "hu", "hu3", 3, "»¢ä°»£çú" },
	{ "hu", "hu4", 4, "»¤»§»¥ºý»¢»¦ìïìèìæóËá²âïð­ð×Ùü" },
	{ "hua", "hua1", 1, "»ª»¯»¨»©í¹" },
	{ "hua", "hua2", 2, "»ª»®»¬»©»í»«æèîü" },
	{ "hua", "hua4", 4, "»°»ª»¯»®»­èë" },
	{ "huai", "huai2", 2, "»³»²»´»±õ×" },
	{ "huai", "huai4", 4, "»µ" },
	{ "huai", "huai5", 5, "»®" },
	{ "huan", "huan1", 1, "»¶âµ" },
	{ "huan", "huan2", 2, "»¹»·å¾÷ß»¸à÷ä¡Û¨çÙïÌÝÈ" },
	{ "huan", "huan3", 3, "»º" },
	{ "huan", "huan4", 4, "»»»¼»Ã»½»Â»À»¾»Áä½Û¼ß§»¿äñåÕöé" },
	{ "huang", "huang1", 1, "»Ä»ÅëÁ" },
	{ "huang", "huang2", 2, "»Æ»Ê»Í»Ìáåè«»É»Ëäê»Èó¨åØÚò»Çñ¥äÒóòöü" },
	{ "huang", "huang3", 3, "»Î»Ð»Ñ»Ï" },
	{ "huang", "huang4", 4, "»Î" },
	{ "hui", "hui1", 1, "»Ó»Ô»Ò»Ö»Õ¶éÚ¶êÍ÷âçõßÔò³ãÄ" },
	{ "hui", "hui2", 2, "»Ø»²»×Üîä§" },
	{ "hui", "hui3", 3, "»Ù»Úò³" },
	{ "hui", "hui4", 4, "»á»ã»Ý»ÛÀ£»æ»ä»ß»Þ»à»ååç»âÜö»Üà¹í£ä«ßÜçÀèíÞ¥ó³" },
	{ "hun", "hun1", 1, "»é»è»çãÔ" },
	{ "hun", "hun2", 2, "»ì»ê»ëâÆçõ" },
	{ "hun", "hun4", 4, "»ìÚ»äã" },
	{ "huo", "huo1", 1, "»íØåß«ïÁñë" },
	{ "huo", "huo2", 2, "ºÍ»î" },
	{ "huo", "huo3", 3, "»ð»ïâ·îØ" },
	{ "huo", "huo4", 4, "ºÍ»ò»ñ»õ»ö»ó»ô»íÞ½àëïìó¶" },
	{ "ji", "ji1", 1, "Æä¼¸ÆÚ»ú»ù»÷Ææ¼¤»ý¼¦¼£¼¨¼¢¼©»ø¼§í¶¼¡¼¥ß´»ü»ûõÒî¿ïúßóçÜì´»þåìØÞçáêåê÷ÛÔÜ¸Ø¢ßÒóÇØÀ" },
	{ "ji", "ji2", 2, "¸ï¼°¼´¼­¼¶¼«¼¯¼±¼®¼ª¼²¼µ½å¼¹¼¬¼³á§óÅñ¤ÚµØ½é®ÝðéêÙ¥ê«áÕÞª" },
	{ "ji", "ji3", 3, "¼¸¸ø¼º¸ï¼Ã¼Í¼·¼¹êªò±Þá÷ä" },
	{ "ji", "ji4", 4, "¼ÇÏµ¼Æ¼Ã¼Ä¼Ê¼¼¼Í¼Ì¼ÈÆë¼¾¼Å¼À¼É¼Á¼½¼Ëæ÷¼»¼Â¼¿ôßö«ð¢ÙÊöê÷ÙêéÜùõÕßâöÝä©ÜÁ" },
	{ "jia", "jia1", 1, "¼Ò¼Ó¼Ñ¼Ð¼ÎÇÑÐ®¼ÏçìåÈÙ¤ä¤ðèóÕÝçïØôÂõÊ" },
	{ "jia", "jia2", 2, "¼Ð¼Õê©¼ÔÛ£í¢îòñÊòÌ" },
	{ "jia", "jia3", 3, "¼Ù½Ç½Å¼×½Á¼Ö½É½Ê½È½ÃÙ®½Æ½Ë½Äð¨ëÎ½ÂÞØáµáèäÐë¸¼ØØÅðý" },
	{ "jia", "jia4", 4, "¼Û¼Ù¼Ü¼Ý¼Þ¼Ú" },
	{ "jia", "jia5", 5, "¼Ò" },
	{ "jian", "jian1", 1, "¼ä¼á¼à½¥¼æ¼è¼çÇ³¼â¼é½¦¼å¼ß¼ê¼ãÝÑÝóÞöäÕçÌê§êùðÏöä÷µ" },
	{ "jian", "jian3", 3, "¼ò¼õ¼ì¼ô¼ñ¼ð¼ó¼î¼ë¼íå¿åÀ¼ïíúïµèÅê¯ÚÙàîñÐóÈôåõÂ" },
	{ "jian", "jian4", 4, "¼û¼ä¼þ½¨¼à½¥½¡½£¼ü¼ö¼ø¼ù½¢¼ý¼ú½¦¼÷ÚÉÙÔ½§½¤ë¦ïµé¥ëìêðõÝ" },
	{ "jiang", "jiang1", 1, "½«½­½®½ª½¬½©çÖÜüíäôø" },
	{ "jiang", "jiang3", 3, "½²½±½¯½°ñð" },
	{ "jiang", "jiang4", 4, "½«Ç¿½µ½´½¬ºç½³êñç­ä®ôÝ" },
	{ "jiao", "jiao1", 1, "½Ì½»½¹½¾½¼½º½·½¿½½æ¯õÓ½¶½¸öÞÙÕðÔòÔÜ´Üú" },
	{ "jiao", "jiao2", 2, "½À½Ãá½" },
	{ "jiao", "jiao3", 3, "½Ç½Å½Á½É½Ê½È½ÃÙ®½Æ½Ë½Äð¨ÞØáèäÐë¸½Â" },
	{ "jiao", "jiao4", 4, "½Ì¾õÐ£½Ð½Ï½Î½À½Ñ½ÍàÝá½áèõ´" },
	{ "jie", "jie1", 1, "½Ó½á½Ú½Ö½×½Ô½Ò¿¬àµ½ÕðÜà®" },
	{ "jie", "jie2", 2, "½á½Ú½Ü½Ý½Ø½à½Ù½ß½Þ½ÛÞ×æÝÚµèîíÙÙÊò¡Ú¦æ¼ôÉöÚ" },
	{ "jie", "jie3", 3, "½â½ã" },
	{ "jie", "jie4", 4, "½ç½â¼Û½é½è½ä½ì½å½ë½æ½êò»÷º" },
	{ "jie", "jie5", 5, "¼Ò¼Û" },
	{ "jin", "jin1", 1, "½ñ½ð½û½ò½ï½î½í½óñæñÆ" },
	{ "jin", "jin3", 3, "¾¡½ö½ô½÷½õèªâËÚáâÛÝÀéÈ" },
	{ "jin", "jin4", 4, "½ø½ü¾¡½ö½û¾¢½ú½þ½ùçÆ½ýàäêîÝ£êáæ¡" },
	{ "jing", "jing1", 1, "¾­¾©¾«¾ª¾¦¾§¾£¾¤¾¨ãþìº¾¥ëæÝ¼¾¬" },
	{ "jing", "jing3", 3, "¾¯¾°¾®¾±ã½ÚåÙÓØÙëÂ" },
	{ "jing", "jing4", 4, "¾­¾³¾¹¾²¾´¾µ¾¢¾º¾»¾¶¾¸¾·åÉëÖåòæºâ°ö¦" },
	{ "jiong", "jiong1", 1, "ìç" },
	{ "jiong", "jiong3", 3, "¾½¾¼åÄêÁ" },
	{ "jiu", "jiu1", 1, "¾¿¾À¾¾ð¯ôñà±ãÎ÷Ý" },
	{ "jiu", "jiu3", 3, "¾Å¾Æ¾Ã¾Â¾Ä¾Á" },
	{ "jiu", "jiu4", 4, "¾Í¾É¾È¾Î¾Ë¾Ì¾ÊðÕÙÖ¾ÇèêèÑ" },
	{ "ju", "ju1", 1, "³µ¾ÝÇÒ¾Ó¾ã¾Ð¾Ô¾Ï¾âôòÞä¾ÒñÕÜÚé§ï¸¾Ñè¢öÂ÷¶" },
	{ "ju", "ju2", 2, "¾Ö¾Õ½ÛéÙï¸" },
	{ "ju", "ju3", 3, "¾Ù¹ñ¾Ø¾×¾Úõáö´é·ÜìèÛ" },
	{ "ju", "ju4", 4, "¾Ý¾ä¾ß¾ç¾Þ¾Û¾Ü¾à¾ã¾å¾ÚöÄ¾â¾æôòì«¾áåáÙÆîÒêøåðé°ñÀÚªõ¶ÜÄ" },
	{ "juan", "juan1", 1, "¾èÈ¦¾ê¾éä¸ïÔîÃ" },
	{ "juan", "juan3", 3, "¾íïÃ" },
	{ "juan", "juan4", 4, "È¦¾í¾ë¾ìöÁ¾îáúèðÛ²" },
	{ "jue", "jue1", 1, "àµ¾ïàÙ" },
	{ "jue", "jue2", 2, "¾õ¾ø¾ö½Ç½Å½À¾ò¾÷áÈ¾ô¾ñ¾óâ±ØÊõê¾ðÚÜÛÇæÞéÓàåçåèöØãìßïãÞ§õû" },
	{ "jue", "jue3", 3, "õê" },
	{ "jue", "jue4", 4, "¾ó" },
	{ "jun", "jun1", 1, "¾ü¾ù¾ý¾ûóÞ¹ê¾úñä÷å" },
	{ "jun", "jun4", 4, "¿¡¾þöÁ¾ú¿¤¿¥¿¢ÞÜ¿£" },
	{ "ka", "ka1", 1, "¿§¿¦ßÇ" },
	{ "ka", "ka3", 3, "¿¨¿©ßÇØûëÌ" },
	{ "kai", "kai1", 1, "¿ª¿«ï´" },
	{ "kai", "kai3", 3, "¿®¿­îø¿¬âýÝÜØÜÛîïÇ" },
	{ "kai", "kai4", 4, "âé" },
	{ "kan", "kan1", 1, "¿´¿¯¿°¿±íèê¬" },
	{ "kan", "kan3", 3, "Ù©¿³¿²¼÷ãÛÝ¨" },
	{ "kan", "kan4", 4, "¿´Ç¶î«ãÛ" },
	{ "kang", "kang1", 1, "¿µ¿¶¿·ãÊ" },
	{ "kang", "kang2", 2, "¿¸" },
	{ "kang", "kang4", 4, "¿¹¿»¿ºØøãÊîÖ" },
	{ "kao", "kao1", 1, "åê" },
	{ "kao", "kao3", 3, "¿¼¿¾¿½èà" },
	{ "kao", "kao4", 4, "¿¿îíêû" },
	{ "ke", "ke1", 1, "¿Æ¿Å¿ÂºÇ¿Ã¿Á¿Ä¿Àà¾î§éðïýðâòòîÝñ½ò¤çæ÷Á" },
	{ "ke", "ke2", 2, "¿È¿Çò¤" },
	{ "ke", "ke3", 3, "¿É¿Ê¿Àéðá³" },
	{ "ke", "ke4", 4, "¿É¿Ë¿Í¿Ì¿Îã¡à¾äÛæìç¼ë´ï¾òÂ" },
	{ "ken", "ken3", 3, "¿Ï¿Ò¿Ð¿Ñö¸" },
	{ "ken", "ken4", 4, "ñÌ" },
	{ "keng", "keng1", 1, "¿Ó¿Ôï¬" },
	{ "kong", "kong1", 1, "¿ÕÙÅáÇóí" },
	{ "kong", "kong3", 3, "¿Ö¿×ÙÅ" },
	{ "kong", "kong4", 4, "¿Õ¿Ø" },
	{ "kou", "kou1", 1, "¿ÙÜÒíî" },
	{ "kou", "kou3", 3, "¿Ú" },
	{ "kou", "kou4", 4, "¿Û¿ÜßµÞ¢óØ" },
	{ "ku", "ku1", 1, "¿Þ¿Ý¿ß÷¼ØÚÜ¥" },
	{ "ku", "ku3", 3, "¿à" },
	{ "ku", "ku4", 4, "¿â¿ã¿áà·ç«" },
	{ "kua", "kua1", 1, "¿ä" },
	{ "kua", "kua3", 3, "¿åÙ¨" },
	{ "kua", "kua4", 4, "¿ç¿æ¿è" },
	{ "kuai", "kuai3", 3, "Øá" },
	{ "kuai", "kuai4", 4, "»á¿ì¿é¿êëÚßà¿ëáöä«Û¦" },
	{ "kuan", "kuan1", 1, "¿í÷Å" },
	{ "kuan", "kuan3", 3, "¿î" },
	{ "kuang", "kuang1", 1, "¿ò¿ð¿ïßÑÚ²" },
	{ "kuang", "kuang2", 2, "¿ñÚ¿" },
	{ "kuang", "kuang3", 3, "ÞÅ" },
	{ "kuang", "kuang4", 4, "¿ö¿ó¿ò¿õ¿ôÚ÷ÛÛæþêÜ" },
	{ "kui", "kui1", 1, "¿÷¿ú¿ø¿ùã¦" },
	{ "kui", "kui2", 2, "¿ýî¥åÓ¿û¿üØ¸Ùçà­ÚóêÒÞñòñ" },
	{ "kui", "kui3", 3, "¿þõÍ" },
	{ "kui", "kui4", 4, "À¢À£À¡ØÑà°ñùóñÝÞã´" },
	{ "kun", "kun1", 1, "À¥À¤öïï¿õ«çû÷Õ" },
	{ "kun", "kun3", 3, "À¦ã§ãÍ" },
	{ "kun", "kun4", 4, "À§" },
	{ "kuo", "kuo4", 4, "À¨ÊÊÀ«À©ÀªèéòÒ" },
	{ "la", "la1", 1, "À­À²À®À¬åå" },
	{ "la", "la2", 2, "À­À®ê¹íÇ" },
	{ "la", "la3", 3, "À­À®" },
	{ "la", "la4", 4, "ÂäÀ­À±À°À¯ØÝðø" },
	{ "la", "la5", 5, "À¶À²" },
	{ "lai", "lai2", 2, "À´À³áâäµáÁïª" },
	{ "lai", "lai4", 4, "Àµíùñ®ô¥êãäþ" },
	{ "lan", "lan2", 2, "À¼À¶À¸À¹ÀºÀ½À·á°ìµÀ»ñÜïçÀ¾" },
	{ "lan", "lan3", 3, "ÀÁÀÀÀ¿é­ÀÂäíî½" },
	{ "lan", "lan4", 4, "ÀÃÀÄ" },
	{ "lang", "lang1", 1, "à¥" },
	{ "lang", "lang2", 2, "ÀÇÀÉÀÈÀÅòëÀÆï¶ïüãÏ" },
	{ "lang", "lang3", 3, "ÀÊ" },
	{ "lang", "lang4", 4, "ÀËÀÉÝ¹ÝõãÏ" },
	{ "lao", "lao1", 1, "ÀÌ" },
	{ "lao", "lao2", 2, "ÀÍÀÎßëáÀï©ðìõ²" },
	{ "lao", "lao3", 3, "ÀÏÀÑÀÐÁÊèáîî" },
	{ "lao", "lao4", 4, "ÂäÂçßëÀÓÀÒÀÔñì" },
	{ "le", "le1", 1, "Àß" },
	{ "le", "le4", 4, "ÀÖÀÕØìß·ãî÷¦" },
	{ "le", "le5", 5, "ÁË" },
	{ "lei", "lei1", 1, "ÀÕÀÞ" },
	{ "lei", "lei2", 2, "ÀÛÀ×ÀÞÙúÀØæÐçÐéÛ" },
	{ "lei", "lei3", 3, "ÀÛÀÙÀÝÀÚÀÜÚ³ñç" },
	{ "lei", "lei4", 4, "ÀàÀáÀÛÀÞÀßõª" },
	{ "lei", "lei5", 5, "àÏ" },
	{ "leng", "leng1", 1, "Àâ" },
	{ "leng", "leng2", 2, "ÀãÀâÜ¨" },
	{ "leng", "leng3", 3, "Àä" },
	{ "leng", "leng4", 4, "ã¶" },
	{ "li", "li1", 1, "Á¨" },
	{ "li", "li2", 2, "ÀëÀöÀèÁ§ÀìÀêÀæÀéÀçÀåî¾Þ¼æêòÛ÷óçÊà¬ð¿æËó»öâÝñ" },
	{ "li", "li3", 3, "ÀïÀíÀîÀñÁ¨ÀðÙµåÎæ²ã¦å¢ï®ó»õ·÷¯" },
	{ "li", "li4", 4, "Á¦ÀûÁ¢ÀúÀýÀöÀøÀ÷ÀòóÒÁ£ÀþÀõÁ¥ÀôÁ¤ö¨Ý°ìåÙ³íÂÁ¡ÛªîºÀóèÀß¿à¦áûäàÀùèÝéöÀüÛÞÜÂðÝðßòÃØªóöôÏõÈÞº" },
	{ "li", "li5", 5, "Á§Á¨" },
	{ "lia", "lia3", 3, "Á©" },
	{ "lian", "lian2", 2, "ÁªÁ¬Á¯Á«Á®Á±Á°Á­ñÍå¥ì¡ÞÆó¹öã" },
	{ "lian", "lian3", 3, "Á³Á²çöÝüñÏ" },
	{ "lian", "lian4", 4, "Á·ÁµÁ¶Á´éçé¬äò" },
	{ "liang", "liang2", 2, "Á¿Á¼ÁºÁ¹Á¸Á»õÔÝ¹é£Ü®" },
	{ "liang", "liang3", 3, "Á½Á©÷Ë" },
	{ "liang", "liang4", 4, "Á¿ÁÁÁ¾Á¹ÁÂÁÀõÔö¦" },
	{ "liao", "liao1", 1, "ÁÃÁÌ" },
	{ "liao", "liao2", 2, "ÁÄÁÆÁÉÁÅÁÈÁÃÁÌçÔå¼ÁÇàÚâ²ðÓ" },
	{ "liao", "liao3", 3, "ÁËÁÊÁÇÞ¤îÉ" },
	{ "liao", "liao4", 4, "ÁËÁÏÁÎÁÍÁÃÁÌÞÍîÉ" },
	{ "lie", "lie1", 1, "ßÖ" },
	{ "lie", "lie3", 3, "ÁÑßÖ" },
	{ "lie", "lie4", 4, "ÁÐÁÒÁÑÁÓÁÔôóÙýä£ÞæÛøõñ÷à" },
	{ "lie", "lie5", 5, "ßÖ" },
	{ "lin", "lin2", 2, "ÁÖÁÙÃØÁÚÁÕÁÜÁØ÷ëÁÛÁ×á×ê¥ôÔåàßøî¬" },
	{ "lin", "lin3", 3, "ÁÝãÁéÝâÞ" },
	{ "lin", "lin4", 4, "ÁÜÁßõïÁÞÝþì¢" },
	{ "ling", "ling1", 1, "Áà" },
	{ "ling", "ling2", 2, "ÁîÁéÁãÁäÁèÁáÁåÁêÁæñöàòÀâÁâÜßôáèùê²ç±Û¹ãöÁçòÈèÚöì" },
	{ "ling", "ling3", 3, "ÁìÁîÁë" },
	{ "ling", "ling4", 4, "ÁîÁíßÊ" },
	{ "liu", "liu1", 1, "ÁïìÖ" },
	{ "liu", "liu2", 2, "ÁôÁ÷ÁõÁöÁñä¯ÁòÁðåÞÁóïÖì¼æòöÌ" },
	{ "liu", "liu3", 3, "Áøç¸ï³" },
	{ "liu", "liu4", 4, "ÁùÂ½ÁïÂµåÞÁóïÖðÒ" },
	{ "lo", "lo5", 5, "¿©" },
	{ "long", "long1", 1, "Â¡" },
	{ "long", "long2", 2, "ÁúÂ¡ÁýëÊÁüÁûççÁþÜ×èÐãñíÃñª" },
	{ "long", "long3", 3, "ÁýÂ£Â¢Â¤Ûâ" },
	{ "long", "long4", 4, "Åª" },
	{ "lou", "lou1", 1, "Â§" },
	{ "lou", "lou2", 2, "Â¥à¶ÙÍÂ¦÷Ãò÷Ýäñï" },
	{ "lou", "lou3", 3, "Â§Â¨áÐ" },
	{ "lou", "lou4", 4, "Â¶ÂªÂ©ïÎðü" },
	{ "lou", "lou5", 5, "à¶" },
	{ "lu", "lu1", 1, "ààß£" },
	{ "lu", "lu2", 2, "Â¬Â¯Â®Â«Â­ãòéñöÔÛäëÍðµôµèÓ" },
	{ "lu", "lu3", 3, "Â³Â«Â±Â²Â°éÖïå" },
	{ "lu", "lu4", 4, "ÁùÂ·Â½Â¼Â¶ÂÌÂ¹ÂµÂ»ê¤Â´Â¸äõÂ¾óüðØÂºè´éûäËÞ¤åÖ" },
	{ "lu", "lu5", 5, "éñëª" },
	{ "lv", "lv3", 3, "ÂÃÂÄÂÅÂÂÂÆÂÀÞÛÂÁÙÍñÚëöïù" },
	{ "lv", "lv4", 4, "ÂÉÂÌÂÊÂÇÂËÂÈ" },
	{ "lv", "lv5", 5, "Â¿éµãÌ" },
	{ "luan", "luan2", 2, "ÂÍÂÎÂÏèïöÇÂÐð½æ®Ùõ" },
	{ "luan", "luan3", 3, "ÂÑ" },
	{ "luan", "luan4", 4, "ÂÒ" },
	{ "lue", "lue3", 3, "ÂÓ" },
	{ "lue", "lue4", 4, "ÂÔÂÓï²" },
	{ "lun", "lun1", 1, "ÂÕ" },
	{ "lun", "lun2", 2, "ÂÛÂÖÂ×ÂÙÂØÂÕàðÂÚ" },
	{ "lun", "lun4", 4, "ÂÛ" },
	{ "luo", "luo1", 1, "ÂäÂÞÞÛ" },
	{ "luo", "luo2", 2, "ÂÞÂßÂÜÂÝÂàÂáÂââ¤é¡ëáïÝ" },
	{ "luo", "luo3", 3, "ÂãÙÀÙùñ§" },
	{ "luo", "luo4", 4, "ÂäÂçÂåÂæ¿©ÞûÀÓçóãøäðÜýíÑöÃ" },
	{ "luo", "luo5", 5, "ÂÞ" },
	{ "m", "m2", 2, "ß¼" },
	{ "ma", "ma1", 1, "ÂèÂéÄ¦Ä¨ÂìæÖ" },
	{ "ma", "ma2", 2, "ÂðÂéó¡" },
	{ "ma", "ma3", 3, "ÂíÂðÂëÂêÂìáï" },
	{ "ma", "ma4", 4, "ÂîÂìßéè¿" },
	{ "ma", "ma5", 5, "Ã´ÂðÂï" },
	{ "mai", "mai2", 2, "Âñö²" },
	{ "mai", "mai3", 3, "ÂòÝ¤" },
	{ "mai", "mai4", 4, "ÂôÂóÂõÂöÛ½" },
	{ "man", "man1", 1, "ò©" },
	{ "man", "man2", 2, "ÂñÂùÂøÂ÷ÂûÃ¡÷©÷´" },
	{ "man", "man3", 3, "Âúòý" },
	{ "man", "man4", 4, "ÂýÂþÂüÂûÃ¡Ü¬á£çÏì×ïÜ" },
	{ "mang", "mang2", 2, "Ã¦Ã£Ã¤Ã¢Ã¥ÚøíË" },
	{ "mang", "mang3", 3, "Ã§òþäÝ" },
	{ "mao", "mao1", 1, "Ã¨" },
	{ "mao", "mao2", 2, "Ã«Ã¨Ã¬Ã©÷ÖÃªêóì¸òúó±Üâ" },
	{ "mao", "mao3", 3, "Ã®Ã­á¹ã÷êÄ" },
	{ "mao", "mao4", 4, "Ã°Ã³Ã±Ã²Ã¯ë£è£í®Ùóî¦" },
	{ "me", "me5", 5, "Ã´÷á" },
	{ "mei", "mei2", 2, "Ã»Ã¼Ã·Ã½Ã¶ÃºÃ¹ÃµÃÓÃ¸Ý®áÒäØé¹â­ïÑðÌ" },
	{ "mei", "mei3", 3, "ÃÀÃ¿Ã¾ä¼" },
	{ "mei", "mei4", 4, "ÃÃ÷ÈÃÁÃÕÃÄÃÂñÇ" },
	{ "men", "men1", 1, "ÃÆ" },
	{ "men", "men2", 2, "ÃÅÞÑîÍ" },
	{ "men", "men4", 4, "ÃÆí¯ìË" },
	{ "men", "men5", 5, "ÃÇ" },
	{ "meng", "meng1", 1, "ÃÉ" },
	{ "meng", "meng2", 2, "ÃÉÃËëüÃ¥ÃÈÃÊÞ«Ýùíæòµô¿" },
	{ "meng", "meng3", 3, "ÃÉÃÍÛÂãÂó·òìÃÌô»" },
	{ "meng", "meng4", 4, "ÃÎÃÏ" },
	{ "mi", "mi1", 1, "ÃÐßä" },
	{ "mi", "mi2", 2, "ÃÔÃÖÃÕÃÒÃÓÃÑ÷çâ¨ìò÷ãÞÂ" },
	{ "mi", "mi3", 3, "Ã×ÃÐÃÒåôôÍëßØÂ" },
	{ "mi", "mi4", 4, "ÃÜÃØÃÙÃÛÚ×ÃÚãèåµÃÝà×ôé" },
	{ "mian", "mian2", 2, "ÃÞÃßÃà" },
	{ "mian", "mian3", 3, "ÃâÃåÃãëïÃáÃääÅäÏãæííö¼" },
	{ "mian", "mian4", 4, "Ãæ" },
	{ "miao", "miao1", 1, "ß÷" },
	{ "miao", "miao2", 2, "ÃèÃçÃéðÅ" },
	{ "miao", "miao3", 3, "ÃëÃìÃêç¿íµèÂåãíð" },
	{ "miao", "miao4", 4, "ÃîÃíçÑ" },
	{ "mie", "mie1", 1, "Ø¿ßã" },
	{ "mie", "mie4", 4, "ÃðÃïóúóº" },
	{ "min", "min2", 2, "ÃñçëáºçÅçäÜå" },
	{ "min", "min3", 3, "ÃôÃõÃöãýÃóÃòãÉíªö¼÷ª" },
	{ "ming", "ming2", 2, "ÃûÃ÷ÃùÃËÃúÚ¤Üøäéî¨êÔÃø" },
	{ "ming", "ming3", 3, "õ¤" },
	{ "ming", "ming4", 4, "Ãü" },
	{ "miu", "miu4", 4, "ÃýçÑ" },
	{ "mo", "mo1", 1, "Ãþ" },
	{ "mo", "mo2", 2, "ÎÞÄ£÷áÄ¥ÃþÄ¦Ä§Ä¤Ä¢âÉÄ¡ÚÓæÆ" },
	{ "mo", "mo3", 3, "Ä¨" },
	{ "mo", "mo4", 4, "Ã»ÍòÄ¬ÄªÄ©Ã°Ä¥Ä¯Ä®Ä«Ä¨Ä°ÂöºÙÄ­ÝëÜÔºÑï÷ïÒéâñ¢ñòõöõø" },
	{ "mou", "mou1", 1, "ßè" },
	{ "mou", "mou2", 2, "Ä±Ä²íøçÑöÊòÖÙ°" },
	{ "mou", "mou3", 3, "Ä³" },
	{ "mu", "mu2", 2, "Ä£ë¤" },
	{ "mu", "mu3", 3, "Ä¸Ä·ÀÑÄ¶Ä´Äµ" },
	{ "mu", "mu4", 4, "Ä¿Ä¾Ä»Ä½ÄÁÄ¹Ä¼ÄºÄ²ÄÂÄÀãåÛéÜÙØïîâ" },
	{ "n", "n2", 2, "àÅßí" },
	{ "n", "n3", 3, "àÅßí" },
	{ "n", "n4", 4, "àÅ" },
	{ "na", "na1", 1, "ÄÇÄÏ" },
	{ "na", "na2", 2, "ÄÃïÕ" },
	{ "na", "na3", 3, "ÄÇÄÄ" },
	{ "na", "na4", 4, "ÄÇÄØÄÉÄÈÄÅÞàÄÆëÇñÄ" },
	{ "na", "na5", 5, "ÄÄÄÅ" },
	{ "nai", "nai3", 3, "ÄÄÄËÄÌÄÊÜµ" },
	{ "nai", "nai4", 4, "ÄÎÄÍØ¾Ù¦ÝÁèÍ" },
	{ "nan", "nan1", 1, "àîàï" },
	{ "nan", "nan2", 2, "ÄÑÄÏÄÐéªà«" },
	{ "nan", "nan3", 3, "ëîòïôö" },
	{ "nan", "nan4", 4, "ÄÑ" },
	{ "nang", "nang1", 1, "ÄÒàì" },
	{ "nang", "nang2", 2, "ÄÒâÎ" },
	{ "nang", "nang3", 3, "âÎß­êÙ" },
	{ "nao", "nao1", 1, "Ø«" },
	{ "nao", "nao2", 2, "Å¬ÄÓßÎâ®îóíÐòÍ" },
	{ "nao", "nao3", 3, "ÄÔÄÕè§Ûñ" },
	{ "nao", "nao4", 4, "ÄÖÄ×" },
	{ "ne", "ne2", 2, "ÄÄ" },
	{ "ne", "ne4", 4, "ÄØÄÅÚ«" },
	{ "ne", "ne5", 5, "ÄØÄÅ" },
	{ "nei", "nei3", 3, "ÄÄÄÙ" },
	{ "nei", "nei4", 4, "ÄÇÄÚ" },
	{ "nen", "nen4", 4, "ÄÛí¥" },
	{ "neng", "neng2", 2, "ÄÜ" },
	{ "ng", "ng2", 2, "àÅßí" },
	{ "ng", "ng3", 3, "àÅßí" },
	{ "ng", "ng4", 4, "àÅ" },
	{ "ni", "ni1", 1, "ÄÝ" },
	{ "ni", "ni2", 2, "ÄØÄáÄàÄßÄÞÛèâ¥âõîêöò" },
	{ "ni", "ni3", 3, "ÄãÄâì»ìò" },
	{ "ni", "ni4", 4, "ÄàÄòÄæÄäÄåêÇÄçíþí«Ù£" },
	{ "nian", "nian1", 1, "ÄèÄé" },
	{ "nian", "nian2", 2, "ÄêÕ³ð¤öÓöó" },
	{ "nian", "nian3", 3, "ÄëÄíÄìéý" },
	{ "nian", "nian4", 4, "ÄîØ¥ÄðÛþ" },
	{ "niang", "niang2", 2, "ÄïÄð" },
	{ "niang", "niang4", 4, "Äð" },
	{ "niao", "niao3", 3, "ÄñôÁæÕÜà" },
	{ "niao", "niao4", 4, "ÄòÄçëå" },
	{ "nie", "nie1", 1, "Äó" },
	{ "nie", "nie4", 4, "ÄùÄôÄõõæà¿ÄöÄ÷ÄøØ¿Úíò¨ô«ÞÁ" },
	{ "nin", "nin2", 2, "Äúí¥" },
	{ "ning", "ning2", 2, "ÄþÄýÅ¡ßÌÄüÄûÜÑå¸ñ÷" },
	{ "ning", "ning3", 3, "Å¡" },
	{ "ning", "ning4", 4, "ÄþÅ¡Å¢Øú" },
	{ "niu", "niu1", 1, "æ¤" },
	{ "niu", "niu2", 2, "Å£" },
	{ "niu", "niu3", 3, "Å¦Å¤Å¥áðâî" },
	{ "niu", "niu4", 4, "ÞÖ" },
	{ "nong", "nong2", 2, "Å©Å¨Ù¯ßæÅ§" },
	{ "nong", "nong4", 4, "Åª" },
	{ "nou", "nou4", 4, "ññ" },
	{ "nu", "nu2", 2, "Å«æÛæå" },
	{ "nu", "nu3", 3, "Å¬åóæÀ" },
	{ "nu", "nu4", 4, "Å­" },
	{ "nv", "nv3", 3, "Å®îÏ" },
	{ "nv", "nv4", 4, "í¤ô¬" },
	{ "nuan", "nuan3", 3, "Å¯" },
	{ "nue", "nue4", 4, "Å°Å±" },
	{ "nuo", "nuo2", 2, "ÄÈÅ²ÙÐ" },
	{ "nuo", "nuo4", 4, "ÅµÅ³Å´ßöÞùï»" },
	{ "O", "o1", 1, "àÞà¸" },
	{ "O", "o2", 2, "Å¶" },
	{ "O", "o4", 4, "Å¶" },
	{ "Ou", "ou1", 1, "ÇøÅ·Å¹Å¸Ú©ê±Å½" },
	{ "Ou", "ou3", 3, "Å¼Å»Åºñî" },
	{ "Ou", "ou4", 4, "Å»Å½âæ" },
	{ "pa", "pa1", 1, "ÅÉ°ÇÅ¿Å¾Ýâ" },
	{ "pa", "pa2", 2, "ÅÀ°Ç°ÒèËîÙóá" },
	{ "pa", "pa4", 4, "ÅÂÅÁ" },
	{ "pa", "pa5", 5, "ÅÃ" },
	{ "pai", "pai1", 1, "ÅÄ" },
	{ "pai", "pai2", 2, "ÅÅÅÆÅÇÙ½" },
	{ "pai", "pai3", 3, "ÅÅÆÈ" },
	{ "pai", "pai4", 4, "ÅÉÅÈÝåßß" },
	{ "pan", "pan1", 1, "·¬ÅÊÅË°â" },
	{ "pan", "pan2", 2, "°ãÅÌÅÖÅÍõçãÝó´" },
	{ "pan", "pan4", 4, "ÅÐÅÎÅÑÅÏÞÕñáñÈãú" },
	{ "pang", "pang1", 1, "ÅÒ°òäè" },
	{ "pang", "pang2", 2, "ÅÔÅÓ°ò°õáÝó¦åÌ" },
	{ "pang", "pang3", 3, "ÅÕ" },
	{ "pang", "pang4", 4, "ÅÖ" },
	{ "pao", "pao1", 1, "ÅÚÅ×ÅÝëã" },
	{ "pao", "pao2", 2, "ÅÜÅÚÅÛÅÙÅØáóÞËâÒ" },
	{ "pao", "pao3", 3, "ÅÜ" },
	{ "pao", "pao4", 4, "ÅÚÅÝðå" },
	{ "pei", "pei1", 1, "ÅÞÅßõ¬" },
	{ "pei", "pei2", 2, "ÅãÅàÅâÅáïÂ" },
	{ "pei", "pei4", 4, "ÅäÅåÅæàÎàúì·ö¬" },
	{ "pen", "pen1", 1, "Åç" },
	{ "pen", "pen2", 2, "ÅèäÔ" },
	{ "pen", "pen4", 4, "Åç" },
	{ "peng", "peng1", 1, "ÅëÅêÅéÅìâñàØ" },
	{ "peng", "peng2", 2, "ÅóÅôÅíÅïÅîÅòÅñÅìÅðÜ¡ó²" },
	{ "peng", "peng3", 3, "Åõ" },
	{ "peng", "peng4", 4, "Åö" },
	{ "pi", "pi1", 1, "Åú»µÅû±ÙÅüÅ÷ÅùàèØ§ç¢ÅøÚüîë" },
	{ "pi", "pi2", 2, "Æ¤Æ£Æ¡Æ¢ÅýÅþÛ¯Ü±ñÔÛýÚðÜÅèÁî¼îëÚéò·òçõù" },
	{ "pi", "pi3", 3, "·ñÆ¥ÅüÆ¦ñ±ÛÜß¨ßÁâÏØòñâ" },
	{ "pi", "pi4", 4, "Æ¨±ÙÆ§Æ©æÇäÄê¶î¢" },
	{ "pian", "pian1", 1, "Æ¬ÆªÆ«ôæ±âêú" },
	{ "pian", "pian2", 2, "±ãõäçÂëÝæé" },
	{ "pian", "pian3", 3, "ÚÒ" },
	{ "pian", "pian4", 4, "Æ¬Æ­" },
	{ "piao", "piao1", 1, "Æ¯Æ®ØâçÎóª" },
	{ "piao", "piao2", 2, "ÆÓÆ°æÎ" },
	{ "piao", "piao3", 3, "Æ¯î©çÎéèÝ³" },
	{ "piao", "piao4", 4, "Æ±Æ¯æôàÑ" },
	{ "pie", "pie1", 1, "Æ²Æ³ë­" },
	{ "pie", "pie3", 3, "Æ²Ø¯ÜÖ" },
	{ "pin", "pin1", 1, "Æ´ÞÕæ°" },
	{ "pin", "pin2", 2, "Æ¶ÆµæÉò­" },
	{ "pin", "pin3", 3, "Æ·é¯" },
	{ "pin", "pin4", 4, "Æ¸êò" },
	{ "ping", "ping1", 1, "Æ¹æ³Ù·" },
	{ "ping", "ping2", 2, "Æ½ÆÀÆ¿Æ¾Æ¼ÆÁ·ëÆ»ÆºèÒöÒ" },
	{ "po", "po1", 1, "ÆÄÆÂ²´ÆÓÆÃÚéãøê·îÇ" },
	{ "po", "po2", 2, "·±ÆÅÛ¶ð«" },
	{ "po", "po3", 3, "ØÏîÞóÍ" },
	{ "po", "po4", 4, "ÆÆÆÈÆÓÆÇÆÉçê" },
	{ "pou", "pou1", 1, "ÆÊ" },
	{ "pou", "pou2", 2, "ÙöÞå" },
	{ "pou", "pou3", 3, "Þå" },
	{ "pu", "pu1", 1, "ÆÌÆËÆÍàÛ" },
	{ "pu", "pu2", 2, "ÆÏÆÑÆÍ¸¬ÆÐÙéè±å§ÆÎïä" },
	{ "pu", "pu3", 3, "ÆÕ±¤ÆÓÆ×ÆÖäßÆÒÆÔë«ïèõë" },
	{ "pu", "pu4", 4, "±©ÆÌ±¤ÆØÆÙ" },
	{ "qi", "qi1", 1, "ÆÚÆßÆÞÆÛ¼©ÆÝÆàÆáÆÜÆãõèàÒÝÂéÊÆâì¥èç" },
	{ "qi", "qi2", 2, "ÆäÆæÆåÆëÆìÆïÆçç÷ÆíÆêì÷ÆîÆéçùä¿áªÜùÙ¹êÈÜÎñýÛßæëÆèØÁÝ½Þ­ÆèòÓòàôë÷¢÷è" },
	{ "qi", "qi3", 3, "ÆðÆóÆôÆñÆò»üç²è½Ü»á¨ôì" },
	{ "qi", "qi4", 4, "ÆøÆÞÆ÷ÆûÆëÆúÆüÆõÆùÆöí¬ãàØ½ÆýÝÝíÓ" },
	{ "qia", "qia1", 1, "ÆþÙ¤ÝÖñÊ" },
	{ "qia", "qia3", 3, "¿¨" },
	{ "qia", "qia4", 4, "Ç¡Ç¢÷Ä" },
	{ "qian", "qian1", 1, "Ç§Ç©Ç£Ç¨Ç«Ç¦å¹ã¥Ü·í©ÚäÇªá©Ç¤ÙÝåºå½Ç¥" },
	{ "qian", "qian2", 2, "Ç°Ç®Ç±Ç¬ò¯Ç¯ÞçÇ­Ý¡îÔêùóé÷Ü" },
	{ "qian", "qian3", 3, "Ç³Ç²Ç´ç×ëÉ" },
	{ "qian", "qian4", 4, "Ç·Ç¸ÏËÇ¶Ù»ÇµÜçÜÍã»èý" },
	{ "qiang", "qiang1", 1, "½«Ç¹ÇÀÇ»ÇºïÏõÄÇ¼ãÞê¨ïêòÞïº" },
	{ "qiang", "qiang2", 2, "Ç¿Ç½Ç¾éÉæÍ" },
	{ "qiang", "qiang3", 3, "Ç¿ÇÀñßïêôÇ" },
	{ "qiang", "qiang4", 4, "ÇºõÄìÁê¨" },
	{ "qiao", "qiao1", 1, "ÇÄÇÃÈ¸ÇÂõÎÇÁçØíÍØä" },
	{ "qiao", "qiao2", 2, "ÇÅÇÇÇÈÇÆÇÌ½¶ã¾éÔá½ÚÛÜñ÷³" },
	{ "qiao", "qiao3", 3, "ÇÄÇÉÈ¸ã¸" },
	{ "qiao", "qiao4", 4, "ÇÌÇÎÇÏ¿ÇÇÍÇËÇÊÚ½ÚÛ" },
	{ "qie", "qie1", 1, "ÇÐ" },
	{ "qie", "qie2", 2, "ÇÑÙ¤" },
	{ "qie", "qie3", 3, "ÇÒ" },
	{ "qie", "qie4", 4, "ÇÐÇÔÇÓôòæªÆöã«ïÆêüÛ§óæã»" },
	{ "qin", "qin1", 1, "Ç×ÇÕÇÖôÀ" },
	{ "qin", "qin2", 2, "ÇÙÇØÇÚÇÛÇÜñæñûÇÝàßâÛäÚéÕï·àºÜËòû" },
	{ "qin", "qin3", 3, "ÇÞ" },
	{ "qin", "qin4", 4, "ÇßÞìßÄ" },
	{ "qing", "qing1", 1, "ÇàÇåÇáÇãÇäÇâòßàõöë" },
	{ "qing", "qing2", 2, "ÇéÇçÇæÇèéÑ÷ô" },
	{ "qing", "qing3", 3, "ÇëÇêö¥ÜÜ" },
	{ "qing", "qing4", 4, "Ç×ÇìóÀíàóäôì" },
	{ "qiong", "qiong2", 2, "ÇîÇíñ·ÜäÚöòËóÌõ¼öÆ" },
	{ "qiu", "qiu1", 1, "ÇïÇñÇð¹êòÇöúé±äÐ" },
	{ "qiu", "qiu2", 2, "ÇóÇò³ðÇôÇõôÃò°Ù´åÙêäÇöåÏáìòøÛÏ÷ü" },
	{ "qiu", "qiu3", 3, "ôÜ" },
	{ "qu", "qu1", 1, "ÇøÇúÇüÇ÷ÇýÇûêïá«òÐìîÇùôðÚ°÷ñ" },
	{ "qu", "qu2", 2, "ÇþöÄáéñ³Û¾è³ë¬ëÔíáð¶Þ¡ó½Þ¾" },
	{ "qu", "qu3", 3, "È¡ÇúÈ¢È£ÜÄ" },
	{ "qu", "qu4", 4, "È¥È¤êïãÖ" },
	{ "qu", "qu5", 5, "Ðç" },
	{ "quan", "quan1", 1, "È¦ãª" },
	{ "quan", "quan2", 2, "È«È¨ÈªÈ­Ú¹È§òéÜõîýÈ¬È©éúóÜ÷Ü" },
	{ "quan", "quan3", 3, "È®ç¹î°" },
	{ "quan", "quan4", 4, "È°È¯" },
	{ "que", "que1", 1, "È±ãÚÈ²" },
	{ "que", "que2", 2, "È³" },
	{ "que", "que4", 4, "È´È·È¸È¶Èµã×ãÚí¨" },
	{ "qun", "qun1", 1, "åÒ" },
	{ "qun", "qun2", 2, "ÈºÈ¹÷å" },
	{ "ran", "ran2", 2, "È»È¼÷×òÅ" },
	{ "ran", "ran3", 3, "È¾È½ÜÛ" },
	{ "rang", "rang1", 1, "ÈÂ" },
	{ "rang", "rang2", 2, "È¿ìüð¦" },
	{ "rang", "rang3", 3, "ÈÂÈÁÈÀìü" },
	{ "rang", "rang4", 4, "ÈÃ" },
	{ "rao", "rao2", 2, "ÈÄæ¬èãÜé" },
	{ "rao", "rao3", 3, "ÈÅÈÆæ¬" },
	{ "rao", "rao4", 4, "ÈÆ" },
	{ "re", "re3", 3, "ÈôÈÇßö" },
	{ "re", "re4", 4, "ÈÈ" },
	{ "ren", "ren2", 2, "ÈËÈÎÈÊÈÉ" },
	{ "ren", "ren3", 3, "ÈÌïþÜó" },
	{ "ren", "ren4", 4, "ÈÎÈÏÈÍÈÐÈÒâ¿ØðÝØÈÑéíñÅ" },
	{ "reng", "reng1", 1, "ÈÓ" },
	{ "reng", "reng2", 2, "ÈÔ" },
	{ "ri", "ri4", 4, "ÈÕ" },
	{ "rong", "rong2", 2, "ÈÝÈÙÈÚÈØÈÜÈÞÈÛéÅÈÖáÉÈ×áõëÀòî" },
	{ "rong", "rong3", 3, "Èß" },
	{ "rou", "rou2", 2, "ÈáÈàõåôÛ÷·" },
	{ "rou", "rou4", 4, "Èâ" },
	{ "ru", "ru2", 2, "ÈçÈåÈãàéå¦ÈæÈäÞ¸ï¨ñàò¬" },
	{ "ru", "ru3", 3, "ÈèÈéÈê" },
	{ "ru", "ru4", 4, "ÈëÈìçÈä²äáÝê" },
	{ "ruan", "ruan3", 3, "ÈíÈîëÃ" },
	{ "rui", "rui2", 2, "Þ¨" },
	{ "rui", "rui3", 3, "Èï" },
	{ "rui", "rui4", 4, "ÈðÈñÜÇî£èÄò¸" },
	{ "run", "run4", 4, "ÈóÈò" },
	{ "ruo", "ruo4", 4, "ÈôÈõÙ¼óè" },
	{ "sa", "sa1", 1, "ÈöØíêý" },
	{ "sa", "sa3", 3, "È÷Èö" },
	{ "sa", "sa4", 4, "ÈøØ¦ìªëÛ" },
	{ "sai", "sai1", 1, "Ë¼ÈûÈùÈúàç" },
	{ "sai", "sai4", 4, "ÈüÈû" },
	{ "san", "san1", 1, "ÈýÈþë§" },
	{ "san", "san3", 3, "É¢É¡âÌôÖö±" },
	{ "san", "san4", 4, "É¢" },
	{ "sang", "sang1", 1, "É¥É£" },
	{ "sang", "sang3", 3, "É¤Þúíßòª" },
	{ "sang", "sang4", 4, "É¥" },
	{ "sao", "sao1", 1, "É§É¦ëýçØçÒöþ" },
	{ "sao", "sao3", 3, "É¨É©" },
	{ "sao", "sao4", 4, "É¨ÉÒëýÜ£ðþ" },
	{ "se", "se4", 4, "É«ÈûÉ¬ÉªØÄï¤ð£" },
	{ "sen", "sen1", 1, "É­" },
	{ "seng", "seng1", 1, "É®" },
	{ "sha", "sha1", 1, "É±É³É²É´É¼É¯É·É°êýöèððôÄï¡" },
	{ "sha", "sha3", 3, "Éµ" },
	{ "sha", "sha4", 4, "É³É¶ÏÃÉ·ö®àÄì¦ßþ" },
	{ "shai", "shai1", 1, "É¸õ§" },
	{ "shai", "shai3", 3, "É«" },
	{ "shai", "shai4", 4, "É¹" },
	{ "shan", "shan1", 1, "É½ÉÀÉ¾É¿ÉÈÉºÉ¼Õ¤õÇæ©äúëþÜÏÛïîÌô®É»÷Ô" },
	{ "shan", "shan3", 3, "ÉÁÉÂ²ôµ§" },
	{ "shan", "shan4", 4, "µ¥ÉÆÉÈìøÉÃÉÅÚ¨ÉÇÉÄÉÉæÓµ§æóØßÉ»Û·îÌðÞóµ÷­" },
	{ "shang", "shang1", 1, "ÉÌÉËÌÀéäõüìØÉÊ" },
	{ "shang", "shang3", 3, "ÉÏÉÍÉÎÛð" },
	{ "shang", "shang4", 4, "ÉÏÉÐç´" },
	{ "shang", "shang5", 5, "ÉÑ" },
	{ "shao", "shao1", 1, "ÉÕÉÔÉÒÉÓÇÊòÙóâô¹" },
	{ "shao", "shao2", 2, "É×ÉØÜæè¼ÉÖ" },
	{ "shao", "shao3", 3, "ÉÙ" },
	{ "shao", "shao4", 4, "ÉÙÉÜÕÙÉÔÉÚÉÛÉÓäûÛ¿" },
	{ "she", "she1", 1, "ÉÝÉÞâ¦î´" },
	{ "she", "she2", 2, "ÕÛÉàÉßÙÜ" },
	{ "she", "she3", 3, "Éá" },
	{ "she", "she4", 4, "ÉçÉèÉáÉæÉäÉãÉâÉå÷êäÜì¨ØÇ" },
	{ "shei", "shei2", 2, "Ë­" },
	{ "shen", "shen1", 1, "ÉíÉî²ÎÉêÉìÉðÉëÝ·ÉïÚ·ÉéôÖ" },
	{ "shen", "shen2", 2, "Ê²ÉñÉõ" },
	{ "shen", "shen3", 3, "ÉóÉòÉôÚÅßÓäÉïò" },
	{ "shen", "shen4", 4, "ÉõÉ÷ÉøÉöò×ÝØëÏé©" },
	{ "sheng", "sheng1", 1, "ÉúÉùÊ¤ÉýÉüÉûóÏ" },
	{ "sheng", "sheng2", 2, "ÉþäÅ" },
	{ "sheng", "sheng3", 3, "Ê¡íò" },
	{ "sheng", "sheng4", 4, "Ê¤Ê¥Ê¢³ËÊ£áÓêÉ" },
	{ "shi", "shi1", 1, "Ê¦Ê«Ê§Ê©Ê¬ÊªÊ¨ÐêÊ­Ýéõ§öõ" },
	{ "shi", "shi2", 2, "Ê±Ê®ÊµÊ²Ê¶Ê³Ê¯Ê°Ê´ÛõÝªìÂöå" },
	{ "shi", "shi3", 3, "Ê¹Ê¼Ê·Ê»ÊºÊ¸õ¹" },
	{ "shi", "shi4", 4, "ÊÇÊÂÊÀÊÐÊ¿Ê½ÊÓËÆÊ¾ÊÒÊÆÊÔÊÍÊÊÊÏÊÎÊÅÊÄÊÈÊÌÖÅÊËÊÑÊÁéøÊÃÊÉß±ÚÖÝªêÛîæó§óÂóß" },
	{ "shi", "shi5", 5, "Ö³³×" },
	{ "shou", "shou1", 1, "ÊÕ" },
	{ "shou", "shou2", 2, "Êì" },
	{ "shou", "shou3", 3, "ÊÖÊ×ÊØô¼" },
	{ "shou", "shou4", 4, "ÊÜÊÚÊÛÊÝÊÙÊÞá÷ç·" },
	{ "shu", "shu1", 1, "ÊéÊäÊâÊæÊåÊèÊãÊçÊáÊàÊßÙ¿ÝÄÞóæ­ç£ë¨ì¯ñâ" },
	{ "shu", "shu2", 2, "ÊìÊëÊêÛÓïø" },
	{ "shu", "shu3", 3, "ÊýÊôÊðÊóÊíÊîÊñÊòÊï" },
	{ "shu", "shu4", 4, "ÊýÊõÊ÷ÊöÊøÊúË¡ÊûÊþÓáÊùÊüäøãðØ­ëò" },
	{ "shua", "shua1", 1, "Ë¢à§" },
	{ "shua", "shua3", 3, "Ë£" },
	{ "shua", "shua4", 4, "Ë¢" },
	{ "shuai", "shuai1", 1, "Ë¥Ë¤" },
	{ "shuai", "shuai3", 3, "Ë¦" },
	{ "shuai", "shuai4", 4, "ÂÊË§ó°" },
	{ "shuan", "shuan1", 1, "Ë¨Ë©ãÅ" },
	{ "shuan", "shuan4", 4, "äÌ" },
	{ "shuang", "shuang1", 1, "Ë«Ëªæ×ãñ" },
	{ "shuang", "shuang3", 3, "Ë¬" },
	{ "shui", "shui2", 2, "Ë­" },
	{ "shui", "shui3", 3, "Ë®" },
	{ "shui", "shui4", 4, "ËµË°Ë¯" },
	{ "shun", "shun3", 3, "Ë±" },
	{ "shun", "shun4", 4, "Ë³Ë´Ë²" },
	{ "shuo", "shuo1", 1, "Ëµ" },
	{ "shuo", "shuo4", 4, "ÊýË·Ë¶Ë¸îååùÝôéÃÞ÷" },
	{ "si", "si1", 1, "Ë¼Ë¹Ë¾Ë½Ë¿ËºØËË»ð¸ßÐäùçÁïÈÛÌòÏ" },
	{ "si", "si3", 3, "ËÀ" },
	{ "si", "si4", 4, "ËÄËÆÊ³ËÂËÁËÅËÇËÃËÈìëæáãôÙ¹ãáÙîæ¦ñêóÓ" },
	{ "si", "si5", 5, "²Þ" },
	{ "song", "song1", 1, "ËÉâìäÁáÂáÔÚ¡Ý¿" },
	{ "song", "song3", 3, "ËÊã¤ËËñµ" },
	{ "song", "song4", 4, "ËÍËÎËÐËÌËÏ" },
	{ "sou", "sou1", 1, "ËÑËÒâÈà²äÑì¬ïËòô" },
	{ "sou", "sou3", 3, "ËÓÛÅÞ´àÕî¤" },
	{ "sou", "sou4", 4, "ËÔËÓ" },
	{ "su", "su1", 1, "ËÕöÕËÖ" },
	{ "su", "su2", 2, "Ë×" },
	{ "su", "su4", 4, "ËßËÙËØËàËÞËõËÜËÝËÚóùÙíà¼ÚÕËÛãºä³Ýøö¢" },
	{ "suan", "suan1", 1, "Ëáâ¡" },
	{ "suan", "suan4", 4, "ËãËâ" },
	{ "sui", "sui1", 1, "ËäÄòÝ´î¡íõå¡" },
	{ "sui", "sui2", 2, "ËæËìËåËç" },
	{ "sui", "sui3", 3, "Ëè" },
	{ "sui", "sui4", 4, "ËêËéËìËîËíåäËëìÝÚÇ" },
	{ "sun", "sun1", 1, "ËïÝ¥áøâ¸" },
	{ "sun", "sun3", 3, "ËðËñé¾öÀ" },
	{ "suo", "suo1", 1, "ËõÉ¯ËóàÂËôêýæ¶íüèøàÊËòôÈ" },
	{ "suo", "suo3", 3, "ËùË÷ËøËößï" },
	{ "ta", "ta1", 1, "ËûËýËüÌ¤ËúåÝäâîèõÁ" },
	{ "ta", "ta3", 3, "Ëþ÷£Ì¡" },
	{ "ta", "ta4", 4, "Ì¤ÍØé½àªÌ£í³Ì¢ãËäð" },
	{ "tai", "tai1", 1, "Ì¨Ì¥Ì¦" },
	{ "tai", "tai2", 2, "Ì¨Ì§Ì¦Û¢Þ·ææìÆõÌöØ" },
	{ "tai", "tai3", 3, "ß¾" },
	{ "tai", "tai4", 4, "Ì«Ì¬Ì©Ì­ÌªëÄîÑ" },
	{ "tan", "tan1", 1, "Ì¯Ì°Ì²Ì±Ì®" },
	{ "tan", "tan2", 2, "Ì¸µ¯Ì³Ì·Ì¶ñûÌµå£Ì´ê¼ïÄïâÛ°" },
	{ "tan", "tan3", 3, "Ì¹ÌºìþÌ»îã" },
	{ "tan", "tan4", 4, "Ì½Ì¾Ì¿Ì¼" },
	{ "tang", "tang1", 1, "ÌÀÌËï¦ïÛñíôÊ" },
	{ "tang", "tang2", 2, "ÌÃÌÆÌÇÌÅÌÁÌÄÌÂäçó«è©éÌïÛó¥â¼õ±" },
	{ "tang", "tang3", 3, "ÌÉÌÈÌÊÙÎàû" },
	{ "tang", "tang4", 4, "ÌËÌÌ" },
	{ "tao", "tao1", 1, "ÌÎÌÍÌÏß¶ìâèº÷ÒÌÐ" },
	{ "tao", "tao2", 2, "ÌÓÌÕÌÒÌÔÌÑßûä¬Ø»" },
	{ "tao", "tao3", 3, "ÌÖ" },
	{ "tao", "tao4", 4, "Ì×" },
	{ "te", "te4", 4, "ÌØìýß¯í«ï«" },
	{ "tei", "tei1", 1, "ß¯" },
	{ "teng", "teng2", 2, "ÌÚÌÛÌÙÌÜëø" },
	{ "ti", "ti1", 1, "ÌåÌßÌÝÌÞÌà" },
	{ "ti", "ti2", 2, "ÌáÌâÌäÌãõ®ç°ç¾ðÃÜè" },
	{ "ti", "ti3", 3, "Ìå" },
	{ "ti", "ti4", 4, "ÌæÌéÌêÌèÌëÌçã©ÙÃåÑç°ñÓ" },
	{ "tian", "tian1", 1, "ÌìÌí" },
	{ "tian", "tian2", 2, "ÌïÌîÌðÌñµèãÙî±îä" },
	{ "tian", "tian3", 3, "ÌóÌòãÃéå" },
	{ "tian", "tian4", 4, "ÞÝ" },
	{ "tiao", "tiao1", 1, "ÌôÙ¬ìö" },
	{ "tiao", "tiao2", 2, "Ìõµ÷ÌööæÜæ÷Øö¶òèóÔ" },
	{ "tiao", "tiao3", 3, "Ìôñ»" },
	{ "tiao", "tiao4", 4, "ÌøÌ÷ôÐ" },
	{ "tie", "tie1", 1, "ÌùÌûÝÆ" },
	{ "tie", "tie3", 3, "ÌúÌû" },
	{ "tie", "tie4", 4, "Ìû÷Ñ" },
	{ "ting", "ting1", 1, "ÌýÌüÍ¡Ìþ" },
	{ "ting", "ting2", 2, "Í£Í¥Í¤æÃÍ¢öªòÑÝãÜð" },
	{ "ting", "ting3", 3, "Í¦Í§î®îúèè" },
	{ "ting", "ting4", 4, "èè" },
	{ "tong", "tong1", 1, "Í¨¶²àÌ" },
	{ "tong", "tong2", 2, "Í¬Í¯Í®Í­Í©Í«Ù¡Íª¶±ÙÚÛíÜíá¼äüíÅ" },
	{ "tong", "tong3", 3, "Í³Í²Í°Í±¶±" },
	{ "tong", "tong4", 4, "Í¬Í¨Í´âú" },
	{ "tou", "tou1", 1, "Íµ" },
	{ "tou", "tou2", 2, "Í·Í¶÷»" },
	{ "tou", "tou3", 3, "î×" },
	{ "tou", "tou4", 4, "Í¸" },
	{ "tu", "tu1", 1, "Í»ÍºÍ¹" },
	{ "tu", "tu2", 2, "Í¼Í¾Í½ÍÀÍ¿Ý±ÝËõ©" },
	{ "tu", "tu3", 3, "ÍÁÍÂîÊ" },
	{ "tu", "tu4", 4, "ÍÂÍÃÜ¢ÝË" },
	{ "tuan", "tuan1", 1, "ÍÄ" },
	{ "tuan", "tuan2", 2, "ÍÅÞÒ" },
	{ "tuan", "tuan3", 3, "î¶" },
	{ "tuan", "tuan4", 4, "åè" },
	{ "tui", "tui1", 1, "ÍÆß¯" },
	{ "tui", "tui2", 2, "ÍÇ" },
	{ "tui", "tui3", 3, "ÍÈ" },
	{ "tui", "tui4", 4, "ÍËÍÊÍÉìÕ" },
	{ "tun", "tun1", 1, "ÍÌêÕ" },
	{ "tun", "tun2", 2, "ÍÍâ½ÍÎ¶Úëà" },
	{ "tun", "tun3", 3, "ÙÛ" },
	{ "tun", "tun4", 4, "ÍÊ" },
	{ "tuo", "tuo1", 1, "ÍÐÍÑÍÏØ±" },
	{ "tuo", "tuo2", 2, "ÍÓ¶æÍÕíÈÍÔãûõÉÛçÍÒéÒÙ¢îèõ¢èÞö¾" },
	{ "tuo", "tuo3", 3, "Í×ÍÖâÕ" },
	{ "tuo", "tuo4", 4, "ÆÇÍØÍÙèØóê" },
	{ "wa", "wa1", 1, "ÍÚÍÛ°¼æ´ÍÜÍÝ" },
	{ "wa", "wa2", 2, "ÍÞ" },
	{ "wa", "wa3", 3, "ÍßØô" },
	{ "wa", "wa4", 4, "ÍßÍàëð" },
	{ "wa", "wa5", 5, "ÍÛ" },
	{ "wai", "wai1", 1, "Íá" },
	{ "wai", "wai3", 3, "áË" },
	{ "wai", "wai4", 4, "Íâ" },
	{ "wan", "wan1", 1, "ÍåÍäòêØàÍã" },
	{ "wan", "wan2", 2, "ÍêÍæÍçÍèæýÜ¹Íé" },
	{ "wan", "wan3", 3, "ÍíÍëÍìÍñÍïÍðÝ¸ÃäîµÍîçºçþëäÝÒ" },
	{ "wan", "wan4", 4, "ÍòÍóÂû" },
	{ "wang", "wang1", 1, "ÍôÞÌ" },
	{ "wang", "wang2", 2, "ÍõÍüÍöÃ¢" },
	{ "wang", "wang3", 3, "ÍùÍøÍ÷ã¯Øèéþ÷Í" },
	{ "wang", "wang4", 4, "ÍûÍõÍùÍüÍúÍý" },
	{ "wei", "wei1", 1, "Î¯ÍþÎ¢Î£Î¡Î®ÙËÞ±åÔìÐáËÝÚÚñ" },
	{ "wei", "wei2", 2, "ÎªÎ¬Î§Î¨Î¥Î¤Î©á¡àøÛ×àíÎ«Î¦áÍãÇãíä¶" },
	{ "wei", "wei3", 3, "Î¯Î°Î¨Î²çâÎ±ì¿Î³Î®æ¸Î­â«ðôè¸ä¢ÚóÚÃôºöÛ" },
	{ "wei", "wei4", 4, "ÎªÎ»Î´Î¶ÎÀÎ½ÒÅÎ¿ÎºÎµÎ·Î¸Î¹Î¾Î¼â¬ê¦" },
	{ "wen", "wen1", 1, "ÎÂÎÁ" },
	{ "wen", "wen2", 2, "ÎÄÎÅÎÆÎÃö©è·ãÓ" },
	{ "wen", "wen3", 3, "ÎÈÎÇÎÉØØ" },
	{ "wen", "wen4", 4, "ÎÊÎÆãëè·" },
	{ "weng", "weng1", 1, "ÎÌÎË" },
	{ "weng", "weng3", 3, "Ýî" },
	{ "weng", "weng4", 4, "ÎÍÞ³" },
	{ "wo", "wo1", 1, "ÎÑÎÐÎÏà¸ÙÁÎÎÝ«" },
	{ "wo", "wo2", 2, "Å¶" },
	{ "wo", "wo3", 3, "ÎÒ" },
	{ "wo", "wo4", 4, "ÎÕÎÔÅ¶ä×ÎÖÎÓá¢ë¿íÒö»" },
	{ "wu", "wu1", 1, "ì¶¶ñÎÝÎÛÎÚÎ×ÎØÎÜØ£ÎÙÚùÛØ" },
	{ "wu", "wu2", 2, "ÎÞÍöÎâÎáÎæÎãÎàßíÎßä´òÚ÷ù" },
	{ "wu", "wu3", 3, "ÎåÎäÎçÎèÎéÎêÎæåüâèðÄêõåÃâÐâäØõ" },
	{ "wu", "wu4", 4, "ÎïÎñÎó¶ñÎòÎÚÎíÎðÎëÎìØ£ÎîðÍðíå»æðÜÌè»ìÉÚãæÄöÈ" },
	{ "xi", "xi1", 1, "Î÷Ï¢Ï£ÎüÏ§Ï¡Ï¤ÎöÏ¦ÎþÀ°ÎôÎõÙâÏªÎûÎýÎúéØÏ¨Ï¥ÆÜÛ­Ï¬êØÞÉôËßñõèäÀðªÏ«æÒÜçìäÏ©ôâó¬ì¨ä»ÙÒñ¶òáó£Ý¾ô¸ÎùôÑÎøõµì¤÷û" },
	{ "xi", "xi2", 2, "Ï¯Ï°Ï®Ï±Ï­Úôêê" },
	{ "xi", "xi3", 3, "Ï²Ï´ìûáãçôåïÝßÝûÏ³" },
	{ "xi", "xi4", 4, "ÏµÏ¸Ï·Ï¶â¾ãÒìùôª" },
	{ "xia", "xia1", 1, "Ï¹ÏºßÈ" },
	{ "xia", "xia2", 2, "Ï¿ÏÀÏÁÏ¼Ï¾Ï½åÚÏ»÷ïè¦áòíÌðýèÔ" },
	{ "xia", "xia4", 4, "ÏÂÏÄÏÅÏÃ»£óÁ" },
	{ "xian", "xian1", 1, "ÏÈÏÊÏÉÏÆÏËåßÝ²ÏÇë¯ììôÌõ£õÑ" },
	{ "xian", "xian2", 2, "ÏÐÏÍÏÓÏÌÏÒæµÏÎÏÑÏÏðÂðï" },
	{ "xian", "xian3", 3, "ÏÔÏÕÏÊÏ´õÐáýÞºÏ³ìÞò¹óÚÙþ" },
	{ "xian", "xian4", 4, "ÏÖ¼ûÏßÏÞÏØÏ×ÏÜÏÝÏÛÏÚÏÙá­ÜÈö±" },
	{ "xiang", "xiang1", 1, "ÏàÏãÏçÏäÏáÏæÏâÏåæøÝÙÜ¼ç½" },
	{ "xiang", "xiang2", 2, "½µÏêÏéÏèâÔ" },
	{ "xiang", "xiang3", 3, "ÏëÏìÏí÷ÏâÃöß" },
	{ "xiang", "xiang4", 4, "ÏàÏòÏóÏñÏîÏïÏðó­" },
	{ "xiao", "xiao1", 1, "ÏûÏúäìÐ¤ÏôÏüÏ÷ÏùåÐÏõÏöÏøèÉæçóïèÕßØòÙç¯÷Ì" },
	{ "xiao", "xiao2", 2, "ÏýáÅ" },
	{ "xiao", "xiao3", 3, "Ð¡Ïþóã" },
	{ "xiao", "xiao4", 4, "Ð¦Ð£Ð§Ð¤Ð¢Ð¥" },
	{ "xie", "xie1", 1, "Ð©ÐªÐ¨Ð«" },
	{ "xie", "xie2", 2, "Ò¶Ð­Ð¬Ð¯Ð±Ð²Ð³Ð°Ð®ÙÉß¢ÛÄò¡çÓ" },
	{ "xie", "xie3", 3, "Ð´Ñª" },
	{ "xie", "xie4", 4, "Ð´½âÐ»Ð¹ÆõÐµÐ¼Ð¶Ð¸ÐºÙôÐ·åâé¿å¬Þ¯ÛÆõóâÝç¥äÍéÇâ³" },
	{ "xin", "xin1", 1, "ÐÄÐÂÐÀÐÁÐ½Ü°öÎÐ¾ê¿ÐÃì§Ð¿" },
	{ "xin", "xin2", 2, "Ñ°ïâ" },
	{ "xin", "xin4", 4, "ÐÅÐ¾ÐÆØ¶" },
	{ "xing", "xing1", 1, "ÐËÐÇÐÈÐÊÐÉ" },
	{ "xing", "xing2", 2, "ÐÐÐÎÐÍÐÌÐÏÚêÜþâ¼íÊ" },
	{ "xing", "xing3", 3, "Ê¡ÐÑß©" },
	{ "xing", "xing4", 4, "ÐÔÐËÐÕÐÒÐÓã¬Üô" },
	{ "xiong", "xiong1", 1, "ÐÖÐØÐ×ÐÙÐÚÜº" },
	{ "xiong", "xiong2", 2, "ÐÛÐÜ" },
	{ "xiu", "xiu1", 1, "ÐÞÐÝÐßßÝâÊâÓð¼õ÷÷Û" },
	{ "xiu", "xiu3", 3, "ËÞÐà" },
	{ "xiu", "xiu4", 4, "ÐãÐäËÞ³ôÐåÐâÐáá¶äå" },
	{ "xu", "xu1", 1, "ÐèÐëÐéÓõÐêÐæÐçñãí¹Û×íìçï" },
	{ "xu", "xu2", 2, "Ðì" },
	{ "xu", "xu3", 3, "Ðíä°èòÚ¼ôÚõ¯" },
	{ "xu", "xu4", 4, "ÐøÐòÐ÷ÐîÐðÐóÐôÐõÐñÐöÐïìãäªäÓÛÃ" },
	{ "xu", "xu5", 5, "Þ£" },
	{ "xuan", "xuan1", 1, "ÐûÐúÐùÝæêÑÚÎÞïÙØìÓ" },
	{ "xuan", "xuan2", 2, "ÐýÐüÐþäöè¯ðç" },
	{ "xuan", "xuan3", 3, "Ñ¡Ñ¢" },
	{ "xuan", "xuan4", 4, "ÐýÈ¯ìÅäÖÑ¤Ñ£îçãùíÛé¸ïà" },
	{ "xue", "xue1", 1, "Ï÷Ñ¥Ñ¦" },
	{ "xue", "xue2", 2, "Ñ§Ñ¨àåõ½í´" },
	{ "xue", "xue3", 3, "Ñ©÷¨" },
	{ "xue", "xue4", 4, "ÑªÚÊ" },
	{ "xun", "xun1", 1, "Ñ¬Ñ«»çõ¸Þ¹Û÷êÖñ¿â´" },
	{ "xun", "xun2", 2, "Ñ°Ñ¯Ñ²Ñ­Ñ®Ñ±Ü÷á¾ä­âþÛ¨ä±öà" },
	{ "xun", "xun4", 4, "ÑµÑ¸Ñ¶Ñ·Ñ¬Ñ³ÙãáßÑ´Þ¦¿£" },
	{ "ya", "ya1", 1, "Ñ¹ÑÅÑ½ÑºÑ»ÑÆÑ¼Ñ¾Ûëèâ" },
	{ "ya", "ya2", 2, "ÑÀÑÄÑÂÑ¿ÑÃíýØóá¬çðÑÁ" },
	{ "ya", "ya3", 3, "ÑÅÇÆÆ¥ðéñâ" },
	{ "ya", "ya4", 4, "ÑÇÑ¹ÑÈÔþæ«åÂÞëë²í¼" },
	{ "ya", "ya5", 5, "Ñ½" },
	{ "yan", "yan1", 1, "ÑÌÑàÑÊÒóÑÉÑÍÑËëçæÌëÙäÎãÕÛ³ÝÎáÃâû" },
	{ "yan", "yan2", 2, "ÑÔÑÏÑÐÑÓÑØÑÕÑ×ÑÖÑÎÑÒÇ¦ÑÑéÜåûóÛÜ¾ãÆÚç" },
	{ "yan", "yan3", 3, "ÑÛÑÝÑÚÑÜÑÙÙ²ÙÈ÷Ê÷úÙðÛ±çüî»ØÉØß÷ü" },
	{ "yan", "yan4", 4, "ÑÐÑéÑØÑáÑàÑçÑÊÑãÑæÑÞÑèÑåìÍêÌÑäÑâÑßØÍ÷ÐäÙõ¦ÚÝ" },
	{ "yang", "yang1", 1, "ÑëãóÑíÑìÑê÷±" },
	{ "yang", "yang2", 2, "ÑóÑôÑîÑïÑòÑñÑðìÈáàì¾òÕ" },
	{ "yang", "yang3", 3, "ÑøÑöÑ÷Ñõ" },
	{ "yang", "yang4", 4, "ÑùÑúí¦ìÈâó÷±" },
	{ "yao", "yao1", 1, "ÒªÔ¼ÑûÑüØ²ÑýßºçÛ" },
	{ "yao", "yao2", 2, "Ò¡Ò£Ò¦ÌÕÒ¢Ò¥ÑþÒ¤ëÈ½Äï¢çòé÷Ø³áæôí÷¥" },
	{ "yao", "yao3", 3, "Ò§èÃñºÒ¨áÊ" },
	{ "yao", "yao4", 4, "ÒªÒ©Ò«Ô¿ðÎê×Å±" },
	{ "ye", "ye1", 1, "Ò®Ò­Ò¬Ò´" },
	{ "ye", "ye2", 2, "Ò¯Ò®Ð°ÞÞîô" },
	{ "ye", "ye3", 3, "Ò²Ò°Ò±" },
	{ "ye", "ye4", 4, "ÒµÒ¹Ò¶Ò³ÒºÑÊ»©Ò·×§ìÇÒ´Ò¸ÚËÚþØÌêÊ" },
	{ "yi", "yi1", 1, "Ò»Ò½ÒÂÒÀÒÎÒÁäôßÞÒ¾àæâ¢Ò¼Ò¿ì¥÷ð" },
	{ "yi", "yi2", 2, "ÒÆÒÉÒÅÒËÒÇÉßÒÌÒÄâùÒÃÒÍß×êÝåÆðêÒÈÒÊâÂÛÝÜèÚ±íôáÚ" },
	{ "yi", "yi3", 3, "ÒÔÒÑÒÂÎ²ÒÎÒÓÒÒÒÏÒÐåÆ¶êì½ÜÓîÆô¯ôý" },
	{ "yi", "yi4", 4, "ÒâÒåÒéÒ×ÒÂÒÕÒëÒìÒæÒàÒÚÒäÒêÒÖÒíÒÛ°¬ÒçÒãÒáÒÝéóÞÄÒîÒßÒïØýÞÈìÚÒèß®æäÜ²ß½ÒÙÞ²àæï×çËÒØÒÜØ×ôàØîá»âøã¨ÒÞÙ«éìÞÚÛüØæïîðùñ¯ñ´òæàÉôè" },
	{ "yin", "yin1", 1, "ÒòÒôÑÌÒõÒöÒóÒðÒñà³äÎë³Ü§ä¦î÷" },
	{ "yin", "yin2", 2, "ÒøÒ÷ÒúÒùÛóÛ´ö¯áþâ¹Ûßö¸" },
	{ "yin", "yin3", 3, "ÒýÒþÒûñ«ÒóÒüò¾ßÅ" },
	{ "yin", "yin4", 4, "Ó¡ÒûÒñØ·Üáñ¿" },
	{ "ying", "ying1", 1, "Ó¦Ó¢Ó¥Ó¤Ó£âßÝºó¿ðÐÓ§çøè¬ÞüàÓ" },
	{ "ying", "ying2", 2, "ÓªÓ­Ó®Ó¯Ó¬Ó¨Ó«Ó©ÝÓå­éºÙøÜãäÞäëÜþÝö" },
	{ "ying", "ying3", 3, "Ó°Ó±ò£ñ¨Û«" },
	{ "ying", "ying4", 4, "Ó¦Ó²Ó³ëô" },
	{ "yo", "yo1", 1, "ÓýÓ´à¡" },
	{ "yo", "yo5", 5, "Ó´" },
	{ "yong", "yong1", 1, "ÓµÓ¹Ó¶ÓºÓ·çßïÞÜ­ã¼Ó¸ÛÕ÷«÷Ó" },
	{ "yong", "yong2", 2, "à¯" },
	{ "yong", "yong3", 3, "ÓÀÓÂÓ¿Ó»Ó¾Ó½Ù¸ÓÁð®Ó¼" },
	{ "yong", "yong4", 4, "ÓÃÓ¶" },
	{ "you", "you1", 1, "ÓÅÓÄÓÇÓÆØüßÏ" },
	{ "you", "you2", 2, "ÓÉÓÎÓÍÓÊÓÈÓÌèÖöÏÝµÞÌÓËéàðàòÄòöòøôíÝ¯" },
	{ "you", "you3", 3, "ÓÐÓÑ÷îÓÏÝ¬ë»îðØÕ" },
	{ "you", "you4", 4, "ÓÐÓÖÓÒÓ×ÓÕÓÓèÖàó÷øå¶Ù§òÊÓÔ" },
	{ "yu", "yu1", 1, "ì¶ÓõÓØÓÙæúðö" },
	{ "yu", "yu2", 2, "ÓÚÓëÓàÓèÓãÓÞÓßÓéÓäâÅÓâÓæÓåÓáÝÇè¤ÓçÞíÓÜÓÝØ®ÚÄëéóÄæ¥ô§ì£êìÓÛñ¾òõáÎáüô¨ö§" },
	{ "yu", "yu3", 3, "ÓëÓïÓêÓèÓîÓðÓíàôÓìö¹Øñàöâ×ðõñÁÙ¶" },
	{ "yu", "yu4", 4, "ÓëÓïÓýÓöÓüÓêÓûÔ¤ÓñÓú¹ÈÓòÓþÓõÎµÔ¢Ô¥ÖàÓôÓ÷Ô£Ô¡ÓùÔ¦Î¾ÚÍØ¹åýÓøÓóêÅìÏìÙìÛÝÒÝ÷âÀãÐå÷í²îÚðÁðÖòâ" },
	{ "yuan", "yuan1", 1, "Ô©Ô¨Ô§íóð°óî" },
	{ "yuan", "yuan2", 2, "Ô±ÔªÔ­Ô°Ô´Ô²ÔµÔ®Ô¬Ô³Ô«Ô¯ãäæÂÜ¾éÚà÷Ü«ë¼ó¢ö½" },
	{ "yuan", "yuan3", 3, "Ô¶" },
	{ "yuan", "yuan4", 4, "ÔºÔ¸Ô¹Ô·æÂÞòÛùè¥" },
	{ "yue", "yue1", 1, "Ô¼Ô»" },
	{ "yue", "yue4", 4, "ËµÔÂÀÖÔ½ÔÄÔ¾ÔÃÔÀÔÁÔ¿ë¾å®èÝéÐÙßîá" },
	{ "yun", "yun1", 1, "ÔÎëµ" },
	{ "yun", "yun2", 2, "Ô±ÔÆÔÈóÞÜ¿ÔÅç¡êÀÔÇ" },
	{ "yun", "yun3", 3, "ÔÊÔÉéæáñ" },
	{ "yun", "yun4", 4, "Ô±ÔË¾ùÔÏÔÎÔÐÔÌÔÍã³ìÙÛ©è¹ã¢" },
	{ "za", "za1", 1, "ÔúßÆÔÑÞÙ" },
	{ "za", "za2", 2, "ÔÓÔÛÔÒ" },
	{ "za", "za3", 3, "Õ¦" },
	{ "zai", "zai1", 1, "ÔÖÔÕÔÔçÞ" },
	{ "zai", "zai3", 3, "ÔØ×ÐÔ×áÌ" },
	{ "zai", "zai4", 4, "ÔÚÔÙÔØ" },
	{ "zan", "zan1", 1, "ô¢ôØ" },
	{ "zan", "zan2", 2, "ÔÛ" },
	{ "zan", "zan3", 3, "ÔÜÞÙêÃôõ" },
	{ "zan", "zan4", 4, "ÔÞÔÝè¶öÉ" },
	{ "zan", "zan5", 5, "ÔÛ" },
	{ "zang", "zang1", 1, "Ôßê°Õà" },
	{ "zang", "zang3", 3, "æà" },
	{ "zang", "zang4", 4, "²ØÔàÔáÞÊ" },
	{ "zao", "zao1", 1, "ÔâÔã" },
	{ "zao", "zao2", 2, "Ôä" },
	{ "zao", "zao3", 3, "ÔçÔèÔæÔéÔåçØ" },
	{ "zao", "zao4", 4, "ÔìÔîÔêÔëÔíÔïßð" },
	{ "ze", "ze2", 2, "ÔòÔðÔóÔñÕ¦ßõåÅàýØÓóÐóåô·" },
	{ "ze", "ze4", 4, "²àØÆê¾" },
	{ "zei", "zei2", 2, "Ôô" },
	{ "zen", "zen3", 3, "Ôõ" },
	{ "zen", "zen4", 4, "ÚÚ" },
	{ "zeng", "zeng1", 1, "ÔøÔöÔ÷çÕîÀ" },
	{ "zeng", "zeng4", 4, "Ôù×ÛçÕêµï­" },
	{ "zha", "zha1", 1, "²éÔúÕ¦ÔüÔûÞêé«ßîß¸÷þ" },
	{ "zha", "zha2", 2, "Õ¨ÔúÔýà©ÔþÕ¢Õ¡" },
	{ "zha", "zha3", 3, "Õ£íÄ" },
	{ "zha", "zha4", 4, "Õ¨Õ¦Õ©Õ§À¯Õ¤Õ¥×õß¸ßåðäòÆ" },
	{ "zhai", "zhai1", 1, "Õª²àÕ«" },
	{ "zhai", "zhai2", 2, "ÔñÕ¬µÔ" },
	{ "zhai", "zhai3", 3, "Õ­" },
	{ "zhai", "zhai4", 4, "Õ®¼ÀÕ¯íÎñ©" },
	{ "zhan", "zhan1", 1, "Õ¼Õ´Õ³Õ°Õ²Õ±ÚÞì¹" },
	{ "zhan", "zhan3", 3, "Õ¹Õ¶Õ·ÕµÕ¸Þø" },
	{ "zhan", "zhan4", 4, "Õ½Õ¾Õ¼²üÕÀÕ¿ÕºÕ»" },
	{ "zhang", "zhang1", 1, "ÕÅÕÂÕÃè°ó¯ÕÁÕÄæÑÛµâ¯" },
	{ "zhang", "zhang3", 3, "³¤ÕÆÕÇØë" },
	{ "zhang", "zhang4", 4, "ÕÉÕÇÕÊÕÏÕËÕÍÕÌÕÈÕÎáÖá¤" },
	{ "zhao", "zhao1", 1, "×ÅÕÐ³¯³°ÕÑîÈßú" },
	{ "zhao", "zhao2", 2, "×Å" },
	{ "zhao", "zhao3", 3, "ÕÒ×¦ÕÓ" },
	{ "zhao", "zhao4", 4, "ÕÕÕÔÕÙÕÖÕ×ÕØÚ¯èþóÉ" },
	{ "zhe", "zhe1", 1, "ÕÛÕÚòØ" },
	{ "zhe", "zhe2", 2, "ÕÛÕÜÕÞéüÚØÕÝß¡íÝòØ" },
	{ "zhe", "zhe3", 3, "ÕßñÞÕàô÷" },
	{ "zhe", "zhe4", 4, "ÕâÕãÕáðÑèÏ" },
	{ "zhe", "zhe5", 5, "×Å" },
	{ "zhei", "zhei4", 4, "Õâ" },
	{ "zhen", "zhen1", 1, "ÕæÕëÕäÕåÕêÕìÕçÕéóðÕèèåäÚÝèé©é»ëÓìõä¥" },
	{ "zhen", "zhen3", 3, "ÕïÕíÕîçÇî³éôð¡" },
	{ "zhen", "zhen4", 4, "ÕóÕòÕðÛÚÕñêâëÞð²" },
	{ "zheng", "zheng1", 1, "ÕýÕùÕ÷¶¡ÕõÖ¢ÕöáçÕôÕúóÝï£á¿ÕøîÛöë" },
	{ "zheng", "zheng3", 3, "ÕûÕü" },
	{ "zheng", "zheng4", 4, "ÕþÕýÖ¤ÕõÖ£Ö¢Õúï£ÚºÖ¡" },
	{ "zhi", "zhi1", 1, "Ö®Ö»ÖªÖ¸Ö§Ö¯ÊÏÖ¦Ö­ÖÀÖ¥Ö¨Ö«Ö¬Ö©èÙØ´ëÕìó" },
	{ "zhi", "zhi2", 2, "Ö±Ö¸Ö°ÖµÖ´Ö²Ö³Ö¶õÜÞýôêõÅÛú" },
	{ "zhi", "zhi3", 3, "Ö»Ö¸Ö½Ö¹Ö·Ö¼áçÖºåëÜÆè×ìíéòíéõ¥" },
	{ "zhi", "zhi4", 4, "ÖªÖÁÖÆÊ¶ÖÎÖ¾ÖÂÖÊÖÇÖÃÖÈÖÍÖÄÖÉÖ¿ÖÀÖÅÖÏÖËÖÌèÎèäàùéùêÞðëõôÚìâååéëùïôðºæïòÎõÙÛ¤ö£" },
	{ "zhong", "zhong1", 1, "ÖÐÖÕÖÓÖÒÖÔïñÖÑâìó®ô±" },
	{ "zhong", "zhong3", 3, "ÖÖÖ×õàÚ£" },
	{ "zhong", "zhong4", 4, "ÖÐÖÖÖØÖÚÖÙ" },
	{ "zhou", "zhou1", 1, "ÖÜÖÝÖÞÖàÖÛÖßßú" },
	{ "zhou", "zhou2", 2, "Öáæ¨íØ" },
	{ "zhou", "zhou3", 3, "ÖâÖã" },
	{ "zhou", "zhou4", 4, "ÖåÖèÖáÖæÖäÖçëÐæûç§Ý§ô¦ôíôü" },
	{ "zhu", "zhu1", 1, "ÖîÖìÖéÖíÖêÖëä¨ÖïîùÜïÛ¥äóéÆéÍÙª" },
	{ "zhu", "zhu2", 2, "ÊõÖðÖþÖñÖòõîóÃô¶ðñ" },
	{ "zhu", "zhu3", 3, "Ö÷ÊôÖóÖöÖõÖôñÒä¾÷æ" },
	{ "zhu", "zhu4", 4, "×¡×¢ÖúÖø×¤×£ÖþÖùÖýØùÖüóçìÄÖûèÌôãÜÑðæ" },
	{ "zhua", "zhua1", 1, "×¥ÎÎ" },
	{ "zhua", "zhua3", 3, "×¦" },
	{ "zhuai", "zhuai1", 1, "×§" },
	{ "zhuai", "zhuai3", 3, "×ª" },
	{ "zhuai", "zhuai4", 4, "Ò·×§àÜ" },
	{ "zhuan", "zhuan1", 1, "×¨×©ò§" },
	{ "zhuan", "zhuan3", 3, "×ª" },
	{ "zhuan", "zhuan4", 4, "´«×ª×¬×«ãç×­ßùâÍ" },
	{ "zhuang", "zhuang1", 1, "×°×¯×±×®" },
	{ "zhuang", "zhuang3", 3, "ÞÊ" },
	{ "zhuang", "zhuang4", 4, "×´×³×²´±Ù×í°" },
	{ "zhui", "zhui1", 1, "×·×¶ö¿×µæí" },
	{ "zhui", "zhui4", 4, "×¹×º×¸ã·çÄ" },
	{ "zhun", "zhun1", 1, "ÍÍ×»ëÆñ¸" },
	{ "zhun", "zhun3", 3, "×¼" },
	{ "zhuo", "zhuo1", 1, "×À×½×¿×¾äÃìÌÙ¾" },
	{ "zhuo", "zhuo2", 2, "×ÅÖø×Á½É×Æ×Ã×Çåª×Â×Äí½ïíÚÂìúßªä·" },
	{ "zi", "zi1", 1, "×Ê×É×Ì×Ð×ËÖ¨×È×ÎÚÑßÚö·ïÅê¢×Í÷ÚêßæÜôÒôôõþö¤ç»ööáÑ" },
	{ "zi", "zi3", 3, "×Ó×Ï×Ðè÷æ¢×Ñ×ÒïöóÊñèÜëö¤" },
	{ "zi", "zi4", 4, "×Ô×Ö×Õí§íö" },
	{ "zong", "zong1", 1, "×Ú×Ù×Û×Ø××èÈëê" },
	{ "zong", "zong3", 3, "×ÜÙÌ" },
	{ "zong", "zong4", 4, "×ÝôÕ" },
	{ "zou", "zou1", 1, "×ÞÚÁÚîÛ¸æãöí" },
	{ "zou", "zou3", 3, "×ß" },
	{ "zou", "zou4", 4, "×à×á" },
	{ "zu", "zu1", 1, "×âÝÏ" },
	{ "zu", "zu2", 2, "×ã×å×äïß" },
	{ "zu", "zu3", 3, "×é×æ×è×çÙÞ" },
	{ "zuan", "zuan1", 1, "×êõò" },
	{ "zuan", "zuan3", 3, "×ëçÚ" },
	{ "zuan", "zuan4", 4, "×¬×êß¬" },
	{ "zui", "zui1", 1, "¶Ñ" },
	{ "zui", "zui3", 3, "×ì¾×õþ" },
	{ "zui", "zui4", 4, "×î×ï×íÞ©" },
	{ "zun", "zun1", 1, "×ð×ñé×÷®" },
	{ "zun", "zun3", 3, "ß¤" },
	{ "zuo", "zuo1", 1, "×÷àÜ" },
	{ "zuo", "zuo2", 2, "×÷×ò×ÁóÐ" },
	{ "zuo", "zuo3", 3, "×ó×ô´é" },
	{ "zuo", "zuo4", 4, "×÷×ö×ø×ùÔä×õâôëÑÚèßòìñõ¡" }
};

int  PY_CNT = sizeof(p2h) / sizeof(p2h[0]);
char *getcnchar(char *cnChar)
{
	char query[] = "°¬";
	char *curHzString;
	char curHz[4] = "";
	int i, j;
	int found = 0;
	char *gotChar = NULL;
	query[0] = cnChar[0];
	query[1] = cnChar[1];

	for (i = 0; i < PY_CNT && !found; i++) {
		curHzString = p2h[i].hz;
		for (j = 0; j < (int)strlen((const char *)curHzString); j += 2) {
			memcpy(curHz, curHzString + j, 2);
			if (strcmp(query, curHz))
				continue;
			else {
				gotChar = p2h[i].py;
				found = 1;
				break;
			}
		}
	}
	if (!found)
		return NULL;
	else
	{
		return gotChar;
	}

}





//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/* ºº×Ö¶ÁÒô·Ö½çµã´¦µÄÂëÖµÊý×é*/
const unsigned short code_pin[] = {
	0xb0a1, 0xb0a3, 0xb0b0, 0xb0b9, 0xb0bc, 0xb0c5, 0xb0d7, 0xb0df, 0xb0ee, 0xb0fa, 0xb1ad, 0xb1bc, 0xb1c0, 0xb1c6,
	0xb1de, 0xb1ea, 0xb1ee, 0xb1f2, 0xb1f8, 0xb2a3, 0xb2b8, 0xb2c1, 0xb2c2, 0xb2cd, 0xb2d4, 0xb2d9, 0xb2de, 0xb2e3,
	0xb2e5, 0xb2f0, 0xb2f3, 0xb2fd, 0xb3ac, 0xb3b5, 0xb3bb, 0xb3c5, 0xb3d4, 0xb3e4, 0xb3e9, 0xb3f5, 0xb4a7, 0xb4a8,
	0xb4af, 0xb4b5, 0xb4ba, 0xb4c1, 0xb4c3, 0xb4cf, 0xb4d5, 0xb4d6, 0xb4da, 0xb4dd, 0xb4e5, 0xb4e8, 0xb4ee, 0xb4f4,
	0xb5a2, 0xb5b1, 0xb5b6, 0xb5c2, 0xb5c5, 0xb5cc, 0xb5df, 0xb5ef, 0xb5f8, 0xb6a1, 0xb6aa, 0xb6ab, 0xb6b5, 0xb6bc,
	0xb6cb, 0xb6d1, 0xb6d5, 0xb6de, 0xb6ea, 0xb6f7, 0xb6f8, 0xb7a2, 0xb7aa, 0xb7bb, 0xb7c6, 0xb7d2, 0xb7e1, 0xb7f0,
	0xb7f1, 0xb7f2, 0xb8c1, 0xb8c3, 0xb8c9, 0xb8d4, 0xb8dd, 0xb8e7, 0xb8f8, 0xb8f9, 0xb8fb, 0xb9a4, 0xb9b3, 0xb9bc,
	0xb9ce, 0xb9d4, 0xb9d7, 0xb9e2, 0xb9e5, 0xb9f5, 0xb9f8, 0xb9fe, 0xbaa1, 0xbaa8, 0xbabb, 0xbabe, 0xbac7, 0xbad9,
	0xbadb, 0xbadf, 0xbae4, 0xbaed, 0xbaf4, 0xbba8, 0xbbb1, 0xbbb6, 0xbbc4, 0xbbd2, 0xbbe7, 0xbbed, 0xbbf7, 0xbcce,
	0xbcdf, 0xbda9, 0xbdb6, 0xbdd2, 0xbded, 0xbea3, 0xbebc, 0xbebe, 0xbecf, 0xbee8, 0xbeef, 0xbef9, 0xbfa6, 0xbfaa,
	0xbfaf, 0xbfb5, 0xbfbc, 0xbfc0, 0xbfcf, 0xbfd3, 0xbfd5, 0xbfd9, 0xbfdd, 0xbfe4, 0xbfe9, 0xbfed, 0xbfef, 0xbff7,
	0xc0a4, 0xc0a8, 0xc0ac, 0xc0b3, 0xc0b6, 0xc0c5, 0xc0cc, 0xc0d5, 0xc0d7, 0xc0e2, 0xc0e5, 0xc1a9, 0xc1aa, 0xc1b8,
	0xc1c3, 0xc1d0, 0xc1d5, 0xc1e1, 0xc1ef, 0xc1fa, 0xc2a5, 0xc2ab, 0xc2bf, 0xc2cd, 0xc2d3, 0xc2d5, 0xc2dc, 0xc2e8,
	0xc2f1, 0xc2f7, 0xc3a2, 0xc3a8, 0xc3b4, 0xc3b5, 0xc3c5, 0xc3c8, 0xc3d0, 0xc3de, 0xc3e7, 0xc3ef, 0xc3f1, 0xc3f7,
	0xc3fd, 0xc3fe, 0xc4b1, 0xc4b4, 0xc4c3, 0xc4ca, 0xc4cf, 0xc4d2, 0xc4d3, 0xc4d8, 0xc4d9, 0xc4db, 0xc4dc, 0xc4dd,
	0xc4e8, 0xc4ef, 0xc4f1, 0xc4f3, 0xc4fa, 0xc4fb, 0xc5a3, 0xc5a7, 0xc5ab, 0xc5ae, 0xc5af, 0xc5b0, 0xc5b2, 0xc5b6,
	0xc5b7, 0xc5be, 0xc5c4, 0xc5ca, 0xc5d2, 0xc5d7, 0xc5de, 0xc5e7, 0xc5e9, 0xc5f7, 0xc6aa, 0xc6ae, 0xc6b2, 0xc6b4,
	0xc6b9, 0xc6c2, 0xc6cb, 0xc6da, 0xc6fe, 0xc7a3, 0xc7b9, 0xc7c1, 0xc7d0, 0xc7d5, 0xc7e0, 0xc7ed, 0xc7ef, 0xc7f7,
	0xc8a6, 0xc8b1, 0xc8b9, 0xc8bb, 0xc8bf, 0xc8c4, 0xc8c7, 0xc8c9, 0xc8d3, 0xc8d5, 0xc8d6, 0xc8e0, 0xc8e3, 0xc8ed,
	0xc8ef, 0xc8f2, 0xc8f4, 0xc8f6, 0xc8f9, 0xc8fd, 0xc9a3, 0xc9a6, 0xc9aa, 0xc9ad, 0xc9ae, 0xc9af, 0xc9b8, 0xc9ba,
	0xc9ca, 0xc9d2, 0xc9dd, 0xc9e9, 0xc9f9, 0xcaa6, 0xcad5, 0xcadf, 0xcba2, 0xcba4, 0xcba8, 0xcbaa, 0xcbad, 0xcbb1,
	0xcbb5, 0xcbb9, 0xcbc9, 0xcbd1, 0xcbd4, 0xcbe1, 0xcbe4, 0xcbef, 0xcbf2, 0xcbfa, 0xcca5, 0xccae, 0xccc0, 0xcccd,
	0xccd8, 0xccd9, 0xccdd, 0xccec, 0xccf4, 0xccf9, 0xccfc, 0xcda8, 0xcdb5, 0xcdb9, 0xcdc4, 0xcdc6, 0xcdcc, 0xcdcf,
	0xcdda, 0xcde1, 0xcde3, 0xcdf4, 0xcdfe, 0xcec1, 0xcecb, 0xcece, 0xced7, 0xcef4, 0xcfb9, 0xcfc6, 0xcfe0, 0xcff4,
	0xd0a8, 0xd0bd, 0xd0c7, 0xd0d6, 0xd0dd, 0xd0e6, 0xd0f9, 0xd1a5, 0xd1ab, 0xd1b9, 0xd1c9, 0xd1ea, 0xd1fb, 0xd2ac,
	0xd2bb, 0xd2f0, 0xd3a2, 0xd3b4, 0xd3b5, 0xd3c4, 0xd3d9, 0xd4a7, 0xd4bb, 0xd4c5, 0xd4d1, 0xd4d4, 0xd4db, 0xd4df,
	0xd4e2, 0xd4f0, 0xd4f4, 0xd4f5, 0xd4f6, 0xd4fa, 0xd5aa, 0xd5b0, 0xd5c1, 0xd5d0, 0xd5da, 0xd5e4, 0xd5f4, 0xd6a5,
	0xd6d0, 0xd6db, 0xd6e9, 0xd7a5, 0xd7a7, 0xd7a8, 0xd7ae, 0xd7b5, 0xd7bb, 0xd7bd, 0xd7c8, 0xd7d7, 0xd7de, 0xd7e2,
	0xd7ea, 0xd7ec, 0xd7f0, 0xd7f2 };

/* ºº×Ö¶ÁÒôÊý×é*/
const char *str_pin[] = {
	"a", "ai", "an", "ang", "ao", "ba", "bai", "ban", "bang", "bao", "bei", "ben", "beng", "bi", "bian", "biao",
	"bie", "bin", "bing", "bo", "bu", "ca", "cai", "can", "cang", "cao", "ce", "ceng", "cha", "chai", "chan",
	"chang", "chao", "che", "chen", "cheng", "chi", "chong", "chou", "chu", "chuai", "chuan", "chuang", "chui",
	"chun", "chuo", "ci", "cong", "cou", "cu", "cuan", "cui", "cun", "cuo", "da", "dai", "dan", "dang", "dao",
	"de", "deng", "di", "dian", "diao", "die", "ding", "diu", "dong", "dou", "du", "duan", "dui", "dun", "duo",
	"e", "en", "er", "fa", "fan", "fang", "fei", "fen", "feng", "fo", "fou", "fu", "ga", "gai", "gan", "gang",
	"gao", "ge", "gei", "gen", "geng", "gong", "gou", "gu", "gua", "guai", "guan", "guang", "gui", "gun",
	"guo", "ha", "hai", "han", "hang", "hao", "he", "hei", "hen", "heng", "hong", "hou", "hu", "hua", "huai",
	"huan", "huang", "hui", "hun", "huo", "ji", "jia", "jian", "jiang", "jiao", "jie", "jin", "jing",
	"jiong", "jiu", "ju", "juan", "jue", "jun", "ka", "kai", "kan", "kang", "kao", "ke", "ken", "keng",
	"kong", "kou", "ku", "kua", "kuai", "kuan", "kuang", "kui", "kun", "kuo", "la", "lai", "lan", "lang",
	"lao", "le", "lei", "leng", "li", "lia", "lian", "liang", "liao", "lie", "lin", "ling", "liu", "long", "lou",
	"lu", "lv", "luan", "lue", "lun", "luo", "ma", "mai", "man", "mang", "mao", "me", "mei", "men", "meng",
	"mi", "mian", "miao", "mie", "min", "ming", "miu", "mo", "mou", "mu", "na", "nai", "nan", "nang", "nao", "ne",
	"nei", "nen", "neng", "ni", "nian", "niang", "niao", "nie", "nin", "ning", "niu", "nong", "nu", "nv", "nuan",
	"nue", "nuo", "o", "ou", "pa", "pai", "pan", "pang", "pao", "pei", "pen", "peng", "pi", "pian", "piao", "pie",
	"pin", "ping", "po", "pu", "qi", "qia", "qian", "qiang", "qiao", "qie", "qin", "qing", "qiong", "qiu", "qu",
	"quan", "que", "qun", "ran", "rang", "rao", "re", "ren", "reng", "ri", "rong", "rou", "ru", "ruan", "rui",
	"run", "ruo", "sa", "sai", "san", "sang", "sao", "se", "sen", "seng", "sha", "shai", "shan", "shang", "shao",
	"she", "shen", "sheng", "shi", "shou", "shu", "shua", "shuai", "shuan", "shuang", "shui", "shun", "shuo",
	"si", "song", "sou", "su", "suan", "sui", "sun", "suo", "ta", "tai", "tan", "tang", "tao", "te", "teng",
	"ti", "tian", "tiao", "tie", "ting", "tong", "tou", "tu", "tuan", "tui", "tun", "tuo", "wa", "wai", "wan",
	"wang", "wei", "wen", "weng", "wo", "wu", "xi", "xia", "xian", "xiang", "xiao", "xie", "xin", "xing",
	"xiong", "xiu", "xu", "xuan", "xue", "xun", "ya", "yan", "yang", "yao", "ye", "yi", "yin", "ying", "yo",
	"yong", "you", "yu", "yuan", "yue", "yun", "za", "zai", "zan", "zang", "zao", "ze", "zei", "zen", "zeng",
	"zha", "zhai", "zhan", "zhang", "zhao", "zhe", "zhen", "zheng", "zhi", "zhong", "zhou", "zhu", "zhua",
	"zhuai", "zhuan", "zhuang", "zhui", "zhun", "zhuo", "zi", "zong", "zou", "zu", "zuan", "zui", "zun", "zuo" };



/* Êý×é³¤¶È*/
static const size_t SIZE_ARRAY = sizeof(code_pin) / sizeof(short);


/* »ñÈ¡Ò»¸öºº×ÖµÄÆ´Òô*/
const char* get_pin(unsigned short char_zh, const char* pszTemp)
{
	size_t low = 0, high = SIZE_ARRAY - 1;
	size_t index;

	if (char_zh > code_pin[high])
	{
		char* pszChar = getcnchar((char*)pszTemp);
		if (pszChar != NULL)
		{
			return pszChar;
		}
	}

	while (high - low != 1)
	{
		index = (low + high) / 2;
		if (code_pin[index] == char_zh)
			return str_pin[index];
		if (code_pin[index] < char_zh)
			low = index;
		else
			high = index;
	}
	return str_pin[code_pin[high] <= char_zh ? high : low];
}

/*
½«ÖÐÓ¢¼°Êý×Ö»ìºÏ´®×ª»¯³ÉÆ´Òô£¬ÇÒÃ¿Ò»Á¬Ðø²¿·ÖÖ®¼ä·ÅÖÃÒ»¸ö¿Õ¸ñ¡£

ÊäÈë±ØÐëÂú×ãÖ»ÓÐÉÏÊöÈýÖÖ×Ö·ûµÄÇé¿ö£¬ÕýÈ·ÐÔÓÉµ÷ÓÃ¶Ë±£Ö¤¡£
*/
std::string  NStr::ChineseToPinyin(std::string  strData)
{
	std::string  result;
	unsigned short char_zh;
	int inputLength = strData.length();
	unsigned char high, low;
	for (int i = 0; i < inputLength; ++i)
	{
		high = strData[i];
		if (high < 123)
		{
			if (i > 0 && (strData[i - 1] < 0 || strData[i - 1] > 57))
			{
				//result.append(1, ' ');
			}
			result.append(1, (high >= 'A' && high <= 'Z') ? (high + ('a' - 'A')) : high);
		}
// 		else if (high > 64 && high < 123)
// 		{
// 			if (i > 0 && (strData[i - 1] < 0 || strData[i - 1] < 58))
// 			{
// 				//result.append(1, ' ');
// 			}
// 			result.append(1, (high >= 'A' && high <= 'Z') ? (high + ('a' - 'A')) : high);
// 		}
		else
		{
			//if (i > 0) result.append(1, ' ');//¼Ó¿Õ¸ñÇø·ÖÊ××ÖÄ¸
			low = strData[++i];
			char_zh = (high << 8) + low;
			result.append(get_pin(char_zh, strData.c_str() + i - 1));
		}
	}
	return result;
}


#endif
