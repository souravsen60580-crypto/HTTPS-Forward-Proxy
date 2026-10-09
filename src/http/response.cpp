#include "http/response.h"

#include <sstream>

using namespace std;

string HttpResponse::toRaw() const
{

  ostringstream out;

  out << version<< " "<< status<< " "<< reason<< "\r\n";

  for (const auto &[key, value] : headers)
  {

    out << key<< ": "<< value<< "\r\n";
  }

  out << "\r\n";

  out << body;

  return out.str();
}