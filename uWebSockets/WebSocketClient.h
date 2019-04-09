#pragma once

#include "define.h"







class UWebSocketsClientEvent;

class UWBESOCKET_EXPORTIMPL CWebSocketClient
{
public:
	CWebSocketClient();
	~CWebSocketClient();
	//连接到web服务器
	void Start(std::string url);
	//发送消息
	void sendTextMessage(std::string, OpCode opCode = TEXT);
	//设置心跳
	void StartTimeEvent(int nMiliseconds);
	//停止心跳
	void StopTimeEvent();
	//退出websocket连接
	void Stop();
	//设置回调事件
	void SetEvent(UWebSocketsClientEvent*);

	void onTimeEvent();

	void SetExtraHeaders(std::map<std::string, std::string>& extraHeaders);
private:
	std::string m_url;
	void* m_pus = NULL;
	UWebSocketsClientEvent* m_event;

	int m_timerid;
	std::recursive_mutex m_asyncMutex;
	std::map<std::string, std::string> m_extraHeaders;
	std::atomic<bool>    m_bRun;
};

//回调类
class UWBESOCKET_EXPORTIMPL UWebSocketsClientEvent
{
public:
	UWebSocketsClientEvent()
	{

	}
	~UWebSocketsClientEvent()
	{

	}
	//连接成功回调
	virtual void onConnection() = 0;
	
	virtual void onDisconnection() = 0;
	
	virtual void OnMessage(std::string message, CWebSocketClient* client) = 0;
	
	virtual void onTimeEvent() = 0;

	virtual void onError(int port) = 0;

	virtual void onError(void *user) = 0;
};