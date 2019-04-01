#pragma once

#include <string>
#include "../Common/Json/Header/NSJson.h"
#include "../Common/HSingleton.h"
class CAria2RPC
{
public:
	CAria2RPC();
	~CAria2RPC();

public:
	void addedTask(const std::string &gid);

	void updateStatus(const std::string &fileName, const std::string &gid, const int &status, const long long &totalLength,
		const long long &completedLenth, const long long &speed, const int &percent);

	void addUri(const std::string &uri, const std::string &id);
	void remove(const std::string &gid);
	void pause(const std::string &gid);
	void unpause(const std::string &gid);
	void tellStatus(const std::string &gid);
	void tellActive();
	void tellWaiting();
	void tellStopped();

private:
	void sendMessage(const std::string &method, const std::string &id, NSJsonArray params);
	//void handleNetworkReply(QNetworkReply *reply, const std::string &method);
	void handleTellStatus(const NSJsonObject &object);
};

#define gblAria2RPCGet HSingletonTemplatePtr<CAria2RPC>::Instance()
