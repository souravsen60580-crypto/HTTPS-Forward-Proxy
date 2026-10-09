//this domain handles domain filtering

#pragma once

#include <mutex>
#include <string>
#include <unordered_set>

using namespace std;

class PolicyEngine {

private:

    unordered_set<string> blocked_;

    mutable mutex mutex_;

public:

    bool load(
        const string& file
    );

    bool allowed(
        const string& host
    ) const;
};