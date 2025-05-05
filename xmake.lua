add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

if is_config("target_type", "server") then
    add_requires("levilamina 1.1.1", {configs = {target_type = "server"}})
else
    add_requires("levilamina 1.0.0-rc.3", {configs = {target_type = "client"}})
end
add_requires("levibuildscript")
add_requires("sqlitecpp 3.2.1")


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
    set_kind("shared")
    set_symbols("debug")
    set_languages("c++20")
    add_includedirs("src")
    add_defines("NOMINMAX", "UNICODE")
    add_files("src/**.cc")
    add_packages("levilamina", "sqlitecpp")
    set_exceptions("none") -- To avoid conflicts with /EHa.

    if is_mode("debug") then
        add_defines("DEBUG", "LL_I18N_COLLECT_STRINGS")
    end

    add_defines("PLUGIN_NAME=\"LeviTPSystem\"")
    after_build(function (target)
        -- local bindir = path.join(os.projectdir(), "bin")
        -- local outputdir = path.join(bindir, target:name())
        -- -- copy data files
        -- local datadir = path.join(os.projectdir(), "assets", "data")
        -- os.cp(datadir, outputdir)
    end)