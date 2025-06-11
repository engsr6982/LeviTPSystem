#pragma once
#include "ltps/Global.h"
#include <memory>

class Player;
namespace mce {
class UUID;
}

namespace SQLite {
class Database;
}

namespace ltps {

using PlayerID = int;

class LeviTPSystemStorage final {
private:
    std::unique_ptr<SQLite::Database> mDatabase;

    explicit LeviTPSystemStorage();

    inline void initRelationShipTable() const;

    [[nodiscard]] inline SQLite::Database& getDatabase() const;

    friend class IStorage;

public:
    TPS_DISALLOW_COPY_AND_MOVE(LeviTPSystemStorage);

    TPSNDAPI static LeviTPSystemStorage& getInstance();

    TPSAPI void init();

public:
    /**
     * @brief 初始化玩家信息
     */
    TPSAPI void initPlayer(Player& player) const;

    /**
     * @brief 检查并更新玩家信息
     */
    TPSAPI void checkAndUpdatePlayer(Player& player) const;

    /**
     * @brief 获取玩家ID，此ID为数据库中唯一标识（如果返回-1则代表不存在）
     */
    TPSNDAPI PlayerID getPlayerId(std::string const& realName) const;
    TPSNDAPI PlayerID getPlayerId(mce::UUID const& uuid) const;
    TPSNDAPI PlayerID getPlayerId(Player& player) const;

    // 辅助接口，这里的信息来自数据库
    TPSNDAPI std::optional<std::string> queryRealName(mce::UUID const& uuid) const;
    TPSNDAPI std::optional<mce::UUID> queryUUID(std::string const& realName) const;
};


} // namespace ltps
