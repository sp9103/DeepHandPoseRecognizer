#include <stdio.h>
#include <string>
#include <Windows.h>
#include <Leap.h>

#include "leapMotion.h"
//#include "openCV.h"

#define EXPORT_API __declspec(dllexport)
#define GEST_COUNT 10

using namespace std;

extern "C"{
	LeapMotion leap;

	void EXPORT_API TEST(){
		MessageBox(NULL, L"Client DLL Test Successs", L"TEST", MB_OK);
	}

	void EXPORT_API Init(){
		leap.Init();
	}

	void EXPORT_API CalcProbabilty(float prob[]){
		//TO-DO

		for (int i = 0; i < GEST_COUNT; i++){
			if (i == 3)
				prob[i] = 1.0f;
			else
				prob[i] = 0.0f;
		}
	}

	void EXPORT_API LeapUpdataFrame(){
		leap.updateFrame();
	}

	void EXPORT_API LeapShowImage(){
		for (int i = 0; i < 2; i++){
			cv::Mat Image(HEIGHT, WIDTH, CV_8UC1, leap.imgdata.data[i]);
			char id[10];
			itoa(i,  id, 10);
			cv::imshow(id, Image);
		}
		cv::waitKey(1);
	}

}