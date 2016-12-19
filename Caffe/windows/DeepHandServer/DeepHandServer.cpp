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

	// Load Winsock 2.2 DLL
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
	}

	// 서버 소켓 생성
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
	{
		ErrorHandling("socket() error");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(_portNum);

	// 소켓에 주소 할당
	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind() error");
	}

	// 연결 요청 대기 상태
	if (listen(hServSock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen() error");
	}

	// 연결 요청 수락 - block until client access
	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
	if (hClntSock == INVALID_SOCKET)
	{
		ErrorHandling("accept() error");
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

	char totalBuf[200000];
	char buf[504];

	// 데이터 수신 
	int dataLen = recv(hClntSock, totalBuf, sizeof(char) * 200000, 0);
	if (dataLen == -1)
	{
		ErrorHandling("read() error");
	}

	memcpy(&recvData, totalBuf, sizeof(ImgPacket));

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
	// 데이터 전송
	send(hClntSock, data, sizeof(char) * 4, 0);
}