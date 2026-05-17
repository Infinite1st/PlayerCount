#pragma once
#include <string>

struct HttpResult
{
    bool        ok{ false };
    std::string body;
    std::string error;
};

class HttpClient
{
public:
    HttpClient();
    ~HttpClient();
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;

    HttpResult Get(const std::string& url, long timeoutSec = 10) noexcept;

private:
    void* m_curl{ nullptr };
};
