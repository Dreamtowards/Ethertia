//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_DEDICATEDSERVER_CPP
#define ETHERTIA_DEDICATEDSERVER_CPP


#include <ethertia/util/Log.h>


#include <ethertia/server/Network.h>



void runServer() {

    Network::Host* serv = Network::newServer(8081);

    Log::info("Server Started");


    Network::Poll(serv, true, [](auto& e){
        // Conn

        Log::info("New connection from {}, packet: {}, ev_data: {}", e.peer->host, e.packet, e.data);
    }, [](auto& e) {
        // Recv

        Log::info("Received {} bytes '{}'", e.packet->dataLength, e.packet->data);


        Network::SendPacket(e.peer, "I AM ok my friend");

    }, [](auto& e) {
        // Drop

        Log::info("Disconn ", e.packet);
    });

    Network::Deinit(serv);

}

int main() {




    runServer();

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
