#include "ll/api/event/Event.h"
#include "utils/Date.h"
#include <string>


using string = std::string;

namespace tps::event {


class TpaRequestSendEvent final : public ll::event::Event {
private:
    string      mSender;
    string      mReciever;
    utils::Date mTime;
    string      mType;
    int         mLifeSpan;

public:
    TpaRequestSendEvent(string sender, string reciever, utils::Date time, string type, int lifespan);

    string      getSender();
    string      getReciever();
    utils::Date getTime();
    string      getType();
    int         getLifeSpan();
};


} // namespace tps::event