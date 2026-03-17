// Author: Piyusha Akash (0x3xp) | https://0x3xp.github.io
// GitHub: https://github.com/0x3xp
// Project: Basic C2 Socket Framework (Research & Education)
// Disclaimer: This code is for educational purposes and authorized security testing only.

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
	
	SOCKET client_socket; // create a SOCKET structure
	WSADATA wsastructure; //create a WSADATA structure required for winsock initialization
	int result;
	struct sockaddr_in client_addr; //create a structure required by the connect() function
	char recv_data[500];
	char send_data[500] = "Hello from the client!\n";
	
	memset(recv_data, 0, sizeof(recv_data));
	
	result = WSAStartup(MAKEWORD(2, 2), &wsastructure); //initialize winsock
	
	if(result != 0) {
		printf("Winsock initialization failed!\n"); //print the error if program didn't execute propery
		return 1;
	}
	
	client_socket = socket(AF_INET, SOCK_STREAM, 0); //create client socket
	
	client_addr.sin_family = AF_INET; //family | type
	client_addr.sin_port = htons(9001); //port
	client_addr.sin_addr.s_addr = inet_addr("172.18.160.161"); //IP
	
	connect(client_socket, (struct sockaddr *) &client_addr, sizeof(client_addr)); //syntaxt: socketfd, pointer to structure, size of structure | connect to the target server
	
	recv(client_socket, recv_data, sizeof(recv_data), 0); //receive data
	printf("Data from the server : %s\n", recv_data);
	send(client_socket, send_data, (int)strlen(send_data), 0); //send data
	closesocket(client_socket); //close socket connection
	WSACleanup(); //close the program
	return 0;
}
