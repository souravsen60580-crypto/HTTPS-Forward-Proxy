#pragma once

#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

using namespace std;

class Cache {

private:

    struct Entry {

        string response;

        chrono::steady_clock::time_point
            expiry;
    };

    unordered_map<string, Entry> data_;

    mutable mutex mutex_;

    int ttl_;

public:

    explicit Cache(
        int ttl_seconds = 30
    );

    bool get(
        const string& key,
        string& value
    );

    void put(
        const string& key,
        const string& value
    );
};