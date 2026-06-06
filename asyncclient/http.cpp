#include "http.h"

#include <curl/curl.h>

// Collect the response body into a std::string.
static size_t writeCb(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string *out = static_cast<std::string *>(userdata);
	out->append(ptr, size * nmemb);
	return size * nmemb;
}

void httpInit() {
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

void httpCleanup() {
	curl_global_cleanup();
}

// Shared request helper. If jsonBody is non-empty the request is a POST.
static HttpResponse request(const std::string &url, const std::string &jsonBody, bool post) {
	HttpResponse resp;
	CURL *curl = curl_easy_init();
	if (!curl) {
		resp.error = "curl init failed";
		return resp;
	}

	std::string out;
	struct curl_slist *headers = nullptr;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

	if (post) {
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)jsonBody.size());
	}

	CURLcode rc = curl_easy_perform(curl);
	if (rc == CURLE_OK) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.status);
		resp.body = out;
	} else {
		resp.error = curl_easy_strerror(rc);
	}

	if (headers)
		curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	return resp;
}

HttpResponse httpPostJson(const std::string &url, const std::string &jsonBody) {
	return request(url, jsonBody, true);
}

HttpResponse httpGet(const std::string &url) {
	return request(url, "", false);
}
