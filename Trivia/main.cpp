#pragma comment (lib, "ws2_32.lib")

#include "WSAInitializer.h"
#include "TriviaServer.h"
#include <iostream>

int main()
{
	try
	{

		std::cout << "Server open:" << std::endl;
		WSAInitializer wsa_init;
		TriviaServer server;
		server.server();
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception was thrown in function: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown exception!" << std::endl;
	}
	return 0;
}