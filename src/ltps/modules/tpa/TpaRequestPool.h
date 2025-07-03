#pragma once
#include "TpaRequest.h"
#include "ltps/Global.h"
#include "mc/platform/UUID.h"
#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>


#include "ll/api/coro/InterruptableSleep.h"
#include "ll/api/thread/ThreadPoolExecutor.h"

namespace ltps::tpa {

class TpaRequestPool {
    // Receiver -> Sender -> Request
    using RequestMap = std::unordered_map<mce::UUID, std::shared_ptr<TpaRequest>>;
    std::unordered_map<mce::UUID, RequestMap> mPool;

    std::shared_ptr<std::atomic_bool>             mPollingAbortFlag{nullptr};
    std::shared_ptr<ll::coro::InterruptableSleep> mInterruptableSleep{nullptr};

    friend class TpaModule;

public:
    TPS_DISALLOW_COPY_AND_MOVE(TpaRequestPool)

    TPSAPI explicit TpaRequestPool(ll::thread::ThreadPoolExecutor& executor);
    TPSAPI virtual ~TpaRequestPool();


public:
    TPSNDAPI std::shared_ptr<TpaRequest> createRequest(Player& sender, Player& receiver, TpaRequest::Type type);

    TPSNDAPI bool hasRequest(mce::UUID const& sender, mce::UUID const& receiver);
    TPSNDAPI bool hasRequest(Player& sender, Player& receiver);

    TPSAPI bool addRequest(std::shared_ptr<TpaRequest> const& request);

    TPSAPI bool removeRequest(mce::UUID const& sender, mce::UUID const& receiver);

    TPSNDAPI std::shared_ptr<TpaRequest> getRequest(mce::UUID const& sender, mce::UUID const& receiver);

    TPSNDAPI std::vector<mce::UUID> getSenders(mce::UUID const& receiver);

    TPSAPI void cleanupExpiredRequests();
};


} // namespace ltps::tpa