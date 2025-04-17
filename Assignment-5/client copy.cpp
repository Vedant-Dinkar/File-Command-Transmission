#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define PORT 6969
#define BUFFER_SIZE 1024

int main(){
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1){
        cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        cerr << "Connection failed\n";
        return EXIT_FAILURE;
    }

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    cout << "Server Info: " << buffer << endl;

    cout << "Connected to server. Type commands to execute remotely.\n";

    while(true){
        cout << "BASH > ";
        cin.getline(buffer, BUFFER_SIZE);
        int flag = 1;

        if(strcmp(buffer, "LIST") == 0){
            flag = 0;
        }
        else if(strcmp(buffer, "GET") == 0){
            flag = 0;
        }
        else if(strcmp(buffer, "QUIT") == 0){
            flag = 2;
        }

        if(flag == 1){
            cout << "Invalid Command!\n";
            continue;
        }

        send(client_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);

        if(flag == 2){
            cout << "Disconnected from Server\n";
            break;
        }

        recv(client_socket, buffer, BUFFER_SIZE, 0);

        cout << buffer << endl;
    }

    close(client_socket);
    return 0;
}
