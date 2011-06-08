#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>

#include <Windows.h>


#include "handle.h"
#include "debug.h"
#include "common.h"

#undef ENABLE_DRAG

CIHandle::CIHandle(){
	dataGenerator = NULL;
	mouseState = 0;
	displayer = CIDisplayer::getInstance();
	displayer->show();
	dragWnd = NULL;
	drawState = 0;
	dragState = false;
}

CIHandle::~CIHandle(){

}

void CIHandle::setDataGenerator(CIInterface * generator){
	dataGenerator = generator;
}

void CIHandle::onClick(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	//push
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->clearScreen();
}

void XN_CALLBACK_TYPE CIHandle::onPull(void* cxt){
	//pull5
}

void XN_CALLBACK_TYPE CIHandle::onWave(void* cxt){
	//wave hand
	console.warn("wave");  //can't work caused by UpdateLayeredWindow, timeout
}

void CIHandle::onSessionInit(){
	displayer->textOut("Wave Hands now.");
}

void XN_CALLBACK_TYPE CIHandle::onSessionStart(const XnPoint3D& ptFocusPoint, void* UserCxt){
	//on session start
	CIHandle * pHandler = (CIHandle *)UserCxt;
	pHandler->displayer->textOut();
}

void XN_CALLBACK_TYPE CIHandle::onSessionEnd(void* UserCxt){
	//on session end
}

void XN_CALLBACK_TYPE CIHandle::onSessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt){
	//on session progress
}

void XN_CALLBACK_TYPE CIHandle::onSteady(XnUInt32 nId, XnFloat fVelocity, void* cxt){
	//steady
}

void XN_CALLBACK_TYPE CIHandle::onSwipeUp(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	keybd_event(VK_F5, 0, 0, 0);
	keybd_event(VK_F5, 0, KEYEVENTF_KEYUP, 0);
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->clearScreen();
}

void XN_CALLBACK_TYPE CIHandle::onSwipeDown(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	keybd_event(VK_ESCAPE, 0, 0, 0);
	keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
}

void XN_CALLBACK_TYPE CIHandle::onSwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	CIHandle * pHandler = (CIHandle *)cxt;
	//if(pHandler->drawState){
		CIHandle::onDrawEnd(0 , cxt, cvPoint(0, 0));
	//} else {
	//	keybd_event(VK_LEFT, 0, 0, 0);
	//	keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
	//}
}

void XN_CALLBACK_TYPE CIHandle::onSwipeRight(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	CIHandle * pHandler = (CIHandle *)cxt;
	//if(pHandler->drawState){
		CIHandle::onDrawEnd(0 , cxt, cvPoint(0, 0));
	//} else {
	//	keybd_event(VK_RIGHT, 0, 0, 0);
	//	keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
	//}
}

void XN_CALLBACK_TYPE CIHandle::onNoHands(void* cxt){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->textOut("Wave hand to be detected");
}

void XN_CALLBACK_TYPE CIHandle::onDragStart(int count , void * cxt, CvPoint center){
	#ifdef ENABLE_DRAG
		CIHandle * pHandler = (CIHandle *)cxt;
		pHandler->displayer->setCursorPos(center.x, center.y);	
		pHandler->dragPt.x = center.x;
		pHandler->dragPt.y = center.y;
		pHandler->dragWnd = WindowFromPoint(pHandler->dragPt);
		pHandler->dragWnd  = GetParent(pHandler->dragWnd);
		::GetWindowRect(pHandler->dragWnd, &pHandler->dragWndRect);
	#endif
}

void XN_CALLBACK_TYPE CIHandle::onDrag(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	if(!pHandler->dragState){
		keybd_event(VK_LEFT, 0, 0, 0);
		keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
	}
	pHandler->dragState = true;
	#ifdef ENABLE_DRAG
		pHandler->displayer->setCursorPos(center.x, center.y);	
		if(pHandler->dragWnd){
			::SetWindowPos(pHandler->dragWnd,0,  
				pHandler->dragWndRect.left + center.x - pHandler->dragPt.x,
				pHandler->dragWndRect.top + center.y - pHandler->dragPt.y,
				pHandler->dragWndRect.right - pHandler->dragWndRect.left, 
				pHandler->dragWndRect.bottom - pHandler->dragWndRect.top, 
				SWP_NOSIZE);
		}
	#endif
}

void XN_CALLBACK_TYPE CIHandle::onDragEnd(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->dragState = false;
	#ifdef ENABLE_DRAG
		CIHandle * pHandler = (CIHandle *)cxt;
		pHandler->dragWnd = NULL;
	#endif
}

void XN_CALLBACK_TYPE CIHandle::onDraw(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->drawState = 1;
	pHandler->displayer->setCursorPos(center.x, center.y, false);
	pHandler->displayer->lineTo(center.x, center.y);
}


void XN_CALLBACK_TYPE CIHandle::onDrawEnd(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	//pHandler->displayer->setCursorPos(center.x, center.y, false);
	pHandler->displayer->clearScreen();
	pHandler->drawState = 0;
}

void XN_CALLBACK_TYPE CIHandle::onZoom(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->addZoomPoint(count, center.x, center.y);
}

void XN_CALLBACK_TYPE CIHandle::onZoomEnd(int count , void * cxt, CvPoint center){
}

void XN_CALLBACK_TYPE CIHandle::onMove(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->setCursorPos(center.x, center.y, true);
}