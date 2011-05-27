#ifndef __CI_NI_DEVICE_H_
#define __CI_NI_DEVICE_H_

#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <XnCppWrapper.h>

#include "ciinterface.h"
#include "common.h"

using namespace std;
using namespace cv;

class CNIDevice : public CIInterface{
public:
	bool connect();
	IplImage * getDepthImage();
	IplImage * getImage();
	XnPoint3D r2p(const XnPoint3D coordinate);
	XnPoint3D p2r(const XnPoint3D coordinate);
	int getMaxDistance();
	bool run();
	bool start();
	bool stop();
	xn::Context * getContext();
	CNIDevice();
	~CNIDevice();
protected :
	xn::Context context; 
	xn::DepthGenerator depthGenerator;
	xn::ImageGenerator imageGenerator;
	Mat * pMatDepth;
	Mat * pMatImage;
	IplImage * pDepthImage;
	IplImage * pImage;
private:
	bool isConnected;
	bool isStarted;
	int maxDistance;
	Mat * pMatDepth16;
	xn::DepthMetaData m_DepthMD;
	xn::ImageMetaData m_ImageMD;
};

#endif