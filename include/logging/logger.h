#pragma once

#include <string>

using namespace std;

namespace Logger {

void info(
    const string& message
);

void warn(
    const string& message
);

void error(
    const string& message
);

}