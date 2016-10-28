#include <caffe\caffe.hpp>

#include "Leap_pretrain_data_loader.h"

//STEREO
#define SOLVER "..\\caffe\\resnet_pretrain\\resnet_pretrain_18_deploy.prototxt"
#define TRAINRESULT "..\\caffe\\resnet_pretrain\\snapshot_pretrain\\resnet_pretrain_18_iter_200000.caffemodel"

//////SINGLE
//#define SOLVER "..\\caffe\\resnet_pretrain_single\\resnet_pretrain_18_single_deploy.prototxt"
//#define TRAINRESULT "..\\caffe\\resnet_pretrain_single\\snapshot_pretrain\\resnet_pretrain_18_single_iter_200000.caffemodel"

typedef struct resultData_{
	float Thumb;
	float Index;
	float Middle;
	float Ring;
	float Pinky;
	float Total;
	cv::Mat left;
	cv::Mat right;
}resultData;

typedef struct writeData_{
	float startPos[5][3];
	float netFinger[5][4][3];
	float oriFinger[5][4][3];
}writeData;


bool ErrorMaxSort(resultData first, resultData second){
	return first.Total > second.Total;
}

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

	//Test & 통계 내기;
	Blob<float> rgbBlob(1, 2, 240, 240);

	float totalDist = 0.0f;
	float ThumbDist = 0.0f, IndexDist = 0.0f, MiddleDist = 0.0f, RingDist = 0.0f, PinkyDist = 0.0f;
	std::vector<resultData> resultVec;
	FILE *fp = fopen("resultData.bin", "wb");
	for (int i = 0; i < dataLoader.getCount(); i++){
		cv::Mat left, right;
		Leap_pretrain_data_loader::FilePath FilePathData;
		dataLoader.getData(&left, &right, &FilePathData, i);

		cv::Mat streoMat(240, 240, CV_32FC2);
		for (int h = 0; h < 240; h++){
			for (int w = 0; w < 240; w++){
				streoMat.at<float>(0 * 240 * 240 + 240 * h + w) = (float)left.at<uchar>(h, w) / 255.0f;
				streoMat.at<float>(1 * 240 * 240 + 240 * h + w) = (float)right.at<uchar>(h, w) / 255.0f;
			}
		}

		//network calculate
		float loss;
		memcpy(rgbBlob.mutable_cpu_data(), streoMat.ptr<float>(0), sizeof(float) * 240 * 240 * 2);
		vector<Blob<float>*> input_vec;				//입력 RGB, DEPTH
		input_vec.push_back(&rgbBlob);
		const vector<Blob<float>*>& result = caffe_test_net.Forward(input_vec, &loss);

		//거리 계산
		float output[60], outFinger[5][4][3], diffFinger[5][4];
		memcpy(output, result.at(0)->cpu_data(), sizeof(float) * 60);
		float tempDist[5], tempTotal = 0;
		for (int f = 0; f < 5; f++){
			tempDist[f] = 0;
			for (int j = 0; j < 4; j++){
				for (int c = 0; c < 3; c++){
					outFinger[f][j][c] = output[f * 12 + j * 3 + c];
				}
				diffFinger[f][j] = sqrt(pow(outFinger[f][j][0] - FilePathData.fingerJoint[f][j][0], 2) + pow(outFinger[f][j][1] - FilePathData.fingerJoint[f][j][1], 2) + pow(outFinger[f][j][2] - FilePathData.fingerJoint[f][j][2], 2));
				tempDist[f] += diffFinger[f][j] / 4.0f;
			}
			tempTotal += tempDist[f] / 5.f;
		}
		printf("%.4fcm %.4fcm %.4fcm %.4fcm %.4fcm (%.4fcm)\n", tempDist[0], tempDist[1], tempDist[2], tempDist[3], tempDist[4], tempTotal);
		totalDist += tempTotal / dataLoader.getCount();
		ThumbDist += tempDist[0] / dataLoader.getCount();
		IndexDist += tempDist[1] / dataLoader.getCount();
		MiddleDist += tempDist[2] / dataLoader.getCount();
		RingDist += tempDist[3] / dataLoader.getCount();
		PinkyDist += tempDist[4] / dataLoader.getCount();

		resultData tempResult;
		tempResult.Thumb = tempDist[0];
		tempResult.Index = tempDist[1];
		tempResult.Middle = tempDist[2];
		tempResult.Ring = tempDist[3];
		tempResult.Pinky = tempDist[4];
		tempResult.Total = tempTotal;
		tempResult.left = left.clone();
		tempResult.right = right.clone();
		resultVec.push_back(tempResult);

		////StoreData
		writeData tempWriteData;
		for (int f = 0; f < 5; f++){
			for (int j = 0; j < 4; j++){
				for (int c = 0; c < 3; c++){
					tempWriteData.netFinger[f][j][c] = outFinger[f][j][c] * 10.f;
					tempWriteData.oriFinger[f][j][c] = FilePathData.fingerJoint[f][j][c] * 10.f;
				}
			}

			for (int c = 0; c < 3; c++)
				tempWriteData.startPos[f][c] = FilePathData.startJoint[f][c] * 10.f;
		}
		fwrite(&tempWriteData, sizeof(writeData), 1, fp);
	}
	fclose(fp);

	//통계
	printf("===========Average\n===================");
	printf("%.4fcm %.4fcm %.4fcm %.4fcm %.4fcm (%.4fcm)\n", ThumbDist, IndexDist, MiddleDist, RingDist, PinkyDist, totalDist);

	std::sort(resultVec.begin(), resultVec.end(), ErrorMaxSort);
	for (int i = 0; i < resultVec.size(); i++){
		cv::imshow("left", resultVec.at(i).left);
		cv::imshow("right", resultVec.at(i).right);
		cv::waitKey(0);
	}

	return 0;
}