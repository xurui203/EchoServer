#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int server(uint16_t port);
int client(const char * addr, uint16_t port);

#define MAX_MSG_LENGTH (1300)
#define MAX_BACK_LOG (5)

//argc = argument count = number of strings pointed to by argv
//argv = argument vector
int main(int argc, char ** argv)
{
	if (argc < 3) { 
		printf("usage: myprog c <port> <address> or myprog s <port>\n");
		return 0;
	}


	uint16_t port = atoi(argv[2]);
	if (port < 1024) {
		fprintf(stderr, "port number should be equal to or larger than 1024\n");
		return 0;
	}
	if (argv[1][0] == 'c') { 
		return client(argv[3], port);
	} else if (argv[1][0] == 's') {
		return server(port);
	} else {
		fprintf(stderr, "unkonwn commend type %s\n", argv[1]);
		return 0;
	}
	return 0;
}

int client(const char * addr, uint16_t port) //server's IP address, server's port number
{
	int sock;
	struct sockaddr_in server_addr;
	char msg[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH];

	//Create socket
	if ((sock = socket(AF_INET, SOCK_STREAM/* use tcp */, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}

	printf("Socket created\n");

	//Construct the address data structure expected by socket interface
	server_addr.sin_addr.s_addr = inet_addr(addr); //set address to be address given
	server_addr.sin_family = AF_INET; //set protocol family to be Internet TCP
	server_addr.sin_port = htons(port); //specify server port to be port number given in argument


	// Active open, connect to server
	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("Connect error:");
		return 1;
	}

	printf("Connected to server %s:%d\n", addr, port);

	while (1) {
		printf("Enter message: \n");
		scanf("%s", msg);

		if (send(sock, msg, strnlen(msg, MAX_MSG_LENGTH), 0) < 0) {
			perror("Send error:");
			return 1;
		}
		int recv_len = 0;
		if ((recv_len = recv(sock, reply, MAX_MSG_LENGTH, 0)) < 0) {
			perror("Recv error:");
			return 1;
		}
		reply[recv_len] = 0;
		printf("Server reply:\n%s\n", reply);
	}
	return 0;
}

int server(uint16_t port)
{
	int sock, new_sock;;
	struct sockaddr_in client_addr;
	char buf[MAX_MSG_LENGTH];
	int recv_len;

	//Construct the address data structure by filling in its own port number
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr = INADDR_ANY;
	client_addr.sin_port = htons(port);


	//Passive open: Create socket
	
	if ((sock = socket(AF_INET, SOCK_STREAM/, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}


	//Bind socket to local address

	if ((bind(sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0){
		perror("Binding error:");
		return 1;
	}

	//Set up maximum number of pending connections to be allowed
	listen(sock, MAX_BACK_LOG);

	//Loop: wait for a remote host to try to connect.
		//When connected, receive and echo characters that arrive on the connection

	while (1){
		if ((new_sock = accept(sock, (struct sockaddr *)&client_addr, &recv_len)) < 0){ //&??
			perror("Accepting error:");
			return 1;
		}

		while (recv_len = recv(new_sock, buf, sizeof(buf), 0)){
			fputs(buf, stdout);
		}
		close(new_sock);
	}
	return 0;
}
