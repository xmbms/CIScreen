#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <vector>
#include <Windows.h>

#include "XnVPointControl.h"
#include "ciinterface.h"
#include "handgesture.h"
#include "debug.h"
#include "common.h"

using namespace std;

#define MAX_HANDSIZE 100

CIHandGesture::CIHandGesture(){
	dataGenerator = NULL;

	//storage
	contourImg = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U,1);
	avgVector  = cvCreateMat(1, 2, CV_32FC1);  //two dimension data (x,y)
	eigenValue = cvCreateMat(2, 1, CV_32FC1);
	eigenVector= cvCreateMat(2, 2, CV_32FC1);
	

	//initialize
	contour = NULL;
	pSeqHull = NULL;
	orginMat = NULL;
	storage = cvCreateMemStorage(0);
	ptStorage = cvCreateMemStorage();
	fingerCount = 0;
	thumbCount  = 0;
	dragState   = 0;
	drawState   = 0;
	zoomState   = 0;
	thumbCBs    = NULL;
	fingerCBs   = NULL;
	thumb       = false;
	pDragStart = pDrag = pDragEnd = NULL;
	pDraw = pDrawEnd = pZoom = pZoomEnd = NULL;

	drawGestureCount = 0;

	//register event hand
	RegisterNoPoints(this, &CIHandGesture::onNoHands);
	RegisterPointCreate(this, &CIHandGesture::onHandCreate);
	RegisterPointUpdate(this, &CIHandGesture::onHandUpdate);
	RegisterPointDestroy(this, &CIHandGesture::onHandDestory);
}

CIHandGesture::~CIHandGesture(){
	cvReleaseMat(&avgVector);
	cvReleaseMat(&eigenValue);
	cvReleaseMat(&eigenVector);
	cvReleaseImage(&contourImg);
}

void CIHandGesture::setDataGenerator(CIInterface * generator){
	dataGenerator = generator;
}

void XN_CALLBACK_TYPE CIHandGesture::onHandCreate(const XnVHandPointContext *pContext, void *cxt){

}

void CIHandGesture::checkContour(XnPoint3D center){

	IplImage * image = dataGenerator->getImage();
	IplImage * depth = dataGenerator->getDepthImage();
	vector<int> xhull; //used for pca projective
	vector<int> yhull;
	xhull.push_back(handCenter.x); //kinect detected center always near to the finger
	yhull.push_back(handCenter.y);
	IplImage * gray = contourImg;
	cvCanny(gray, gray, 50, 150, 3);
	cvFindContours(gray, storage, &contour, sizeof(CvContour), 
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
	

	CvSeq* ptSeq = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour),
                                 sizeof(CvPoint), ptStorage );

	//all the vertexs are used to calculate hand contour
	for (; contour != NULL; contour = contour->h_next)  {  
        pSeqHull = cvConvexHull2(contour, NULL, CV_CLOCKWISE, 0);  
		if(pSeqHull->total > 0){
			int hullcount = pSeqHull->total;
		    for(int i = 0; i < hullcount; i++ ){
				CvPoint pt = **CV_GET_SEQ_ELEM( CvPoint*, pSeqHull, i );
				cvSeqPush(ptSeq, &pt);
            }
		}  
		//detect  defects
		//CvSeq * pSeqDefect = cvConvexityDefects(contour, pSeqHull, ptStorage);  
	}  
	pSeqHull = cvConvexHull2(ptSeq, 0, CV_CLOCKWISE, 0 );
	if(pSeqHull){
		int hullcount = pSeqHull->total;
		CvPoint pt0 = **CV_GET_SEQ_ELEM( CvPoint*, pSeqHull, hullcount - 1 );
		for(int i = 0; i < hullcount; i++ ){
			CvPoint pt = **CV_GET_SEQ_ELEM( CvPoint*, pSeqHull, i );
			xhull.push_back(pt.x);
			yhull.push_back(pt.y);
			cvCircle(image, pt,  4, CV_RGB(0,255, i * 10 % 255));
			cvLine(image, pt0, pt, CV_RGB( 0, 255, 0 ), 2);
			pt0 = pt;
		}
	}

	
	//pca axis
	CvMat * orgHull = cvCreateMat(xhull.size() , 2, CV_32FC1);
	CvMat * proHull = cvCreateMat(xhull.size() , 2, CV_32FC1);
	for(int i = 0, len = xhull.size(); i < len; i++){
		cvSet2D(orgHull, i, 0, cvRealScalar(xhull[i]));
		cvSet2D(orgHull, i, 1, cvRealScalar(yhull[i]));
	}

	double x = cvGet2D(avgVector, 0, 0).val[0];
	double y = cvGet2D(avgVector, 0, 1).val[0];
	cvCircle(image, cvPoint(x, y), 5, CV_RGB(255, 0, 0), -1);
	
	//coefficients
	double a11 = cvGet2D(eigenVector, 0, 0).val[0];
	double a12 = cvGet2D(eigenVector, 0, 1).val[0];
	double a21 = cvGet2D(eigenVector, 1, 0).val[0];
	double a22 = cvGet2D(eigenVector, 1, 1).val[0];
	double b1 = (-a11 * x - a12 *y );
	double b2 = (-a21 * x - a22 *y );
	int y1 = (-a11 * bl.x - b1) / a12;
	int y2 = (-a11 * tr.x - b1) / a12;
	cvLine(image, cvPoint(bl.x, y1), cvPoint(tr.x,  y2), CV_RGB(0, 0, 255), 2);

	//main axis
	y1 = (-a21 * bl.x - b2) / a22;
	y2 = (-a21 * tr.x - b2) / a22;
	cvLine(image, cvPoint(bl.x, y1), cvPoint(tr.x, y2), CV_RGB(255, 0, 0), 2);

	cvProjectPCA(orgHull, avgVector,eigenVector, proHull);
	CvSize size = cvGetSize(proHull); 
	double flag = cvGet2D(proHull, 0, 0).val[0]; //hand center projective
	vector<int>  fingerX;
	vector<int>  fingerY;
	CvPoint thumbA;
	thumbA.x = cvGet2D(proHull, 0, 0).val[0];
	thumbA.y = cvGet2D(proHull, 0, 1).val[0];
	for(int i = 1; i < size.height; i++){
		x = cvGet2D(proHull, i, 0).val[0];
		y = cvGet2D(proHull, i, 1).val[0];
		if(x * flag > 0){
			fingerX.push_back(x);
			fingerY.push_back(y);
			//x = cvGet2D(orgHull, i, 0).val[0];
			//y = cvGet2D(orgHull, i, 1).val[0];
			//cvCircle(image, cvPoint(x, y), 5, CV_RGB(255, 0, 0), CV_FILLED);
		}
		if(abs(y) > abs(thumbA.y) && (abs(y) > abs(x))){
			x = cvGet2D(orgHull, i, 0).val[0];
			y = cvGet2D(orgHull, i, 1).val[0];
			thumbA.x = x;
			thumbA.y = y;
		}
	}

	double xdiff, ydiff, zdiff;
	XnPoint3D temp;
	fingerCount = 0;
	if(fingerX.size()){
		CvMat * finger  = cvCreateMat(fingerX.size() , 2, CV_32FC1);
		CvMat * clusters = cvCreateMat(fingerX.size() , 1, CV_32SC1 );
		for(int i = 0, len = fingerX.size(); i < len; i++){
			cvSet2D(finger, i, 0, cvRealScalar(fingerX[i]));
			cvSet2D(finger, i, 1, cvRealScalar(fingerY[i]));
		}
		cvKMeans2(finger, 5, clusters,
                   cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 ));
		double xcenter[5] = {0};
		double ycenter[5] = {0};
		int counts[5] = {0};
		int label = 0;
		for(int i = 0; i < fingerX.size(); i++){
			label = cvGet2D(clusters, i, 0).val[0];
			xcenter[label] += fingerX[i]; 
			ycenter[label] += fingerY[i];
			counts[label] ++;
		}
		CvMat * projective = cvCreateMat(1 , 2, CV_32FC1);
		CvMat * result     = cvCreateMat(1 , 2, CV_32FC1);
		for(int i = 0; i < 5; i ++){
			if(counts[i]){
				xcenter[i] /= counts[i];
				ycenter[i] /= counts[i];
			}
			cvSet2D(projective, 0, 0,  cvRealScalar(xcenter[i]));
			cvSet2D(projective, 0, 1,  cvRealScalar(ycenter[i]));
			cvBackProjectPCA(projective, avgVector, eigenVector, result);
			x = cvGet2D(result, 0, 0).val[0];
			y = cvGet2D(result, 0, 1).val[0];
			cvCircle(image, cvPoint(x, y), 5, CV_RGB(255, 0, 0), CV_FILLED);
			temp.X = x;
			temp.Y = y;
			//temp.Z = depth->imageData[index] * dataGenerator->getMaxDistance() / 255;
			temp.Z = center.Z;
			temp = dataGenerator->p2r(temp);
			xdiff = temp.X - center.X;
			ydiff = temp.Y - center.Y;
			zdiff = temp.Z - center.Z;
			if(xdiff * xdiff + ydiff * ydiff>= FINGER_SQUARE_SIZE){
				fingerCount ++;
			}
		}
		cvReleaseMat(&projective);
		cvReleaseMat(&result);
		cvReleaseMat(&finger);
		cvReleaseMat(&clusters);
	}
	cvCircle(image, thumbA, 5, CV_RGB(255, 0, 255), -1);
	temp.X = thumbA.x;
	temp.Y = thumbA.y;
	temp.Z = center.Z;
	temp = dataGenerator->p2r(temp);
	xdiff = temp.X - center.X;
	ydiff = temp.Y - center.Y;
	zdiff = temp.Z - center.Z;
	if(xdiff * xdiff + ydiff * ydiff>= THUMB_SQUARE_SIZE){
		thumbCount++;
	} else {
		thumbCount = 0;
	}

	if(thumbCount > THUMB_MIN_APPERA_COUNT){
		thumb = true;
	} else {
		thumb = false;
	}

	cvReleaseMat(&orgHull);
	cvReleaseMat(&proHull);
}

void CIHandGesture::detectHand(XnPoint3D center){
	IplImage * depth = dataGenerator->getDepthImage();
	IplImage * image = dataGenerator->getImage();

	int iStep = depth->widthStep;
	int distThreshold = MAX_HANDSIZE * 255 / dataGenerator->getMaxDistance();
	XnPoint3D temp(center);
	temp = dataGenerator->r2p(temp);
	handCenter.x = temp.X;
	handCenter.y = temp.Y;
	int centerZ = center.Z * 255 / dataGenerator->getMaxDistance();

	cvCircle(image, handCenter, 4, CV_RGB(255, 255, 0), -1);

	//get hand region according the predefined hand size
	//convert the real word hand region into projective image region
	int xstep[] = {1, 0, -1, 0}, ystep[] = {0, 1, 0, -1};
	int minX = temp.X, maxX = temp.X, minY = temp.Y, maxY = temp.Y;
	for(int i = 0; i < 4; i++){
		temp.X = center.X + xstep[i] * MAX_HANDSIZE;
		temp.Y = center.Y + ystep[i] * MAX_HANDSIZE;
		temp.Z = center.Z;
		temp = dataGenerator->r2p(temp);
		if(temp.X < minX){
			minX =temp.X;
		} else if(temp.X > maxX){
			maxX = temp.X;
		}
		if(temp.Y < minY){
			minY = temp.Y;
		} else if(temp.Y > maxY){
			maxY = temp.Y;
		}
	}
	minX = (minX >= 0)? minX : 0;
	maxX = (maxX < depth->width) ? maxX : depth->width -1;
	minY = (minY >= 0)? minY : 0;
	maxY = (maxY < depth->height) ? maxY : depth->height - 1;
	bl.x = minX;
	bl.y = minY;
	tr.x = maxX;
	tr.y = maxY;
	
	//record data for calc pca
	cvZero(contourImg);
	for(int i = minY; i <= maxY; i++){
		for(int j = minX; j <= maxX; j++){
			if(abs(depth->imageData[i * iStep + j] - centerZ) <= distThreshold){
				contourImg->imageData[i * iStep + j] = 255;
				//depth->imageData[i * iStep + j] = 255;
				handX.push_back(j);
				handY.push_back(i);
			} else {
				contourImg->imageData[i * iStep + j] = 0;
				//depth->imageData[i * iStep + j] = 0;
			}
		}
	}
	
	//cvShowImage("Contour", contourImg);
}

void CIHandGesture::calcPCA(){
	if(handX.size()){
		orginMat = cvCreateMat(handX.size() , 2, CV_32FC1);
		for(int i = 0, len = handX.size(); i < len; i++){
			cvSet2D(orginMat, i, 0, cvRealScalar(handX[i]));
			cvSet2D(orginMat, i, 1, cvRealScalar(handY[i]));
		}
		cvCalcPCA(orginMat, avgVector, eigenValue, eigenVector,CV_PCA_DATA_AS_ROW);
		cvReleaseMat(&orginMat);
		orginMat = NULL;
	}
	handX.clear();
	handY.clear();
}

void XN_CALLBACK_TYPE CIHandGesture::onHandUpdate(const XnVHandPointContext *pContext, void *ctx){
	CIHandGesture * pHandler = (CIHandGesture *)ctx;
	pHandler->currentHandId = pContext->nID;
	if(pHandler->dataGenerator){
		pHandler->detectHand(pContext->ptPosition);
		pHandler->calcPCA();
		pHandler->checkContour(pContext->ptPosition);
		pHandler->activeCBs();
	} else {
		console.error("NO Generator");
	}
}

void XN_CALLBACK_TYPE CIHandGesture::onHandDestory(XnUInt32 nID, void *cxt){

}

void XN_CALLBACK_TYPE CIHandGesture::onNoHands(void *cxt){

}

// Register/Unregister for Pull event
XnCallbackHandle CIHandGesture::RegisterThumbStatus(void* UserContext, HandGestureCB pCB){
	pThumb = UserContext;
	thumbCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterFingerStatus(void* UserContext, HandGestureCB pCB){
	pFinger = UserContext;
	fingerCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterDragStart(void* UserContext, HandGestureCB pCB){
	pDragStart = UserContext;
	dragStartCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterDrag(void* UserContext, HandGestureCB pCB){
	pDrag = UserContext;
	dragCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterDragEnd(void* UserContext, HandGestureCB pCB){
	pDragEnd = UserContext;
	dragEndCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterDraw(void* UserContext, HandGestureCB pCB){
	pDraw = UserContext;
	drawCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterDrawEnd(void* UserContext, HandGestureCB pCB){
	pDrawEnd = UserContext;
	drawEndCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterZoom(void* UserContext, HandGestureCB pCB){
	pZoom = UserContext;
	zoomCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterMove(void* UserContext, HandGestureCB pCB){
	pMove = UserContext;
	moveCBs = pCB;
	return NULL;
}

XnCallbackHandle CIHandGesture::RegisterZoomEnd(void* UserContext, HandGestureCB pCB){
	pZoomEnd = UserContext;
	zoomEndCBs = pCB;
	return NULL;
}

void CIHandGesture::activeCBs(){
	CvPoint cursor = hand2Screen();
	bool primary = isPrimaryHand();
	if(primary && isDragGesture()){
		if(drawState){
			resetActionState("drawEnd");
			drawState = 0;
		} else {
			if(!dragState){
				resetActionState("drag");
			}
			if(dragStartCBs && dragState != 1){
				dragStartCBs(0, pDragStart, cursor);
			}
			dragState = 1;
		}

	} else	if(primary && isDrawGesture()){
		if(!drawState){
			resetActionState("draw");
		}
		if(drawCBs){
			drawCBs(0, pDraw, cursor);
		}
		drawState = 1;
	} else if(isZoomGesture()){
		if(!zoomState){
			resetActionState("zoom");
		}
		if(zoomCBs){
			zoomCBs(0, pZoom, cursor);
		}
		zoomState = 1;
	} else{
		resetActionState();
	}

	if(primary && dragState){
		dragCBs(0, pDrag, cursor);
		return ;
	}
	if(primary && drawState){
		drawCBs(0, pDrag, cursor);
		return ;
	}
	if(zoomState){
		zoomCBs(0, pZoom, cursor);
		return ;
	}
	if(primary && moveCBs){
		moveCBs(0, pMove, cursor);
	}
}

void CIHandGesture::resetActionState(string action){ //drag draw move zoom drawEnd
	if(dragState && dragEndCBs && action.length()){
		dragEndCBs(0, pDragEnd, cvPoint(0, 0));
	}

	if(drawState && drawEndCBs && "drawEnd" == action){
		drawEndCBs(0, pDrawEnd, cvPoint(0, 0));
	}
	
	if(zoomState && drawEndCBs){
		zoomEndCBs(0, pZoomEnd, cvPoint(0, 0));
	}
	dragState = drawState = zoomState = 0;
	if(action.length()){
		if(action != "drag"){
		
		}
		if(action != "draw"){
			drawGestureCount = 0;
		}
		if(action != "zoom"){
		
		}
	}
}


bool CIHandGesture::isDragGesture(){
	if(!fingerCount && !thumb){
		console.warn("Sure");
		return true;
	}
	console.error("Not sure");
	return false;
}

bool CIHandGesture::isDrawGesture(){
	if(fingerCount && !thumb){
		drawGestureCount ++;
		if(drawGestureCount > DRAW_MIN_GESTURE_COUNT){
			return true;
		}
	} else{
		drawGestureCount = 0;
	}
	return false;
}

bool CIHandGesture::isZoomGesture(){
	if(fingerCount && fingerCount < 3 && thumb){
		return true;
	}
	return false;
}

CvPoint CIHandGesture::hand2Screen(){
	DWORD dwWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD dwHeight = GetSystemMetrics(SM_CYSCREEN);
	CvPoint temp = cvPoint(handCenter.x, handCenter.y);
	temp.x -= IMAGE_SCREEN_MARGIN;
	if(temp.x < 0){
		temp.x = 0;
	} else if(temp.x > dwWidth - IMAGE_SCREEN_MARGIN){
		temp.x = dwWidth - IMAGE_SCREEN_MARGIN;
	}
	temp.y -= IMAGE_SCREEN_MARGIN;
	if(temp.y < 0){
		temp.y = 0;
	} else if(temp.y > dwHeight - IMAGE_SCREEN_MARGIN){
		temp.y = dwHeight - IMAGE_SCREEN_MARGIN;
	}
	
	temp.x = temp.x * dwWidth  / (IMAGE_WIDTH - 2 * IMAGE_SCREEN_MARGIN);
	temp.y = temp.y * dwHeight / (IMAGE_HEIGHT - 2 * IMAGE_SCREEN_MARGIN);
	return temp;
}

bool CIHandGesture::isPrimaryHand(){
	return currentHandId == this->GetPrimaryID();
}