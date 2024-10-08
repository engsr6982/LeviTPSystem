#pragma once
#include "ll/api/form/SimpleForm.h"
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


class TpaRequest {
public:
    TpaRequest(Player& sender, Player& receiver, TpaType type);

    // getters
    string const& getSender() const;
    string const& getReceiver() const;
    TpaType       getType() const;
    Date const&   getTime() const;
    int           getLifespan() const;

    // others
    bool isAvailable() const; // 判断请求是否可用
    bool isOutdated() const;  // 判断请求是否过期

    void _accept() const; // 接受请求
    void _deny() const;   // 拒绝请求

    Available sendAskForm();        // 发送请求表单
    Available getAvailable() const; // 获取可用状态

    void destroy() const; // 销毁请求

    // operators
    bool     operator==(const TpaRequest& other) const;
    bool     operator!=(const TpaRequest& other) const;
    // explicit operator bool() const; // isAvailable


    // static
    static string getAvailableDescription(Available avail); // 获取可用状态描述
    static string tpaTypeToString(TpaType type);            // 获取请求类型描述

private:
    ll::form::SimpleForm mAskForm;  // 请求表单
    string               mSender;   // 请求者
    string               mReceiver; // 接收者
    TpaType              mType;     // 请求类型
    Date                 mTime;     // 请求创建时间
    int                  mLifespan; // 请求有效期

    // TpaRequest(const TpaRequest&)            = delete; // 禁止拷贝构造
    // TpaRequest& operator=(const TpaRequest&) = delete; // 禁止拷贝赋值
    // TpaRequest(TpaRequest&&)                 = delete; // 禁止移动构造
    // TpaRequest& operator=(TpaRequest&&)      = delete; // 禁止移动赋值
};


} // namespace tps::tpa
