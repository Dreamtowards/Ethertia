//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_DEDICATEDSERVER_CPP
#define ETHERTIA_DEDICATEDSERVER_CPP

#include <stdexcept>

#include <ethertia/util/Log.h>

#define ENET_IMPLEMENTATION
#include <enet.h>


int main() {

    if (enet_initialize()) {
        throw std::runtime_error("a");
    }

    ENetAddress addr = {};
    addr.host = ENET_HOST_ANY;
    addr.port = 8081;

    ENetHost* serv = enet_host_create(&addr, 10, 2, 0, 0);
    if (!serv)
        throw std::runtime_error("aaa");

    Log::info("Server Started");


    ENetEvent event;

    while (enet_host_service(serv, &event, 100000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                printf("A new client connected from %x:%u.\n",  event.peer->address.host, event.peer->address.port);


                event.peer->data = (void*)"Cli Mark 123";
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
                       event.packet->dataLength,
                       event.packet->data,
                       event.peer->data,
                       event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                printf("%s disconnected due to timeout.\n", event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    enet_host_destroy(serv);
    enet_deinitialize();

}



//#include <asio.hpp>
//
//char BUF[1024];
//
//void ReadData(asio::ip::tcp::socket& conn) {
//
//    conn.async_read_some(asio::buffer(BUF, 1024), [&](std::error_code err, std::size_t len)
//    {
//        if (len == 0)
//            return ;
//
//        Log::info("Read[{}]: ", len);
//
//        for (int i = 0; i < len; ++i) {
//            std::cout << BUF[i];
//        }
//        Log::info("ReadFinished", len);
//
//        ReadData(conn);
//    });
//
//}
//
//int main()
//{
//
//    std::error_code err;
//
//    asio::io_context ctx;
//
//    asio::io_context::work idleWork(ctx);
//
//    std::thread ctx_thread = std::thread([&ctx](){
//        ctx.run();
//    });
//
//    {
//        int port = 8081;
//        asio::ip::tcp::acceptor serv(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
//
//        serv.async_accept([](std::error_code err, asio::ip::tcp::socket conn)
//        {
//
//        });
//    }
//
//    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34"), 80);
//
//    asio::ip::tcp::socket conn(ctx);
//
//    conn.connect(endpoint, err);
//
//    if (err) {
//        Log::info("Failed to connect: ", err.message());
//    }
//
//    if (conn.is_open())
//    {
////        ReadData(conn);
//
//        std::string msg = "GET / HTTP/1.1\n"
//                          "Host: example.com\n"
//                          "Connection: close\n\n";
//
//        conn.write_some(asio::buffer(msg.data(), msg.size()));
//
//
//        char buf[1024];
//        int n = conn.read_some(asio::buffer(buf, 1024));
//
//        Log::info("Read[{}]: ", n, buf);
//
//
//
////        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
//
//    }
//
//    ctx.stop();
//    if (ctx_thread.joinable())
//        ctx_thread.join();
//
//    return 0;
//}


#endif //ETHERTIA_DEDICATEDSERVER_CPP
