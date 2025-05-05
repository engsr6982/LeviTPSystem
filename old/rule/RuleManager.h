#include "data/LevelDB.h"
#include "data/Structure.h"
#include <memory>
#include <string>

using string = std::string;

namespace tps::rule {


class RuleManager {
private:
    std::unique_ptr<data::Rule> mRule;

    RuleManager()                              = default;
    RuleManager(const RuleManager&)            = delete;
    RuleManager& operator=(const RuleManager&) = delete;

public:
    static RuleManager& getInstance();

    void syncToLevelDB();
    void syncFromLevelDB();

    bool hasPlayerRule(const string& realName);

    bool initPlayerRule(const string& realName);

    data::RuleItem getPlayerRule(const string& realName);

    std::vector<string> getPlayerNames();

    bool setPlayerRule(const string& realName, const data::RuleItem rule);

    bool updatePlayerRule(const string& realName, const data::RuleItem rule);
};


} // namespace tps::rule