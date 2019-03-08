// JsonTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../Common/Json/Header/json.h"


int _tmain(int argc, _TCHAR* argv[])
{
	Json::Value json_temp;      //临时对象，供如下代码使用
	json_temp["name"] = Json::Value("huchao");
	json_temp["age"] = Json::Value(26);

	Json::Value root;  //表示整个json对象
	root["key_string"] = Json::Value("value_string");   //新建一个Key（名为：key_string），赋予字符串值："value_string"。 
	root["key_number"] = Json::Value(12345);            //新建一个Key（名为：key_number），赋予数值：12345。 
	root["key_boolean"] = Json::Value(false);           //新建一个Key（名为：key_boolean），赋予bool值：false。
	root["key_double"] = Json::Value(12.345);           //新建一个Key（名为：key_double），赋予double值：12.345。
	root["key_object"] = json_temp;                     //新建一个Key（名为：key_object），赋予json::Value对象值。
	root["key_array"].append("array_string");           //新建一个Key（名为：key_array），类型为数组，对第一个元素赋值为字符串："array_string"。
	root["key_array"].append(1234);                     //为数组key_array赋值，对第二个元素赋值为：1234。
	Json::ValueType type = root.type();                 //获得root的类型，此处为objectValue类型。

	Json::FastWriter fast_writer;
	std::cout << fast_writer.write(root) << std::endl;

	Json::StyledWriter styled_writer;
	std::cout << styled_writer.write(root) << std::endl;

	Json::Reader reader;
	Json::Value json_object;
	const char* json_document = "{\"age\" : 26,\"name\" : \"huchao\",\"key_object\":{\"age\" : 26,\"name\" : \"huchao\"}}";
	if (!reader.parse(json_document, json_object))
		return 0;

	std::cout << json_object["name"] << std::endl;
	std::cout << json_object["age"] << std::endl;
	std::cout << json_object["key_object"]["name"] << std::endl;
	std::cout << json_object["key_object"]["age"] << std::endl;
	std::cout << json_object["key_object"] << std::endl;

	return 0;
}

