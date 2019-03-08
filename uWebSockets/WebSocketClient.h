#pragma once

#include "define.h"

class UWebSocketsClientEvent;

class UWBESOCKET_EXPORTIMPL CWebSocketClient
{
public:
	CWebSocketClient();
	~CWebSocketClient();
	void sendTextMessage(std::string, OpCode);
	void Start(std::string url);
	void StartTimeEvent(int nMiliseconds);
	void StopTimeEvent();
	void Stop();
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
class UWBESOCKET_EXPORTIMPL UWebSocketsClientEvent
{
public:
	UWebSocketsClientEvent()
	{

	}
	~UWebSocketsClientEvent()
	{

	}

	virtual void onConnection() = 0;
	virtual void onDisconnection() = 0;
	virtual void OnMessage(std::string message, CWebSocketClient* client) = 0;
	virtual void onTimeEvent() = 0;
};