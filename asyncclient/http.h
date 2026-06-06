#ifndef ACLIENT_HTTP_H
#define ACLIENT_HTTP_H

#include <string>

// Minimal HTTP result.
struct HttpResponse {
	long status = 0;          // HTTP status code (0 if the request itself failed)
	std::string body;         // response body
	std::string error;        // transport error message, if any
};

void httpInit();              // call once at startup (curl_global_init)
void httpCleanup();           // call once at shutdown

HttpResponse httpPostJson(const std::string &url, const std::string &jsonBody);
HttpResponse httpGet(const std::string &url);

#endif
