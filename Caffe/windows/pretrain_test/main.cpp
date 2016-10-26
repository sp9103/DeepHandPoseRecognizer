#include <caffe\caffe.hpp>

#define SOLVER "deploy_val.prototxt"
#define TRAINRESULT "..\\caffe\\IK_AlexNet\\snapshot_with_bn\\IK_AlexNet_iter_85035.caffemodel"

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

	return 0;
}