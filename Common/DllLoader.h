#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <functional>
namespace WisdomUtils
{
	class CDllLoader
	{
	public:	
		~CDllLoader(void)
		{
			if (hDll)
			{
				FreeLibrary(hDll);
				hDll = NULL;
			}
		};

		CDllLoader(char* pDllPath) : hDll(NULL)
		{
			hDll = LoadLibraryA(pDllPath);
		}
		CDllLoader(TCHAR* pDllPath) : hDll(NULL)
		{
			hDll = LoadLibrary(pDllPath);
		}

		BOOL LoadResult()
		{
			if (hDll == NULL)
			{
				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
		//this->GetFunction<void(int, int)>("TestDll");
		template<typename T>
		std::function<T> GetFunction(std::string strFunctionName)
		{
			if (hDll == NULL)
			{
				return NULL;
			}

			auto iter = mapFunctions.find(strFunctionName);
			if (iter == mapFunctions.end())
			{
				mapFunctions[strFunctionName] = GetProcAddress(hDll, strFunctionName.c_str());
				if (mapFunctions[strFunctionName] == NULL)
				{
					return NULL;
				}
			}
			return std::function<T>((T*)mapFunctions[strFunctionName]);
		}

	private:
		HMODULE hDll;
		std::map<std::string, FARPROC> mapFunctions;
	};
}
