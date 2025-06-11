#include "ltps/database/IStorage.h"

namespace ltps {


class HomeStorage final : public IStorage {
public:
    TPSAPI explicit HomeStorage();

    TPSAPI void initStorage() override;
};


} // namespace ltps