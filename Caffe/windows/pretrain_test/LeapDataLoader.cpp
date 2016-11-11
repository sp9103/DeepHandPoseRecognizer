#include "LeapDataLoader.h"


LeapDataLoader::LeapDataLoader()
{
}


LeapDataLoader::~LeapDataLoader()
{
}

void LeapDataLoader::LoadDataAll(char *datapath){
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
			HANDLE dFind = FindFirstFile(cDir, &cffd);
			while (FindNextFile(dFind, &cffd) != 0){
				//Tchar to char
				size_t clen;
				StringCchLength(cDir, MAX_PATH, &clen);
				WideCharToMultiByte(CP_ACP, 0, cffd.cFileName, clen, ccDataName, 256, NULL, NULL);
				printf("File : %s load.\n", ccDataName);

				if (ccDataName[0] != '.'){
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
							tempPath.id = atoi(ccFileName);
							if (tempPath.id < 0 || tempPath.id > 13)
								printf("Error Label create\n");
							char file_left[256], file_right[256];
							int img_idx = targetData.data_counter;
							sprintf(file_left, "%s\\%s\\%s\\%d_f_%d.jpg", datapath, ccFileName, ccDataName, 0, img_idx);
							sprintf(file_right, "%s\\%s\\%s\\%d_f_%d.jpg", datapath, ccFileName, ccDataName, 1, img_idx);
							tempPath.left_path = file_left;
							tempPath.right_path = file_right;

							dataList.push_back(tempPath);
						}
					}
					fclose(fp);
				}
			}
		}
	}
}

int LeapDataLoader::getCount(){
	return dataList.size();
}

void LeapDataLoader::getData(cv::Mat *left, cv::Mat *right, int *label, int idx){
	FilePath temp = dataList.at(idx);
	*left = cv::imread(temp.left_path, 0);
	*right = cv::imread(temp.right_path, 0);
	*label = temp.id;
}