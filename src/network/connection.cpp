#include "network/connection.h"

#include <sys/socket.h>

#include <stdexcept>

using namespace std;


ssize_t sendAll(
    int fd,
    const void* data,
    size_t length
) {

    const char* pointer =
        static_cast<const char*>(
            data
        );

    size_t sent = 0;

    while (sent < length) {

        ssize_t result =
            send(
                fd,
                pointer + sent,
                length - sent,
                0
            );

        if (result <= 0) {
            return result;
        }

        sent += result;
    }

    return static_cast<ssize_t>(
        sent
    );
}


ssize_t sendAll(
    int fd,
    const string& data
) {

    return sendAll(
        fd,
        data.data(),
        data.size()
    );
}


string recvUntilHeaderEnd(
    int fd,
    size_t max_bytes
) {

    string result;

    char buffer[8192];

    while (
        result.find("\r\n\r\n")
        == string::npos
    ) {

        ssize_t bytes =
            recv(
                fd,
                buffer,
                sizeof(buffer),
                0
            );

        if (bytes <= 0) {
            break;
        }

        result.append(
            buffer,
            bytes
        );

        if (
            result.size() >
            max_bytes
        ) {

            throw runtime_error(
                "HTTP headers too large"
            );
        }
    }

    return result;
}


string recvUntilClose(
    int fd,
    size_t max_bytes
) {

    string result;

    char buffer[16384];

    while (true) {

        ssize_t bytes =
            recv(
                fd,
                buffer,
                sizeof(buffer),
                0
            );

        if (bytes <= 0) {
            break;
        }

        result.append(
            buffer,
            bytes
        );

        if (
            result.size() >
            max_bytes
        ) {

            throw runtime_error(
                "Response too large"
            );
        }
    }

    return result;
}