#pragma once

#include <string>
#include <sys/types.h>

using namespace std;

ssize_t sendAll(
    int fd,
    const void* data,
    size_t length
);

ssize_t sendAll(
    int fd,
    const string& data
);

string recvUntilHeaderEnd(
    int fd,
    size_t max_bytes = 1024 * 1024
);

string recvUntilClose(
    int fd,
    size_t max_bytes = 10 * 1024 * 1024
);