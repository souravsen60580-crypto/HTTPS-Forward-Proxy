#include "network/socket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

using namespace std;


int createListenSocket(
    int port
) {

    int fd =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if (fd < 0) {
        return -1;
    }

    int yes = 1;

    setsockopt(
        fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &yes,
        sizeof(yes)
    );

    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_ANY);

    address.sin_port =
        htons(port);

    if (
        bind(
            fd,
            reinterpret_cast<sockaddr*>(
                &address
            ),
            sizeof(address)
        ) < 0
    ) {

        close(fd);

        return -1;
    }

    if (
        listen(
            fd,
            128
        ) < 0
    ) {

        close(fd);

        return -1;
    }

    return fd;
}


int connectToHost(
    const char* host,
    int port,
    int /* timeout_seconds */
) {

    addrinfo hints{};

    hints.ai_family =
        AF_UNSPEC;

    hints.ai_socktype =
        SOCK_STREAM;

    addrinfo* results =
        nullptr;

    string portString =
        to_string(port);

    int result =
        getaddrinfo(
            host,
            portString.c_str(),
            &hints,
            &results
        );

    if (result != 0) {
        return -1;
    }

    int socketFd = -1;

    for (
        addrinfo* current = results;
        current != nullptr;
        current = current->ai_next
    ) {

        socketFd =
            socket(
                current->ai_family,
                current->ai_socktype,
                current->ai_protocol
            );

        if (socketFd < 0) {
            continue;
        }

        if (
            connect(
                socketFd,
                current->ai_addr,
                current->ai_addrlen
            ) == 0
        ) {

            break;
        }

        close(socketFd);

        socketFd = -1;
    }

    freeaddrinfo(results);

    return socketFd;
}


void closeSocket(
    int fd
) {

    if (fd >= 0) {
        close(fd);
    }
}