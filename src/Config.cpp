#include "PCH.h"
#include "Config.h"
#include <SimpleIni.h>
#include <filesystem>

void Config::Load()
{
    auto dir = SKSE::log::log_directory();
    if (!dir) return;

    std::filesystem::path path = dir->parent_path() / "Plugins" / "PlayerCount.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    if (ini.LoadFile(path.string().c_str()) < 0) {
        // Файла нет — создаём с дефолтом
        ini.SetLongValue("General", "IntervalMinutes", 5,
            "; Интервал между уведомлениями в минутах (1-60)");
        ini.SaveFile(path.string().c_str());
        logger::info("Config: created default PlayerCount.ini");
        return;
    }

    int v = static_cast<int>(ini.GetLongValue("General", "IntervalMinutes", 5));
    m_interval.store(std::clamp(v, 1, 60));
    logger::info("Config: IntervalMinutes = {}", m_interval.load());
}
