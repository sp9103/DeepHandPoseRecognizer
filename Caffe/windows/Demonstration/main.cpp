#include <caffe\caffe.hpp>
#include < windows.h >
#include <opencv2\opencv.hpp>
#include "leapMotion.h"
#include "ResHandServer.h"

//Classification
#define SOLVER "..\\caffe\\resnet_classification_single\\resnet_classification_18_deploy_single.prototxt"
#define TRAINRESULT "..\\caffe\\resnet_classification_single\\snapshot\\resnet_18_iter_200000.caffemodel"

using namespace caffe;

int findMaxIdx(float *src, int count){
	float max = -FLT_MAX;
	int retVal = -1;
	for (int i = 0; i < count; i++)
		if (max < src[i]){
			max = src[i];
			retVal = i;
		}

	return retVal;
}

int main(){
	// mode setting - CPU/GPU
	Caffe::set_mode(Caffe::GPU);

	// gpu device number
	int device_id = 0;
	Caffe::SetDevice(device_id);

	//network test setting
	Net<float> caffe_test_net(SOLVER, caffe::TEST);

	// caffemodel(weight)
	caffe_test_net.CopyTrainedLayersFrom(TRAINRESULT);

	//Test & 통계 내기;
	Blob<float> rgbBlob(1, /*2*/1, 240, 240);

	//leap motion
	LeapMotion leap;
	leap.Init();

	//Server
	ResHandServer server;
	server.Init(NULL, 2252);

	while(1){
		cv::Mat left, right;
		leap.updateFrame();

		leap.ForeGroundImgs(false);
		left = leap.ForeGrouond[0].clone();
		right = leap.ForeGrouond[1].clone();
		if (left.rows == 0)	continue;

		cv::Mat streoMat(240, 240, CV_32FC1);
		for (int h = 0; h < 240; h++){
			for (int w = 0; w < 240; w++){
				streoMat.at<float>(0 * 240 * 240 + 240 * h + w) = (float)left.at<uchar>(h, w) / 255.0f;
				//streoMat.at<float>(1 * 240 * 240 + 240 * h + w) = (float)right.at<uchar>(h, w) / 255.0f;
			}
		}

		//network calculate
		float loss;
		memcpy(rgbBlob.mutable_cpu_data(), streoMat.ptr<float>(0), sizeof(float) * 240 * 240 * /*2*/1);
		vector<Blob<float>*> input_vec;				//입력 RGB, DEPTH
		input_vec.push_back(&rgbBlob);
		const vector<Blob<float>*>& result = caffe_test_net.Forward(input_vec, &loss);

		float resultProb[14];
		memcpy(resultProb, result.at(0)->cpu_data(), sizeof(float) * 14);
		
		//server.sendVec(resultProb);
		//for (int i = 0; i < 14; i++){
		//	printf("%.2f ", resultProb[i]);
		//}
		//printf("\n");

		cv::imshow("Image", left);
		if (cv::waitKey(10) == 27)
			break;
	}

	server.DeInit();

	return 0;
}