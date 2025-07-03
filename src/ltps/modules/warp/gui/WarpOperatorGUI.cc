#include "WarpOperatorGUI.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ltps/Global.h"
#include "ltps/TeleportSystem.h"
#include "ltps/common/BackSimpleForm.h"
#include "ltps/modules/warp/event/WarpEvents.h"
#include "ltps/utils/McUtils.h"
#include "mc/world/level/dimension/VanillaDimensions.h"
#include <utility>


namespace ltps::warp {


using ll::form::CustomForm;
using ll::form::SimpleForm;


void WarpOperatorGUI::sendMainGUI(Player& player) { sendChooseWarpGUI(player, sendOperatorMenu); }

void WarpOperatorGUI::sendChooseWarpGUI(Player& player, ChooseWarpCallback callback) {
    auto storage = TeleportSystem::getInstance().getStorageManager().getStorage<WarpStorage>();
    if (!storage) {
        return;
    }

    auto localeCode = player.getLocaleCode();

    auto& warps = storage->getWarps();

    auto fm = BackSimpleForm();
    fm.setTitle("Teleport System - Warp Manager"_trl(localeCode));
    fm.setContent("共有 {} 个传送点, 请选择一个: "_trl(localeCode, warps.size()));

    fm.appendButton("创建"_trl(localeCode), "textures/ui/color_plus", "path", [](Player& self) {
        sendCreateOrEditWarpGUI(self);
    });

    for (auto const& warp : warps) {
        auto _home = warp;
        fm.appendButton(warp.name, [callback, warp = std::move(_home)](Player& self) { callback(self, warp); });
    }

    fm.sendTo(player);
}

void WarpOperatorGUI::sendOperatorMenu(Player& player, WarpStorage::Warp warp) {
    auto localeCode = player.getLocaleCode();

    BackSimpleForm::make<sendChooseWarpGUI>(sendOperatorMenu)
        .setTitle("Teleport System - Warp Manager"_trl(localeCode))
        .setContent("名称: {}\n坐标: {}\n创建时间: {}\n修改时间: {}"_trl(
            localeCode,
            warp.name,
            warp.toPosString(),
            warp.createdTime,
            warp.modifiedTime
        ))
        .appendButton(
            "前往"_trl(localeCode),
            "textures/ui/send_icon",
            "path",
            [warp](Player& self) { ll::event::EventBus::getInstance().publish(AdminRequestGoWarpEvent{self, warp}); }
        )
        .appendButton(
            "编辑"_trl(localeCode),
            "textures/ui/book_edit_default",
            "path",
            [warp](Player& self) { sendCreateOrEditWarpGUI(self, warp); }
        )
        .appendButton(
            "删除"_trl(localeCode),
            "textures/ui/trash_default",
            "path",
            [warp](Player& self) {
                ll::event::EventBus::getInstance().publish(AdminRequestRemoveWarpEvent{self, warp});
            }
        )
        .sendTo(player);
}

void WarpOperatorGUI::sendCreateOrEditWarpGUI(Player& player, std::optional<WarpStorage::Warp> warp) {
    auto localeCode = player.getLocaleCode();

    CustomForm fm{"Warp Manager - Create Warp"_trl(localeCode)};
    fm.appendInput("name", "请输入名称: "_trl(localeCode), "string", warp ? warp->name : "");
    fm.appendInput(
        "pos",
        "请输入坐标: "_trl(localeCode),
        "string",
        (warp ? "{},{},{}"_tr(warp->x, warp->y, warp->z) : ""),
        "使用半角逗号分隔坐标, 例如: x,y,z"_tr(localeCode)
    );

    auto&                           dimMap = VanillaDimensions::DimensionMap();
    static std::vector<std::string> dimNames;
    if (dimNames.empty() || dimMap.mRight.size() != dimNames.size()) {
        dimNames.clear();
        dimNames.reserve(dimMap.mRight.size());
        for (auto const& pair : dimMap.mRight) {
            dimNames.push_back(pair.first);
        }
    }

    auto index = 0;
    if (warp) {
        auto nameIter = dimMap.mLeft.find(warp->dimid);
        if (nameIter != dimMap.mLeft.end()) {
            auto const& dimName = nameIter->second; // std::string
            auto        it      = std::find(dimNames.begin(), dimNames.end(), dimName);
            if (it != dimNames.end()) {
                index = static_cast<int>(std::distance(dimNames.begin(), it));
            }
        }
    }

    fm.appendDropdown("dimName", "请选择一个维度: "_trl(localeCode), dimNames, index);

    fm.sendTo(
        player,
        [localeCode = std::move(localeCode),
         warp       = std::move(warp)](Player& self, ll::form::CustomFormResult const& result, auto) {
            if (!result) {
                return;
            }

            auto name = std::get<std::string>(result->at("name"));

            int dimid = -1;
            {
                auto  dimName = std::get<std::string>(result->at("dimName"));
                auto& dimMap  = VanillaDimensions::DimensionMap();
                auto  dimIter = dimMap.mRight.find(dimName);
                if (dimIter == dimMap.mRight.end()) {
                    mc_utils::sendText<mc_utils::Error>(self, "无效的维度名称"_trl(localeCode));
                    return;
                }
                dimid = dimIter->second;
            }

            Vec3 v3; // x, y, z
            {
                auto posStr = std::get<std::string>(result->at("pos"));

                std::vector<std::string> parts;
                std::istringstream       ss(posStr);
                std::string              part;
                while (std::getline(ss, part, ',')) {
                    parts.push_back(part);
                }
                if (parts.size() != 3) {
                    mc_utils::sendText<mc_utils::Error>(self, "坐标格式错误"_trl(localeCode));
                    return;
                } else {
                    try {
                        v3.x = std::stof(parts[0]);
                        v3.y = std::stof(parts[1]);
                        v3.z = std::stof(parts[2]);
                    } catch (...) {
                        mc_utils::sendText<mc_utils::Error>(self, "捕获到异常，请检查坐标"_trl(localeCode));
                        return;
                    }
                }
            }

            if (warp) {
                auto newWarp = WarpStorage::Warp::make(v3, dimid, name);
                ll::event::EventBus::getInstance().publish(AdminRequestEditWarpEvent{self, warp.value(), newWarp});
            } else {
                ll::event::EventBus::getInstance().publish(AdminRequestCreateWarpEvent{self, name, dimid, v3});
            }
        }
    );
}


} // namespace ltps::warp