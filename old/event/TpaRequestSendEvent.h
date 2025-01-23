#include "ll/api/event/Event.h"
#include "tpa/core/TpaRequest.h"

namespace tps::event {

class TpaRequestSendEvent final : public ll::event::Event {
private:
    tpa::TpaRequest* mRequest;

public:
    constexpr explicit TpaRequestSendEvent(tpa::TpaRequest* req) : mRequest(req) {}

    tpa::TpaRequest* getRequest() const;
};


} // namespace tps::event