//#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct HandClass_{
	int isHand;
	float prob[14];
	float next[60];
	float prev[60];
}HandClass;

class ResHandServer
{
public:
	ResHandServer();
	~ResHandServer();

	void GetIPAddress(char *ip);
	void Init(char *ip, int portNum);
	void DeInit();
	void sendVec(HandClass src);

private:
	int _portNum;
	char _IP[256];
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;

	void ErrorHandling(char *message);
};

