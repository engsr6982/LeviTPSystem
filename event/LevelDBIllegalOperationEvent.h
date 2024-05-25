#include "ll/api/event/Event.h"
#include "ll/api/event/EventBus.h"
#include <string>

using string = std::string;

namespace lbm::plugin::tpsystem::event {

// 事件: LevelDB 非法操作事件
// 定义： 除各个模块的Manager之外操作数据库，均为非法操作。
class LevelDBIllegalOperationEvent final : public ll::event::Event {
public:
    LevelDBIllegalOperationEvent() {}
};


} // namespace lbm::plugin::tpsystem::event
