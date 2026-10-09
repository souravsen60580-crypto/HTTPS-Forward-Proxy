#pragma once

#include <string>
#include <unordered_map>

using namespace std;

struct HttpResponse
{

  string version = "HTTP/1.1";

  int status = 0;

  string reason;

  unordered_map<string, string> headers;

  string body;

  string toRaw() const;
};