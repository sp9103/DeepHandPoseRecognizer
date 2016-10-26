#include <caffe\caffe.hpp>

#include "Leap_pretrain_data_loader.h"

//STEREO
#define SOLVER "..\\caffe\\resnet_pretrain\\resnet_pretrain_18_deploy.prototxt"
#define TRAINRESULT "..\\caffe\\resnet_pretrain\\snapshot_pretrain\\resnet_pretrain_18_iter_200000.caffemodel"

//SINGLE
#define SOLVER "..\\caffe\\resnet_pretrain_single\\resnet_pretrain_18_single_deploy.prototxt"
#define TRAINRESULT "..\\caffe\\resnet_pretrain_single\\snapshot_pretrain\\resnet_pretrain_18_single_iter_200000.caffemodel"

using namespace caffe;

int main(int argc, char** argv) {
	// mode setting - CPU/GPU
	Caffe::set_mode(Caffe::GPU);

	// gpu device number
	int device_id = 0;
	Caffe::SetDevice(device_id);

	//network test setting
	Net<float> caffe_test_net(SOLVER, caffe::TEST);

	// caffemodel(weight)
	caffe_test_net.CopyTrainedLayersFrom(TRAINRESULT);

	Leap_pretrain_data_loader dataLoader;
	dataLoader.LoadDataAll("M:\\HandData\\TEST");

	float totalDist = 0.0f;
	float ThumbDist = 0.0f, IndexDist = 0.0f, MiddleDist = 0.0f, RingDist = 0.0f, PinkyDist = 0.0f;
	for (int i = 0; i < dataLoader.getCount(); i++){

	}

	//Ελ°θ

	return 0;
}