#include "HomeGUI.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ltps/Global.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/base/Config.h"
#include "ltps/common/BackSimpleForm.h"
#include "ltps/modules/home/HomeStorage.h"
#include "ltps/modules/home/event/HomeEvents.h"
#include "ltps/utils/McUtils.h"


namespace ltps::home {

using ll::form::CustomForm;
using ll::form::CustomFormResult;
using ll::form::SimpleForm;

void HomeGUI::sendMainMenu(Player& player, BackCB backCB) {
    auto localeCode = player.getLocaleCode();

    auto fm = BackSimpleForm{std::move(backCB)};
    fm.setTitle("Home Menu"_trl(localeCode))
        .appendLabel(" · 请选择一个操作"_trl(localeCode))
        .appendButton("新建家"_tr(), "textures/ui/color_plus", "path", [](Player& self) { sendAddHomeGUI(self); })
        .appendButton(
            "前往家"_tr(),
            "textures/ui/send_icon",
            "path",
            [](Player& self) { sendGoHomeGUI(self, BackSimpleForm::makeCallback<HomeGUI::sendMainMenu>(BackCB{})); }
        )
        .appendButton(
            "编辑家"_tr(),
            "textures/ui/book_edit_default",
            "path",
            [](Player& self) { sendEditHomeGUI(self, BackSimpleForm::makeCallback<HomeGUI::sendMainMenu>(BackCB{})); }
        )
        .appendButton(
            "删除家"_tr(),
            "textures/ui/trash_default",
            "path",
            [](Player& self) { sendRemoveHomeGUI(self, BackSimpleForm::makeCallback<HomeGUI::sendMainMenu>(BackCB{})); }
        )
        .sendTo(player);
}

void HomeGUI::sendAddHomeGUI(Player& player) {
    auto localeCode = player.getLocaleCode();

    CustomForm fm{"Home - Add"_trl(localeCode)};
    fm.appendLabel("输入要创建的家园名称，例如: My Home\n注意：家园名称不能超过 {} 个字符。"_trl(
        localeCode,
        getConfig().modules.home.nameLength
    ));

    fm.appendInput(
        "name",
        "请输入家园名称",
        "string",
        "",
        "不能超过 {} 个字符哦!"_trl(localeCode, getConfig().modules.home.nameLength)
    );

    fm.sendTo(player, [localeCode{std::move(localeCode)}](Player& self, CustomFormResult const& result, auto) {
        if (!result) return;

        auto name = std::get<std::string>(result->at("name"));
        if (name.empty()) {
            mc_utils::sendText<mc_utils::Error>(self, "家园名称不能为空哦!"_trl(localeCode));
            return;
        }

        ll::event::EventBus::getInstance().publish(PlayerRequestAddHomeEvent(self, name));
    });
}

void HomeGUI::sendChooseHomeGUI(Player& player, ChooseCallBack chooseCB, BackCB backCB) {
    auto localeCode = player.getLocaleCode();

    auto fm = BackSimpleForm{std::move(backCB)};
    fm.setTitle("Choose Home"_trl(localeCode)).setContent("请选择一个家"_trl(localeCode));

    auto storage = LeviTPSystem::getInstance().getStorageManager().getStorage<HomeStorage>();

    auto homes = storage->getHomes(player.getRealName());
    for (auto const& home : homes) {
        fm.appendButton(home.name, [cb = std::move(chooseCB), name = home.name](Player& self) { cb(self, name); });
    }

    fm.sendTo(player);
}


void HomeGUI::sendGoHomeGUI(Player& player, BackCB backCB) {
    sendChooseHomeGUI(
        player,
        [](Player& self, std::string name) {
            ll::event::EventBus::getInstance().publish(PlayerRequestGoHomeEvent(self, std::move(name)));
        },
        std::move(backCB)
    );
}

void HomeGUI::sendRemoveHomeGUI(Player& player, BackCB backCB) {
    sendChooseHomeGUI(
        player,
        [](Player& self, std::string name) {
            ll::event::EventBus::getInstance().publish(PlayerRequestRemoveHomeEvent(self, std::move(name)));
        },
        std::move(backCB)
    );
}


void HomeGUI::sendEditHomeGUI(Player& player, BackCB backCB) {
    // TODO: implement
}

void HomeGUI::_sendEditHomeNameGUI(Player& player, std::string const& name, BackCB backCB) {
    // TODO: implement
}


} // namespace ltps::home