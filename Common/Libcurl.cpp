
#include "libcurl.h"
#ifdef _WIN64   
#pragma comment(lib, "../Lib/libcurl_a.lib")
#else
#pragma comment(lib, "../Lib/libcurl.lib")
#endif
#include <string>
#include <assert.h>
#include <iostream>
#include "FileManager/FileManange.h"
#include "log/log_deffine.h"
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Iphlpapi")
#pragma comment(lib, "Wldap32")


CLibcurl::CLibcurl(void)
	: m_pCurl(NULL)
	, m_nPort(80)
	, m_hFile(INVALID_HANDLE_VALUE)
	, m_pCallback(NULL)
	, m_pCallbackParam(NULL)
	, m_curlCode(CURLE_OK)
	, m_lfFlag(Lf_None)
	, m_curlList(NULL)
{
	m_pCurl = curl_easy_init();

	// 设置写数据的回调函数
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);

	// 设置默认的超时时间
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, DEFAULT_CONNECTTIMEOUT_TIMEOUT);
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, DEFAULT_TIMEOUT);


}


CLibcurl::~CLibcurl(void)
{
	ClearHeaderList();
	curl_easy_cleanup(m_pCurl);

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

bool CLibcurl::SetPort(LONG port)
{
	if (port == m_nPort)
		return true;
	m_nPort = port;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_PORT, m_nPort);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetTimeout(int nSecond)
{
	if (nSecond < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, nSecond);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetConnectTimeout(int nSecond)
{
	if (nSecond < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, nSecond);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetUserAgent(LPCSTR lpAgent)
{
	if (NULL == lpAgent)
		return false;
	int nLen = strlen(lpAgent);
	if (nLen == 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, lpAgent);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetResumeFrom(LONG lPos)
{
	if (lPos < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM, lPos);
	return CURLE_OK == m_curlCode;
}

bool  CLibcurl::setReferer(LPCSTR lpReferer)
{
	if (NULL == lpReferer)
		return false;
	int nLen = strlen(lpReferer);
	if (nLen == 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_REFERER, lpReferer);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::SetResumeFromLarge(LONGLONG llPos)
{
	if (llPos < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM_LARGE, llPos);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::AddHeader(LPCSTR lpKey, LPCSTR lpValue)
{
	assert(lpKey != NULL && lpValue != NULL);
	int nLen1 = strlen(lpKey), nLen2 = strlen(lpValue);
	assert(nLen1 > 0 && nLen2 > 0);
	string strHeader(lpKey);
	strHeader.append(": ");
	strHeader.append(lpValue);
	m_curlList = curl_slist_append(m_curlList, strHeader.c_str());
	if (m_curlList)
	{
		m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_curlList);
	}
	else
	{
		m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, 0L);
	}
	return CURLE_OK == m_curlCode;
}

std::string CLibcurl::ReadHeader(LPCSTR lpUrl)
{

	// 初始化libcurl
	curl_easy_setopt(m_pCurl, CURLOPT_FAILONERROR, true);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	curl_easy_setopt(m_pCurl, CURLOPT_NOBODY, 1L);
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(m_pCurl, CURLOPT_HEADERFUNCTION, ResponseHeadCallBack);
	curl_easy_setopt(m_pCurl, CURLOPT_HEADERDATA, this);


	m_lfFlag = Lf_Get;
	m_strRespons.clear();
	m_strHead.clear();
	//开始执行请求

	m_curlCode = curl_easy_perform(m_pCurl);
	if (CURLE_OK != m_curlCode)
	{
		return "";
	}
	// 获取状态码
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);
	if (m_nResponseCode != 200)
	{
		return "";
	}

	return m_strHead;
}

/*
*	Function:		_ParseHead
*	Explanation:	解析Http头部
*	Input:			pData			头部数据
sSize			    pData大小
*  Output:			mapHeader			头部列表
*	Return:			true 成功 false 失败
*/
bool CLibcurl::_ParseHead(char *pData, size_t sSize,std::map<string, string> &mapHeader)
{
	char*				pBuf = NULL;
	char*				pStart = NULL;
	char*				pEnd = NULL;
	char*				pPos = NULL;

	mapHeader.clear();

	if (0 >= sSize)
	{
		return false;
	}

	if (NULL == pData || 0 == pData[0])
	{
		return false;
	}

	pBuf = (char *)malloc(sSize + 1);
	memset(pBuf, 0, sSize + 1);
	memcpy(pBuf, pData, sSize);
	pPos = pBuf;

	// 过滤第一行（协议信息）
	if (false == _GetNextLine(&pPos, &pStart, &pEnd))
	{
		free(pBuf);
		return false;
	}
	
	// 开始解析数据
	while (_GetNextLine(&pPos, &pStart, &pEnd))
	{
		std::string 	strName = "";
		std::string		strValue = "";
		std::string		strTemp = "";
		char*			pFind = strchr(pStart, ':');

		if (NULL == pFind)
		{
			continue;
		}

		*pFind = 0;
		pFind += 2;

		strName = pStart;
		strValue.append(pFind, pEnd);

		if ("set-cookie" == strName)
		{
			// cookie 需要特别处理
			strName = "cookie";
			//_ParseCookie(strValue);
		}
		mapHeader[strName] = strValue;
	}

	free(pBuf);
	return true;
}
/*
*	Function:		_GetNextLine
*	Explanation:	获取下一行数据（解析Http头部调用）
*	Input:			pPos		数据起始位置的指针
*  Output:			pStart		一行数据起始位置的指针
pEnd		一行数据结束位置的指针
*	Return:			true 成功 false 失败
*/
bool CLibcurl::_GetNextLine(char **pPos, char **pStart, char **pEnd)
{
	// 是否已经到了尾部
	if (0 == (*pPos)[0] || '\r' == *(*pPos) || '\n' == *(*pPos))
	{
		return false;
	}

	*pStart = *pPos;

	for (; 0 != (*pPos)[0]; *pPos = *pPos + 1)
	{
		// 找到行尾（头部一行数据已\r\n结束）
		if ('\r' == *(*pPos) && '\n' == *(*pPos + 1))
		{
			*pEnd = *pPos;
			*pPos = *pPos + 2;
			return true;
		}
	}

	return false;
}

void CLibcurl::ClearHeaderList()
{
	if (m_curlList)
	{
		curl_slist_free_all(m_curlList);
		m_curlList = NULL;
	}
}

bool CLibcurl::SetCookie(LPCSTR lpCookie)
{
	assert(lpCookie != NULL);
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIE, lpCookie);
	return CURLE_OK == m_curlCode;
}

//设置http头中的cookielist信息。
bool CLibcurl::SetCookieList(LPCSTR lpCookielist)
{
	assert(lpCookielist != NULL);
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIELIST, lpCookielist);
	return CURLE_OK == m_curlCode;

}
//设置HTTP保存cookie
bool CLibcurl::SaveCookie(LPCSTR lpCookie)
{
	assert(lpCookie != NULL);
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIEJAR, lpCookie);//设置cookie的保存目录，这里很重要，你懂的
	return CURLE_OK == m_curlCode;

}


bool CLibcurl::SetCookieFile(LPCSTR lpFilePath)
{
	assert(lpFilePath != NULL);
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIEFILE, lpFilePath);
	return CURLE_OK == m_curlCode;
}

const char* CLibcurl::GetError() const
{
	return curl_easy_strerror(m_curlCode);
}

void CLibcurl::SetCallback(CLibcurlCallback* pCallback, void* lpParam)
{
	m_pCallback = pCallback;
	m_pCallbackParam = lpParam;
}

bool CLibcurl::AsynDownloadToFile(LPCSTR lpUrl, LPCSTR lpFile)
{
	// 设置访问网址
	CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	DeleteFileA(lpFile);
	m_strRespons.clear();
	m_hFile = CreateFileA(lpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return FALSE;
	}

	curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);
	m_lfFlag = Lf_Download;
	//开始执行请求
	m_curlCode = curl_easy_perform(m_pCurl);
	if (CURLE_OK != m_curlCode)
	{
		return false;
	}

	return CURLE_OK == m_curlCode;
}

bool CLibcurl::DownloadToFile(LPCSTR lpUrl, LPCSTR lpFile)
{
	bool bres=AsynDownloadToFile(lpUrl, lpFile);
	if (true != bres)
	{
		return false;
	}

	// 判断返回值
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);

	if (200 != m_nResponseCode)
	{
		return false;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return CURLE_OK == m_curlCode;
}

bool CLibcurl::DownloadToFile2(LPCSTR lpUrl, LPCSTR lpFile)
{
	// 设置访问网址
	CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	DeleteFileA(lpFile);
	m_strRespons.clear();
	m_hFile = CreateFileA(lpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return FALSE;
	}

	curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);

	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, false);

	m_lfFlag = Lf_Download;
	//开始执行请求
	m_curlCode = curl_easy_perform(m_pCurl);
	if (CURLE_OK != m_curlCode)
	{
		return false;
	}
	// 判断返回值
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);

	if (200 != m_nResponseCode)
	{
		return false;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return CURLE_OK == m_curlCode;
}

bool CLibcurl::Post(LPCSTR lpUrl, LPCSTR lpData)
{
	assert(lpData != NULL);

	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);
	//curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, lpData);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	
	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
}

bool CLibcurl::Post(LPCSTR lpUrl, unsigned char* lpData, unsigned int nSize)
{
	assert(lpData != NULL && nSize > 0);
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, nSize);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
}

std::string CLibcurl::Posts(std::string strUrl, std::string strData)
{
	assert(strData.size()>0);

	std::string		strReturn = "";
	curl_easy_setopt(m_pCurl, CURLOPT_URL, strUrl.c_str());

	// 设置为Post方式
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	// post数据
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, strData.c_str());
	//curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, strData.size());

	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, false);

	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	// 执行请求
	m_curlCode = curl_easy_perform(m_pCurl);
	if (CURLE_OK != m_curlCode)
	{
		return strReturn;
	}

	// 判断返回值
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);

	strReturn = GetRespons();

	if (200 != m_nResponseCode)
	{
		return strReturn;
	}
	return strReturn;

}

bool CLibcurl::AsynGet(LPCSTR lpUrl)
{
	assert(lpUrl != NULL);
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);//支持重定向
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, false);//设为false 下面才能设置进度响应函数
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);//进度响应函数
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);//数据传输的对象
	m_lfFlag = Lf_Get;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
}

string CLibcurl::Get(LPCSTR lpUrl)
{
	assert(lpUrl != NULL);
	string		strReturn = "";
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);//支持重定向
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);

	m_lfFlag = Lf_Get;
	m_strRespons.clear();
	// 执行请求
	m_curlCode = curl_easy_perform(m_pCurl);

	if (CURLE_OK != m_curlCode)
	{
		return strReturn;
	}

	// 判断返回值
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &m_nResponseCode);
	if (200 != m_nResponseCode)
	{
		strReturn = GetRespons();
		return strReturn;
	}

	strReturn = GetRespons();

	return strReturn;
}

const string& CLibcurl::GetRespons() const
{
	return m_strRespons;
}

const char* CLibcurl::GetResponsPtr() const
{
	return m_strRespons.c_str();
}

int CLibcurl::GetResponseCode()
{
	return m_nResponseCode;
}

size_t CLibcurl::WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//把下载到的数据以追加的方式写入文件(一定要有a，否则前面写入的内容就会被覆盖了)
	CLibcurl* pThis = (CLibcurl*)pParam;
	DWORD dwWritten = 0;
	switch (pThis->m_lfFlag)
	{
	case Lf_Download://下载
	{
		if (pThis->m_pCallback)
		{
			pThis->m_pCallback->WriteCallback(pBuffer, nSize, nMemByte, pParam);
		}

		pThis->m_strRespons.append((const char*)pBuffer, nSize*nMemByte);

		if (pThis->m_hFile == INVALID_HANDLE_VALUE)
			return 0;
		if (!WriteFile(pThis->m_hFile, pBuffer, nSize*nMemByte, &dwWritten, NULL))
			return 0;

	}
	break;
	case Lf_Post://Post数据
	case Lf_Get://Get数据
	{
		pThis->m_strRespons.append((const char*)pBuffer, nSize*nMemByte);
		dwWritten = nSize * nMemByte;
	}
	break;
	case Lf_None://未定义
		break;
	}
	return dwWritten;
}

size_t CLibcurl::ResponseHeadCallBack(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	

	CLibcurl* pThis = (CLibcurl*)pParam;
	size_t				sCurLen = nSize * nMemByte;

	if (NULL == pThis || 0 >= sCurLen || NULL == pBuffer)
	{
		return sCurLen;
	}

	pThis->m_strHead.append((char *)pBuffer, sCurLen);

	return sCurLen;
}

int CLibcurl::ProgressCallback(void *pParam, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CLibcurl* pThis = (CLibcurl*)pParam;
	if (pThis->m_pCallback)
	{
		pThis->m_pCallback->Progress(pThis->m_pCallbackParam, dltotal, dlnow);
	}
	if (dltotal == dlnow && dlnow>0)
	{
		if (pThis->m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(pThis->m_hFile);
			pThis->m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	return 0;
}


CDownloader::CDownloader(void)
: m_pCurl(NULL)
, m_pFile(NULL)
, m_bRunning(false)
{
	m_nStartSize = 0;
	m_dltotal = 0;
	m_dlnow = 0;
	m_Content_Length = 0;
	m_bPause = false;
	// 初始化libcurl
	m_pCurl = curl_easy_init();
	m_callbackfuc = nullptr;
	m_pCallbackParam = nullptr;
	m_bDelete = false;
}

CDownloader::~CDownloader(void)
{
	m_bRunning = false;

	clear();

	if (m_pCurl)
	{
		curl_easy_cleanup(m_pCurl);
		m_pCurl = NULL;
		//curl_global_cleanup();
	}

	if (m_pCallbackParam!=nullptr)
	{
		delete m_pCallbackParam;
		m_pCallbackParam = nullptr;
	}
}

void CDownloader::setOption()
{

	//设置认证
	curl_easy_setopt(m_pCurl, CURLOPT_FAILONERROR, true);//显示HTTP状态码，默认行为是忽略编号小于等于400的HTTP信息。

	// 远程URL，支持 http, https, ftp
	curl_easy_setopt(m_pCurl, CURLOPT_URL, m_strDownloadUrl.c_str());

	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);

	// https
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, false);//设定为不验证证书和HOST  
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, false);

	curl_easy_setopt(m_pCurl, CURLOPT_REFERER, "http://zhushou.4399.com");

	// 设置301、302跳转跟随location
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);

	// 下载内容回调函数
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteDataCallBack);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

	// 设置返回头部回调
	curl_easy_setopt(m_pCurl, CURLOPT_HEADERFUNCTION, ResponseHeadCallBack);
	curl_easy_setopt(m_pCurl, CURLOPT_HEADERDATA, this);

	curl_easy_setopt(m_pCurl, CURLOPT_LOW_SPEED_LIMIT, 1024*1); //设置一个长整形数，控制传送多少字节
	curl_easy_setopt(m_pCurl, CURLOPT_LOW_SPEED_TIME, 5);   //设置一个长整形数，控制多少秒传送CURLOPT_LOW_SPEED_LIMIT规定的字节数。

	// 设置重定向的最大次数
	curl_easy_setopt(m_pCurl, CURLOPT_MAXREDIRS, 5);

	// 进度回调函数
	curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, handleProgress);

	
	// 设置默认的超时时间
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 10);//连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
	//curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, 60*60*24);//接收数据时超时设置，如果10秒内数据未接收完，直接退出
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, 0);
	// 跳过服务器SSL验证，不使用CA证书
	//curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	//curl_easy_setopt(m_pCurl, CURLOPT_HEADER, 1);//下载数据包括HTTP头部 	
	//curl_easy_setopt(m_pCurl, CURLOPT_NOBODY, 1L);//不需body
	//curl_easy_setopt(m_pCurl, CURLOPT_RANGE, "0-500");//用于断点续传, 设置下载的分片 


	// 验证服务器端发送的证书，默认是 2(高)，1（中），0（禁用）
	//curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);



}

bool CDownloader::run()
{
	while (m_bPause)
	{
		Sleep(100);
	}

	CLibcurl  liburl;
	liburl.setReferer("http://zhushou.4399.com");
	std::string strhead = liburl.ReadHeader(m_strDownloadUrl.c_str());
	std::map<string, string> mapHeader;
	liburl._ParseHead((char *)strhead.c_str(), strhead.length(), mapHeader);

	 m_Content_Length = (double)atoll(mapHeader["Content-Length"].c_str());
	 if (0 == m_Content_Length)
	 {
		 UpdateState(DOWN_FAIL);
		 m_bRunning = false;
		 return false;
	 }

	double  nCurSize = NFile::GetFileSize(m_strLocalFilePath.c_str());

	//匹对文件大小，不一致就删除,一致就不进行下载
	if (nCurSize == m_Content_Length  && m_Content_Length >1)
	{
		UpdateState(CURLE_OK);
		return true;
	}
	else
	{
		//删除之前的文件 
		remove(m_strLocalFilePath.c_str());

		m_strTemp = m_strLocalFilePath + ".tmp";

		double nTempSize = NFile::GetFileSize(m_strTemp.c_str());

		if (nTempSize == m_Content_Length)
		{
			rename(m_strTemp.c_str(), m_strLocalFilePath.c_str());
			UpdateState(CURLE_OK);
			return true;
		}
		else if (nTempSize > m_Content_Length)
		{
			remove(m_strTemp.c_str());
		}

		SetResumeFromLarge(nTempSize);
		// 创建文件
		fopen_s(&m_pFile, m_strTemp.c_str(), "ab+");

		if (m_pFile == NULL)
		{
			return false;
		}
	}

	bool bres= onDownload();

	return bres;
}

bool CDownloader::onDownload()
{
	m_dltotal = 0;

	m_dlnow = 0;

	// 执行下载
	CURLcode return_code = CURLE_OK;

	return_code = curl_easy_perform(m_pCurl);

	// 关闭文件
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	// 下载失败
	if (return_code != CURLE_OK)
	{
		if (isRunning() == false)
		{
			if (GetDeleteFlag())
			{
				DelTempFile();
			}

			UpdateState(return_code);
		}		
		//remove(m_strTemp.c_str());	
		return false;

	}	
	
	//LOG_INFO("%.2f / %.2f (%2.4g %%)", m_dlnow, m_dltotal, m_dlnow*100.0 / m_dltotal);
	
	// 获取状态码
	int response_code = 0;
	curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &response_code);

	//LOG_INFO("return_code=[%d],response_code=[%d],m_dltotal=[%.2f],m_Content_Length=[%.2f] ", return_code, response_code, m_dltotal, m_Content_Length);

	if (response_code < 200 || response_code > 206)
	{

		return false;
	}

	if (return_code == CURLE_OK && m_dltotal - m_Content_Length<0.00001 &&m_dltotal - m_Content_Length>-0.00001)
	{
		rename(m_strTemp.c_str(), m_strLocalFilePath.c_str());
		UpdateState(return_code);
		return true;
	}
	else
	{
		return false;
	}


}

bool CDownloader::isRunning() const
{
	return m_bRunning;
}

int CDownloader::getState() const
{
	return m_nStates.load();
}

void CDownloader::clear()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}	
	m_strDownloadUrl.clear();
	m_strLocalFilePath.clear();
	m_nStartSize = 0;
}

bool CDownloader::start(const std::string& strUrl, const std::string& strLocalFile)
{
	if (strUrl.empty()) return false;
	

	if (m_bRunning == true) return false;

	clear();

	m_strDownloadUrl = strUrl;

	m_strLocalFilePath = strLocalFile;


	if (m_pCurl == NULL)
	{
		return false;
	}

	// 设置libcurl的选项
	setOption();

	m_bRunning = true;

	UpdateState(DOWNLOADSTATES::DOWN_START);

	return true;
}

bool CDownloader::stop()
{
	m_bRunning = false;

	clear();

	UpdateState(DOWNLOADSTATES::DOWN_STOP);

	return true;
}

void CDownloader::DelTempFile()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	remove(m_strTemp.c_str());
}

void CDownloader::SetDeleteFlag(bool bDelete)
{
	m_bDelete = bDelete;
}

int CDownloader::pause()
{
	double dProgress = m_dlnow / m_dltotal * 100;
	if (!(dProgress > 99))
	{
		m_bPause = true;

		m_curlCode = curl_easy_pause(m_pCurl, CURLPAUSE_RECV);
		UpdateState(DOWNLOADSTATES::DOWN_PAUSE);
	}

	return 1;
}

int CDownloader::resume()
{
	m_bPause = false;
	m_curlCode = curl_easy_pause(m_pCurl, CURLPAUSE_RECV_CONT);
	UpdateState(DOWNLOADSTATES::DOWN_CONTINUE);	
	return 1;
}

size_t CDownloader::WriteDataCallBack(void *buffer, size_t size, size_t nmemb, void *userp)
{
	CDownloader* pDownloader = (CDownloader*)userp;
	if (pDownloader)
	{
		if (pDownloader->isRunning() == false)
		{
			return 0;
		}
		return pDownloader->onWrite(buffer, size, nmemb);
	}
	return 0;
}

size_t CDownloader::onWrite(void *buffer, size_t size, size_t nmemb)
{
	size_t return_size = { 0 };
	if (m_pFile != NULL)
	{
		return_size = fwrite(buffer, size, nmemb, m_pFile);
		return return_size;
	}
	return 0;
}

size_t CDownloader::handleProgress(void *buffer, double dltotal, double dlnow, double ultotal, double ulnow)
{
	

	CDownloader* pDownloader = (CDownloader*)buffer;
	if (pDownloader)
	{
		if (pDownloader->isRunning() == false)
		{
			//1. 返回非0值就会终止 curl_easy_perform 执行  
			return -1;
		}
		while (pDownloader->m_bPause )
		{
			if (pDownloader->isRunning() == false)
			{
				//1. 返回非0值就会终止 curl_easy_perform 执行  
				return -1;
			}
			Sleep(10);
		}	
		pDownloader->onProgress(dltotal, dlnow);
	}
	return 0;
	/*
	char timeFormat[50] = "Unknow";
	double speed;
	string unit = "B";
	curl_easy_getinfo(pDownloader->m_pCurl, CURLINFO_SPEED_DOWNLOAD, &speed);

	if (speed != 0)
	{
		// Time remaining
		double leftTime = (dltotal - dlnow) / speed;
		int hours = leftTime / 3600;
		int minutes = (leftTime - hours * 3600) / 60;
		int seconds = leftTime - hours * 3600 - minutes * 60;

#ifdef _WIN32
		sprintf_s(timeFormat, 50, "lefttime:%02d:%02d:%02d   ", hours, minutes, seconds);
		OutputDebugStringA(timeFormat);
#else
		sprintf(timeFormat, "%02d:%02d:%02d", hours, minutes, seconds);
#endif
	}

	if (speed > 1024 * 1024 * 1024)
	{
		unit = "G";
		speed /= 1024 * 1024 * 1024;
	}
	else if (speed > 1024 * 1024)
	{
		unit = "M";
		speed /= 1024 * 1024;
	}
	else if (speed > 1024)
	{
		unit = "kB";
		speed /= 1024;
	}
	char szBuf[256] = { 0 };
	sprintf_s(szBuf, 256,"speed:%.2f%s/s \r\n", speed, unit.c_str());
	OutputDebugStringA(szBuf);


	return 0;
	*/
}

size_t CDownloader::onProgress(double dltotal, double dlnow)
{

	if (dltotal < 1 || dlnow < 1)
		return 0;

	m_dlnow.store(dlnow + m_nStartSize);

	m_dltotal.store(dltotal + m_nStartSize);

	UpdateState(DOWNLOADSTATES::DOWN_LOADING);

	char szBuf[256] = { 0 };
	
	//sprintf_s(szBuf, 256, "Progress：%.2f M / %.2f M (%2.4g %%)\n", m_dlnow/1024/1024, m_dltotal/1024/1024, m_dlnow*100.0 / m_dltotal);
	//OutputDebugStringA(szBuf);
	//LOG_INFO("Progress：%.2f / %.2f (%2.4g %%)\n", m_dlnow, m_dltotal, m_dlnow*100.0 / m_dltotal);
	return 0;
}

// 返回头部信息回调
size_t CDownloader::ResponseHeadCallBack(void *pData, size_t sSize, size_t sMem, void *pParam)
{
	CDownloader*				pThis = (CDownloader *)pParam;
	size_t				sCurLen = sSize * sMem;

	if (NULL == pThis || 0 >= sCurLen || NULL == pData)
	{
		return sCurLen;
	}

	pThis->m_strHead.append((char *)pData, sCurLen);

	return sCurLen;

}

bool CDownloader::SetResumeFromLarge(double llPos)
{
	if (llPos < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM_LARGE, (LONGLONG) llPos);
	m_nStartSize = llPos;
	return CURLE_OK == m_curlCode;
}

bool  CDownloader::setSpeed(long limitSpeed)
{
	if (limitSpeed < 0 || limitSpeed == 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)limitSpeed * 1024);
	return CURLE_OK == m_curlCode;
}

void CDownloader::SetCache(const char * pCachePath)
{
	m_strCachePath = pCachePath;
	if (!NFile::IsDirectory(m_strCachePath.c_str()))
	{
		NFile::CreateDir(m_strCachePath.c_str());
	}

}

void CDownloader::UpdateState(int nStates)
{
	if (nStates != DOWNLOADSTATES::DOWN_LOADING)
	{
		m_nStates.store(nStates);
	
	}

	//char szBuf[256] = { 0 };
	//sprintf_s(szBuf, 256, "nStates=[%d]\n", nStates);
	//OutputDebugStringA(szBuf);

	if (m_callbackfuc!=nullptr)
	{
		m_callbackfuc(m_pCallbackParam, m_dltotal, m_dlnow, nStates);
	}
}

//设置回调
void  CDownloader::setCallback(CallBackFunc callbackfuc, void* lpParam)
{
	m_callbackfuc = callbackfuc;
	m_pCallbackParam = lpParam;

}

void CDownloader::SetCallback(CLibcurlCallback* pCallback, void* lpParam)
{
	m_pCallback = pCallback;
	m_pCallbackParam = lpParam;
}
