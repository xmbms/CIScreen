#include "XnVMessage.h"

#include "pulldetector.h"
#include "debug.h"
#include "common.h"

#define MIN_DURATION_TIME 10
#define MIN

CIPullDetector::CIPullDetector() : XnVPointControl("CI Pull Detector"){
	preMs = getPrecisionTime();
	detected = false;
}

CIPullDetector::~CIPullDetector(){
	m_pullCBs.Clear();
}

// Register/Unregister for Pull event
XnCallbackHandle CIPullDetector::RegisterPull(void* UserContext, PullCB pCB){
	XnCallbackHandle handle;
	m_pullCBs.Register(pCB, UserContext, &handle);
	return handle;
}

void CIPullDetector::UnregisterPull(XnCallbackHandle handle){
	m_pullCBs.Unregister(handle);
}

void CIPullDetector::OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptSessionStarter) {
	detected = true;
	preMs = getPrecisionTime();
}

void CIPullDetector::OnPrimaryPointUpdate(const XnVHandPointContext* pContext) {
	
}

void CIPullDetector::OnPrimaryPointReplace(XnUInt32 nOldId, const XnVHandPointContext* pContext) {

}

void CIPullDetector::OnPrimaryPointDestroy(XnUInt32 nID) {
	detected = false;
}

void CIPullDetector::OnNoPoints() {
	detected = false;
}