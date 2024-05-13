#pragma once
#include "mc/world/actor/player/Player.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "TpaRequest.h"

namespace lbm::plugin::tpsystem::tpa::core {

using string = std::string;

class TpaRequestPool {

private: //            receiver                   sender                   request
    std::unordered_map<string, std::unordered_map<string, std::unique_ptr<TpaRequest>>> mPool;

    TpaRequestPool()                                 = default;
    ~TpaRequestPool()                                = default;
    TpaRequestPool(const TpaRequestPool&)            = delete;
    TpaRequestPool& operator=(const TpaRequestPool&) = delete;

    void initSender(const string& realName);
    bool cleanUpIsRunning = false;

public:
    static TpaRequestPool& getInstance();

    bool hasRequest(const string& receiver, const string& sender);

    bool addRequest(std::unique_ptr<TpaRequest> request);

    bool deleteRequest(const string& receiver, const string& sender);

    void newCleanUp();

    std::vector<string> getReceiverList();
    std::vector<string> getSenderList(const string& receiver);

    std::unordered_map<string, std::unique_ptr<TpaRequest>>* getSenderPool(const string& receiver);
    TpaRequest*                                              getRequest(const string& receiver, const string& sender);

    TpaRequest* createRequest(Player& sender, Player& receiver, const string& type, int lifespan);
};

} // namespace lbm::plugin::tpa::core