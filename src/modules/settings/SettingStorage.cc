#include "levitpsystem/modules/settings/SettingStorage.h"
#include "SQLiteCpp/Statement.h"
#include "levitpsystem/database/LeviTPSystemStorage.h"


namespace tps {

SettingStorage::SettingStorage() = default;

SettingStorage& SettingStorage::getInstance() {
    static SettingStorage instance;
    return instance;
}

void SettingStorage::init() {
    auto& database = getDatabase();

    // Create the settings table if it doesn't exist
    database.exec(
        R"(
            CREATE TABLE IF NOT EXISTS settings (
                player_id     INTEGER PRIMARY KEY,                              -- 玩家ID（主键）
                death_popup   INTEGER NOT NULL CHECK (death_popup IN (0, 1)),   -- 是否启用死亡弹窗（布尔）
                allow_tpa     INTEGER NOT NULL CHECK (allow_tpa IN (0, 1)),     -- 是否允许接收传送请求
                tpa_popup     INTEGER NOT NULL CHECK (tpa_popup IN (0, 1)),     -- 是否显示传送请求弹窗
                language      TEXT NOT NULL,                                    -- 玩家语言
                FOREIGN KEY (player_id) REFERENCES player(player_id)
            )
        )"
    );
}

void SettingStorage::initPlayer(PlayerID id) const {
    if (id == -1) {
        return;
    }

    SQLite::Statement query(
        getDatabase(),
        "INSERT OR IGNORE INTO settings (player_id, death_popup, allow_tpa, tpa_popup, language) VALUES (?, ?, ?, ?, "
        "?);"
    );
    query.bind(1, id);
    query.bind(2, 1);
    query.bind(3, 1);
    query.bind(4, 1);
    query.bind(5, "en_US");
    query.exec();
}
void SettingStorage::initPlayer(Player& player) const { initPlayer(getMainStorage().getPlayerId(player)); }


std::optional<SettingData> SettingStorage::getSettingData(Player& player) const {
    auto const id = getMainStorage().getPlayerId(player);
    if (id == -1) {
        return std::nullopt;
    }

    SQLite::Statement query(
        getDatabase(),
        "SELECT death_popup, allow_tpa, tpa_popup, language FROM settings WHERE player_id = ?;"
    );
    query.bind(1, id);
    if (query.executeStep()) {
        SettingData data;
        data.deathPopup = query.getColumn(0).getInt() != 0;
        data.allowTpa   = query.getColumn(1).getInt() != 0;
        data.tpaPopup   = query.getColumn(2).getInt() != 0;
        data.language   = query.getColumn(3).getText();
        return data;
    }
    return std::nullopt;
}

void SettingStorage::setSettingData(Player& player, SettingData const& data) const {
    auto const id = getMainStorage().getPlayerId(player);
    if (id == -1) {
        return;
    }

    SQLite::Statement query(
        getDatabase(),
        "UPDATE settings SET death_popup = ?, allow_tpa = ?, tpa_popup = ?, language = ? WHERE player_id = ?;"
    );
    query.bind(1, data.deathPopup ? 1 : 0);
    query.bind(2, data.allowTpa ? 1 : 0);
    query.bind(3, data.tpaPopup ? 1 : 0);
    query.bind(4, data.language);
    query.bind(5, id);
    query.exec();
}


} // namespace tps