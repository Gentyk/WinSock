#include <iostream>
#include <winsock2.h>

const int WinsockVer = 2;	// Минимальная версия WinSock

//предупреждения об ошибках и освобождение ресурсов
void processing_error(std::string s, int flag, SOCKET sock = NULL)
{
	std::cout << "ошибка:" << s.c_str() << "\n";
	getchar();
	switch (flag)
	{
	case 1: //в случае необходимости очистки без сокета
		WSACleanup();
		break;
	case 2://в случае необходимости очистки с сокетом
		if (sock != NULL)
			closesocket(sock);
		WSACleanup();
		break;
	case 3://в случае необходимости очистки только сокета
		if (sock != NULL)
			closesocket(sock);
		break;
	default:
		break;
	}
}

//проверка при подключения библиотеки 
int library_check()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(WinsockVer, 2), &wsaData) == 0)
	{
		// Проверяем если старшая версия больше или равна требуемой
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
	//проверяем подключение библиотеки
	if (library_check() == 1)
		return 1;

	//создаем сокет
	SOCKET servSocket;
	servSocket=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	//AF_INET - чтобы использовать TCP и UDP «адресное семейство».
	//SOCK_STREAM -для создания потокового socket’а
	//IPPROTO_TCP - для создания TCP socket'a
	if (servSocket == INVALID_SOCKET)
	{
		processing_error("при создании сокета", 1);
		return 1;
	}

	//задаем структуру
	sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);
	serv_addr.sin_addr.S_un.S_addr = INADDR_ANY; // используем адрес по умолчанию (т.е. любой)

	//bind(связываемся с портом)
	if (bind(servSocket, (sockaddr*)(&serv_addr), sizeof(serv_addr)) != 0)
	{
		// Действия в случае ошибки
		processing_error("bind", 2, servSocket);
		return 1;
	}

	//задаем адрес
	sockaddr_in kl_addr;

	//прослушивание
	printf("listen\n");
	if (listen(servSocket, SOMAXCONN) != 0)
	{
		// Действия в случае ошибки
		processing_error("listen", 2, servSocket);
		return 1;
	}

	//разрешение подключения
	sockaddr_in remote_addr;
	int remote_addr_len = (sizeof(remote_addr));
	SOCKET remote_sock;
	remote_sock = accept(servSocket, (sockaddr*)&remote_addr, &remote_addr_len);
	printf("accept\n");
	if (remote_sock == INVALID_SOCKET)
	{
		// Действия в случае ошибки
		processing_error("accept", 2, servSocket);
		return 1;
	}

	//получение данных
	char rs_buffer[1024];
	int bytesReceived = recv(remote_sock, rs_buffer, sizeof(rs_buffer), 0);
	if (bytesReceived == SOCKET_ERROR)
	{
		processing_error(" bytesReceived == SOCKET_ERROR", 2, servSocket);
		processing_error("", 3,remote_sock);
		return 1;
	}

	
	//передача сообщения
	if (send(remote_sock, rs_buffer, sizeof(rs_buffer), 0) == SOCKET_ERROR)
	{
		processing_error("при передаче", 2, remote_sock);
		processing_error("",3, servSocket);
		return 1;
	}
	std::cout << "соединение прошло успешно)\n";

	getchar();
	//освобождение ресурсов
	delete[] rs_buffer;
	closesocket(remote_sock);
	closesocket(servSocket);
	WSACleanup();
}