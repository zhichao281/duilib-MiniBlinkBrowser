/*!
 * \file Libcurl.h
 *封装Libcurl下载库
 * \author zhengzc
 * \date 五月 2018
 *
 * 
 */
#pragma once
//libcurl采用静态库引用
#define  CURL_STATICLIB

#define  DEFAULT_CONNECTTIMEOUT_TIMEOUT  10  //默认连接超时时间
#define  DEFAULT_TIMEOUT  5					//默认超时时间
#include "curl/curl.h"
#include <string>
#include <fstream>
#include <memory>
#include <map>
#include <atomic>
#include <functional>
using namespace  std;



class CLibcurlCallback
{
public:
	// 虚函数，进度显示
	virtual void Progress(void* lpParam, double dTotal, double bLoaded) = 0;

	// 写数据回调函数
	virtual void	WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam) = 0;

};

enum LibcurlFlag
{
	Lf_None = 0,
	Lf_Download,
	Lf_Post,
	Lf_Get,
};

class CLibcurl
{
public:
	CLibcurl(void);
	~CLibcurl(void);
	/******************封装类的外部调用接口************/

	/**
	* @brief HTTP 设置连接端口号
	* @param port 端口号
	* @return 返回是否成功
	*/
	bool SetPort(LONG port);								
	
	//设置执行超时（秒）
	bool SetTimeout(int nSecond);										
	
	//设置连接超时（秒）
	bool SetConnectTimeout(int nSecond);							
	
	bool  setReferer(LPCSTR lpReferer);

	//设置用户代理
	bool SetUserAgent(LPCSTR lpAgent);								
	
	//设置断点续传起始位置
	bool SetResumeFrom(LONG lPos);										
	
	//设置断点续传起始位置，针对大文件
	bool SetResumeFromLarge(LONGLONG llPos);							
	
	//添加自定义头
	bool AddHeader(LPCSTR lpKey, LPCSTR lpValue);

	// 获取Http头部
	std::string ReadHeader(LPCSTR lpUrl);

	// 解析Http头部
	bool	_ParseHead(char *pData, size_t sSize, std::map<string,string> &mapHeader);

	// 获取下一行数据（解析Http头部调用）
	bool	_GetNextLine(char **pPos, char **pStart, char **pEnd);
			
	//清理HTTP列表头
	void ClearHeaderList();												
	
	//设置http头中的cookie信息。
	bool SetCookie(LPCSTR lpCookie);

	//设置http头中的cookielist信息。
	bool SetCookieList(LPCSTR lpCookielist);

	//设置cookie的保存目录
	bool SaveCookie(LPCSTR lpCookie);


	//设置HTTP请求cookie文件
	bool SetCookieFile(LPCSTR lpFilePath);								
	
	//获取错误详细信息
	const char* GetError()const;		

	//设置下载进度回调
	void SetCallback(CLibcurlCallback* pCallback, void* lpParam);		
	
	/**
	* @brief HTTP 异步下载文件到磁盘
	* @param lpUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param lpFile 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	bool AsynDownloadToFile(LPCSTR lpUrl, LPCSTR lpFile);	

	/**
	* @brief HTTP 同步下载文件到磁盘
	* @param lpUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param lpFile 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	bool DownloadToFile(LPCSTR lpUrl, LPCSTR lpFile);

	/**
	* @brief HTTP 同步下载文件到磁盘
	* @param lpUrl 输入参数,请求的Url地址,如:https://www.baidu.com
	* @param lpFile 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	bool DownloadToFile2(LPCSTR lpUrl, LPCSTR lpFile);

	/**
	* @brief HTTP POST请求
	* @param lpUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param lpData 输入参数
	* @return 返回是否Post成功
	*/
	bool Post(LPCSTR lpUrl, LPCSTR lpData);	

	/**
	* @brief HTTP Post 字符串或者二进制数据
	* @param lpUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param lpData 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	bool Post(LPCSTR lpUrl, unsigned char* lpData, unsigned int nSize);

	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strData 输入参数
	* @return 返回是否Post成功
	*/
	std::string Posts(std::string strUrl, std::string strData );

	//异步执行 Get 请求
	bool AsynGet(LPCSTR lpUrl);		
	
	// 同步执行发送Get请求，等待返回字符串
	string Get(LPCSTR strUrl);

	//获取Post/Get请求返回数据
	const string& GetRespons()const;									

	//获取Post/Get请求返回数据
	const char*	GetResponsPtr()const;									

	// 获取响应代码
	int		GetResponseCode();

protected:
	// 写数据回调函数
	static size_t	WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);

	// 返回头部信息回调
	static size_t	ResponseHeadCallBack(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);

	// 虚函数，进度显示
	static int		ProgressCallback(void *pParam, double dltotal, double dlnow, double ultotal, double ulnow);

private:
	CURL * m_pCurl;
	LONG		m_nPort;
	HANDLE		m_hFile;
	CURLcode	m_curlCode;
	string		m_strRespons;
	LibcurlFlag m_lfFlag;
	curl_slist *m_curlList;
	void		*m_pCallbackParam;
	CLibcurlCallback	*m_pCallback;

	// 响应代码
	int				m_nResponseCode;
	// 头部数据
	std::string		m_strHead;
};



enum DOWNLOADSTATES
{
	DOWN_OK       = 0,		//下载成功	
	DOWN_START    = 100,	//开始下载	
	DOWN_LOADING  = 200,	//下载中	
	DOWN_PAUSE    = 300,	//下载暂停
	DOWN_CONTINUE = 400,	//下载继续
	DOWN_STOP     = 500,	//停止下载
	DOWN_FAIL     = 600		//下载失败

};

typedef std::function<void(void* lpParam, const double& dltotal, const double& dlnow, int  nStates)> CallBackFunc;



class CDownloader
{
public:
	CDownloader(void);
	virtual ~CDownloader(void);

public:
	/// 线程入口函数
	virtual bool run();

	/// 启动下载
	virtual bool start(const std::string& strUrl, const std::string& strLocalFile);
	
	// 暂停下载
	int pause();

	// 继续下载
	int resume();

	// 停止下载
	virtual bool stop();

	/// 是否运行状态
	bool isRunning() const;
	
	/// 是状态
	int getState() const;

	//设置断点续传起始位置，针对大文件
	bool SetResumeFromLarge(double llPos);

	// 设置缓存地址
	void SetCache(const char * pCachePath);

	// 设置下载速度
	bool  setSpeed(long limitSpeed);

	//设置回调
	void  setCallback(CallBackFunc callbackfuc, void* lpParam = nullptr);

	//设置下载进度回调
	void SetCallback(CLibcurlCallback* pCallback, void* lpParam);
	
	//删除临时文件
	void DelTempFile();

	void SetDeleteFlag(bool bDelete);

	bool GetDeleteFlag() { return m_bDelete; }
protected:
	/// 写入回调
	static size_t WriteDataCallBack(void *buffer, size_t size, size_t nmemb, void *userp);

	// 返回头部信息回调
	static size_t ResponseHeadCallBack(void *pData, size_t sSize, size_t sMem, void *pParam);

	/// 进度回调
	static size_t handleProgress(void *buffer, double dltotal, double dlnow, double ultotal, double ulnow);

protected:
	/// 写入回调
	size_t onWrite(void *buffer, size_t size, size_t nmemb);

	/// 进度回调
	size_t onProgress(double dltotal, double dlnow);

	/// 下载
	bool onDownload();


protected:
	// 设置libcurl选项
	void setOption();

	// 清除数据
	void clear();

	void UpdateState(int nStates);



protected:
	CURL* m_pCurl;									//< libcurl句柄

	FILE*  m_pFile;									//< 文件指针

	std::string			  m_strDownloadUrl;			//< 下载链接

	std::string			   m_strLocalFilePath;		//< 本地文件路径

	std::string			   m_strCachePath;			//下载的中间临时目录

	std::string			   m_strTemp;				//下载的临时文件

	std::string			   m_strHead;				// 头部数据

	CURLcode			   m_curlCode;				//返回值

	void				   *m_pCallbackParam;		//回调参数指针

	CLibcurlCallback	   *m_pCallback;		    //回调函数类

	CallBackFunc		   m_callbackfuc;			//回调函数

	std::atomic<double>    m_dltotal;				//已经下载总的大小

	std::atomic<double>    m_dlnow;					//当前下载的进度

	std::atomic<double>    m_Content_Length;		//需要下载总的大小

	std::atomic<double>    m_nStartSize;			//开始下载的大小

	std::atomic<int>	   m_nStates;		 		//当前状态

	volatile bool		   m_bPause;				//当前下载的进度
	 
	volatile bool		   m_bDelete;				//是否删除临时文件

	volatile bool m_bRunning;						//< 运行标志

	double                 m_dSpeed;//下载速度
};
