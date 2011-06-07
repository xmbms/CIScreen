#ifndef __CI_HAND_GESTURE_H_
#define __CI_HAND_GESTURE_H_

#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <vector>

#include "XnVPointControl.h"
#include "ciinterface.h"

using namespace std;

#define FINGER_SQUARE_SIZE     3600
#define THUMB_SQUARE_SIZE	   4000
#define THUMB_MIN_APPERA_COUNT 5
#define DRAW_MIN_GESTURE_COUNT 5

class CIHandGesture : public XnVPointControl{
public:
	typedef void (XN_CALLBACK_TYPE *HandGestureCB)(int count, void* pUserCxt, CvPoint center);
	XnCallbackHandle RegisterThumbStatus(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterFingerStatus(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterDragStart(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterDrag(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterDragEnd(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterDraw(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterDrawEnd(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterZoom(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterZoomEnd(void* UserContext, HandGestureCB pCB);
	XnCallbackHandle RegisterMove(void* UserContext, HandGestureCB pCB);


	void setDataGenerator(CIInterface * generator);
	void detectHand(XnPoint3D center);
	void checkContour(XnPoint3D center);
	void calcPCA();
	void activeCBs();

	void resetActionState(string action = "");

	bool isDragGesture();
	bool isDrawGesture();
	bool isZoomGesture();
	bool isPrimaryHand();
	//bool isZoomEndGesture(); //open mouse

	CvPoint hand2Screen();

	HandGestureCB dragStartCBs;
	HandGestureCB dragCBs;
	HandGestureCB dragEndCBs;
	HandGestureCB drawCBs;
	HandGestureCB drawEndCBs;
	HandGestureCB zoomCBs;
	HandGestureCB zoomEndCBs;
	HandGestureCB moveCBs;
	HandGestureCB thumbCBs;
	HandGestureCB fingerCBs;

	void * pDragStart;
	void * pDrag;
	void * pDragEnd;
	void * pDraw;
	void * pDrawEnd;
	void * pZoom;
	void * pZoomEnd;
	void * pMove;
	void * pThumb;
	void * pFinger;

public:
	CIInterface * dataGenerator;
	IplImage * contourImg;
	CvMat *avgVector;
	CvMat *eigenValue;
	CvMat *eigenVector;
	CvMat *orginMat;
	CvMat *projectMat;
	CvSeq * contour;
	CvSeq * pSeqHull;
	CvMemStorage * storage;
	CvMemStorage* ptStorage;
	int fingerCount;
	int thumbCount;
	bool thumb;

	vector<int> handX; //used for pca
	vector<int> handY;
	CvPoint handCenter;
	CvPoint bl; //bottom left of hand region
	CvPoint tr; //top right

public:
	//event handle
	static void XN_CALLBACK_TYPE onNoHands(void *cxt);
	static void XN_CALLBACK_TYPE onHandCreate(const XnVHandPointContext *pContext, void *cxt);
	static void XN_CALLBACK_TYPE onHandUpdate(const XnVHandPointContext *pContext, void *cxt);
	static void XN_CALLBACK_TYPE onHandDestory(XnUInt32 nID, void *cxt);

private:
	int dragState;
	int drawState;
	int zoomState;

	int drawGestureCount;
	int currentHandId;

public:
	CIHandGesture();
	~CIHandGesture();
	
};

#endif
