#ifndef __CI_COMMON_H_
#define __CI_COMMON_H_

#include <string>



using namespace std;

extern XnStatus nRetVal;

void checkErrors(string tips);

double getPrecisionTime();

//global settings
#define IMAGE_WIDTH   640
#define IMAGE_HEIGHT  480
#define FRAME_RATE    30
#define MAX_DISTANCE  7000
#define IMAGE_SCREEN_MARGIN 120

#endif