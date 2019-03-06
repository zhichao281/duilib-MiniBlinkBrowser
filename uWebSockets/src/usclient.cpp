#include "usclient.h"
#include "Hub.h"
#include <openssl/sha.h>
UWebSocketsClient::UWebSocketsClient()
{
	m_bStop.reset(new bool(false));
	m_event = NULL;
	m_timerid = -1;
}
UWebSocketsClient::~UWebSocketsClient()
{
	StopTimeEvent();
	Stop();
	
	
}

void UWebSocketsClient::Start(std::string url)
{
	m_url = url;
	if (m_bStop)
	{
		*m_bStop = false;
	}
	bool bStartThred = false;
	std::shared_ptr<bool> tmpBStop = m_bStop;
	std::thread jthread([=, &tmpBStop]()
	{
		std::weak_ptr<bool> bStop = tmpBStop;
		tmpBStop.reset();
		while (bStop.lock() && !(*bStop.lock()))
		{
			UWebSocketsClient* pthis = this;
			uWS::Hub h;
			m_pus = &h;
			UWebSocketsClientEvent* pevent = m_event;
			h.onMessage([&h, &pthis, &pevent](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode opCode) {
				if (pevent)
				{
					std::string strmsg(message, message + length);
					pevent->OnMessage(strmsg, pthis);

				}
			});
			int length = 0;
			h.onConnection([&h, &pthis, &pevent](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
				if (pevent)
				{
					pevent->onConnection();
				}
			});
			h.onDisconnection([&h, &pthis, &pevent](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
				if (pevent&&code != 1000)
				{
					pevent->onDisconnection();
				}
			});
			h.connect(m_url, this, m_extraHeaders, 3000);
			h.run();		
			if (bStop.lock() && !(*bStop.lock()))
			{
				m_asyncMutex.lock();
				m_pus = NULL;
				m_asyncMutex.unlock();
			}			
			if (bStop.lock() && !(*bStop.lock()))
			{
				Sleep(300);
			}
		}

	});
	if (tmpBStop)
	{
		Sleep(1);
	}
	jthread.detach();
}
void WINAPI onTimeEventFunc(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dwl, DWORD_PTR dw2)
{
	UWebSocketsClient* p = (UWebSocketsClient*)dwUser;
	p->onTimeEvent();
	return;
}
void UWebSocketsClient::StartTimeEvent(int nMiliseconds)
{
	m_timerid = timeSetEvent(nMiliseconds, 1, (LPTIMECALLBACK)onTimeEventFunc, (DWORD_PTR)this, TIME_PERIODIC);
}
void UWebSocketsClient::StopTimeEvent()
{
	if (m_timerid != -1)
	{
		timeKillEvent(m_timerid);
		m_timerid = -1;
	}
}
void UWebSocketsClient::onTimeEvent()
{
	m_asyncMutex.lock();
	if (m_pus)
	{
		if (m_event)
		{
			m_event->onTimeEvent();
		}
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::CLIENT>().broadcast(0, 0, uWS::OpCode::PING);
	}
	m_asyncMutex.unlock();
}
#pragma  comment(lib, "winmm.lib")

void UWebSocketsClient::sendTextMessage(std::string text, OpCode type)
{
	m_asyncMutex.lock();
	if (m_pus)
	{
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::CLIENT>().broadcast(text.c_str(), text.length(), uWS::OpCode(type));
	}
	m_asyncMutex.unlock();
}
void UWebSocketsClient::SetEvent(UWebSocketsClientEvent* pevent)
{
	m_event = pevent;
}
void UWebSocketsClient::Stop()
{
	m_asyncMutex.lock();
	*m_bStop = true;
	if (m_pus)
	{
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::CLIENT>().close();
		m_pus = NULL;
	}
	m_asyncMutex.unlock();
}
void UWebSocketsClient::SetExtraHeaders(std::map<std::string, std::string>& extraHeaders)
{
	m_extraHeaders = extraHeaders;
}