#include "levitpsystem/modules/tpa/event/TpaEvents.h"
#include "levitpsystem/Global.h"
#include "ll/api/event/Emitter.h"
#include <utility>


namespace tps {


// ICreateTpaRequestEvent
ICreateTpaRequestEvent::ICreateTpaRequestEvent(Player& sender, Player& receiver, TpaRequest::Type type)
: mSender(sender),
  mReceiver(receiver),
  mType(type) {}

Player& ICreateTpaRequestEvent::getSender() const { return mSender; }

Player& ICreateTpaRequestEvent::getReceiver() const { return mReceiver; }

TpaRequest::Type ICreateTpaRequestEvent::getType() const { return mType; }


// CreateTpaRequestEvent
CreateTpaRequestEvent::CreateTpaRequestEvent(Player& sender, Player& receiver, TpaRequest::Type type, Callback callback)
: ICreateTpaRequestEvent(sender, receiver, type),
  mCallback(std::move(callback)) {}

void CreateTpaRequestEvent::setCallback(Callback callback) { mCallback = std::move(callback); }

CreateTpaRequestEvent::Callback const& CreateTpaRequestEvent::getCallback() const { return mCallback; }


// CreatingTpaRequestEvent
CreatingTpaRequestEvent::CreatingTpaRequestEvent(CreateTpaRequestEvent const& event)
: ICreateTpaRequestEvent(event.getSender(), event.getReceiver(), event.getType()) {}

CreatingTpaRequestEvent::CreatingTpaRequestEvent(Player& sender, Player& receiver, TpaRequest::Type type)
: ICreateTpaRequestEvent(sender, receiver, type) {}


// CreatedTpaRequestEvent
CreatedTpaRequestEvent::CreatedTpaRequestEvent(std::shared_ptr<TpaRequest> request) : mRequest(std::move(request)) {}

std::shared_ptr<TpaRequest> CreatedTpaRequestEvent::getRequest() const { return mRequest; }


// IAcceptOrDenyTpaRequestEvent
IAcceptOrDenyTpaRequestEvent::IAcceptOrDenyTpaRequestEvent(TpaRequest& request) : mRequest(request) {}

TpaRequest& IAcceptOrDenyTpaRequestEvent::getRequest() const { return mRequest; }

// TpaRequestAcceptingEvent
TpaRequestAcceptingEvent ::TpaRequestAcceptingEvent(TpaRequest& request) : IAcceptOrDenyTpaRequestEvent(request) {}

// TpaRequestAcceptedEvent
TpaRequestAcceptedEvent ::TpaRequestAcceptedEvent(TpaRequest& request) : IAcceptOrDenyTpaRequestEvent(request) {}

// TpaRequestDenyingEvent
TpaRequestDenyingEvent::TpaRequestDenyingEvent(TpaRequest& request) : IAcceptOrDenyTpaRequestEvent(request) {}

// TpaRequestDeniedEvent
TpaRequestDeniedEvent ::TpaRequestDeniedEvent(TpaRequest& request) : IAcceptOrDenyTpaRequestEvent(request) {}


// PlayerExecuteTpaAcceptOrDenyCommandEvent
PlayerExecuteTpaAcceptOrDenyCommandEvent::PlayerExecuteTpaAcceptOrDenyCommandEvent(Player& player, bool isAccept)
: mPlayer(player),
  mIsAccept(isAccept) {}

Player& PlayerExecuteTpaAcceptOrDenyCommandEvent::getPlayer() const { return mPlayer; }

bool PlayerExecuteTpaAcceptOrDenyCommandEvent::isAccept() const { return mIsAccept; }


IMPL_EVENT_EMITTER(CreateTpaRequestEvent);
IMPL_EVENT_EMITTER(CreatingTpaRequestEvent);
IMPL_EVENT_EMITTER(CreatedTpaRequestEvent);
IMPL_EVENT_EMITTER(TpaRequestAcceptingEvent);
IMPL_EVENT_EMITTER(TpaRequestAcceptedEvent);
IMPL_EVENT_EMITTER(TpaRequestDenyingEvent);
IMPL_EVENT_EMITTER(TpaRequestDeniedEvent);
IMPL_EVENT_EMITTER(PlayerExecuteTpaAcceptOrDenyCommandEvent);

} // namespace tps