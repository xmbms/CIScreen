#ifndef __CI_DISPLAYER_H_
#define __CI_DISPLAYER_H_

#include <Windows.h>
#include <vector>

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

public:
	~CIDisplayer();

private:
	bool registerClass();
	static void instaceThread(void * vptr);
	static CIDisplayer * instance; //TODO: auto memory collection
	vector<int> xPoints;
	vector<int> yPoints;
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