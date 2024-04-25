#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

void receiveData(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead;

    do {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << "[SWITCH] " << buffer << std::endl;

    } while (bytesRead > 0);
}

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Bind socket to port 12345
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr =
    INADDR_ANY;
    serverAddress.sin_port = htons(2635);

    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
    std::cerr << "Error binding socket" << std::endl;
    close(serverSocket);
    return -1;
}

// Listen for connections
if (listen(serverSocket, 1) == -1) {
    std::cerr << "Error listening for connections" << std::endl;
    close(serverSocket);
    return -1;
}

// Accept a connection
std::cout << "Waiting for a connection on port: " << serverAddress.sin_port  << std::endl;
sockaddr_in clientAddress;
socklen_t clientAddressLength = sizeof(clientAddress);
int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddressLength);

if (clientSocket == -1) {
    std::cerr << "Error accepting connection" << std::endl;
    close(serverSocket);
    return -1;
}

std::cout << "Connection accepted. Client connected." << std::endl;

// Start a thread to receive data
//std::thread receiveThread(receiveData, clientSocket);
std::thread receiveThread([clientSocket]() { receiveData(clientSocket); });
// Main thread: Send data from cin to the client
std::string sendData;
while (true) {
    if (!std::getline(std::cin, sendData)) {
        // Ctrl+D was pressed, terminate the thread
        break;
    }

    if (!sendData.empty() && sendData != "\n" && sendData != "\r" && sendData != " ") {
        send(clientSocket, sendData.c_str(), sendData.size(), 0);
    }
}

// Close sockets and join the receive thread
close(clientSocket);
close(serverSocket);

std::cout << "Server closed." << std::endl;

// Wait for the receive thread to finish
receiveThread.join();

return 0;
    
}
