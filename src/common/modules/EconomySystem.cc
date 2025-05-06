#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"

#include "levitpsystem/common/modules/EconomySystem.h"
#include <stdexcept>


namespace tps {

std::shared_ptr<EconomySystem> GlobalEconomySystem = nullptr;
std::mutex                     EconomySystem::mInstanceMutex;

std::shared_ptr<EconomySystem> createEconomySystem(EconomySystem::EconomyConfig config) {
    switch (config.kit) {
    case tps::EconomySystem::EconomyKit::LegacyMoney: {
        return std::make_shared<internals::LegacyMoneyEconomySystem>(std::move(config));
    }

    case tps::EconomySystem::EconomyKit::ScoreBoard: {
        throw std::runtime_error("ScoreBoard Economy System not implemented yet.");
    }
    }
}

void EconomySystem::init(EconomyConfig config) {
    std::lock_guard<std::mutex> lock(mInstanceMutex);
    if (GlobalEconomySystem) {
        throw std::runtime_error("EconomySystem already initialized.");
    }
    GlobalEconomySystem = createEconomySystem(std::move(config));
}

void EconomySystem::reload(EconomyConfig config) {
    std::lock_guard<std::mutex> lock(mInstanceMutex);
    if (!GlobalEconomySystem) {
        throw std::runtime_error("EconomySystem not initialized.");
    }
    if (GlobalEconomySystem->mConfig.kit == config.kit) {
        GlobalEconomySystem->mConfig = std::move(config);
        return;
    }
    GlobalEconomySystem = createEconomySystem(std::move(config));
}

std::shared_ptr<EconomySystem> EconomySystem::getInstance() {
    std::lock_guard<std::mutex> lock(mInstanceMutex);
    if (!GlobalEconomySystem) {
        throw std::runtime_error("EconomySystem not initialized.");
    }
    return GlobalEconomySystem;
}

EconomySystem::EconomySystem(EconomyConfig config) : mConfig(std::move(config)) {}

std::string EconomySystem::getCostMessage(Player& player, long long amount) const {
    using namespace ll::i18n_literals;
    // using ll::operator""_trl;
    // TODO: Implement this
    return "abc"_trl("");
    // return std::string{ll::i18n::getInstance().get("abc", "")};
}

void EconomySystem::sendNotEnoughMoneyMessage(Player& player, long long amount) const {}

namespace internals {

LegacyMoneyEconomySystem::LegacyMoneyEconomySystem(EconomyConfig config) : EconomySystem(std::move(config)) {}

long long LegacyMoneyEconomySystem::get(Player& player) const { return 0; }
long long LegacyMoneyEconomySystem::get(mce::UUID const& uuid) const { return 0; }

bool LegacyMoneyEconomySystem::set(Player& player, long long amount) const { return 0; }
bool LegacyMoneyEconomySystem::set(mce::UUID const& uuid, long long amount) const { return 0; }

bool LegacyMoneyEconomySystem::add(Player& player, long long amount) const { return 0; }
bool LegacyMoneyEconomySystem::add(mce::UUID const& uuid, long long amount) const { return 0; }

bool LegacyMoneyEconomySystem::reduce(Player& player, long long amount) const { return 0; }
bool LegacyMoneyEconomySystem::reduce(mce::UUID const& uuid, long long amount) const { return 0; }

bool LegacyMoneyEconomySystem::transfer(Player& from, Player& to, long long amount) const { return 0; }
bool LegacyMoneyEconomySystem::transfer(mce::UUID const& from, mce::UUID const& to, long long amount) const {
    return 0;
}


} // namespace internals


} // namespace tps
