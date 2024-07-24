#include "config/Config.h"
#include "data/LevelDB.h"
#include "data/Structure.h"
#include "ll/api/i18n/I18n.h"
#include "mc/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "modules/Moneys.h"
#include "utils/Mc.h"
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>


#include "PrManager.h"
#include "warp/WarpManager.h"

namespace tps::pr {


unsigned int random_char() {
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}

string PrManager::randomGuid(const int len) {
    std::stringstream ss;
    for (auto i = 0; i < len; i++) {
        const auto        rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << rc;
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}

PrManager& PrManager::getInstance() {
    static PrManager instance;
    return instance;
}

void PrManager::syncToLevelDB() { data::LevelDB::getInstance().setPr(*mPr); }

void PrManager::syncFromLevelDB() {
    auto prs = data::LevelDB::getInstance().getPr();
    mPr      = std::make_unique<data::Pr>(prs);
}

bool PrManager::addPr(const string& realName, data::PrItemSubData subData) {
    data::PrItem it;
    it.data           = subData;
    it.playerRealName = string(realName);
    it.time           = utils::Date{}.toString();
    it.guid           = randomGuid();
    mPr->insert(mPr->begin(), it); // 从前面插入
    syncToLevelDB();
    return true;
}

bool PrManager::deletePr(const string& guid) {
    auto it = std::find_if(mPr->begin(), mPr->end(), [&guid](const data::PrItem& item) { return item.guid == guid; });
    if (it == mPr->end()) {
        return false;
    }
    mPr->erase(it);
    syncToLevelDB();
    return true;
}

bool PrManager::acceptPr(const string& guid) {
    auto it = std::find_if(mPr->begin(), mPr->end(), [&guid](const data::PrItem& item) { return item.guid == guid; });
    if (it == mPr->end()) {
        return false;
    }
    warp::WarpManager::getInstance().createWarp(it->data.name, it->data); // 创建Warp传送点
    mPr->erase(it);                                                       // 从缓存中删除
    syncToLevelDB();
    return true;
}


std::optional<data::PrItem> PrManager::getPr(const string& guid) {
    auto it = std::find_if(mPr->begin(), mPr->end(), [&guid](const data::PrItem& item) { return item.guid == guid; });
    if (it == mPr->end()) {
        return std::nullopt;
    }
    return *it;
}


std::vector<data::PrItem> PrManager::getPlayerPrs(const string& realName) {
    std::vector<data::PrItem> prs;
    std::copy_if(mPr->begin(), mPr->end(), std::back_inserter(prs), [&realName](const data::PrItem& item) {
        return item.playerRealName == realName;
    });
    return prs;
}

std::vector<data::PrItem> PrManager::getPrs() { return std::vector<data::PrItem>(*mPr); }

} // namespace tps::pr
