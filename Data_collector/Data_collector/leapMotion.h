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

	bool isMkdir = 0;
	void makeDir(void);

	std::ofstream myfile_binary;
	std::ofstream myfile_text;

	Leap::Frame frame;
	
	const std::string imgExtension = ".jpg";
	const std::string textFileName = "handsData.txt";
	const std::string binaryFileName = "handsData.dat";
	const std::string fingerNames[5] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
	const std::string boneNames[4] = { "Metacarpal", "Proximal", "Middle", "Distal" };
	
public:

	struct FingerData
	{
		int finger_type = NULL;
		int finger_id = NULL;
		float finger_length = NULL;
		float finger_width = NULL;
	
		Leap::Vector bone[4][3]; // prevJoint, nextJoint, direction
	};
	struct HandData
	{
		size_t data_counter = NULL;
		bool isLeft = NULL;
		int hand_id = NULL;

		Leap::Vector hand_palmPosition;
		Leap::Vector hand_palmNormal;
		Leap::Vector hand_direction;

		Leap::Vector arm_direction;
		Leap::Vector arm_wristPosition;
		Leap::Vector arm_elbowPosition;

		FingerData finger[5];
	};

	Leap::Controller controller;

	LeapMotion::LeapMotion(std::string savePath);
	LeapMotion::~LeapMotion();
	void updateFrame(void);
	void getImg(std::vector<cv::Mat>* output);
	void saveImgs(void);
	void saveHands(void);
	void saveHands_text(void); //debuging 용
	void loadImgs(std::string date, size_t idx, std::vector<cv::Mat> &output);
	void loadHands(std::string date, size_t idx, std::vector<HandData> &output);
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
}

LeapMotion::~LeapMotion()
{
	if (myfile_text.is_open()) myfile_text.close();

	if (myfile_binary.is_open()) myfile_binary.close();
}

void LeapMotion::makeDir(void)
{
	if (mainDir.length() == 0) return;

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

	isMkdir = 1;
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

	if (!isMkdir) makeDir();

	bool isSaved = 1;
	for (int i = 0; i < (int)images.size(); i++)
	{
		std::string file_path = dataDir + std::to_string(i) + "_" + std::to_string(data_counter) + imgExtension;

		if(!cv::imwrite(file_path, images.at(i))) isSaved = 0;

		std::string windowName = "Save : " + dataDir + std::to_string(i);
		cv::imshow(windowName, images.at(i));
	}

	if (isValid) isValid = isSaved && images.size() != 0;
}

void LeapMotion::saveHands(void)
{
	if (!controller.isConnected()) return;

	if (!isMkdir) makeDir();

	if (!myfile_binary.is_open())
	{
		std::string binaryFile_path = dataDir + binaryFileName;
		myfile_binary.open(binaryFile_path, std::ios::out | std::ios::binary | std::ios_base::trunc);
	}

	const Leap::HandList hands = frame.hands();
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
	{
		HandData temp;

		temp.data_counter = data_counter;

		// Get the first hand
		const Leap::Hand hand = *hl;
		temp.isLeft = hand.isLeft();
		temp.hand_id = hand.id();
		temp.hand_palmPosition = hand.palmPosition();
		// Get the hand's normal vector and direction
		temp.hand_palmNormal = hand.palmNormal();
		temp.hand_direction = hand.direction();

		// Get the Arm bone
		Leap::Arm arm = hand.arm();
		temp.arm_direction = arm.direction();
		temp.arm_wristPosition = arm.wristPosition();
		temp.arm_elbowPosition = arm.elbowPosition();

		// Get fingers
		const Leap::FingerList fingers = hand.fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		{
			const Leap::Finger finger = *fl;

			temp.finger[finger.type()].finger_type = finger.type();
			temp.finger[finger.type()].finger_id = finger.id();
			temp.finger[finger.type()].finger_length = finger.length();
			temp.finger[finger.type()].finger_width = finger.width();


			// Get finger bones
			for (int b = 0; b < 4; ++b)
			{
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
				Leap::Bone bone = finger.bone(boneType);

				temp.finger[finger.type()].bone[b][0] = bone.prevJoint();
				temp.finger[finger.type()].bone[b][1] = bone.nextJoint();
				temp.finger[finger.type()].bone[b][2] = bone.direction();
			}
		}

		myfile_binary.write((char*)&temp, sizeof(HandData));
	}
}

void LeapMotion::saveHands_text(void)
{
	if (!controller.isConnected()) return;

	if (!isMkdir) makeDir();

	if (!myfile_text.is_open())
	{
		std::string textFilepath = dataDir + textFileName;
		myfile_text.open(textFilepath, std::ios::out);
	}


	myfile_text << "Data Idx : " << data_counter << std::endl;

	const Leap::HandList hands = frame.hands();
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
	{
		// Get the first hand
		const Leap::Hand hand = *hl;
		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
		myfile_text << std::string(2, ' ') << handType << ", id: " << hand.id()
			<< ", palm position: " << hand.palmPosition() << std::endl;
		// Get the hand's normal vector and direction
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();

		// Calculate the hand's pitch, roll, and yaw angles
		myfile_text << std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, "
			<< "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;

		// Get the Arm bone
		Leap::Arm arm = hand.arm();
		myfile_text << std::string(2, ' ') << "Arm direction: " << arm.direction() << std::endl
			<< " wrist position: " << arm.wristPosition() << std::endl
			<< " elbow position: " << arm.elbowPosition() << std::endl;

		// Get fingers
		const Leap::FingerList fingers = hand.fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		{
			const Leap::Finger finger = *fl;
			myfile_text << std::string(4, ' ') << fingerNames[finger.type()]
				<< " finger, id: " << finger.id()
				<< ", length: " << finger.length()
				<< "mm, width: " << finger.width() << std::endl;

			// Get finger bones
			for (int b = 0; b < 4; ++b)
			{
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
				Leap::Bone bone = finger.bone(boneType);
				myfile_text << std::string(6, ' ') << boneNames[boneType]
					<< " bone, start: " << bone.prevJoint()
					<< ", end: " << bone.nextJoint()
					<< ", direction: " << bone.direction() << std::endl;
			}
		}
	}

	myfile_text << std::endl << std::endl;
}

void LeapMotion::loadImgs(std::string loadDate, size_t idx, std::vector<cv::Mat> &output)
{
	if (!output.empty()) output.clear();

	for (int i = 0; i < 2; i++)
	{
		std::string file_path = mainDir + "\\" + loadDate + "\\"
			+ std::to_string(i) + "_" + std::to_string(idx) + imgExtension;

		cv::Mat temp = cv::imread(file_path);

		if (temp.empty()) continue;
			
		output.push_back(temp);

		std::string windowName = "Load : " + mainDir + "\\" + loadDate + "\\" + std::to_string(i);
		cv::imshow(windowName, temp);
	}
}

void LeapMotion::loadHands(std::string date, size_t idx, std::vector<HandData> &output)
{
	if (!output.empty()) output.clear();

	std::string binaryFile_path = mainDir + "\\" + date + "\\" + binaryFileName;
	std::ifstream myfile_binary;

	myfile_binary.open(binaryFile_path, std::ios::in | std::ios::binary);

	if (!myfile_binary.is_open()) return;

	HandData temp;
	while (1)
	{
		myfile_binary.read((char*)&temp, sizeof(HandData));
		if (myfile_binary.eof()) break;

		if (temp.data_counter == idx)
			output.push_back(temp);

		if (temp.data_counter > idx)
			break;
	}
}

void LeapMotion::exit(void)
{
	if (myfile_text.is_open()) myfile_text.close();
	if (myfile_binary.is_open()) myfile_binary.close();
}