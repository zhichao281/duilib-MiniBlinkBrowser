#pragma once



#ifdef   UWBESOCKET_EXPORT
#define UWBESOCKET_EXPORTIMPL __declspec(dllexport)
#else
#define UWBESOCKET_EXPORTIMPL __declspec(dllimport)
#endif 



#include <string>
#include <mutex>
#include <map>
#include <atomic>
enum OpCode : unsigned char
{
	TEXT = 1,
	BINARY = 2,
	CLOSE = 8,
	PING = 9,
	PONG = 10
};