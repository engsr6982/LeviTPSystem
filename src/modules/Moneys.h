#ifdef ENABLE_MONEY
#pragma once
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/deps/core/mce/UUID.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpsystem/tpa/core/TpaRequest.h"
#include "tpsystem/tpa/core/TpaRequestPool.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <memory>
#include <stdexcept>


namespace lbm::modules {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using SimpleForm = ll::form::SimpleForm;


enum class MoneysType : int { Unknown = -1, LLMoney = 0, ScoreBoard = 1 };

struct MoneysConfig {
    bool       Enable    = false;
    MoneysType MoneyType = MoneysType::LLMoney;
    string     MoneyName = "money";
    string     ScoreName = "";
};

class ScoreBoardMoney {
private:
    string mScoreName;

public:
    ScoreBoardMoney(const string& scoreName = "");

    void setScoreName(const string& scoreName);

    int getScore(Player* player);
    int getScore(Player& player);
    int getScore(mce::UUID uuid);
    int getScore(const string& realName);

    bool setScore(Player* player, int score);
    bool setScore(Player& player, int score);
    bool setScore(mce::UUID uuid, int score);
    bool setScore(const string& realName, int score);

    bool addScore(Player* player, int score);
    bool addScore(Player& player, int score);
    bool addScore(mce::UUID uuid, int score);
    bool addScore(const string& realName, int score);

    bool reduceScore(Player* player, int score);
    bool reduceScore(Player& player, int score);
    bool reduceScore(mce::UUID uuid, int score);
    bool reduceScore(const string& realName, int score);
};


// 继承ScoreBoardMoney
class Moneys : public ScoreBoardMoney {
private:
    bool       mIsEnable = false;
    MoneysType mType     = MoneysType::Unknown; // 经济类型
    string     mMoneyName;                      // 货币名称

    Moneys()                         = default;
    ~Moneys()                        = default;
    Moneys(const Moneys&)            = delete;
    Moneys& operator=(const Moneys&) = delete;

    void throwUnknownType(Player* player = nullptr);

public:
    static Moneys& getInstance();

    bool updateConfig(MoneysConfig config);

    long long getMoney(Player& player);
    long long getMoney(Player* player);
    long long getMoney(mce::UUID uuid);
    long long getMoney(const string& realName);

    bool setMoney(Player& player, long long money);
    bool setMoney(Player* player, long long money);
    bool setMoney(mce::UUID uuid, long long money);
    bool setMoney(const string& realName, long long money);

    bool addMoney(Player& player, long long money);
    bool addMoney(Player* player, long long money);
    bool addMoney(mce::UUID uuid, long long money);
    bool addMoney(const string& realName, long long money);

    bool reduceMoney(Player& player, long long money);
    bool reduceMoney(Player* player, long long money);
    bool reduceMoney(mce::UUID uuid, long long money);
    bool reduceMoney(const string& realName, long long money);

    string getMoneySpendTipStr(Player& player, long long money);
    string getMoneySpendTipStr(Player* player, long long money);
    string getMoneySpendTipStr(mce::UUID uuid, long long money);
    string getMoneySpendTipStr(const string& realName, long long money);
};

} // namespace lbm::modules
#endif // ENABLE_MONEY