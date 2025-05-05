#include "TpaRequestSendEvent.h"


namespace tps::event {

tpa::TpaRequest* TpaRequestSendEvent::getRequest() const { return mRequest; }

} // namespace tps::event