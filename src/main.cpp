#include "cache/cache.h"
#include "logging/logger.h"
#include "security/policy_engine.h"
#include "server/tcp_server.h"

#include <exception>

using namespace std;

int main() {

    const int port = 8080;

    PolicyEngine policy;

    if (
        !policy.load(
            "config/blocked_domains.txt"
        )
    ) {

        Logger::warn(
            "blocked_domains.txt not found. "
            "No domains will be blocked."
        );
    }

    Cache cache(
        30
    );

    try {

        TcpServer server(
            port,
            policy,
            cache
        );

        server.run();

    } catch (
        const exception& e
    ) {

        Logger::error(
            e.what()
        );

        return 1;
    }

    return 0;
}