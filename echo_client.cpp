#include <iostream>
#include <winsock2.h>
#include <string>
#include <string.h>
#include "Ws2tcpip.h"

const int WinsockVer = 2;	// Минимальная версия WinSock
#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib,"wsock32.lib")
//#pragma warning(disable:4996)

//предупреждения об ошибках и освобождение ресурсов
void processing_error(std::string s,int flag,SOCKET sock=NULL)
{
	std::cout << "ошибка:" << s << "\n";
	getchar();
	switch (flag)
	{
	case 1: //в случае необходимости очистки без сокета
		WSACleanup();
		break;
	case 2://в случае необходимости очистки с сокетом
		if (sock!=NULL)
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
	if (library_check() != 0)
		return 1;

	//создаем сокет
	SOCKET klient_socket;
	klient_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//AF_INET - чтобы использовать TCP и UDP «адресное семейство».
	//SOCK_STREAM -для создания потокового socket’а
	//IPPROTO_TCP - для создания TCP socket'a
	if (klient_socket == INVALID_SOCKET)
	{
		processing_error("функции создания сокета", 1);
		return 1;
	}

	//задаем адрес
	sockaddr_in kl_addr;
	// Устанавливаем адресное семейство
	kl_addr.sin_family = AF_INET;
	// Преобразуем номер порта 80 в сетевой порядок байтов 
	kl_addr.sin_port = htons(80);	
	std::string sss = "127.0.0.1";
	inet_pton(AF_INET, sss.c_str(), &kl_addr.sin_addr);

	// Подключаемся к серверу
	if (connect(klient_socket,(sockaddr*)(&kl_addr), sizeof(kl_addr)) != 0)
	{
		processing_error("при подключении к серверу", 2, klient_socket);
		return 1;
	}


	//ввод сообщения
	std::cout << "Соединение с сервером установлено\n";
	std::cout << "Введите сообщение(длиной до 1024 символов):\n";
	std::string example;
	std::cin >> example;
	while (example.length() >= 1024)
	{
		std::cout << "Сообщение дожно иметь длину меньше 1024 символов. Попробуйте еще раз:\n";
		std::cin >> example;
	}
	char s_buffer[1024];
	strncpy_s(s_buffer,1024, example.c_str(),example.length());
	std::cout << s_buffer<<"\n";
	//передача сообщения
	if (send(klient_socket, s_buffer, sizeof(s_buffer), 0) == SOCKET_ERROR)
	{
		processing_error("при передаче", 2, klient_socket);
		return 1;
	}
	std::cout << "Передача прошла успешно\n";

	//прием сообщения
	char r_buffer[1024];
	int bytesReceived = recv(klient_socket, r_buffer, sizeof(r_buffer), 1);
	if (bytesReceived == SOCKET_ERROR)
	{
		processing_error(" bytesReceived == SOCKET_ERROR", 2, klient_socket);	
		return 1;
	}
	std::cout << "Принято ссообщение: "<<r_buffer<<"\n";

	getchar();
	//очистка ресурсов
	
	delete[] r_buffer;
	closesocket(klient_socket);
	WSACleanup();
}