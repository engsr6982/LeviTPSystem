#include "PermissionStorage.h"
#include "ll/api/io/FileUtils.h"
#include "ltps/TeleportSystem.h"
#include "ltps/utils/JsonUtls.h"
#include "magic_enum/magic_enum.hpp"
#include "mc/world/actor/player/Player.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include <filesystem>


namespace ltps {


PermissionStorage::PermissionStorage() = default;

void PermissionStorage::load() {
    if (_hasLegacyPermissionFile()) {
        _tryLoadLegacyPermissionFile(); // 加载旧版权限文件
        _renameLegacyPermissionFile();  // 重命名旧版权限文件
        writeBack();                    // 将权限写入数据库
        TeleportSystem::getInstance().getSelf().getLogger().trace("Loaded legacy permission file");
        return;
    }
    TeleportSystem::getInstance().getSelf().getLogger().trace("No legacy permission file found");

    auto& db = getDatabase();

    if (!db.has(STORAGE_KEY)) {
        db.set(STORAGE_KEY, "{}");
    }

    auto rawJson = db.get(STORAGE_KEY);
    if (!rawJson.has_value()) {
        throw std::runtime_error("Failed to load permissions");
    }

    try {
        auto json = nlohmann::json::parse(rawJson.value());

        json_utils::json2structTryPatch(mData, json);

        TeleportSystem::getInstance().getSelf().getLogger().info(
            "Loaded permissions, {} entries",
            mData.mPlayerPerms.size()
        );
    } catch (nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to parse permissions: " + std::string(e.what()));
    }
}

void PermissionStorage::unload() { writeBack(); }

void PermissionStorage::writeBack() {
    auto& db   = getDatabase();
    auto  json = json_utils::struct2json(mData);
    db.set(STORAGE_KEY, json.dump());
}


bool PermissionStorage::_hasLegacyPermissionFile() const {
    auto path = TeleportSystem::getInstance().getSelf().getDataDir() / LEGACY_FILE_NAME;
    return std::filesystem::exists(path);
}

void PermissionStorage::_tryLoadLegacyPermissionFile() {
    if (!_hasLegacyPermissionFile()) {
        return;
    }

    auto path    = TeleportSystem::getInstance().getSelf().getDataDir() / LEGACY_FILE_NAME;
    auto content = ll::file_utils::readFile(path);
    if (!content.has_value()) {
        throw std::runtime_error("Failed to read legacy permission file");
    }

    try {
        auto json = nlohmann::json::parse(content.value());

        json_utils::json2structTryPatch(mData, json);

        TeleportSystem::getInstance().getSelf().getLogger().info(
            "Loaded legacy permissions, {} entries",
            mData.mPlayerPerms.size()
        );
    } catch (nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to parse legacy permission file: " + std::string(e.what()));
    }
}

void PermissionStorage::_renameLegacyPermissionFile() const {
    if (!_hasLegacyPermissionFile()) {
        return;
    }
    auto path = TeleportSystem::getInstance().getSelf().getDataDir() / LEGACY_FILE_NAME;
    std::filesystem::rename(path, path.replace_extension(".old"));
}


bool PermissionStorage::hasDefaultPermission(Permission permission) const {
    return (mData.mDefaultPerms & static_cast<int>(permission)) != 0;
}

bool PermissionStorage::hasPermission(RealName const& realName, Permission permission, bool includeDefault) const {
    if (includeDefault && hasDefaultPermission(permission)) return true;
    if (!mData.mPlayerPerms.contains(realName)) return false;
    return (mData.mPlayerPerms.at(realName) & static_cast<int>(permission)) != 0;
}

Result<void> PermissionStorage::grantPermission(RealName const& realName, Permission permission) {
    if (hasPermission(realName, permission, false)) return std::unexpected("Permission already granted");
    mData.mPlayerPerms[realName] |= static_cast<int>(permission);
    return {};
}

Result<void> PermissionStorage::revokePermission(RealName const& realName, Permission permission) {
    if (!hasPermission(realName, permission, false)) return std::unexpected("Permission not granted");
    mData.mPlayerPerms[realName] &= ~static_cast<int>(permission);
    return {};
}

std::vector<PermissionStorage::Permission> PermissionStorage::getPermissions(RealName const& realName) const {
    if (!mData.mPlayerPerms.contains(realName)) return {};
    std::vector<Permission> result;
    for (auto const& p : magic_enum::enum_values<Permission>()) {
        if (hasPermission(realName, p, false)) result.push_back(p);
    }
    return result;
}


Result<void> PermissionStorage::grantDefaultPermission(Permission permission) {
    if (hasDefaultPermission(permission)) return std::unexpected("Permission already granted");
    mData.mDefaultPerms |= static_cast<int>(permission);
    return {};
}

Result<void> PermissionStorage::revokeDefaultPermission(Permission permission) {
    if (!hasDefaultPermission(permission)) return std::unexpected("Permission not granted");
    mData.mDefaultPerms &= ~static_cast<int>(permission);
    return {};
}

std::vector<PermissionStorage::Permission> PermissionStorage::getDefaultPermissions() const {
    std::vector<Permission> result;
    for (auto const& p : magic_enum::enum_values<Permission>()) {
        if (hasDefaultPermission(p)) result.push_back(p);
    }
    return result;
}

Result<std::pair<std::vector<PermissionStorage::Permission>, std::vector<PermissionStorage::Permission>>>
PermissionStorage::tracePermissions(RealName const& realName) const {
    if (!mData.mPlayerPerms.contains(realName)) {
        return std::unexpected("Player not found");
    }
    auto defaultPerms = getDefaultPermissions();
    auto playerPerms  = getPermissions(realName);
    return std::make_pair<std::vector<PermissionStorage::Permission>, std::vector<PermissionStorage::Permission>>(
        std::move(defaultPerms),
        std::move(playerPerms)
    );
}


std::string PermissionStorage::toString(Permission permission) {
    return std::string(magic_enum::enum_name(permission));
}

PermissionStorage::Permission PermissionStorage::fromString(std::string const& str) {
    return magic_enum::enum_cast<Permission>(str).value_or(Permission::None);
}

std::vector<PermissionStorage::Permission> PermissionStorage::getPermissions() {
    auto vals = magic_enum::enum_values<PermissionStorage::Permission>();
    return {vals.begin(), vals.end()};
}

Result<std::vector<PermissionStorage::Permission>> PermissionStorage::resolve(std::string const& permissions) {
    if (permissions.empty()) {
        return std::unexpected("Please provide permissions");
    }
    auto&                                      logger = TeleportSystem::getInstance().getSelf().getLogger();
    std::vector<PermissionStorage::Permission> result;
    std::stringstream                          ss(permissions);
    std::string                                token;
    while (std::getline(ss, token, '|')) {
        if (token.empty()) {
            logger.trace("Empty token, skipping");
            continue;
        }
        auto perm = magic_enum::enum_cast<PermissionStorage::Permission>(token);
        if (!perm.has_value()) {
            return std::unexpected("Parsing failed, invalid permissions: " + token);
        }
        logger.trace("Parsed permission: {} -> {}", token, perm.value());
        result.push_back(perm.value());
    }
    return result;
}

} // namespace ltps