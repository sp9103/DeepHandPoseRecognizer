#include "ResHandServer.h"


ResHandServer::ResHandServer()
{
}


ResHandServer::~ResHandServer()
{
}

void ResHandServer::GetIPAddress(char *ip){
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

void ResHandServer::Init(char *ip, int portNum){
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

	// 연결 요청 수락
	int szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
	if (hClntSock == INVALID_SOCKET)
	{
		ErrorHandling("accept() error");
	}
}

void ResHandServer::ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void ResHandServer::DeInit(){
	// 연결 종료
	closesocket(hClntSock);
	WSACleanup();
}

void ResHandServer::sendVec(float *data){
	char buf[256];
	memcpy(buf, data, sizeof(float) * 14);
	send(hClntSock, buf, sizeof(float) * 14, 0);
}