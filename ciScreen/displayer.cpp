#include <Windows.h>
#include <process.h>

#include "displayer.h"

#include <iostream>

using namespace std;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg) { 
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    } 
}


CIDisplayer::CIDisplayer(){
	m_hmemBmp = 0;
	hFont     = CreateFont(80,0,0,0,FW_BOLD,0,0,0,0,0,0,2,0,"SYSTEM_FIXED_FONT");
	hPen	  = CreatePen(PS_SOLID,10,RGB(255, 0, 0));
	hlPen	  = CreatePen(PS_SOLID,10,RGB(255, 255, 0));
	output	  = "";
	cursorX   = 0;
	cursorY   = 0;
}

CIDisplayer::~CIDisplayer(){
	DeleteObject(hFont);
	DeleteObject(hPen);
	DeleteObject(m_hmemBmp);
}

CIDisplayer * CIDisplayer::getInstance(){
	if(!instance){
		instance = new CIDisplayer();
	}
	return instance;
}

bool CIDisplayer::registerClass(){
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0; //new instance
	wcex.hIcon			= LoadIcon(NULL, MAKEINTRESOURCE(IDI_INFORMATION));
	wcex.hCursor		= LoadCursor(NULL, IDC_CROSS);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszClassName  = "CIDisplayer class";
    wcex.lpszMenuName   = "CIDisplayer menu class";
	wcex.hIconSm		= LoadIcon(NULL, MAKEINTRESOURCE(IDI_INFORMATION));;
    return ::RegisterClassEx(&wcex);
}

void CIDisplayer::instaceThread(void * vptr){
	CIDisplayer * pDisplayer = (CIDisplayer *) vptr;
	DWORD defStyle = WS_VISIBLE ;
	DWORD exStyle  = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	DWORD dwWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD dwHeight = GetSystemMetrics(SM_CYSCREEN);
	pDisplayer->hWnd = ::CreateWindowEx(exStyle, "CIDisplayer class", "CIDisplayer", defStyle,
                             0, 0, dwWidth, dwHeight, NULL,
							 NULL, 0, NULL);
	if(!pDisplayer->hWnd){
		cout<<"Error : "<<::GetLastError()<<endl;
		return ;
	}
	
    ShowWindow(pDisplayer->hWnd, SW_SHOW);
	pDisplayer->renderScreen();
    MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){ 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CIDisplayer::show(){
	if(registerClass()){
		_beginthread(CIDisplayer::instaceThread, 0, this);	
	}
}

void CIDisplayer::renderScreen(){
	HDC	hDc = GetWindowDC(hWnd);
	HDC hMemDC = CreateCompatibleDC(hDc);
	HDC hdcScreen=::GetDC(NULL);
	COLORREF	clr;
	
	//client size
	RECT rcWnd;
	GetWindowRect(hWnd, &rcWnd);
	POINT ptWinPos={rcWnd.left,rcWnd.top};
	SIZE sizeWindow={rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top};
	BLENDFUNCTION Blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	POINT ptSrc={0,0};
	BITMAPINFO  bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		= sizeWindow.cx;
	bmi.bmiHeader.biHeight		= sizeWindow.cy;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32;
	bmi.bmiHeader.biCompression	= BI_RGB;
	if(!m_hmemBmp){
		m_hmemBmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&m_pBits, 0, 0);
	}

    DWORD m_dwSize=bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;
    memset(m_pBits,0,m_dwSize);
	SelectObject(hMemDC, m_hmemBmp);


	//text output
	if(output.length()){
		SetBkMode(hMemDC, TRANSPARENT);
		clr = SetTextColor(hMemDC, RGB(255, 0,  0));
		SelectObject(hMemDC, hFont);
		TextOut(hMemDC, 0, 0, output.c_str(), output.length());
		SetTextColor(hMemDC, clr);
	}

	//draw line
	SelectObject(hMemDC, hPen);
	if(xPoints.size()){
		::MoveToEx(hMemDC, xPoints[0], yPoints[0], NULL);
		for(int i = 1, len = xPoints.size(); i < len; i++){
			::LineTo(hMemDC, xPoints[i], yPoints[i]);
		}
	}
	SelectObject(hMemDC, hlPen);
	::Ellipse(hMemDC, cursorX, cursorY, cursorX + 20, cursorY + 20);	
	::UpdateLayeredWindow(hWnd, hdcScreen, &ptWinPos, &sizeWindow, hMemDC, &ptSrc,0, &Blend, 1);
	ReleaseDC(hWnd, hDc);
	ReleaseDC(hWnd, hMemDC);
	::DeleteDC(hMemDC);
	ReleaseDC(NULL, hdcScreen);
}

void CIDisplayer::clearScreen(){
	output = "";
	xPoints.clear();
	yPoints.clear();
	renderScreen();
}

void CIDisplayer::textOut(string str){
	output = str;
	renderScreen();
}

bool CIDisplayer::lineTo(int x, int y){
	xPoints.push_back(x);
	yPoints.push_back(y);
	renderScreen();
	return true;
}


void CIDisplayer::setCursorPos(int x, int y, bool draw){
	cursorX = x;
	cursorY = y;
	if(draw){
		renderScreen();
	}
}

CIDisplayer * CIDisplayer::instance = 0;