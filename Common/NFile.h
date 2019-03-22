#if !defined(_AFX_COMMON_NFILE_H)
#define _AFX_COMMON_NFILE_H


#include <vector>
#include <string>

#ifdef NString
#undef NString
#endif

#ifdef UNICODE
#define NString std::wstring
#else
#define NString std::string
#endif

//using namespace std;


namespace NFile
{
	// 在工程资源文件中抽取文件
	// nID 表示资源文件ID  lpType文件类型  lpPaht 表示文件的保存路径
	BOOL ExtrFileFromRes(UINT nID, LPCTSTR lpType, LPCTSTR lpPath);


	// 删除目录树 lpDir为指向目录的指针
	BOOL	DeleteDirTree(LPCTSTR lpDir, LPCTSTR exclusionFile=NULL);
	BOOL	DeleteDirFilesByPrefix(LPCTSTR lpDir, LPCTSTR lpPrefix);
	// 获得目录下的所有文件路径
	BOOL	GetFilePathInDir(LPCTSTR lpDir, std::vector<NString> &vFilePaths);
	// 由路径提取目录和文件名
	BOOL	SplitPathBySlash(LPCTSTR lpPath, LPTSTR lpDirTree, LPTSTR lpFileName=NULL);
	
	// 生成目录树
	BOOL	CreateDirTree(LPCTSTR lpDir);
	// 生成目录树
	BOOL	CreateDirTree(LPCSTR lpDir);

	// 获取所在目录路径
	std::string	GetDir(std::string strPath);

	// 获取文件大小
	double GetFileSize(std::string strFilePath, unsigned long *pHighSize = NULL);
	

	// 获得文件在模块中的全路径
	BOOL	GetFilePathInModule(TCHAR *pFilePath, TCHAR *pFileName=NULL, HMODULE hMod=NULL);
	// 目录下文件的数量
	int		GetFileCountInDir(LPCTSTR lpDir);
	// 获取目录下文件大小总和
	__int64 GetFolderSize(const std::wstring& strDir);

	// 判断指定目录是否存在
	BOOL	IsDirectory(LPCTSTR lpDir);
	// 判断指定目录是否存在
	BOOL	IsDirectory(LPCSTR pDir);


	// 判断文件是否存在
	BOOL	IsFileExist(LPCTSTR lpPath);
	// 获取文件数据
	BYTE* GetFileData(LPCTSTR pFileName, DWORD * pdwSize);
	// 打开文件夹
	BOOL	OpenFolder(LPCTSTR lpPath);
	// 获取全路径
	BOOL	GetFullPath(LPCTSTR lpShortPath, LPTSTR lpFullPath);
	// 拷贝目录树 lpDir为指向目录的指针
	BOOL	CopyDirTree(LPCTSTR lpSrc, LPCTSTR lpDest);

	// 清除IE缓存
	BOOL	DeleteInternetCache();
	// 删除IE缓存中指定模糊文件名的文件
	BOOL	DeleteInternetTempFile(LPCTSTR lpName=NULL);
	// 清除IE Cookie
	BOOL	DeleteInternetCookie();
	// 设置IE缓存大小
	BOOL	SetInternetCacheLimit(int size);
	// 清除flash缓存
	BOOL	DeleteFlashCache();

	int ReplaceStr(std::string  &strBuf, std::string strSrc, std::string strDes);

	NString GetSpecificDocumentDir(LPCTSTR dirName);
	NString Get4399Dir();
	NString Get4399TempPath();
	NString Get4399DownPath();
	NString GetBinDir();
	NString GetBinFileName();
	NString GetUserProfileDir();
	std::vector<NString> GetVolumes();
}

#endif