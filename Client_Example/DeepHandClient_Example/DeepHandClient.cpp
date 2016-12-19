#include "DeepHandClient.h"


DeepHandClient::DeepHandClient()
{
}


DeepHandClient::~DeepHandClient()
{
}

void DeepHandClient::GetIPAddress(char *ip){
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

void DeepHandClient::Init(char *ip, int portNum){
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
	// 서버 접속을 위한 소켓 생성
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		ErrorHandling("hSocketet(), error");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(_IP);
	servAddr.sin_port = htons(_portNum);

	// 서버로 연결 요청
	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("Connect() error");
	}


}

void DeepHandClient::DeInit(){
	closesocket(hSocket);
	WSACleanup();  /* Cleanup Winsock */
}

void DeepHandClient::ErrorHandling(char *message)
{
	wchar_t text1[100];
	mbstowcs(text1, message, strlen(message) + 1);
	LPCWSTR test = text1;

	MessageBox(NULL, test, L"DLL Error", MB_OK);
}

int DeepHandClient::SendAndRecognition(cv::Mat src){
	if (src.rows != 240 || src.cols != 240)
		cv::resize(src, src, cv::Size(240, 240));

	char buf[504];
	char totalBuf[200000];
	char recvBuf[4];
	int fromSize, recvSize;;
	int dataLen;

	ImgPacket sendData;
	sendData.channel = src.channels();
	GetIPAddress(sendData.senderIP);
	memcpy(sendData.val, src.ptr(0), 240 * 240 * src.channels());
	memcpy(totalBuf, &sendData, sizeof(ImgPacket));

	send(hSocket, totalBuf, sizeof(char) * 200000, 0);

	dataLen = recv(hSocket, recvBuf, sizeof(int), 0);
	if (dataLen == -1)
	{
		ErrorHandling("read() error");
	}

	int returnVal;
	memcpy(&returnVal, recvBuf, sizeof(int));
	return returnVal;
}