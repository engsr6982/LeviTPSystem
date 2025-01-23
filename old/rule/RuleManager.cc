#include "RuleManager.h"
#include "config/Config.h"
#include "data/LevelDB.h"
#include <memory>
#include <vector>


namespace tps::rule {


RuleManager& RuleManager::getInstance() {
    static RuleManager instance;
    return instance;
}

void RuleManager::syncToLevelDB() { data::LevelDB::getInstance().setRule(*mRule); }

void RuleManager::syncFromLevelDB() {
    auto rules = data::LevelDB::getInstance().getRule();
    mRule      = std::make_unique<data::Rule>(rules);
}

bool RuleManager::hasPlayerRule(const string& realName) { return mRule->find(realName) != mRule->end(); }

bool RuleManager::initPlayerRule(const string& realName) {
    if (hasPlayerRule(realName)) return false;
    mRule->emplace(realName, data::RuleItem(Config::cfg.Rule));
    syncToLevelDB();
    return true;
}

data::RuleItem RuleManager::getPlayerRule(const string& realName) {
    if (!hasPlayerRule(realName)) initPlayerRule(realName); // 初始化玩家规则
    return mRule->at(realName);
}

std::vector<string> RuleManager::getPlayerNames() {
    std::vector<string> names;
    for (auto& [name, _] : *mRule) names.push_back(name);
    return names;
}

bool RuleManager::setPlayerRule(const string& realName, const data::RuleItem rule) {
    if (!hasPlayerRule(realName)) initPlayerRule(realName); // 初始化玩家规则
    mRule->at(realName) = rule;
    syncToLevelDB();
    return true;
}

bool RuleManager::updatePlayerRule(const string& realName, const data::RuleItem rule) {
    auto fin = mRule->find(realName);
    if (fin == mRule->end()) return false;
    fin->second = rule;
    syncToLevelDB();
    return true;
}

} // namespace tps::rule