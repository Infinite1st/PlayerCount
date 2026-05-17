#include "PCH.h"
#include "HttpClient.h"
#include <curl/curl.h>

namespace
{
    struct CurlGlobal
    {
        CurlGlobal()  { curl_global_init(CURL_GLOBAL_DEFAULT); }
        ~CurlGlobal() { curl_global_cleanup(); }
    };

    CurlGlobal& EnsureInit()
    {
        static CurlGlobal g;
        return g;
    }

    std::size_t WriteCallback(char* ptr, std::size_t size, std::size_t n, void* userdata)
    {
        static_cast<std::string*>(userdata)->append(ptr, size * n);
        return size * n;
    }
}

HttpClient::HttpClient()
{
    EnsureInit();
    m_curl = curl_easy_init();
}

HttpClient::~HttpClient()
{
    if (m_curl)
        curl_easy_cleanup(static_cast<CURL*>(m_curl));
}

HttpResult HttpClient::Get(const std::string& url, long timeoutSec) noexcept
{
    HttpResult r;
    if (!m_curl) { r.error = "curl_easy_init failed"; return r; }

    auto* c = static_cast<CURL*>(m_curl);
    std::string body;

    curl_easy_reset(c);
    curl_easy_setopt(c, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION,  WriteCallback);
    curl_easy_setopt(c, CURLOPT_WRITEDATA,      &body);
    curl_easy_setopt(c, CURLOPT_TIMEOUT,        timeoutSec);
    curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_USERAGENT,      "PlayerCount-SKSE/1.0");
    curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode rc = curl_easy_perform(c);
    if (rc != CURLE_OK) {
        r.error = curl_easy_strerror(rc);
        return r;
    }

    long code = 0;
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
    if (code < 200 || code >= 300) {
        r.error = "HTTP " + std::to_string(code);
        return r;
    }

    r.ok   = true;
    r.body = std::move(body);
    return r;
}
