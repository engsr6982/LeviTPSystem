#include "TpaRequestPool.h"
#include "config/Config.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/schedule/Scheduler.h"
#include "ll/api/schedule/Task.h"
#include "ll/api/service/Bedrock.h"
#include <memory>
#include <unordered_map>
#include <vector>


ll::schedule::GameTickScheduler scheduler;

namespace tps::tpa::core {

TpaRequestPool& TpaRequestPool::getInstance() {
    static TpaRequestPool instance;
    instance.checkAndRunCleanUpTask();
    return instance;
}

void TpaRequestPool::initSender(const string& realName) {
    if (mPool.find(realName) == mPool.end()) {
        // 玩家第一次接受tpa请求，初始化 发起者池
        mPool[realName] = std::make_shared<std::unordered_map<string, std::shared_ptr<TpaRequest>>>();
    }
}

std::shared_ptr<std::unordered_map<string, std::shared_ptr<TpaRequest>>>
TpaRequestPool::getSenderPool(const string& receiver) {
    auto receiverPool = mPool.find(receiver);
    if (receiverPool == mPool.end()) {
        return nullptr;
    }
    return receiverPool->second;
}

bool TpaRequestPool::hasRequest(const string& receiver, const string& sender) {
    const auto& receiverPool = getSenderPool(receiver);
    if (!receiverPool) {
        return false;
    }
    auto request = receiverPool->find(sender);
    if (request == receiverPool->end()) {
        return false;
    }
    return true;
}

bool TpaRequestPool::addRequest(std::shared_ptr<TpaRequest> request) {
    initSender(request->sender);
    // 为了安全，检查是否有重复请求，有责销毁旧请求
    deleteRequest(request->receiver, request->sender); // 删除旧请求(如果有)
    // 获取接收者池
    auto receiverPool = mPool.find(request->receiver);
    if (receiverPool == mPool.end()) {
        return false;
    }
    // 获取发送者池，添加请求
    auto senderPool = receiverPool->second->find(request->sender);
    if (senderPool == receiverPool->second->end()) {
        (*receiverPool->second)[request->sender] = request;
        return true;
    }
    return false;
}

bool TpaRequestPool::deleteRequest(const string& receiver, const string& sender) {
    auto receiverPool = mPool.find(receiver); // 获取接收者池
    if (receiverPool == mPool.end()) {
        return false;
    }
    auto request = receiverPool->second->find(sender); // 获取发送者池
    if (request == receiverPool->second->end()) {
        return false;
    }
    receiverPool->second->erase(request); // 删除请求
    return true;
}

void TpaRequestPool::checkAndRunCleanUpTask() {
    if (cleanUpIsRunning) return;
    cleanUpIsRunning = true;
    using ll::chrono_literals::operator""_tick;
    scheduler.add<ll::schedule::RepeatTask>(config::cfg.Tpa.CacehCheckFrequency * 20_tick, []() {
        auto  level    = ll::service::getLevel();
        auto& instance = TpaRequestPool::getInstance();
        for (auto& [receiver, senderPool] : instance.mPool) { // 遍历接收者池
            for (auto& [sender, request] : *senderPool) {     // 遍历发送者池
                auto avail = request->getAvailable();         // 获取请求可用性
                if (avail != Available::Available) {
                    auto ptr = level->getPlayer(request->sender); // 获取发送者指针
                    if (ptr) {
                        utils::mc::sendText<utils::mc::MsgLevel::Error>(ptr, "{0}", AvailDescription(avail));
                    }
                    instance.deleteRequest(receiver, sender); // 删除请求
                }
            }
        }
    });
}

std::shared_ptr<TpaRequest> TpaRequestPool::getRequest(const string& receiver, const string& sender) {
    auto receiverPool = mPool.find(receiver); // 获取接收者池
    if (receiverPool == mPool.end()) {
        return nullptr;
    }
    auto request = receiverPool->second->find(sender); // 获取发送者池
    if (request == receiverPool->second->end()) {
        return nullptr;
    }
    return request->second;
}

std::vector<string> TpaRequestPool::getReceiverList() {
    std::vector<string> receiverList;
    for (const auto& [receiver, _] : mPool) {
        receiverList.push_back(receiver);
    }
    return receiverList;
}

std::vector<string> TpaRequestPool::getSenderList(const string& receiver) {
    auto receiverPool = mPool.find(receiver); // 获取接收者池
    if (receiverPool == mPool.end()) {
        return {};
    }
    std::vector<string> senderList;
    for (const auto& [sender, _] : *receiverPool->second) {
        senderList.push_back(sender);
    }
    return senderList;
}


} // namespace tps::tpa::core