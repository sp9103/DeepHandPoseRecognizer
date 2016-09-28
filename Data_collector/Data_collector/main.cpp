#include <iostream>
#include <windows.h> 
#include "openCV.h"
#include "leapMotion.h"

int main(int argc, char** argv)
{
	LeapMotion lm("..\\data");

	bool save = 0;
	
	while (save)
	{
		lm.updateFrame();

		lm.saveImgs();

		lm.saveHands();

		lm.saveHands_text();

		Sleep(100);
		cv::waitKey(1);
	}

	size_t idx = 0;
	std::string loadDate = "928_1020";
	while (!save)
	{
		std::vector<cv::Mat> imgs;
		lm.loadImgs(loadDate, idx, imgs);

		std::vector<LeapMotion::HandData> temp;
		lm.loadHands(loadDate, idx, temp);

		idx++;

		cv::waitKey(10);
	}

	return 1;
}