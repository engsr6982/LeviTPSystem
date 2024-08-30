#include "ll/api/event/Event.h"
#include "tpa/core/TpaRequest.h"
#include "utils/Date.h"
#include <string>


using string = std::string;

namespace tps::event {


class TpaRequestSendEvent final : public ll::event::Event {
private:
    string const&      mSender;
    string const&      mReciever;
    utils::Date const& mTime;
    tpa::TpaType       mType;
    int                mLifeSpan;

public:
    constexpr explicit TpaRequestSendEvent(
        string const&      sender,
        string const&      reciever,
        utils::Date const& time,
        tpa::TpaType       type,
        int                lifespan
    )
    : mSender(sender),
      mReciever(reciever),
      mTime(time),
      mType(type),
      mLifeSpan(lifespan) {}

    string const&      getSender();
    string const&      getReciever();
    utils::Date const& getTime();
    tpa::TpaType       getType();
    int                getLifeSpan();
};


} // namespace tps::event