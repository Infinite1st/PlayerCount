#include "PCH.h"
#include "SteamPlayerService.h"
#include "Config.h"
#include "HttpClient.h"
#include <nlohmann/json.hpp>

void SteamPlayerService::Start()
{
    bool exp = false;
    if (!m_running.compare_exchange_strong(exp, true)) return;
    m_thread = std::thread(&SteamPlayerService::Loop, this);
}

void SteamPlayerService::Stop()
{
    if (!m_running.exchange(false)) return;
    m_cv.notify_all();
    if (m_thread.joinable()) m_thread.join();
}

void SteamPlayerService::Loop()
{
    while (m_running.load()) {
        Fetch();

        int minutes = Config::Get().GetIntervalMinutes();
        std::unique_lock lk(m_wakeMx);
        m_cv.wait_for(lk, std::chrono::minutes(minutes),
                      [this] { return !m_running.load(); });
    }
}

void SteamPlayerService::Fetch()
{
    HttpClient client;
    auto res = client.Get(kUrl, 10);

    if (!res.ok) {
        Callback cb;
        { std::lock_guard l(m_cbMx); cb = m_cb; }
        if (cb) cb(-1, false);
        return;
    }

    try {
        auto j     = nlohmann::json::parse(res.body);
        int  count = j.at("response").at("player_count").get<int>();
        Callback cb;
        { std::lock_guard l(m_cbMx); cb = m_cb; }
        if (cb) cb(count, true);
    } catch (const std::exception& ex) {
        Callback cb;
        { std::lock_guard l(m_cbMx); cb = m_cb; }
        if (cb) cb(-1, false);
    }
}
