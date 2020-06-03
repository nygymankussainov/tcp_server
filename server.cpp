#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int		main(void) {

	// Create a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if ( sockfd == -1 ) {
		std::cerr << "Cannot create socket" << std::endl;
		return -1;
	}

	// Bind the socket to an IP/Port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54001); // htons() - host to network short - converts port number into big/little endian
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); // inet_pton() - converts 127.0.0.1(localhost) into array of integers

	if ( bind(sockfd, reinterpret_cast<sockaddr*>(&hint), sizeof(hint)) == -1 ) {
		std::cerr << "Cannot bind to IP/port" << std::endl;
		return -2;
	}

	// Mark the socket for listening in
	if ( listen(sockfd, SOMAXCONN) == - 1) {
		std::cerr << "Cannot listen" << std::endl;
		return -3;
	}

	// Accept all
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	int clientSocket = accept( sockfd, reinterpret_cast<sockaddr*>(&client), &clientSize );
	if ( clientSocket == -1 ) {
		std::cerr << "Problem with client connecting" << std::endl;
		return -4;
	}

	// Close the listening socket
	close(sockfd);

	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int result = getnameinfo( reinterpret_cast<sockaddr*>(&client), sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0 );
	if ( result ) {
		std::cout << host << " connected on " << svc << std::endl;
	} else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST); // opposite of inet_pton() (line 24)
		std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
	}

	// While receiving - dipslay message, echo message
	char buff[4096];

	while ( true ) {
		// Clear the buffer
		memset(buff, 0, 4096);

		// Wait for a message
		int bytesRecv = recv( clientSocket, buff, 4096, 0 );
		if ( bytesRecv == -1 ) {
			std::cerr << "There was a connection issue" << std::endl;
			break;
		} else if ( bytesRecv == 0 ) {
			std::cout << "The client has connected" << std::endl;
			break;
		}
		
		// Display message
		std::cout << "Received: " << std::string(buff, 0, bytesRecv) << std::endl;

		// Resend message
		send(clientSocket, buff, bytesRecv + 1, 0);
	}
	
	// Close the socket
	close(clientSocket);
	return 0;
}
