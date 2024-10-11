#include "TpaRequestPool.h"
#include "config/Config.h"
#include "entry/Entry.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/schedule/Scheduler.h"
#include "ll/api/schedule/Task.h"
#include "ll/api/service/Bedrock.h"
#include "tpa/core/TpaRequest.h"
#include "utils/Mc.h"
#include "utils/McAPI.h"
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <vector>


ll::schedule::GameTickScheduler scheduler;

namespace tps::tpa {

void TpaRequestPool::_initTask() {
    static bool isInited = false;
    if (isInited) return;
    isInited = true;

    scheduler.add<ll::schedule::RepeatTask>(ll::chrono::ticks(Config::cfg.Tpa.CacehCheckFrequency * 20), [this]() {
        try {
            if (mPool.empty()) {
                return;
            }

            auto level = ll::service::getLevel();
            if (!level.has_value()) {
                return;
            }

            // 使用迭代器来安全地删除元素
            for (auto receiverIt = mPool.begin(); receiverIt != mPool.end();) {
                auto& [receiver, senderPool] = *receiverIt;
                if (senderPool.empty()) {
                    receiverIt = mPool.erase(receiverIt);
                    continue;
                }

                for (auto senderIt = senderPool.begin(); senderIt != senderPool.end();) {
                    auto& [sender, request] = *senderIt;
                    if (!request) {
                        senderIt = senderPool.erase(senderIt);
                        continue;
                    }

                    // 添加额外的空指针检查
                    if (request.get() == nullptr) {
                        entry::getInstance().getSelf().getLogger().error(
                            "Null pointer detected for sender: {}",
                            sender
                        );
                        senderIt = senderPool.erase(senderIt);
                        continue;
                    }

                    try {
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
                            senderIt = senderPool.erase(senderIt);
                        } else {
                            ++senderIt;
                        }
                    } catch (const std::exception& e) {
                        entry::getInstance().getSelf().getLogger().error(
                            "Exception in getAvailable for sender {}: {}",
                            sender,
                            e.what()
                        );
                        senderIt = senderPool.erase(senderIt);
                    }
                }

                if (senderPool.empty()) {
                    receiverIt = mPool.erase(receiverIt);
                } else {
                    ++receiverIt;
                }
            }
        } catch (const std::exception& e) {
            entry::getInstance().getSelf().getLogger().error("Exception in {}: {}", __FUNCTION__, e.what());
        } catch (...) {
            entry::getInstance().getSelf().getLogger().error("Unknown exception in {}", __FUNCTION__);
        }
    });
}


TpaRequestPool& TpaRequestPool::getInstance() {
    static TpaRequestPool instance;
    instance._initTask();
    return instance;
}


bool TpaRequestPool::hasRequest(const string& receiver, const string& sender) const {
    if (mPool.contains(receiver)) {
        return mPool.at(receiver).contains(sender); // receiver => sender => request
    }
    return false;
}

bool TpaRequestPool::addRequest(std::unique_ptr<TpaRequest> request) {
    auto& logger = entry::getInstance().getSelf().getLogger();

    string const& receiver = request->getReceiver();
    string const& sender   = request->getSender();

    // 初始化接收者池
    if (!mPool.contains(receiver)) {
        mPool[string(receiver)] = std::unordered_map<string, std::unique_ptr<TpaRequest>>{};
    }

    if (hasRequest(receiver, sender)) {
        deleteRequest(receiver, sender); // 同玩家重复请求，删除旧请求
    }

    auto senderPool = mPool.find(receiver);
    if (senderPool == mPool.end()) {
        logger.debug("senderPool not found");
        return false; // 接收者池不存在
    }

    senderPool->second.emplace(string(sender), std::move(request));
    return true;
}

bool TpaRequestPool::deleteRequest(const string& receiver, const string& sender) {
    if (mPool.contains(receiver)) {
        auto& senders = mPool.at(receiver);
        if (senders.contains(sender)) {
            senders.erase(sender);
            return true;
        }
    }
    return false;
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

TpaRequest* TpaRequestPool::getRequest(const string& receiver, const string& sender) const {
    auto senderPool = mPool.find(receiver); // receiver => sender => request
    if (senderPool == mPool.end()) {
        return nullptr;
    }

    auto request = senderPool->second.find(sender); // sender => request
    if (request == senderPool->second.end()) {
        return nullptr;
    }
    return request->second.get();
}

TpaRequest* TpaRequestPool::makeRequest(Player& sender, Player& receiver, TpaType type) {
    addRequest(std::make_unique<TpaRequest>(sender, receiver, type));
    return getRequest(receiver.getRealName(), sender.getRealName());
}


} // namespace tps::tpa