#include "Command.h"
#include "event/LevelDBIllegalOperationEvent.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/event/Event.h"
#include "ll/api/event/EventBus.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"

namespace lbm::plugin::tpsystem::command {

struct ParamKey {
    string key1;
    string key2;
};

struct ParamFileName {
    string fileName;
};

void registerCommandWithLevelDB(const string& name) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(name);

    // tps leveldb list [key1] [key2]
    cmd.overload<ParamKey>().text("leveldb").text("list").optional("key1").optional("key2").execute(
        [](CommandOrigin const& origin, CommandOutput& output, const ParamKey& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::DedicatedServer);
            auto& db = tpsystem::data::LevelDB::getInstance();

            if (!param.key1.empty() && param.key2.empty()) {
                auto raw = db.getRaw(param.key1);
                sendText(output, raw);
            } else if (!param.key1.empty() && !param.key2.empty()) {
                auto json = db.getJson(param.key1);
                if (json.find(param.key2) != json.end()) {
                    sendText(output, json[param.key2].dump());
                } else {
                    sendText(output, "键 {0} 不存在"_tr(param.key2));
                }
            } else {
                auto keys = db.getAllKeys();
                sendText(output, "键列表: {0}"_tr(join(keys)));
            }
        }
    );

    // tps leveldb del <key1> [key2]
    cmd.overload<ParamKey>().text("leveldb").text("del").required("key1").optional("key2").execute(
        [](CommandOrigin const& origin, CommandOutput& output, const ParamKey& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::DedicatedServer);
            auto& db = tpsystem::data::LevelDB::getInstance();
            if (param.key2.empty()) {
                db.getDB().del(param.key1);
                sendText(output, "键 {0} 已删除"_tr(param.key1));
            } else {
                auto json = db.getJson(param.key1);
                if (json.find(param.key2) != json.end()) {
                    json.erase(param.key2);
                    db.setJson(param.key1, json);
                    sendText(output, "键 {0} 的子键 {1} 已删除"_tr(param.key1, param.key2));
                } else {
                    sendText<MsgLevel::Error>(output, "键 {0} 的子键 {1} 不存在"_tr(param.key1, param.key2));
                }
            }
            db.initKey();
            ll::event::EventBus::getInstance().publish(event::LevelDBIllegalOperationEvent()); // 触发事件
        }
    );

    // tps leveldb import <fileName>
    cmd.overload<ParamFileName>()
        .text("leveldb")
        .text("import")
        .required("fileName")
        .execute([](CommandOrigin const& origin, CommandOutput& output, const ParamFileName& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::DedicatedServer);
            sendText(output, "正在导入数据..."_tr());
            bool isSuccess = tpsystem::data::LevelDB::getInstance().importData(param.fileName);
            if (isSuccess) sendText<MsgLevel::Success>(output, "导入成功!"_tr());
            else sendText<MsgLevel::Error>(output, "导入失败!"_tr());
            ll::event::EventBus::getInstance().publish(event::LevelDBIllegalOperationEvent()); // 触发事件
        });

    // tps leveldb export
    cmd.overload().text("leveldb").text("export").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::DedicatedServer);
        bool isSuccess = tpsystem::data::LevelDB::getInstance().exportData();
        if (isSuccess) sendText<MsgLevel::Success>(output, "导出成功! "_tr());
        else sendText<MsgLevel::Error>(output, "导出失败!"_tr());
    });
}

} // namespace lbm::plugin::tpsystem::command