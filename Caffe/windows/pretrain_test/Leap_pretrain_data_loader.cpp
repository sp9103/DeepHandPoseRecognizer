#include "Leap_pretrain_data_loader.h"


Leap_pretrain_data_loader::Leap_pretrain_data_loader()
{
}


Leap_pretrain_data_loader::~Leap_pretrain_data_loader()
{
}

void Leap_pretrain_data_loader::LoadDataAll(char *datapath){
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
		char ccFileName[256];
		WideCharToMultiByte(CP_ACP, 0, ffd.cFileName, len, ccFileName, 256, NULL, NULL);
		printf("directory : %s load.\n", ccFileName);

		if (ccFileName[0] != '.'){
			WIN32_FIND_DATA class_ffd;
			TCHAR szProcDir[MAX_PATH] = { 0, };
			HANDLE hDataFind = INVALID_HANDLE_VALUE;

			int idx = 0;
			char handFile[256];
			sprintf(handFile, "%s\\%s\\handsData.dat", datapath, ccFileName);
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
					char file_left[256], file_right[256];
					int img_idx = targetData.data_counter;
					sprintf(file_left, "%s\\%s\\Resize\\%d_%d.jpg", datapath, ccFileName, 0, img_idx);
					sprintf(file_right, "%s\\%s\\Resize\\%d_%d.jpg", datapath, ccFileName, 1, img_idx);
					tempPath.left_path = file_left;
					tempPath.right_path = file_right;

					for (int i = 0; i < 5; i++){
						for (int j = 0; j < 4; j++)
							for (int k = 0; k < 3; k++)
								tempPath.fingerJoint[i][j][k] = targetData.finger[i].bone[j][1][k] / 10.f;

						for (int c = 0; c < 3; c++)
							tempPath.startJoint[i][c] = targetData.finger[i].bone[0][0][c];
					}
					dataList.push_back(tempPath);
				}
			}
			fclose(fp);
		}
	}
}

int Leap_pretrain_data_loader::getCount(){
	return dataList.size();
}

void Leap_pretrain_data_loader::getData(cv::Mat *left, cv::Mat *right, FilePath *data, int idx){
	FilePath targetPath = dataList.at(idx);

	*left = cv::imread(targetPath.left_path, CV_LOAD_IMAGE_GRAYSCALE);
	*right = cv::imread(targetPath.right_path, CV_LOAD_IMAGE_GRAYSCALE);

	*data = targetPath;
}