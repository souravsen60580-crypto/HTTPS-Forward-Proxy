#include "logging/logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>

using namespace std;

namespace {

mutex logMutex;


void log(
    const char* level,
    const string& message
) {

    lock_guard<mutex> lock(
        logMutex
    );

    auto now =
        chrono::system_clock::now();

    auto time =
        chrono::system_clock::to_time_t(
            now
        );

    tm localTime{};

    localtime_r(
        &time,
        &localTime
    );

    cout
        << put_time(
            &localTime,
            "[%Y-%m-%d %H:%M:%S] "
        )
        << level
        << " "
        << message
        << '\n';
}

}


namespace Logger {

void info(
    const string& message
) {

    log(
        "[INFO]",
        message
    );
}


void warn(
    const string& message
) {

    log(
        "[WARN]",
        message
    );
}


void error(
    const string& message
) {

    log(
        "[ERROR]",
        message
    );
}

}