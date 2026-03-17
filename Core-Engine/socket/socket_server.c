// Author: Piyusha Akash (0x3xp) | https://0x3xp.github.io
// GitHub: https://github.com/0x3xp
// Project: Basic C2 Socket Framework (Research & Education)
// Disclaimer: This code is for educational purposes and authorized security testing only.

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
int main() {

        int server_socket; // Used for creating server socket and store values
        int client_socket; // Used for accept connections
        struct sockaddr_in server_addr; // Structure for bind() function
        char send_buffer[500] = "Hey Client!"; // Message when display after connected to the server
        char recv_buffer[500]; //recieving buffer
	
	//clear the receve buffer
	memset(recv_buffer, 0, sizeof(recv_buffer));

        //creates a TCP socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0); //syntax: --> socketfamily(IPv), sokettype(tcp), protocol | all are int data type

        //Fill the strucure. Specify the informations: server_addr
        server_addr.sin_family = AF_INET; //type (IPv4)
        server_addr.sin_port = htons(9001); // port of the server
        server_addr.sin_addr.s_addr = inet_addr("172.18.160.161"); // server IP address (computer IP)

        //declaring the bind function
        bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)); //syntax: --> socketfd(server_socket), pointer to structure, sizeof structure

        //declaring the listen function
        listen(server_socket, 5); //syntax: --> socketfd(server_socket), number of listening connections | fd means file descryptor

        //store values in clien_socket variable
        client_socket = accept(server_socket, NULL, NULL); //synatx: --> socketfd, NULL, NULL
        printf("Connection accepted. Client connected successfully!\n");

        //declaring the send function
        send(client_socket, send_buffer, sizeof(send_buffer), 0); //syntax: --> clientfd(client_socket), buffer, size of buffer, 0

        //declaring the receving function
        recv(client_socket, recv_buffer, sizeof(recv_buffer), 0); //syntax: --> same as send
        printf("Client has sent: %s", recv_buffer);

        close(server_socket); //close the socket_server

        return 0;

}
