/*!
* \file url.h
*封装url的解析
* \author zhengzc
* \date 五月 2018
*
*
*/
#pragma  once

#include <string>

using namespace std;

//Scheme :// hostname[:port] / path / [;parameters][?query]#fragment
//http://www.google.com/adsense/support/bin/request.py?contact=afv_violation&url=null&client=ca%2Dgames%2Dpub%2D9606551472994074&videoid=0
namespace common
{
	class Url
	{
	public:
		Url(const char* url);
		//http
		const string GetScheme() const
		{
			return m_scheme;
		}
		//主机名  www.google.com
		const string GetHost() const
		{
			return m_host;
		}
		//端口号
		const string GetPort() const
		{
			return m_port;
		}
		//由零或多个“/”符号隔开的字符串，一般用来表示主机上的一个目录或文件地址。
		///adsense/support/bin/request.py
		const string GetPath() const
		{
			return m_path;
		}
		//查询，用于给动态网页传递参数，可有多个参数，用“&”符号隔开，每个参数的名和值用“ = ”符号隔开。
		//contact=afv_violation&url=null&client=ca%2Dgames%2Dpub%2D9606551472994074&videoid=0
		const string GetQuery() const
		{
			return m_query;
		}
		//信息片断，用于指定网络资源中的片断。例如一个网页中有多个名词解释，可使用fragment直接定位到某一名词解释。
		const string GetFragment() const
		{
			return m_fragment;
		}
		const string GetUserName() const
		{
			return m_username;
		}
		const string GetPassword() const
		{
			return m_password;
		}
	private:
		bool Init(const char* url);
		bool IsSchemeChar(int c)
		{
			return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
		}
	private:
		string m_scheme;               /* mandatory */
		string m_host;                 /* mandatory */
		string m_port;                 /* optional */
		string m_path;                 /* optional */
		string m_query;                /* optional */
		string m_fragment;             /* optional */
		string m_username;             /* optional */
		string m_password;             /* optional */
	};
}

