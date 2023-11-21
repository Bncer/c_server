#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_REQUEST_SIZE 1024

void send_file(FILE *file, int client_socket) {
    char buffer[MAX_REQUEST_SIZE];
    int bytes_read;

    while ((bytes_read = fread(buffer, 1, MAX_REQUEST_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr;
    FILE *html_file, *css_file;

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
        char message_buff[MAX_REQUEST_SIZE];
        
        if ((client_sock = accept(server_sock,
                                  (struct sockaddr *)&client_addr,
                                  &client_addr_len)) == -1) {
            perror("Connection with client failed");
            return -1;
        }
        

        int bytes_received = recv(client_sock, message_buff, MAX_REQUEST_SIZE, 0);
        if (bytes_received < 0) {
            perror("Failed to read request");
            close(client_sock);
            continue;
        }

        message_buff[bytes_received] = '\0';
        
        if (strstr(message_buff, "GET / ") != NULL) {
            // Send index.html
            html_file = fopen("index.html", "r");
            if (html_file == NULL) {
                perror("Failed to open index.html");
                close(client_sock);
                continue;
            }

            char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            send(client_sock, response, strlen(response), 0);
            send_file(html_file, client_sock);
            close(html_file);
        } else if (strstr(message_buff, "GET /styles.css ") != NULL) {
            // Send styles.css
            css_file = fopen("styles.css", "r");
            if (css_file == NULL) {
                perror("Failed to open styles.css");
                close(client_sock);
                continue;
            }

            char css_response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n";
            send(client_sock, css_response, strlen(css_response), 0);
            send_file(css_file, client_sock);
            close(css_file);
        } else if (strstr(message_buff, "GET /favicon.ico ") != NULL) {
            // No favicon for now
            char empty_response[] = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(client_sock, empty_response, strlen(empty_response), 0);
        } else {
            // Invalid request
            char error_response[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
            send(client_sock, error_response, strlen(error_response), 0);
        }

        // Close the client socket
        close(client_sock);
    }
}
