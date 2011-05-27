#include "router.h"
#include "handle.h"
#include "debug.h"
#include "pulldetector.h"
#include "handgesture.h"


CIFlowRouter::CIFlowRouter(){
	handler = new CIHandle();
}

CIFlowRouter::~CIFlowRouter(){

}

void CIFlowRouter::addToSession(XnVSessionManager * pSessionManager, CIInterface * dataGenerator){

	// Create internal objects
	handler->setDataGenerator(dataGenerator);

	//wave
	wave->RegisterWave(handler, &CIHandle::onWave);
	pSessionManager->AddListener(wave);
	
	//push click
	push->RegisterPush(handler, &CIHandle::onClick);
	pSessionManager->AddListener(push);

	pull->RegisterPull(handler, &CIHandle::onPull);
	pSessionManager->AddListener(pull);

	// Swip
	swipe->RegisterSwipeUp(handler, &CIHandle::onSwipeUp);
	swipe->RegisterSwipeDown(handler, &CIHandle::onSwipeDown);
	swipe->RegisterSwipeLeft(handler, &CIHandle::onSwipeLeft);
	swipe->RegisterSwipeRight(handler, &CIHandle::onSwipeRight);
	pSessionManager->AddListener(swipe);

	//steady
	steady->RegisterSteady(handler, &CIHandle::onSteady);
	pSessionManager->AddListener(steady);

	//hand
	hand->setDataGenerator(dataGenerator);
	hand->RegisterNoPoints(handler, &CIHandle::onNoHands);
	hand->RegisterPrimaryPointUpdate(handler, &CIHandle::onPrimaryHandUpdate);
	hand->RegisterPointUpdate(handler,&CIHandle::onHandUpdate);
	pSessionManager->AddListener(hand);
	
	// Register session callbacks
	pSessionManager->RegisterSession(&handler, &CIHandle::onSessionStart, &CIHandle::onSessionEnd, &CIHandle::onSessionProgress);
	pSessionManager->AddListener(this);
}

XnVWaveDetector   * CIFlowRouter:: wave   = new XnVWaveDetector();
XnVPushDetector   * CIFlowRouter:: push   = new XnVPushDetector();
XnVSteadyDetector * CIFlowRouter:: steady = new XnVSteadyDetector();
CIPullDetector    * CIFlowRouter:: pull   = new CIPullDetector();
XnVSwipeDetector  * CIFlowRouter:: swipe  = new XnVSwipeDetector();
CIHandGesture     * CIFlowRouter:: hand   = new CIHandGesture();