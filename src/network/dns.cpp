#include "network/dns.h"

#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

namespace Dns {

vector<string> resolve(
    const string& host,
    int port
) {

    vector<string> addresses;

    addrinfo hints{};

    hints.ai_socktype =
        SOCK_STREAM;

    hints.ai_family =
        AF_UNSPEC;

    addrinfo* results =
        nullptr;

    string portString =
        to_string(port);

    if (
        getaddrinfo(
            host.c_str(),
            portString.c_str(),
            &hints,
            &results
        ) != 0
    ) {

        return addresses;
    }

    char buffer[
        INET6_ADDRSTRLEN
    ];

    for (
        addrinfo* current = results;
        current != nullptr;
        current = current->ai_next
    ) {

        void* address =
            nullptr;

        if (
            current->ai_family ==
            AF_INET
        ) {

            address =
                &(
                    reinterpret_cast<
                        sockaddr_in*
                    >(current->ai_addr)
                    ->sin_addr
                );

        } else if (
            current->ai_family ==
            AF_INET6
        ) {

            address =
                &(
                    reinterpret_cast<
                        sockaddr_in6*
                    >(current->ai_addr)
                    ->sin6_addr
                );
        }

        if (
            address != nullptr &&
            inet_ntop(
                current->ai_family,
                address,
                buffer,
                sizeof(buffer)
            )
        ) {

            addresses.push_back(
                buffer
            );
        }
    }

    freeaddrinfo(results);

    return addresses;
}

}