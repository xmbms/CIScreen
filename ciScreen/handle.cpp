#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>

#include <Windows.h>


#include "handle.h"
#include "debug.h"
#include "common.h"

CIHandle::CIHandle(){
	dataGenerator = NULL;
	mouseState = 0;
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
	//pull
	console.info("Pull");
}

void XN_CALLBACK_TYPE CIHandle::onWave(void* cxt){
	//wave hand
	console.info("Wave");
}

void XN_CALLBACK_TYPE CIHandle::onSessionStart(const XnPoint3D& ptFocusPoint, void* UserCxt){
	//on session start
	console.warn("Session Start");
}

void XN_CALLBACK_TYPE CIHandle::onSessionEnd(void* UserCxt){
	//on session end
	console.warn("Session End");
}

void XN_CALLBACK_TYPE CIHandle::onSessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt){
	//on session progress
	console.info("Session Progress");
}

void XN_CALLBACK_TYPE CIHandle::onSteady(XnUInt32 nId, XnFloat fVelocity, void* cxt){
	//steady
	console.info("Steady");
}

void XN_CALLBACK_TYPE CIHandle::onSwipeUp(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	console.info("Swipe Up");
}

void XN_CALLBACK_TYPE CIHandle::onSwipeDown(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	console.info("Swipe Down");
}

void XN_CALLBACK_TYPE CIHandle::onSwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	console.info("Swipe Left");
}

void XN_CALLBACK_TYPE CIHandle::onSwipeRight(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	console.info("Swipe Right");
}

void XN_CALLBACK_TYPE CIHandle::onNoHands(void* cxt){
	console.warn("No hands");
}

void XN_CALLBACK_TYPE CIHandle::onDragStart(int count , void * cxt, CvPoint center){
	SetCursorPos(center.x, center.y);	
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
}

void XN_CALLBACK_TYPE CIHandle::onDrag(int count , void * cxt, CvPoint center){
	SetCursorPos(center.x, center.y);	
}

void XN_CALLBACK_TYPE CIHandle::onDragEnd(int count , void * cxt, CvPoint center){
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void XN_CALLBACK_TYPE CIHandle::onDraw(int count , void * cxt, CvPoint center){
	
}


void XN_CALLBACK_TYPE CIHandle::onDrawEnd(int count , void * cxt, CvPoint center){
}

void XN_CALLBACK_TYPE CIHandle::onZoom(int count , void * cxt, CvPoint center){
	
}

void XN_CALLBACK_TYPE CIHandle::onZoomEnd(int count , void * cxt, CvPoint center){
	
}

void XN_CALLBACK_TYPE CIHandle::onMove(int count , void * cxt, CvPoint center){
	//console.info("Hand Move");
	SetCursorPos(center.x, center.y);
}