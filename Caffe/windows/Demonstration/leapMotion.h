#pragma once

#include <Windows.h>
#include <Leap.h>
#include "opencv2\opencv.hpp"

#define HEIGHT 240
#define WIDTH 640

struct ImgData
{
	int witdh = WIDTH, height = HEIGHT;
	unsigned char data[2][HEIGHT * WIDTH];
};

struct FingerData
{
	int finger_type = NULL;
	int finger_id = NULL;
	float finger_length = NULL;
	float finger_width = NULL;

	float bone[4][3][3];// prevJoint, nextJoint, direction
};
struct HandData
{
	size_t data_counter = NULL;
	int state = 0;
	bool isLeft = NULL;
	int hand_id = NULL;

	float hand_palmPosition[3];
	float hand_palmNormal[3];
	float hand_direction[3];

	float arm_direction[3];
	float arm_wristPosition[3];
	float arm_elbowPosition[3];

	FingerData finger[5];
};


class LeapMotion
{
private:
	std::string mainDir, dataDir, date;

	//하나라도 삐꾸나면 인덱스 증가 안함
	bool isValid = false;

	size_t ImgSize = sizeof(unsigned char) * HEIGHT * WIDTH;

	bool isMkdir = 0;

	Leap::Frame frame;
	Leap::Controller *controller;
	cv::Mat BackGrouond[2];
	/*Leap::Controller controller;*/

	void vector2float(Leap::Vector &from, float *to)
	{
		to[0] = from.x;
		to[1] = from.y;
		to[2] = from.z;
	};

public:

	ImgData imgdata;
	HandData handsdata[2];

	size_t data_counter = 0;

	LeapMotion::LeapMotion();
	LeapMotion::~LeapMotion();

	void Init();

	void updateFrame(void);

	void getImgData(ImgData *output);
	void getHandsData(HandData *output);
	void exit(void);
	void ForeGroundImgs(bool save);

	bool saveValid = false;
	bool UpdatValid = false;
	bool pauseFlag = true;
	cv::Mat ForeGrouond[2];
};

LeapMotion::LeapMotion()
{
}

LeapMotion::~LeapMotion()
{
}

void LeapMotion::Init(){
	controller = new Leap::Controller();
	controller->setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
	controller->setPolicy(Leap::Controller::POLICY_IMAGES);
}


void LeapMotion::updateFrame(void)
{
	if (controller->isPolicySet(Leap::Controller::POLICY_BACKGROUND_FRAMES)){
		printf("false\n");
		controller->setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
	}

	frame = controller->frame();

	if (isValid) data_counter++;

	if (!controller->isConnected()){
		isValid = false;
	}
	else isValid = true;

	getImgData(&imgdata);
	getHandsData(handsdata);
}

void LeapMotion::getImgData(ImgData *output)
{
	if (output == nullptr) return;
	if (!controller->isConnected()) return;

	std::memset(output->data[0], 0, ImgSize);
	std::memset(output->data[1], 0, ImgSize);

	int counter = 0;
	const Leap::ImageList images = frame.images();
	for (Leap::ImageList::const_iterator il = images.begin(); il != images.end(); ++il)
	{
		const Leap::Image image = *il;
		std::memcpy(output->data[counter++], image.data(), ImgSize);
	}
}

void LeapMotion::getHandsData(HandData *output)
{
	if (output == nullptr) return;

	std::memset(output, 0, sizeof(HandData) * 2);

	if (!controller->isConnected()) return;

	const Leap::HandList hands = frame.hands();
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
	{
		const Leap::Hand hand = *hl;
		int hand_idx = (int)hand.isLeft();

		HandData *temp = &output[hand_idx];
		temp->state = 1;

		temp->data_counter = data_counter;

		// Get the first hand
		temp->isLeft = hand.isLeft();
		temp->hand_id = hand.id();
		vector2float(hand.palmPosition(), temp->hand_palmPosition);

		// Get the hand's normal vector and direction
		vector2float(hand.palmNormal(), temp->hand_palmNormal);
		vector2float(hand.direction(), temp->hand_direction);

		// Get the Arm bone
		Leap::Arm arm = hand.arm();
		vector2float(arm.direction(), temp->arm_direction);
		vector2float(arm.wristPosition(), temp->arm_wristPosition);
		vector2float(arm.elbowPosition(), temp->arm_elbowPosition);

		// Get fingers
		const Leap::FingerList fingers = hand.fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		{
			const Leap::Finger finger = *fl;

			temp->finger[finger.type()].finger_type = finger.type();
			temp->finger[finger.type()].finger_id = finger.id();
			temp->finger[finger.type()].finger_length = finger.length();
			temp->finger[finger.type()].finger_width = finger.width();


			// Get finger bones
			for (int b = 0; b < 4; ++b)
			{
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
				Leap::Bone bone = finger.bone(boneType);

				vector2float(bone.prevJoint(), temp->finger[finger.type()].bone[b][0]);
				vector2float(bone.nextJoint(), temp->finger[finger.type()].bone[b][1]);
				vector2float(bone.direction(), temp->finger[finger.type()].bone[b][2]);
			}
		}
	}
}

void LeapMotion::exit(void)
{
}

void LeapMotion::ForeGroundImgs(bool save = false)
{
	if (!controller->isConnected())
	{
		saveValid = false;
		return;
	}

	bool updateBackGround = 0;
	updateBackGround |= (BackGrouond[0].empty() && UpdatValid);
	updateBackGround |= (controller->hasFocus() && GetAsyncKeyState(VK_SPACE));

	if (updateBackGround)
		for (int i = 0; i < 2; i++)
		{
			cv::Mat img(HEIGHT, WIDTH, CV_8UC1, imgdata.data[i]);
			BackGrouond[i] = img.clone();
		}

	for (int i = 0; i < 2; i++)
	{
		cv::Mat img(HEIGHT, WIDTH, CV_8UC1, imgdata.data[i]);
		ForeGrouond[i] = abs(img - BackGrouond[i]) > 40;
		ForeGrouond[i] /= 255;
		ForeGrouond[i] = img.mul(ForeGrouond[i]);

		cv::resize(ForeGrouond[i], ForeGrouond[i], cv::Size(HEIGHT, HEIGHT));
	}
}