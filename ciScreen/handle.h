#ifndef __CI_HANDLE_H_
#define __CI_HANDLE_H_

#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>
#include "ciinterface.h"
#include "displayer.h"

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
	static void XN_CALLBACK_TYPE onDragStart(int count, void * cxt, CvPoint center);				
	static void XN_CALLBACK_TYPE onDrag(int count, void * cxt, CvPoint center);				
	static void XN_CALLBACK_TYPE onDragEnd(int count, void * cxt, CvPoint center);				
	static void XN_CALLBACK_TYPE onMove(int count, void * cxt, CvPoint center); //move with open hands
	static void XN_CALLBACK_TYPE onZoom(int count, void * cxt, CvPoint center);				//zoom with two hands
	static void XN_CALLBACK_TYPE onZoomEnd(int count, void * cxt, CvPoint center);			//zoom end
	static void XN_CALLBACK_TYPE onDraw(int count, void * cxt, CvPoint center);
	static void XN_CALLBACK_TYPE onDrawEnd(int count, void * cxt, CvPoint center);

public:
	CIInterface * dataGenerator;
	CIDisplayer * displayer;
	void setDataGenerator(CIInterface * generator);
	void onSessionInit();
	HWND dragWnd;
	POINT dragPt; 
	RECT  dragWndRect;

public:
	CIHandle();
	~CIHandle();

public:
	int mouseState;
};



#endif