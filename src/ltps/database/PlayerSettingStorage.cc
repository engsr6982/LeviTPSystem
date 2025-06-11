#include "ltps/database/PlayerSettingStorage.h"
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/database/LeviTPSystemStorage.h"
#include <optional>


namespace tps {

PlayerSettingStorage::PlayerSettingStorage() = default;

// IStorage
void PlayerSettingStorage::initStorage() {
    auto& database = getDatabase();

    // database.exec(
    //     R"(
    //         CREATE TABLE IF NOT EXISTS settings (
    //             player_id     INTEGER PRIMARY KEY,                              -- 玩家ID（主键）
    //             death_popup   INTEGER NOT NULL CHECK (death_popup IN (0, 1)),   -- 是否启用死亡弹窗（布尔）
    //             allow_tpa     INTEGER NOT NULL CHECK (allow_tpa IN (0, 1)),     -- 是否允许接收传送请求
    //             tpa_popup     INTEGER NOT NULL CHECK (tpa_popup IN (0, 1)),     -- 是否显示传送请求弹窗
    //             FOREIGN KEY (player_id) REFERENCES player(player_id)
    //         )
    //     )"
    // );

    database.exec(
        R"(
            CREATE TABLE IF NOT EXISTS setting_category (
                id   INTEGER AUTOINCREMENT,         -- 设置分类ID
                name TEXT NOT NULL UNIQUE,          -- 设置分类名称
                PRIMARY KEY (id)
            );
        )"
    );
    database.exec(
        R"(
            CREATE TABLE IF NOT EXISTS settings (
                player_id    INTEGER NOT NULL,          -- 玩家ID
                category_id  INTEGER NOT NULL,          -- 设置分类ID
                value        BLOB  NOT NULL,            -- 设置值
                PRIMARY KEY (player_id, category_id),
                FOREIGN KEY (category_id) REFERENCES setting_category(id)
            )
        )"
    );

    // internal settings
    database.exec("INSERT OR IGNORE INTO setting_category ('name') VALUES ('death_popup')");
    database.exec("INSERT OR IGNORE INTO setting_category ('name') VALUES ('allow_tpa')");
    database.exec("INSERT OR IGNORE INTO setting_category ('name') VALUES ('tpa_popup')");
}


void PlayerSettingStorage::initPlayer(PlayerID id) const {
    if (id == -1) {
        LeviTPSystem::getInstance().getSelf().getLogger().error("{}: Invalid player id: {}", __func__, id);
        return;
    }

    SQLite::Statement query(
        getDatabase(),
        "INSERT OR IGNORE INTO settings (player_id, death_popup, allow_tpa, tpa_popup) VALUES (?, ?, ?, ?);"
    );
    query.bind(1, id);
    query.bind(2, 1);
    query.bind(3, 1);
    query.bind(4, 1);
    query.exec();
}
void PlayerSettingStorage::initPlayer(Player& player) const { initPlayer(getMainStorage().getPlayerId(player)); }


std::optional<SettingData> PlayerSettingStorage::getSettingData(Player& player) const {
    auto const id = getMainStorage().getPlayerId(player);
    if (id == -1) {
        return std::nullopt;
    }

    SQLite::Statement query(
        getDatabase(),
        "SELECT death_popup, allow_tpa, tpa_popup FROM settings WHERE player_id = ?;"
    );
    query.bind(1, id);
    if (query.executeStep()) {
        SettingData data;
        data.deathPopup = query.getColumn(0).getInt() != 0;
        data.allowTpa   = query.getColumn(1).getInt() != 0;
        data.tpaPopup   = query.getColumn(2).getInt() != 0;
        return data;
    }
    return std::nullopt;
}

void PlayerSettingStorage::setSettingData(Player& player, SettingData const& data) const {
    auto const id = getMainStorage().getPlayerId(player);
    if (id == -1) {
        return;
    }

    SQLite::Statement query(
        getDatabase(),
        "UPDATE settings SET death_popup = ?, allow_tpa = ?, tpa_popup = ? WHERE player_id = ?;"
    );
    query.bind(1, data.deathPopup ? 1 : 0);
    query.bind(2, data.allowTpa ? 1 : 0);
    query.bind(3, data.tpaPopup ? 1 : 0);
    query.bind(5, id);
    query.exec();
}

} // namespace tps