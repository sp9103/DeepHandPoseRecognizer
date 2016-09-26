#pragma once

#include <fstream>
#include <direct.h>
#include <time.h>
#include "Leap.h"

#ifdef _WIN64
#pragma comment(lib, "x64\\Leap.lib")
#else
#pragma comment(lib, "x86\\Leap.lib")
#endif


class LeapMotion
{
private:
	std::string mainDir, dataDir, date;

	time_t startTime;

	//하나라도 삐꾸나면 인덱스 증가 안함
	bool isValid = false;
	size_t data_counter = 0;

	std::ofstream myfile;

	Leap::Frame frame;
	
	const std::string fingerNames[5] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
	const std::string boneNames[4] = { "Metacarpal", "Proximal", "Middle", "Distal" };
	
public:
	Leap::Controller controller;

	LeapMotion::LeapMotion(std::string savePath);
	LeapMotion::~LeapMotion();
	void updateFrame(void);
	void getImg(std::vector<cv::Mat>* output);
	void saveImgs(void);
	void saveHands(void);
	void exit(void);
};

LeapMotion::LeapMotion(std::string savePath)
{
	controller.setPolicy(Leap::Controller::POLICY_IMAGES);
	controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

	time_t t = time(0);
	struct tm now;
	localtime_s(&now, &t);
	date = std::to_string(now.tm_mon + 1) + std::to_string(now.tm_mday) + "_"
		+ std::to_string(now.tm_hour) + std::to_string(now.tm_min);
	std::cout << "Start time : " << date << std::endl;

	mainDir = savePath;

	dataDir = mainDir + "\\" + date + "\\";
	int fail_counter = 0;
	while (_mkdir(dataDir.c_str()))
	{
		dataDir = mainDir + "\\" + date + "(" + std::to_string(++fail_counter) + ")" + "\\";
		if (fail_counter > 10)
		{
			printf("Fail to make Dir\n");
			break;
		}
	}

	std::string textFileName = dataDir + "handsData.txt";
	myfile.open(textFileName, std::ios::out);
}

LeapMotion::~LeapMotion()
{

}

void LeapMotion::updateFrame(void)
{
	if (controller.hasFocus() && GetAsyncKeyState(VK_ESCAPE))
	{
		exit();
		_exit(1);
	}

	frame = controller.frame();

	if (isValid) data_counter++;

	std::cout << "isValid : " << isValid << "  data_idx :" << data_counter << std::endl;

	if (!controller.isConnected()) isValid = false;
	else isValid = true;
}

void LeapMotion::getImg(std::vector<cv::Mat>* output)
{
	output->clear();

	if (!controller.isConnected()) return;
	
	const Leap::ImageList images = frame.images();
	for (Leap::ImageList::const_iterator il = images.begin(); il != images.end(); ++il)
	{
		const Leap::Image image = *il;
		output->push_back(cv::Mat(image.height(), image.width(), CV_8UC1, (unsigned char*)image.data()));
	}
}

void LeapMotion::saveImgs()
{
	std::vector<cv::Mat> images;
	getImg(&images);

	bool isSaved = 1;
	for (int i = 0; i < (int)images.size(); i++)
	{
		std::string file_path = dataDir + std::to_string(i) + "_" + std::to_string(data_counter) + ".jpg";

		if(!cv::imwrite(file_path, images.at(i))) isSaved = 0;

		std::string windowName = dataDir + std::to_string(i);
		cv::imshow(windowName, images.at(i));
	}

	if (isValid) isValid = isSaved && images.size() != 0;
}

void LeapMotion::saveHands(void)
{
	if (!controller.isConnected()) return;

	myfile << "Data Idx : " << data_counter << std::endl;

	const Leap::HandList hands = frame.hands();
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
	{
		// Get the first hand
		const Leap::Hand hand = *hl;
		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
		myfile << std::string(2, ' ') << handType << ", id: " << hand.id()
			<< ", palm position: " << hand.palmPosition() << std::endl;
		// Get the hand's normal vector and direction
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();

		// Calculate the hand's pitch, roll, and yaw angles
		myfile << std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, "
			<< "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;

		// Get the Arm bone
		Leap::Arm arm = hand.arm();
		myfile << std::string(2, ' ') << "Arm direction: " << arm.direction()
			<< " wrist position: " << arm.wristPosition()
			<< " elbow position: " << arm.elbowPosition() << std::endl;

		// Get fingers
		const Leap::FingerList fingers = hand.fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		{
			const Leap::Finger finger = *fl;
			myfile << std::string(4, ' ') << fingerNames[finger.type()]
				<< " finger, id: " << finger.id()
				<< ", length: " << finger.length()
				<< "mm, width: " << finger.width() << std::endl;

			// Get finger bones
			for (int b = 0; b < 4; ++b)
			{
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
				Leap::Bone bone = finger.bone(boneType);
				myfile << std::string(6, ' ') << boneNames[boneType]
					<< " bone, start: " << bone.prevJoint()
					<< ", end: " << bone.nextJoint()
					<< ", direction: " << bone.direction() << std::endl;
			}
		}
	}

	myfile << std::endl << std::endl;
}

void LeapMotion::exit(void)
{
	myfile.close();
}