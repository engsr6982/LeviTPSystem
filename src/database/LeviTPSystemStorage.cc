#include "levitpsystem/database/LeviTPSystemStorage.h"
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "levitpsystem/LeviTPSystem.h"
#include "mc/platform/UUID.h"
#include "mc/world/actor/player/Player.h"
#include <filesystem>


namespace tps {

LeviTPSystemStorage::LeviTPSystemStorage() = default;

LeviTPSystemStorage& LeviTPSystemStorage::getInstance() {
    static LeviTPSystemStorage instance;
    return instance;
}

SQLite::Database& LeviTPSystemStorage::getDatabase() const {
    if (!mDatabase) {
        throw std::runtime_error("Database not initialized.");
    }
    return *mDatabase;
}

void LeviTPSystemStorage::initRelationShipTable() const {
    getDatabase().exec(R"(
        CREATE TABLE IF NOT EXISTS relationship (
            player_id   INTEGER PRIMARY KEY AUTOINCREMENT,  -- 玩家ID（主键，自增）
            real_name   TEXT    NOT NULL UNIQUE,            -- 真实姓名（唯一）
            uuid        TEXT    NOT NULL UNIQUE             -- UUID（唯一）
        );
    )");
}

void LeviTPSystemStorage::init() {
    namespace fs = std::filesystem;

    fs::path path = LeviTPSystem::getInstance().getSelf().getDataDir();
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }

    mDatabase =
        std::make_unique<SQLite::Database>(path / "LeviTPSystem.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    initRelationShipTable();
}

void LeviTPSystemStorage::initPlayer(Player& player) const {
    SQLite::Statement query(getDatabase(), "INSERT OR IGNORE INTO relationship (real_name, uuid) VALUES (?, ?);");
    query.bind(1, player.getRealName());
    query.bind(2, player.getUuid().asString());
    query.exec();
}

void LeviTPSystemStorage::checkAndUpdatePlayer(Player& player) const {
    if (getPlayerId(player) == -1) {
        initPlayer(player);
    } else {
        SQLite::Statement query(getDatabase(), "UPDATE relationship SET real_name = ? WHERE uuid = ?;");
        query.bind(1, player.getRealName());
        query.bind(2, player.getUuid().asString());
        query.exec();
    }
}

PlayerID LeviTPSystemStorage::getPlayerId(std::string const& realName) const {
    SQLite::Statement query(getDatabase(), "SELECT player_id FROM relationship WHERE real_name = ?;");
    query.bind(1, realName);
    if (query.executeStep()) {
        return query.getColumn(0);
    }
    return -1;
}

PlayerID LeviTPSystemStorage::getPlayerId(mce::UUID const& uuid) const {
    SQLite::Statement query(getDatabase(), "SELECT player_id FROM relationship WHERE uuid = ?;");
    query.bind(1, uuid.asString());
    if (query.executeStep()) {
        return query.getColumn(0);
    }
    return -1;
}

PlayerID LeviTPSystemStorage::getPlayerId(Player& player) const { return getPlayerId(player.getUuid()); }

} // namespace tps
