#include "http/parser.h"

#include <cassert>
#include <iostream>

using namespace std;

int main() {

    string raw =
        "GET /hello HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: test\r\n"
        "\r\n";

    HttpRequest request =
        HttpParser::parseRequest(
            raw
        );

    assert(
        request.method == "GET"
    );

    assert(
        request.target == "/hello"
    );

    assert(
        request.version == "HTTP/1.1"
    );

    assert(
        request.header("host")
        == "example.com"
    );

    cout
        << "Parser test passed\n";

    return 0;
}