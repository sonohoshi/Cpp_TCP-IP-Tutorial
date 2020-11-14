#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 10000
#define BUFSIZE 512

typedef struct Product {
	int productType;
	double area;
}Product;

double entryLeft = 10000;
double deluxeLeft = 15000;

int cal(Product prod) {
	if (!prod.productType && prod.area <= entryLeft) {
		if (prod.area <= 50) {
			entryLeft -= prod.area;
			return prod.area * 30000;
		}
		else if (prod.area <= 1000) {
			entryLeft -= prod.area;
			return prod.area * 25000;
		}
		else if (prod.area <= 10000) {
			entryLeft -= prod.area;
			return prod.area * 20000;
		}
		else {
			return 0;
		}
	}
	else if(prod.productType && prod.area <= deluxeLeft){
		if (prod.area <= 50) {
			deluxeLeft -= prod.area;
			return prod.area * 60000;
		}
		else if (prod.area <= 1000) {
			deluxeLeft -= prod.area;
			return prod.area * 50000;
		}
		else if (prod.area <= 10000) {
			deluxeLeft -= prod.area;
			return prod.area * 40000;
		}
		else {
			return 0;
		}
	}
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
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		return 1;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	puts("클라이언트 접속 대기 중 !!");

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		
		puts("클라이언트 연결 완료");

		while (1) {
			Product prod;
			retval = recv(client_sock, buf, BUFSIZE, 0);

			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) break;

			prod = *((Product*)buf);
			printf("수신 메시지 : 제품(%s), 면적(%lf)\n", prod.productType ? "고급형" : "보급형", prod.area);

			int res = cal(prod);
			sprintf(buf, "%d", res);

			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			printf("응답 메시지 : %s\n", buf);
		}

		closesocket(client_sock);
		printf("end client ip : %s, port : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	closesocket(listen_sock);
	WSACleanup();
	return 0;
}