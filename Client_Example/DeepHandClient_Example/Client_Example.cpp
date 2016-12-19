#include <stdio.h>

#include "DeepHandClient.h"

int main(){
	DeepHandClient client;
	client.Init(NULL, DEFAULT_PORT);

	cv::Mat img = cv::imread("1_f_177.jpg");
	int gestIdx = client.SendAndRecognition(img);
	printf("idx : %d\n", gestIdx);
	img = cv::imread("1_f_211.jpg");
	gestIdx = client.SendAndRecognition(img);
	printf("idx : %d\n", gestIdx);

	client.DeInit();

	return 0;
}