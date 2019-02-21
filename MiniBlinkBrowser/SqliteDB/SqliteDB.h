//#ifndef SQLITEDB_H
//#define SQLITEDB_H
//
//#define SQLITE_HAS_CODEC 1
//
//extern "C"
//{
//#include "../../SqliteLib/sqlite3.h"
//};
//#ifdef _DEBUG
//#pragma comment(lib, "../lib/Debug/SqliteLib_d.lib")
//#else
//#pragma comment(lib, "../lib/Release/SqliteLib.lib")
//#endif
//#include <string>
//#include <map>
//#include "../Define.h"
//
//class CSqliteDB
//{
//public:
//	CSqliteDB(void);
//	~CSqliteDB(void);
//	bool InitDB();//第一次运行程序调用
//
//	bool Open();
//	bool IsOpen();
//
//	//downinfo表的增删查改
//	bool InsertTaskInfo(const TaskListItemInfo& taskInfo);
//	bool DeleteTaskInfo(int id, bool delBt = true);
//	bool DeleteTaskInfoBySate(TaskState state);
//	bool UpdateTaskFileName(int id, const std::string& strFileName);
//	bool UpdateTaskDownTime(int id, int downTime);
//	bool UpdateTaskDownSize(int id, LONG64 downSize);
//	bool UpdateTaskState(int id, int state);
//	bool UpdateBeforeRecycleState(int id, int state);
//	bool UpdateTaskIsOpen(int id, bool isOpen);
//	bool UpdateTaskBaseInfo(int id, TaskState state, bool isOpen, int downTime, LONG64 downSize, LONG64 totalSize);
//	void LoadTaskInfo(TaskInfoMap& map);
//
//	//bt信息表的增删查改
//	bool SaveBTInfo(BTInfoMap& btMapInfo);
//	bool InsertBTInfo(const BTItemInfo& btInfo);
//	bool DeleteBTInfo(int id);
//	bool UpdateBTInfo(const BTItemInfo& btInfo);
//	bool UpdateBTDownload(int id, int index, BTDownType download);
//	void LoadBTInfo(__in int id, __out BTInfoMap& map, __out std::string& strFilePriorities);
//	bool CheckBTFileIsOk(__in int id, const std::string& strDownDir);
//
//	//最近下载目录列表
//	void LoadHistroyDirInfo(HistroyDirList& historyDirList);
//	bool InsertDir(const std::string& strDir);
//	bool DeleteDir(const std::string& strDir);
//
//	//用户基本信息表 key=> value
//	bool SetValue(__in const std::string& strKey, __in const std::string& strValue);
//	bool GetValue(__in const std::string& strKey, __out std::string& strValue);
//	bool DeleteKey(__in const std::string& strKey);
//
//private:
//	bool CreateTable();
//	bool InsertDbVersion();
//
//	sqlite3 *m_pSqlDb;
//	std::string m_password;
//	std::string m_dbFile;
//	bool m_isOpen;
//};
//
//#endif/*SQLITEDB_H*/
//
//
