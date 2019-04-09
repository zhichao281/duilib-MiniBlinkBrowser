#include "FileManange.h"
#include <Windows.h>
#include <direct.h>
#include <io.h>
#include <string>
#include <sys/stat.h>
#include <atlimage.h>
using namespace std;



NFile::NFile(void)
{
}


NFile::~NFile(void)
{
}

std::string NFile::LoadFile( const char* pFilePath )
{
	FILE* pFile = fopen(pFilePath, "rb");
	std::string strData;
	if (pFile)
	{
		int nReadSize = 1024;
		char szBuff[1024];
		int nLen = 0;
		while(1)
		{
			nLen = fread(szBuff, 1, 1024, pFile);
			if (nLen <= 0)
			{
				break;
			}
			strData += std::string(szBuff, nLen);
		}
	}
	fclose(pFile);
	return strData;
}

int NFile::WriteFile( const char* pFilePath, std::string strData, bool bAppend )
{
	FILE* pFile = fopen(pFilePath, bAppend ? "a" : "w");
	if (pFile)
	{
		fwrite(strData.data(), 1, strData.length(), pFile);
		fclose(pFile);
		return 0;
	}	
	return GetLastError();
}

int NFile::DelFile( const char* pFilePath )
{
	return remove(pFilePath);
}

int NFile::CpyFile( const char* pSrcFilePath, const char* pDescFilePath, bool bFailIfExist )
{
	return CopyFileA(pSrcFilePath, pDescFilePath, bFailIfExist);
}

DWORD CALLBACK CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,              // 文件总字节数
	LARGE_INTEGER TotalBytesTransferred,      // 已复制的字节数
	LARGE_INTEGER StreamSize,                 // 当前流的字节数
	LARGE_INTEGER StreamBytesTransferred,     // 当前流已拷贝的字节数
	DWORD dwStreamNumber,                     // 当前流序号
	DWORD dwCallbackReason,                   // 回调函数的状态; 下面有补充... 
	HANDLE hSourceFile,                       // 源文件句柄
	HANDLE hDestinationFile,                  // 目标文件句柄
	LPVOID lpData                             // CopyFileEx 传递的参数指针
	)
{
		//return PROGRESS_CANCEL;
		CopyFileCallbackFunction pFun = (CopyFileCallbackFunction)lpData;
		return pFun(TotalFileSize.QuadPart, TotalBytesTransferred.QuadPart);
};

int NFile::CpyFileEx( const char* pSrcFilePath, const char* pDescFilePath, bool bFailIfExist, CopyFileCallbackFunction pFunc )
{
	if (pFunc == nullptr)
	{
		return -1;
	}
	return CopyFileExA(pSrcFilePath, pDescFilePath, CopyProgressRoutine, pFunc, 0, 0);
}

int NFile::CreateDir( const char* pDir )
{
	int nLen = strlen(pDir);
	std::string strPath;

	for (int i = 0; i < nLen; i++){
		if (pDir[i] == '\\' || pDir[i] == '/'){
			strPath = std::string(pDir, &pDir[i] - pDir);			
			if (_access(strPath.c_str(), 0 != 0)){
				_mkdir(strPath.c_str());
			}
		}
	}
	if (pDir[nLen -1] != '\\' && pDir[nLen -1] != '/'){
		strPath = pDir;
		if (_access(strPath.c_str(), 0 )!= 0){
			_mkdir(strPath.c_str());
		}	
	}		
	return 0;
}

std::list<std::pair<std::string, bool>> NFile::GetDirFiles( const char* pDir )
{
	list<pair<string, bool>> lstFileInfo;
	WIN32_FIND_DATAA fd;
	HANDLE lFile;
	char szDir[1024] = "";
	int nLen = strlen(pDir);
	sprintf(szDir, "%s%s*.*", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\");

	lFile = FindFirstFileA(szDir, &fd);
	char szSub[1024];
	while(lFile != INVALID_HANDLE_VALUE)
	{
		if(FindNextFileA(lFile, &fd) == false){
			break;
		}
		if ((fd.dwFileAttributes & 0x10) == 0x10){						
			sprintf(szSub, "%s%s%s\\*.*", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
			if (string(fd.cFileName) != "." && string(fd.cFileName) != ".."){
				char szFileName[1024];
				sprintf(szFileName, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
				lstFileInfo.push_back(make_pair(szFileName, true));
				list<pair<string, bool>> listTemp = GetDirFiles(szFileName);
				for (list<pair<string, bool>>::iterator iter = listTemp.begin(); iter != listTemp.end(); iter++){
					lstFileInfo.push_back(*iter);
				}
			}			
		}else{
			char szFileName[1024];
			sprintf(szFileName, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
			lstFileInfo.push_back(make_pair(szFileName, false));
		}		
	}
	FindClose(lFile);
	return lstFileInfo;
}

bool NFile::CheckFileExist( const char* pFile )
{
	if (_access(pFile, 0) != 0){
		return false;
	}
	return true;
}

int NFile::DeleteDir( const char* pDir )
{
	char szPath[1024] = "";
	int nLen = strlen(pDir);
	sprintf(szPath, "%s%s*.*", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\");
	WIN32_FIND_DATAA fd;
	HANDLE lFile;
	lFile = FindFirstFileA(szPath, &fd);

	while(lFile != INVALID_HANDLE_VALUE){
		if (FindNextFileA(lFile, &fd) == false){
			break;
		}

		if (strcmp(fd.cFileName, "..") == 0 || strcmp(fd.cFileName, ".") == 0){
			continue;
		}

		if ((fd.dwFileAttributes & 0x10) == 0x10){
			char szSubDir[1024];
			sprintf(szSubDir, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);
			DeleteDir(szSubDir);
		}else{
			char szFile[1024];
			sprintf(szFile, "%s%s%s", pDir, (pDir[nLen - 1] == '\\' || pDir[nLen - 1] == '/') ? "" : "\\", fd.cFileName);			
			DelFile(szFile);
		}
	}	
	FindClose(lFile);
	return _rmdir(pDir);
}

long NFile::GetFileSize( const char* pFile )
{
	struct _stat info;
	_stat((char*)pFile, &info);
	return info.st_size;
}

//描述：获取当前程序所在路径
void NFile::GetRootDirectoryA(LPSTR pszRootDir)
{
	if(NULL != pszRootDir)
	{
		char szModuleFileName[MAX_PATH] = "";
		GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH);

		char* pRchr = strrchr(szModuleFileName, '\\');
		if(NULL == pRchr)
		{
			pRchr = strrchr(szModuleFileName, '/');
		}

		if(NULL != pRchr)
		{
			pRchr++;
			*pRchr = '\0';
		}

		strcpy(pszRootDir, szModuleFileName);
	}
}


//描述：获取当前程序所在路径
void  NFile::GetRootDirectoryW(LPTSTR pszRootDir)
{
	if(NULL != pszRootDir)
	{
		TCHAR szModuleFileName[MAX_PATH] = _T("");
		GetModuleFileName(NULL, szModuleFileName, MAX_PATH);

		TCHAR* pRchr = wcsrchr(szModuleFileName, '\\');
		if(NULL == pRchr)
		{
			pRchr = wcsrchr(szModuleFileName, '/');
		}

		if(NULL != pRchr)
		{
			pRchr++;
			*pRchr = '\0';
		}

		StrCpy(pszRootDir, szModuleFileName);
	}
}

std::wstring NFile::GetFileInModulePathW(std::wstring strFileName /* = L"" */, HMODULE hModule /* = NULL */)
{
	wchar_t				szModulePath[1024] = { 0 };
	unsigned long		ulSize = 0;
	wchar_t*			pFind = NULL;
	std::wstring			strReturn;

	ulSize = GetModuleFileNameW(hModule, szModulePath, sizeof(szModulePath) / sizeof(wchar_t) - 1);

	if (0 == ulSize)
	{
		return L"";
	}

	pFind = wcsrchr(szModulePath, '\\');

	if (NULL == pFind)
	{
		return L"";
	}

	*pFind = 0;

	if (strFileName.empty())
	{
		return szModulePath;
	}

	strReturn = szModulePath;
	strReturn.append(L"\\");
	strReturn.append(strFileName);

	return strReturn;
}

BOOL NFile::GetFilePathInModule(TCHAR *pFilePath, TCHAR *pFileName, HMODULE hMod)
{
	TCHAR	szModPath[MAX_PATH];
	TCHAR*	pFind;

	GetModuleFileName(hMod, szModPath, MAX_PATH);
	pFind = _tcsrchr(szModPath, _T('\\'));
	if (pFind == NULL)
	{
		pFilePath[0] = 0;
		return FALSE;
	}

	_tcscpy_s(pFilePath, MAX_PATH, szModPath);
	if (pFileName)
	{
		pFilePath[pFind - szModPath + 1] = 0;
		_tcscat_s(pFilePath, MAX_PATH, pFileName);
	}

	return TRUE;
}


BOOL NFile::GetFilePathInModule(CHAR *pFilePath, CHAR *pFileName, HMODULE hMod)
{
	CHAR	szModPath[MAX_PATH];
	CHAR*	pFind;

	GetModuleFileNameA(hMod, szModPath, MAX_PATH);
	pFind = strrchr(szModPath, '\\');
	if (pFind == NULL)
	{
		pFilePath[0] = 0;
		return FALSE;
	}

	strcpy_s(pFilePath, MAX_PATH, szModPath);
	if (pFileName)
	{
		pFilePath[pFind - szModPath + 1] = 0;
		strcat_s(pFilePath, MAX_PATH, pFileName);
	}

	return TRUE;
}

string NFile::GetImageHead(wstring wstrPath)
{
	string strFomat="";
	char pfile[4096];
	//std::wstring wstrPath = strPath.toStdWString();

	int nwstrPathLen = ::WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wstrPath.c_str(), -1, 0, 0, 0, 0);
	if (nwstrPathLen <= 0)
	{
		strFomat = "png";
	}
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wstrPath.c_str(), -1, pfile, nwstrPathLen, 0, 0);
	FILE *file=fopen(pfile,"rb");
	if (file)
	{
		char buf[5] ={0},buf1[5] ={0},buf2[5] ={0};		
		fread(buf,5,5,file);
		fclose(file);
		memcpy(buf1, buf, 3);
		memcpy(buf2, &buf[1], 3);
		std::string str1(buf1, 3);
		const char szJpgHead[] = {0xff, 0xd8, 0xff};
		const string strJpgHead(szJpgHead, 3);
		if (strJpgHead == str1)
		{
			strFomat = "jpg";
		}
		else if (strcmp(buf1,"GIF") == 0)
		{
			strFomat="gif";
		}
		else if (strcmp(buf2,"PNG") == 0)
		{
			strFomat="png";
		}
		else		
		{
			strFomat="bmp";
		}

	}
	fclose(file);
	return strFomat;
}

HRESULT NFile::SaveImage(wstring strLoadPath, wstring strSavePath)
{
	CImage image;
	HRESULT hResult = image.Load(strLoadPath.c_str());
	if (hResult == S_OK)
	{
		hResult = image.Save(strSavePath.c_str());
	}
	return hResult;
	//HRESULT hResult = image.Save(strPath.c_str());
}



/*
 *	Function:		GetModulePath
 *	Explanation:	获取程序运行路径
 *	Input:			hModule		模块
 *	Return:			程序运行路径
 */
std::string NFile::GetModulePath(HMODULE hModule /* = NULL */)
{
	char				szModulePath[1024] = { 0 };
	unsigned long		ulSize = 0;

	ulSize = GetModuleFileNameA(hModule, szModulePath, sizeof(szModulePath) - 1);

	if (0 == ulSize)
	{
		return "";
	}

	return szModulePath;
}

std::wstring NFile::GetModulePathW(HMODULE hModule /* = NULL */)
{

	TCHAR	szModPath[MAX_PATH];
	TCHAR*	pFind;
	TCHAR *pFilePath=new TCHAR[MAX_PATH];

	GetModuleFileName(hModule, szModPath, MAX_PATH);
	pFind = _tcsrchr(szModPath, _T('\\'));
	if (pFind == NULL)
	{
	
		return L"";
	}

	_tcscpy_s(pFilePath, MAX_PATH, szModPath);
	pFilePath[pFind - szModPath + 1] = 0;
	return pFilePath;
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

std::string NFile::GetRootDirectoryA()
{
	std::string strCurrentDir;
	char szModuleFileName[MAX_PATH] = "";
	GetModuleFileNameA(NULL, szModuleFileName, MAX_PATH);

	char* pRchr = strrchr(szModuleFileName, '\\');
	if (NULL == pRchr)
	{
		pRchr = strrchr(szModuleFileName, '/');
	}

	if (NULL != pRchr)
	{
		pRchr++;
		*pRchr = '\0';
	}
	strCurrentDir = szModuleFileName;
	return strCurrentDir;

}


std::wstring NFile::GetRootDirectoryW()
{

	std::wstring strCurrentDir;
	TCHAR szModuleFileName[MAX_PATH] = _T("");
	GetModuleFileName(NULL, szModuleFileName, MAX_PATH);

	TCHAR* pRchr = wcsrchr(szModuleFileName, '\\');
	if (NULL == pRchr)
	{
		pRchr = wcsrchr(szModuleFileName, '/');
	}

	if (NULL != pRchr)
	{
		pRchr++;
		*pRchr = '\0';
	}

	strCurrentDir = szModuleFileName;
	return strCurrentDir;

}