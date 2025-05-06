#pragma once
#include "SQLiteCpp/Database.h"
#include "levitpsystem/Global.h"
#include <memory>

class Player;
namespace mce {
class UUID;
}


namespace tps {

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
     * @brief Initialize the player information in the database.
     */
    TPSAPI void initPlayer(Player& player) const;

    /**
     * @brief Check and update the player information in the database.
     */
    TPSAPI void checkAndUpdatePlayer(Player& player) const;

    /**
     * @brief Get the player id, if not found, return -1.
     */
    TPSNDAPI PlayerID getPlayerId(std::string const& realName) const;
    TPSNDAPI PlayerID getPlayerId(mce::UUID const& uuid) const;
    TPSNDAPI PlayerID getPlayerId(Player& player) const;
};


} // namespace tps
