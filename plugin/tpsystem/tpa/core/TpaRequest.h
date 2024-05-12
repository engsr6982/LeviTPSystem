#pragma once
#include "mc/world/actor/player/Player.h"
#include "utils/Date.h"
#include <memory>
#include <string>


namespace lbm::plugin::tpa::core {

using string = std::string;

enum class Available : int {
    Available          = 0, // 有效
    Expired            = 1, // 过期
    SenderOffline      = 2, // 发送者离线
    RecieverOffline    = 3, // 接收者离线
    Unaffordable       = 4, // 余额不足
    ProhibitTpaRequest = 5  // 禁止发起tpa请求
};

string AvailDescription(Available avail); // 获取可用状态描述

using Date = utils::Date;

class TpaRequest {

public:
    string sender;   // 请求者
    string receiver; // 接收者
    string type;     // 请求类型 tpa / tpahere
    Date   time;     // 请求创建时间
    int    lifespan; // 请求有效期

    TpaRequest(Player& sender, Player& receiver, const string& type, int lifespan);

    bool isOutdated(); // 判断请求是否过期

    void accept(); // 接受请求
    void deny();   // 拒绝请求

    Available ask(); // 向接受者发送请求

    Available getAvailable(); // 获取可用状态
private:
    void destoryThisRequestFormPool(); // 销毁请求

    TpaRequest(const TpaRequest&)            = delete; // 禁止拷贝构造
    TpaRequest& operator=(const TpaRequest&) = delete; // 禁止拷贝赋值
};


} // namespace lbm::plugin::tpa::core
