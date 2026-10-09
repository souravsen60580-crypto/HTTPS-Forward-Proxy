#pragma once

#include "http/request.h"
#include "security/policy_engine.h"

using namespace std;

void handleHttpsConnect(
    int client_fd,
    const HttpRequest& request,
    const PolicyEngine& policy
);