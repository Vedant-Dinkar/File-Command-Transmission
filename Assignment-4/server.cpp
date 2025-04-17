#include <bits/stdc++.h>
#include <arpa/inet.h>

using namespace std;

sockaddr_in serverAddress;

sockaddr_in addresses[1024];

map<string, int> balance;
map<string, string> username;
map<string, int> address;
map<string, vector<pair<string, string>>> mails;

string conv(sockaddr_in &addr){
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    
    string result = string(ip) + ":" + to_string(ntohs(addr.sin_port));

    cout << "Converted: " << result << endl;
    return result;
}

string time(){
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    tm* tm_now = localtime(&now_time);

    ostringstream oss;
    oss << put_time(tm_now, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

void chat(int serverSocket, char arr[], string sender){
    string to = "", message = "";
    int i = 0;

    string buf = string(arr);

    cout << buf << endl;
    int n = buf.size();

    while(i < n && buf[i] != '"') i++;
    while(i < n && buf[i] != '"'){
        to.push_back(arr[i++]);
    }
    while(i < n && buf[i] != '"') i++;
    while(i < n && buf[i] != '"'){
        message.push_back(arr[i++]);
    }

    sockaddr_in idx = addresses[address[to]];

    cout << "Added: " << sender << " " << message << endl;

    mails[to].push_back({sender, message});
}

int main(){
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    int index = 0;

    if(serverSocket == -1){
        cerr << "Error Creating Socket!\n";
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(9090);
    if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        cerr << "Error Binding Socket! (Port already in use)\n";
        close(serverSocket);
        return -1;
    }

    cout << "Listening on port 9090...\n";

    while(true){
        char buffer[1024];
        sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);

        ssize_t bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if(bytesRead == -1){
            cerr << "Error Receiving Data\n";
            continue;
        }

        buffer[bytesRead] = '\0';
        cout << "Received Data: " << buffer << "\n";
        
        string userID = conv(clientAddress);

        string result = "";

        if(buffer[0] == 'T'){
            result = time();
            cout << "User " << username[userID] << " asked: Time()" << endl;
        }
        else if(buffer[0] == 'I'){
            result = "127.0.1.1";
            cout << "User " << username[userID] << " asked: IP()" << endl;
        }
        else if(buffer[0] == 'U'){
            result = username[userID];
            cout << "User " << username[userID] << " asked: User()" << endl;
        }
        else if(buffer[0] == 'D'){
            result = "Success!";
            balance[userID] += 100;
            cout << "User " << username[userID] << "'s balance updated to: " << balance[userID] << endl;
        }
        else if(buffer[0] == 'C'){
            try{
                chat(serverSocket, buffer, username[userID]);
            }
            catch(exception){
                balance[userID] = INT_MIN;
                cout << "User " << username[userID] << " got penalized." << endl;
            }
        }
        else{
            username[userID] = buffer;
            addresses[index] = clientAddress;
            index++;
            balance[userID] = 0;
            cout << "Welcome " << buffer << "!" << endl;
            string rec = username[userID];

            string ip = "127.0.1.1";

            result = time() + " " + ip;
            
            if(mails[rec].size()){
                result = result + "\n" + mails[rec].back().first + " says: " + mails[rec].back().second;
                mails[rec].pop_back();
            }
        }

        ssize_t bytesSent = sendto(serverSocket, result.c_str(), result.length(), 0, (struct sockaddr *)&clientAddress, clientAddrLen);
        if(bytesSent == -1){
            cerr << "Error Sending!\n";
        }
    }

    close(serverSocket);
    return 0;
}
