#include "server/client_handler.h"

#include "http/parser.h"
#include "logging/logger.h"
#include "network/connection.h"
#include "network/socket.h"
#include "proxy/http_proxy.h"
#include "proxy/https_proxy.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include <sys/socket.h>

using namespace std;


static string receiveRequest(
    int fd
) {

    string raw =
        recvUntilHeaderEnd(fd);

    if (raw.empty()) {

        throw runtime_error(
            "Client disconnected"
        );
    }

    HttpRequest request =
        HttpParser::parseRequest(
            raw
        );

    string contentLength =
        request.header(
            "Content-Length"
        );

    if (
        !contentLength.empty()
    ) {

        size_t expected =
            stoul(
                contentLength
            );

        size_t received =
            request.body.size();

        char buffer[8192];

        while (
            received < expected
        ) {

            size_t needed =
                min(
                    sizeof(buffer),
                    expected - received
                );

            ssize_t bytes =
                recv(
                    fd,
                    buffer,
                    needed,
                    0
                );

            if (bytes <= 0) {

                throw runtime_error(
                    "Incomplete request body"
                );
            }

            raw.append(
                buffer,
                bytes
            );

            received +=
                static_cast<size_t>(
                    bytes
                );
        }
    }

    return raw;
}


void handleClient(
    int client_fd,
    const PolicyEngine& policy,
    Cache& cache
) {

    try {

        string raw =
            receiveRequest(
                client_fd
            );

        HttpRequest request =
            HttpParser::parseRequest(
                raw
            );

        Logger::info(
            "Request: "
            + request.method
            + " "
            + request.target
        );

        string method =
            HttpParser::lower(
                request.method
            );

        if (
            method == "connect"
        ) {

            handleHttpsConnect(
                client_fd,
                request,
                policy
            );

        } else {

            handleHttpProxy(
                client_fd,
                request,
                policy,
                cache
            );
        }

    } catch (
        const exception& e
    ) {

        Logger::error(
            string(
                "Client error: "
            )
            + e.what()
        );

        string response =
            "HTTP/1.1 400 Bad Request\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        sendAll(
            client_fd,
            response
        );
    }

    closeSocket(
        client_fd
    );
}