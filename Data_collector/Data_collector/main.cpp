#include <iostream>
#include <windows.h> 
#include "openCV.h"
#include "leapMotion.h"

int main(int argc, char** argv)
{
	LeapMotion lm("..\\data");

	while (1)
	{
		lm.updateFrame();

		lm.saveImgs();

		lm.saveHands();

		Sleep(100);
		cv::waitKey(1);
	}

	return 1;
}