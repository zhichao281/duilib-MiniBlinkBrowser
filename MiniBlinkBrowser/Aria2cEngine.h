
#pragma once
#include "stdafx.h"
#include "../uWebSockets/WebSocketClient.h"
#include "../uWebSockets/WebSocketsServer.h"
#include "../Common/HSingleton.h"

class ClientEvent;

class ServerEvent;

class CAria2cEngine 
{
public:
	CAria2cEngine();
	~CAria2cEngine();

	//回调函数
public:

	void StartAria2c();

	void StartDownload(std::string  strUrl);

	bool Remove(CDuiString szGid);

	bool pause(CDuiString szGid);

	bool unpause(CDuiString szGid);

	//关闭Aria2c.exe
	void Shutdown();

	void UnpauseAll();

	void StartServer();

	void SentMessage(std::string strMessage);


public:
	ClientEvent  *m_event;
	ServerEvent  *m_serverevent;

	CWebSocketsServer  server;
	CWebSocketClient  client;
	std::vector<std::string> m_vecGid;
};

class ServerEvent :public UWebSocketsServerEvent
{
public:
	ServerEvent()
	{

	}
	~ServerEvent()
	{

	}
	virtual void onConnection();

	virtual void onDisconnection(int code);

	virtual void OnMessage(std::string message, CWebSocketsServer* client);

	virtual void onTimeEvent();

	virtual void onError(int port);

	virtual void onError(void *user);

};





class ClientEvent :public UWebSocketsClientEvent 
{
public:
	ClientEvent()
	{

	}
	~ClientEvent()
	{

	}
	virtual void onConnection();
	virtual void onDisconnection();

	virtual void OnMessage(std::string message, CWebSocketClient* client);

	virtual void onTimeEvent();

	virtual void onError(int port) ;

	virtual void onError(void *user) ;
};





#define gblDownloadMgrGet HSingletonTemplatePtr<CAria2cEngine>::Instance()
