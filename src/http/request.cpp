#include "http/request.h"
#include "http/parser.h"

using namespace std;

string HttpRequest::header(
    const string& name
) const {

    string wanted =
        HttpParser::lower(name);

    for (const auto& [key, value] : headers) {

        if (HttpParser::lower(key) == wanted) {
            return value;
        }
    }

    return "";
}