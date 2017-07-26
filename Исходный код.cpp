#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/version.hpp>

using namespace boost::asio;
using boost::system::error_code;
io_service service;

ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001); // Создание конечной точки.
//Конечная точка - это адрес подключения вместе с портом. 
//Вы можете создать конечную точку тремя способами :
//endpoint() : конструктор по умолчанию и он может быть иногда использован для UDP / ICMP сокетов
//endpoint(protocol, port) : обычно используется на серверных сокетах для приема новых подключений
//endpoint(addr, port) : создание конечной точки по адресу и порту - наш вариант



size_t read_complete(char * buf, const error_code & err, size_t bytes)
{
	if (err) return 0;
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	// we read one-by-one until we get to enter, no buffering
	return found ? 0 : 1;
}


void handle_connections()
{
	ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
	char buff[1024];
	while (true)
	{
		ip::tcp::socket sock(service);
		acceptor.accept(sock);//создание сервера
		int bytes = read(sock, buffer(buff), boost::bind(read_complete, buff, _1, _2));
		std::string msg(buff, bytes);
		sock.write_some(buffer(msg));
		sock.close();
	}
}

int main()
{
	
	handle_connections();
	getchar();
	return 0;
}
