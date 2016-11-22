#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct HandClass_{
	int isHand;
	float prob[14];
	float next[60];
	float prev[60];
}HandClass;

class ResHandClient
{
public:
	ResHandClient();
	~ResHandClient();

	void Init(char *ip, int portNum);
	void DeInit();

	int getData(float *dst);
	void Recv(HandClass *dst);

private:
	int _portNum;
	char _IP[256];

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	void ErrorHandling(char *message);
	void GetIPAddress(char *ip);
};

