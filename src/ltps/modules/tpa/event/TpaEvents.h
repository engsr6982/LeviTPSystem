#pragma once
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/Event.h"
#include "ltps/Global.h"
#include "ltps/modules/tpa/TpaRequest.h"
#include <functional>


class Player;

namespace ltps::tpa {

using ll::event::Cancellable;
using ll::event::Event;

class ICreateTpaRequestEvent {
protected:
    Player&          mSender;
    Player&          mReceiver;
    TpaRequest::Type mType;

public:
    TPSAPI explicit ICreateTpaRequestEvent(Player& sender, Player& receiver, TpaRequest::Type type);

    TPSNDAPI Player& getSender() const;

    TPSNDAPI Player& getReceiver() const;

    TPSNDAPI TpaRequest::Type getType() const;
};


/**
 * @brief 创建 TPA 请求事件
 *  流程: CreateTpaRequestEvent -> CreatingTpaRequestEvent -> TpaRequestPool::createRequest() -> CreatedTpaRequestEvent
 */
class CreateTpaRequestEvent final : public ICreateTpaRequestEvent, public Event {
    using Callback = std::function<void(std::shared_ptr<TpaRequest> request)>;
    Callback mCallback;

public:
    TPSAPI explicit CreateTpaRequestEvent(
        Player&          sender,
        Player&          receiver,
        TpaRequest::Type type,
        Callback         callback = {}
    );

    TPSAPI void invokeCallback(std::shared_ptr<TpaRequest> request) const;
};


// 正在创建 TPA 请求事件
class CreatingTpaRequestEvent final : public ICreateTpaRequestEvent, public Cancellable<Event> {
public:
    TPSAPI explicit CreatingTpaRequestEvent(CreateTpaRequestEvent const& event);
    TPSAPI explicit CreatingTpaRequestEvent(Player& sender, Player& receiver, TpaRequest::Type type);
};


// TPA 请求创建完毕事件
class CreatedTpaRequestEvent final : public Event {
    std::shared_ptr<TpaRequest> mRequest;

public:
    TPSAPI explicit CreatedTpaRequestEvent(std::shared_ptr<TpaRequest> request);

    TPSNDAPI std::shared_ptr<TpaRequest> getRequest() const;
};


class IAcceptOrDenyTpaRequestEvent {
protected:
    TpaRequest& mRequest;

public:
    TPSAPI explicit IAcceptOrDenyTpaRequestEvent(TpaRequest& request);

    TPSNDAPI TpaRequest& getRequest() const;
};


// Tpa 请求正在被接受
class TpaRequestAcceptingEvent final : public IAcceptOrDenyTpaRequestEvent, public Cancellable<Event> {
public:
    TPSAPI explicit TpaRequestAcceptingEvent(TpaRequest& request);
};

// Tpa 请求已接受
class TpaRequestAcceptedEvent final : public IAcceptOrDenyTpaRequestEvent, public Event {
public:
    TPSAPI explicit TpaRequestAcceptedEvent(TpaRequest& request);
};

// Tpa 请求正在被拒绝
class TpaRequestDenyingEvent final : public IAcceptOrDenyTpaRequestEvent, public Cancellable<Event> {
public:
    TPSAPI explicit TpaRequestDenyingEvent(TpaRequest& request);
};

// Tpa 请求已拒绝
class TpaRequestDeniedEvent final : public IAcceptOrDenyTpaRequestEvent, public Event {
public:
    TPSAPI explicit TpaRequestDeniedEvent(TpaRequest& request);
};


/**
 * @brief 玩家执行 TPA 接受或拒绝命令事件
 * 流程: PlayerExecuteTpaAcceptOrDenyCommandEvent -> TpaRequest::accept/deny() ->
 * TpaRequestAcceptingEvent/TpaRequestDenyingEvent -> TpaRequestAcceptedEvent/TpaRequestDeniedEvent
 */
class PlayerExecuteTpaAcceptOrDenyCommandEvent final : public Event {
    Player& mPlayer;
    bool    mIsAccept; // true: accept, false: deny

public:
    TPSAPI explicit PlayerExecuteTpaAcceptOrDenyCommandEvent(Player& player, bool isAccept);

    TPSNDAPI Player& getPlayer() const;

    // true: accept, false: deny
    TPSNDAPI bool isAccept() const;
};

} // namespace ltps::tpa