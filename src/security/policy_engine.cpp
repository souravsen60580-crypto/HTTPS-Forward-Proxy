#include "security/policy_engine.h"

#include "http/parser.h"

#include <fstream>

using namespace std;


bool PolicyEngine::load(
    const string& file
) {

    ifstream input(file);

    if (!input) {
        return false;
    }

    string line;

    lock_guard<mutex> lock(
        mutex_
    );

    while (
        getline(input, line)
    ) {

        line =
            HttpParser::trim(line);

        if (
            line.empty() ||
            line[0] == '#'
        ) {
            continue;
        }

        blocked_.insert(
            HttpParser::lower(line)
        );
    }

    return true;
}


bool PolicyEngine::allowed(
    const string& host
) const {

    string hostname =
        HttpParser::lower(host);

    size_t colon =
        hostname.find(':');

    if (
        colon != string::npos
    ) {

        hostname =
            hostname.substr(
                0,
                colon
            );
    }

    lock_guard<mutex> lock(
        mutex_
    );

    return blocked_.find(
        hostname
    ) == blocked_.end();
}