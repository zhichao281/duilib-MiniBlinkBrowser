#ifndef __FILE_MANAGE_H__
#define __FILE_MANAGE_H__

#include <string>
#include <list>


#include <Shlwapi.h>


#include <tchar.h>
using namespace std;


#ifdef UNICODE
#define GetRootDirectory GetRootDirectoryW
#else
#define GetRootDirectory GetRootDirectoryA
#endif





enum emCopyFileRes
{
	emContinue,
	emCancel,
	emStop,
	emQuiet
};

typedef emCopyFileRes(*CopyFileCallbackFunction)(long lTotalFileSize, long lTotalBytesTransferred);

class NFile
{
public:

	NFile(void);

	virtual ~NFile(void);

	/****************************/
	//读取文件内容
	//pFilePath 文件地址
	/****************************/
	static std::string LoadFile(const char* pFilePath);

	/****************************/
	//往文件写入内容
	//pFilePath 文件地址  strData：要写入的内容 bAppend 是否追加到文件末尾
	/****************************/
	static int WriteFile(const char* pFilePath, std::string strData, bool bAppend = true);

	/****************************/
	//删除文件
	//pFilePath ：文件地址
	/****************************/
	static int DelFile(const char* pFilePath);

	/****************************/
	//pSrcFilePath, // 你要拷贝的源文件名 
	//pDescFilePath, // 你要拷贝的目标文件名 
	//bFailIfExists // 如果目标已经存在，不拷贝（True）并返回False，覆盖目标（false）
	/****************************/
	static int CpyFile(const char* pSrcFilePath, const char* pDescFilePath, bool bFailIfExist = true);

	/****************************/
	//pSrcFilePath, // 你要拷贝的源文件名 
	//pDescFilePath, // 你要拷贝的目标文件名 
	//bFailIfExists // 如果目标已经存在，不拷贝（True）并返回False，覆盖目标（false）
	/****************************/
	static int CpyFileEx(const char* pSrcFilePath, const char* pDescFilePath, bool bFailIfExist, CopyFileCallbackFunction pFunc);

	static int CreateDir(const char* pDir);

	static int DeleteDir(const char* pDir);

	// 获取程序运行路径
	static std::string	GetModulePath(HMODULE hModule = NULL);

	static std::wstring	GetModulePathW(HMODULE hModule = NULL);

	static std::list<std::pair<std::string, bool>> GetDirFiles(const char* pDir);

	/************************/
	//描述：检测文件是否存在
	//pFile ：文件地址
	/************************/
	static bool CheckFileExist(const char* pFile);

	/************************/
	//描述：获取当前文件大小
	/************************/
	static long GetFileSize(const char* pFile);

	/************************/
	//描述：获取当前程序所在路径
	/************************/
	static void GetRootDirectoryA(LPSTR pszRootDir);





	//描述：获取当前程序所在路径

	static void GetRootDirectoryW(LPTSTR pszRootDir);

	static std::string GetRootDirectoryA();

	//描述：获取当前程序所在路径
	static std::wstring GetRootDirectoryW();



	// 获取路径
	static std::wstring	GetFileInModulePathW(std::wstring strFileName = L"", HMODULE hModule = NULL);
	
	static BOOL GetFilePathInModule(TCHAR *pFilePath, TCHAR *pFileName, HMODULE hMod);
	
	static BOOL GetFilePathInModule(CHAR *pFilePath,CHAR *pFileName, HMODULE hMod);



	//描述：获取图片的类型
	
	static string GetImageHead(wstring strPath);

	static HRESULT SaveImage(wstring strLoadPath,wstring strSavePath);


	// 判断指定目录是否存在
	static BOOL	IsDirectory(LPCTSTR lpDir);
	// 判断指定目录是否存在
	static BOOL	IsDirectory(LPCSTR pDir);


};

#endif