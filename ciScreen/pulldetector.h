#ifndef __CI_PULL_DETECTOR_H_
#define __CI_PULL_DETECTOR_H_

#include "XnVMessage.h"
#include "XnVNiteEvents.h"
#include "XnVPointControl.h"
#include "XnVMultipleHands.h"

class CIPullDetector : public XnVPointControl{
public:
	typedef void (XN_CALLBACK_TYPE *PullCB)(void* pUserCxt);

	XnCallbackHandle RegisterPull(void* UserContext, PullCB pCB);
	void UnregisterPull(XnCallbackHandle handle);

    void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptSessionStarter);
	void OnPrimaryPointUpdate(const XnVHandPointContext* pContext);
	void OnPrimaryPointReplace(XnUInt32 nOldId, const XnVHandPointContext* pContext);
	void OnPrimaryPointDestroy(XnUInt32 nID);
	void OnNoPoints();

	XnUInt32 GetPushDuration() const;

	/**
	* Start push detection over from scratch.
	*/
	void Reset();

	/**
	* Get the minimum velocity in the time span to define as push, in m/s
	*
	* @return the velocity
	*/
	XnFloat GetPushImmediateMinimumVelocity() const;
	/**
	* Get the time used to detect push, in ms
	*
	* @return the time
	*/
	XnUInt32 GetPushImmediateDuration() const;
	/**
	 * Get the the immediate timespan to skip, in ms
	 *
	 * @return the time
	 */
	XnUInt32 GetPushImmediateOffset() const;

	/**
	* Get the minimum velocity of the previous time span, in m/s
	*
	* @return the velocity
	*/
	XnFloat GetPushPreviousMinimumVelocity() const;
	/**
	* Get the time of the previous time span, in ms
	*
	* @return the time
	*/
	XnUInt32 GetPushPreviousDuration() const;
	/**
	* Get the time to skip of the previous time span, in ms
	*
	* @return the time
	*/
	XnUInt32 GetPushPreviousOffset() const;

	/**
	 * Get the maximum angle between the immediate direction and the Z-axis, in degrees
	 *
	 * @return the angle
	 */
	XnFloat GetPushMaximumAngleBetweenImmediateAndZ() const;
	/**
	* Get the maximum angle between the immediate direction and the previous direction, in degrees
	*
	* @return the angle
	*/
	XnFloat GetPushMinimumAngleBetweenImmediateAndPrevious() const;
	
	/**
	* Get the maximum velocity in the time span to define as stable, in m/s
	*
	* @return the velocity
	*/
	XnFloat GetStableMaximumVelocity() const;
	/**
	* Get the time used to detect stable state, in ms
	*
	* @return the time
	*/
	XnUInt32 GetStableDuration() const;

	/**
	* Change the minimum velocity in the time span to define as push. Default is 0.33 m/s
	*
	* @return the velocity
	*/
	void SetPushImmediateMinimumVelocity(XnFloat fVelocity);
	/**
	* Change the time used to detect push. Default is 240ms
	*
	* @return the time
	*/
	void SetPushImmediateDuration(XnUInt32 nDuration);
	/**
	* Change the the immediate timespan to skip. Default is 0ms
	*
	* @return the time
	*/
	void SetPushImmediateOffset(XnUInt32 nOffset);

	/**
	* Change the minimum velocity of the previous time span. Default is 0.17 m/s
	*
	* @return the velocity
	*/
	void SetPushPreviousMinimumVelocity(XnFloat fVelocity);
	/**
	* Change the time of the previous time span. Default is 150 ms
	*
	* @return the time
	*/
	void SetPushPreviousDuration(XnUInt32 nDuration);
	/**
	* Change the time to skip of the previous time span. Default is 240ms
	*
	* @return the time
	*/
	void SetPushPreviousOffset(XnUInt32 nOffset);

	/**
	* Change the maximum angle between the immediate direction and the Z-axis. Default is 30 degrees
	*
	* @return the angle
	*/
	void SetPushMaximumAngleBetweenImmediateAndZ(XnFloat fAngle);
	/**
	* Change the maximum angle between the immediate direction and the previous direction. Default is 20 degrees
	*
	* @return the angle
	*/
	void SetPushMinimumAngleBetweenImmediateAndPrevious(XnFloat fAngle);

	/**
	* Change the maximum velocity in the time span to define as stable. Default is 0.13 m/s
	*
	* @return the velocity
	*/
	void SetStableMaximumVelocity(XnFloat fVelocity);
	/**
	* Get the time used to detect stable state. Default is 360 ms
	*
	* @return the time
	*/
	void SetStableDuration(XnUInt32 nDuration);
	CIPullDetector();
	~CIPullDetector();

private:
	double preMs;
	bool   detected;
	XnVEvent m_pullCBs;
};

#endif