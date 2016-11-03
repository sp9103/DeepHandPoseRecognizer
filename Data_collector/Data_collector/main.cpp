#include <iostream>
#include <windows.h> 
#include "openCV.h"
#include "leapMotion.h"

ImgData* getImgDataPtr(LeapMotion *LeapMotion) { return &LeapMotion->imgdata; }; // 1°³
HandData* getHandsDataPtr(LeapMotion *LeapMotion) { return LeapMotion->handsdata; }; // 2°³

int main(int argc, char** argv)
{
	LeapMotion lm("..\\data");

	cv::Mat img1(HEIGHT, WIDTH, CV_8UC1, lm.imgdata.data[0]);
	cv::Mat img2(HEIGHT, WIDTH, CV_8UC1, lm.imgdata.data[1]);
	cv::Mat BackGroundImg[2];


	bool load = 0; // 0:live data, 1:loaded data
	bool save = 0;
	bool show = 1;
	bool backgroundSubtraction = 1;

	int save_pause_count = 10;

	cv::Ptr<cv::BackgroundSubtractor>  bg1 = cv::createBackgroundSubtractorMOG2();
	cv::Ptr<cv::BackgroundSubtractor>  bg2 = cv::createBackgroundSubtractorMOG2();
	cv::Mat bg_img[2];

	std::string loadDate = "108_1614";

	while (1)
	{
		if (save_pause_count > 0) save_pause_count--;
		else if (GetAsyncKeyState(VK_CONTROL))
		{
			save_pause_count = 10;
			if (save)
			{
				std::cout << "Pause save" << std::endl;
				save = 0;
				lm.pauseFlag = 1;
			}
			else
			{
				std::cout << "Start save" << std::endl;
				save = 1;
				lm.pauseFlag = 0;
			}
		}

		if (load)
		{
			lm.loadImgs(loadDate, lm.data_counter);
			lm.loadHands(loadDate, lm.data_counter);
			if(lm.UpdatValid) lm.data_counter++;
			lm.UpdatValid = true;
			if(!lm.pauseFlag) std::cout << "Load. data_idx :" << lm.data_counter << std::endl;
		}
		else
		{
			lm.updateFrame();
			if (!lm.pauseFlag)  std::cout << "Live. data_idx :" << lm.data_counter << std::endl;
		}

		if (save)
		{
			lm.saveImgs();
			lm.saveHands();
			lm.saveHands_text();
		}

		if (backgroundSubtraction)
		{
			lm.ForeGroundImgs(save);
		}

		if (show)
		{
			cv::imshow("img1", img1);
			cv::imshow("img2", img2);

			if (backgroundSubtraction)
			{
				cv::imshow("ForeGrouond1", lm.ForeGrouond[0]);
				cv::imshow("ForeGrouond2", lm.ForeGrouond[1]);
			}

			cv::waitKey(1);
		}

		//Sleep(100);
		if (lm.controller.hasFocus() && GetAsyncKeyState(VK_ESCAPE))
		{
			lm.exit();
			_exit(1);
		}
	}

	return 1;
}