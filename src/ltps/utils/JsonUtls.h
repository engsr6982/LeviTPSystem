#pragma once
#include "ll/api/reflection/Deserialization.h"
#include "ll/api/reflection/Serialization.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace ltps::json_utils {


template <typename T>
[[nodiscard]] inline nlohmann::ordered_json struct2json(T& obj) {
    return ll::reflection::serialize<nlohmann::ordered_json>(obj).value();
}

template <typename T, class J = nlohmann::ordered_json>
inline void json2struct(T& obj, J& json) {
    ll::reflection::deserialize(obj, json).value();
}

template <typename T, class J = nlohmann::ordered_json>
inline void json2structTryPatch(T& obj, J& json) {
    auto patch = struct2json(obj);
    patch.merge_patch(json);
    json2struct(obj, patch);
}


template <class T>
concept HasVersion =
    ll::reflection::Reflectable<T> && std::integral<std::remove_cvref_t<decltype((std::declval<T>().version))>>;

template <HasVersion T, class J = nlohmann::ordered_json>
bool merge_patch(T& obj, J& data) {
    data.erase("version");
    auto patch = ll::reflection::serialize<J>(obj);
    patch.value().merge_patch(data);
    data = *std::move(patch);
    return true;
}

template <HasVersion T, class J = nlohmann::ordered_json, class F = bool(T&, J&)>
inline void json2structTryPatch(J& json, T& obj, F&& fixer = merge_patch<T, J>) {
    bool noNeedMerge = true;
    if (!json.contains("version") || (int64)(json["version"]) != obj.version) {
        noNeedMerge = false;
    }
    if (noNeedMerge || std::invoke(std::forward<F>(fixer), obj, json)) {
        ll::reflection::deserialize(obj, json).value();
    }
}


} // namespace ltps::json_utils