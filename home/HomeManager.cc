#include "HomeManager.h"
#include "config/Config.h"
#include "data/LevelDB.h"
#include "mc/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "modules/Moneys.h"
#include <memory>
#include <utility>
#include <vector>


namespace lbm::plugin::tpsystem::home {


HomeManager& HomeManager::getInstance() {
    static HomeManager instance;
    return instance;
}


void HomeManager::syncToLevelDB() {
    // 同步到leveldb，传递引用
    data::LevelDB::getInstance().setHome(*this->mHomeData.get());
}

void HomeManager::syncFromLevelDB() {
    auto homeData   = data::LevelDB::getInstance().getHome();
    this->mHomeData = std::make_unique<data::Home>(homeData);
}

bool HomeManager::hasPlayerVector(const string& realName) {
    auto pl = mHomeData->find(realName);
    return pl != mHomeData->end();
}

bool HomeManager::hasPlayerHomeData(const string& realName, const string& homeName) {
    auto pl = mHomeData->find(realName);
    if (pl != mHomeData->end()) {
        auto pl_home_vec = pl->second;
        return std::find_if(
                   pl_home_vec.begin(),
                   pl_home_vec.end(),
                   [homeName](const data::HomeItem& it) { return it.name == homeName; }
               )
            != pl_home_vec.end();
    }
    return false;
}

bool HomeManager::initPlayerHomeVector(const string& realName) {
    if (hasPlayerVector(realName)) {
        return true; // 已经有这个玩家
    }
    mHomeData.get()->emplace(string(realName), std::vector<data::HomeItem>());
    syncToLevelDB(); // 同步到leveldb

#ifdef DEBUG
    std::cout << "HomeManager::initPlayerHomeVector.realName = " << realName << std::endl;
#endif

    return true;
}

std::optional<data::HomeItem> HomeManager::getPlayerHomeData(const string& realName, const string& homeName) {
    auto pl = mHomeData->find(realName);
    if (pl != mHomeData->end()) {
        auto pl_home_vec = pl->second;
        auto it          = std::find_if(pl_home_vec.begin(), pl_home_vec.end(), [homeName](const data::HomeItem& it) {
            return it.name == homeName;
        });
        if (it != pl_home_vec.end()) {
            return *it;
        }
    }
    return std::nullopt;
}

bool HomeManager::setPlayerHomeData(const string& realName, const string& homeName, const data::HomeItem newData) {
    auto pl = mHomeData->find(realName);
    if (pl != mHomeData->end()) {
        auto pl_home_vec = pl->second;
        auto it          = std::find_if(pl_home_vec.begin(), pl_home_vec.end(), [homeName](const data::HomeItem& it) {
            return it.name == homeName;
        });
        if (it != pl_home_vec.end()) {
            *it = newData;
            syncToLevelDB(); // 同步到leveldb
            return true;
        }
    }
    return false;
}

bool HomeManager::updatePlayerHomeData(const string& realName, const string& homeName, const data::HomeItem newData) {
    auto pl = mHomeData->find(realName);
    // 先查找是否有这个玩家
    if (pl != mHomeData->end()) {
        // 再查找这个玩家是否有这个home
        auto& pl_home_vec = pl->second;
        auto  it          = std::find_if(pl_home_vec.begin(), pl_home_vec.end(), [homeName](const data::HomeItem& it) {
            return it.name == homeName;
        });
        // 如果有这个home，则更新
        if (it != pl_home_vec.end()) {
            *it = newData;
            syncToLevelDB(); // 同步到leveldb
            return true;
        }
    }
    return false; // 没有这个玩家
}

bool HomeManager::updatePlayerHomeData(
    Player&              player,
    const string&        homeName,
    const data::HomeItem newData,
    bool                 ignoreMoneys
) {
    auto& mon = modules::Moneys::getInstance();
    if (ignoreMoneys) { // 忽略经济系统
        return updatePlayerHomeData(player.getRealName(), string(homeName), newData);
    } else if (mon.reduceMoney(player, config::cfg.Home.EditHomeMoney)) { // 扣钱
        return updatePlayerHomeData(player.getRealName(), string(homeName), newData);
    }

#ifdef DEBUG
    std::cout << "HomeManager::updatePlayerHomeData[overload: player]: return false, money not enough" << std::endl;
#endif

    return false; // 钱不够
}

bool HomeManager::createHome(const string& realName, const string& homeName, const data::Vec4 vec4) {
    initPlayerHomeVector(realName);
    if (hasPlayerHomeData(realName, homeName)) {
        return false; // 已经有这个home
    }
    auto pl = mHomeData->find(realName);
    if (pl != mHomeData->end()) {
        auto& pl_home_vec = pl->second;

        // 限制最大家园数量
        if (static_cast<int>(pl_home_vec.size()) >= config::cfg.Home.MaxHome) {
            utils::mc::sendText<utils::mc::MsgLevel::Error>(
                realName,
                "创建家园传送点\"{}\"失败！\n最大家园数量：{}"_tr(homeName, config::cfg.Home.MaxHome)
            );
            return false; // 超过最大数量
        }

        // 添加家园传送点
        pl_home_vec.emplace_back(
            data::HomeItem(vec4.x, vec4.y, vec4.z, vec4.dimid, utils::Date{}.toString(), "", string(homeName))
        );
        syncToLevelDB(); // 同步到leveldb
        return true;
    }
    return false; // 没有这个玩家
}

bool HomeManager::createHome(Player& player, const string& homeName, const data::Vec4 vec4, bool ignoreMoneys) {
    auto& mon = modules::Moneys::getInstance();
    if (ignoreMoneys) {
        return createHome(player.getRealName(), string(homeName), vec4);
    } else if (mon.reduceMoney(player, config::cfg.Home.CreatHomeMoney)) {
        return createHome(player.getRealName(), string(homeName), vec4);
    }

#ifdef DEBUG
    std::cout << "HomeManager::createHome[overload: player]: return false, money not enough" << std::endl;
#endif

    return false;
}

bool HomeManager::deleteHome(const string& realName, const string& homeName) {
    auto pl = mHomeData->find(realName);
    if (pl != mHomeData->end()) {
        auto& pl_home_vec = pl->second;
        pl_home_vec.erase(
            std::remove_if(
                pl_home_vec.begin(),
                pl_home_vec.end(),
                [homeName](const data::HomeItem& it) { return it.name == homeName; }
            ),
            pl_home_vec.end()
        );
        syncToLevelDB(); // 同步到leveldb
        return true;
    }
    return false; // 没有这个玩家
}

bool HomeManager::deleteHome(Player& player, const string& homeName, bool ignoreMoneys) {
    auto& mon = modules::Moneys::getInstance();
    if (ignoreMoneys) {
        return deleteHome(player.getRealName(), string(homeName));
    } else if (mon.reduceMoney(player, config::cfg.Home.DeleteHomeMoney)) {
        return deleteHome(player.getRealName(), string(homeName));
    }

#ifdef DEBUG
    std::cout << "HomeManager::deleteHome[overload: player]: return false, money not enough" << std::endl;
#endif

    return false;
}

bool HomeManager::teleportToHome(Player& player, const string& homeName, bool ignoreMoneys) {
    auto& mon = modules::Moneys::getInstance();
    if (ignoreMoneys == false) {
        if (mon.reduceMoney(player, config::cfg.Home.GoHomeMoney) == false) {
            return false; // 钱不够
        }
    }
    // 处理传送
    auto homeData = getPlayerHomeData(player.getRealName(), homeName);
    if (homeData.has_value()) {
        player.teleport(Vec3{homeData->x, homeData->y, homeData->z}, homeData->dimid);
        return true;
    }
    return false; // 没有这个home
}

std::vector<data::HomeItem> HomeManager::getPlayerHomes(const string& realName) {
    auto pl = mHomeData->find(realName);
    if (pl != mHomeData->end()) {
        return pl->second;
    }
    return std::vector<data::HomeItem>(); // 没有这个玩家
}

std::vector<string> HomeManager::getAllPlayerName() {
    std::vector<string> ret;
    for (auto& [name, homeVec] : *mHomeData) {
        ret.push_back(name);
    }
    return ret;
}

} // namespace lbm::plugin::tpsystem::home