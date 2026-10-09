#include "proxy/http_proxy.h"

#include "http/parser.h"
#include "logging/logger.h"
#include "network/connection.h"
#include "network/socket.h"

#include <stdexcept>
#include <string>

using namespace std;


static bool splitHostPort(
    const string& input,
    string& host,
    int& port,
    int defaultPort
) {

    if (input.empty()) {
        return false;
    }

    // IPv6: [::1]:8080

    if (input.front() == '[') {

        size_t end =
            input.find(']');

        if (
            end == string::npos
        ) {
            return false;
        }

        host =
            input.substr(
                1,
                end - 1
            );

        port =
            defaultPort;

        if (
            end + 1 < input.size() &&
            input[end + 1] == ':'
        ) {

            port =
                stoi(
                    input.substr(
                        end + 2
                    )
                );
        }

        return true;
    }

    size_t colon =
        input.rfind(':');

    if (
        colon != string::npos &&
        input.find(':') == colon
    ) {

        host =
            input.substr(
                0,
                colon
            );

        port =
            stoi(
                input.substr(
                    colon + 1
                )
            );

    } else {

        host = input;

        port = defaultPort;
    }

    return
        !host.empty() &&
        port > 0 &&
        port < 65536;
}


void handleHttpProxy(
    int client_fd,
    const HttpRequest& request,
    const PolicyEngine& policy,
    Cache& cache
) {

    string host =
        request.header("Host");

    if (host.empty()) {

        sendAll(
            client_fd,
            "HTTP/1.1 400 Bad Request\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n"
            "\r\n"
        );

        return;
    }

    string hostname;

    int port = 80;

    try {

        if (
            !splitHostPort(
                host,
                hostname,
                port,
                80
            )
        ) {

            throw runtime_error(
                "Invalid host"
            );
        }

    } catch (...) {

        sendAll(
            client_fd,
            "HTTP/1.1 400 Bad Request\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n"
            "\r\n"
        );

        return;
    }

    // -------------------------
    // Security
    // -------------------------

    if (
        !policy.allowed(hostname)
    ) {

        Logger::warn(
            "Blocked HTTP host: "
            + hostname
        );

        sendAll(
            client_fd,
            "HTTP/1.1 403 Forbidden\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n"
            "\r\n"
        );

        return;
    }

    // -------------------------
    // Cache
    // -------------------------

    string cacheKey =
        request.method
        + " "
        + hostname
        + ":"
        + to_string(port)
        + request.target;

    if (
        request.method == "GET"
    ) {

        string cached;

        if (
            cache.get(
                cacheKey,
                cached
            )
        ) {

            Logger::info(
                "Cache HIT: "
                + cacheKey
            );

            sendAll(
                client_fd,
                cached
            );

            return;
        }

        Logger::info(
            "Cache MISS: "
            + cacheKey
        );
    }

    // -------------------------
    // Connect to server
    // -------------------------

    int upstream =
        connectToHost(
            hostname.c_str(),
            port
        );

    if (upstream < 0) {

        sendAll(
            client_fd,
            "HTTP/1.1 502 Bad Gateway\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n"
            "\r\n"
        );

        return;
    }

    // -------------------------
    // Forward request
    // -------------------------

    string outgoing =
        HttpParser::serializeRequest(
            request,
            true
        );

    if (
        sendAll(
            upstream,
            outgoing
        ) < 0
    ) {

        closeSocket(upstream);

        return;
    }

    // -------------------------
    // Receive response
    // -------------------------

    string response;

    try {

        response =
            recvUntilClose(
                upstream
            );

    } catch (...) {

        closeSocket(upstream);

        sendAll(
            client_fd,
            "HTTP/1.1 502 Bad Gateway\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n"
            "\r\n"
        );

        return;
    }

    closeSocket(upstream);

    // -------------------------
    // Cache GET response
    // -------------------------

    if (
        request.method == "GET" &&
        response.size()
            <= 10 * 1024 * 1024
    ) {

        cache.put(
            cacheKey,
            response
        );
    }

    // -------------------------
    // Send response to client
    // -------------------------

    sendAll(
        client_fd,
        response
    );

    Logger::info(
        "HTTP proxied: "
        + hostname
        + " "
        + request.target
    );
}