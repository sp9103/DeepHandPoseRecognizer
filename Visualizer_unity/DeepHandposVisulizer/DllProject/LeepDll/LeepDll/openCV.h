#pragma once
#include <opencv2\opencv.hpp>

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "x64\\opencv_ts300d.lib")
#pragma comment(lib, "x64\\opencv_world300d.lib")
#else
#pragma comment(lib, "x64\\opencv_ts300.lib")
#pragma comment(lib, "x64\\opencv_world300.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "x86\\opencv_ts300d.lib")
#pragma comment(lib, "x86\\opencv_world300d.lib")
#else
#pragma comment(lib, "x86\\opencv_ts300.lib")
#pragma comment(lib, "x86\\opencv_world300.lib")
#endif
#endif