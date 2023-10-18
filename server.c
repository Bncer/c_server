#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 10000

int main() {
    int server_sock;
    struct sockaddr_in server_addr;
    /*char *message_buff = (char *)malloc(BUFFER_SIZE * sizeof(char));*/
    char message_buff[BUFFER_SIZE];
    
    memset(message_buff, '\0', sizeof(message_buff));

    // Create server socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Fill data for sockaddr_in struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if ((bind(server_sock,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr))) == -1) {
        perror("Couldn't bind socket");
        return -1;
    }
    
    if (listen(server_sock, 2) == -1) {
        perror("Listening failed");
        return -1;
    }

    printf("Server running on port %d\n", server_addr.sin_port);
    
    while (1) {
        int client_sock;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        if ((client_sock = accept(server_sock,
                                  (struct sockaddr *)&client_addr,
                                  &client_addr_len)) == -1) {
            perror("Connection with client failed");
            return -1;
        }
        
        printf("Client connected at IP: %s and port: %i\n",
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if (recv(client_sock, message_buff, sizeof(message_buff), 0) <= 0){
            printf("Couldn't receive\n");
            return -1;
        }
        printf("Msg from client: %s\n", message_buff);


        char *response = "HTTP/1.1 200 OK\r\nServer: Apache\r\nContent-Length: 12\r\nConnection: keep-alive\r\nContent-Type: text/html\r\n\r\nHello world!";
        
        if (send(client_sock, response, strlen(response), 0) < 0){
            printf("Can't send\n");
            return -1;
        }

        // Closing the socket:
        close(client_sock);
    }
    close(server_sock);
}
