add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

-- add_requires("levilamina x.x.x") for a specific version
-- add_requires("levilamina develop") to use develop version
-- please note that you should add bdslibrary yourself if using dev version
add_requires(
    "levilamina 0.12.1",
    "PermissionCore"
)

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

option("plugin")
    set_default("tpsystem")
    set_values("tpsystem", "fakeplayer")

package("PermissionCore")
    set_urls("https://github.com/engsr6982/PermissionCore/releases/download/$(version)/SDK-PermissionCore.zip")
    add_versions("v0.8.0", "7bedb1668a9610ce6626432f73b91026209eb254cebec2615b62e7a47c663d69")
    add_includedirs("include/")
    on_install(function (package)
        os.cp("*", package:installdir())
    end)

target("LeviBoom") -- Change this to your plugin name.
    add_cxflags(
        "/EHa",
        "/utf-8",
        "/W4",
        "/w44265",
        "/w44289",
        "/w44296",
        "/w45263",
        "/w44738",
        "/w45204"
    )
    add_defines("NOMINMAX", "UNICODE")
    add_files("src/**.cpp")
    add_includedirs("src")
    add_packages("levilamina", "PermissionCore")
    add_shflags("/DELAYLOAD:bedrock_server.dll") -- To use symbols provided by SymbolProvider.
    set_exceptions("none") -- To avoid conflicts with /EHa.
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    local mapping = {
        ["tpsystem"] = "TPSystem",
        ["fakeplayer"] = "FakePlayer"
    }

    for k, v in pairs(mapping) do
        if is_config("plugin", k) then
            add_includedirs("plugin")
            -- add_includedirs("plugin/" .. k)
            add_files("plugin/" .. k .. "/**.cc")
            add_defines("PLUGIN_NAME=\"" .. k .. "\"")
            add_defines("LEVIBOOM_PLUGIN_" .. string.upper(v))
            set_basename("LeviBoom_" .. k .. (is_mode("debug") and "_Debug" or ""))
        end
    end

    after_build(function (target)
        local plugin_packer = import("scripts.after_build")

        local tag = os.iorun("git describe --tags --abbrev=0 --always")
        local major, minor, patch, suffix = tag:match("v(%d+)%.(%d+)%.(%d+)(.*)")
        if not major then
            print("Failed to parse version tag, using 0.0.0")
            major, minor, patch = 0, 0, 0
        end
        local plugin_define = {
            -- pluginName = target:name(),
            pluginName = target:basename(),
            pluginFile = path.filename(target:targetfile()),
            pluginVersion = major .. "." .. minor .. "." .. patch,
        }
        
        plugin_packer.pack_plugin(target,plugin_define)
    end)
