#include "Aria2RPC.h"
#include "../Common/Libcurl.h"


CAria2RPC::CAria2RPC()
{

}

CAria2RPC::~CAria2RPC()
{
}


void CAria2RPC::addUri(const std::string &uri, const std::string &id)
{
	NSJsonArray array;
	NSJsonArray item;
	item.AddString(uri);
	array.AddArray(item);
	sendMessage("aria2.addUri", id, array);
}

void CAria2RPC::remove(const std::string &gid)
{
	NSJsonArray params;

	if (gid != "0") {
		params.AddString(gid);
	}

	params.AddString(gid);
	sendMessage("aria2.remove", gid, params);
}

void CAria2RPC::pause(const std::string &gid)
{
	NSJsonArray params;
	params.AddString(gid);
	sendMessage("aria2.pause", gid, params);
}

void CAria2RPC::unpause(const std::string &gid)
{
	NSJsonArray params;
	params.AddString(gid);
	sendMessage("aria2.unpause", gid, params);
}

void CAria2RPC::tellStatus(const std::string &gid)
{
	NSJsonArray params;
	params.AddString(gid);
	sendMessage("aria2.tellStatus", gid, params);
}

void CAria2RPC::tellActive()
{
	NSJsonArray params;
	params.AddInt(0);
	params.AddInt(100);
	sendMessage("aria2.tellActive", 0, params);
}

void CAria2RPC::tellWaiting()
{
	NSJsonArray params;
	params.AddInt(0);
	params.AddInt(100);
	sendMessage("aria2.tellWaiting", 0, params);
}

void CAria2RPC::tellStopped()
{
	NSJsonArray params;
	params.AddInt(0);
	params.AddInt(100);
	sendMessage("aria2.tellStopped", 0, params);
}

void CAria2RPC::sendMessage(const std::string &method, const std::string &id, NSJsonArray params)
{

	CLibcurl libcurl;
	libcurl.AddHeader("content-type", "application/json");
	libcurl.Post("http://localhost:7200/jsonrpc", "");
	/*QNetworkAccessManager *manager = new QNetworkAccessManager;

	NSJsonObject object;
	object.Put("jsonrpc", "2.0");
	object.Put("id", id);
	object.Put("method", method);

	if (!params.IsNull()) {
		object.Put("params", params);
	}



	QNetworkRequest *request = new QNetworkRequest;
	request->setUrl(QUrl("http://localhost:7200/jsonrpc"));
	request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	manager->post(*request, QJsonDocument(object).toJson());

	connect(manager, &QNetworkAccessManager::finished, this,
		[=](QNetworkReply *reply) {
		handleNetworkReply(reply, method);
		manager->deleteLater();
		manager->destroyed();
	});*/
}
//
//void CAria2RPC::handleNetworkReply(QNetworkReply *reply, const std::string &method)
//{
//	if (reply->error() == QNetworkReply::NoError) {
//		QByteArray buffer = reply->readAll();
//		QJsonDocument document = QJsonDocument::fromJson(buffer);
//		QJsonObject object = document.object();
//
//		if (!object.isEmpty()) {
//			if (method == "aria2.tellStatus") {
//				handleTellStatus(object);
//			}
//			else if (method == "aria2.addUri") {
//				Q_EMIT addedTask(object.value("result").toString());
//			}
//		}
//	}
//	else {
//		qDebug() << "error: " << reply->errorString();
//	}
//}

void CAria2RPC::handleTellStatus(const NSJsonObject &object)
{
	//QJsonObject result = object.value("result").toObject();
	//NSJsonArray files = result.value("files").toArray();
	//std::string filePath;

	//for (int i = 0; i < files.size(); ++i) {
	//	QJsonObject file = files[i].toObject();
	//	filePath = file.value("path").toString();
	//}


	//const long long speed = result.value("downloadSpeed").toString().toLong();
	//const long long totalLength = result.value("totalLength").toString().toLong();
	//const long long completedLength = result.value("completedLength").toString().toLong();
	//const std::string fileName = Utils::getFileName(filePath);
	//const std::string gid = result.value("gid").toString();
	//const std::string statusStr = result.value("status").toString();

	//int percent = 0;
	//int status = 0;

	//if (completedLength != 0) {
	//	percent = completedLength * 100.0 / totalLength;
	//}

	//if (statusStr == "active") {
	//	status = Global::Status::Active;
	//}
	//else if (statusStr == "waiting") {
	//	status = Global::Status::Waiting;
	//}
	//else if (statusStr == "paused") {
	//	status = Global::Status::Paused;
	//}
	//else if (statusStr == "error") {
	//	status = Global::Status::Error;
	//}
	//else if (statusStr == "complete") {
	//	status = Global::Status::Complete;
	//}
	//else if (statusStr == "removed") {
	//	status = Global::Status::Removed;
	//}

	
}
