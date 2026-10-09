#pragma once

int createListenSocket(
    int port
);

int connectToHost(
    const char* host,
    int port,
    int timeout_seconds = 5
);

void closeSocket(
    int fd
);