#ifndef __CI_HANDLE_H_
#define __CI_HANDLE_H_

#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>
#include "ciinterface.h"

class CIHandle{
public:
	
	//session events
	static void XN_CALLBACK_TYPE onSessionStart(const XnPoint3D& ptFocusPoint, void* UserCxt);		//session start
	static void XN_CALLBACK_TYPE onSessionEnd(void* UserCxt);		//session end
	static void XN_CALLBACK_TYPE onSessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt);   //session progress

	//control events
	static void XN_CALLBACK_TYPE onClick(XnFloat fVelocity, XnFloat fAngle, void* cxt);				//push
	static void XN_CALLBACK_TYPE onPull(void* cxt);				//pull
	static void XN_CALLBACK_TYPE onWave(void* cxt);				//wave hand
	static void XN_CALLBACK_TYPE onSteady(XnUInt32 nId, XnFloat fVelocity, void* cxt); //steady
	
	//swipe events
	static void XN_CALLBACK_TYPE onSwipeUp(XnFloat fVelocity, XnFloat fAngle, void* cxt);
	static void XN_CALLBACK_TYPE onSwipeDown(XnFloat fVelocity, XnFloat fAngle, void* cxt);
	static void XN_CALLBACK_TYPE onSwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* cxt);
	static void XN_CALLBACK_TYPE onSwipeRight(XnFloat fVelocity, XnFloat fAngle, void* cxt);
	
	//hand events
	static void XN_CALLBACK_TYPE onNoHands(void* cxt);
	static void XN_CALLBACK_TYPE onSelect();			//make a fist
	static void XN_CALLBACK_TYPE onDrag();				//move with a fist
	static void XN_CALLBACK_TYPE onMove(const XnVHandPointContext* pContext, void* cxt);				//move with open hands
	static void XN_CALLBACK_TYPE onZoomSelect();		//two hands gesture;
	static void XN_CALLBACK_TYPE onZoom();				//zoom with two hands
	static void XN_CALLBACK_TYPE onZoomEnd();			//zoom end

	static void XN_CALLBACK_TYPE onPrimaryHandUpdate(const XnVHandPointContext *pContext, void *cxt);
	static void XN_CALLBACK_TYPE onHandUpdate(const XnVHandPointContext *pContext, void *cxt);

public:
	CIInterface * dataGenerator;
	void setDataGenerator(CIInterface * generator);

public:
	CIHandle();
	~CIHandle();
};

#endif