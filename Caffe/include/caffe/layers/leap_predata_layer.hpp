#ifndef CAFFE_IK_DATA_LAYER_HPP_
#define CAFFE_IK_DATA_LAYER_HPP_

#include <string>
#include <utility>
#include <vector>
#include <list>
#include <algorithm>

#include <opencv2\opencv.hpp>
#include <thread>
#include <mutex>

#include "caffe/blob.hpp"
#include "caffe/data_reader.hpp"
#include "caffe/data_transformer.hpp"
#include "caffe/internal_thread.hpp"
#include "caffe/layer.hpp"
#include "caffe/layers/base_data_layer.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"

namespace caffe {

	template <typename Dtype>
	class LeapPredataLayer : public BaseDataLayer<Dtype> {
	public:
		explicit LeapPredataLayer(const LayerParameter& param)
			: BaseDataLayer<Dtype>(param) {}
		virtual void DataLayerSetUp(const vector<Blob<Dtype>*>& bottom,
			const vector<Blob<Dtype>*>& top);

		virtual inline const char* type() const { return "LeapPredata"; }
		virtual inline int ExactNumBottomBlobs() const { return 0; }

		// Reset should accept const pointers, but can't, because the memory
		//  will be given to Blob, which is mutable
		void Reset(Dtype* data, Dtype* label, int n);
		void set_batch_size(int new_size);

		int batch_size() { return batch_size_; }
		int channels() { return channels_; }
		int height() { return height_; }
		int width() { return width_; }
		int data_limit() { return data_limit_; }

	protected:
		typedef struct path_{
			std::string left_path;
			std::string right_path;
			float fingerJoint[5][4][3];
			int id;
		}FilePath;

		virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
			const vector<Blob<Dtype>*>& top);

		void Leap_PredataLoadAll(const char* datapath);
		bool fileTypeCheck(char *fileName);
		void LoadFuc(int totalThread, int id);

		int batch_size_, channels_, height_, width_, size_;
		int n_;
		int data_limit_;

		std::string data_path_;

		std::list<cv::Mat> left_blob;						//rgb image
		std::list<cv::Mat> right_blob;						//distance
		std::list<cv::Mat> finger_blob;			//pregrasping pos (image idx, pos)

		std::vector<FilePath> FileList;

		std::mutex idx_mtx, save_mtx;
		std::thread LoadThread[4];
		int ThreadCount;
		bool stop_thread;

		int *randbox;
		int dataidx;
	};

}  // namespace caffe

#endif  // CAFFE_DATA_LAYER_HPP_
