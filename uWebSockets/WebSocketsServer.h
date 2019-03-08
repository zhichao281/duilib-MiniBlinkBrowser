#pragma once


#include "define.h"


class UWebSocketsServerEvent;



class UWBESOCKET_EXPORTIMPL CWebSocketsServer
{
public:
	CWebSocketsServer();
	~CWebSocketsServer();
	//启动ws服务器
	void Start(int nPort);

	void sendTextMessage(std::string, OpCode opCode= TEXT);

	//关闭ws服务器
	void Stop();

	void StartTimeEvent(int nMiliseconds);


	void StopTimeEvent();
	
	void SetEvent(UWebSocketsServerEvent* event);

	void onTimeEvent();	
private:

	int   m_nPort;
	void* m_pus = NULL;
	UWebSocketsServerEvent* m_event;
	int m_timerid;
	std::recursive_mutex m_asyncMutex;
	std::map<std::string, std::string> m_extraHeaders;
	std::atomic<bool>    m_bRun;
};


class UWBESOCKET_EXPORTIMPL UWebSocketsServerEvent
{
public:
	UWebSocketsServerEvent()
	{

	}
	~UWebSocketsServerEvent()
	{

	}
	virtual void onConnection() = 0;
	virtual void onDisconnection(int code) = 0;
	virtual void OnMessage(std::string message, CWebSocketsServer* client) = 0;
	virtual void onTimeEvent() = 0;

	virtual void onError(int port) = 0;
	virtual void onError(void *user) = 0;

};