// Capture.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#define DLL_API __declspec(dllexport)

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

Mat HdcToMat(HDC _hdc, int _nWidth, int _nHeight)
{
	BITMAPINFOHEADER  bi;
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = _nWidth;
	bi.biHeight = -_nHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	HDC hWindowCompatibleDC = CreateCompatibleDC(_hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(_hdc, _nWidth, _nHeight);
	SelectObject(hWindowCompatibleDC, hBitmap);
	BitBlt(hWindowCompatibleDC, 0, 0, _nWidth, _nHeight, _hdc, 0, 0, SRCCOPY);

	Mat matOut;
	matOut.create(_nHeight, _nWidth, CV_8UC4);
	GetDIBits(hWindowCompatibleDC, hBitmap, 0, _nHeight, matOut.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
	return matOut;
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

void Sharpen(const Mat &image, Mat &result)
{
	/*
	Mat kernel(3, 3, CV_32F, cv::Scalar(0));
	// 커널 생성(모든 값을 0으로 초기화)
	kernel.at<float>(1, 1) = 5.0; // 커널 값에 할당
	kernel.at<float>(0, 1) = -1.0;
	kernel.at<float>(2, 1) = -1.0;
	kernel.at<float>(1, 0) = -1.0;
	kernel.at<float>(1, 2) = -1.0;
	*/
	Mat kernel(3, 3, CV_32F, cv::Scalar(-1));
	kernel.at<float>(1, 1) = 9.0; // 커널 값에 할당
	filter2D(image, result, image.depth(), kernel);
}

bool IsNumber(string str)
{
	if (str.empty()) return false;
	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == '.') continue;
		if (!(str[i] >= '0' && str[i] <= '9')) return false;
	}
	return true;
}

void ImageProcessing(Mat large, vector< vector<Point> > &contours, vector<Vec4i> &hierarchy)
{
	Mat small;
	cvtColor(large, small, CV_BGR2GRAY);
	Mat grad;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
	small.release();
	Mat bw;
	threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	grad.release();
	morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
	Mat connected;
	morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
	morphKernel.release();
	bw.release();
	findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	connected.release();
}

extern "C"
{		
	DLL_API char *capture(char *_pfilename)
	{
		Mat mat = HdcToMat(CreateDC("DISPLAY", NULL, NULL, NULL), GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		if (strcmp(_pfilename, "") == 0)
			imwrite(".\\default.bmp", mat);
		else
			imwrite(_pfilename, mat);
		mat.release();
		return "Capture.dll capture 완료";
	}	
	
	DLL_API char *cut(char *pstr)
	{		
		vector<string> vecStrList = split(pstr, ' ');
		if (vecStrList.size() < 5) return "오류 : 인자 개수 5개 이상 (file1,x1,y1,x2,y2,scale=1.0,file2)";

		int x = stoi(vecStrList[1]);
		int y = stoi(vecStrList[2]);
		int w = stoi(vecStrList[3]) - x;
		int h = stoi(vecStrList[4]) - y;

		if (x < 0)
			return "오류 : x < 0";

		if (y < 0)
			return "오류 : y < 0";

		if (w <= 0)
			return "오류 : x1 >= x2";

		if (h <= 0)
			return "오류 : y1 >= y2";

		Rect rect(x, y, w, h);

		Mat matCrop;
		Mat matOri = imread(vecStrList[0]);

		if (!matOri.data) return "오류 : 데이타가 없다";

		if (matOri.size().width < x) { matOri.release(); return "오류 : x1 원본 이미지의 범위를 넘었습니다."; }
		if (matOri.size().width < x + w) { matOri.release(); return "오류 : x2 원본 이미지의 범위를 넘었습니다."; }

		if (matOri.size().height < y) { matOri.release(); return "오류 : y1 원본 이미지의 범위를 넘었습니다."; }
		if (matOri.size().height < y + h) { matOri.release(); return "오류 : y2 원본 이미지의 범위를 넘었습니다."; }

		matOri(rect).copyTo(matCrop);

		string strWrite;
		for (int i = 5; i < (int)vecStrList.size(); i++)
		{
			if (IsNumber(vecStrList[i].c_str()))
			{
				double dScale = atof(vecStrList[i].c_str());
				resize(matCrop, matCrop, Size(), dScale, dScale);
			}			
			else
				strWrite = vecStrList[i];
		}		
		
		if (strWrite.empty())
		{
			strWrite = ".\\";
			for (int i = 1; i < 5; i++)
				strWrite += vecStrList[i];
			strWrite += ".bmp";
		}		
		imwrite(strWrite, matCrop);
		matOri.release();
		matCrop.release();
		return "Capture.dll cut 완료";
	}

	string g_str;

	DLL_API char *analysis(char *pstr)
	{
		double dScale = 1.0;
		string strFilename = ".\\default.bmp";
		vector<string> vecStrList = split(pstr,' ');

		for (int i = 0; i < (int)vecStrList.size(); i++)
		{
			if (IsNumber(vecStrList[i].c_str()))
				dScale = atof(vecStrList[i].c_str());
			else
				strFilename = vecStrList[i];
		}

		Mat large = imread(strFilename);		
		
		if (!large.data)
		{			
			return "오류 : 파일을 열수 없다";
		}
		resize(large, large, Size(), dScale, dScale);

		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		ImageProcessing(large, contours, hierarchy);

		Mat mask = Mat::zeros(large.size(), CV_8UC1);
		g_str.clear();
		for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
		{
			Rect rect = boundingRect(contours[idx]);
			Mat maskROI(mask, rect);
			maskROI = Scalar(0, 0, 0);
			drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
			double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

			if (r > .45 && (rect.height > 8 && rect.width > 8))
			{
				char buf[32];
				sprintf_s(buf, "%d,%d,%d,%d;", (int)((double)rect.x / dScale - 1.0),
					(int)((double)rect.y / dScale - 1.0),
					(int)((double)(rect.x + rect.width) / dScale + 1.0),
					(int)((double)(rect.y + rect.height) / dScale + 1.0));
				g_str.append(buf);
			}
		}
		return (char*)g_str.c_str();
	}

	DLL_API char *comp(char *pstr)
	{
		g_str.empty();
		vector<string> vecStrList = split(pstr, ' ');
		if (vecStrList.size() != 2) return "오류 : 인자 개수 2개 (file1,file2)";
		Mat image[2];
		for (int i = 0; i < (int)vecStrList.size(); i++)
		{
			image[i] = imread(vecStrList[i].c_str());
			if (!image[i].data)
			{				
				g_str.append("오류 : ");				
				g_str.append(vecStrList[i]);
				g_str.append(" 파일을 열수 없습니다.");
				return (char*)g_str.c_str();
			}
		}

		if (image[0].size() != image[1].size())
			return "파일 사이즈가 다릅니다.";

		if (image[0].depth() != image[1].depth())
			return "파일 구조가 다릅니다.";		
		size_t size = image[0].size().width * image[0].size().height * image[0].elemSize();

		if (memcmp(image[0].data, image[1].data, size) != 0)
			g_str.append("데이타가 다릅니다.");
		else
			g_str.append("같습니다.");
		/*
		int w = image[0].size().width;
		int esize = (int)image[0].elemSize();
		for (int y = 0; y < image[0].size().height; y++)
		{
			for (int x = 0; x < w; x++)
			{
				int pos = (y*w + x)*esize;
				if (memcmp(&image[0].data[pos], &image[1].data[pos], image[0].elemSize()) != 0)
				{
					g_str.append("x = ");
					g_str.append();
					g_str.append("y = ");
					for (int i = 0; i < 2; i++)
						image[i].release();
					return (char*)g_str.c_str();
				}
			}
		}
		*/

		for (int i = 0; i < 2; i++)
			image[i].release();

		return (char*)g_str.c_str();
	}
}


