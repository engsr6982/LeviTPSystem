#include "Menu.h"
#include "entry/Entry.h"
#include "ll/api/form/SimpleForm.h"
#include "utils/Mc.h"
#include "utils/McAPI.h"
#include <functional>
#include <memory>
#include <optional>



namespace tps::modules {

// 定义静态成员变量
std::filesystem::path                                         tps::modules::Menu::rootDir;
std::unordered_map<std::string, std::function<void(Player&)>> tps::modules::Menu::functions;

std::unique_ptr<Menu> Menu::fromJSON(const json& json) {
    auto& logger = tps::entry::getInstance().getSelf().getLogger();
    try {
        auto me = std::make_unique<Menu>();
        json.at("title").get_to(me->title);
        json.at("content").get_to(me->content);

        int                           i = 0;
        std::vector<Menu::ButtonItem> buttons;
        for (const auto& button : json.at("buttons")) {
            i++;
            try {
                Menu::ButtonItem it;
                button.at("title").get_to(it.title);
                button.at("imageData").get_to(it.imageData);
                button.at("imageType").get_to(it.imageType);
                button.at("callbackType").get_to(it.callbackType);
                button.at("callbackRun").get_to(it.callbackRun);
                buttons.push_back(it);
            } catch (...) {
                logger.error("Fail in Menu::fromJSON, buttons parse error in button #{}", i);
                continue;
            }
        }
        me->buttons = std::move(buttons);
        return me;
    } catch (...) {
        logger.error("Fail in Menu::fromJSON, parse error");
        return nullptr;
    }
}

std::unique_ptr<Menu> Menu::fromJsonFile(const string fileName) {
    auto& logger = tps::entry::getInstance().getSelf().getLogger();
    try {
        std::filesystem::path path = Menu::rootDir / fileName;
        if (path.extension() != ".json") {
            logger.error("Fail in Menu::fromJsonFile, not a json file");
            return nullptr;
        }
        if (!std::filesystem::exists(path)) {
            logger.error("Fail in Menu::fromJsonFile, file not exist");
            return nullptr;
        }
        std::ifstream file(path);
        json          jsonData;
        file >> jsonData;
        return fromJSON(jsonData);
    } catch (...) {
        logger.error("Fail in Menu::fromJsonFile, parse error");
        return nullptr;
    }
}
void Menu::fromJsonFile(Player& player) {
    auto me = fromJsonFile("index.json");
    if (me) me->sendTo(player);
    else utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "Menu file not found");
}

bool Menu::hasFunction(const string name) { return Menu::functions.find(name) != Menu::functions.end(); }

void Menu::sendTo(Player& player) {
    using namespace ll::form;
    auto& logger = tps::entry::getInstance().getSelf().getLogger();
    try {
        SimpleForm fm;
        fm.setTitle(title);
        fm.setContent(content);

        for (const auto& bt : buttons) {

            std::function<void(Player&)> callback = [bt](Player& p) {
                auto& logger = tps::entry::getInstance().getSelf().getLogger();
                if (bt.callbackType == "function") {
                    if (Menu::hasFunction(bt.callbackRun)) {
                        Menu::functions[bt.callbackRun](p);
                    } else logger.error("Fail in Menu::sendTo, function not found {}", bt.callbackRun);
                } else if (bt.callbackType == "cmd") {
                    api::executeCommand(bt.callbackRun, &p);
                } else if (bt.callbackType == "subform") {
                    auto sub = Menu::fromJsonFile(bt.callbackRun);
                    if (sub) sub->sendTo(p);
                    else logger.error("Fail in Menu::sendTo, subform not found {}", bt.callbackRun);
                } else {
                    logger.error("Fail in Menu::sendTo, unknown callback type {}", bt.callbackType);
                }
            };

            if (bt.imageType == "path" || bt.imageType == "url") {
                fm.appendButton(bt.title, bt.imageData, bt.imageType, callback);
            } else {
                fm.appendButton(bt.title, callback);
            }
        }

        fm.sendTo(player);
    } catch (...) {
        logger.error("Fail in Menu::sendTo, unknown error");
    }
}

} // namespace tps::modules