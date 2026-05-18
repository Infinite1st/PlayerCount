#include "PCH.h"
#include "Config.h"
#include "SteamPlayerService.h"

namespace
{
    void OnFetch(int count, bool connected)
    {
        std::string msg = connected
            ? "Players in-game: " + std::to_string(count)
            : "Players in-game: N/A";

        if (auto* task = SKSE::GetTaskInterface()) {
            task->AddTask([msg]() { RE::DebugNotification(msg.c_str()); });
        }
    }

    void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
    {
        if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
            SteamPlayerService::Get().SetCallback(OnFetch);
            SteamPlayerService::Get().Start();
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);

    Config::Get().Load();

    auto* messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener("SKSE", MessageHandler)) {
        return false;
    }

    return true;
}
