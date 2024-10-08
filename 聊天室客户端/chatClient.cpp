//#include<iostream>
//#include<stdio.h>
//#include<winsock.h>
//#include<graphics.h>
//#pragma comment(lib,"ws2_32.lib")
//using namespace std;
//
//SOCKET serverSocket;
//
//void yonghu() {
//	//5.通信 接受用户输入并发送给服务器
//	char buff[256];
//	printf("请在下面输入\n");
//	while (1) {
//		printf(">>:");
//		scanf_s("%s", buff, sizeof(buff));
//		send(serverSocket, buff, strlen(buff), NULL);
//	}
//}
//
//int main() {
//	//initgraph(300, 600,NULL);
//	initwindow(300, 600);
//	//1.确定协议版本信息
//	WSADATA wsaData;
//	WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { //防御性编程
//		printf("确定协议版本信息错误：%d\n", GetLastError());
//		return -1;
//	}
//	printf("确定协议版本信息成功！\n");
//	//2.创建socket
//	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (SOCKET_ERROR == serverSocket) {  //防御性编程
//		//清除协议版本信息
//		WSACleanup();
//		printf("创建socket错误：%d\n", GetLastError());
//		return -1;
//	}
//	printf("创建socket成功！\n");
//	//3.确定服务器协议地址簇
//	SOCKADDR_IN sAddr = { 0 };
//	sAddr.sin_family = AF_INET;//socket函数的第一个参数
//	sAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.183");//服务器IP地址
//	sAddr.sin_port = htons(8989);//端口号
//	//4.连接服务器
//	int r = connect(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
//	if (SOCKET_ERROR == r) {  //防御性编程
//		//关闭socket
//		closesocket(serverSocket);
//		//清除协议版本信息
//		WSACleanup();
//		printf("连接服务器错误：%d\n", GetLastError());
//		return -1;
//	}
//	printf("连接服务器成功！\n");
//	
//	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)yonghu, NULL, NULL, NULL);
//	//同时接受服务器发来的数据并显示
//	char recvBuff[256];
//	int n = 0;
//	while (1) {
//		r = recv(serverSocket, recvBuff, 255, NULL);
//		if (r > 0) {
//			recvBuff[r] = 0;
//			COLOR(10,20,30);
//			outtextxy(3, n * 20, recvBuff);
//		}
//		n++;
//	}
//	
//	//8.关闭socket
//	closesocket(serverSocket);
//	//9.清除协议版本信息
//	WSACleanup();
//	return 0;
//}
//
//

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

SOCKET serverSocket;
#define MAX_MESSAGES 1000
char messages[MAX_MESSAGES][256];
int messageCount = 0;
CRITICAL_SECTION cs;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    switch (uMsg)
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        // 设置文本颜色
        SetTextColor(hdc, RGB(10, 20, 30));
        // 绘制接收到的消息
        EnterCriticalSection(&cs);
        for (int i = 0; i < messageCount; i++)
        {
            TextOutA(hdc, 3, i * 20, messages[i], strlen(messages[i]));
        }
        LeaveCriticalSection(&cs);
        EndPaint(hwnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

DWORD WINAPI yonghu(LPVOID lpParam)
{
    // 通信：接受用户输入并发送给服务器
    char buff[256];
    printf("请在下面输入\n");
    while (1) {
        printf(">>:");
        scanf_s("%s", buff, (unsigned int)sizeof(buff));
        send(serverSocket, buff, strlen(buff), NULL);
    }
    return 0;
}

DWORD WINAPI recvData(LPVOID lpParam)
{
    HWND hwnd = (HWND)lpParam;
    char recvBuff[256];
    int r = 0;
    while (1) {
        r = recv(serverSocket, recvBuff, 255, NULL);
        if (r > 0) {
            recvBuff[r] = 0;
            // 添加消息到数组
            EnterCriticalSection(&cs);
            if (messageCount < MAX_MESSAGES) {
                strcpy_s(messages[messageCount], recvBuff);
                messageCount++;
                // 使窗口无效以触发重绘
                InvalidateRect(hwnd, NULL, TRUE);
            }
            LeaveCriticalSection(&cs);
        }
    }
    return 0;
}

int main()
{
    // 初始化 WinSock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { // 防御性编程
        printf("确定协议版本信息错误：%d\n", GetLastError());
        return -1;
    }
    printf("确定协议版本信息成功！\n");

    // 创建 socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == serverSocket) {  // 防御性编程
        WSACleanup();
        printf("创建 socket 错误：%d\n", GetLastError());
        return -1;
    }
    printf("创建 socket 成功！\n");

    // 确定服务器协议地址簇
    SOCKADDR_IN sAddr = { 0 };
    sAddr.sin_family = AF_INET;// socket 函数的第一个参数
    sAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.183");// 服务器 IP 地址
    sAddr.sin_port = htons(8989);// 端口号

    // 连接服务器
    int r = connect(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
    if (SOCKET_ERROR == r) {  // 防御性编程
        closesocket(serverSocket);
        WSACleanup();
        printf("连接服务器错误：%d\n", GetLastError());
        return -1;
    }
    printf("连接服务器成功！\n");

    // 初始化窗口
    const char CLASS_NAME[] = "MyWindowClass";
    WNDCLASSA wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassA(&wc)) {
        printf("注册窗口类失败：%d\n", GetLastError());
        return -1;
    }

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "0",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 600,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (hwnd == NULL)
    {
        printf("创建窗口失败：%d\n", GetLastError());
        return -1;
    }

    ShowWindow(hwnd, SW_SHOW);

    // 初始化临界区
    InitializeCriticalSection(&cs);

    // 创建用户输入线程
    HANDLE hThreadUser = CreateThread(NULL, 0, yonghu, NULL, 0, NULL);
    if (hThreadUser == NULL) {
        printf("创建用户线程失败：%d\n", GetLastError());
        return -1;
    }

    // 创建接收数据线程
    HANDLE hThreadRecv = CreateThread(NULL, 0, recvData, hwnd, 0, NULL);
    if (hThreadRecv == NULL) {
        printf("创建接收线程失败：%d\n", GetLastError());
        return -1;
    }

    // 运行消息循环
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 清理
    DeleteCriticalSection(&cs);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

