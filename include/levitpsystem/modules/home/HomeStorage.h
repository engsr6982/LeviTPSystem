#include "levitpsystem/database/IStorage.h"

namespace tps {


class HomeStorage final : public IStorage {
public:
    TPSAPI explicit HomeStorage();

    TPSAPI void initStorage() override;
};


} // namespace tps