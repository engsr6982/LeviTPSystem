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
            uuid        TEXT             UNIQUE             -- UUID（唯一，由于遗留性问题，UUID可空）
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
        auto realNameQuery = getPlayerId(player.getRealName());
        auto uuidQuery     = getPlayerId(player.getUuid());

        // 旧数据采用的 realName 作为唯一标识符 uuid 为空，优先使用 realName 查询 ID，其次使用 uuid 查询 ID
        // 查询到 ID 后，更新 realName 和 uuid
        auto id = realNameQuery != -1 ? realNameQuery : uuidQuery;
        if (id != -1) {
            SQLite::Statement query(
                getDatabase(),
                "UPDATE relationship SET real_name = ?, uuid = ? WHERE player_id = ?;"
            );
            query.bind(1, player.getRealName());
            query.bind(2, player.getUuid().asString());
            query.bind(3, id);
            query.exec();
        }
    }
}

PlayerID LeviTPSystemStorage::getPlayerId(std::string const& realName) const {
    SQLite::Statement query(getDatabase(), "SELECT player_id FROM relationship WHERE real_name = ?;");
    query.bind(1, realName);
    if (query.executeStep()) {
        return static_cast<PlayerID>(query.getColumn(0).getInt());
    }
    return -1;
}

PlayerID LeviTPSystemStorage::getPlayerId(mce::UUID const& uuid) const {
    SQLite::Statement query(getDatabase(), "SELECT player_id FROM relationship WHERE uuid = ?;");
    query.bind(1, uuid.asString());
    if (query.executeStep()) {
        return static_cast<PlayerID>(query.getColumn(0).getInt());
    }
    return -1;
}

PlayerID LeviTPSystemStorage::getPlayerId(Player& player) const { return getPlayerId(player.getUuid()); }

std::optional<std::string> LeviTPSystemStorage::queryRealName(mce::UUID const& uuid) const {
    SQLite::Statement query(getDatabase(), "SELECT real_name FROM relationship WHERE uuid =?;");
    query.bind(1, uuid.asString());
    if (query.executeStep()) {
        return query.getColumn(0).getString();
    }
    return std::nullopt;
}
std::optional<mce::UUID> LeviTPSystemStorage::queryUUID(std::string const& realName) const {
    SQLite::Statement query(getDatabase(), "SELECT uuid FROM relationship WHERE real_name =?;");
    query.bind(1, realName);
    if (query.executeStep()) {
        return mce::UUID::fromString(query.getColumn(0).getString());
    }
    return std::nullopt;
}


} // namespace tps
