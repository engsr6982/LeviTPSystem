#pragma once
#include "TpaRequest.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


using string = std::string;

namespace tps::tpa {


class TpaRequestPool {
public:
    // Query: 接收者 => 发起者池 => 发起者 => 请求实例
    std::unordered_map<string, std::unordered_map<string, std::unique_ptr<TpaRequest>>> mPool;

    TpaRequestPool()                                 = default;
    TpaRequestPool(const TpaRequestPool&)            = delete;
    TpaRequestPool& operator=(const TpaRequestPool&) = delete;


    static TpaRequestPool& getInstance();

    TpaRequest* makeRequest(Player& sender, Player& receiver, TpaType type); // 创建请求实例

    bool        hasRequest(const string& receiver, const string& sender) const;
    bool        addRequest(std::unique_ptr<TpaRequest> request); // 添加请求实例 (需移交所有权)
    bool        deleteRequest(const string& receiver, const string& sender);
    TpaRequest* getRequest(const string& receiver, const string& sender) const; // 获取请求实例

    std::vector<string> getSenderList(const string& receiver) const; // 获取所有发起者

    // private:
    void _initTask();
};


} // namespace tps::tpa