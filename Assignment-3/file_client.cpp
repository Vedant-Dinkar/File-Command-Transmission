#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 6969
#define BUFFER 1
#define IP_LOCAL "127.0.0.1"

int delimiter(char c) {
    return (c == ' ' || c == '\t' || c == ',' || c == ';' || c == ':' || c == '.');
}

void create_filename(const char *filename, char *new_filename) {
    const char *ext = strrchr(filename, '.');
    
    if (ext != NULL) {
        size_t base_len = ext - filename;
        strncpy(new_filename, filename, base_len);
        new_filename[base_len] = '\0';

        strcat(new_filename, " (copy)");
        strcat(new_filename, ext);
    } 
    else {
        strcpy(new_filename, filename);
        strcat(new_filename, " (copy)");
    }
}

int main() {
    int sock_fd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER];
    char filename[256];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket Creation Error!");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, IP_LOCAL, &serv_addr.sin_addr) <= 0) {
        perror("Invalid Address!");
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed!");
        exit(EXIT_FAILURE);
    }

    printf("Enter the Filename: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("Error Reading Filename!");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    filename[strcspn(filename, "\n")] = '\0';

    if (write(sock_fd, filename, strlen(filename)) < 0) {
        perror("Error Sending Filename!");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    char new_filename[300];
    create_filename(filename, new_filename);

    int file_fd = -1;
    ssize_t total_bytes = 0, total_words = 0;
    int in_word = 0, data_received = 0;

    ssize_t bytes_received;
    while((bytes_received = read(sock_fd, buffer, BUFFER)) > 0){
        if(!data_received){
            file_fd = open(new_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (file_fd < 0) {
                perror("Error Opening!");
                close(sock_fd);
                exit(EXIT_FAILURE);
            }
        }
        data_received = 1;

        if(write(file_fd, buffer, bytes_received) < 0){
            perror("Error Writing!");
            close(file_fd);
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        total_bytes += bytes_received;

        for(ssize_t i = 0; i < bytes_received; i++){
            if(!delimiter(buffer[i]) && !in_word) {
                total_words++;
                in_word = 1;
            } 
            else if(delimiter(buffer[i])){
                in_word = 0;
            }
        }
    }

    if(file_fd > 0) close(file_fd);
    close(sock_fd);

    if(!data_received){
        printf("ERR 01: File Not Found\n");
    } 
    else{
        printf("Transfer Complete!\nSize = %zd bytes; Number of Words = %zd\n", total_bytes, total_words);
    }

    return 0;
}
