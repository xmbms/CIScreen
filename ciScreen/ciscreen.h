#ifndef __CI_SCREEN_H_
#define __CI_SCREEN_H_

#include <XnCppWrapper.h>
#include <XnVSessionManager.h>
#include "nidevice.h"
#include "handle.h"
#include "router.h"

using namespace xn;

class CIScreen{
public:
	void run();
	CIScreen();
	~CIScreen();
	void processEvent();

protected:
	XnVSessionManager * pSessionManager;
	CNIDevice kinect;
	CIFlowRouter router;

private:
	void setSessionConfig();
};

#endif