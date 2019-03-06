#include "usclient.h"
#include "Hub.h"
#include <openssl/sha.h>
#include <iostream>
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
void testConnections() 
{
	uWS::Hub h;

	h.onError([](void *user) {
		switch ((long)user) {
		case 1:
			std::cout << "Client emitted error on invalid URI" << std::endl;
			break;
		case 2:
			std::cout << "Client emitted error on resolve failure" << std::endl;
			break;
		case 3:
			std::cout << "Client emitted error on connection timeout (non-SSL)" << std::endl;
			break;
		case 5:
			std::cout << "Client emitted error on connection timeout (SSL)" << std::endl;
			break;
		case 6:
			std::cout << "Client emitted error on HTTP response without upgrade (non-SSL)" << std::endl;
			break;
		case 7:
			std::cout << "Client emitted error on HTTP response without upgrade (SSL)" << std::endl;
			break;
		case 10:
			std::cout << "Client emitted error on poll error" << std::endl;
			break;
		case 11:
		{
			static int protocolErrorCount = 0;
			protocolErrorCount++;
			std::cout << "Client emitted error on invalid protocol" << std::endl;
			if (protocolErrorCount > 1) {
				std::cout << "FAILURE:  " << protocolErrorCount << " errors emitted for one connection!" << std::endl;
				exit(-1);
			}
			break;
		}

		default:
			std::cout << "FAILURE: " << user << " should not emit error!" << std::endl;
			exit(-1);
		}
	});

	h.onConnection([](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
		switch ((long)ws->getUserData()) {
		case 8:
			std::cout << "Client established a remote connection over non-SSL" << std::endl;
			ws->close(1000);
			break;
		case 9:
			std::cout << "Client established a remote connection over SSL" << std::endl;
			ws->close(1000);
			break;
		default:
			std::cout << "FAILURE: " << ws->getUserData() << " should not connect!" << std::endl;
			exit(-1);
		}
	});

	h.onDisconnection([](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
		std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code << ", message: <" << std::string(message, length) << ">" << std::endl;
	});

	h.connect("invalid URI", (void *)1);
	h.connect("invalid://validButUnknown.yolo", (void *)11);
	h.connect("ws://validButUnknown.yolo", (void *)2);
	h.connect("ws://echo.websocket.org", (void *)3, {}, 10);
	h.connect("ws://echo.websocket.org", (void *)8);
	h.connect("wss://echo.websocket.org", (void *)5, {}, 10);
	h.connect("wss://echo.websocket.org", (void *)9);
	h.connect("ws://google.com", (void *)6);
	h.connect("wss://google.com", (void *)7);
	h.connect("ws://127.0.0.1:6000", (void *)10, {}, 60000);

	h.run();
	std::cout << "Falling through testConnections" << std::endl;
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