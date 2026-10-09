#pragma once

#include "cache/cache.h"
#include "http/request.h"
#include "security/policy_engine.h"

using namespace std;

void handleHttpProxy(
    int client_fd,
    const HttpRequest& request,
    const PolicyEngine& policy,
    Cache& cache
);