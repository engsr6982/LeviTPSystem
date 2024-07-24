#include "TpaRequestSendEvent.h"
#include "utils/Date.h"


namespace tps::event {

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


} // namespace tps::event