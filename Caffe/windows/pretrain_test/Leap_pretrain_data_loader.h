#pragma once
#include <opencv2\opencv.hpp>
#include <Windows.h>
#include <strsafe.h>
#include <conio.h>

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
	int data_counter = NULL;
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

class Leap_pretrain_data_loader
{
public:
	Leap_pretrain_data_loader();
	~Leap_pretrain_data_loader();

	void LoadDataAll(char *path);
	int getCount();

	typedef struct path_{
		std::string left_path;
		std::string right_path;
		float fingerJoint[5][4][3];
		float startJoint[5][3];
		int id;
	}FilePath;

	void getData(cv::Mat *left, cv::Mat *right, FilePath *data, int idx);

private:
	std::vector<FilePath> dataList;

};

