#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 6969
#define BUFFER 100

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Binding Failed!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 3) < 0){
        perror("Listening Failed!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);

    while(true){
        client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0) {
            perror("Accept Failed!");
            continue;
        }

        memset(buffer, 0, BUFFER);
        int bytes_read = read(client_fd, buffer, BUFFER - 1);
        if (bytes_read <= 0) {
            close(client_fd);
            continue;
        }
        buffer[bytes_read] = '\0';

        printf("Client Requested to Read: %s", buffer);
        fflush(stdout);

        sleep(5);

        int file_fd = open(buffer, O_RDONLY);
        if(file_fd < 0){
            close(client_fd);
            continue;
        }

        ssize_t read_bytes;
        while((read_bytes = read(file_fd, buffer, BUFFER)) > 0){
            ssize_t sent_bytes = 0;
            while(sent_bytes < read_bytes){
                ssize_t n = write(client_fd, buffer + sent_bytes, read_bytes - sent_bytes);
                if (n <= 0) break;
                sent_bytes += n;
            }
        }

        close(file_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
