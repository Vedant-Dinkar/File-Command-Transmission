#include <iostream>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include<map>

using namespace std;


const int BUFFER_SIZE = 100;
const int MAX_PENDING_CONNECTIONS = 5;
const int BUFFER_READ_SIZE= 1024;




#include <chrono>
#include <iomanip>

string getCurrentDateTime() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


string exec(const char* cmd) {
    array<char, BUFFER_READ_SIZE> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


class FileServer {
private:
    int port;
    int serverSocket;

    void setupServerSocket() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) throw runtime_error("Failed to create socket");

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
            throw runtime_error("Failed to bind socket");

        if (listen(serverSocket, MAX_PENDING_CONNECTIONS) < 0)
            throw runtime_error("Failed to listen on socket");

        cout << "Server listening on port " << port << endl;
    }



    void handleClient(int clientSocket, const string& clientIP) {
        
        char commands[BUFFER_SIZE] = {0};
        int bytesRead = read(clientSocket, commands, BUFFER_SIZE - 1);
        if (bytesRead <= 0) return;

        commands[bytesRead] = '\0';
        cout << "Client requested command: " << commands << endl;
        string result = exec(commands);
        cout << result << endl;

        sendFile(clientSocket, result);
        cout << "Command execution completed for " << commands << endl;
    }



    

    void sendFile(int clientSocket, const string& result) {
        string reply="Success. Output: \n "+result;
        const char* data = reply.c_str();
        size_t dataSize = reply.size();
        size_t totalSent = 0;

        while (totalSent < dataSize) {
            ssize_t sent = write(clientSocket, data + totalSent, dataSize - totalSent);
            if (sent < 0) throw runtime_error("Failed to write to client");
            totalSent += sent;
        }
    }

public:
    map<string,int> clientMap;
    FileServer(int serverPort) : port(serverPort), serverSocket(-1) {}

    ~FileServer() {
        if (serverSocket >= 0) close(serverSocket);
    }

    void run() {
        setupServerSocket();

        while (true) {
            sockaddr_in clientAddr{};
            socklen_t clientLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
            
            if (clientSocket < 0) {
                cerr << "Failed to accept client connection" << endl;
                continue;
            }
        
            string clientIP = inet_ntoa(clientAddr.sin_addr);
            if(clientMap.find(clientIP)==clientMap.end()){
                cout << "Accepted connection from " << clientIP << endl;
                clientMap[clientIP]=1;
                string dateTime = getCurrentDateTime();
            
                string initialResponse = "Connected on: " + dateTime + "\n";
                initialResponse += "Client IP: " + clientIP + "\n\n";
                send(clientSocket, initialResponse.c_str(), initialResponse.length(), 0);
            }
            
        
            
            try {
                handleClient(clientSocket, clientIP);
            } catch (const exception& e) {
                cerr << "Error handling client: " << e.what() << endl;
            }

            close(clientSocket);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    try {
        FileServer server(atoi(argv[1]));
        server.run();
    } catch (const exception& e) {
        cerr << "Server error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
