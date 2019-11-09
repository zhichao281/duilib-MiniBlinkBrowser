
#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\lib\\DuiLib_d.lib")
#		pragma comment(lib, "..\\lib\\TroyControls_d.lib")
#       pragma comment(lib, "..\\lib\\uWebSockets_d.lib")
#   else
#       pragma comment(lib, "..\\lib\\DuiLibA_d.lib")
#		pragma comment(lib, "..\\lib\\TroyControlsA_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\lib\\DuiLib.lib")
//#       pragma comment(lib, "..\\lib\\TroyControls.lib")
#       pragma comment(lib, "..\\lib\\uWebSockets.lib")
#   else
#       pragma comment(lib, "..\\lib\\DuiLibA.lib")
//#       pragma comment(lib, "..\\lib\\TroyControls.lib")
#   endif
#endif


