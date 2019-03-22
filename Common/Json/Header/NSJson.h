/**  * @copyright (C) 2016 ChinaNetCenter Inc. All rights reserved.
*
* @file NSJson.h
* @summary  NSJsonObject	Json对象
*			NSJsonArray		Json数组
*			NSJson			Json解析
*
* @version 1.0
* @author zhengzc@wangsu.com
* @date 2018-03-05 14:25:06
*
* @others
* @MODIFY    VERSION  YYYYMMDD    TITLE
* zhengzc      1.0    20180320     title
*
*/


#ifndef NSJsonH
#define NSJsonH

#define JSON_HAS_INT64
#include "json.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;


class NSJsonArray;
class NSJsonObject
{

#define NSJsonObj_NULL NSJsonObject()

public:

	// 构造函数
	NSJsonObject();

	// 析构函数
	~NSJsonObject();

public:

	// 获取Json::Value对象
	Json::Value	_GetValue();

	// 设置Json::Value
	void		_SetValue(Json::Value Value);

	__declspec(property (get = _GetValue, put = _SetValue)) Json::Value Value;

public:

	// 添加数据
	void		Put(std::string Key, int Value, bool Isbool = false);

	void		Put(std::string Key, unsigned int Value);

	void		Put(std::string Key, __int64 Value);

	void		Put(std::string Key, unsigned __int64 Value);

	void		Put(std::string Key, double Value);

	void		Put(std::string Key, float Value);

	void		Put(std::string Key, std::string Value);

	void		Put(std::string Key, NSJsonObject JsonObj);

	void		Put(std::string Key, NSJsonArray JsonArr);

	// 判断指定键的值的类型
	bool		IsInt(std::string Key);

	bool		IsUInt(std::string Key);

	bool		IsInt64(std::string Key);

	bool		IsUInt64(std::string Key);

	bool		IsBool(std::string Key);

	bool		IsDouble(std::string Key);

	bool		IsString(std::string Key);

	bool		IsObject(std::string Key);

	bool		IsArray(std::string Key);

	//返回默认值
	bool				_GetDefaultBool(std::string key);
	int					_GetDefaultInt(std::string key);
	float				_GetDefaultFloat(std::string key);
	std::string			_GetDefaultString(std::string key);
	NSJsonObject		_GetDefaultObject(std::string key);
	NSJsonArray			_GetDefaultArray(std::string key);

	// 获取值（Key对应的键值对必须存在）
	bool				GetBool(std::string Key);

	int					GetInt(std::string Key);

	unsigned int		GetUInt(std::string Key);

	double				GetDouble(std::string Key);

	float				GetFloat(std::string Key);

	std::string			GetString(std::string Key);

	NSJsonObject		Get(std::string Key);

	NSJsonArray			GetArr(std::string Key);


	// 获取值（Key对应的键值可以不存在）
	bool				OptGetBool(std::string Key, bool Default = false);

	int					OptGetInt(std::string Key, int Default = 0);

	unsigned int		OptGetUInt(std::string Key, unsigned int Default = 0);

	double				OptGetDouble(std::string Key, double Default = 0);

	float				OptGetFloat(std::string Key, float Default = 0);

	std::string			OptGetString(std::string Key, std::string Default = "");

	NSJsonObject		OptGet(std::string Key, NSJsonObject Default = NSJsonObj_NULL);


	// 判断指定键的值是否为空(若Name为空则判断当前对象)
	bool				IsNull(std::string Key = "");

	// 获取所有Key
	std::vector<std::string>	GetKeys();

	// 移除键值对
	void				Remove(std::string Key);

	// 获取键值对总数
	unsigned int		Size();

	// 比较两个Obj
	int					Compare(NSJsonObject JsonObj);

	// 清空
	void				Clear();

	// 获取Json格式字符串
	std::string			ToString(bool bStyled = true);

	// 转换成NSJsonArray
	NSJsonArray			ToArray();


private:

	Json::Value		m_Obj;
};

class NSJsonArray
{

public:

	// 构造函数
	NSJsonArray();

	NSJsonArray(const std::vector<int> &vectList);

	NSJsonArray(const std::vector<std::string> &vectList);

	// 析构函数
	~NSJsonArray();

public:

	// 获取Json::Value对象
	Json::Value	_GetValue();

	// 设置Json::Value
	void		_SetValue(Json::Value Value);

	__declspec(property (get = _GetValue, put = _SetValue)) Json::Value Value;

public:

	// 是否为空对象
	bool				IsNull();

	// 数组个数
	unsigned int		Size();

	// 获取NSJsonObject
	NSJsonObject		Get(unsigned int unIndex);

	std::string			GetString(unsigned int unIndex);

	int					GetInt(unsigned int unIndex);

	bool				GetBool(unsigned int unIndex);

	// 添加
	void				Add(NSJsonObject JsonObj);

	void				AddArray(NSJsonArray JsonObj);

	void				AddInt(int nValue);

	void				AddString(std::string strValue);

	// 移除
	void				Remove(unsigned int unIndex);

	// 清空
	void				Clear();

	// 转换成NSJsonObject
	NSJsonObject		ToObject();

	// 获取Json格式字符串
	std::string			ToString(bool bStyled = true);

private:

	Json::Value		m_Obj;

};


class NSJson
{
public:

	// 构造函数
	NSJson();

	// 析构函数
	~NSJson();

public:

	// 解析Json格式字符串
	static NSJsonObject ParseStr(std::string strJson);

	// 读取文件，解析json
	static NSJsonObject ParaseFile(std::string strFile);

	// 保存到文件
	static bool SaveToFile(std::string strFile, NSJsonObject JsonObj);
};

#endif