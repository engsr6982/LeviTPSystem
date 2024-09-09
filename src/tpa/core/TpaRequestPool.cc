#include "TpaRequestPool.h"
#include "config/Config.h"
#include "entry/Entry.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/schedule/Scheduler.h"
#include "ll/api/schedule/Task.h"
#include "ll/api/service/Bedrock.h"
#include "tpa/core/TpaRequest.h"
#include <memory>
#include <unordered_map>
#include <vector>


ll::schedule::GameTickScheduler scheduler;

namespace tps::tpa {

void TpaRequestPool::_initTask() {
    static bool isInited = false;
    if (isInited) {
        return;
    }
    isInited = true;

    using ll::chrono_literals::operator""_tick;
    scheduler.add<ll::schedule::RepeatTask>(Config::cfg.Tpa.CacehCheckFrequency * 20_tick, [this]() {
        auto level = ll::service::getLevel();
        if (!level.has_value()) {
            return;
        }

        for (auto& [receiver, senderPool] : this->mPool) {
            for (auto& [sender, request] : senderPool) {
                auto avail = request->getAvailable();
                if (avail != Available::Available) {
                    auto player = level->getPlayer(sender);
                    if (player) {
                        utils::mc::sendText<utils::mc::MsgLevel::Error>(
                            player,
                            "{0}",
                            TpaRequest::getAvailableDescription(avail)
                        );
                    }
                    this->deleteRequest(receiver, sender);
                }
            }
        }
    });
}
void TpaRequestPool::_initReceiver(const string& receiver) {
    if (mPool.find(receiver) == mPool.end()) {
        mPool.emplace(string(receiver), std::unordered_map<string, TpaRequestPtr>());
    }
}


TpaRequestPool& TpaRequestPool::getInstance() {
    static TpaRequestPool instance;
    instance._initTask();
    return instance;
}


bool TpaRequestPool::hasRequest(const string& receiver, const string& sender) const {
    auto senderPool = mPool.find(receiver); // receiver => sender => request
    if (senderPool == mPool.end()) {
        return false;
    }

    auto request = senderPool->second.find(sender);
    if (request == senderPool->second.end()) {
        return false;
    }

    return true;
}

bool TpaRequestPool::addRequest(TpaRequestPtr request) {
    auto& logger = entry::getInstance().getSelf().getLogger();

    string const& receiver = request->receiver;
    string const& sender   = request->sender;

    _initReceiver(receiver); // 初始化接收者

    if (hasRequest(receiver, sender)) {
        deleteRequest(receiver, sender); // 同玩家重复请求，删除旧请求
    }

    auto senderPool = mPool.find(receiver);
    if (senderPool == mPool.end()) {
        logger.debug("senderPool not found");
        return false; // 接收者池不存在
    }

    senderPool->second.emplace(string(sender), request);
    return true;
}

bool TpaRequestPool::deleteRequest(const string& receiver, const string& sender) {
    auto senderPool = mPool.find(receiver); // receiver => sender => request

    if (senderPool == mPool.end()) {
        return false;
    }

    auto request = senderPool->second.find(sender); // sender => request
    if (request == senderPool->second.end()) {
        return false;
    }

    senderPool->second.erase(request); // 删除请求
    return true;
}


std::vector<string> TpaRequestPool::getReceiverList() const {
    std::vector<string> receiverList{};
    for (const auto& [receiver, _] : mPool) {
        receiverList.push_back(receiver);
    }
    return receiverList;
}

std::vector<string> TpaRequestPool::getSenderList(const string& receiver) const {
    auto senderPool = mPool.find(receiver); // receiver => sender => request
    if (senderPool == mPool.end()) {
        return {};
    }

    std::vector<string> senders{};
    for (const auto& [sender, _] : senderPool->second) {
        senders.push_back(sender);
    }
    return senders;
}

TpaRequestPtr TpaRequestPool::getRequest(const string& receiver, const string& sender) const {
    auto senderPool = mPool.find(receiver); // receiver => sender => request
    if (senderPool == mPool.end()) {
        return nullptr;
    }

    auto request = senderPool->second.find(sender); // sender => request
    if (request == senderPool->second.end()) {
        return nullptr;
    }
    return request->second;
}

std::unordered_map<string, TpaRequestPtr>* TpaRequestPool::getSenderPool(const string& receiver) {
    auto senderPool = mPool.find(receiver); // receiver => sender => request
    if (senderPool == mPool.end()) {
        return nullptr;
    }
    return &senderPool->second;
}


} // namespace tps::tpa