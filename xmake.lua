add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

if is_config("target_type", "server") then
    add_requires("levilamina 1.3.1", {configs = {target_type = "server"}})
else
    add_requires("levilamina 1.0.0-rc.3", {configs = {target_type = "client"}})
end
add_requires("bedrockdata v1.21.80-server.5")
add_requires("prelink v0.5.0")
add_requires("levibuildscript 0.4.0")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

target("LeviTPSystem")
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
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
    set_exceptions("none") -- To avoid conflicts with /EHa.
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")
    add_packages("bedrockdata")

    if is_mode("debug") then
        add_defines("DEBUG", "LL_I18N_COLLECT_STRINGS")
    end

    add_defines("PLUGIN_NAME=\"LeviTPSystem\"")
    after_build(function (target)
        local bindir = path.join(os.projectdir(), "bin")
        local outputdir = path.join(bindir, target:name())
        -- copy data files
        local datadir = path.join(os.projectdir(), "assets", "data")
        os.cp(datadir, outputdir)
    end)