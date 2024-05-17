#include "TpaRequestSendEvent.h"
#include "utils/Date.h"


namespace lbm::plugin::tpsystem::event {

TpaRequestSendEvent::TpaRequestSendEvent(string sender, string reciever, utils::Date time, string type, int lifespan) {
    mSender   = sender;
    mReciever = reciever;
    mTime     = time;
    mType     = type;
    mLifeSpan = lifespan;
}


string      TpaRequestSendEvent::getSender() { return mSender; }
string      TpaRequestSendEvent::getReciever() { return mReciever; }
string      TpaRequestSendEvent::getType() { return mType; }
int         TpaRequestSendEvent::getLifeSpan() { return mLifeSpan; }
utils::Date TpaRequestSendEvent::getTime() { return mTime; }


} // namespace lbm::plugin::tpsystem::event