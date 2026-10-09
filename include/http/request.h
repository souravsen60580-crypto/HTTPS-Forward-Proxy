#pragma once

#include <string>
#include <unordered_map>

using namespace std;

struct HttpRequest
{

  string method;

  string target;

  string version;

  unordered_map<string, string> headers;

  string body;

  string header(const string &name) const;
};