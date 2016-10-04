#pragma once

#include <fstream>
#include <direct.h>
#include <time.h>
#include <Leap.h>
#include "openCV.h"

#ifdef _WIN64
#pragma comment(lib, "x64\\Leap.lib")
#else
#pragma comment(lib, "x86\\Leap.lib")
#endif

#define HEIGHT 240
#define WIDTH 640

struct ImgData
{
	int witdh = WIDTH, height = HEIGHT;
	UCHAR data[2][HEIGHT * WIDTH];
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

	time_t startTime;

	//하나라도 삐꾸나면 인덱스 증가 안함
	bool isValid = false;

	size_t ImgSize = sizeof(UCHAR) * HEIGHT * WIDTH;

	bool isMkdir = 0;
	void makeDir(void);

	std::ofstream myfile_binary;
	std::ofstream myfile_text;

	Leap::Frame frame;

	std::string imgExtension;
	std::string textFileName;
	std::string binaryFileName;
	std::string fingerNames[5];
	std::string boneNames[4];

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
	Leap::Controller controller;

	LeapMotion::LeapMotion();
	LeapMotion::~LeapMotion();

	void updateFrame(void);

	void getImgData(ImgData *output);
	void saveImgs(void);

	void getHandsData(HandData *output);
	void saveHands(void);
	void saveHands_text(void); //debuging 용
	void loadImgs(std::string date, size_t idx);
	void loadHands(std::string date, size_t idx);
	void exit(void);
};

LeapMotion::LeapMotion()
{
	controller.setPolicy(Leap::Controller::POLICY_IMAGES);
	controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

	imgExtension = ".jpg";
	textFileName = "handsData.txt";
	binaryFileName = "handsData.dat";
	fingerNames[0] = "Thumb";
	fingerNames[1] = "Index";
	fingerNames[2] = "Middle";
	fingerNames[3] = "Ring";
	fingerNames[4] = "Pinky";

	boneNames[0] = "Metacarpal";
	boneNames[1] = "Proximal";
	boneNames[2] = "Middle";
	boneNames[3] = "Distal";
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
	frame = controller.frame();

	if (isValid) data_counter++;

	if (!controller.isConnected()) isValid = false;
	else isValid = true;

	getImgData(&imgdata);
	getHandsData(handsdata);
}

void LeapMotion::getImgData(ImgData *output)
{
	if (output == nullptr) return;
	if (!controller.isConnected()) return;

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

	if (!controller.isConnected()) return;

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


void LeapMotion::saveImgs(void)
{
	if (!isMkdir) makeDir();

	int isSaved = 0;
	for (int i = 0; i < 2; i++)
	{
		std::string file_path = dataDir + std::to_string(i) + "_" + std::to_string(data_counter) + imgExtension;

		cv::Mat Image(HEIGHT, WIDTH, CV_8UC1, imgdata.data[i]);

		if (cv::imwrite(file_path, Image)) isSaved++;
	}

	if (isValid) isValid = isSaved == 2;
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

	for (int i = 0; i < 2; i++)
		myfile_binary.write((char*)&handsdata[i], sizeof(HandData));
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

	for (int i = 0; i < 2; i++)
	{
		HandData *temp = &handsdata[i];

		if (temp->state == 0)
		{
			myfile_text << std::endl;
			continue;
		}

		std::string handType = temp->isLeft ? "Left hand" : "Right hand";
		myfile_text << std::string(2, ' ') << handType << "stats:" << temp->state << std::endl;

		myfile_text << ", id: " << temp->hand_id << std::endl
			<< ", palm position: " << temp->hand_palmPosition[0]
			<< ", " << temp->hand_palmPosition[1]
			<< ", " << temp->hand_palmPosition[2] << std::endl

			<< ", palm palmNormal: " << temp->hand_palmNormal[0]
			<< ", " << temp->hand_palmNormal[1]
			<< ", " << temp->hand_palmNormal[2] << std::endl

			<< ", palm direction: " << temp->hand_direction[0]
			<< ", " << temp->hand_direction[1]
			<< ", " << temp->hand_direction[2] << std::endl;

		myfile_text << ", arm direction: " << temp->arm_direction[0]
			<< ", " << temp->arm_direction[1]
			<< ", " << temp->arm_direction[2] << std::endl

			<< ", arm wristPosition: " << temp->arm_wristPosition[0]
			<< ", " << temp->arm_wristPosition[1]
			<< ", " << temp->arm_wristPosition[2] << std::endl

			<< ", arm elbowPosition: " << temp->arm_elbowPosition[0]
			<< ", " << temp->arm_elbowPosition[1]
			<< ", " << temp->arm_elbowPosition[2] << std::endl;

		for (int f = 0; f < 5; f++)
		{
			myfile_text << std::string(4, ' ') << fingerNames[temp->finger[f].finger_type]
				<< " finger, id: " << temp->finger[f].finger_id
				<< ", length: " << temp->finger[f].finger_length
				<< "mm, width: " << temp->finger[f].finger_width << std::endl;

			for (int b = 0; b < 4; ++b)
			{
				myfile_text << std::string(6, ' ') << boneNames[b]
					<< " bone, start: " << temp->finger[f].bone[b][0][0]
					<< ", " << temp->finger[f].bone[b][0][1]
					<< ", " << temp->finger[f].bone[b][0][2]

					<< ", end: " << temp->finger[f].bone[b][1][0]
					<< ", " << temp->finger[f].bone[b][1][1]
					<< ", " << temp->finger[f].bone[b][1][2]

					<< ", direction: " << temp->finger[f].bone[b][2][0]
					<< ", " << temp->finger[f].bone[b][2][1]
					<< ", " << temp->finger[f].bone[b][2][2] << std::endl;
			}
		}
	}
}

void LeapMotion::loadImgs(std::string loadDate, size_t idx)
{
	std::memset(imgdata.data[0], 0, ImgSize);
	std::memset(imgdata.data[1], 0, ImgSize);

	for (int i = 0; i < 2; i++)
	{
		std::string file_path = mainDir + "\\" + loadDate + "\\"
			+ std::to_string(i) + "_" + std::to_string(idx) + imgExtension;

		cv::Mat temp = cv::imread(file_path, CV_LOAD_IMAGE_GRAYSCALE);

		if (temp.empty()) continue;

		std::memcpy(imgdata.data[i], temp.data, ImgSize);
	}
}

void LeapMotion::loadHands(std::string date, size_t idx)
{
	std::string binaryFile_path = mainDir + "\\" + date + "\\" + binaryFileName;
	std::ifstream myfile_binary_read;

	myfile_binary_read.open(binaryFile_path, std::ios::in | std::ios::binary);

	if (!myfile_binary_read.is_open()) return;

	myfile_binary_read.seekg(sizeof(HandData) * 2 * idx);
	for (int i = 0; i < 2; i++)
	{
		myfile_binary_read.read((char*)&handsdata[i], sizeof(HandData));
		if (myfile_binary_read.eof()) break;
	}

	myfile_binary_read.close();
}

void LeapMotion::exit(void)
{
	if (myfile_text.is_open()) myfile_text.close();
	if (myfile_binary.is_open()) myfile_binary.close();
}