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
		// ip address ÆÄ¾Ç
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

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
	{
		ErrorHandling("WSAStartup() failed");
	}
	/* Create a best-effort datagram socket using UDP */
	if ((hSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		ErrorHandling("socket() failed");

	/* Construct the server address structure */
	memset(&servAddr, 0, sizeof(servAddr));    /* Zero out structure */
	servAddr.sin_family = AF_INET;                 /* Internet address family */
	servAddr.sin_addr.s_addr = inet_addr(_IP);  /* Server IP address */
	servAddr.sin_port = htons(_portNum);     /* Server port */

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
	char totalBuf[500 * FRAGMENT];
	ImgPacket sendData;
	sendData.channel = src.channels();
	GetIPAddress(sendData.senderIP);
	memcpy(sendData.val, src.ptr(0), 240 * 240 * src.channels());
	memcpy(totalBuf, &sendData, sizeof(ImgPacket));

	for (int i = 0; i < FRAGMENT; i++){
		memcpy(buf, &totalBuf[i * 500], sizeof(char) * 500);
		memcpy(&buf[500], &i, sizeof(int));

		//send data
		int sendLen;
		if ((sendLen = sendto(hSocket, buf, sizeof(char) * 504, 0, (struct sockaddr *)
			&servAddr, sizeof(servAddr))) != sizeof(char) * 504)
			ErrorHandling("sendto() sent a different number of bytes than expected");
	}

	char recvBuf[4];
	int fromSize;
	if (recvfrom(hSocket, recvBuf, sizeof(int), 0, (struct sockaddr *) &fromAddr,
		&fromSize) != sizeof(int))
		ErrorHandling("recvfrom() failed");

	if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
		ErrorHandling("Error: received a packet from unknown source.\n");
	}

	int returnVal;
	memcpy(&returnVal, recvBuf, sizeof(int));
	return returnVal;
}