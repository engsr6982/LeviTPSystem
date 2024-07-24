add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

add_requires(
    "levilamina 0.13.4",
    "permissioncore 0.10.1",
    "legacymoney 0.8.2"
)


if not has_config("vs_runtime") then
    set_runtimes("MD")
end

package("permissioncore")
    set_urls("https://github.com/engsr6982/PermissionCore/releases/download/$(version)/SDK-PermissionCore.zip")
    add_versions("v0.10.1", "c626fa4ff421e5e3a2b84986f9135fa6e1961c65746ca7147601993629e379fc")
    add_includedirs("include/")
    on_install(function (package)
        os.cp("*", package:installdir())
    end)


target("LeviTPSystem")
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
    add_defines("NOMINMAX", "UNICODE", "_HAS_CXX23=1")
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

    add_defines("PLUGIN_NAME=\"LeviTPSystem\"")
    add_packages("permissioncore")
    add_packages("legacymoney")

    after_build(function (target)
        local plugin_packer = import("scripts.after_build")

        local tag = os.iorun("git describe --tags --abbrev=0 --always")
        local major, minor, patch, suffix = tag:match("v(%d+)%.(%d+)%.(%d+)(.*)")
        if not major then
            print("Failed to parse version tag, using 0.0.0")
            major, minor, patch = 0, 0, 0
        end
        local plugin_define = {
            pluginName = target:name(),
            pluginFile = path.filename(target:targetfile()),
            pluginVersion = major .. "." .. minor .. "." .. patch,
        }
        
        plugin_packer.pack_plugin(target,plugin_define)
    end)