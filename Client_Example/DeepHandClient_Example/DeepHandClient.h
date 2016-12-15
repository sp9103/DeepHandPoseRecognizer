#include <opencv2\opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define SERVER_IP 127.0.0.1

#pragma comment(lib, "ws2_32.lib")

typedef struct ImgPacket_{
	int channel;
	unsigned char val[240 * 240 * 3];
	char senderIP[256];
}ImgPacket;

class DeepHandClient
{
public:
	DeepHandClient();
	~DeepHandClient();

	void GetIPAddress(char *ip);
	void Init(char *ip, int portNum);
	void DeInit();

	int SendAndRecognition(cv::Mat src);

private:
	int _portNum;
	char _IP[256];
	WSADATA wsaData;                 /* Structure for WinSock setup communication */
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	void ErrorHandling(char *message);
};

