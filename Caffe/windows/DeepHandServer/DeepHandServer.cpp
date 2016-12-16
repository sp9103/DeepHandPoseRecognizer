#include "DeepHandServer.h"


DeepHandServer::DeepHandServer()
{
}


DeepHandServer::~DeepHandServer()
{
}

void DeepHandServer::GetIPAddress(char *ip){
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	PHOSTENT hostinfo;
	char hostname[50];
	char ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int nError = gethostname(hostname, sizeof(hostname));
	if (nError == 0)
	{
		hostinfo = gethostbyname(hostname);
		// ip address 파악
		strcpy(ipaddr, inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[0]));
	}

	WSACleanup();

	strcpy(ip, ipaddr);
}

void DeepHandServer::Init(char *ip, int portNum){
	_portNum = portNum;

	if (ip == NULL){
		char temp[256];
		GetIPAddress(temp);
		strcpy(_IP, temp);
	}
	else{
		strcpy(_IP, ip);
	}

	// Load WinSocket 2.2 DLL
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup(), error");
	}

	if ((hServSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		ErrorHandling("socket() failed");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(_portNum);

	// 소켓에 주소 할당
	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) != 0)
	{
		ErrorHandling("bind() error");
	}

	szClntAddr = sizeof(clntAddr);
}

void DeepHandServer::DeInit(){

}

void DeepHandServer::ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

cv::Mat DeepHandServer::recvFromClient(){
	char buf[173060];
	/* Block until receive message from a client */
	if ((recvMsgSize = recvfrom(hServSock, buf, sizeof(recvData), 0,
		(struct sockaddr *) &clntAddr, &szClntAddr)) < 0)
		ErrorHandling("recvfrom() failed");

	memcpy(buf, &recvData, sizeof(recvData));

	//이미지 만들기
	cv::Mat retMat;
	if (recvData.channel == 3){
		retMat.create(240, 240, CV_8UC3);
		memcpy(retMat.ptr(0), recvData.val, sizeof(char) * 240 * 240 * 3);
	}
	else if (recvData.channel == 1){
		retMat.create(240, 240, CV_8UC1);
		memcpy(retMat.ptr(0), recvData.val, sizeof(char) * 240 * 240 * 1);
	}

	return retMat;
}

void DeepHandServer::sendAnswer(int answer){
	char data[4];
	memcpy(data, &answer, sizeof(int));
	if (sendto(hServSock, data, sizeof(int), 0, (struct sockaddr *) &szClntAddr, sizeof(szClntAddr)) != recvMsgSize)
		ErrorHandling("sendto() sent a different number of bytes than expected");
}