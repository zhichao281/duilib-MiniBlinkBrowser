#include "NFile.h"
#include <Wininet.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")


BOOL NFile::ExtrFileFromRes(UINT nID, LPCTSTR lpType, LPCTSTR lpPath)
{
	if (NULL == lpPath || _T('\0') == lpPath[0])
		return FALSE;

	HRSRC	hRsrcUnist	= NULL;
	HGLOBAL	hGlobal		= NULL;
	LPVOID	pData		= NULL;

	hRsrcUnist = FindResource(NULL, MAKEINTRESOURCE(nID), lpType);
	if (NULL == hRsrcUnist)
		return FALSE;

	hGlobal = LoadResource(NULL, hRsrcUnist);
	if (NULL == hGlobal)
		return FALSE;

	pData = LockResource(hGlobal);
	if (NULL == pData)
	{
		FreeResource(hGlobal);
		return FALSE;
	}


	HANDLE fp = CreateFile(lpPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	if(fp != INVALID_HANDLE_VALUE)
	{
		DWORD dwWrite	= 0;
		DWORD dwLen		= ::SizeofResource(NULL, hRsrcUnist);
		WriteFile (fp, pData, dwLen, &dwWrite, NULL);
	}

	CloseHandle(fp);			//关闭句柄
	FreeResource(hGlobal);		//释放内存

	return TRUE;
}

BOOL	NFile::DeleteDirTree(LPCTSTR lpDir, LPCTSTR exclusionFile)
{
	assert(lpDir != NULL && _T('\0') != lpDir[0]);

	if (NULL == lpDir || 0 == lpDir[0])
		return FALSE;

	WIN32_FIND_DATA		FindData;
	HANDLE				hFind;
	TCHAR				szFileName[MAX_PATH];
	TCHAR				szDelDir[MAX_PATH];
	BOOL				bFindMore;
	BOOL				bRetVal;
	int					nError;

	nError = ERROR_SUCCESS;
	ZeroMemory(szFileName, sizeof(szFileName));
	ZeroMemory(&FindData, sizeof(FindData));

	_tcscpy_s(szDelDir, MAX_PATH, lpDir);
	if(szDelDir[_tcslen(szDelDir) - 1] != _T('\\'))
	{
		_tcscat_s(szDelDir, MAX_PATH, _T("\\"));
	}

	_tcscpy_s(szFileName, MAX_PATH, szDelDir);
	_tcscat_s(szFileName, MAX_PATH, _T("*"));

	hFind = FindFirstFile(szFileName, &FindData);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		return RemoveDirectory(lpDir);	//移除空目录
	}

	for(bFindMore = TRUE;
		bFindMore;
		bFindMore = FindNextFile(hFind, &FindData))
	{
		if(_tcscmp(FindData.cFileName, _T(".")) == 0 ||
			_tcscmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}

		if((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_tcscpy_s(szFileName, MAX_PATH, szDelDir);
			_tcscat_s(szFileName, MAX_PATH, FindData.cFileName);
			DeleteDirTree(szFileName, exclusionFile);
		}
		else
		{
			if (NULL != exclusionFile && NULL != _tcsstr(FindData.cFileName, exclusionFile))
			{
				continue;
			}

			_tcscpy_s(szFileName, MAX_PATH, szDelDir);
			_tcscat_s(szFileName, MAX_PATH, FindData.cFileName);

			//去掉只读属性
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			{
				FindData.dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(szFileName, FindData.dwFileAttributes);
			}

			bRetVal = DeleteFile(szFileName);
			if(bRetVal == FALSE)
			{
				nError = GetLastError();
			}
		}
	}
	FindClose(hFind);

	bRetVal = RemoveDirectory(lpDir);
	if(bRetVal == FALSE && (nError == ERROR_SUCCESS))
	{
		nError = GetLastError();
	}

	if(nError != ERROR_SUCCESS)
	{
		SetLastError(nError);
		return FALSE;
	}

	return TRUE;
}

BOOL	NFile::DeleteDirFilesByPrefix(LPCTSTR lpDir, LPCTSTR lpPrefix)
{
	assert(lpDir != NULL && _T('\0') != lpDir[0]);

	WIN32_FIND_DATA		FindData;
	HANDLE				hFind;
	TCHAR				szFileName[MAX_PATH];
	TCHAR				szDelDir[MAX_PATH];
	BOOL				bFindMore;
	BOOL				bRetVal;
	int					nError;

	nError = ERROR_SUCCESS;
	ZeroMemory(szFileName, sizeof(szFileName));
	ZeroMemory(&FindData, sizeof(FindData));

	_tcscpy_s(szDelDir, MAX_PATH, lpDir);
	if (szDelDir[_tcslen(szDelDir) - 1] != _T('\\'))
	{
		_tcscat_s(szDelDir, MAX_PATH, _T("\\"));
	}

	_tcscpy_s(szFileName, MAX_PATH, szDelDir);
	_tcscat_s(szFileName, MAX_PATH, _T("*"));

	hFind = FindFirstFile(szFileName, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return RemoveDirectory(lpDir);	//移除空目录
	}

	for (bFindMore = TRUE;
		bFindMore;
		bFindMore = FindNextFile(hFind, &FindData))
	{
		if (_tcscmp(FindData.cFileName, _T(".")) == 0 ||
			_tcscmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}

		_tcscpy_s(szFileName, MAX_PATH, szDelDir);
		_tcscat_s(szFileName, MAX_PATH, FindData.cFileName);
		if (_tcsstr(FindData.cFileName, lpPrefix) == FindData.cFileName)
		{
			if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				DeleteDirTree(szFileName);
			}
			else
			{
				//去掉只读属性
				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					FindData.dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
					SetFileAttributes(szFileName, FindData.dwFileAttributes);
				}

				bRetVal = DeleteFile(szFileName);
				if (bRetVal == FALSE)
				{
					nError = GetLastError();
				}
			}
		}
	}
	FindClose(hFind);

	bRetVal = RemoveDirectory(lpDir);
	if (bRetVal == FALSE && (nError == ERROR_SUCCESS))
	{
		nError = GetLastError();
	}

	if (nError != ERROR_SUCCESS)
	{
		SetLastError(nError);
		return FALSE;
	}

	return TRUE;
}

BOOL	NFile::GetFilePathInDir(LPCTSTR lpDir, std::vector<NString> &vFilePaths)
{
	assert(NULL != lpDir && _T('\0') != lpDir[0]);

	if (NULL == lpDir || 0 == lpDir[0])
		return FALSE;

	WIN32_FIND_DATA		FindData;
	HANDLE				hFind;
	TCHAR				szDirCpy[MAX_PATH] = {0};
	BOOL				bFindMore;

	ZeroMemory(&FindData, sizeof(FindData));
	_tcscpy_s(szDirCpy, MAX_PATH, lpDir);

	if (szDirCpy[_tcslen(szDirCpy)-1] != _T('\\'))
	{
		_tcscat_s(szDirCpy, MAX_PATH, _T("\\"));
	}

	_tcscat_s(szDirCpy, MAX_PATH, _T("*"));

	hFind = FindFirstFile(szDirCpy, &FindData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}

	for (bFindMore = TRUE;
		bFindMore;
		bFindMore = FindNextFile(hFind, &FindData))
	{
		if(_tcscmp(FindData.cFileName, _T(".")) == 0 ||
			_tcscmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}
		
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			TCHAR szSubDir[MAX_PATH] = {0};
			_tcscpy_s(szSubDir, MAX_PATH, szDirCpy);
			szSubDir[_tcslen(szSubDir)-1] = _T('\0');
			_tcscat_s(szSubDir, MAX_PATH, FindData.cFileName);
			GetFilePathInDir(szSubDir, vFilePaths);
		}
		else
		{
			NString strPath(szDirCpy);
			strPath.erase(strPath.length()-1, 1);	//删除"*"
 			strPath += FindData.cFileName;
 			vFilePaths.push_back(strPath);
		}
	}

	FindClose(hFind);

	return TRUE;
}

BOOL NFile::SplitPathBySlash(LPCTSTR lpPath, LPTSTR lpDirTree, LPTSTR lpFileName/* =NULL */)
{
	if (NULL == lpPath)
		return FALSE;

	const TCHAR *p = _tcsrchr(lpPath, _T('\\'));

	if (NULL == p)
		p = _tcsrchr(lpPath, _T('/'));
	if (NULL == p)
		return FALSE;

	if (NULL != lpDirTree)
		_tcsncpy_s(lpDirTree, p-lpPath+1, lpPath, p-lpPath);

	if (NULL != lpFileName)
		_tcscpy_s(lpFileName, lpPath + _tcslen(lpPath) - p, p+1);

	return TRUE;
}

BOOL NFile::CreateDirTree(LPCTSTR lpDir)
{
	if (NULL == lpDir || _tcslen(lpDir)<=3)
	{
		return FALSE;
	}

	TCHAR szParentDir[MAX_PATH] = {0};
	SplitPathBySlash(lpDir, szParentDir);	//取得父目录

	if (FILE_ATTRIBUTE_DIRECTORY == GetFileAttributes(lpDir))
		return TRUE;

	CreateDirTree(szParentDir);				//先生成上级目录
	CreateDirectory(lpDir, NULL);

	return TRUE;
}

BOOL NFile::CreateDirTree(LPCSTR lpDir)
{
	if (NULL == lpDir || strlen(lpDir) <= 3)
	{
		return FALSE;
	}
	std::string			strDir;

	if (IsDirectory(lpDir))
	{
		return true;
	}

	strDir = GetDir(lpDir);

	if (!CreateDirTree(strDir.c_str()))
	{
		return false;
	}

	if (IsDirectory(lpDir))
	{
		return true;
	}

	if (!::CreateDirectoryA(lpDir, NULL))
	{
		return false;
	}

	return true;
}

/*
*	Function:		GetDir
*	Explanation:	获取所在目录路径
*	Input:			strPath		路径
*	Return:			所在目录路径，失败返回""
*/
std::string  NFile::GetDir(std::string strPath)
{
	// "\" -> "\\"
	ReplaceStr(strPath, "/", "\\");

	char*				pFind = NULL;
	char*				pStart = NULL;

	if (strPath.empty())
	{
		return "";
	}

	pStart = (char *)strPath.c_str();
	pFind = strrchr(pStart, '\\');

	if (NULL == pFind)
	{
		pFind = strrchr(pStart, '/');
	}

	if (NULL == pFind)
	{
		return strPath;
	}

	return strPath.substr(0, pFind - pStart);
}

BOOL	NFile::GetFilePathInModule(TCHAR *pFilePath, TCHAR *pFileName, HMODULE hMod)
{
	TCHAR	szModPath[MAX_PATH];
	TCHAR*	pFind;

	GetModuleFileName(hMod, szModPath, MAX_PATH);
	pFind = _tcsrchr(szModPath, _T('\\'));
	if(pFind == NULL)
	{
		pFilePath[0] = 0;
		return FALSE;
	}

	_tcscpy_s(pFilePath, MAX_PATH, szModPath);
	if(pFileName)
	{
		pFilePath[pFind-szModPath+1] = 0;
		_tcscat_s(pFilePath, MAX_PATH, pFileName);
	}

	return TRUE;
}

int NFile::GetFileCountInDir(LPCTSTR lpDir)
{
	assert(NULL != lpDir && _T('\0') != lpDir[0]);

	if (NULL == lpDir || 0 == lpDir[0])
		return FALSE;

	int					nFileCount			= 0;	// 目录下文件数量
	WIN32_FIND_DATA		FindData;
	HANDLE				hFind;
	TCHAR				szDirCpy[MAX_PATH]	= {0};
	BOOL				bFindMore;

	ZeroMemory(&FindData, sizeof(FindData));
	_tcscpy_s(szDirCpy, MAX_PATH, lpDir);

	if (szDirCpy[_tcslen(szDirCpy)-1] != _T('\\'))
	{
		_tcscat_s(szDirCpy, MAX_PATH, _T("\\"));
	}

	_tcscat_s(szDirCpy, MAX_PATH, _T("*"));

	hFind = FindFirstFile(szDirCpy, &FindData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return TRUE;
	}

	for (bFindMore = TRUE;
		bFindMore;
		bFindMore = FindNextFile(hFind, &FindData))
	{
		if(_tcscmp(FindData.cFileName, _T(".")) == 0 ||
			_tcscmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			TCHAR szSubDir[MAX_PATH] = {0};
			_tcscpy_s(szSubDir, MAX_PATH, szDirCpy);
			szSubDir[_tcslen(szSubDir)-1] = _T('\0');
			_tcscat_s(szSubDir, MAX_PATH, FindData.cFileName);
			nFileCount += GetFileCountInDir(szSubDir);
		}
		else
		{
			nFileCount++;
		}
	}

	FindClose(hFind);

	return nFileCount;
}

__int64 NFile::GetFolderSize(const std::wstring& strDir)
{
	__int64 nSize = 0;
	std::wstring strRootPath = strDir + L"\\";
	std::wstring strRoot = strRootPath + L"*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strRoot.c_str(), &fd);
	if (INVALID_HANDLE_VALUE == hFind)
		return nSize;
	while (FindNextFile(hFind, &fd))
	{
		if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
			continue;
		std::wstring strPath = strRootPath + fd.cFileName;
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			nSize += GetFolderSize(strPath);
		else
		{
			HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile)
				continue;
			LARGE_INTEGER size;
			if (::GetFileSizeEx(hFile, &size))
			{
				nSize += size.QuadPart;
			}
			CloseHandle(hFile);
		}
	}
	FindClose(hFind);
	return nSize;
}


BOOL NFile::IsDirectory(LPCTSTR lpDir)
{
	DWORD		dwAttr;

	dwAttr = GetFileAttributes(lpDir);
	if (dwAttr == INVALID_FILE_ATTRIBUTES)
		return FALSE;

	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;

	return FALSE;
}
BOOL NFile::IsDirectory(LPCSTR pDir)
{
	DWORD		dwAttr;

	dwAttr = GetFileAttributesA(pDir);
	if (dwAttr == INVALID_FILE_ATTRIBUTES)
		return FALSE;

	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;

	return FALSE;
}




BOOL NFile::IsFileExist(LPCTSTR lpPath)
{
	DWORD dwAttrib = GetFileAttributes(lpPath);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BYTE* NFile::GetFileData(LPCTSTR pFileName, DWORD * pdwSize)
{
	BY_HANDLE_FILE_INFORMATION	HFileInfo;
	HANDLE						hFile;
	BOOL						bRetVal;
	DWORD						dwSize, dwRead;
	CHAR						* MainBuf;

	*pdwSize = 0;
	hFile = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
		NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	bRetVal = GetFileInformationByHandle(hFile, &HFileInfo);
	if(bRetVal == FALSE)
	{
		CloseHandle(hFile);
		return NULL;
	}

	if(HFileInfo.nFileSizeHigh != 0)
	{
		CloseHandle(hFile);
		return NULL;
	}

	dwSize = HFileInfo.nFileSizeLow;
	if(pdwSize)
		*pdwSize = dwSize;

	MainBuf = (CHAR *)malloc(dwSize+2);
	if(MainBuf == NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	bRetVal = ::ReadFile(hFile, MainBuf, dwSize, &dwRead, NULL);
	if(bRetVal == FALSE || dwRead != dwSize)
	{
		CloseHandle(hFile);
		free(MainBuf);
		return NULL;
	}

	MainBuf[dwSize] = 0;
	MainBuf[dwSize+1] = 0;
	CloseHandle(hFile);
	return (BYTE *)MainBuf;
}

BOOL NFile::DeleteInternetCache()
{
	BOOL						bRet			= FALSE;
	DWORD						dwEntrySize		= 0;
	LPINTERNET_CACHE_ENTRY_INFO	lpCacheEntry	= NULL;  
	HANDLE hEntry;
	TCHAR szPath[MAX_PATH];

	//delete the files
	hEntry = FindFirstUrlCacheEntry(NULL, NULL, &dwEntrySize);
	lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new TCHAR[dwEntrySize];
	hEntry = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwEntrySize);

	if (!hEntry)
		goto cleanup;

	do
	{
		if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
		{
			DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
		}

		dwEntrySize = 0;
		FindNextUrlCacheEntry(hEntry, NULL, &dwEntrySize);
		delete [] lpCacheEntry; 
		lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new TCHAR[dwEntrySize];
	} while ( FindNextUrlCacheEntry(hEntry, lpCacheEntry, &dwEntrySize) );

cleanup:
	if (lpCacheEntry)
	{
		delete [] lpCacheEntry; 
	}

	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_INTERNET_CACHE, FALSE))
	{
		bRet = NFile::DeleteDirTree(szPath);
	}

	return bRet;
}

BOOL NFile::DeleteInternetTempFile(LPCTSTR lpName/* =NULL */)
{
	BOOL	bResult		= FALSE;
	BOOL	bDone		= FALSE;
	DWORD	dwTrySize	= 0;
	DWORD	dwEntrySize	= 1024; // start buffer size
	HANDLE	hCacheDir	= NULL;
	DWORD	dwError		= ERROR_INSUFFICIENT_BUFFER;

	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;
	TCHAR	szName[128]	= {0};

	if (NULL != lpName)
	{
		_tcscpy_s(szName, 128, lpName);
		_tcslwr_s(szName, 128);
	}

	do
	{
		switch (dwError)
		{
			// need a bigger buffer
		case ERROR_INSUFFICIENT_BUFFER:
			delete [] lpCacheEntry;

			lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
			lpCacheEntry->dwStructSize = dwEntrySize;
			dwTrySize = dwEntrySize;

			BOOL bSuccess;
			
			if (hCacheDir == NULL)
				bSuccess = (hCacheDir = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwTrySize)) != NULL;
			else
				bSuccess = FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize);

			if (bSuccess)
			{
				dwError = ERROR_SUCCESS;
			}
			else
			{
				dwError = GetLastError();
				dwEntrySize = dwTrySize; // use new size returned
			}
			break;

			// we are done
		case ERROR_NO_MORE_ITEMS:
			bDone = TRUE;
			bResult = TRUE;
			break;

			// we have got an entry
		case ERROR_SUCCESS:

			// don't delete cookie entry
			if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
			{
				if (NULL != lpName)
				{
					int nLen = _tcslen(lpCacheEntry->lpszSourceUrlName) + 1;
					TCHAR *pUrlName = new TCHAR[nLen];

					_tcscpy_s(pUrlName, nLen, lpCacheEntry->lpszSourceUrlName);
					_tcslwr_s(pUrlName, nLen);

					if (NULL != _tcsstr(pUrlName, szName))
						DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);

					delete [] pUrlName;
				}
				else
				{
					DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
				}
			}

			// get ready for next entry
			dwTrySize = dwEntrySize;
			if (FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize))
			{
				dwError = ERROR_SUCCESS;
			}
			else
			{
				dwError = GetLastError();
				dwEntrySize = dwTrySize; // use new size returned
			}
			break;

			// unknown error
		default:
			bDone = TRUE;
			break;
		}

		if (bDone)
		{
			delete [] lpCacheEntry;
			if (hCacheDir)
				FindCloseUrlCache(hCacheDir);
		}

	} while (!bDone);

	return bResult;
}

BOOL NFile::SetInternetCacheLimit(int size)
{
	HKEY hKey;
	const TCHAR* szSubKey = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\5.0\\Cache\\Content");
	LSTATUS ret = ::RegOpenKeyEx(HKEY_CURRENT_USER, szSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if( ret != ERROR_SUCCESS )
		return FALSE;

	ret = ::RegSetValueEx(hKey, TEXT("CacheLimit"), 0, REG_DWORD, (LPBYTE)&size, sizeof(size));
	if( ret != ERROR_SUCCESS )
		return FALSE;

	ret = ::RegCloseKey(hKey);
	if( ret != ERROR_SUCCESS )
		return FALSE;

	return TRUE;
}

BOOL NFile::DeleteFlashCache()
{
	TCHAR			szCache[MAX_PATH]	= {0};
	LPITEMIDLIST	pidl;
	LPMALLOC		pShellMalloc;

	if(SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{	
		if(SUCCEEDED(SHGetSpecialFolderLocation(NULL,CSIDL_APPDATA ,&pidl))) 
		{
			SHGetPathFromIDList(pidl, szCache);
			pShellMalloc->Free(pidl);
		}

		pShellMalloc->Release();
	}

	_tcscat_s(szCache, MAX_PATH, _T("\\Macromedia\\Flash Player\\#SharedObjects"));

	return DeleteDirTree(szCache);
}

BOOL NFile::DeleteInternetCookie()
{
	BOOL bRet = FALSE;
	HANDLE hEntry;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;  
	DWORD dwEntrySize;

	//delete the files
	dwEntrySize = 0;
	hEntry = FindFirstUrlCacheEntry(NULL, NULL, &dwEntrySize);
	lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
	hEntry = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwEntrySize);
	if (!hEntry)
	{
		goto cleanup;
	}

	do
	{
		if (lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY)
		{
			DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
		}

		dwEntrySize = 0;
		FindNextUrlCacheEntry(hEntry, NULL, &dwEntrySize);
		delete [] lpCacheEntry; 
		lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
	}
	while (FindNextUrlCacheEntry(hEntry, lpCacheEntry, &dwEntrySize));

	bRet = TRUE;
cleanup:
	if (lpCacheEntry)
	{
		delete [] lpCacheEntry; 
	}

	//得到目录，并清空
	TCHAR szPath[MAX_PATH];
	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_COOKIES, FALSE))
	{
		DeleteDirTree(szPath);
	}

	return bRet;
}

BOOL	NFile::CopyDirTree(LPCTSTR lpSrc, LPCTSTR lpDest)
{
	assert(lpSrc != NULL && _T('\0') != lpSrc[0]);
	assert(lpDest != NULL && _T('\0') != lpDest[0]);

	WIN32_FIND_DATA		FindData;
	HANDLE				hFind;
	TCHAR				szFileName[MAX_PATH];
	TCHAR				szSrcDir[MAX_PATH];
	TCHAR				szDestDir[MAX_PATH];
	TCHAR               szDestFile[MAX_PATH];
	BOOL				bFindMore;
	BOOL				bRetVal;
	int					nError;

	nError = ERROR_SUCCESS;
	ZeroMemory(szFileName, sizeof(szFileName));
	ZeroMemory(szDestFile, sizeof(szDestFile));
	ZeroMemory(&FindData, sizeof(FindData));

	_tcscpy_s(szSrcDir, MAX_PATH, lpSrc);
	if (szSrcDir[_tcslen(szSrcDir) - 1] != _T('\\'))
	{
		_tcscat_s(szSrcDir, MAX_PATH, _T("\\"));
	}
	_tcscpy_s(szFileName, MAX_PATH, szSrcDir);
	_tcscat_s(szFileName, MAX_PATH, _T("*"));

	_tcscpy_s(szDestDir, MAX_PATH, lpDest);
	if (szDestDir[_tcslen(szDestDir) - 1] != _T('\\'))
	{
		_tcscat_s(szDestDir, MAX_PATH, _T("\\"));
	}

	CreateDirTree(lpDest);

	hFind = FindFirstFile(szFileName, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	for (bFindMore = TRUE;
		bFindMore;
		bFindMore = FindNextFile(hFind, &FindData))
	{
		if (_tcscmp(FindData.cFileName, _T(".")) == 0 ||
			_tcscmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}

		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_tcscpy_s(szFileName, MAX_PATH, szSrcDir);
			_tcscat_s(szFileName, MAX_PATH, FindData.cFileName);

			_tcscpy_s(szDestFile, MAX_PATH, szDestDir);
			_tcscat_s(szDestFile, MAX_PATH, FindData.cFileName);
			CopyDirTree(szFileName, szDestFile);
		}
		else
		{
			_tcscpy_s(szFileName, MAX_PATH, szSrcDir);
			_tcscat_s(szFileName, MAX_PATH, FindData.cFileName);

			_tcscpy_s(szDestFile, MAX_PATH, szDestDir);
			_tcscat_s(szDestFile, MAX_PATH, FindData.cFileName);

			bRetVal = CopyFile(szFileName, szDestFile, FALSE);
			if (bRetVal == FALSE)
			{
				nError = GetLastError();
			}
		}
	}
	FindClose(hFind);

	if (nError != ERROR_SUCCESS)
	{
		SetLastError(nError);
		return FALSE;
	}

	return TRUE;
}

BOOL NFile::OpenFolder(LPCTSTR lpPath)
{
	if (!IsDirectory(lpPath))
	{
		return FALSE;
	}
	ShellExecute(NULL, _T("open"), NString(lpPath).c_str(), NULL, NULL, SW_SHOWNORMAL);
	return TRUE;
}

BOOL NFile::GetFullPath(LPCTSTR lpShortPath, LPTSTR lpFullPath)
{
	if (lpShortPath && lpFullPath){
		int length = GetLongPathName(lpShortPath, 0, 0);
		length = GetLongPathName(lpShortPath, lpFullPath, length);
		return TRUE;
	}
	return FALSE;
}

NString NFile::GetSpecificDocumentDir(LPCTSTR dirName)
{
	LPITEMIDLIST	pidl = NULL;
	SHGetFolderLocation(NULL, CSIDL_MYDOCUMENTS, NULL, 0, &pidl);
	if (pidl)
	{
		TCHAR path[MAX_PATH] = { 0 };
		SHGetPathFromIDList(pidl, path);
		NString dir = path;
		dir = dir + _T("\\") + dirName + _T("\\");

		DWORD attr = GetFileAttributes(dir.c_str());
		if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if (CreateDirectory(dir.c_str(), NULL) == FALSE)
			{
				return _T("");
			}
		}

		return dir;
	}
	else
	{
		// TODO FIXME throw error
		return _T("");
	}
}

NString NFile::Get4399Dir()
{
	return GetSpecificDocumentDir(_T("4399"));
}

NString NFile::Get4399TempPath()
{
	TCHAR buffer[MAX_PATH + 1];
	if (GetTempPath(MAX_PATH + 1, buffer) > 0)
	{
		NString tmpdir = buffer;
		tmpdir = tmpdir + _T("4399\\");
		DWORD attr = GetFileAttributes(tmpdir.c_str());
		if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if (CreateDirectory(tmpdir.c_str(), NULL) == FALSE)
			{
				return _T("");
			}
		}
		return tmpdir;
	}
	else
	{
		return _T("");
	}

}

NString NFile::Get4399DownPath()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szCfgPath[MAX_PATH] = { 0 };

	NString defPath = NFile::Get4399Dir() + _T("DownloadCenter");
	NFile::GetFilePathInModule(szCfgPath, _T("config.ini"));
	GetPrivateProfileString(TEXT("Setting"), TEXT("DownPath"), defPath.c_str(), szPath, MAX_PATH, szCfgPath);
	return szPath;
}

NString NFile::GetBinDir()
{
	TCHAR path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	NString dir = path;
	if (dir.rfind(_T("\\")) != std::wstring::npos)
	{
		return dir.substr(0, dir.rfind(_T("\\")) + 1);
	}
	else
	{
		return _T("");
	}
}

NString NFile::GetBinFileName()
{
	TCHAR path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	return PathFindFileName(path);
}

NString NFile::GetUserProfileDir()
{
	LPITEMIDLIST	pidl = NULL;
	SHGetFolderLocation(NULL, CSIDL_PROFILE, NULL, 0, &pidl);
	if (pidl)
	{
		TCHAR path[MAX_PATH] = { 0 };
		SHGetPathFromIDList(pidl, path);
		NString dir = path;

		return dir;
	}
	else
	{
		// TODO FIXME throw error
		return _T("");
	}

}

std::vector<NString> NFile::GetVolumes()
{
	std::vector<NString> list;
	DWORD mask = ::GetLogicalDrives();
	unsigned int count = 0;
	while (mask)
	{
		if (mask & 1)
		{
			NString value;
			value = CHAR(97 + count);
			list.push_back(value);
		}
		mask = mask >> 1;
		++count;
	}
	return list;
}


/*
*	Function:		ReplaceStr
*	Explanation:	替换字符串中指定的字符串
*	Input:			strBuf		字符串
strSrc		需要替换的字符串
strDes		替换的字符串
*  Output:			strBuf		替换后的字符串
*	Return:			替换次数
*/
int NFile::ReplaceStr(std::string  &strBuf, std::string strSrc, std::string strDes)
{
	size_t				sBufSize = strBuf.size();
	char*				pStart = (char *)strBuf.c_str();
	char*				pEnd = pStart + sBufSize;
	std::string			strReturn;
	int					nCount = 0;

	if (strBuf.empty())
	{
		return strBuf.size();
	}

	for (;; Sleep(1))
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




/*
*	Function:		GetFileSize
*	Explanation:	获取文件大小
*	Input:			strFilePath		文件路径
*  Output:			pHighSize		高阶大小
*	Return:			低阶大小
*/
double NFile::GetFileSize(std::string strFilePath, unsigned long *pHighSize /* = NULL */)
{

	double filesize = 0;
	struct _stat64 buf;
	if (_stat64(strFilePath.c_str(), &buf) < 0)
	{
		filesize = 0;
	}
	else
	{
		filesize = (double)buf.st_size;		
	}
	return filesize;

	//FILE *stream;
	// fopen_s(&stream,strFilePath.c_str(), "r");
	//double curpos, length;
	//curpos = _ftelli64(stream);
	//_fseeki64(stream, 0L, SEEK_END);
	//length = _ftelli64(stream);
	//fclose(stream);
	//return length;

}
