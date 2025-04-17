#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

const int BUFFER_SIZE = 100;

bool isWordSeparator(char c) {
    return strchr(",;:. \t\n", c) != nullptr;
}

class FileClient {
private:
    string serverIP;
    int serverPort;
    int socketFD;
    
    void setupConnection() {
        socketFD = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFD < 0) throw runtime_error("Socket creation failed");

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0)
            throw runtime_error("Invalid IP address");

        if (connect(socketFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
            throw runtime_error("Connection failed");
    }

    void sendFilename(const string& filename) {
        if (write(socketFD, filename.c_str(), filename.length()) < 0)
            throw runtime_error("Failed to send filename");
    }

    void receiveAndProcessFile(const string& outputFilename) {
        long totalBytes = 0, wordCount = 0;
        bool iw = false;
        char buffer[BUFFER_SIZE];

        while (true) {
            int bytesRead = read(socketFD, buffer, BUFFER_SIZE);
            if (bytesRead <= 0) break;

            totalBytes += bytesRead;

            for (int i = 0; i < bytesRead; ++i) {
                cout << buffer[i];
            }
        }

        if (totalBytes == 0) throw runtime_error("ERR 01: Error Executing Command");

        cout << "\nCommand Execution successful."<<endl;
    }

public:
    FileClient(const string& ip, int port) : serverIP(ip), serverPort(port) {}

    void run() {
        while(true){
            try {
                setupConnection();

                string filename;
                cout << "Enter command : ";
                getline(cin, filename);

                sendFilename(filename);
                receiveAndProcessFile("received_" + filename);
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }

            if (socketFD >= 0) close(socketFD);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_ip> <port>" << endl;
        return 1;
    }

    FileClient client(argv[1], atoi(argv[2]));
    client.run();
    return 0;
}
