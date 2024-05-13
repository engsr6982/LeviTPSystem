add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

add_requires("levilamina 0.12.1") -- LeviLamina version x.x.x

-- Key: PluginName, Value: [[Deps], [Define]]
local ProjectPlugins = {
    ["TPSystem"] = {{"PermissionCore", "legacymoney 0.7.0"}, {"ENABLE_MONEY", "ENABLE_PERMISSIONCORE"}},
    ["FakePlayer"] = {{}, {}}
}

-- Auto require plugin dependencies.
if get_config("plugin") ~= nil then
    printf("[Deps] Require dependencies for plugin: '%s', deps: \n\t%s\n\n", get_config("plugin"), table.concat(ProjectPlugins[get_config("plugin")][1], "\n\t"))
    add_requires(unpack(ProjectPlugins[get_config("plugin")][1]))
end 

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

-- Auto generate plugin option.
option("plugin")
    local plugins = {}
    for k, _ in pairs(ProjectPlugins) do
        table.insert(plugins, k)
    end
    set_default(plugins[1])
    set_values(unpack(plugins))


-- Build target.
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
    add_files("src/**.cpp", "src/**.cc")
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
    if get_config("plugin") ~= nil then
        add_includedirs("plugin") -- Global include directory for plugins.
        add_files("plugin/" .. get_config("plugin") .. "/**.cc") -- Add build files for plugin.
        add_defines("PLUGIN_NAME=\"Levi" .. get_config("plugin") .. "\"") -- Add plugin name define.
        add_defines("LEVIBOOM_PLUGIN_" .. string.upper(get_config("plugin"))) -- Add plugin define.
        set_basename("Levi" .. get_config("plugin") .. (is_mode("debug") and "_Debug" or "")) -- Set output name.
        add_defines(unpack(ProjectPlugins[get_config("plugin")][2])) -- Add plugin defines.
        -- Parse plugin dependencies and add them as packages.
        local packages = {}
        for _, dep in ipairs(ProjectPlugins[get_config("plugin")][1]) do
            local dep_name = dep:match("^([^%s]+)")
            if not dep_name then
                error("Invalid dependency name: " .. dep)
            end
            table.insert(packages, dep_name)
        end
        add_packages(unpack(packages))
        -- printf("[Packages] Added packages for plugin: '%s', packages: \n\t%s\n\n", get_config("plugin"), table.concat(packages, "\n\t"))
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
