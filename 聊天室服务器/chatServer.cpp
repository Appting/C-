#include<iostream>
#include<winsock.h>
#include<windows.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define NUM 1024
SOCKET clientSocket[NUM]; //��󲢷���

int clientCount;

void tongxin(int idx) {
	//7.ͨ�� ���ܿͻ��˷��������ݲ���ʾ
	char buff[256];
	int r;
	char temp[256];
	while (1) {
		r = recv(clientSocket[idx], buff, 255, NULL);
		if (r > 0) {
			buff[r] = 0; //��ӽ������� '\0'
			printf(">>:%s\n", buff);
			memset(temp, 0, 256);//�������
			sprintf_s(temp,sizeof(temp), "%d:%s", idx, buff);
			for (int i = 0;i < clientCount;i++) { //�㲥
				send(clientSocket[i], temp, strlen(temp), NULL);
			}
		}
	}
}

int main() {
	clientCount = 0;
	//1.ȷ��Э��汾��Ϣ
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { //�����Ա��
		printf("ȷ��Э��汾��Ϣ����%d\n", GetLastError());
		return -1;
	}
	printf("ȷ��Э��汾��Ϣ�ɹ���\n");
	//2.����socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {  //�����Ա��
		//���Э��汾��Ϣ
		WSACleanup();
		printf("����socket����%d\n", GetLastError());
		return -1;
	}
	printf("����socket�ɹ���\n");
	//3.ȷ��������Э���ַ��
	SOCKADDR_IN sAddr = { 0 };
	sAddr.sin_family = AF_INET;//socket�����ĵ�һ������
	sAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.183");//������IP��ַ
	sAddr.sin_port = htons(8989);//�˿ں�
	//4.��
	int r = bind(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
	if (SOCKET_ERROR == r) {  //�����Ա��
		//�ر�socket
		closesocket(serverSocket);
		//���Э��汾��Ϣ
		WSACleanup();
		printf("�󶨴���%d\n", GetLastError());
		return -1;
	}
	printf("�󶨳ɹ���\n");
	//5.����
	r = listen(serverSocket, 10);
	if (SOCKET_ERROR == r) {  //�����Ա��
		//�ر�socket
		closesocket(serverSocket);
		//���Э��汾��Ϣ
		WSACleanup();
		printf("��������%d\n", GetLastError());
		return -1;
	}
	printf("�����ɹ���\n");
	//6.��������
	for (int i = 0;i < NUM;i++) {
		clientSocket[i] = accept(serverSocket, NULL, NULL);
		if (SOCKET_ERROR == clientSocket[i]) {  //�����Ա��
			printf("���������\n");
			//�ر�socket
			closesocket(serverSocket);
			//���Э��汾��Ϣ
			WSACleanup();
			return -1;
		}
		printf("�ͻ��������ӷ�������\n");
		clientCount++;
		//���� ���߳�
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)tongxin, (LPVOID)i, NULL, NULL);
	}
	
	
	//8.�ر�socket
	closesocket(serverSocket);
	//9.���Э��汾��Ϣ
	WSACleanup();
	return 0;
}