#include "ltps/database/IStorage.h"

namespace ltps {


class HomeStorage final : public IStorage {
public:
    TPSAPI explicit HomeStorage();

    TPSAPI void onStorageLoad() override;
    TPSAPI void onStorageUnload() override;
};


} // namespace ltps