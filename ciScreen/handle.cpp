#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>

#include <Windows.h>


#include "handle.h"
#include "debug.h"
#include "common.h"

CIHandle::CIHandle(){
	dataGenerator = NULL;
	mouseState = 0;
	displayer = CIDisplayer::getInstance();
	displayer->show();
	dragWnd = NULL;
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
}

void XN_CALLBACK_TYPE CIHandle::onPull(void* cxt){
	//pull5
}

void XN_CALLBACK_TYPE CIHandle::onWave(void* cxt){
	//wave hand
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
	console.info("Swipe Up");
}

void XN_CALLBACK_TYPE CIHandle::onSwipeDown(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	console.info("Swipe Down");
}

void XN_CALLBACK_TYPE CIHandle::onSwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->clearScreen();
	console.info("Swipe");
}

void XN_CALLBACK_TYPE CIHandle::onSwipeRight(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->clearScreen();
	console.info("Swpie");
}

void XN_CALLBACK_TYPE CIHandle::onNoHands(void* cxt){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->textOut("Wave hand to be detected");
}

void XN_CALLBACK_TYPE CIHandle::onDragStart(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->setCursorPos(center.x, center.y);	
	pHandler->dragPt.x = center.x;
	pHandler->dragPt.y = center.y;
	pHandler->dragWnd = WindowFromPoint(pHandler->dragPt);
	::GetWindowRect(pHandler->dragWnd, &pHandler->dragWndRect);
	console.info("DragStart");
}

void XN_CALLBACK_TYPE CIHandle::onDrag(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->setCursorPos(center.x, center.y);	
	if(pHandler->dragWnd){
		::SetWindowPos(pHandler->dragWnd,0,  
			pHandler->dragWndRect.left + center.x - pHandler->dragPt.x,
			pHandler->dragWndRect.top + center.y - pHandler->dragPt.y,
			pHandler->dragWndRect.right - pHandler->dragWndRect.left, 
			pHandler->dragWndRect.bottom - pHandler->dragWndRect.top, 
			SWP_NOSIZE);
	}
	console.info("Drag");
}

void XN_CALLBACK_TYPE CIHandle::onDragEnd(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->dragWnd = NULL;
	console.info("DragEnd");
}

void XN_CALLBACK_TYPE CIHandle::onDraw(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->setCursorPos(center.x, center.y, false);
	pHandler->displayer->lineTo(center.x, center.y);
}


void XN_CALLBACK_TYPE CIHandle::onDrawEnd(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->setCursorPos(center.x, center.y, false);
	pHandler->displayer->clearScreen();
}

void XN_CALLBACK_TYPE CIHandle::onZoom(int count , void * cxt, CvPoint center){
	
}

void XN_CALLBACK_TYPE CIHandle::onZoomEnd(int count , void * cxt, CvPoint center){
	
}

void XN_CALLBACK_TYPE CIHandle::onMove(int count , void * cxt, CvPoint center){
	CIHandle * pHandler = (CIHandle *)cxt;
	pHandler->displayer->setCursorPos(center.x, center.y);
}