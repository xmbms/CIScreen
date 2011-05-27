#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>

#include "nidevice.h"
#include "common.h"
#include "ciinterface.h"

// xml to initialize OpenNI
#define CONFIG_XML_PATH "ciscreen.xml"

using namespace std;
using namespace cv;

CNIDevice::CNIDevice(){
	maxDistance = MAX_DISTANCE;
	isConnected = isStarted = false;
	pMatDepth = new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
	pMatImage = new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);
	pMatDepth16 = new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_16UC1);
	pDepthImage = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), IPL_DEPTH_8U,1);
	pImage = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), IPL_DEPTH_8U, 3);
}

CNIDevice::~CNIDevice(){
	delete pMatDepth16;
}


xn::Context * CNIDevice::getContext(){
	return &context;
}

int CNIDevice::getMaxDistance(){
	return maxDistance;
}

bool CNIDevice::connect(){
  //initial context 
  nRetVal = context.InitFromXmlFile(CONFIG_XML_PATH);
  checkErrors("Initialize context");  
  
  //create depth generator  
  nRetVal = depthGenerator.Create(context); 
  checkErrors("Depth generator");  
 
  //create image generator 
  nRetVal = imageGenerator.Create(context); 
  checkErrors("Image generator");
 
  //set map mode  
  XnMapOutputMode mapMode; 
  mapMode.nXRes = IMAGE_WIDTH;  
  mapMode.nYRes = IMAGE_HEIGHT; 
  mapMode.nFPS = FRAME_RATE; 
  nRetVal = depthGenerator.SetMapOutputMode(mapMode);  
  nRetVal = imageGenerator.SetMapOutputMode(mapMode);  
  
  //correct view port  
  nRetVal = depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator); 
  checkErrors("Set View Port");  
  return true;
}

bool CNIDevice::start(){
	if(!isStarted){
		nRetVal = context.StartGeneratingAll();  
		checkErrors("Start generating data");
	}
	return true;
}

bool CNIDevice::stop(){
	context.Shutdown();
	isStarted = false;
	return true;
}

bool CNIDevice::run(){

	if(!isStarted){
		start();
	}

	nRetVal = context.WaitAnyUpdateAll();
	checkErrors("Generate data");
	depthGenerator.GetMetaData(m_DepthMD);
	imageGenerator.GetMetaData(m_ImageMD);
	memcpy(pMatDepth16->data, m_DepthMD.Data(), IMAGE_WIDTH * IMAGE_HEIGHT * 2);
	memcpy(pMatImage->data,m_ImageMD.Data(), IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	
	//const XnDepthPixel* pDepthMap = depthGenerator.GetDepthMap(); 	
	//maxDistance = pDepthMap[i];
	// convert depth 11 bit image to 8 bit image
    pMatDepth16->convertTo(*pMatDepth, CV_8U, 255.0/maxDistance, 0);
    cvtColor(*pMatImage, *pMatImage, CV_RGB2BGR);

	cv::Mat temp(pImage);
	pMatImage->copyTo(temp);

	cv::Mat temp2(pDepthImage);
	pMatDepth->copyTo(temp2);

	return true;
}

IplImage * CNIDevice::getDepthImage(){
	return pDepthImage;
}

IplImage * CNIDevice::getImage(){
	return pImage;
}

XnPoint3D CNIDevice::r2p(const XnPoint3D coordinate){
	XnPoint3D temp(coordinate);
	depthGenerator.ConvertRealWorldToProjective(1, &temp, &temp);
	return temp;
}

XnPoint3D CNIDevice::p2r(const XnPoint3D coordinate){
	XnPoint3D temp(coordinate);
	depthGenerator.ConvertProjectiveToRealWorld(1, &temp, &temp);
	return temp;
}