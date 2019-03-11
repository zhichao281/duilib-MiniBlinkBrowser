
#include <iostream>
#include <windows.h>
#include "Aria2cEngine.h"
#include "../Common/Json/Header/NSJson.h"
#include "../Common/String/NSString.h"

CAria2cEngine::CAria2cEngine()
{

	m_event = new ClientEvent();
	m_serverevent = new ServerEvent();
}


CAria2cEngine::~CAria2cEngine()
{
}

void CAria2cEngine::StartAria2c()
{
	CHAR szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH);
	::GetCurrentDirectoryA(MAX_PATH, szPath);
	std::string szTmpPath = szPath;
	szTmpPath += "\\download\\";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.dwFlags = STARTF_USESHOWWINDOW;  // 指定wShowWindow成员有效

	si.wShowWindow = FALSE;          // 此成员设为TRUE的话则显示新建进程的主窗口，
	CDuiString szApp;
	
	szApp.Format(_T("aria2c.exe -enable-rpc=true --rpc-allow-origin-all=true --rpc-listen-all=true --dir=%s --conf-path=aria2.conf"), NStr::StrToWStr(szTmpPath).c_str());
	BOOL bRet = ::CreateProcess(NULL,           // 不在此指定可执行文件的文件名
		(LPWSTR)szApp.GetData(),      // 命令行参数
		NULL,           // 默认进程安全性
		NULL,           // 默认线程安全性
		FALSE,          // 指定当前进程内的句柄不可以被子进程继承
		CREATE_NEW_CONSOLE, // 为新进程创建一个新的控制台窗口
		NULL,           // 使用本进程的环境变量
		_T("E:\\study\\MiniBlinkBrowser\\bin"),           // 使用本进程的驱动器和目录
		&si,
		&pi);
	::CloseHandle(pi.hProcess);


	client.SetEvent(m_event);
	client.Start("ws://127.0.0.1:6800/jsonrpc");
}
std::string Gbk_To_Utf8(const char* szBuff)
{
	std::string resault;
	int widLen = 0;
	int MultiLen = 0;
	WCHAR* widPtr = nullptr;
	CHAR* MulitPtr = nullptr;
	widLen = ::MultiByteToWideChar(CP_ACP, NULL, szBuff, -1, NULL, NULL);//获取转换后需要的空间
	widPtr = new WCHAR[widLen + 1];
	if (!widPtr)
		return resault;
	ZeroMemory(widPtr, (widLen + 1) * sizeof(WCHAR));
	if (!::MultiByteToWideChar(CP_ACP, NULL, szBuff, -1, widPtr, widLen))
	{
		delete[] widPtr;
		widPtr = nullptr;
		return resault;
	}
	MultiLen = ::WideCharToMultiByte(CP_UTF8, NULL, widPtr, -1, NULL, NULL, NULL, NULL);
	if (MultiLen)
	{
		MulitPtr = new CHAR[MultiLen + 1];
		if (!MulitPtr)
		{
			delete[] widPtr;
			widPtr = nullptr;
			return resault;
		}
		ZeroMemory(MulitPtr, (MultiLen + 1) * sizeof(CHAR));
		::WideCharToMultiByte(CP_UTF8, NULL, widPtr, -1, MulitPtr, MultiLen, NULL, NULL);
		resault = MulitPtr;
		delete[] MulitPtr;
		MulitPtr = nullptr;
	}
	delete[] widPtr;
	widPtr = nullptr;
	return resault;
}

void CAria2cEngine::StartDownload(std::string strUrl)
{
	std::string buf = "{\"jsonrpc\":\"2.0\",\"method\":\"aria2.addUri\",\"params\":[\"token:123\",[\"";
	buf += strUrl + "\"]],\"id\":\"qwer\"}";
	client.sendTextMessage(buf.c_str());
	std::thread getthread([&]()
	{
	 while (1)
	 {
		 for (auto& v : m_vecGid)
		 {
			 std::string strData = "{\"jsonrpc\":\"2.0\", \"id\":\"qwer\", \"method\":\"aria2.tellStatus\",\"params\":[\"token:123\",\"";
			 strData += v;
			 strData += "\"]}";
			 client.sendTextMessage(strData);

		 }
		 Sleep(1000);
	 }
	
	});
	getthread.detach();
}

bool CAria2cEngine::Remove(CDuiString szGid)
{
	std::string strSend;
	CHAR  szSend[512];
	sprintf_s(szSend, 512, "{\"jsonrpc\":\"2.0\", \"id\":\"qwer\",\"method\":\"aria2.remove\",\"params\":[\"token:123\",\"%s\"]}", szGid.GetData());
	client.sendTextMessage(szSend);
	return false;
}

bool CAria2cEngine::pause(CDuiString szGid)
{
	std::string strSend;
	CHAR  szSend[512];
	sprintf_s(szSend, 512, "{\"jsonrpc\":\"2.0\", \"id\":\"qwer\",\"method\":\"aria2.pause\",\"params\":[\"token:123\",\"%s\"]}", szGid.GetData());
	client.sendTextMessage(szSend);
	return false;
}

bool CAria2cEngine::unpause(CDuiString szGid)
{
	std::string strSend;
	CHAR  szSend[512];
	sprintf_s(szSend, 512, "{\"jsonrpc\":\"2.0\", \"id\":\"qwer\",\"method\":\"aria2.unpause\",\"params\":[\"token:123\",\"%s\"]}", szGid.GetData());
	client.sendTextMessage(szSend);
	return false;
}

void CAria2cEngine::Shutdown()
{
	client.sendTextMessage("{\"jsonrpc\":\"2.0\",\"method\":\"aria2.shutdown\",\"params\":[\"token:123\"],\"id\":1}");
}
void CAria2cEngine::UnpauseAll()
{
	client.sendTextMessage("{\"jsonrpc\":\"2.0\",\"method\":\"aria2.forcePauseAll\",\"params\":[\"token:123\"],\"id\":1}");
}

void CAria2cEngine::StartServer()
{
	server.SetEvent(m_serverevent);
	server.Start(43990);
}

void CAria2cEngine::SentMessage(std::string strMessage)
{
	server.sendTextMessage(strMessage);
}

void ServerEvent::onConnection()
{
	std::cout << "onConnection" << std::endl;
	gblDownloadMgrGet->SentMessage("onConnection");

}

void ServerEvent::onDisconnection(int code )
{
	if (code == 1006)
	{
		std::cout << "Server recives terminate close code after terminating" << std::endl;
	}
	else if (code != 1000) {
		std::cout << "FAILURE: Server does not receive correct close code!" << std::endl;
	}
	else
	{
		std::cout << "Server receives correct close code after closing" << std::endl;
	}
}

void ServerEvent::OnMessage(std::string message, CWebSocketsServer * client)
{
	std::cout << message.c_str() << std::endl;
	gblDownloadMgrGet->SentMessage("test"+message);

}

void ServerEvent::onTimeEvent()
{
	std::cout << "FAILURE: Connection failed! Timeout?" << std::endl;
}

void ServerEvent::onError(int port)
{
	switch (port) 
	{
	case 80:
		std::cout << "Server emits error listening to port 80 (permission denied)" << std::endl;
		break;
	case 3000:
		std::cout << "Server emits error listening to port 3000 twice" << std::endl;
		break;
	default:
		std::cout << "FAILURE: port " << port << " should not emit error" << std::endl;
		break;
	}

}

void ServerEvent::onError(void * user)
{
	switch ((long)user) 
	{
	case 1:
		std::cout << "Client emitted error on" << std::endl;
		break;
	default:
		std::cout << "FAILURE: " << user << " should not emit error!" << std::endl;
	}
}







void ClientEvent::onConnection()
{
	
	std::cout << "onConnection" <<std::endl;

}

void ClientEvent::onDisconnection()
{
	std::cout << "onConnection" << std::endl;
}

void ClientEvent::OnMessage(std::string message, CWebSocketClient * client)
{

	OutputDebugStringA(message.c_str());
	OutputDebugStringA("\r\n");
	NSJsonObject jsonObject = NSJson::ParseStr(message);
	if (jsonObject.IsNull() == false)
	{
		std::string strResult , strMethod;
	
		if (jsonObject.IsString("result"))
		{
			strResult = jsonObject.GetString("result");
			gblDownloadMgrGet->m_vecGid.push_back(strResult);
		}
		if (jsonObject.IsObject("result"))
		{
			NSJsonObject resultobj = jsonObject.Get("result");
		
		}
	


		strMethod = jsonObject.GetString("method"); 
		if (strMethod == "aria2.onDownloadStart")	//开始下载
		{
		}
		else if (strMethod == "aria2.onDownloadPause")	//暂停下载
		{

		}
		else if (strMethod == "aria2.onDownloadStop")	//用户自己停止下载
		{

		}
		else if (strMethod == "aria2.onDownloadComplete")	//下载完成提示
		{
		}
		else if (strMethod == "aria2.onDownloadError")	//出现错误导致下载停止
		{

		}
		else if (strMethod == "aria2.onBtDownloadComplete")	//当torrent下载完成但播种仍在继续时，将发送此通知
		{
		}
	}



}

void ClientEvent::onTimeEvent()
{
}

void ClientEvent::onError(int port)
{
}

void ClientEvent::onError(void * user)
{
}
