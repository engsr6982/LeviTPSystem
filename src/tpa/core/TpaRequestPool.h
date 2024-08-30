#pragma once
#include "mc/world/actor/player/Player.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "TpaRequest.h"

namespace tps::tpa {

using string = std::string;

class TpaRequestPool {

private: //            接收者                                     请求者                   请求实例
    std::unordered_map<string, std::shared_ptr<std::unordered_map<string, TpaRequestPtr>>> mPool;

    TpaRequestPool()                                 = default;
    ~TpaRequestPool()                                = default;
    TpaRequestPool(const TpaRequestPool&)            = delete;
    TpaRequestPool& operator=(const TpaRequestPool&) = delete;

    void initSender(const string& realName);
    bool cleanUpIsRunning = false;

public:
    static TpaRequestPool& getInstance();

    bool hasRequest(const string& receiver, const string& sender);

    bool addRequest(TpaRequestPtr request);

    bool deleteRequest(const string& receiver, const string& sender);

    void checkAndRunCleanUpTask();

    std::vector<string> getReceiverList();
    std::vector<string> getSenderList(const string& receiver);

    std::shared_ptr<std::unordered_map<string, TpaRequestPtr>> getSenderPool(const string& receiver);
    TpaRequestPtr                                              getRequest(const string& receiver, const string& sender);
};

} // namespace tps::tpa