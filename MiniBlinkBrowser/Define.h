#ifndef DEFINE_H
#define DEFINE_H


#include "stdafx.h"
#include "UI/MiniControls.h"
#include <map>
#include <vector>
#include <list>

#define DUI_MSGTYPE_RECYCLE_ITEMDELETE 		   	   (_T("recycleitemdelete"))
#define DUI_MSGTYPE_RECYCLE_ITEMRESTORE 		   (_T("recycleitemrestore"))
#define DUI_MSGTYPE_FINISHED_ITEMDELETE 		   (_T("finisheditemdelete"))
#define DUI_MSGTYPE_FINISHED_ITEMSELECT 		   (_T("finisheditemselect"))
#define DUI_MSGTYPE_FINISHED_FILEOPEN 		       (_T("finishedfileopen"))
#define DUI_MSGTYPE_FINISHED_FOLDEROPEN 		   (_T("finishedfolderopen"))
#define DUI_MSGTYPE_DOWN_ITEMDELETE 			   (_T("downitemdelete"))
#define DUI_MSGTYPE_DOWN_ITEMDSTOP 				   (_T("downitemstop"))
#define DUI_MSGTYPE_DOWN_ITEMDCONTINUE 			   (_T("downitemcontinue"))
#define DUI_MSGTYPE_DOWN_ITEMDSELECT 			   (_T("downitemselect"))

#define MUTEX_ID		_T("{9815E790-EC44-4D5F-A415-3D3CA0CBFF32}")
#define APP_NAME		_T("小皮下载器")
#define APP_MAJOR		_T("1.0.1.0")
#define APP_CHANNEL		_T("xiaopi")

#define APP_UPDATE_URL	_T("http://xiazaiqiapi.xiaopi.com/down/bturl/?channel=update")
#define APP_VERSION_URL	_T("http://xiazaiqiapi.xiaopi.com/down/param/?channel=%s")
//统计接口
#define STAT_START_URL			_T("http://xiazaiqiapi.xiaopi.com/bt/count/stinfo/")
#define STAT_UPLOADINFO_URL		_T("http://xiazaiqiapi.xiaopi.com/bt/count/uploadinfo/")
#define GET_HEARTBEAT_TIME_URL	_T("http://xiazaiqiapi.xiaopi.com/bt/api/heartbeat/")
#define STAT_BINDAPP_URL		_T("http://xiazaiqiapi.xiaopi.com/bt/count/zsxiazai/")

//绑定下载接口
#define BIND_HTTP_PREURL_FORMAT		_T("http://xiazaiqiapi.xiaopi.com/down/bturl/?channel=%s")
#define BIND_BTFILE_PREURL_FORMAT	_T("http://xiazaiqiapi.xiaopi.com/down/bturl/?channel=%s")
#define REQUEST_IP_URL				_T("http://api.xiaopi.com/helper/helperapi/uip/")

#define UPDATE_TEMPDIR		_T("uptemp")
#define UPDATE_TEMPFILE		_T("upgrade.temp")
#define UPDATE_FILE			_T("upgrade.zip")
#define TEMP_DIR_NAME		_T("xpAppUpgrade")
#define TORRENT_DIR			_T("torrentfiles")
#define UP_STATE_KEY		_T("upState")



#define UIEVENT_TRAYICON				WM_USER + 0x1000
#define WM_NOTIFY_CREATEHTTPTASK		WM_USER + 0x1001
#define WM_NOTIFY_OPENBTFILE			WM_USER + 0x1002
#define WM_NOTIFY_CREATEBTTASK			WM_USER + 0x1003
#define WM_NOTIFY_COPYDATA_PROCESS		WM_USER + 0x1004
#define WM_NOTIFY_COPYDATA_BIND			WM_USER + 0x1005
#define WM_NOTIFY_GETVERSION			WM_USER + 0x1006
#define WM_NOTIFY_GETONLINETIME			WM_USER + 0x1007
#define WM_NOTIFY_REMOTE_BIND			WM_USER + 0x1008
#define WM_NOTIFY_COPYDATA_REMOTE_BIND	WM_USER + 0x1009
#define WM_NOTIFY_CREATEHTTPTASKBATCH	WM_USER + 0x1010
#define WM_NOTIFY_RECYCLE_RESTORE_TASK	WM_USER + 0x1011
#define WM_NOTIFY_RECYCLE_DELETE_TASK	WM_USER + 0x1012
#define WM_NOTIFY_RECYCLE_DELETEALLTASK	WM_USER + 0x1013


//状态
enum TaskState
{
	ERROR_STATE,
	STARTPENDING_STATE,
	NORMAL_STATE,
	STOP_STATE,
	STOPPENDING_STATE,
	FINISHED_STATE,
	RECYCLE_STATE,
};

//创建下载任务类型
enum DownType
{
	DOWNTYPE_UNKNOW,
	DOWNTYPE_HTTP,
	DOWNTYPE_BT
};

//文件下载完成操作
enum OpType
{
	OP_NONE,
	OP_RUN,
	OP_OPENDIR
};

//下载列表 完成列表 回收站列表信息
struct TaskListItemInfo
{
	TaskListItemInfo()
	{
		id = -1;
		memset(fileName, 0, sizeof(fileName));
		totalSize = 0;
		downSize = 0;
		downTime = 0;
		downSpeed = 0;
		isOpen = false;
		linkCnt = 0;
		totalLinkCnt = 0;
		memset(downPath, 0, sizeof(downPath));
		memset(downUrl, 0, sizeof(downUrl));
		memset(uid, 0, sizeof(uid));
		statusChangeTime = 0;
		totalUploadSize = 0;
		creationTime = 0;
		completionTime = 0;
		downloadingPeriod = 0;
		removingToRecycleTime = 0;
		memset(magnetURI, 0, sizeof(magnetURI));
		bIsMultiFile = false;
		autoRunType = OP_NONE;
		memset(channel, 0, sizeof(channel));
		memset(iconPath, 0, sizeof(iconPath));
		taskId = -1;
		memset(szFilePriorities, 0, sizeof(szFilePriorities));
		lastTotalUploadSize = 0;
		bStartSeeding = false;
		bFileNameModify = false;
	}
	int id;//自身维护的id
	TCHAR fileName[260];//下载文件名
	LONG64 totalSize;//总大小
	LONG64 downSize;//已经下载的大小
	TaskState state;//下载状态
	int downTime;//已经下载的时间
	int downSpeed;/*b/s*/
	bool isOpen;/*下载完成后的文件是否打开过*/
	int linkCnt;//下载连接数
	int totalLinkCnt;//总连接数
	char downPath[260];//下载目录
	char downUrl[1024];//下载链接
	DownType downType;//下载类型
	char uid[1024];////文件标识符， 用来唯一指定某个文件， http下载标识符为下载链接地址， bt直接用里面的info_hash字段进行md5
	TaskState beforeRecycleSate;//移到回收站钱的状态
	LONG64 statusChangeTime;//状态变化时间
	LONG64 totalUploadSize;//上传总大小
	LONG64 creationTime;//创建时间
	LONG64 completionTime;//完成时间
	LONG64 downloadingPeriod;//下载优先级
	LONG64 removingToRecycleTime;//移到回收站时间
	char magnetURI[1024];//磁力链接或者重定向链接
	bool bIsMultiFile;//是否是多文件
	OpType autoRunType;//下载结束后的操作, 0: 无操作； 1：立即运行； 2：打开资源所在文件夹
	char channel[260];//渠道 xiaopi、other
	char iconPath[260];//图标路径
	bool bFileNameModify;//是否有修改过文件名
	//以下值没存数据库
	int taskId;//任务启动下载时候才有的id
	char szFilePriorities[2049];   //种子中带下载的文件的开关， 0表示不下载， 1表示下载， 格式如下： 1001
	LONG64 lastTotalUploadSize;//上次上传总大小
	bool bStartSeeding;//是否是第一次做种
};


//bt是否下载
enum BTDownType
{
	BT_UNDOWN,
	BT_DOWN
};


//bt下载信息
struct BTItemInfo
{
	BTItemInfo()
	{
		id = -1;
		status = 0;
		fileIndex = 0;
		visible = 1;
		download = BT_DOWN;
		memset(fileName, 0, sizeof(fileName));
		fileSize = 0;
		fileOffset = 0;
		receivedSize = 0;
	}
	int id;
	int status;
	int fileIndex;
	int visible;
	BTDownType download;
	char fileName[MAX_PATH + 1];
	LONG64 fileSize;
	LONG64 fileOffset;
	LONG64 receivedSize;
};



struct UpLoadItem
{
	UpLoadItem()
	{
		memset(fileName, 0, sizeof(fileName));
		memset(downUrl, 0, sizeof(downUrl));
		memset(channel, 0, sizeof(channel));
		memset(info_tag, 0, sizeof(info_tag));
		type = DOWNTYPE_UNKNOW;
		upSize = 0;
		status = 1;
	}
	char fileName[MAX_PATH + 1];//上传文件名
	char downUrl[1024];//上传文件名
	char channel[MAX_PATH + 1];//渠道 xiaopi other
	DownType type;//下载类型（http， bt等）
	char info_tag[128];//文件标识符， 用来唯一指定某个文件， http下载标识符为下载链接地址+大小进行md5， bt直接用里面的info_hash字段进行md5
	LONG64 upSize;//上传大小
	int status;//做种状态， 0：结束做种； 1：正在做种
};

//http任务
struct HttpTask
{
	HttpTask()
	{
		memset(szUrl, 0, sizeof(szUrl));
		memset(szSavePath, 0, sizeof(szSavePath));
		memset(szSaveName, 0, sizeof(szSaveName));
		memset(szChannel, 0, sizeof(szChannel));
		memset(szIconPath, 0, sizeof(szIconPath));
		autoRunType = OP_NONE;
		bFileNameModify = false;
	}
	char szUrl[1024];
	char szSavePath[MAX_PATH + 1];
	char szSaveName[MAX_PATH + 1];
	char szChannel[MAX_PATH + 1];
	char szIconPath[MAX_PATH + 1];
	OpType autoRunType;//下载结束后的操作, 0: 无操作； 1：立即运行； 2：打开资源所在文件夹
	bool bFileNameModify;//是否有修改过文件名
};


typedef std::map<int, BTItemInfo> BTInfoMap;

//bt任务
struct BTTask
{
	BTTask()
	{
		memset(szSavePath, 0, sizeof(szSavePath));
		memset(szSeedFile, 0, sizeof(szSeedFile));
		memset(szSaveName, 0, sizeof(szSaveName));
		memset(szInfoHashMd5, 0, sizeof(szInfoHashMd5));
		bIsMultiFile = false;
		memset(szChannel, 0, sizeof(szChannel));
		memset(szIconPath, 0, sizeof(szIconPath));
		autoRunType = OP_NONE;
	}
	BTInfoMap btInfo;
	char szSavePath[MAX_PATH + 1];
	char szSeedFile[MAX_PATH + 1];
	char szSaveName[MAX_PATH + 1];
	char szInfoHashMd5[128];
	bool bIsMultiFile;
	char szChannel[MAX_PATH + 1];
	char szIconPath[MAX_PATH + 1];
	OpType autoRunType;//下载结束后的操作, 0: 无操作； 1：立即运行； 2：打开资源所在文件夹
};


typedef std::map<int, TaskListItemInfo> TaskInfoMap;


typedef std::vector<CDuiString> HistroyDirList;
typedef std::vector<UpLoadItem> UpLoadList;


//status = 0：结束做种； 1：开始做种 2：刷新上传数据状态
enum SeedStatus
{
	SEED_STOP = 0,
	SEED_START,
	SEED_UPDATE
};

#define XIAOPI_FILE_DEFAULT _T("xpDown")


inline CDuiString ByteToFitFloat(LONG64 size)
{
	CDuiString strSize;
	if(size /1024 / 1024 / 1024 > 0)
	{
		float fRate = size /1024.f / 1024.f / 1024.f;
		strSize.Format(_T("%0.2fG"), fRate);
	}
	else if(size /1024 / 1024 > 0)
	{
		float fRate = size /1024.f / 1024.f;
		strSize.Format(_T("%0.2fM"), fRate);
	}
	else if(size /1024 > 0)
	{
		float fRate = size /1024.f;
		strSize.Format(_T("%0.2fKB"), fRate);
	}
	else
	{
		strSize.Format(_T("%dB"), size);
	}
	return strSize;
}

inline CDuiString ByteToFitUnit(LONG64 size)
{
	CDuiString strSize;
	if(size /1024 / 1024 / 1024 > 0)
	{
		float fRate = size /1024.f / 1024.f / 1024.f;
		strSize.Format(_T("%0.2fG"), fRate);
	}
	else if(size /1024 / 1024 > 0)
	{
		strSize.Format(_T("%dM"), size /1024 / 1024);
	}
	else if(size /1024 > 0)
	{
		strSize.Format(_T("%dKB"), size /1024);
	}
	else
	{
		strSize.Format(_T("%dB"), size);
	}
	return strSize;
}

#endif

