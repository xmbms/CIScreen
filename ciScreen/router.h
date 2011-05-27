#ifndef __CI__FLOW_ROUTER_H_
#define __CI__FLOW_ROUTER_H_

#include <XnVWaveDetector.h>
#include <XnVSessionManager.h>
#include <XnVPushDetector.h>
#include <XnVSteadyDetector.h>
#include <XnVSwipeDetector.h>

#include "XnVFlowRouter.h"
#include "handle.h"
#include "pulldetector.h"
#include "handgesture.h"
#include "ciinterface.h"

class CIFlowRouter : public XnVFlowRouter{
public:
	void addToSession(XnVSessionManager * pSessionManager, CIInterface * dataGenerator);


public:
	CIFlowRouter();
	~CIFlowRouter();

protected:
	//make detector could be reached by sessionManager
	static XnVWaveDetector   * wave; 
	static XnVPushDetector   * push;
	static XnVSteadyDetector * steady;
	static CIPullDetector    * pull;
	static XnVSwipeDetector  * swipe;
	static CIHandGesture     * hand;
	CIHandle * handler;
};

#endif