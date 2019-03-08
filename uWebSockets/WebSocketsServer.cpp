#include "WebSocketsServer.h"

#include "src/Hub.h"
#include <openssl/sha.h>
#include <iostream>




CWebSocketsServer::CWebSocketsServer()
{
	m_extraHeaders = {};
	m_event = NULL;
	m_timerid = -1;
}


CWebSocketsServer::~CWebSocketsServer()
{
	StopTimeEvent();
	Stop();
}
void CWebSocketsServer::Start(int nPort)
{
	m_bRun.store(true);
	m_nPort = nPort;
	std::thread jthread([=]()
	{
		while (m_bRun.load())
		{		
			uWS::Hub hub;
			m_pus = &hub;
			hub.onConnection([&](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req)
			{
				std::string  strReq=req.getUrl().toString();
				if (m_event)
				{
					m_event->onConnection();
				}

			});

			hub.onError([&](int port)
			{
				if (m_event)
				{
					m_event->onError(port);
				}
			});
			hub.onError([&](void *user)
			{
				std::cout << "FAILURE: Connection failed! Timeout?" << std::endl;
				if (m_event)
				{
					m_event->onError(user);
				}
			});

			hub.onDisconnection([&](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length)
			{		
				if (m_event)
				{
					m_event->onDisconnection(code);
				}
			});

			hub.onMessage([&](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode)
			{			
				if (m_event)
				{
					std::string strmsg(message, message + length);
					m_event->OnMessage(strmsg, this);
				}
			});
			if (hub.listen(m_nPort))
			{
				hub.run();
			}
			else
			{
				std::cout << "Failed to listen" << std::endl;
			}
		}
	});

	jthread.detach();
}

void CWebSocketsServer::sendTextMessage(std::string text, OpCode opCode)
{
	m_asyncMutex.lock();
	if (m_pus)
	{
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::SERVER>().broadcast(text.c_str(), text.length(), uWS::OpCode(opCode));
	}
	m_asyncMutex.unlock();

}

void CWebSocketsServer::Stop()
{
	m_asyncMutex.lock();
	m_bRun.store(false);
	if (m_pus)
	{
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::SERVER>().close();
		delete m_pus;
		m_pus = NULL;
	}
	m_asyncMutex.unlock();
}

void CWebSocketsServer::SetEvent(UWebSocketsServerEvent * event)
{
	m_event = event;
}


void CWebSocketsServer::StartTimeEvent(int nMiliseconds)
{
	m_timerid = timeSetEvent(nMiliseconds, 1,
		[](UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dwl, DWORD_PTR dw2)
	{
		CWebSocketsServer* p = (CWebSocketsServer*)dwUser;
		p->onTimeEvent();
		return;
	}, (DWORD_PTR)this, TIME_PERIODIC);
}
void CWebSocketsServer::StopTimeEvent()
{
	if (m_timerid != -1)
	{
		timeKillEvent(m_timerid);
		m_timerid = -1;
	}
}
void CWebSocketsServer::onTimeEvent()
{
	m_asyncMutex.lock();
	if (m_pus)
	{
		if (m_event)
		{
			m_event->onTimeEvent();
		}
		((uWS::Hub*)m_pus)->getDefaultGroup<uWS::SERVER>().broadcast(0, 0, uWS::OpCode::PING);
	}
	m_asyncMutex.unlock();
}
