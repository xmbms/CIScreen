#include <XnVWaveDetector.h>
#include <XnVSessionManager.h>
#include <XnVMultiProcessFlowClient.h>
#include <XnVPushDetector.h>
#include <XnVSteadyDetector.h>

#include "ciscreen.h"
#include "common.h"
#include "debug.h"

using namespace xn;

CIScreen::CIScreen(){
	console.info("Connect to Kinect...");
	kinect.connect();
	console.info("Connect Established");
	// Create the Session Manager
	pSessionManager = new XnVSessionManager();
}

CIScreen::~CIScreen(){
	delete pSessionManager;
}

void CIScreen::setSessionConfig(){
	Context * pContext = kinect.getContext();

	router.addToSession(pSessionManager, &kinect);

	nRetVal = pSessionManager->Initialize(pContext, "Click,Wave", "RaiseHand");
	checkErrors("Init Session");
}

void CIScreen::run(){
	console.info("Preparing for run...");
	kinect.start();
	Context * pContext = kinect.getContext();
	setSessionConfig();
	console.info("Wave hand now");
	IplImage * depth, *image;
	while(kinect.run()){
			pSessionManager->Update(pContext);
			depth = kinect.getDepthImage();
			image = kinect.getImage();
			cvShowImage("Depth", depth);
			cvShowImage("Image", image);			
			cvWaitKey(10);
	}
}