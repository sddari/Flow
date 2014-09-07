// Ocr.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"

#define DLL_API __declspec(dllexport)

#include "baseapi.h"
#include "basedir.h"
#include "strngs.h"

#include <string>
#include <vector>

#pragma comment (lib, "libtesseract302.lib")

using namespace std;
using namespace tesseract;

extern "C"
{
	DLL_API char *ocr(char *argv)
	{
		TessBaseAPI myOCR;
		string str(argv);		
		string delimiter = " ";

		size_t pos = 0;
		string token;
		vector <string> vecArgv;
		while ((pos = str.find(delimiter)) != std::string::npos) 
		{
			token = str.substr(0, pos);
			vecArgv.push_back(token);			
			str.erase(0, pos + delimiter.length());
		}

		vecArgv.push_back(str);
		if (vecArgv.size() != 2)
		{		
			return "오류 : 인자값 : 언어(eng or kor) 파일명";
		}

		if (myOCR.Init(NULL, vecArgv[0].c_str()))
		{				
			return "오류 : Init";
		}

		static string strMsg;
		STRING text_out;
		if (!myOCR.ProcessPages(vecArgv[1].c_str(), NULL, 0, &text_out))
		{			
			return "오류 : ProcessPages";
		}		

		strMsg = text_out.string();		

		int nLen = MultiByteToWideChar(CP_UTF8, 0, strMsg.c_str(), strMsg.size(), NULL, NULL);
		wstring strUni(nLen,0);
		MultiByteToWideChar(CP_UTF8, 0, strMsg.c_str(), strMsg.size(), &strUni[0], nLen);		
		
		strMsg.clear();
		nLen = WideCharToMultiByte(CP_ACP, 0, &strUni[0], -1, NULL, 0, NULL, NULL);			
		strMsg.resize(nLen,0);
		WideCharToMultiByte(CP_ACP, 0, &strUni[0], -1, &strMsg[0], nLen, NULL, NULL);			

		myOCR.End();
		return (char *)strMsg.c_str();
	}
}
