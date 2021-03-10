#include <iostream>
#include <WS2tcpip.h> //includes winsock header file

#pragma comment (lib, "ws2_32.lib")

int main()
{
	//initialize winsock  //add error checking
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0)
	{
		std::cerr << "Can't initialize winsock!" << std::endl;
		return 0;
	}


	//create socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket" << std::endl;
		return 0;
	}


	//bind socket to ip address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(5400); //host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));


	//tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	//wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	//check that clientSocket is valid
	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXHOST]; //service - client is connected

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << "connected on port " << service << std::endl;
	}

	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port" << ntohs(client.sin_port) << std::endl;
	}
	//close listening socket
	closesocket(listening);

	//while loop: accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		// wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(). Quitting " << std::endl;
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected " << std::endl;
			break;
		}

		//echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0); //but is the message, bytesReceived + 1 is the length of message, should send the terminating 0 with the + 1, that 
			//we don't get automatically when we receive a msg
	}


	//close socket
	closesocket(clientSocket);

	//shutdown winscok
	WSACleanup();


	return 0;
}