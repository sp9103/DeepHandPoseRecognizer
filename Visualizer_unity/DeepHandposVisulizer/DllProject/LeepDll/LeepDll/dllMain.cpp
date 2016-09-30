#include <stdio.h>
#include <string>
#include <Windows.h>

#define EXPORT_API __declspec(dllexport)
#define GEST_COUNT 10

using namespace std;

extern "C"{

	void EXPORT_API TEST(){
		MessageBox(NULL, L"Client DLL Test Successs", L"TEST", MB_OK);
	}

	void EXPORT_API Init(){

	}

	void EXPORT_API CalcProbabilty(float prob[]){
		//TO-DO

		for (int i = 0; i < GEST_COUNT; i++){
			if (i == 3)
				prob[i] = 1.0f;
			else
				prob[i] = 0.0f;
		}
	}

}