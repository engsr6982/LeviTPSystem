#include "TpaRequestPool.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace lbm::plugin::tpa::core {

TpaRequestPool& TpaRequestPool::getInstance() {
    static TpaRequestPool instance;
    return instance;
}

void TpaRequestPool::initSender(const string& realName) {
    if (mPool.find(realName) == mPool.end()) {
        // 玩家第一次接受tpa请求，初始化 发起者池
        mPool[realName] = std::unordered_map<string, std::unique_ptr<TpaRequest>>();
    }
}

std::unordered_map<string, std::unique_ptr<TpaRequest>>* TpaRequestPool::getSenderPool(const string& receiver) {
    auto receiverPool = mPool.find(receiver);
    if (receiverPool == mPool.end()) {
        return nullptr;
    }
    return &receiverPool->second;
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

bool TpaRequestPool::addRequest(std::unique_ptr<TpaRequest> request) {
    initSender(request->sender);

    // 获取接收者池
    auto receiverPool = mPool.find(request->receiver);
    if (receiverPool == mPool.end()) {
        return false;
    }
    // 获取发送者池，添加请求
    auto senderPool = receiverPool->second.find(request->sender);
    if (senderPool == receiverPool->second.end()) {
        receiverPool->second[request->sender] = std::move(request);
        return true;
    }
    return false;
}

bool TpaRequestPool::deleteRequest(const string& receiver, const string& sender) {
    auto receiverPool = mPool.find(receiver); // 获取接收者池
    if (receiverPool == mPool.end()) {
        return false;
    }
    auto request = receiverPool->second.find(sender); // 获取发送者池
    if (request == receiverPool->second.end()) {
        return false;
    }
    receiverPool->second.erase(request); // 删除请求
    return true;
}

void TpaRequestPool::newCleanUp() {
    // Todo: 清理过期请求
}

TpaRequest* TpaRequestPool::getRequest(const string& receiver, const string& sender) {
    auto receiverPool = mPool.find(receiver); // 获取接收者池
    if (receiverPool == mPool.end()) {
        return nullptr;
    }
    auto request = receiverPool->second.find(sender); // 获取发送者池
    if (request == receiverPool->second.end()) {
        return nullptr;
    }
    return request->second.get();
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
    for (const auto& [sender, _] : receiverPool->second) {
        senderList.push_back(sender);
    }
    return senderList;
}

TpaRequest* TpaRequestPool::createRequest(Player& sender, Player& receiver, const string& type, int lifespan) {
    addRequest(std::make_unique<TpaRequest>(sender, receiver, type, lifespan));
    return getRequest(receiver.getRealName(), sender.getRealName());
}

} // namespace lbm::plugin::tpa::core