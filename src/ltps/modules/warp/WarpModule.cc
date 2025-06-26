#include "WarpModule.h"

#include "WarpCommand.h"
#include "event/WarpEvents.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/base/Config.h"
#include "ltps/common/PriceCalculate.h"
#include "ltps/database/PermissionStorage.h"
#include "ltps/database/StorageManager.h"
#include "ltps/utils/McUtils.h"

#include <ll/api/event/EventBus.h>

namespace ltps::warp {


WarpModule::WarpModule() = default;

std::vector<std::string> WarpModule::getDependencies() const { return {}; }

bool WarpModule::init() { return true; }

bool WarpModule::enable() {
    auto& bus = ll::event::EventBus::getInstance();

    mListeners.emplace_back(bus.emplaceListener<PlayerRequestGoWarpEvent>(
        [this](PlayerRequestGoWarpEvent& ev) {
            auto& bus = ll::event::EventBus::getInstance();

            auto& player     = ev.getPlayer();
            auto  realName   = player.getRealName();
            auto  localeCode = player.getLocaleCode();
            auto& name       = ev.getName();

            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("WarpStorage not found");
            }

            auto warp = storage->getWarp(name);
            if (!warp) {
                mc_utils::sendText<mc_utils::Error>(player, "公共传送点 {} 不存在"_trl(localeCode, name));
                ev.cancel();
                return;
            }

            auto teleporting = WarpTeleportingEvent(player, warp.value());
            bus.publish(teleporting);

            if (teleporting.isCancelled()) {
                ev.cancel();
                return;
            }

            warp->teleport(player);

            auto teleported = WarpTeleportedEvent(player, warp.value());

            bus.publish(teleported);
            ev.invokeCallback(warp.value());
        },
        ll::event::EventPriority::High
    ));
    mListeners.emplace_back(bus.emplaceListener<WarpTeleportingEvent>(
        [this](WarpTeleportingEvent& ev) {
            auto& player     = ev.getPlayer();
            auto  realName   = player.getRealName();
            auto  localeCode = player.getLocaleCode();

            auto& cooldown = getCooldown();

            if (cooldown.isCooldown(realName)) {
                mc_utils::sendText(
                    player,
                    "传送冷却中, 请稍后重试，冷却时间: {}"_trl(localeCode, cooldown.getCooldownString(realName))
                );
                ev.cancel();
                return;
            }

            auto cl = PriceCalculate(getConfig().modules.warp.goWarpCalculate);
            cl.addVariable("dimid", ev.getWarp().dimid);
            auto price = cl.eval();

            if (!price) {
                mc_utils::sendText<mc_utils::Error>(player, "计算价格失败"_trl(localeCode));
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "[WarpModule]: Calculate price failed! player: {}, warpName: {}, error: {}",
                    realName,
                    ev.getWarp().name,
                    price.error()
                );
                ev.cancel();
                return;
            }

            if (const auto& economy = EconomySystemManager::getInstance();
                !economy->reduce(player, static_cast<llong>(price.value()))) {
                economy->sendNotEnoughMoneyMessage(player, static_cast<llong>(price.value()), localeCode);
                ev.cancel();
                return;
            }

            cooldown.setCooldown(realName, getConfig().modules.warp.cooldownTime);
        },
        ll::event::EventPriority::High
    ));

    mListeners.emplace_back(bus.emplaceListener<PlayerRequestAddWarpEvent>(
        [this](PlayerRequestAddWarpEvent& ev) {
            auto&           player     = ev.getPlayer();
            auto            localeCode = player.getLocaleCode();
            RealName const& realName   = player.getRealName();

            auto warp = WarpStorage::Warp::make(player.getPosition(), player.getDimensionId(), ev.getName());

            auto& bus = ll::event::EventBus::getInstance();

            auto adding = WarpAddingEvent(player, warp);
            bus.publish(adding);
            if (adding.isCancelled()) {
                return;
            }

            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("WarpStorage not found");
            }

            auto res = storage->addWarp(warp);
            if (!res) {
                mc_utils::sendText<mc_utils::Error>(player, "添加公共传送点失败"_trl(localeCode));
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "[WarpModule]: Add warp failed! player: {}, warpName: {}, error: {}",
                    realName,
                    warp.name,
                    res.error()
                );
                ev.cancel();
                return;
            }

            mc_utils::sendText(player, "添加公共传送点成功"_trl(localeCode));

            auto added = WarpAddedEvent(player, warp);
            bus.publish(added);

            ev.invokeCallback(warp);
        },
        ll::event::EventPriority::High
    ));
    mListeners.emplace_back(bus.emplaceListener<WarpAddingEvent>(
        [this](WarpAddingEvent& ev) {
            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("WarpStorage not found");
            }

            auto&           player     = ev.getPlayer();
            auto            localeCode = player.getLocaleCode();
            RealName const& realName   = player.getRealName();

            auto const& dimid = ev.getWarp().dimid;
            if (getConfig().modules.warp.disallowedDimensions.contains(dimid)) {
                mc_utils::sendText<mc_utils::Error>(player, "该维度无法创建公共传送点"_trl(localeCode));
                ev.cancel();
                return;
            }

            auto pe = getStorageManager().getStorage<PermissionStorage>();
            if (pe && !pe->hasPermission(realName, PermissionStorage::Permission::AddWarp)) {
                mc_utils::sendText<mc_utils::Error>(player, "你没有权限创建公共传送点"_trl(localeCode));
                ev.cancel();
                return;
            }

            auto const& warpName = ev.getWarp().name;
            if (storage->hasWarp(warpName)) {
                mc_utils::sendText<mc_utils::Error>(player, "公共传送点名称重复，请使用其它名称"_trl(localeCode));
                ev.cancel();
                return;
            }
        },
        ll::event::EventPriority::High
    ));

    mListeners.emplace_back(bus.emplaceListener<PlayerRequestRemoveWarpEvent>(
        [this](PlayerRequestRemoveWarpEvent& ev) {
            auto& bus = ll::event::EventBus::getInstance();

            auto& player = ev.getPlayer();
            auto& name   = ev.getName();

            auto removeing = WarpRemovingEvent(player, name);
            bus.publish(removeing);
            if (removeing.isCancelled()) {
                ev.invokeCallback(false);
                ev.cancel();
                return;
            }

            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("WarpStorage not found");
            }

            auto res = storage->removeWarp(name);
            if (!res) {
                mc_utils::sendText<mc_utils::Error>(player, "删除公共传送点失败"_trl(player.getLocaleCode()));
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "[WarpModule]: Remove warp failed! player: {}, warpName: {}, error: {}",
                    player.getRealName(),
                    name,
                    res.error()
                );
                ev.invokeCallback(false);
                ev.cancel();
                return;
            }
            mc_utils::sendText(player, "删除公共传送点 {} 成功!"_trl(player.getLocaleCode(), name));

            auto removed = WarpRemovedEvent(player, name);
            bus.publish(removed);

            ev.invokeCallback(true);
        },
        ll::event::EventPriority::High
    ));
    mListeners.emplace_back(bus.emplaceListener<WarpRemovingEvent>(
        [this](WarpRemovingEvent& ev) {
            auto&           player     = ev.getPlayer();
            auto            localeCode = player.getLocaleCode();
            RealName const& realName   = player.getRealName();

            auto pe = getStorageManager().getStorage<PermissionStorage>();
            if (pe && !pe->hasPermission(realName, PermissionStorage::Permission::RemoveWarp)) {
                mc_utils::sendText<mc_utils::Error>(player, "你没有权限删除公共传送点"_trl(localeCode));
                ev.cancel();
                return;
            }
        },
        ll::event::EventPriority::High
    ));

    WarpCommand::setup();
    return true;
}

bool WarpModule::disable() {
    auto& bus = ll::event::EventBus::getInstance();
    for (auto& p : mListeners) {
        bus.removeListener(p);
    }
    mListeners.clear();

    return true;
}

WarpStorage* WarpModule::getStorage() const { return getStorageManager().getStorage<WarpStorage>(); }

Cooldown& WarpModule::getCooldown() { return mCooldown; }


} // namespace ltps::warp