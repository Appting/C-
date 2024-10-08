#include<iostream>
#include<winsock.h>
#include<windows.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define NUM 1024
SOCKET clientSocket[NUM]; //最大并发量

int clientCount;

void tongxin(int idx) {
	//7.通信 接受客户端发来的数据并显示
	char buff[256];
	int r;
	char temp[256];
	while (1) {
		r = recv(clientSocket[idx], buff, 255, NULL);
		if (r > 0) {
			buff[r] = 0; //添加结束符号 '\0'
			printf(">>:%s\n", buff);
			memset(temp, 0, 256);//清空数组
			sprintf_s(temp,sizeof(temp), "%d:%s", idx, buff);
			for (int i = 0;i < clientCount;i++) { //广播
				send(clientSocket[i], temp, strlen(temp), NULL);
			}
		}
	}
}

int main() {
	clientCount = 0;
	//1.确定协议版本信息
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { //防御性编程
		printf("确定协议版本信息错误：%d\n", GetLastError());
		return -1;
	}
	printf("确定协议版本信息成功！\n");
	//2.创建socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {  //防御性编程
		//清除协议版本信息
		WSACleanup();
		printf("创建socket错误：%d\n", GetLastError());
		return -1;
	}
	printf("创建socket成功！\n");
	//3.确定服务器协议地址簇
	SOCKADDR_IN sAddr = { 0 };
	sAddr.sin_family = AF_INET;//socket函数的第一个参数
	sAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.183");//服务器IP地址
	sAddr.sin_port = htons(8989);//端口号
	//4.绑定
	int r = bind(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
	if (SOCKET_ERROR == r) {  //防御性编程
		//关闭socket
		closesocket(serverSocket);
		//清除协议版本信息
		WSACleanup();
		printf("绑定错误：%d\n", GetLastError());
		return -1;
	}
	printf("绑定成功！\n");
	//5.监听
	r = listen(serverSocket, 10);
	if (SOCKET_ERROR == r) {  //防御性编程
		//关闭socket
		closesocket(serverSocket);
		//清除协议版本信息
		WSACleanup();
		printf("监听错误：%d\n", GetLastError());
		return -1;
	}
	printf("监听成功！\n");
	//6.接受连接
	for (int i = 0;i < NUM;i++) {
		clientSocket[i] = accept(serverSocket, NULL, NULL);
		if (SOCKET_ERROR == clientSocket[i]) {  //防御性编程
			printf("网络崩溃！\n");
			//关闭socket
			closesocket(serverSocket);
			//清除协议版本信息
			WSACleanup();
			return -1;
		}
		printf("客户端已连接服务器！\n");
		clientCount++;
		//并发 多线程
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)tongxin, (LPVOID)i, NULL, NULL);
	}
	
	
	//8.关闭socket
	closesocket(serverSocket);
	//9.清除协议版本信息
	WSACleanup();
	return 0;
}