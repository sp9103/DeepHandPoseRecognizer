#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <opencv2\opencv.hpp>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 2252

typedef struct ImgPacket_{
	int channel;
	unsigned char val[240 * 240 * 3];
	char senderIP[256];
}ImgPacket;

class DeepHandServer
{
public:
	DeepHandServer();
	~DeepHandServer();

	void GetIPAddress(char *ip);
	void Init(char *ip, int portNum);
	void DeInit();

	cv::Mat recvFromClient();
	void sendAnswer(int answer);

private:
	unsigned short _portNum;
	char _IP[256];
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	SOCKADDR_IN clntAddr;
	int szClntAddr;
	int recvMsgSize;                 /* Size of received message */

	ImgPacket recvData;

	void ErrorHandling(char *message);
};

