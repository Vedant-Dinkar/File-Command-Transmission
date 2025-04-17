#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>

using namespace std;

#define PORT 6969
#define BUFFER_SIZE 1024*1024

void execute_command(int client_socket) {
    char buffer[BUFFER_SIZE];
    
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break; // Client disconnected
        }

        cout << "Received command: " << buffer << endl;

        if (strcmp(buffer, "QUIT") == 0) {
            cout << "Client disconnected\n";
            close(client_socket);
            break;
        } else if (strcmp(buffer, "LIST") == 0) {
            DIR *dir;
            struct dirent *ent;
            string response;

            dir = opendir(".");
            if (dir != nullptr) {
                while ((ent = readdir(dir)) != nullptr) {
                    response += string(ent->d_name) + "\n";
                }
                closedir(dir);
            } else {
                response = "ERROR: Unable to open directory\n";
            }

            send(client_socket, response.c_str(), response.length(), 0);

        } else if (strncmp(buffer, "GET ", 4) == 0) {
            string filename = string(buffer + 4);
            FILE *file = fopen(filename.c_str(), "rb");
            
            if (!file) {
                send(client_socket, "ERROR: File Not Found\n", strlen("ERROR: File Not Found\n"), 0);
                continue;
            }

            fseek(file, 0L, SEEK_END); 
            long file_size = ftell(file); 
            rewind(file); 

            string header = "SIZE:" + to_string(file_size); 
            send(client_socket, header.c_str(), header.length(), 0);

            char file_buffer[BUFFER_SIZE];
            size_t bytes_read;
            
            while ((bytes_read = fread(file_buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
                send(client_socket, file_buffer, bytes_read, 0);
            }
            
            fclose(file);

        } else {
            send(client_socket, "ERROR: Invalid Command\n", strlen("ERROR: Invalid Command\n"), 0);
        }
    }
    
    close(client_socket);
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Binding failed\n";
        return EXIT_FAILURE;
    }

    if (listen(server_socket, SOMAXCONN) < 0) {
        cerr << "Listening failed\n";
        return EXIT_FAILURE;
    }

    cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            cerr << "Connection Failed\n";
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        string welcome_msg = "Welcome to Simple File Server\n";
        send(client_socket, welcome_msg.c_str(), welcome_msg.length(), 0);

        cout << "Client connected from: " << client_ip << endl;
        
        execute_command(client_socket); // Handle commands for this client
    }

    close(server_socket);
    return EXIT_SUCCESS;
}
