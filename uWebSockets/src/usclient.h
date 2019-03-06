#pragma once
#include <string>
#include <mutex>
#include <map>
#ifdef   UWBESOCKET_EXPORT
#define UWBESOCKET_EXPORTIMPL __declspec(dllexport)
#else
#define UWBESOCKET_EXPORTIMPL __declspec(dllimport)
#endif 
class UWebSocketsClientEvent;
enum OpCode : unsigned char {
	TEXT = 1,
	BINARY = 2,
	CLOSE = 8,
	PING = 9,
	PONG = 10
};
class UWBESOCKET_EXPORTIMPL UWebSocketsClient
{   
public:
	UWebSocketsClient();
	~UWebSocketsClient();
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
	std::shared_ptr<bool> m_bStop;
	int m_timerid;
	std::recursive_mutex m_asyncMutex;
	std::map<std::string, std::string> m_extraHeaders;
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
	virtual void OnMessage(std::string message, UWebSocketsClient* client) = 0;
	virtual void onTimeEvent() = 0;
};