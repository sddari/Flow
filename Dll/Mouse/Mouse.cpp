// Mouse.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"

#define DLL_API __declspec(dllexport)
#include <string>
#include <vector>
HHOOK g_hms = NULL;
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
	//% 02d: % 02d : % 02d",t.tm_hour, t.tm_min, t.tm_sec);

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

string g_out, g_str;

void MouseClick(int x, int y)
{
	SetCursorPos(x, y);
	INPUT input;
	memset(&input, 0, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;	
	SendInput(1, &input, sizeof(INPUT));
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

vector <string> split(const string& s, char seperator)
{
	vector<string> output;
	string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != string::npos)
	{
		string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos));
	return output;
}

extern "C"
{
	LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
		if (pMouseStruct != NULL)
		{			
			g_str.clear();
			switch (wParam)
			{
			case WM_MOUSEMOVE:
				g_str = "이동";
				break;				
			case WM_LBUTTONDOWN:
				g_str = "왼쪽_다운";
				break;
			case WM_LBUTTONUP:
				g_str = "왼쪽_업";
				break;
			case WM_LBUTTONDBLCLK:
				g_str = "왼쪽_더블";
				break;
			case WM_RBUTTONDOWN:
				g_str = "오른쪽_다운";
				break;
			case WM_RBUTTONUP:
				g_str = "오른쪽_업";
				break;
			case WM_RBUTTONDBLCLK:
				g_str = "오른쪽_더블";
				break;
			case WM_MBUTTONDOWN:
				g_str = "가운데_다운";
				break;
			case WM_MBUTTONUP:
				g_str = "가운데_업";
				break;
			case WM_MBUTTONDBLCLK:
				g_str = "가운데_더블";
				break;
			case WM_MOUSEWHEEL:
				g_str = "휠";
				break;
			}
			if (g_str.empty()==false)
			{
				g_str += ";";
				g_str += to_string(pMouseStruct->pt.x);
				g_str += ";";
				g_str += to_string(pMouseStruct->pt.y);				
				g_str += ";";
			}			
		}
		return CallNextHookEx(g_hms, nCode, wParam, lParam);
	}
	int g_quit = false;

	DWORD WINAPI MouseLoop(LPVOID lpParam)
	{
		while (g_quit)
		{
			MSG msg;
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return 0;
	}


	DLL_API char *install(char *)
	{			
		if (g_quit) return "오류: 이미 실행 중 입니다";
		g_quit = true;
		g_hms = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, GetModuleHandle(NULL), NULL);
		while (g_quit)
		{
			MSG msg;
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}				
		return "Mouse.dll Install 완료";
	}

	DLL_API char *uninstall(char *)
	{
		UnhookWindowsHookEx(g_hms);
		g_quit = false;	
		return "Mouse.dll Uninstall 완료";
	}

	DLL_API char *get(char *)
	{		
		g_out = g_str;
		g_str.clear();		
		return (char*)g_out.c_str();
	}
	
	DLL_API char *input(char *argv)
	{
		vector<string> strList = split(string(argv), ' ');		
		
		if (strList.size() != 3)
		{
			return "오류 : 인자값(명령,x축,y축) 명령[이동,클릭,더블]";
		}
		
		{
			if (strList[0] == "이동")
				SetCursorPos(atoi(strList[1].c_str()), atoi(strList[2].c_str()));

			if (strList[0] == "클릭")
				MouseClick(atoi(strList[1].c_str()), atoi(strList[2].c_str()));

			if (strList[0] == "더블")
			{
				MouseClick(atoi(strList[1].c_str()), atoi(strList[2].c_str()));
				MouseClick(atoi(strList[1].c_str()), atoi(strList[2].c_str()));
			}
		}
		return "Mouse.dll input 완료";
	}
}
