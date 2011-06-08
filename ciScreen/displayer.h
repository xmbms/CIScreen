#ifndef __CI_DISPLAYER_H_
#define __CI_DISPLAYER_H_

#include <Windows.h>
#include <vector>
#include <map>
#include <cv.h>
using namespace std;

class CIDisplayer{
public:
	void show();
	void textOut(string str = "");
	bool lineTo(int x, int y);
	static CIDisplayer * getInstance();
	void setCursorPos(int x, int y, bool draw = true);
	void renderScreen();
	void clearScreen();
	void addZoomPoint(int nID, int x, int y);

public:
	~CIDisplayer();

private:
	bool registerClass();
	static void instaceThread(void * vptr);
	static CIDisplayer * instance; //TODO: auto memory collection
	vector<int> xPoints;
	vector<int> yPoints;
	map<int, CvPoint> zoomRect;
	string output;
	CIDisplayer();
	HWND	hWnd;
	HBITMAP m_hmemBmp;
	BYTE*	m_pBits;
	HFONT	hFont;
	HPEN	hPen;
	HPEN	hlPen;
	int     cursorX;
	int     cursorY;
};

#endif