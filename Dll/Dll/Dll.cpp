// Dll.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include <string>
#include <vector>

using namespace std;

#define DLL_API __declspec(dllexport)

extern "C"
{
	typedef char *(*Func)(char *);	

	DLL_API int isDll(char *_pchDll,char *_pchFunc)
	{
		HMODULE hDll = LoadLibrary(_pchDll);
		if (hDll == NULL) { return -1; }
		Func f  = (Func)GetProcAddress(hDll,_pchFunc);
		FreeLibrary(hDll);
		if (f == NULL) return -2;				
		return 1;
	}
	
	DLL_API char* run(char *_pchDll,char *_pchFunc, char *_pchArg)
	{	
		HMODULE hDll = LoadLibrary(_pchDll);		
		Func f = (Func)GetProcAddress(hDll, _pchFunc);						
		static string str;
		str.clear();
		str.append(f(_pchArg));
		FreeLibrary(hDll);		
		return (char*)str.c_str();
	}
}
