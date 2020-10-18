#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512

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

int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (!received)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main() {
	int retval;
	char ip[512]{ 0 };
	int port = 0;

	printf("서버 IP 입력 : ");
	scanf("%s", ip);
	printf("서버 Port 입력 : ");
	scanf(" %d", &port);

	getchar();

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		return 1;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(port);
	retval = connect(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	printf("서버와 정상적으로 연결되었습니다.\n");
	char buf[BUFSIZE + 1];
	int len;

	int count = 0;
	printf("전송할 정수의 개수 입력 : ");
	scanf("%d", &count);

	int* nums = new int[count];

	for (int i = 0; i < count; i++) {
		printf("%dst 데이터 입력 : ", i + 1);
		scanf("%d", &nums[i]);
	}

	retval = send(listen_sock, (char*)nums, count * sizeof(int), 0);

	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	printf("송신 메시지 :");
	for (int i = 0; i < count; i++) {
		printf(" %d", nums[i]);
	}
	puts("");

	retval = recvn(listen_sock, buf, retval, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}

	int* recvNums = (int*)buf;

	printf("서버 응답 :");
	for (int i = 0; i < retval / sizeof(int); i++) {
		printf(" %d", recvNums[i]);
	}
	puts("");

	closesocket(listen_sock);
	WSACleanup();

	delete[] nums;

	return 0;
}