// Keyboard.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#define DLL_API __declspec(dllexport)

#include <string>
#include <vector>
HHOOK g_hkb = NULL;
using namespace std;

#include <stdio.h>
#include <time.h>
void log(const char *_pchFmt, ...)
{
	time_t now;	
	time(&now);	
	struct tm t = *localtime(&now);
	char achFilename[_MAX_PATH];
	sprintf(achFilename, ".\\log\\%d-%02d-%02d.txt", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);	

	FILE *pFile = fopen(achFilename, "a+");
	fprintf(pFile, "[%02d:%02d:%02d] ", t.tm_hour, t.tm_min, t.tm_sec);

	char achBuffer[1024];
	va_list ap;
	va_start(ap, _pchFmt);
	vsprintf(achBuffer, _pchFmt, ap);
	va_end(ap);
	fprintf(pFile, achBuffer);
	fprintf(pFile, "\n");
	fclose(pFile);
}


string WParamToString(WPARAM _wParam)
{
	switch (_wParam)
	{
	case VK_LBUTTON: return "LBUTTON";
	case VK_RBUTTON: return "RBUTTON";
	case VK_CANCEL: return "CANCEL";
	case VK_MBUTTON: return "MBUTTON";
	case VK_BACK: return "BACK";
	case VK_TAB: return "TAB";
	case VK_CLEAR: return "CLEAR";
	case VK_RETURN: return "RETURN";
	case VK_SHIFT: return "SHIFT";
	case VK_CONTROL: return "CONTROL";
	case VK_MENU: return "MENU";
	case VK_PAUSE: return "PAUSE";
	case VK_CAPITAL: return "CAPITAL";
	case VK_ESCAPE: return "ESCAPE";
	case VK_SPACE: return "SPACE";
	case VK_PRIOR: return "PRIOR";
	case VK_NEXT: return "NEXT";
	case VK_END: return "END";
	case VK_HOME: return "HOME";
	case VK_LEFT: return "LEFT";
	case VK_UP: return "UP";
	case VK_RIGHT: return "RIGHT";
	case VK_DOWN: return "DOWN";
	case VK_SELECT: return "SELECT";
	case VK_PRINT: return "PRINT";
	case VK_EXECUTE: return "EXECUTE";
	case VK_SNAPSHOT: return "SNAPSHOT";
	case VK_INSERT: return "INSERT";
	case VK_DELETE: return "DELETE";
	case VK_HELP: return "HELP";
	case VK_LWIN: return "LWIN";
	case VK_RWIN: return "RWIN";
	case VK_MULTIPLY: return "MULTIPLY";
	case VK_ADD: return "ADD";
	case VK_SEPARATOR: return "SEPARATOR";
	case VK_SUBTRACT: return "SUBTRACT";
	case VK_DECIMAL: return "DECIMAL";
	case VK_DIVIDE: return "DIVIDE";
	case VK_NUMLOCK: return "NUMLOCK";
	case VK_SCROLL: return "SCROLL";
	case VK_F1: return "F1";
	case VK_F2: return "F2";
	case VK_F3: return "F3";
	case VK_F4: return "F4";
	case VK_F5: return "F5";
	case VK_F6: return "F6";
	case VK_F7: return "F7";
	case VK_F8: return "F8";
	case VK_F9: return "F9";
	case VK_F10: return "F10";
	case VK_F11: return "F11";
	case VK_F12: return "F12";
	case VK_F13: return "F13";
	case VK_F14: return "F14";
	case VK_F15: return "F15";
	case VK_F16: return "F16";
	case VK_F17: return "F17";
	case VK_F18: return "F18";
	case VK_F19: return "F19";
	case VK_F20: return "F20";
	case VK_F21: return "F21";
	case VK_F22: return "F22";
	case VK_F23: return "F23";
	case VK_F24: return "F24";
	case VK_NUMPAD0: return "NUMPAD0";
	case VK_NUMPAD1: return "NUMPAD1";
	case VK_NUMPAD2: return "NUMPAD2";
	case VK_NUMPAD3: return "NUMPAD3";
	case VK_NUMPAD4: return "NUMPAD4";
	case VK_NUMPAD5: return "NUMPAD5";
	case VK_NUMPAD6: return "NUMPAD6";
	case VK_NUMPAD7: return "NUMPAD7";
	case VK_NUMPAD8: return "NUMPAD8";
	case VK_NUMPAD9: return "NUMPAD9";
	case 0x30: return "KEY_0";
	case 0x31: return "KEY_1";
	case 0x32: return "KEY_2";
	case 0x33: return "KEY_3";
	case 0x34: return "KEY_4";
	case 0x35: return "KEY_5";
	case 0x36: return "KEY_6";
	case 0x37: return "KEY_7";
	case 0x38: return "KEY_8";
	case 0x39: return "KEY_9";
	case 0x41: return "KEY_A";
	case 0x42: return "KEY_B";
	case 0x43: return "KEY_C";
	case 0x44: return "KEY_D";
	case 0x45: return "KEY_E";
	case 0x46: return "KEY_F";
	case 0x47: return "KEY_G";
	case 0x48: return "KEY_H";
	case 0x49: return "KEY_I";
	case 0x4a: return "KEY_J";
	case 0x4b: return "KEY_K";
	case 0x4c: return "KEY_L";
	case 0x4d: return "KEY_M";
	case 0x4e: return "KEY_N";
	case 0x4f: return "KEY_O";
	case 0x50: return "KEY_P";
	case 0x51: return "KEY_Q";
	case 0x52: return "KEY_R";
	case 0x53: return "KEY_S";
	case 0x54: return "KEY_T";
	case 0x55: return "KEY_U";
	case 0x56: return "KEY_V";
	case 0x57: return "KEY_W";
	case 0x58: return "KEY_X";
	case 0x59: return "KEY_Y";
	case 0x5a: return "KEY_Z";
	}
	return "";
}

string g_out,g_str;

extern "C"
{		
	LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		// Declare our pointer to the KBDLLHOOKSTRUCT
		KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:	
			break;
		case WM_KEYUP:
			g_str = WParamToString(pKeyBoard->vkCode);			
			break;
		}
		return CallNextHookEx(NULL, nCode, wParam, lParam);		
	}	
	HANDLE g_hThread;

	int g_quit = false;	

	DLL_API char *install(char *)
	{		
		if (g_quit) return "오류 : 이미 실행 중 입니다.";
		g_hkb = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, GetModuleHandle(NULL), 0);	
		g_quit = true;
		while (g_quit)
		{
			MSG msg;
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}				
		return "Keybord.dll install 완료";
	}

	DLL_API char *uninstall(char *)
	{		
		UnhookWindowsHookEx(g_hkb);	
		g_quit = false;		
		return "Keybord.dll uninstall 완료";
	}

	DLL_API char *get(char *)
	{		
		g_out = g_str;
		g_str.clear();			
		return (char*)g_out.c_str();
	}
}



