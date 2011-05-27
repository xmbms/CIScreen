#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>


#include "handle.h"
#include "debug.h"

CIHandle::CIHandle(){
	dataGenerator = NULL;
}

CIHandle::~CIHandle(){

}

void CIHandle::setDataGenerator(CIInterface * generator){
	dataGenerator = generator;
}

void CIHandle::onClick(XnFloat fVelocity, XnFloat fAngle, void* cxt){
	//push
	console.info("Click");
}

void XN_CALLBACK_TYPE CIHandle::onPull(void* cxt){
	//pull
	console.info("Pull");
}

void XN_CALLBACK_TYPE CIHandle::onSelect(){
	//make a fist
	console.info("Select");
}

void XN_CALLBACK_TYPE CIHandle::onDrag(){
	//move with a fist
	console.info("Drag");
}

void XN_CALLBACK_TYPE CIHandle::onMove(const XnVHandPointContext* pContext, void* cxt){
	//move with open hands
	console.info("Move");
}

void XN_CALLBACK_TYPE CIHandle::onZoomSelect(){
	//two hands gesture;
	console.info("ZoomSelect");
}

void XN_CALLBACK_TYPE CIHandle::onZoom(){
	//zoom with two hands
	console.info("Zoom");
}

void XN_CALLBACK_TYPE CIHandle::onZoomEnd(){
	//zoom end
	console.info("ZoomEnd");
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

void  XN_CALLBACK_TYPE CIHandle::onPrimaryHandUpdate(const XnVHandPointContext *pContext, void *ctx){
	console.info("Primary Hand Move");
}

void  XN_CALLBACK_TYPE CIHandle::onHandUpdate(const XnVHandPointContext *pContext, void *ctx){
	console.info("Hand Move");
}