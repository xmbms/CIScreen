#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <vector>

#include "XnVPointControl.h"
#include "ciinterface.h"
#include "handgesture.h"
#include "debug.h"

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

void CIHandGesture::checkContour(){

	IplImage * image = dataGenerator->getImage();
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
    for (; contour != NULL; contour = contour->h_next)  
    {  
        pSeqHull = cvConvexHull2(contour, NULL, CV_CLOCKWISE, 0);  
		if(pSeqHull->total > 0){
			int hullcount = pSeqHull->total;
		    for(int i = 0; i < hullcount; i++ ){
				CvPoint pt = **CV_GET_SEQ_ELEM( CvPoint*, pSeqHull, i );
				cvSeqPush(ptSeq, &pt);
            }
		}  
		//detect  defects
		//pSeqDefect = cvConvexityDefects(contour, pSeqHull, dstorage);  
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
	for(int i = 1; i < size.height; i++){
		x = cvGet2D(proHull, i, 0).val[0];
		y = cvGet2D(proHull, i, 1).val[0];
		if(x * flag > 0){
			x = cvGet2D(orgHull, i, 0).val[0];
			y = cvGet2D(orgHull, i, 1).val[0];
			cvCircle(image, cvPoint(x, y), 5, CV_RGB(255, 0, 0), CV_FILLED);
		}
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
				depth->imageData[i * iStep + j] = 255;
				handX.push_back(j);
				handY.push_back(i);
			} else {
				contourImg->imageData[i * iStep + j] = 0;
				depth->imageData[i * iStep + j] = 0;
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
	if(pHandler->dataGenerator){
		pHandler->detectHand(pContext->ptPosition);
		pHandler->calcPCA();
		pHandler->checkContour();
	} else {
		console.error("NO Generator");
	}
}

void XN_CALLBACK_TYPE CIHandGesture::onHandDestory(XnUInt32 nID, void *cxt){

}

void XN_CALLBACK_TYPE CIHandGesture::onNoHands(void *cxt){

}