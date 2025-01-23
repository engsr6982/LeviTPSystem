#include "data/Structure.h"
#include "mc/world/actor/player/Player.h"
#include <memory>
#include <string>
#include <vector>

using string = std::string;


namespace tps::pr {


class PrManager {
private:
    std::unique_ptr<data::Pr> mPr;

    PrManager()                            = default;
    PrManager(PrManager const&)            = delete;
    PrManager& operator=(PrManager const&) = delete;

    string randomGuid(const int len = 32);

public:
    static PrManager& getInstance();

    void syncToLevelDB();
    void syncFromLevelDB();

    bool addPr(const string& realName, data::PrItemSubData subData);

    bool deletePr(const string& guid);

    bool acceptPr(const string& guid);

    std::optional<data::PrItem> getPr(const string& guid);

    std::vector<data::PrItem> getPlayerPrs(const string& realName);

    std::vector<data::PrItem> getPrs();
};


} // namespace tps::pr