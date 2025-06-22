#pragma once
#include "ltps/Global.h"
#include "mc/deps/ecs/WeakEntityRef.h"
#include <chrono>


class Player;

namespace ltps::tpa {


class TpaRequest {
public:
    enum class Type {
        To   = 0, // Sender -> Receiver
        Here = 1  // Receiver -> Sender
    };

    enum class State {
        Available,       // 请求有效
        Accepted,        // 请求已接受
        Denied,          // 请求已拒绝
        SenderOffline,   // 发起者离线
        ReceiverOffline, // 接收者离线
        Expired,         // 请求已过期
    };

    using Time = std::chrono::system_clock::time_point;

private:
    WeakRef<EntityContext> mSender;
    WeakRef<EntityContext> mReceiver;
    Type                   mType;
    State                  mState;
    Time                   mCreationTime;

    friend class TpaRequestPool;

public:
    TPS_DISALLOW_COPY_AND_MOVE(TpaRequest);

    TPSAPI explicit TpaRequest(Player& sender, Player& receiver, Type type);

    TPSAPI virtual ~TpaRequest();

public:
    TPSNDAPI Player* getSender() const;

    TPSNDAPI Player* getReceiver() const;

    TPSNDAPI Type getType() const;

    TPSNDAPI State getState() const;

    // 获取请求的创建时间
    TPSNDAPI Time const& getCreationTime() const;

    // 获取请求剩余有效时间（单位：秒）
    TPSNDAPI std::chrono::seconds getRemainingTime() const;

    // 获取请求失效时间 (yyyy-mm-dd hh:mm:ss)
    TPSNDAPI std::string getExpirationTime() const;

    TPSAPI void setState(State state);

    TPSNDAPI bool isExpired() const;

    // 检查当前请求是否有效，此API不会执行任何检查，仅判断当前存储的状态
    TPSNDAPI bool isAvailable() const;

    // 强制更新当前请求状态
    TPSAPI void forceUpdateState();

    TPSAPI void accept();

    TPSAPI void deny();

    TPSAPI void sendFormToReceiver();


    TPSNDAPI static std::string getStateDescription(State state, std::string const& localeCode = "zh_CN");
    TPSNDAPI static std::string getTypeString(Type type);
};


} // namespace ltps::tpa