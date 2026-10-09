#pragma once

#include "http/request.h"
#include "http/response.h"

#include <string>

using namespace std;

namespace HttpParser
{

  string lower(string s);

  string trim(string s);

  HttpRequest parseRequest(
      const string &raw);

  string serializeRequest(
      const HttpRequest &request,
      bool origin_form = true);

}