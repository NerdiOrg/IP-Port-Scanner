#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib") // Link with Ws2_32.lib

bool scanPort(const char* ipAddress, int port) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in address;
    bool isOpen = false;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed." << std::endl;
        return false;
    }

    // Creating socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return false;
    }

    address.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress, &address.sin_addr);
    address.sin_port = htons(port);

    // Connect to the port
    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        isOpen = false;
    } else {
        isOpen = true;
    }

    closesocket(sock);
    WSACleanup();
    return isOpen;
}

int main(int argc, char* argv[]) {
    std::vector<int> commonPorts = {21, 22, 23, 25, 80, 110, 443}; // Common ports

    // Check for minimum number of arguments
    if (argc < 2) {
        std::cout << "Usage: ./portscanner <IP Address> [minPort] [maxPort]" << std::endl;
        return 1;
    }

    const char* ipAddress = argv[1];

    if (argc == 2) {
        // Scan common ports
        for (int port : commonPorts) {
            if (scanPort(ipAddress, port)) {
                std::cout << "Port " << port << " is open." << std::endl;
            }
        }
    } else if (argc == 4) {
        // Scan ports in the given range
        int minPort = std::stoi(argv[2]);
        int maxPort = std::stoi(argv[3]);
        for (int port = minPort; port <= maxPort; port++) {
            if (scanPort(ipAddress, port)) {
                std::cout << "Port " << port << " is open." << std::endl;
            }
        }
    } else {
        std::cout << "Invalid usage. Use either:\n";
        std::cout << "./portscanner <IP Address>\n";
        std::cout << "./portscanner <IP Address> <minPort> <maxPort>\n";
        return 1;
    }

    return 0;
}
