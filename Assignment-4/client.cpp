#include <bits/stdc++.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if(clientSocket == -1){
        cerr << "Error Creating Socket!\n";
        return -1;
    }

    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(9090);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if(setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0){
        cerr << "Error in Socket Timeout!\n";
        close(clientSocket);
        return -1;
    }

    string user;
    cout << "Enter Username: ";
    cin >> user;
    
    ssize_t bytesSent = sendto(clientSocket, user.c_str(), user.length(), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if(bytesSent == -1){
        cerr << "Error Establishing Connection! Restart the application.\n";
        close(clientSocket);
        return 0;
    }

    char buffer[1024];
    sockaddr_in fromAddress;
    socklen_t fromLen = sizeof(fromAddress);
    ssize_t bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&fromAddress, &fromLen);

    if(bytesRead == -1){
        cerr << "Connection Request Timed Out!" << endl;
        close(clientSocket);
        return 0;
    } 
    else{
        buffer[bytesRead] = '\0';
        cout << buffer << "\n";
    }

    cout << "-- Usage Enter Function (Time(), IP(), Deposit(), User(), Chat([username][message])) --" << endl;

    string choice = "";

    while(true){
        cout << "</> ";
        cin >> choice;

        string pingMessage = choice;

        if(choice == "Time()" || choice == "IP()" || choice == "Deposit()" || choice == "User()" || choice[0] == 'C'){
            ssize_t bytesSent = sendto(clientSocket, pingMessage.c_str(), pingMessage.length(), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
            if(bytesSent == -1){
                cerr << "An unexpected error occurred! Kindly restart the application.\n";
                close(clientSocket);
                return 0;
            }

            char buffer[1024];
            sockaddr_in fromAddress;
            socklen_t fromLen = sizeof(fromAddress);
            ssize_t bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&fromAddress, &fromLen);

            if(bytesRead == -1){
                cerr << "Receive Request Timed Out!" << endl;
            } 
            else{
                buffer[bytesRead] = '\0';
                cout << buffer << "\n";
            }
        }
        else{
            cerr << "Invalid Command!\n";
            break;
        }
    }

    close(clientSocket);
    return 0;
}
