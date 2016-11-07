#include <opencv2/core/core.hpp>

#include <vector>

#include "caffe/layers/leap_data_layer.hpp"
#include "caffe/layer.hpp"
#include "caffe/util/io.hpp"

#include <opencv2\opencv.hpp>
#include <conio.h>
#include <strsafe.h>
#include <Windows.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace caffe {

template <typename Dtype>
void LeapDataLayer<Dtype>::DataLayerSetUp(const vector<Blob<Dtype>*>& bottom,
     const vector<Blob<Dtype>*>& top) {
	batch_size_ = this->layer_param_.data_load_param().batch_size();
	channels_ = this->layer_param_.data_load_param().channels();
	height_ = this->layer_param_.data_load_param().height();
	width_ = this->layer_param_.data_load_param().width();

	data_path_ = this->layer_param_.data_load_param().data_path();
	data_limit_ = this->layer_param_.data_load_param().data_limit();

  size_ = channels_ * height_ * width_;
  CHECK_GT(batch_size_ * size_, 0) <<
      "batch_size, channels, height, and width must be specified and"
      " positive in memory_data_param";

  top[0]->Reshape(batch_size_, 2, height_, width_);					//[0] streo

  std::vector<int> fin_dim(2);
  fin_dim[0] = batch_size_;
  fin_dim[1] = 1;
  top[1]->Reshape(fin_dim);													//[1] Angle (label)

  //전체 로드
  Leap_LoadAll(data_path_.c_str());
  CHECK_GT(FileList.size(), 0) << "data is empty";

  //랜덤 박스 생성
  dataidx = 0;
  std::random_shuffle(FileList.begin(), FileList.end());
  LoadThread = std::thread(&LeapDataLayer::LoadFuc, this);
}

template <typename Dtype>
void LeapDataLayer<Dtype>::Reset(Dtype* data, Dtype* labels, int n) {
  CHECK(data);
  CHECK(labels);
  CHECK_EQ(n % batch_size_, 0) << "n must be a multiple of batch size";
  // Warn with transformation parameters since a memory array is meant to
  // be generic and no transformations are done with Reset().
  if (this->layer_param_.has_transform_param()) {
    LOG(WARNING) << this->type() << " does not transform array data on Reset()";
  }
  n_ = n;
}

template <typename Dtype>
void LeapDataLayer<Dtype>::set_batch_size(int new_size) {
  /*CHECK(!has_new_data_) <<
      "Can't change batch_size until current data has been consumed.";*/
  batch_size_ = new_size;
}

template <typename Dtype>
void LeapDataLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
	const vector<Blob<Dtype>*>& top) {
	Dtype* streo_data = top[0]->mutable_cpu_data();					//[0] Left
	Dtype* label_data = top[1]->mutable_cpu_data();				//[1] finger postion (label)

	while (label_blob.size() < batch_size_);

	for (int i = 0; i < batch_size_; i++){
		int labelIdx = *label_blob.begin();
		cv::Mat	streoImg = *streo_blob.begin();

		caffe_copy(height_ * width_ * channels_, streoImg.ptr<Dtype>(0), streo_data);
		*label_data = (Dtype)labelIdx;

		streo_data += top[0]->offset(1);
		label_data += top[1]->offset(1);

		save_mtx.lock();
		label_blob.pop_front();
		streo_blob.pop_front();
		save_mtx.unlock();
	}
}

template <typename Dtype>
void LeapDataLayer<Dtype>::Leap_LoadAll(const char* datapath){
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR szDir[MAX_PATH] = { 0, };

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, datapath, strlen(datapath), szDir, MAX_PATH);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);
	while (FindNextFile(hFind, &ffd) != 0){
		TCHAR subDir[MAX_PATH] = { 0, };
		memcpy(subDir, szDir, sizeof(TCHAR)*MAX_PATH);
		size_t len;
		StringCchLength(subDir, MAX_PATH, &len);
		subDir[len - 1] = '\0';
		StringCchCat(subDir, MAX_PATH, ffd.cFileName);
		char tBuf[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, subDir, MAX_PATH, tBuf, MAX_PATH, NULL, NULL);

		//Tchar to char
		char ccFileName[256], ccDataName[256];;
		WideCharToMultiByte(CP_ACP, 0, ffd.cFileName, len, ccFileName, 256, NULL, NULL);
		printf("directory : %s load.\n", ccFileName);

		//class directory
		if (ccFileName[0] != '.'){
			WIN32_FIND_DATA class_ffd;
			TCHAR szProcDir[MAX_PATH] = { 0, };
			HANDLE hDataFind = INVALID_HANDLE_VALUE;

			char classPath[256];
			TCHAR cDir[MAX_PATH] = { 0, };
			WIN32_FIND_DATA cffd;
			sprintf(classPath, "%s\\%s\\*", datapath, ccFileName);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, classPath, strlen(classPath), cDir, MAX_PATH);
			StringCchCat(cDir, MAX_PATH, TEXT("\\*"));
			HANDLE dFind = FindFirstFile(cDir, &cffd);
			while (FindNextFile(dFind, &cffd) != 0){
				//Tchar to char
				size_t clen;
				StringCchLength(cDir, MAX_PATH, &clen);
				WideCharToMultiByte(CP_ACP, 0, ffd.cFileName, clen, ccDataName, 256, NULL, NULL);
				printf("directory : %s load.\n", ccDataName);

				if (ccDataName[0] != 0){
					int idx = 0;
					char handFile[256];
					sprintf(handFile, "%s\\%s\\%s\\handsData.dat", datapath, ccFileName, ccDataName);
					FILE *fp = fopen(handFile, "rb");
					while (!feof(fp)){
						HandData tempHandData[2], targetData;
						fread(tempHandData, sizeof(HandData), 2, fp);
						for (int lridx = 0; lridx < 2; lridx++){
							targetData = tempHandData[lridx];
							if (tempHandData[lridx].state != 0 && tempHandData[lridx].isLeft == false)
								break;
						}
						if (targetData.state != 0){
							FilePath tempPath;
							tempPath.id = ccDataName[0] - '0';
							char file_left[256], file_right[256];
							int img_idx = targetData.data_counter;
							sprintf(file_left, "%s\\%s\\%s\\Resize\\%d_f_%d.jpg", datapath, ccFileName, ccDataName, 0, img_idx);
							sprintf(file_right, "%s\\%s\\%s\\Resize\\%d_f_%d.jpg", datapath, ccFileName, ccDataName, 1, img_idx);
							tempPath.left_path = file_left;
							tempPath.right_path = file_right;

							for (int i = 0; i < 5; i++)
								for (int j = 0; j < 4; j++)
									for (int k = 0; k < 3; k++)
										tempPath.fingerJoint[i][j][k] = targetData.finger[i].bone[j][1][k] / 10.f;

							FileList.push_back(tempPath);
						}
					}
					fclose(fp);
				}
			}
		}
	}
}

template <typename Dtype>
bool LeapDataLayer<Dtype>::fileTypeCheck(char *fileName){
	size_t fileLen;
	fileLen = strlen(fileName);

	if (fileLen < 5)
		return false;

	if (fileName[fileLen - 1] != 'g' && fileName[fileLen - 1] != 'p')
		return false;
	if (fileName[fileLen - 2] != 'p' && fileName[fileLen - 2] != 'm')
		return false;
	if (fileName[fileLen - 3] != 'j' && fileName[fileLen - 3] != 'b')
		return false;

	return true;
}

template <typename Dtype>
void LeapDataLayer<Dtype>::LoadFuc(){
	while (1){
		if (label_blob.size() < 4000){
			FilePath srcPath = FileList.at(dataidx++);
			//불러오기 쓰레드
			std::thread FileLoadThread = std::thread(&LeapDataLayer::ReadFuc, this, srcPath);

			//초과됬을때
			if (dataidx > FileList.size()){
				dataidx = 0;
				std::random_shuffle(FileList.begin(), FileList.end());
			}
		}
	}
}

template <typename Dtype>
void LeapDataLayer<Dtype>::ReadFuc(FilePath src){
	//Left & Right load
	std::string leftFilePath = src.left_path;
	std::string rightFilePath = src.right_path;
	cv::Mat leftImg = cv::imread(leftFilePath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat rightImg = cv::imread(rightFilePath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat tempStreoMat(height_, width_, CV_32FC2);

	if (leftImg.rows == 0 || rightImg.rows == 0)
		return;

	for (int h = 0; h < leftImg.rows; h++){
		for (int w = 0; w < leftImg.cols; w++){
			tempStreoMat.at<float>(0 * height_*width_ + width_*h + w) = (float)leftImg.at<uchar>(h, w) / 255.0f;
			tempStreoMat.at<float>(1 * height_*width_ + width_*h + w) = (float)rightImg.at<uchar>(h, w) / 255.0f;
		}
	}

	//store
	save_mtx.lock();
	streo_blob.push_back(tempStreoMat);
	label_blob.push_back(src.id);
	save_mtx.unlock();
}

INSTANTIATE_CLASS(LeapDataLayer);
REGISTER_LAYER_CLASS(LeapData);

}  // namespace caffe
