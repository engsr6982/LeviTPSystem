#include "ltps/modules/home/HomeModule.h"
#include "HomeStorage.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ltps/Global.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/base/Config.h"
#include "ltps/common/EconomySystem.h"
#include "ltps/common/PriceCalculate.h"
#include "ltps/database/PermissionStorage.h"
#include "ltps/database/StorageManager.h"
#include "ltps/modules/home/HomeCommand.h"
#include "ltps/modules/home/event/HomeEvents.h"
#include "ltps/utils/McUtils.h"
namespace ltps::home {

HomeModule::HomeModule() = default;

std::vector<std::string> HomeModule::getDependencies() const { return {}; }

bool HomeModule::init() { return true; }

bool HomeModule::enable() {
    auto& bus = ll::event::EventBus::getInstance();

    mListeners.emplace_back(bus.emplaceListener<PlayerRequestAddHomeEvent>(
        [this](PlayerRequestAddHomeEvent& ev) {
            auto&           player     = ev.getPlayer();
            auto            localeCode = player.getLocaleCode();
            RealName const& realName   = player.getRealName();

            auto home = HomeStorage::Home::make(player.getPosition(), player.getDimensionId(), ev.getName());

            auto& bus = ll::event::EventBus::getInstance();

            auto adding = HomeAddingEvent(player, home);
            bus.publish(adding);
            if (adding.isCancelled()) {
                return;
            }

            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("HomeStorage not found");
            }

            auto res = storage->addHome(realName, home);
            if (!res) {
                mc_utils::sendText<mc_utils::Error>(player, "添加家园失败"_trl(localeCode));
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "[HomeModule]: Add home failed! player: {}, homeName: {}, error: {}",
                    realName,
                    home.name,
                    res.error()
                );
                ev.cancel();
                return;
            }

            mc_utils::sendText(player, "添加家园成功"_trl(localeCode));

            auto added = HomeAddedEvent(player, home);
            bus.publish(added);

            ev.invokeCallback(home);
        },
        ll::event::EventPriority::High
    ));

    mListeners.emplace_back(bus.emplaceListener<HomeAddingEvent>(
        [this](HomeAddingEvent& ev) {
            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("HomeStorage not found");
            }

            auto&           player     = ev.getPlayer();
            auto            localeCode = player.getLocaleCode();
            RealName const& realName   = player.getRealName();

            auto const& dimid = ev.getHome().dimid;
            if (getConfig().modules.home.disallowedDimensions.contains(dimid)) {
                mc_utils::sendText<mc_utils::Error>(player, "该维度无法创建家园"_trl(localeCode));
                ev.cancel();
                return;
            }

            auto const& homeName = ev.getHome().name;

            if (storage->hasHome(realName, homeName)) {
                mc_utils::sendText<mc_utils::Error>(player, "家园名称重复，请使用其它名称"_trl(localeCode));
                ev.cancel();
                return;
            }

            auto count = 0;
            if (storage->hasPlayer(realName)) {
                if (auto res = storage->getHomeCount(realName)) {
                    count = res.value();
                }
            }

            bool unLimited = false;
            if (auto pe = getStorageManager().getStorage<PermissionStorage>()) {
                unLimited = pe->hasPermission(realName, PermissionStorage::Permission::UnlimitedHome);
            }

            if (count > getConfig().modules.home.maxHome && !unLimited) {
                mc_utils::sendText<mc_utils::Error>(player, "家园数量超过上限，无法创建"_trl(localeCode));
                ev.cancel();
                return;
            }

            PriceCalculate cl(getConfig().modules.home.createHomeCalculate);
            cl.addVariable("count", count);

            auto price = cl.eval();
            if (!price.has_value()) {
                mc_utils::sendText<mc_utils::Error>(player, "计算价格失败"_trl(localeCode));
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "[HomeModule]: Calculate price failed! player: {}, homeName: {}, count: {}, error: {}",
                    realName,
                    homeName,
                    count,
                    price.error()
                );
                ev.cancel();
                return;
            }

            if (!EconomySystemManager::getInstance()->reduce(player, static_cast<llong>(price.value()))) {
                mc_utils::sendText<mc_utils::Error>(player, "经济不足，无法创建"_trl(localeCode));
                ev.cancel();
                return;
            }
        },
        ll::event::EventPriority::High
    ));

    mListeners.emplace_back(bus.emplaceListener<PlayerRequestRemoveHomeEvent>(
        [this](PlayerRequestRemoveHomeEvent& ev) {
            auto& bus = ll::event::EventBus::getInstance();

            auto& player = ev.getPlayer();
            auto& name   = ev.getName();

            auto removeing = HomeRemovingEvent(player, name);
            bus.publish(removeing);
            if (removeing.isCancelled()) {
                ev.invokeCallback(false);
                ev.cancel();
                return;
            }

            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("HomeStorage not found");
            }

            auto res = storage->removeHome(player.getRealName(), name);
            if (!res) {
                mc_utils::sendText<mc_utils::Error>(player, "删除家园失败"_trl(player.getLocaleCode()));
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "[HomeModule]: Remove home failed! player: {}, homeName: {}, error: {}",
                    player.getRealName(),
                    name,
                    res.error()
                );
                ev.invokeCallback(false);
                ev.cancel();
                return;
            }

            auto removed = HomeRemovedEvent(player, name);
            bus.publish(removed);

            ev.invokeCallback(true);
        },
        ll::event::EventPriority::High
    ));

    mListeners.emplace_back(bus.emplaceListener<PlayerRequestGoHomeEvent>(
        [this](PlayerRequestGoHomeEvent& ev) {
            auto& bus = ll::event::EventBus::getInstance();


            auto& player     = ev.getPlayer();
            auto  realName   = player.getRealName();
            auto  localeCode = player.getLocaleCode();
            auto& name       = ev.getName();

            auto storage = getStorage();
            if (!storage) {
                throw std::runtime_error("HomeStorage not found");
            }

            auto home = storage->getHome(realName, name);
            if (!home) {
                mc_utils::sendText<mc_utils::Error>(player, "家园不存在"_trl(localeCode));
                ev.cancel();
                return;
            }

            auto teleporting = HomeTeleportingEvent(player, home.value());
            bus.publish(teleporting);

            if (teleporting.isCancelled()) {
                ev.cancel();
                return;
            }

            home->teleport(player);

            auto teleported = HomeTeleportedEvent(player, home.value());

            bus.publish(teleported);
            ev.invokeCallback(home.value());
        },
        ll::event::EventPriority::High
    ));

    mListeners.emplace_back(bus.emplaceListener<HomeTeleportingEvent>(
        [this](HomeTeleportingEvent& ev) {
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

            cooldown.setCooldown(realName, getConfig().modules.home.cooldownTime);
        },
        ll::event::EventPriority::High
    ));


    HomeCommand::setup();

    return true;
}

bool HomeModule::disable() {
    auto& bus = ll::event::EventBus::getInstance();
    for (auto& ptr : mListeners) {
        bus.removeListener(ptr);
    }
    mListeners.clear();

    return true;
}

HomeStorage* HomeModule::getStorage() const { return getStorageManager().getStorage<HomeStorage>(); }

Cooldown& HomeModule::getCooldown() { return mCooldown; }

} // namespace ltps::home