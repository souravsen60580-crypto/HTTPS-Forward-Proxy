#include "http/parser.h"

#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace HttpParser {

string lower(string s) {

    for (char& c : s) {

        c = static_cast<char>(
            tolower(
                static_cast<unsigned char>(c)
            )
        );
    }

    return s;
}


string trim(string s) {

    size_t start = 0;
    size_t end = s.size();

    while (
        start < end &&
        isspace(
            static_cast<unsigned char>(
                s[start]
            )
        )
    ) {
        start++;
    }

    while (
        end > start &&
        isspace(
            static_cast<unsigned char>(
                s[end - 1]
            )
        )
    ) {
        end--;
    }

    return s.substr(
        start,
        end - start
    );
}


HttpRequest parseRequest(
    const string& raw
) {

    size_t headerEnd =
        raw.find("\r\n\r\n");

    if (
        headerEnd == string::npos
    ) {

        throw runtime_error(
            "Incomplete HTTP headers"
        );
    }

    HttpRequest request;

    string headerPart =
        raw.substr(
            0,
            headerEnd
        );

    istringstream stream(
        headerPart
    );

    string line;

    // -------------------------
    // Request line
    // -------------------------

    if (!getline(stream, line)) {

        throw runtime_error(
            "Empty HTTP request"
        );
    }

    if (
        !line.empty() &&
        line.back() == '\r'
    ) {
        line.pop_back();
    }

    istringstream requestLine(line);

    if (
        !(requestLine
            >> request.method
            >> request.target
            >> request.version)
    ) {

        throw runtime_error(
            "Invalid HTTP request line"
        );
    }

    // -------------------------
    // Headers
    // -------------------------

    while (getline(stream, line)) {

        if (
            !line.empty() &&
            line.back() == '\r'
        ) {
            line.pop_back();
        }

        if (line.empty()) {
            break;
        }

        size_t colon =
            line.find(':');

        if (
            colon == string::npos
        ) {
            continue;
        }

        string key =
            trim(
                line.substr(
                    0,
                    colon
                )
            );

        string value =
            trim(
                line.substr(
                    colon + 1
                )
            );

        request.headers[key] =
            value;
    }

    // -------------------------
    // Body
    // -------------------------

    request.body =
        raw.substr(
            headerEnd + 4
        );

    return request;
}


string serializeRequest(
    const HttpRequest& request,
    bool origin_form
) {

    string target =
        request.target;

    /*
        Proxy request:

        GET http://example.com/index.html HTTP/1.1

        Origin server usually expects:

        GET /index.html HTTP/1.1
    */

    if (origin_form) {

        size_t scheme =
            target.find("://");

        if (
            scheme != string::npos
        ) {

            size_t slash =
                target.find(
                    '/',
                    scheme + 3
                );

            if (
                slash == string::npos
            ) {

                target = "/";

            } else {

                target =
                    target.substr(
                        slash
                    );
            }
        }
    }

    ostringstream out;

    out << request.method
        << " "
        << target
        << " "
        << request.version
        << "\r\n";

    for (
        const auto& [key, value]
        : request.headers
    ) {

        string lowerKey =
            lower(key);

        if (
            lowerKey ==
            "proxy-connection"
        ) {
            continue;
        }

        if (
            lowerKey ==
            "connection"
        ) {
            continue;
        }

        out << key
            << ": "
            << value
            << "\r\n";
    }

    out << "Connection: close\r\n";

    out << "\r\n";

    out << request.body;

    return out.str();
}

}