#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <cstring>

std::mutex output_mutex;

bool scanPort(const char* ipAddress, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return false;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress, &address.sin_addr);
    address.sin_port = htons(port);

    // Set timeout for connect
    struct timeval timeout;
    timeout.tv_sec = 1; // 1 second timeout
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    bool isOpen = connect(sock, (struct sockaddr *)&address, sizeof(address)) == 0;
    close(sock);
    return isOpen;
}

void scanAndReport(const char* ipAddress, int port) {
    if (scanPort(ipAddress, port)) {
        std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << "Port " << port << " is open." << std::endl;
    }
}

int main(int argc, char* argv[]) {

    std::cout << "Starting Port Scanner..." << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <IP Address> [minPort] [maxPort]" << std::endl;
        return 1;
    }

    const char* ipAddress = argv[1];
    std::vector<std::thread> threads;

    if (argc == 2) {
        // Scan common ports
        std::cout << "Scanning common ports for IP Address: " << ipAddress << std::endl;
        std::vector<int> commonPorts = {21, 22, 23, 25, 80, 110, 443};
        for (int port : commonPorts) {
            threads.emplace_back(scanAndReport, ipAddress, port);
        }
    } else if (argc == 4) {
        // Scan ports in the given range
        int minPort = std::stoi(argv[2]);
        int maxPort = std::stoi(argv[3]);
        std::cout << "Scanning Ports: " << argv[2] << " - " << argv[3] << " for IP Address: " << ipAddress << std::endl;
        for (int port = minPort; port <= maxPort; port++) {
            threads.emplace_back(scanAndReport, ipAddress, port);
        }
    } else {
        std::cerr << "Invalid usage. Use either:\n";
        std::cerr << argv[0] << " <IP Address>\n";
        std::cerr << argv[0] << " <IP Address> <minPort> <maxPort>\n";
        return 1;
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Port scanning completed!" << std::endl;
    return 0;
}
