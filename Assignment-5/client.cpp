#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define PORT 6969
#define BUFFER_SIZE 1024*1024

int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed\n";
        return EXIT_FAILURE;
    }

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    cout << "Server Info: " << buffer << endl;

    cout << "Connected to server. Type commands to execute remotely.\n";

    while (true) {
        cout << "BASH > ";
        cin.getline(buffer, BUFFER_SIZE);
        
        if(strcmp(buffer, "QUIT") == 0){
            send(client_socket, buffer, strlen(buffer), 0);
            cout << "Disconnected from Server\n";
            break;
        } 
        else if (strcmp(buffer, "LIST") == 0 || strncmp(buffer, "GET ", 4) == 0) {
            string filename = string(buffer + 4);
            auto t1 = high_resolution_clock::now();

            send(client_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, BUFFER_SIZE);

            // Receive response from server
            recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (strncmp(buffer, "ERROR", 5) == 0) {
                cout << buffer << endl; // Display error message
            } else {
                // If GET command was issued
                if (strncmp(buffer, "SIZE:", 5) == 0) {
                    
                    int file_size = atoi(buffer + 5);
                    cout << "Receiving file of size: " << file_size << " bytes\n";
                    ofstream outfile(filename, ios::binary);
                    int total_bytes_received = 0;

                    while (total_bytes_received < file_size) {
                        memset(buffer, 0, BUFFER_SIZE);
                        int bytes = recv(client_socket, buffer, min(BUFFER_SIZE, file_size - total_bytes_received), 0);
                        if (bytes <= 0) break; // Error or connection closed
                        outfile.write(buffer, bytes);
                        total_bytes_received += bytes;
                    }
                    outfile.close();
                    cout << "File received successfully: " << total_bytes_received << " bytes.\n";
                    auto t2 = high_resolution_clock::now();
                    
                    auto ms_int = duration_cast<milliseconds>(t2 - t1);
                    cout << ms_int.count() << "ms\n";
                } 
                else {
                    cout << buffer; // Display LIST output or success message
                }
            }
        } else {
            cout << "Invalid Command!\n";
        }
    }

    close(client_socket);
    return 0;
}
