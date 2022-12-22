//
// Created by Dreamtowards on 2022/12/23.
//


#include <sys/socket.h>
#include <netinet/in.h>  // struct sockaddr_in
#include <arpa/inet.h>   // ip addr lookup
#include <unistd.h>      // close(conn)

#include <ethertia/util/Log.h>

void client()
{
    int port = 8081;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return;

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        return;

    int client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (client_fd < 0)
        return;

    char* data = "Hello From Client";
    send(sock, data, strlen(data), 0);

    char buf[1024];
    int n = recv(sock, buf, sizeof(buf), 0);
    Log::info("Cli Recv: ", buf);


    close(client_fd);
}

int main() {

    client();

}