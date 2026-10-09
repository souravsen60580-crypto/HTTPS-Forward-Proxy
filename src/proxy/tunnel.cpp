#include "proxy/tunnel.h"

#include <sys/socket.h>
#include <unistd.h>
#include <thread>

using namespace std;


static void forwardBytes(
    int from,
    int to
) {

    char buffer[8192];

    while (true) {

        ssize_t bytes =
            recv(
                from,
                buffer,
                sizeof(buffer),
                0
            );

        if (bytes <= 0) {
            break;
        }

        ssize_t sent = 0;

        while (sent < bytes) {

            ssize_t n =
                send(
                    to,
                    buffer + sent,
                    bytes - sent,
                    0
                );

            if (n <= 0) {
                break;
            }

            sent += n;
        }
    }

    shutdown(
        to,
        SHUT_WR
    );
}


void tunnel(
    int client_fd,
    int server_fd
) {

    thread clientToServer(
        forwardBytes,
        client_fd,
        server_fd
    );

    thread serverToClient(
        forwardBytes,
        server_fd,
        client_fd
    );

    clientToServer.join();

    serverToClient.join();
}