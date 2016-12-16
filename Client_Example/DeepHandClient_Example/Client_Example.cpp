#include <stdio.h>

#include "DeepHandClient.h"

int main(){
	DeepHandClient client;
	client.Init(NULL, DEFAULT_PORT);

	cv::Mat img = cv::imread("1_f_177.jpg");
	int gestIdx = client.SendAndRecognition(img);
	printf("idx : %d\n", gestIdx);
	img = cv::imread("1_f_211.jpg");
	gestIdx = client.SendAndRecognition(img);
	printf("idx : %d\n", gestIdx);

	client.DeInit();

	return 0;
}

//#include <stdio.h>      /* for printf(), fprintf() */
//#include <winsock.h>    /* for socket(),... */
//#include <stdlib.h>     /* for exit() */
//#define ECHOMAX 255     /* Longest string to echo */
//
//#pragma comment(lib, "ws2_32.lib")
//void main(int argc, char *argv[])
//{
//	int sock;                        /* Socket descriptor */
//	struct sockaddr_in echoServAddr; /* Echo server address */
//	struct sockaddr_in fromAddr;     /* Source address of echo */
//	unsigned short echoServPort;     /* Echo server port */
//	int fromSize;           /* In-out of address size for recvfrom() */
//	char *servIP;                    /* IP address of server */
//	char *echoString;                /* String to send to echo server */
//	char echoBuffer[ECHOMAX];        /* Buffer for echo string */
//	int echoStringLen;               /* Length of string to echo */
//	int respStringLen;               /* Length of response string */
//	WSADATA wsaData;                 /* Structure for WinSock setup communication */
//
//	servIP = "166.104.142.22";           /* first arg: server IP address (dotted quad)*/
//	echoString = "TEST";       /* second arg: string to echo */
//	if ((echoStringLen = strlen(echoString) + 1) > ECHOMAX)  /* Check input length */
//		printf("Echo word too long");
//	echoServPort = atoi("2252");  /* Use given port, if any */
//
//	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
//	{
//		fprintf(stderr, "WSAStartup() failed");
//		exit(1);
//	}
//	/* Create a best-effort datagram socket using UDP */
//	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
//		printf("socket() failed");
//	/* Construct the server address structure */
//	memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
//	echoServAddr.sin_family = AF_INET;                 /* Internet address family */
//	echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
//	echoServAddr.sin_port = htons(echoServPort);     /* Server port */
//	/* Send the string, including the null terminator, to the server */
//	if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)
//		&echoServAddr, sizeof(echoServAddr)) != echoStringLen)
//		printf("sendto() sent a different number of bytes than expected");
//
//	/* Recv a response */
//
//	fromSize = sizeof(fromAddr);
//	if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr,
//		&fromSize)) != echoStringLen)
//		printf("recvfrom() failed");
//	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
//	{
//		fprintf(stderr, "Error: received a packet from unknown source.\n");
//		exit(1);
//	}
//	if (echoBuffer[respStringLen - 1])  /* Do not printf unless it is terminated */
//		printf("Received an unterminated string\n");
//	else
//		printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */
//	closesocket(sock);
//	WSACleanup();  /* Cleanup Winsock */
//	exit(0);
//}