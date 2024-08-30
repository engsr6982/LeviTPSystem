#include "TpaRequestSendEvent.h"
#include "utils/Date.h"


namespace tps::event {

string const&      TpaRequestSendEvent::getSender() { return mSender; }
string const&      TpaRequestSendEvent::getReciever() { return mReciever; }
tpa::TpaType       TpaRequestSendEvent::getType() { return mType; }
int                TpaRequestSendEvent::getLifeSpan() { return mLifeSpan; }
utils::Date const& TpaRequestSendEvent::getTime() { return mTime; }


} // namespace tps::event