#ifndef __CI_INTERFACE_H_
#define __CI_INTERFACE_H_

#include <cv.h>
#include <XnCppWrapper.h>

class CIInterface{
public:
	virtual IplImage * getDepthImage() = 0;
	virtual IplImage * getImage() = 0;
	virtual XnPoint3D r2p(const XnPoint3D coordinate) = 0;
	virtual XnPoint3D p2r(const XnPoint3D coordinate) = 0;
	virtual int  getMaxDistance() = 0;

};

#endif