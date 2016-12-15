#include <caffe\caffe.hpp>
#include < windows.h >
#include <opencv2\opencv.hpp>

#include "DeepHandServer.h"

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
	//Caffe::set_mode(Caffe::CPU);

	// gpu device number
	int device_id = 0;
	Caffe::SetDevice(device_id);

	//network test setting
	Net<float> caffe_test_net(SOLVER, caffe::TEST);

	// caffemodel(weight)
	caffe_test_net.CopyTrainedLayersFrom(TRAINRESULT);

	//Test & 통계 내기;
	Blob<float> rgbBlob(1, /*2*/1, 240, 240);

	//Server
	DeepHandServer server;
	server.Init(NULL, DEFAULT_PORT);

	while (1){
		cv::Mat left, right;
		left = server.recvFromClient();
		if (left.rows == 0)	continue;

		///////////////////////////////////////////////////////////////////이미지 전처리 부분////////////////////////////////////////////////////////////////////
		//TO-DO

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		
		float prob[14];
		memcpy(prob, result.at(0)->cpu_data(), sizeof(float) * 14);
		int top1Idx = findMaxIdx(prob, 14);
		server.sendAnswer(top1Idx);

		cv::imshow("Left", left);
		if (cv::waitKey(10) == 27)
			break;
	}

	return 0;
}