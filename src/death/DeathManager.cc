#include "DeathManager.h"
#include "config/Config.h"
#include "data/LevelDB.h"
#include "data/Structure.h"
#include "ll/api/i18n/I18n.h"
#include "mc/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "modules/Moneys.h"
#include "utils/Mc.h"
#include <memory>
#include <vector>


using ll::i18n_literals::operator""_tr;


namespace tps::death {


DeathManager& DeathManager::getInstance() {
    static DeathManager instance;
    return instance;
}

void DeathManager::syncToLevelDB() { data::LevelDB::getInstance().setDeath(*mDeath); }

void DeathManager::syncFromLevelDB() {
    auto deaths = data::LevelDB::getInstance().getDeath();
    mDeath      = std::make_unique<data::Death>(deaths);
}

bool DeathManager::addDeathInfo(const string& realName, data::DeathItem deathInfo) {
    auto fin = mDeath->find(realName);
    if (fin == mDeath->end()) {
        mDeath->emplace(string(realName), std::vector<data::DeathItem>()); // init
        fin = mDeath->find(realName);                                      // 重新获取迭代器
    };
    // 从头插入数据
    auto& vec = fin->second;
    vec.insert(vec.begin(), deathInfo);
    // 检查数据长度
    if (static_cast<int>(vec.size()) > Config::cfg.Death.MaxDeath) {
        vec.pop_back();
    }
    syncToLevelDB();
    return true;
}

bool DeathManager::hasDeathInfo(const string& realName) { return mDeath->find(realName) != mDeath->end(); }

bool DeathManager::teleport(Player& player, const int& index) {
    if (!hasDeathInfo(player.getRealName())) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "您还没有死亡点信息!");
        return false;
    }
    auto fin = mDeath->find(player.getRealName());
    if (fin->second.size() == 0) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "您还没有死亡点信息!");
        return false;
    }
    if (modules::Moneys::getInstance().reduceMoney(player, Config::cfg.Death.GoDeathMoney)) {
        auto death = fin->second[index];
        player.teleport(Vec3(death.x, death.y, death.z), death.dimid);
        return true;
    }
    return false;
}

std::vector<string> DeathManager::getAllKeys() {
    std::vector<string> vec;
    for (auto& [key, _] : *mDeath) {
        vec.push_back(string(key));
    }
    return vec;
}

std::vector<data::DeathItem> DeathManager::getPlayerDeaths(const string& realName) {
    auto fin = mDeath->find(realName);

    if (fin == mDeath->end()) return {};

    return fin->second;
}

} // namespace tps::death