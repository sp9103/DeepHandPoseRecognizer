#include <iostream>
#include <windows.h> 
#include "openCV.h"
#include "leapMotion.h"

ImgData* getImgDataPtr(LeapMotion *LeapMotion) { return &LeapMotion->imgdata; }; // 1°³
HandData* getHandsDataPtr(LeapMotion *LeapMotion) { return LeapMotion->handsdata; }; // 2°³

int main(int argc, char** argv)
{
	LeapMotion lm("..\\data");

	bool load = 1; // 0:live data, 1:loaded data
	bool save = 1;

	std::string loadDate = "930_1659";

	while (1)
	{
		if (load)
		{
			lm.loadImgs(loadDate, lm.data_counter);

			lm.loadHands(loadDate, lm.data_counter);

			lm.data_counter++;

			std::cout << "Load. data_idx :" << lm.data_counter << std::endl;
		}
		else
		{
			lm.updateFrame();

			std::cout << "Live. data_idx :" << lm.data_counter << std::endl;
		}


		
		if (save)
		{
			lm.saveImgs();

			lm.saveHands();

			lm.saveHands_text();
		}

		Sleep(100);
		if (lm.controller.hasFocus() && GetAsyncKeyState(VK_ESCAPE))
		{
			lm.exit();
			_exit(1);
		}
	}

	return 1;
}