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

	printf("���� IP �Է� : ");
	scanf("%s", ip);
	printf("���� Port �Է� : ");
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
	printf("������ ���������� ����Ǿ����ϴ�.\n");
	char buf[BUFSIZE + 1];
	int len;

	int nums[4] = { 0 };

	for (int i = 0; i < 4; i++) {
		printf("%dst ������ �Է� : ", i + 1);
		scanf("%d", &nums[i]);
	}

	retval = send(listen_sock, (char*)nums, sizeof(nums), 0);

	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	printf("�۽� �޽��� : %d %d %d %d\n", nums[0], nums[1], nums[2], nums[3]);

	retval = recvn(listen_sock, buf, retval, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}

	int* recvNums = (int*)buf;

	printf("���� ���� : %d %d %d %d\n", recvNums[0], recvNums[1], recvNums[2], recvNums[3]);

	closesocket(listen_sock);
	WSACleanup();

	return 0;
}