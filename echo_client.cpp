#include <iostream>
#include <winsock2.h>
#include <string>
#include <string.h>
#include "Ws2tcpip.h"

const int WinsockVer = 2;	// ����������� ������ WinSock
#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib,"wsock32.lib")
//#pragma warning(disable:4996)

//�������������� �� ������� � ������������ ��������
void processing_error(std::string s,int flag,SOCKET sock=NULL)
{
	std::cout << "������:" << s << "\n";
	getchar();
	switch (flag)
	{
	case 1: //� ������ ������������� ������� ��� ������
		WSACleanup();
		break;
	case 2://� ������ ������������� ������� � �������
		if (sock!=NULL)
			closesocket(sock);
		WSACleanup();
		break;
	case 3://� ������ ������������� ������� ������ ������
		if (sock != NULL)
			closesocket(sock);
		break;
	default:
		break;
	}
}

//�������� ��� ����������� ���������� 
int library_check()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(WinsockVer, 2), &wsaData) == 0)
	{
		// ��������� ���� ������� ������ ������ ��� ����� ���������
		if (LOBYTE(wsaData.wVersion) >= WinsockVer)
		{
			return 0;
		}
		else
		{
			printf("Could not find a usable version of Winsock.dll\n");
			WSACleanup();
			return 1;
		}
	}
	else
	{
		printf("WSAStartup failed:error while initializing the library\n");
		return 1;
	}
}

int main()
{
	system("chcp 1251");
	//��������� ����������� ����������
	if (library_check() != 0)
		return 1;

	//������� �����
	SOCKET klient_socket;
	klient_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//AF_INET - ����� ������������ TCP � UDP ��������� ���������.
	//SOCK_STREAM -��� �������� ���������� socket��
	//IPPROTO_TCP - ��� �������� TCP socket'a
	if (klient_socket == INVALID_SOCKET)
	{
		processing_error("������� �������� ������", 1);
		return 1;
	}

	//������ �����
	sockaddr_in kl_addr;
	// ������������� �������� ���������
	kl_addr.sin_family = AF_INET;
	// ����������� ����� ����� 80 � ������� ������� ������ 
	kl_addr.sin_port = htons(80);	
	std::string sss = "127.0.0.1";
	inet_pton(AF_INET, sss.c_str(), &kl_addr.sin_addr);

	// ������������ � �������
	if (connect(klient_socket,(sockaddr*)(&kl_addr), sizeof(kl_addr)) != 0)
	{
		processing_error("��� ����������� � �������", 2, klient_socket);
		return 1;
	}


	//���� ���������
	std::cout << "���������� � �������� �����������\n";
	std::cout << "������� ���������(������ �� 1024 ��������):\n";
	std::string example;
	std::cin >> example;
	while (example.length() >= 1024)
	{
		std::cout << "��������� ����� ����� ����� ������ 1024 ��������. ���������� ��� ���:\n";
		std::cin >> example;
	}
	char s_buffer[1024];
	strncpy_s(s_buffer,1024, example.c_str(),example.length());
	std::cout << s_buffer<<"\n";
	//�������� ���������
	if (send(klient_socket, s_buffer, sizeof(s_buffer), 0) == SOCKET_ERROR)
	{
		processing_error("��� ��������", 2, klient_socket);
		return 1;
	}
	std::cout << "�������� ������ �������\n";

	//����� ���������
	char r_buffer[1024];
	int bytesReceived = recv(klient_socket, r_buffer, sizeof(r_buffer), 1);
	if (bytesReceived == SOCKET_ERROR)
	{
		processing_error(" bytesReceived == SOCKET_ERROR", 2, klient_socket);	
		return 1;
	}
	std::cout << "������� ����������: "<<r_buffer<<"\n";

	getchar();
	//������� ��������
	
	delete[] r_buffer;
	closesocket(klient_socket);
	WSACleanup();
}