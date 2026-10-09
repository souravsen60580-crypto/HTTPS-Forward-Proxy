#pragma once

#include <string>
#include <vector>

using namespace std;

namespace Dns {

vector<string> resolve(
    const string& host,
    int port
);

}