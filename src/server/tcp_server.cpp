#include "server/tcp_server.h"

#include "logging/logger.h"
#include "network/socket.h"
#include "server/client_handler.h"

#include <stdexcept>
#include <string>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;


TcpServer::TcpServer(
    int port,
    const PolicyEngine& policy,
    Cache& cache
)
    : port_(port),
      server_fd_(-1),
      policy_(policy),
      cache_(cache) {
}


void TcpServer::run() {

    server_fd_ =
        createListenSocket(
            port_
        );

    if (server_fd_ < 0) {

        throw runtime_error(
            "Failed to create listening socket"
        );
    }

    Logger::info(
        "Proxy listening on port "
        + to_string(port_)
    );

    while (true) {

        sockaddr_in clientAddress{};

        socklen_t clientLength =
            sizeof(clientAddress);

        int clientFd =
            accept(
                server_fd_,
                reinterpret_cast<
                    sockaddr*
                >(
                    &clientAddress
                ),
                &clientLength
            );

        if (clientFd < 0) {
            continue;
        }

        /*
         * Create one thread
         * for each client.
         */

        thread(
            handleClient,
            clientFd,
            cref(policy_),
            ref(cache_)
        ).detach();
    }
}