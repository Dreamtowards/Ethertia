//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_DEDICATEDSERVER_CPP
#define ETHERTIA_DEDICATEDSERVER_CPP

#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>  // struct sockaddr_in
#include <arpa/inet.h>   // ip addr lookup
#include <unistd.h>      // close(conn)

#include <ethertia/util/Log.h>

int main()
{
    int port = 8081;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw std::runtime_error("failed init socket.");
    }

    // // optional, force attach socket. reuse address.
    // int opt = 1;
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    //     throw std::runtime_error("failed reuse socket address.");
    // }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("failed bind socket.");
    }

    const int backlog = 3;
    if (listen(server_fd, backlog) < 0) {
        throw std::runtime_error("failed listen socket.");
    }


    while (true)
    {
        struct sockaddr_in cli_addr;
        socklen_t addrlen = sizeof(addr);
        int conn = accept(server_fd, (struct sockaddr*)&cli_addr, &addrlen);
        if (conn < 0) {
            throw std::runtime_error("failed accept socket.");
        }

        char cli_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);
        Log::info("client ip: ", cli_ip);

        char buf[1024];
        int n = recv(conn, buf, sizeof(buf), 0);
        Log::info("Recv ", buf);


        char* data = "Hello Text";
        send(conn, data, strlen(data), 0);
        Log::info("Msg Sent");


        close(conn);

        break;
    }


    close(server_fd);
    shutdown(server_fd, SHUT_RDWR);

    return 0;
}


#endif //ETHERTIA_DEDICATEDSERVER_CPP
