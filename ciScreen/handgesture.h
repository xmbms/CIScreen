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

class CIHandGesture : public XnVPointControl{
public:
	CIInterface * dataGenerator;
	void setDataGenerator(CIInterface * generator);
	void detectHand(XnPoint3D center);
	void checkContour(XnPoint3D center);
	void calcPCA();

public:
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

public:
	CIHandGesture();
	~CIHandGesture();
	
};

#endif
