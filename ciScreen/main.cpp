#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <XnCppWrapper.h>

#include "debug.h"
#include "nidevice.h"
#include "ciscreen.h"


using namespace std;
using namespace cv;

int main(void){
	CIScreen wall;
	wall.run();
	system("pause");
	return 0;
}
