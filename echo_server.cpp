#include <iostream>
#include <winsock2.h>

const int WinsockVer = 2;	// ����������� ������ WinSock

//�������������� �� ������� � ������������ ��������
void processing_error(std::string s, int flag, SOCKET sock = NULL)
{
	std::cout << "������:" << s.c_str() << "\n";
	getchar();
	switch (flag)
	{
	case 1: //� ������ ������������� ������� ��� ������
		WSACleanup();
		break;
	case 2://� ������ ������������� ������� � �������
		if (sock != NULL)
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
	if (library_check() == 1)
		return 1;

	//������� �����
	SOCKET servSocket;
	servSocket=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	//AF_INET - ����� ������������ TCP � UDP ��������� ���������.
	//SOCK_STREAM -��� �������� ���������� socket��
	//IPPROTO_TCP - ��� �������� TCP socket'a
	if (servSocket == INVALID_SOCKET)
	{
		processing_error("��� �������� ������", 1);
		return 1;
	}

	//������ ���������
	sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);
	serv_addr.sin_addr.S_un.S_addr = INADDR_ANY; // ���������� ����� �� ��������� (�.�. �����)

	//bind(����������� � ������)
	if (bind(servSocket, (sockaddr*)(&serv_addr), sizeof(serv_addr)) != 0)
	{
		// �������� � ������ ������
		processing_error("bind", 2, servSocket);
		return 1;
	}

	//������ �����
	sockaddr_in kl_addr;

	//�������������
	printf("listen\n");
	if (listen(servSocket, SOMAXCONN) != 0)
	{
		// �������� � ������ ������
		processing_error("listen", 2, servSocket);
		return 1;
	}

	//���������� �����������
	sockaddr_in remote_addr;
	int remote_addr_len = (sizeof(remote_addr));
	SOCKET remote_sock;
	remote_sock = accept(servSocket, (sockaddr*)&remote_addr, &remote_addr_len);
	printf("accept\n");
	if (remote_sock == INVALID_SOCKET)
	{
		// �������� � ������ ������
		processing_error("accept", 2, servSocket);
		return 1;
	}

	//��������� ������
	char rs_buffer[1024];
	int bytesReceived = recv(remote_sock, rs_buffer, sizeof(rs_buffer), 0);
	if (bytesReceived == SOCKET_ERROR)
	{
		processing_error(" bytesReceived == SOCKET_ERROR", 2, servSocket);
		processing_error("", 3,remote_sock);
		return 1;
	}

	
	//�������� ���������
	if (send(remote_sock, rs_buffer, sizeof(rs_buffer), 0) == SOCKET_ERROR)
	{
		processing_error("��� ��������", 2, remote_sock);
		processing_error("",3, servSocket);
		return 1;
	}
	std::cout << "���������� ������ �������)\n";

	getchar();
	//������������ ��������
	delete[] rs_buffer;
	closesocket(remote_sock);
	closesocket(servSocket);
	WSACleanup();
}