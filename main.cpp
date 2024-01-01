#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

std::mutex output_mutex;

bool scanPort(const char* ipAddress, int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return false;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress, &address.sin_addr);
    address.sin_port = htons(port);

    // Set timeout for connect
    DWORD timeout = 1000; // 1 second timeout
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    bool isOpen = connect(sock, (struct sockaddr *)&address, sizeof(address)) == 0;
    closesocket(sock);
    return isOpen;
}

void scanAndReport(const char* ipAddress, int port) {
    if (scanPort(ipAddress, port)) {
        std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << "Port " << port << " is open." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed." << std::endl;
        return 1;
    }

    if (argc < 2) {
        std::cerr << "Usage: ./portscanner <IP Address> [minPort] [maxPort]" << std::endl;
        WSACleanup();
        return 1;
    }

    const char* ipAddress = argv[1];
    std::vector<std::thread> threads;

    if (argc == 2) {
        // Scan common ports
        std::vector<int> commonPorts = {21, 22, 23, 25, 80, 110, 443};
        for (int port : commonPorts) {
            threads.emplace_back(scanAndReport, ipAddress, port);
        }
    } else if (argc == 4) {
        // Scan ports in the given range
        int minPort = std::stoi(argv[2]);
        int maxPort = std::stoi(argv[3]);
        for (int port = minPort; port <= maxPort; port++) {
            threads.emplace_back(scanAndReport, ipAddress, port);
        }
    } else {
        std::cerr << "Invalid usage. Use either:\n";
        std::cerr << "./portscanner <IP Address>\n";
        std::cerr << "./portscanner <IP Address> <minPort> <maxPort>\n";
        WSACleanup();
        return 1;
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    WSACleanup();
    return 0;
}
