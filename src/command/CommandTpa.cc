#include "Command.h"
#include "event/TpaRequestSendEvent.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/form/SimpleForm.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandSelector.h"
#include "tpa/TpaGUI.h"
#include "tpa/core/TpaRequestPool.h"


namespace tps::command {

enum TpOption { accept, deny };
struct ParamOption {
    TpOption option;
};

struct ParamTp {
    tpa::TpaType            type;
    CommandSelector<Player> target;
};

void registerCommandWithTpa(const string& _commandName) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(_commandName);

    // tps tpa
    cmd.overload().text("tpa").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        auto& player = *static_cast<Player*>(origin.getEntity());
        tps::tpa::TpaGUI::TpaEntryGUI(player);
    });

    // tps tpa <accept|deny>
    cmd.overload<ParamOption>().text("tpa").required("option").execute(
        [](CommandOrigin const& origin, CommandOutput& output, const ParamOption& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            auto& receiver = *static_cast<Player*>(origin.getEntity()); // 获取玩家实体

            if (receiver.isSleeping()) {
                sendText<LogLevel::Error>(output, "无法在睡觉中执行此操作!"_tr());
                return;
            }
            if (!Config::checkOpeningDimensions(Config::cfg.Tpa.OpenDimensions, receiver.getDimensionId())) {
                sendText<LogLevel::Error>(output, "当前维度不允许使用此功能!"_tr());
                return;
            }


            auto pool    = &tpa::TpaRequestPool::getInstance();         // 获取请求池
            auto senders = pool->getSenderList(receiver.getRealName()); // 获取发起者列表

            if (senders.empty()) { // 没有请求
                sendText<LogLevel::Error>(output, "你没有收到任何TPA请求！"_tr());
                return;

            } else if (senders.size() == 1) { // 只有一个请求
                auto req = pool->getRequest(receiver.getRealName(), senders[0]);
                param.option == TpOption::accept ? req->_accept() : req->_deny();
                return;

            } else {
                // 多个请求
                ll::form::SimpleForm fm;
                for (auto& sender : senders) {
                    fm.appendButton(sender, [sender, pool, param](Player& receiver2) {
                        auto req = pool->getRequest(receiver2.getRealName(), sender);
                        if (req->getAvailable() == tpa::Available::Available) {
                            param.option == TpOption::accept ? req->_accept() : req->_deny();
                        }
                    });
                }
                fm.sendTo(receiver);
            }
        }
    );

    // tps tpa <here|to> <Player>
    cmd.overload<ParamTp>().text("tpa").required("type").required("target").execute(
        [](CommandOrigin const& origin, CommandOutput& output, const ParamTp& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            auto& sender = *static_cast<Player*>(origin.getEntity());
            if (!Config::checkOpeningDimensions(Config::cfg.Tpa.OpenDimensions, sender.getDimensionId())) {
                sendText<LogLevel::Error>(output, "当前维度不允许使用此功能!"_tr());
                return;
            }


            auto li = param.target.results(origin);
            if (li.empty()) {
                sendText<LogLevel::Error>(output, "请至少选择一位玩家！"_tr());
                return;
            } else if (li.size() > 1) {
                sendText<LogLevel::Error>(output, "仅支持对一位玩家发起TPA！"_tr());
                return;
            }

            auto request = tpa::TpaRequestPool::getInstance().makeRequest(sender, *(*li.data)[0], param.type);
            if (!request) {
                return;
            }

            tpa::Available avail = request->sendAskForm(); // 发送请求

            if (avail != tpa::Available::Available) {
                sendText<LogLevel::Error>(sender, "{}"_tr(tpa::TpaRequest::getAvailableDescription(avail)));
            }

            // Tpa 请求发送事件
            ll::event::EventBus::getInstance().publish(event::TpaRequestSendEvent(request));
        }
    );
}

} // namespace tps::command