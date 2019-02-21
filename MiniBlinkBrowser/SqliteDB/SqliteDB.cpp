//#include "SqliteDB.h"
//
//#include <Windows.h>
//#include <io.h>
//#include <time.h>
//
//#define DB_VERSION _T("1")
//
//#define CREATETABEL_DOWNINFO "create table IF NOT EXISTS downinfo (\
//							id                    INT           PRIMARY KEY\
//																NOT NULL\
//																UNIQUE,\
//							fileName              CHAR( 260 ),\
//							totalSize             INTEGER       DEFAULT '0',\
//							downSize              INTEGER       DEFAULT '0',\
//							state                 INT           DEFAULT '0',\
//							downTime              INT           DEFAULT '0',\
//							isOpen                INT           DEFAULT '0',\
//							downPath              CHAR( 260 ),\
//							downUrl               CHAR( 1024 ),\
//							downType              INT           DEFAULT '0',\
//							uid                   CHAR,\
//							beforeRecycleSate     INT           DEFAULT ( 0 ),\
//							totalUploadSize       INTEGER       DEFAULT ( 0 ),\
//							creationTime          INTEGER       DEFAULT ( 0 ),\
//							completionTime        INTEGER       DEFAULT ( 0 ),\
//							downloadingPeriod     INTEGER       DEFAULT ( 0 ),\
//							removingToRecycleTime INTEGER       DEFAULT ( 0 ),\
//							magnetURI			CHAR( 1024 ),\
//							isMultiFile           INT           DEFAULT ( 0 ),\
//							isAutoRun           INT           DEFAULT ( 0 ),\
//							channel              CHAR( 260 ),\
//							iconPath              CHAR( 260 ),\
//							intExt1               INTEGER       DEFAULT ( 0 ),\
//							intExt2               INTEGER       DEFAULT ( 0 ),\
//							varcharExt1           VARCHAR,\
//							varcharExt2           VARCHAR \
//						);\
//					"
//
//
//#define CREATETABEL_HISTORYDIR "CREATE TABLE IF NOT EXISTS historydir (\
//						id  INTEGER      PRIMARY KEY AUTOINCREMENT\
//						UNIQUE,\
//						dir CHAR( 270 )  DEFAULT ( '' ) \
//						);\
//					"
//
//#define CREATETABEL_USERINFO "CREATE TABLE IF NOT EXISTS userinfo ( \
//						[key] CHAR PRIMARY KEY\
//						UNIQUE,\
//						value CHAR \
//						);\
//					"
//
//
//#define CREATETABEL_BTFILE "CREATE TABLE IF NOT EXISTS btfile ( \
//						btFileId     INTEGER      PRIMARY KEY AUTOINCREMENT,\
//						id		     INTEGER,\
//						status       INTEGER,\
//						fileIndex    INTEGER,\
//						visible      INTEGER,\
//						download     INTEGER,\
//						fileName     CHAR( 260 ),\
//						fileSize     INTEGER      DEFAULT ( 0 ),\
//						fileOffset   INTEGER      DEFAULT ( 0 ),\
//						receivedSize INTEGER      DEFAULT ( 0 ), \
//						intExt1               INTEGER       DEFAULT ( 0 ),\
//						intExt2               INTEGER       DEFAULT ( 0 ),\
//						varcharExt1           VARCHAR,\
//						varcharExt2           VARCHAR \
//						);\
//						"
//
//CSqliteDB::CSqliteDB(void)
//{
//	m_isOpen = false;
//	m_pSqlDb = nullptr;
//	m_password = "";
//	//m_dbFile = GetExePath();
//	m_dbFile += "\\";
//	m_dbFile += "down.db";
//}
//
//CSqliteDB::~CSqliteDB(void)
//{
//	if(m_pSqlDb)
//	{
//		int ret = sqlite3_close(m_pSqlDb);
//		if (ret != SQLITE_OK)
//		{
//		//	//Log("error close database fail: %s", sqlite3_errmsg(m_pSqlDb));  
//		}
//		m_pSqlDb = nullptr;
//	}    
//}
//
//
//bool CSqliteDB::InitDB()
//{
//	if(0 != _access(m_dbFile.c_str(), 0))
//	{
//		//不存在
//		FILE *file = nullptr;
//		errno_t err = fopen_s(&file, m_dbFile.c_str(), "w");
//		if(err != 0)
//		{
//	
//			return false;
//		}
//		fclose(file);
//	}
//	int nRes = sqlite3_open(G2U(m_dbFile).c_str(), &m_pSqlDb);
//	if (nRes != SQLITE_OK)  
//	{  
//	//	//Log("error open database fail: %s", sqlite3_errmsg(m_pSqlDb));  
//		return false;
//	}
//	/*需要密码*/
//	if(m_password.empty())
//	{
//		return CreateTable();
//	}
//	nRes = sqlite3_key(m_pSqlDb, m_password.c_str(), (int)m_password.length());
//	nRes = CreateTable();
//
//	return (nRes == SQLITE_OK);
//}
//
//
//bool CSqliteDB::Open()
//{
//	int nRes = sqlite3_open(G2U(m_dbFile).c_str(), &m_pSqlDb);  
//	if (nRes != SQLITE_OK)  
//	{  
//		//Log("error open database fail: %s", sqlite3_errmsg(m_pSqlDb));  
//		return false;
//	}
//	/*需要密码*/
//	if(m_password.empty())
//	{
//		m_isOpen = (nRes == SQLITE_OK);
//		return m_isOpen;
//	}
//	nRes = sqlite3_key(m_pSqlDb, m_password.c_str(), (int)m_password.length());
//	m_isOpen = (nRes == SQLITE_OK);
//	return m_isOpen;
//}
//
//bool CSqliteDB::IsOpen()
//{
//	return m_isOpen;
//}
//
//bool CSqliteDB::CreateTable()
//{
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, CREATETABEL_DOWNINFO, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error create table downinfo fail: %s", errmsg);
//		return false;
//	}
//	nRes = sqlite3_exec(m_pSqlDb, CREATETABEL_HISTORYDIR, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error create table historydir fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//
//	nRes = sqlite3_exec(m_pSqlDb, CREATETABEL_USERINFO, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error create table userinfo fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//
//	nRes = sqlite3_exec(m_pSqlDb, CREATETABEL_BTFILE, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error create table btfile fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	//db版本号
//	InsertDbVersion();
//	return true;
//}
//
////设置db的版本号
//bool CSqliteDB::InsertDbVersion()
//{
//	return SetValue("dbVersion", DB_VERSION);
//}
//
//
///*
//CREATE TABLE downinfo ( 
//id        INT          PRIMARY KEY NOT NULL UNIQUE,
//fileName  CHAR( 260 ),
//totalSize INTEGER      DEFAULT '0',
//downSize  INTEGER      DEFAULT ( 0 ),
//state     INT          DEFAULT '0',
//downTime  INTEGER      DEFAULT '0',
//isOpen    INT          DEFAULT '0',
//downPath  CHAR( 256 ),
//downUrl   VARCHAR,
//uid                   CHAR,
//beforeRecycleSate     INT,
//totalUploadSize       INTEGER,
//creationTime          INTEGER,
//completionTime        INTEGER,
//downloadingPeriod     INTEGER,
//removingToRecycleTime INTEGER 
//magnetURI   VARCHAR,
//isMultiFile   int,
//isAutoRun		int,
//channel              CHAR( 260 ),\
//iconPath              CHAR( 260 ),\
//intExt1               INTEGER       DEFAULT ( 0 ), 是否修改过名字
//intExt2               INTEGER       DEFAULT ( 0 ),
//varcharExt1           VARCHAR,
//varcharExt2           VARCHAR 
//);
//*/
//bool CSqliteDB::InsertTaskInfo(const TaskListItemInfo& taskInfo)
//{
//	char strSql[1024* 4];
//	//bt保存种子相对目录
//	std::string strDownUrl;
//	if(taskInfo.downType == DOWNTYPE_BT)
//	{
//		char szDownUrl[1024] = {0};
//		_sntprintf_s(szDownUrl, sizeof(szDownUrl), "torrentfiles\\%s.torrent", taskInfo.uid);
//		strDownUrl = szDownUrl;
//	}
//	else
//	{
//		strDownUrl = taskInfo.downUrl;
//	}
//	_sntprintf_s(strSql, sizeof(strSql), "insert into downinfo values(%d, '%s', %lld, %lld, %d, %d, %d, '%s', '%s', %d,\
//							'%s', %d, %lld, %lld, %lld, %lld, %lld, '%s', %d, %d, '%s', '%s', %d, %d, '%s', '%s')",
//		taskInfo.id, 
//		G2U(taskInfo.fileName).c_str(), 
//		taskInfo.totalSize, 
//		taskInfo.downSize, 
//		taskInfo.state, 
//		taskInfo.downTime, 
//		taskInfo.isOpen,
//		G2U(taskInfo.downPath).c_str(),
//		G2U(strDownUrl).c_str(),
//		taskInfo.downType,
//		G2U(taskInfo.uid).c_str(),
//		taskInfo.beforeRecycleSate,
//		taskInfo.totalUploadSize,
//		taskInfo.creationTime,
//		taskInfo.completionTime,
//		taskInfo.downloadingPeriod,
//		taskInfo.removingToRecycleTime,
//		G2U(taskInfo.magnetURI).c_str(),
//		taskInfo.bIsMultiFile,
//		taskInfo.autoRunType,
//		G2U(taskInfo.channel).c_str(), 
//		G2U(taskInfo.iconPath).c_str(), 
//		taskInfo.bFileNameModify,
//		0, "", "" );
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error InsertDownInfo fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::UpdateTaskFileName(int id, const std::string& strFileName)
//{
//	char strSql[512];
//	_sntprintf_s(strSql, sizeof(strSql), "update downinfo set fileName='%s' where id = %d", G2U(strFileName).c_str(), id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateTaskFileName fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::UpdateTaskIsOpen(int id, bool isOpen)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql), "update downinfo set isOpen='%d' where id = %d", isOpen, id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateTaskIsOpen fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::UpdateTaskDownSize(int id, LONG64 downSize)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql), "update downinfo set downSize='%lld' where id = %d", downSize, id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateTaskDownSize fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//bool CSqliteDB::UpdateTaskDownTime(int id, int downTime)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql), "update downinfo set downTime='%d' where id = %d", downTime, id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateTaskDownTime fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//bool CSqliteDB::UpdateTaskState(int id, int state)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql), "update downinfo set state='%d' where id = %d", state, id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateTaskState fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//bool CSqliteDB::UpdateBeforeRecycleState(int id, int state)
//{
//	char strSql[128];
//	_sntprintf_s(strSql, sizeof(strSql), "update downinfo set beforeRecycleSate=%d, removingToRecycleTime=%lld  where id = %d", state, time(NULL), id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateTaskState fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::UpdateTaskBaseInfo(int id, TaskState state, bool isOpen, int downTime, LONG64 downSize, LONG64 totalSize)
//{
//	char strSql[128];
//	_sntprintf_s(strSql, sizeof(strSql), "update downinfo set state='%d',isOpen='%d',downTime='%d',downSize='%lld',totalSize='%lld' where id = %d", state, isOpen, downTime, downSize, totalSize, id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateTaskBaseInfo fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::DeleteTaskInfo(int id, bool delBt/* = true*/)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql), "delete from downinfo where id = %d", id);
//
//	//同时删除bt信息
//	if(delBt)
//	{
//		DeleteBTInfo(id);
//	}
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error DeleteTaskInfo fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::DeleteTaskInfoBySate(TaskState state)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql),"delete from downinfo where state = %d", state);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error DeleteTaskInfoBySate fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//void CSqliteDB::LoadTaskInfo(TaskInfoMap& map)
//{
//	sqlite3_stmt * stmt = NULL; 
//	const char *zTail;
//	if(sqlite3_prepare_v2(m_pSqlDb, "select * from downinfo", -1, &stmt, &zTail) == SQLITE_OK)
//	{       
//		while(sqlite3_step(stmt) == SQLITE_ROW ) 
//		{ 
//			TaskListItemInfo itemInfo;
//			itemInfo.id = sqlite3_column_int(stmt, 0);
//			std::string strFileName = U2G((char*)sqlite3_column_text(stmt, 1));
//		    _sntprintf_s(itemInfo.fileName, sizeof(itemInfo.fileName), "%s", strFileName.c_str());
//				
//			itemInfo.totalSize = sqlite3_column_int64(stmt, 2); 
//			itemInfo.downSize = sqlite3_column_int64(stmt, 3); 
//			itemInfo.state = (TaskState)sqlite3_column_int(stmt, 4); 
//			itemInfo.downTime = sqlite3_column_int(stmt, 5); 
//			itemInfo.isOpen = (bool)sqlite3_column_int(stmt, 6);
//
//			std::string strDownPath = U2G((char*)sqlite3_column_text(stmt, 7));
//			_sntprintf_s(itemInfo.downPath, sizeof(itemInfo.downPath), "%s", strDownPath.c_str());
//
//			itemInfo.downType = (DownType)sqlite3_column_int(stmt, 9);
//
//			//bt下载因为数据库保存相对目录 补全路径
//			std::string strDownUrl = U2G((char*)sqlite3_column_text(stmt, 8));
//			if(itemInfo.downType == DOWNTYPE_BT)
//			{
//				_sntprintf_s(itemInfo.downUrl, sizeof(itemInfo.downUrl), "%s\\%s", GetExePath().c_str(), strDownUrl.c_str());
//			}
//			else
//			{
//				_sntprintf_s(itemInfo.downUrl, sizeof(itemInfo.downUrl), "%s", strDownUrl.c_str());
//			}
//			 
//			//uid
//			std::string strUid = U2G((char*)sqlite3_column_text(stmt, 10));
//			_sntprintf_s(itemInfo.uid, sizeof(itemInfo.uid), "%s", strUid.c_str());
//
//			itemInfo.beforeRecycleSate = (TaskState)sqlite3_column_int(stmt, 11); 
//			itemInfo.totalUploadSize = sqlite3_column_int64(stmt, 12); 
//			itemInfo.creationTime = sqlite3_column_int64(stmt, 13); 
//			itemInfo.completionTime = sqlite3_column_int64(stmt, 14); 
//			itemInfo.downloadingPeriod = sqlite3_column_int64(stmt, 15); 
//			itemInfo.removingToRecycleTime = sqlite3_column_int64(stmt, 16);
//
//			//magnetURI
//			std::string strMagnetURI = U2G((char*)sqlite3_column_text(stmt, 17));
//			_sntprintf_s(itemInfo.magnetURI, sizeof(itemInfo.magnetURI), "%s", strMagnetURI.c_str());
//
//			//bIsMultiFile
//			itemInfo.bIsMultiFile = (bool)sqlite3_column_int(stmt, 18);
//			itemInfo.autoRunType = (OpType)sqlite3_column_int(stmt, 19);
//
//			//channel
//			std::string strChannel = U2G((char*)sqlite3_column_text(stmt, 20));
//			_sntprintf_s(itemInfo.channel, sizeof(itemInfo.channel), "%s", strChannel.c_str());
//			
//			//iconPath
//			std::string strIconPath = U2G((char*)sqlite3_column_text(stmt, 21));
//			_sntprintf_s(itemInfo.iconPath, sizeof(itemInfo.iconPath), "%s", strIconPath.c_str());
//
//			//intExt1
//			itemInfo.bFileNameModify = (bool)sqlite3_column_int(stmt, 22);
//
//			//加载信息
//			if(itemInfo.downType == DOWNTYPE_BT)
//			{
//				BTInfoMap mapInfo;
//				std::string strFilePriorities;
//				LoadBTInfo(itemInfo.id, mapInfo, strFilePriorities);
//				_sntprintf_s(itemInfo.szFilePriorities, sizeof(itemInfo.szFilePriorities), "%s", strFilePriorities.c_str());
//			}
//			map[itemInfo.id] = itemInfo;
//			/*const unsigned char * name = sqlite3_column_text( stmt,1); 
//			int number = sqlite3_column_int(stmt, 2); 
//			//Log("ID: %d  Name: %s  Age: %d /n",id,name,number); */
//
//			
//		} 
//	}
//	else
//	{
//		//Log("error LoadDownInfo fail: %s", sqlite3_errmsg(m_pSqlDb));
//	}
//	sqlite3_finalize(stmt); 
//}
//
//
//
////=======================最近下载目录表===================================================
////加载历史下载目录
//void CSqliteDB::LoadHistroyDirInfo(HistroyDirList& historyDirList)
//{
//	sqlite3_stmt * stmt = NULL; 
//	const char *zTail;
//	if(sqlite3_prepare_v2(m_pSqlDb, "select * from historydir ORDER BY id desc", -1, &stmt, &zTail) == SQLITE_OK)
//	{       
//		while(sqlite3_step(stmt) == SQLITE_ROW ) 
//		{ 
//			std::string strDir = U2G((char*)sqlite3_column_text(stmt, 1));
//			CDuiString str = strDir.c_str();
//			historyDirList.push_back(str);
//			TaskListItemInfo itemInfo;
//			/*const unsigned char * name = sqlite3_column_text( stmt,1); 
//			int number = sqlite3_column_int(stmt, 2); 
//			//Log("ID: %d  Name: %s  Age: %d /n",id,name,number); */
//		} 
//	}
//	else
//	{
//		//Log("error LoadHistroyDirInfo fail: %s", sqlite3_errmsg(m_pSqlDb));
//	}
//	sqlite3_finalize(stmt); 
//}
//
//
//bool CSqliteDB::InsertDir(const std::string& strDir)
//{
//	char strSql[512];
//	_sntprintf_s(strSql, sizeof(strSql), "insert into historydir values(null, '%s')", G2U(strDir).c_str());
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error InsertDir fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::DeleteDir(const std::string& strDir)
//{
//	char strSql[512];
//	_sntprintf_s(strSql, sizeof(strSql), "delete from historydir where dir = '%s'", G2U(strDir).c_str());
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error DeleteDir fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
////==========================用户属性表====================================//
//bool CSqliteDB::SetValue(__in const std::string& strKey, __in const std::string& strValue)
//{
//	char strSql[1024];
//	_sntprintf_s(strSql, sizeof(strSql), "replace into userinfo values('%s', '%s')", G2U(strKey).c_str(), G2U(strValue).c_str());
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error SetValue fail: %s key=[%s] value=[%s]", errmsg, strKey.c_str(), strValue.c_str());
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//bool CSqliteDB::GetValue(__in const std::string& strKey, __out std::string& strValue)
//{
//	char strSql[1024];
//	_sntprintf_s(strSql, sizeof(strSql), "select * from userinfo where key='%s'",  G2U(strKey).c_str());
//
//	sqlite3_stmt * stmt = NULL; 
//	const char *zTail;
//	bool bRet = false;
//	if(sqlite3_prepare_v2(m_pSqlDb, strSql, -1, &stmt, &zTail) == SQLITE_OK)
//	{       
//		while(sqlite3_step(stmt) == SQLITE_ROW ) 
//		{ 
//			strValue = U2G((char*)sqlite3_column_text(stmt, 1));
//			bRet = true;
//		} 
//	}
//	else
//	{
//		//Log("error GetValue fail: %s", sqlite3_errmsg(m_pSqlDb));
//	}
//	sqlite3_finalize(stmt);
//	return bRet;
//}
//
//
//bool CSqliteDB::DeleteKey(__in const std::string& strKey)
//{
//	char strSql[512];
//	_sntprintf_s(strSql, sizeof(strSql), "delete from userinfo where key = '%s'", G2U(strKey.c_str()).c_str());
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error DeleteKey fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
////==========================bt信息表====================================//
//bool CSqliteDB::SaveBTInfo(BTInfoMap& btMapInfo)
//{
//	std::map<int, BTItemInfo>::iterator iter = btMapInfo.begin();
//	while(iter != btMapInfo.end())
//	{
//		InsertBTInfo(iter->second);
//		iter++;
//	}
//	return true;
//}
//
//bool CSqliteDB::InsertBTInfo(const BTItemInfo& btInfo)
//{
//	char strSql[1024* 4];
//	_sntprintf_s(strSql, sizeof(strSql), "insert into btfile values(NULL, %d, %d, %d, %d, %d, '%s', %lld, %lld, %lld, \
//										 %d, %d, '%s', '%s')",
//		btInfo.id, 
//		btInfo.status, 
//		btInfo.fileIndex, 
//		btInfo.visible, 
//		btInfo.download, 
//		G2U(btInfo.fileName).c_str(), 
//		btInfo.fileSize,
//		btInfo.fileOffset,
//		btInfo.receivedSize,
//		0, 0, "", "");
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error InsertBTInfo fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//void CSqliteDB::LoadBTInfo(__in int id, __out BTInfoMap& map, __out std::string& strFilePriorities)
//{
//	sqlite3_stmt * stmt = NULL; 
//	const char *zTail;
//
//	char strSql[128];
//	_sntprintf_s(strSql, sizeof(strSql), "select * from btfile where id=%d", id);
//
//	if(sqlite3_prepare_v2(m_pSqlDb, strSql, -1, &stmt, &zTail) == SQLITE_OK)
//	{       
//		while(sqlite3_step(stmt) == SQLITE_ROW ) 
//		{ 
//			BTItemInfo itemInfo;
//			//sqlite3_column_int(stmt, 0); 自增长的btFileId没用到
//			itemInfo.id = sqlite3_column_int(stmt, 1); 
//			itemInfo.status = sqlite3_column_int(stmt, 2); 
//			itemInfo.fileIndex = sqlite3_column_int(stmt, 3); 
//			itemInfo.visible = sqlite3_column_int(stmt, 4); 
//			itemInfo.download = (BTDownType)sqlite3_column_int(stmt, 5); 
//
//			std::string strFileName = U2G((char*)sqlite3_column_text(stmt, 6));
//			_sntprintf_s(itemInfo.fileName, sizeof(itemInfo.fileName), "%s", strFileName.c_str());
//			
//			itemInfo.fileSize = sqlite3_column_int64(stmt, 7); 
//			itemInfo.fileOffset = sqlite3_column_int64(stmt, 8); 
//			itemInfo.receivedSize = (TaskState)sqlite3_column_int(stmt, 9); 
//		
//			map[itemInfo.fileIndex] = itemInfo;
//			/*const unsigned char * name = sqlite3_column_text( stmt,1); 
//			int number = sqlite3_column_int(stmt, 2); 
//			//Log("ID: %d  Name: %s  Age: %d /n",id,name,number); */
//			if(itemInfo.download == BT_DOWN)
//			{
//				strFilePriorities.append("1");
//			}
//			else
//			{
//				strFilePriorities.append("0");
//			}
//
//		} 
//	}
//	else
//	{
//		//Log("error LoadBTInfo fail: %s", sqlite3_errmsg(m_pSqlDb));
//	}
//	sqlite3_finalize(stmt); 
//}
//
//
////检测bt下载文件是否缺少
//bool CSqliteDB::CheckBTFileIsOk(__in int id, const std::string& strDownDir)
//{
//	bool fileIsOk = false;
//	sqlite3_stmt * stmt = NULL; 
//	const char *zTail;
//
//	char strSql[128];
//	_sntprintf_s(strSql, sizeof(strSql), "select * from btfile where id=%d", id);
//
//	if(sqlite3_prepare_v2(m_pSqlDb, strSql, -1, &stmt, &zTail) == SQLITE_OK)
//	{   
//		fileIsOk = true;
//		while(sqlite3_step(stmt) == SQLITE_ROW ) 
//		{ 
//			BTItemInfo itemInfo;
//			//sqlite3_column_int(stmt, 0); 自增长的btFileId没用到
//			/*itemInfo.id = sqlite3_column_int(stmt, 1); 
//			itemInfo.status = sqlite3_column_int(stmt, 2); 
//			itemInfo.fileIndex = sqlite3_column_int(stmt, 3); 
//			itemInfo.visible = sqlite3_column_int(stmt, 4); */
//			itemInfo.download = (BTDownType)sqlite3_column_int(stmt, 5); 
//
//			std::string strFileName = U2G((char*)sqlite3_column_text(stmt, 6));
//			_sntprintf_s(itemInfo.fileName, sizeof(itemInfo.fileName), "%s", strFileName.c_str());
//			
//			/*itemInfo.fileSize = sqlite3_column_int64(stmt, 7); 
//			itemInfo.fileOffset = sqlite3_column_int64(stmt, 8); 
//			itemInfo.receivedSize = (TaskState)sqlite3_column_int(stmt, 9); */
//
//			//文件不存在 则返回失败
//			if(itemInfo.download == BT_DOWN)
//			{
//				strFileName = strDownDir + _T("\\") + strFileName;
//				if(_access(strFileName.c_str(), 0) != 0)
//				{
//					fileIsOk = false;
//					break;
//				}
//			}
//		} 
//	}
//	else
//	{
//		//Log("error CheckBTFileIsOk fail: %s", sqlite3_errmsg(m_pSqlDb));
//	}
//	sqlite3_finalize(stmt); 
//	return fileIsOk;
//}
//
//
//bool CSqliteDB::DeleteBTInfo(int id)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql), "delete from btfile where id = %d", id);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error DeleteBTInfo fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::UpdateBTDownload(int id, int index, BTDownType download)
//{
//	char strSql[64];
//	_sntprintf_s(strSql, sizeof(strSql), "update btfile set download='%d' where id = %d and fileIndex=%d", download, id, index);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateBTDownload fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}
//
//
//bool CSqliteDB::UpdateBTInfo(const BTItemInfo& btInfo)
//{
//	char strSql[128];
//	_sntprintf_s(strSql, sizeof(strSql), "update btfile set \
//							status='%d',\
//							visible='%d',\
//							download='%d',\
//							fileSize='%lld',\
//							fileOffset='%lld' \
//							receivedSize='%lld' \
//							where id = %d and fileIndex=%d", 
//							btInfo.status, 
//							btInfo.visible,
//							btInfo.download,
//							btInfo.fileSize,
//							btInfo.fileOffset,
//							btInfo.receivedSize,
//							btInfo.id,
//							btInfo.fileIndex);
//
//	char* errmsg = nullptr;
//	int nRes = sqlite3_exec(m_pSqlDb, strSql, NULL, NULL, &errmsg);
//	if (nRes != SQLITE_OK)
//	{
//		//Log("error UpdateBTInfo fail: %s", errmsg);
//		sqlite3_free(errmsg);
//		return false;
//	}
//	return true;
//}