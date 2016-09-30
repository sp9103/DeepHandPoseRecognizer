#include <stdio.h>
#include <string>
#include <Windows.h>

#define EXPORT_API __declspec(dllexport)

using namespace std;

extern "C"{

	void EXPORT_API TEST(){
		MessageBox(NULL, L"Client DLL Test Successs", L"TEST", MB_OK);
	}

}