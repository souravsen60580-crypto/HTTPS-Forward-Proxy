#pragma once

#include "cache/cache.h"
#include "security/policy_engine.h"

using namespace std;

void handleClient(
    int client_fd,
    const PolicyEngine& policy,
    Cache& cache
);