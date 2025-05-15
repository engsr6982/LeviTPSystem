#include "levitpsystem/modules/tpa/TpaModule.h"
#include "levitpsystem/LeviTPSystem.h"
#include "levitpsystem/base/Config.h"
#include "levitpsystem/common/PriceCalculate.h"
#include "levitpsystem/modules/tpa/TpaCommand.h"
#include "levitpsystem/modules/tpa/TpaRequest.h"
#include "levitpsystem/modules/tpa/event/TpaEvents.h"
#include "levitpsystem/utils/McUtils.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/service/PlayerInfo.h"
#include <algorithm>


namespace tps {

TpaModule::TpaModule() = default;

std::vector<std::string> TpaModule::getDependencies() const { return {}; }

bool TpaModule::init() {
    if (!mPool) {
        mPool = std::make_unique<TpaRequestPool>(getThreadPool());
    }
    return true;
}

bool TpaModule::enable() {
    auto& bus = ll::event::EventBus::getInstance();

    mListeners.emplace_back(bus.emplaceListener<CreateTpaRequestEvent>([this, &bus](CreateTpaRequestEvent& ev) {
        auto before = CreatingTpaRequestEvent(ev);
        bus.publish(before);

        if (before.isCancelled()) {
            ev.getCallback()(nullptr);
            return;
        }

        auto ptr = getRequestPool().createRequest(ev.getSender(), ev.getReceiver(), ev.getType());

        ev.getCallback()(ptr);
        bus.publish(CreatedTpaRequestEvent(ptr));
    }));

    mListeners.emplace_back(bus.emplaceListener<CreatingTpaRequestEvent>([this](CreatingTpaRequestEvent& ev) {
        auto& sender = ev.getSender();

        auto localeCode = sender.getLocaleCode();

        // 维度检查
        if (std::find(
                getConfig().modules.tpa.disallowedDimensions.begin(),
                getConfig().modules.tpa.disallowedDimensions.end(),
                sender.getDimensionId()
            )
            != getConfig().modules.tpa.disallowedDimensions.end()) {
            mc_utils::sendText<mc_utils::Error>(sender, "此功能在当前维度不可用"_trl(localeCode));
            ev.cancel();
            return;
        }

        // TPA 请求冷却
        if (this->mCooldown.isCooldown(sender.getRealName())) {
            mc_utils::sendText<mc_utils::Error>(
                sender,
                "TPA 请求冷却中，剩余时间 {0}"_trl(localeCode, this->mCooldown.getCooldownString(sender.getRealName()))
            );
            ev.cancel();
            return;
        }
        this->mCooldown.setCooldown(sender.getRealName(), getConfig().modules.tpa.cooldownTime);

        // 费用检查
        PriceCalculate cl(getConfig().modules.tpa.createRequestCalculate);
        auto           clValue = cl.eval();
        if (!clValue.has_value()) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "An exception occurred while calculating the TPA price, please check the configuration file.\n{}",
                clValue.error()
            );
            mc_utils::sendText<mc_utils::Error>(sender, "TPA 模块异常，请联系管理员"_trl(localeCode));
            ev.cancel();
            return;
        }

        auto price = static_cast<llong>(*clValue);

        auto economy = EconomySystemManager::getInstance().getEconomySystem();
        if (!economy->has(sender, price)) {
            economy->sendNotEnoughMoneyMessage(sender, price, localeCode);
            ev.cancel();
            return;
        }

        if (!economy->reduce(sender, price)) {
            mc_utils::sendText<mc_utils::Error>(sender, "扣除 TPA 费用失败，请联系管理员"_trl(localeCode));
            ev.cancel();
        }
    }));

    mListeners.emplace_back(bus.emplaceListener<CreatedTpaRequestEvent>([](CreatedTpaRequestEvent& ev) {
        auto request  = ev.getRequest();
        auto sender   = request->getSender();
        auto receiver = request->getReceiver();
        auto type     = TpaRequest::getTypeString(request->getType());

        mc_utils::sendText(
            *sender,
            "已向 '{0}' 发起 '{1}' 请求"_trl(sender->getLocaleCode(), receiver->getRealName(), type)
        );
        mc_utils::sendText(
            *receiver,
            "收到来自 '{0}' 的 '{1}' 请求"_trl(receiver->getLocaleCode(), sender->getRealName(), type)
        );
    }));

    mListeners.emplace_back(bus.emplaceListener<TpaRequestAcceptedEvent>([](TpaRequestAcceptedEvent& ev) {
        auto sender   = ev.getRequest().getSender();
        auto receiver = ev.getRequest().getReceiver();
        auto type     = ev.getRequest().getType();

        mc_utils::sendText(
            *sender,
            "'{0}' 接受了您的 '{1}' 请求。"_trl(
                sender->getLocaleCode(),
                receiver->getRealName(),
                TpaRequest::getTypeString(type)
            )
        );
        mc_utils::sendText(
            *receiver,
            "您接受了来自 '{0}' 的 '{1}' 请求。"_trl(
                receiver->getLocaleCode(),
                sender->getRealName(),
                TpaRequest::getTypeString(type)
            )
        );
    }));

    mListeners.emplace_back(bus.emplaceListener<TpaRequestDeniedEvent>([](TpaRequestDeniedEvent& ev) {
        auto sender   = ev.getRequest().getSender();
        auto receiver = ev.getRequest().getReceiver();
        auto type     = ev.getRequest().getType();

        mc_utils::sendText<mc_utils::Error>(
            *sender,
            "'{0}' 拒绝了您的 '{1}' 请求。"_trl(
                sender->getLocaleCode(),
                receiver->getRealName(),
                TpaRequest::getTypeString(type)
            )
        );
        mc_utils::sendText<mc_utils::Warn>(
            *receiver,
            "您拒绝了来自 '{0}' 的 '{1}' 请求。"_trl(
                receiver->getLocaleCode(),
                sender->getRealName(),
                TpaRequest::getTypeString(type)
            )
        );
    }));

    mListeners.emplace_back(bus.emplaceListener<PlayerExecuteTpaAcceptOrDenyCommandEvent>(
        [this](PlayerExecuteTpaAcceptOrDenyCommandEvent& ev) {
            bool const isAccept   = ev.isAccept();
            auto&      receiver   = ev.getPlayer();
            auto const localeCode = receiver.getLocaleCode();

            if (receiver.isSleeping()) {
                mc_utils::sendText<mc_utils::Error>(receiver, "你不能在睡觉时使用此命令"_trl(localeCode));
                return;
            }

            auto& pool    = this->getRequestPool();
            auto  senders = pool.getSenders(receiver.getUuid());

            switch (senders.size()) {
            case 0:
                mc_utils::sendText<mc_utils::Error>(receiver, "您没有收到任何 TPA 请求"_trl(localeCode));
                return;
            case 1: {
                auto request = pool.getRequest(senders[0], receiver.getUuid());
                if (request) {
                    isAccept ? request->accept() : request->deny();
                } else {
                    mc_utils::sendText<mc_utils::Error>(receiver, "TPA 请求不存在"_trl(localeCode));
                    LeviTPSystem::getInstance().getSelf().getLogger().error("An unexpected request is null pointer.");
                }
                return;
            }
            default: {
                auto& infoDb = ll::service::PlayerInfo::getInstance();

                ll::form::SimpleForm fm;
                fm.setTitle("Tpa 请求列表 [{}]"_trl(localeCode, senders.size()));
                fm.setContent("选择一个要 接受/拒绝 的 TPA 请求"_trl(localeCode));

                for (auto& sender : senders) {
                    auto info = infoDb.fromUuid(sender);
                    fm.appendButton(
                        "发起者: {0}"_trl(localeCode, info.has_value() ? info->name : sender.asString()),
                        [&pool, sender, isAccept](Player& self) {
                            if (auto request = pool.getRequest(self.getUuid(), sender)) {
                                isAccept ? request->accept() : request->deny();
                            }
                        }
                    );
                }

                fm.sendTo(receiver);

                return;
            }
            }
        }
    ));

    TpaCommand::setup();

    return true;
}

bool TpaModule::disable() {
    mPool->stopCleanupCoro();

    auto& bus = ll::event::EventBus::getInstance();
    for (auto& listener : mListeners) {
        bus.removeListener(listener);
    }

    return true;
}

Cooldown& TpaModule::getCooldown() { return mCooldown; }

TpaRequestPool&       TpaModule::getRequestPool() { return *mPool; }
TpaRequestPool const& TpaModule::getRequestPool() const { return *mPool; }


} // namespace tps