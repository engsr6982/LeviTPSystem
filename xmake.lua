add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

add_requires("levilamina 0.12.1") -- LeviLamina version x.x.x

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

package("PermissionCore")
    set_urls("https://github.com/engsr6982/PermissionCore/releases/download/$(version)/SDK-PermissionCore.zip")
    add_versions("v0.8.0", "7bedb1668a9610ce6626432f73b91026209eb254cebec2615b62e7a47c663d69")
    add_includedirs("include/")
    on_install(function (package)
        os.cp("*", package:installdir())
    end)

-- Key: PluginName, Value: Deps
local ProjectPlugins = {
    ["TPSystem"] = {"PermissionCore"},
    ["FakePlayer"] = {}
}
local mPluginDepsSocks = nil -- 因作用域问题，只能采用全局变量来传输插件依赖。

-- Auto generate plugin option.
option("plugin")
    local plugins = {}
    for k, _ in pairs(ProjectPlugins) do
        table.insert(plugins, k)
    end
    set_default(plugins[1])
    set_values(table.unpack(plugins))
    after_check(function (option)
        local mPlugin = get_config("plugin")
        if mPlugin == nil then
            return
        end
        local tryGetDeps = ProjectPlugins[mPlugin]
        if tryGetDeps == nil then
            printf("Failed to get plugin dependencies for: %s\n", mPlugin)
            return
        end
        if tryGetDeps == {} then
            return
        end
        mPluginDepsSocks = table.unpack(tryGetDeps)
    end)
-- Auto require plugin dependencies.
if mPluginDepsSocks ~= nil then
    printf("[Deps] Install plugin dependencies: %s\n", mPluginDepsSocks)
    add_requires(mPluginDepsSocks)
end 

target("LeviBoom")
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
    add_packages("levilamina")
    add_shflags("/DELAYLOAD:bedrock_server.dll") -- To use symbols provided by SymbolProvider.
    set_exceptions("none") -- To avoid conflicts with /EHa.
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    -- Auto configure plugins.
    for pluginName, deps in pairs(ProjectPlugins) do
        if is_config("plugin", pluginName) then
            add_includedirs("plugin") -- Global include directory for plugins.
            add_files("plugin/" .. pluginName .. "/**.cc") -- Add build files for plugin.
            add_defines("PLUGIN_NAME=\"" .. pluginName .. "\"") -- Add plugin name define.
            add_defines("LEVIBOOM_PLUGIN_" .. string.upper(pluginName)) -- Add plugin define.
            set_basename("LeviBoom_" .. pluginName .. (is_mode("debug") and "_Debug" or "")) -- Set output name.
            add_packages(table.unpack(deps)) -- Add plugin dependencies.
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
