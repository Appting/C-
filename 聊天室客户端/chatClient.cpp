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
//	//5.ͨ�� �����û����벢���͸�������
//	char buff[256];
//	printf("������������\n");
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
//	//1.ȷ��Э��汾��Ϣ
//	WSADATA wsaData;
//	WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { //�����Ա��
//		printf("ȷ��Э��汾��Ϣ����%d\n", GetLastError());
//		return -1;
//	}
//	printf("ȷ��Э��汾��Ϣ�ɹ���\n");
//	//2.����socket
//	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (SOCKET_ERROR == serverSocket) {  //�����Ա��
//		//���Э��汾��Ϣ
//		WSACleanup();
//		printf("����socket����%d\n", GetLastError());
//		return -1;
//	}
//	printf("����socket�ɹ���\n");
//	//3.ȷ��������Э���ַ��
//	SOCKADDR_IN sAddr = { 0 };
//	sAddr.sin_family = AF_INET;//socket�����ĵ�һ������
//	sAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.183");//������IP��ַ
//	sAddr.sin_port = htons(8989);//�˿ں�
//	//4.���ӷ�����
//	int r = connect(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
//	if (SOCKET_ERROR == r) {  //�����Ա��
//		//�ر�socket
//		closesocket(serverSocket);
//		//���Э��汾��Ϣ
//		WSACleanup();
//		printf("���ӷ���������%d\n", GetLastError());
//		return -1;
//	}
//	printf("���ӷ������ɹ���\n");
//	
//	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)yonghu, NULL, NULL, NULL);
//	//ͬʱ���ܷ��������������ݲ���ʾ
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
//	//8.�ر�socket
//	closesocket(serverSocket);
//	//9.���Э��汾��Ϣ
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
        // �����ı���ɫ
        SetTextColor(hdc, RGB(10, 20, 30));
        // ���ƽ��յ�����Ϣ
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
    // ͨ�ţ������û����벢���͸�������
    char buff[256];
    printf("������������\n");
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
            // �����Ϣ������
            EnterCriticalSection(&cs);
            if (messageCount < MAX_MESSAGES) {
                strcpy_s(messages[messageCount], recvBuff);
                messageCount++;
                // ʹ������Ч�Դ����ػ�
                InvalidateRect(hwnd, NULL, TRUE);
            }
            LeaveCriticalSection(&cs);
        }
    }
    return 0;
}

int main()
{
    // ��ʼ�� WinSock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { // �����Ա��
        printf("ȷ��Э��汾��Ϣ����%d\n", GetLastError());
        return -1;
    }
    printf("ȷ��Э��汾��Ϣ�ɹ���\n");

    // ���� socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == serverSocket) {  // �����Ա��
        WSACleanup();
        printf("���� socket ����%d\n", GetLastError());
        return -1;
    }
    printf("���� socket �ɹ���\n");

    // ȷ��������Э���ַ��
    SOCKADDR_IN sAddr = { 0 };
    sAddr.sin_family = AF_INET;// socket �����ĵ�һ������
    sAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.183");// ������ IP ��ַ
    sAddr.sin_port = htons(8989);// �˿ں�

    // ���ӷ�����
    int r = connect(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
    if (SOCKET_ERROR == r) {  // �����Ա��
        closesocket(serverSocket);
        WSACleanup();
        printf("���ӷ���������%d\n", GetLastError());
        return -1;
    }
    printf("���ӷ������ɹ���\n");

    // ��ʼ������
    const char CLASS_NAME[] = "MyWindowClass";
    WNDCLASSA wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassA(&wc)) {
        printf("ע�ᴰ����ʧ�ܣ�%d\n", GetLastError());
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
        printf("��������ʧ�ܣ�%d\n", GetLastError());
        return -1;
    }

    ShowWindow(hwnd, SW_SHOW);

    // ��ʼ���ٽ���
    InitializeCriticalSection(&cs);

    // �����û������߳�
    HANDLE hThreadUser = CreateThread(NULL, 0, yonghu, NULL, 0, NULL);
    if (hThreadUser == NULL) {
        printf("�����û��߳�ʧ�ܣ�%d\n", GetLastError());
        return -1;
    }

    // �������������߳�
    HANDLE hThreadRecv = CreateThread(NULL, 0, recvData, hwnd, 0, NULL);
    if (hThreadRecv == NULL) {
        printf("���������߳�ʧ�ܣ�%d\n", GetLastError());
        return -1;
    }

    // ������Ϣѭ��
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ����
    DeleteCriticalSection(&cs);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

