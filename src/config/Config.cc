#include "Config.h"
#include "entry/Entry.h"
#include "ll/api/Config.h"
#include "ll/api/i18n/I18n.h"

namespace tps::config {

using ll::i18n_literals::operator""_tr;

Config cfg;

bool loadConfig() {
    auto&      mSelf       = tps::entry::getInstance().getSelf();
    const auto path        = mSelf.getModDir() / "Config.json";
    bool       isNotFailed = ll::config::loadConfig(cfg, path);

    auto& logger = mSelf.getLogger();
    if (!isNotFailed) {
        logger.warn("加载配置文件失败，配置文件错误或版本不匹配!"_tr());
    }
    return isNotFailed;
}


} // namespace tps::config