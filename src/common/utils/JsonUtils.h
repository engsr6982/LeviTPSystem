#pragma once
#include "ll/api/reflection/Deserialization.h"
#include "ll/api/reflection/Serialization.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include <algorithm>
#include <string_view>


namespace tps {


template <class T>
concept HasVersion =
    ll::reflection::Reflectable<T> && std::integral<std::remove_cvref_t<decltype((std::declval<T>().version))>>;


class JsonUtils {
public:
    JsonUtils()                            = delete;
    JsonUtils(JsonUtils&&)                 = delete;
    JsonUtils(const JsonUtils&)            = delete;
    JsonUtils& operator=(JsonUtils&&)      = delete;
    JsonUtils& operator=(const JsonUtils&) = delete;

    static nlohmann::json parse(std::string const& str) { return nlohmann::json::parse(str); }
    static nlohmann::json parse(std::string_view const& str) { return nlohmann::json::parse(str); }
    static std::string    stringify(nlohmann::json const& j, int indent = -1) { return j.dump(indent); }

    // class/vector/struct -> json
    template <class T>
    static nlohmann::ordered_json struct2json(T& obj) {
        return ll::reflection::serialize<nlohmann::ordered_json>(obj).value();
    }

    template <class T, class J = nlohmann::ordered_json>
    static void json2struct(J& j, T& obj) {
        ll::reflection::deserialize(obj, j).value();
    }

    template <HasVersion T, class J = nlohmann::ordered_json, class F = bool(T&, J&)>
    static void json2struct_version_patch(J& j, T& obj, F&& fixer = _tryMergePatch<T, J>) {
        bool noNeedMerge = true;
        if (!j.contains("version") || (int64)(j["version"]) != obj.version) {
            noNeedMerge = false;
        }
        if (noNeedMerge || std::invoke(std::forward<F>(fixer), obj, j)) {
            ll::reflection::deserialize(obj, j).value();
        }
    }

    template <class T, class J = nlohmann::ordered_json>
    static void json2struct_patch(J& json, T& obj) {
        auto  patch      = ll::reflection::serialize<J>(obj); // obj -> patch
        auto& patchValue = patch.value();
        patchValue.merge_patch(json); // patch -> json
        json2struct(patchValue, obj); // json -> obj
    }

    template <HasVersion T, class J>
    static bool _tryMergePatch(T& obj, J& data) {
        data.erase("version");
        auto patch = ll::reflection::serialize<J>(obj);
        patch.value().merge_patch(data);
        data = *std::move(patch);
        return true;
    }
};


} // namespace tps
