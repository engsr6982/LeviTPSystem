#pragma once
#include "TpaRequest.h"
#include <string>
#include <unordered_map>
#include <vector>


using string = std::string;

namespace tps::tpa {


class TpaRequestPool {
public:
    //                 接收者 => 发起者池 => 发起者 => 请求实例
    std::unordered_map<string, std::unordered_map<string, std::unique_ptr<TpaRequest>>> mPool;

    TpaRequestPool()                                 = default;
    TpaRequestPool(const TpaRequestPool&)            = delete;
    TpaRequestPool& operator=(const TpaRequestPool&) = delete;

    static TpaRequestPool& getInstance();

    bool hasRequest(const string& receiver, const string& sender) const;

    bool addRequest(std::unique_ptr<TpaRequest> request); // 添加请求实例 (需移交所有权)

    bool deleteRequest(const string& receiver, const string& sender);

    std::vector<string> getReceiverList() const; // 获取所有接收者

    std::vector<string> getSenderList(const string& receiver) const; // 获取所有发起者

    TpaRequest* getRequest(const string& receiver, const string& sender) const; // 获取请求实例

    std::unordered_map<string, std::unique_ptr<TpaRequest>>* getSenderPool(const string& receiver); // 获取发起者池

    TpaRequest* makeRequest(Player& sender, Player& receiver, TpaType type); // 创建请求实例

    // private:
    void _initReceiver(const string& receiver);
    void _initTask();
};


} // namespace tps::tpa