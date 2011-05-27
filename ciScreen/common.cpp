#include <string>
#include <XnCppWrapper.h>
#include <windows.h>

#include "debug.h"

XnStatus nRetVal = XN_STATUS_OK;

void checkErrors(string tips){
	if(XN_STATUS_OK != nRetVal){
		console.error(tips + " : " + xnGetStatusString(nRetVal));
		system("pause");
		exit(1);
	}
}

LONGLONG getPrecisionCounters(){
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

LONGLONG getPrecisionFreqency(){
	LARGE_INTEGER temp;
	QueryPerformanceFrequency(&temp);
	return temp.QuadPart;
}

double getPrecisionTime(){ //in ms
	LONGLONG freq = getPrecisionFreqency();
	LONGLONG counters = getPrecisionCounters();
	return counters / (double) freq;
}