#pragma once

#include "cache/cache.h"
#include "security/policy_engine.h"

using namespace std;

class TcpServer {

private:

    int port_;

    int server_fd_;

    const PolicyEngine& policy_;

    Cache& cache_;

public:

    TcpServer(
        int port,
        const PolicyEngine& policy,
        Cache& cache
    );

    void run();
};