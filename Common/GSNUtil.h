

#ifndef GSNUtilH
#define GSNUtilH

#include <windows.h>
#include <string>
#include <vector>
#define  _FILETIME_PER_SECOND_		1 * 10 * 1000 * 1000 

#define  _SIZE_1K					1024
#define  _SIZE_1M					_SIZE_1K * 1024
#define	 _SIZE_1G					_SIZE_1M * 1024
#define	 _SIZE_1T					_SIZE_1G * 1024

#define arrayof(x)			( sizeof((x)) / sizeof((x)[0]) )

namespace GSNUtil
{
	// 内存数据转16进制
	unsigned long DataToHex(void *pMem, unsigned long ulMemSize, char* pHexStr, unsigned long ulHexSize);

	// 内存转16进制
	std::string DataToHex(std::string strData);

	// 16进制转内存数据
	unsigned long HexToData(char *pHexStr, unsigned long ulHexSize, void *pMem, unsigned long ulMemSize);

	// 16进制转内存
	std::string HexToData(std::string strHex);

	// 获取系统主版本号
	int		GetOsMajorVersion();

	// 获取系统次版本号
	int		GetOsMinorVersion();

	// 获取系统版本
	std::string	GetOsVersion();

	// 设置访问控制权限
	bool	SetSecurityDescriptorByString(SECURITY_DESCRIPTOR *pSd, std::string strSdString = "");

	// 注册文件
	bool	RegisterFile(std::string strFile, int nWait = INFINITE);
	bool	RegisterFile(std::wstring strFile, int nWait = INFINITE);

	// 反注册文件
	bool	UnRegisterFile(std::string strFile, int nWait = INFINITE);
	bool	UnRegisterFile(std::wstring strFile, int nWait = INFINITE);

	// 计算Hash
	unsigned int BKDRHash(std::string strBuf);

	// 启动进程
	HANDLE	RunProcess(std::string strExe, std::string strCmd = "", STARTUPINFOA *pSi = NULL, PROCESS_INFORMATION *pPi = NULL);

	// 启动进程
	unsigned long RunProcess2(std::string strExe, std::string strCmd = "", bool bHide = false, STARTUPINFOA *pSi = NULL, PROCESS_INFORMATION *pPi = NULL);

	// 根据进程ID判断进程是否存在
	bool	IsProcExist(unsigned long ulProcId);

	// 根据进程句柄判断进程是否存在
	bool	IsProcExistFromHandle(HANDLE hProc);

	// 根据进程名判断进程是否存在
	bool	IsProcExist(std::string strProcName);

	// 退出进程
	bool	KillProc(unsigned long ulProcId);

	// 退出进程
	bool	KillProc(std::string strProcName);

	// 运行程序
	bool	RunProcess3(std::string strExe, std::string strCmd = "", bool bWait = false, bool bHide = false, unsigned long *pulExitCode = NULL);

	// 运行程序
	bool	RunProcess5(std::string strExe, std::string strCmd = "", unsigned long ulWaitTime = 0, bool bHide = false, unsigned long *pulExitCode = NULL);

	// 运行程序
	bool	RunProcess4(std::string strExe, std::string strCmd = "", std::string strCurDir = "", 
							bool bWait = false, bool bHide = false, unsigned long *pulExitCode = NULL);

	// 后台运行程序
	bool	RunProcessBk(std::string strCmdLine, unsigned long *pExitCode, unsigned long ulTimeOut);

	// 根据进程名称获取进程ID
	bool	GetProcIdFromName(std::string strProcName, std::vector<unsigned long> &vecProcId);

	// 根据进程名称获取进程ID
	unsigned long GetProcIdFromName(std::string strProcName);

	// 检测tcp端口是否被占用(true 被占用)
	bool	CheckPortTcp(unsigned short usPort);

	// 检测udp端口是否被占用(true 被占用)
	bool	CheckPortUdp(unsigned short usPort);
	
	

	//-------------Service

	// 创建服务
	bool	CreateOneService(std::string strPath, std::string strName,
							unsigned long ulServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
							unsigned long ulStartType = SERVICE_AUTO_START,
							std::string strDisplayName = "");

	// 检测服务是否存在
	bool	CheckServiceExist(std::string strName);

	// 启动服务
	bool	StartUpService(std::string strName, int nWait = 10000);

	// 暂停服务
	bool	PauseService(std::string strName, bool bResume = false, int nWait = 10000);

	// 停止服务
	bool	StopService(std::string strName, int nWait = 10000);

	// 删除服务
	bool	DeleteService(std::string strName, int nWait = 10000);

	// 服务是否运行
	bool	CheckServiceRunning(std::string strName);

	// 获取本机IP地址
	std::string GetLocalIp();


	// 创建一个工作对象
	HANDLE GetJobObj();

	// 启动进程（加入工作集）
	unsigned long RunProcessEx(std::string strPath, std::string strCmd, HANDLE hJobObj, 
								bool bWait = false, bool bHide = false, int *pExitCode = NULL);


	// 是否是64位系统
	bool IsWow64();

	// 是否win7或更高
	BOOL IsWin7OrGreater();


	// 用户管理员权限运行
	bool RunAsAdmin(std::string strFile, std::string strCmd, bool bHide = false);
	
}

#endif