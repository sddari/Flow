// Screen.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"


#define DLL_API __declspec(dllexport)

#include <string>
#include <vector>

using namespace std;

void Debug(string strFile,const char *pch, ...)
{
	FILE *file = fopen(strFile.c_str(), "a+");
	va_list ap;
	va_start(ap, pch);
	vfprintf(file, pch, ap);
	va_end(ap);	
	fclose(file);
}

vector<string> split(const string& s, char seperator)
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
	bool g_quit = false;

	struct SData
	{
		int m_nType;
		int m_apos[8];
		string m_str;
	};
	vector <SData> m_vecDraw;	
	
	enum E_COMMAND
	{
		E_RECT = 0,
		E_TEXT,
		E_CLEAR,
		E_MAX,
	};

	HWND g_hWnd;
	
	DLL_API char *control(char *_argv)
	{
		string strCom[3] = { "rect", "text" , "clear" };
		vector<string> vecStrList = split(string(_argv),' ');
		SData stData;
		for (int i = 0; i < E_MAX; i++)
		{
			if (strCom[i].compare(vecStrList[0]) == 0)
				stData.m_nType = i;
		}			
		if (stData.m_nType == E_CLEAR)
		{
			m_vecDraw.clear();			
			return "ok";
		} else if (stData.m_nType == E_TEXT)
		{
			stData.m_apos[0] = atoi(vecStrList[1].c_str());
			stData.m_apos[1] = atoi(vecStrList[2].c_str());
			string str;
			for (unsigned int i = 3; i < vecStrList.size(); i++)
				str += vecStrList[i] + " ";
			stData.m_str = str;
		}
		else
		{
			for (int i = 1; i < (int)vecStrList.size(); i++)
				stData.m_apos[i - 1] = atoi(vecStrList[i].c_str());
		}

		m_vecDraw.push_back(stData);
		return "Screen.dll control 완료";
	}

	DLL_API char *finish(char *pch)
	{		
		g_quit = false;
		m_vecDraw.clear();				
		return "Screen.dll finish 완료";
	}

	DLL_API char *loop(char *pch)
	{			
		if (g_quit) return "오류: 이미 실행 중 입니다";
		m_vecDraw.clear();
		g_quit = true;
		g_hWnd = GetDesktopWindow();
		while (g_quit)
		{			
			HDC hdc = GetWindowDC(g_hWnd);
			for (int i = 0; i < (int)m_vecDraw.size(); i++)
			{
				SData data = m_vecDraw.at(i);
				switch (data.m_nType)
				{
				case E_RECT:
					Rectangle(hdc, data.m_apos[0], data.m_apos[1],
						data.m_apos[2], data.m_apos[3]);
					break;
				case E_TEXT:
					TextOut(hdc, data.m_apos[0], data.m_apos[1],
						data.m_str.c_str(), data.m_str.length());
					break;
				}							
			}				
			ReleaseDC(g_hWnd, hdc);
		}				
		return "Screen.dll loop 완료";
	}

	DLL_API char *drawcross(char *_argv)
	{
		HWND hWnd = GetDesktopWindow();
		HDC hdc = GetWindowDC(hWnd);
		vector<string> vecStrList = split(string(_argv), ' ');
		int s = 1;
		if (vecStrList.size() < 2) 
			return "오류 : 인자값은 2개 이상 (x,y,size=1)";
		int x = atoi(vecStrList[0].c_str());
		int y = atoi(vecStrList[1].c_str());
		if (vecStrList.size() > 2)
			s = atoi(vecStrList[2].c_str());

		MoveToEx(hdc,x,y-s,NULL);
		LineTo(hdc,x,y+s);
		MoveToEx(hdc, x - s, y, NULL);
		LineTo(hdc, x+s, y);
		ReleaseDC(hWnd, hdc);
		return "Screen.dll drawCross 완료";
	}

	DLL_API char *clear(char *_argv)
	{	
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = GetSystemMetrics(SM_CXSCREEN);
		rc.bottom = GetSystemMetrics(SM_CYSCREEN);		
		InvalidateRect(NULL, &rc, TRUE);
		return "Screen.dll clear 완료";
	}

	DLL_API char *drawrect(char *_argv)
	{
		HWND hWnd = GetDesktopWindow();
		HDC hdc = GetWindowDC(hWnd);
		vector<string> vecStrList = split(string(_argv), ' ');		
		if (vecStrList.size() != 4)
			return "오류 : 인자값은 4개 (x1,y1,x2,y2)";
		HGDIOBJ bObj = SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, atoi(vecStrList[0].c_str()) ,
			atoi(vecStrList[1].c_str()),
			atoi(vecStrList[2].c_str()),
			atoi(vecStrList[3].c_str()));
		SelectObject(hdc, bObj);
		ReleaseDC(hWnd, hdc);
		return "Screen.dll drawrect 완료";
	}

	DLL_API char *drawtext(char *_argv)
	{
		HWND hWnd = GetDesktopWindow();
		HDC hdc = GetWindowDC(hWnd);
		vector<string> vecStrList = split(string(_argv), ' ');
		if (vecStrList.size() != 3)
			return "오류 : 인자값은 3개 (x,y,string)";
		TextOut(hdc, atoi(vecStrList[0].c_str()),
			atoi(vecStrList[1].c_str()),
			vecStrList[2].c_str(), vecStrList[2].size());
		ReleaseDC(hWnd, hdc);
		return "Screen.dll drawtext 완료";
	}
}
