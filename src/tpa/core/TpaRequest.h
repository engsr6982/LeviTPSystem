#pragma once
#include "mc/world/actor/player/Player.h"
#include "utils/Date.h"
#include <memory>
#include <string>


namespace tps::tpa {

using string = std::string;
using Date   = utils::Date;

enum class Available : int {
    Available          = 0, // 有效
    Expired            = 1, // 过期
    SenderOffline      = 2, // 发送者离线
    RecieverOffline    = 3, // 接收者离线
    Unaffordable       = 4, // 余额不足
    ProhibitTpaRequest = 5  // 禁止发起tpa请求
};

enum class TpaType : int {
    Tpa     = 0, // tpa
    TpaHere = 1  // tpahere
};


class TpaRequest : public std::enable_shared_from_this<TpaRequest> {
public:
    string                sender;   // 请求者
    string                receiver; // 接收者
    TpaType               type;     // 请求类型
    std::unique_ptr<Date> time;     // 请求创建时间
    int                   lifespan; // 请求有效期

    TpaRequest(Player& sender, Player& receiver, TpaType type, int lifespan);

    bool isOutdated(); // 判断请求是否过期

    void accept(); // 接受请求
    void deny();   // 拒绝请求

    Available ask(); // 向接受者发送请求

    Available getAvailable(); // 获取可用状态

    // static
    static string getAvailableDescription(Available avail); // 获取可用状态描述
    static string tpaTypeToString(TpaType type);            // 获取请求类型描述

private:
    void destoryThisRequestFormPool(); // 销毁请求

    TpaRequest(const TpaRequest&)            = delete; // 禁止拷贝构造
    TpaRequest& operator=(const TpaRequest&) = delete; // 禁止拷贝赋值
};

using TpaRequestPtr = std::shared_ptr<TpaRequest>;


} // namespace tps::tpa
