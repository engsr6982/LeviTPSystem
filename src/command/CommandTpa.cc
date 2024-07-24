#include "../tpa/core/TpaRequestPool.h"
#include "../tpa/gui/TpaEntry.h"
#include "Command.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/form/SimpleForm.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandSelector.h"

#include "event/TpaRequestSendEvent.h"
#include "ll/api/event/EventBus.h"

namespace tps::command {

enum TpOption { accept, deny };
struct ParamOption {
    TpOption option;
};

enum TpType { to, here };
struct ParamTp {
    TpType                  type;
    CommandSelector<Player> target;
};

void registerCommandWithTpa(const string& name) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(name);

    // tps tpa
    cmd.overload().text("tpa").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        auto& player = *static_cast<Player*>(origin.getEntity());
        tpa::gui::tpaEntry(player);
    });

    // tps tpa <accept|deny>
    cmd.overload<ParamOption>().text("tpa").required("option").execute(
        [](CommandOrigin const& origin, CommandOutput& output, const ParamOption& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            auto& player      = *static_cast<Player*>(origin.getEntity()); // 获取玩家实体
            auto& pool        = tpa::core::TpaRequestPool::getInstance();  // 获取请求池
            auto  requestList = pool.getSenderList(player.getRealName());  // 获取发起者列表
            if (requestList.empty()) {                                     // 没有请求
                sendText<MsgLevel::Error>(output, "你没有收到任何TPA请求！"_tr());
                return;
            } else if (requestList.size() == 1) { // 只有一个请求
                auto req = pool.getRequest(player.getRealName(), requestList[0]);
                param.option == TpOption::accept ? req->accept() : req->deny();
                return;
            } else {
                // 多个请求
                ll::form::SimpleForm fm;
                for (auto& name : requestList) {
                    fm.appendButton(name, [&](Player& p) {
                        auto req = pool.getRequest(p.getRealName(), name);
                        if (req->getAvailable() == tpa::core::Available::Available) {
                            param.option == TpOption::accept ? req->accept() : req->deny();
                        }
                    });
                }
                fm.sendTo(player);
            }
        }
    );

    // tps tpa <here|to> <Player>
    cmd.overload<ParamTp>().text("tpa").required("type").required("target").execute([](CommandOrigin const& origin,
                                                                                       CommandOutput&       output,
                                                                                       const ParamTp&       param) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        auto&        player = *static_cast<Player*>(origin.getEntity());
        const string type   = (param.type == TpType::to) ? "tpa" : "tpahere";
        auto         li     = param.target.results(origin);
        if (li.empty()) {
            sendText<MsgLevel::Error>(output, "请至少选择一位玩家！"_tr());
            return;
        } else if (li.size() > 1) {
            sendText<MsgLevel::Error>(output, "仅支持对一位玩家发起TPA！"_tr());
            return;
        }
        auto request =
            std::make_shared<tpa::core::TpaRequest>(player, *(*li.data)[0], type, config::cfg.Tpa.CacheExpirationTime);
        tpa::core::Available avail = request->ask();
        if (avail != tpa::core::Available::Available) {
            sendText<MsgLevel::Error>(player, "{}"_tr(tpa::core::AvailDescription(avail)));
        }
        // Tpa 请求发送事件
        ll::event::EventBus::getInstance().publish(event::TpaRequestSendEvent(
            request->sender,
            request->receiver,
            utils::Date::clone(*request->time),
            request->type,
            request->lifespan
        ));
    });
}

} // namespace tps::command