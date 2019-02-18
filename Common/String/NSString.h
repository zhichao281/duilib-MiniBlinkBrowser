// +----------------------------------------------------------------------
// | Green C++ Basis Components
// +----------------------------------------------------------------------
// | std::wstring	宽字字符串类
// | std::string 	多字节字符串类 
// | NSNStr		字符串常用操作命名空间
// +----------------------------------------------------------------------
// +----------------------------------------------------------------------

#pragma  once

#include <string>
#include <vector>
#include <windows.h>



#ifdef _UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif



namespace NStr
{
	// 多字节（std::string ）转成宽字（std::wstring）
	std::wstring	StrToWStr(std::string  strBuf, int nCode = CP_ACP);

	// 宽字（std::wstring）转成多字节（std::string ）
	std::string 	WStrToStr(std::wstring wstrBuf, int nCode = CP_ACP);

	// 编码转换 ANSI => UTF8
	std::string 	GetUtf8(std::string  strBuf);

	// 编码转换 UTF8 => ANSI
	std::string 	GetAnsi(std::string  strBuf);

	_tstring ANSI2T(LPCSTR lpSrc);

	std::string T2ANSI(LPCTSTR lpSrc);

	std::string T2UTF8(LPCTSTR lpSrc);

	std::wstring ANSI2Unicode(LPCSTR lpszSrc);

	std::string Unicode2UTF8(LPCWSTR lpszSrc);

	std::string Unicode2ANSI(LPCWSTR lpszSrc);

	std::wstring utf8ToUtf16(const std::string& utf8String);

	std::string utf16ToUtf8(LPCWSTR lpszSrc);
	// 字符串转成int
	int			StrToInt(std::string  strBuf);
	int			StrToInt(std::wstring strBuf);

	// int转字符串
	std::string 	IntToStr(int nValue);
	std::wstring	IntToWStr(int nValue);

	// 字符串转成float
	float		StrToFloat(std::string  strBuf);
	float		StrToFloat(std::wstring strBuf);

	// float转成字符串
	std::string 	FloatToStr(float fValue);
	std::wstring	FloatToWStr(float fValue);

	// 字符串转成double
	double		StrToDouble(std::string  strBuf);
	double		StrToDouble(std::wstring strBuf);

	// double转字符串
	std::string 	DoubleToStr(double dbValue);
	std::wstring	DoubleToWStr(double dbValue);

	std::string 	DoubleToStr(double dbValue, int nDigit);
	std::wstring	DoubleToWStr(double dbValue, int nDigit);

	// 字符串转long
	long		StringToLong(std::string  strBuf);
	long		StringToLong(std::wstring strBuf);

	// long转字符串
	std::string 	LongToStr(long lValue);
	std::wstring	LongToWStr(long lValue);

	// 字符串转__int64
	__int64		StrToInt64(std::string  strBuf);
	__int64		StrToInt64(std::wstring strBuf);

	// __int64转字符串
	std::string 	Int64ToStr(__int64 nValue);
	std::wstring	Int64ToWStr(__int64 nValue);

	// 字符串分割(按字符c分割)
	int			SpliteStrByChar(std::string  strBuf, std::vector<std::string > &vecList, char c = '|', bool bSkipEmpty = false);
	int			SpliteStrByChar(std::wstring strBuf, std::vector<std::wstring> &vecList, char c = '|', bool bSkipEmpty = false);

	// 字符串分割(以在strChars中出现的字符分割)
	int			SpliteStrByChars(std::string  strBuf, std::vector<std::string > &vecList, std::string  strChars = "|", bool bSkipEmpty = false);
	int			SpliteStrByChars(std::wstring strBuf, std::vector<std::wstring> &vecList, std::wstring strChars = L"|", bool bSkipEmpty = false);

	// 字符串分割（按字符串分割）
	int			SpliteStrByStr(std::string  strBuf, std::vector<std::string > &vecList, std::string  strSplite = "|", bool bSkipEmpty = false);
	int			SpliteStrByStr(std::wstring strBuf, std::vector<std::wstring> &vecList, std::wstring strSplite = L"|", bool bSkipEmpty = false);

	// 多个字符串分割（以\0结束符分割）
	int			MutilStrToArrays(char *pData, std::vector<std::string > &vecList);
	int			MutilStrToArrays(wchar_t *pData, std::vector<std::wstring> &vecList);

	// 删除字符串中指定的字符(在strChars中的都删除)
	int			DelStrByChars(std::string  &strBuf, std::string  strDel);
	int			DelStrByChars(std::wstring &strBuf, std::wstring strDel);

	// 删除字符串中指定的字符串
	int			DelStrByStr(std::string  &strBuf, std::string  strDel);
	int			DelStrByStr(std::wstring &strBuf, std::wstring strDel);

	// 替换字符串中指定的字符串
	int			ReplaceStr(std::string  &strBuf, std::string  strSrc, std::string  strDes);
	int			ReplaceStr(std::wstring &strBuf, std::wstring strSrc, std::wstring strDes);

	// 字符串头部比较
	int			StrHeadCmp(std::string  strBuf, std::string  strHead, bool bNoCase = true);
	int			StrHeadCmp(std::wstring strBuf, std::wstring strHead, bool bNoCase = true);

	// 字符串尾部比较
	int			StrTailCmp(std::string  strBuf, std::string  strTail, bool bNoCase = true);
	int			StrTailCmp(std::wstring strBuf, std::wstring strTail, bool bNoCase = true);

	// 去除字符串两边出现的指定字符(在strChars中出现的都删除)
	int			StrTrim(std::string  &strBuf, std::string  strChars = " ");
	int			StrTrim(std::wstring &strBuf, std::wstring strChars = L" ");

	// 去除字符串左边出现的指定字符(在strChars中出现的都删除)
	int			StrTrimLeft(std::string  &strBuf, std::string  strChars = " ");
	int			StrTrimLeft(std::wstring &strBuf, std::wstring strChars = L" ");

	// 去除字符串右边出现的指定字符(在strChars中出现的都删除)
	int			StrTrimRight(std::string  &strBuf, std::string  strChars = " ");
	int			StrTrimRight(std::wstring &strBuf, std::wstring strChars = L" ");

	// 创建随机字符串
	std::string 	CreateRandomStr(int nSize, std::string  strSeed = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	std::wstring	CreateRandomWStr(int nSize, std::wstring strSeed = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	// 格式化字符串
	std::string 	FormatStr(const char* pFmt, ...);
	std::wstring	FormatWStr(const wchar_t* pFmt, ...);

	// 字符串转小写
	std::string 	StrToLwr(std::string  strBuf);
	std::wstring	StrToLwr(std::wstring strBuf);

	// 字符串转大写
	std::string 	StrToUpr(std::string  strBuf);
	std::wstring	StrToUpr(std::wstring strBuf);

	// 判断字符是否是数字
	bool		IsNumber(char c);

	// 判断字符是否是大写字母
	bool		IsAlphUpr(char c);

	// 判断字符是否是小写字母
	bool		IsAplhLwr(char c);

	// 判断字符是否是字母
	bool		IsAplh(char c);

	// 判断字符是否是操作符
	bool		IsOperate(char c);

	// 判断字符串是否包含中文
	bool		HasChinese(std::string  strBuf);

	// 获取GUID
	std::string 	GetGUIDStr();

	// Base64加密
	std::string 	Base64Encode(std::string  strSrc);

	// Base64解密
	std::string 	Base64Decode(std::string  strSrc);

	// UrlEncode
	std::string 	UrlEncode(std::string  strBuf);

	// UrlDecode
	std::string 	UrlDecode(std::string  strBuf);


#ifdef GS_USE_CRYPT
	// Sha1加密
	std::string 	GetSha1(std::string  strBuf);

	// MD5加密
	std::string 	GetMD5(std::string  strBuf);

	// Aes加密(返回的是内存数据)
	std::string 	Aes128Encode(std::string  strPwd, std::string  strBuf);

	// Aes解密
	std::string 	Aes128Decode(std::string  strPwd, std::string  strBuf);

	// Rsa公钥加密(返回的是内存数据)RSA_PKCS1_PADDING
	std::string 	RsaPubEncode(std::string  strKey, std::string  strBuf);

	// Rsa公钥解密
	std::string 	RsaPubDecode(std::string  strKey, std::string  strBuf);

	// Rsa私钥加密（返回的是内存数据）RSA_PKCS1_PADDING
	std::string 	RsaPriEncode(std::string  strKey, std::string  strBuf);

	// Rsa私钥解密
	std::string 	RsaPriDecode(std::string  strKey, std::string  strBuf);

	// 获取Rsa公钥、私钥对
	bool		GetRsaKey(std::string  &strPubKey, std::string  &strPriKey);


	// Des加密 PKCS7
	std::string 	DesEncode(std::string  strKey, std::string  strBuf, bool bEcb = true);

	// Des解密 PKCS7
	std::string 	DesDecode(std::string  strKey, std::string  strBuf, bool bEcb = true);

	// 3Des加密 PKCS7 ECB
	std::string 	Des3Encode(std::string  strKey, std::string  strBuf);

	// 3Des解密 PKCS7 ECB
	std::string 	Des3Decode(std::string  strKey, std::string  strBuf);
	
#endif

	//////////////////////////////////////////////////////////////////
	//	检测字串是否包含汉字、数字、字符	add by cherish 2016-08-20
	BOOL		CheckUserPass(const char * iName);

	// 检测是否包含汉字
	BOOL		CheckHasChina(std::string  strData);

	// 中文转拼音
	std::string 	ChineseToPinyin(std::string  strData);

	// 文件大小转换为字符串
	std::string 	FileSizeToStr(__int64 ulSize);

	// 文件大小转换为字符串
	std::wstring	FileSizeToWStr(__int64 ulSize);

	// 数字转字符（用以大写转换1）
	std::string 	IntToStrForCapital(int nValue);

	// 数字转字符（用以大写转换1）
	std::wstring	IntToWStrForCapital(int nValue);

	// 版本比较（返回是否有新版本） a.b.c
	bool		VersionCompare(std::string  strOldVer, std::string  strNewVer, std::string  strSplite = ".");
}



