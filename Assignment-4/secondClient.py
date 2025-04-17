import socket
import time

def main():
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    serverAddress = ('127.0.1.1', 9090)
    name = str(input("Enter Username: "))

    try:
        clientSocket.sendto(name.encode(), serverAddress)
        buffer, _ = clientSocket.recvfrom(1024)
        print(buffer.decode())
    except Exception as e:
        print(f"Error: {e}")



    def Time(times, gap):
        for i in range(times):
            try:
                clientSocket.sendto("Time()".encode(), serverAddress)
                buffer, _ = clientSocket.recvfrom(1024)
                print(buffer.decode())
                time.sleep(gap)
            except Exception as e:
                print(f"Error: {e}")

    def IP(times, gap):
        for i in range(times):
            try:
                clientSocket.sendto("IP()".encode(), serverAddress)
                buffer, _ = clientSocket.recvfrom(1024)
                print(buffer.decode())
                time.sleep(gap)
            except Exception as e:
                print(f"Error: {e}")
    
    def Deposit(times, gap):
        for i in range(times):
            try:
                clientSocket.sendto("Deposit()".encode(), serverAddress)
                buffer, _ = clientSocket.recvfrom(1024)
                print(buffer.decode())
                time.sleep(gap)
            except Exception as e:
                print(f"Error: {e}")

    def User(times, gap):
        for i in range(times):
            try:
                clientSocket.sendto("User()".encode(), serverAddress)
                buffer, _ = clientSocket.recvfrom(1024)
                print(buffer.decode())
                time.sleep(gap)
            except Exception as e:
                print(f"Error: {e}")



    print("-- Usage Enter Function (Time(times, gap), IP(times, gap), Deposit(times, gap), User(times, gap)) --")

    while True:
        work = str(input("</> "))

        try:
            exec(work)
        except:
            print("Invalid Command!")
            clientSocket.close()
            break

if __name__ == "__main__":
    main()

