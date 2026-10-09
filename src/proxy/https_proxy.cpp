#include "proxy/https_proxy.h"

#include "logging/logger.h"
#include "network/connection.h"
#include "network/socket.h"
#include "proxy/tunnel.h"

#include <string>

using namespace std;


static bool splitConnectTarget(
    const string& input,
    string& host,
    int& port
) {

    if (input.empty()) {
        return false;
    }

    // IPv6: [::1]:443

    if (input.front() == '[') {

        size_t end =
            input.find(']');

        if (
            end == string::npos ||
            end + 1 >= input.size() ||
            input[end + 1] != ':'
        ) {

            return false;
        }

        host =
            input.substr(
                1,
                end - 1
            );

        port =
            stoi(
                input.substr(
                    end + 2
                )
            );

        return true;
    }

    size_t colon =
        input.rfind(':');

    if (
        colon == string::npos
    ) {

        return false;
    }

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

    return
        !host.empty() &&
        port > 0 &&
        port < 65536;
}


void handleHttpsConnect(
    int client_fd,
    const HttpRequest& request,
    const PolicyEngine& policy
) {

    string host;

    int port = 0;

    try {

        if (
            !splitConnectTarget(
                request.target,
                host,
                port
            )
        ) {

            sendAll(
                client_fd,
                "HTTP/1.1 400 Bad Request\r\n"
                "Connection: close\r\n"
                "Content-Length: 0\r\n"
                "\r\n"
            );

            return;
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
    // Security policy
    // -------------------------

    if (
        !policy.allowed(host)
    ) {

        Logger::warn(
            "Blocked HTTPS host: "
            + host
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
    // Connect to server
    // -------------------------

    int upstream =
        connectToHost(
            host.c_str(),
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
    // Tell client tunnel exists
    // -------------------------

    string established =
        "HTTP/1.1 200 Connection Established\r\n"
        "\r\n";

    if (
        sendAll(
            client_fd,
            established
        ) < 0
    ) {

        closeSocket(upstream);

        return;
    }

    Logger::info(
        "HTTPS tunnel established: "
        + host
        + ":"
        + to_string(port)
    );

    // -------------------------
    // Tunnel encrypted data
    // -------------------------

    tunnel(
        client_fd,
        upstream
    );

    closeSocket(
        upstream
    );
}