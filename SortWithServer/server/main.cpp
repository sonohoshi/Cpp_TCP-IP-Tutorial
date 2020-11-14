#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 10000
#define BUFSIZE 512

int static compare(const void* first, const void* second)
{
	if (*(int*)first > * (int*)second)
		return 1;
	else if (*(int*)first < *(int*)second)
		return -1;
	else
		return 0;
}

void err_quit(const char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char* argv[]) {
	int isEnded = 0;
	printf("클라이언트 접속 대기 중 !!\n");
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		return 1;
	}

	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	
	addrlen = sizeof(clientaddr);
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET) {
		err_display("accept()");
	}
	// 클라이언트 정보 출력
	printf("클라이언트 연결 완료\n");

	retval = recv(client_sock, buf, BUFSIZE, 0);
	
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}
	int* nums = (int*)buf;
	printf("수신 메시지 :");
	for (int i = 0; i < retval / sizeof(int); i++) {
		printf(" %d", nums[i]);
	}
	puts("");

	qsort(nums, retval / sizeof(int), sizeof(int), compare);
	printf("정렬 완료!!\n");
	retval = send(client_sock, (char*)nums, retval, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
	printf("응답 메시지 :");
	for (int i = 0; i < retval / sizeof(int); i++) {
		printf(" %d", nums[i]);
	}
	puts("");

	closesocket(client_sock);
	//printf("end client ip : %s, port : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	

	closesocket(listen_sock);
	WSACleanup();

	printf("프로그램이 종료되었습니다.\n");
	return 0;
}