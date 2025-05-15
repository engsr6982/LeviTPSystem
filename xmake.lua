add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

if is_config("target_type", "server") then
    add_requires("levilamina 1.1.1", {configs = {target_type = "server"}})
else
    add_requires("levilamina 1.0.0-rc.3", {configs = {target_type = "client"}})
end
add_requires("levibuildscript")
add_requires("sqlitecpp 3.2.1")
add_requires("exprtk 0.0.3")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end


option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

option("test")
    set_default(false)
    set_showmenu(true)
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
    set_exceptions("none") -- To avoid conflicts with /EHa.
    add_includedirs(
        "src",
        "include"
    )
    add_files("src/**.cc")
    add_defines(
        "NOMINMAX",
        "UNICODE",
        "_HAS_CXX23=1",
        "TPS_EXPORTS"
    )
    add_packages(
        "levilamina", 
        "sqlitecpp",
        "exprtk"
    )

    if is_mode("debug") then
        add_defines("TPS_DEBUG"--[[ , "LL_I18N_COLLECT_STRINGS" ]])
    end

    if has_config("test") then
        add_defines("TPS_TEST")
        add_includedirs("test")
        add_files("test/**.cc")
    end

    add_defines("MOD_NAME=\"LeviTPSystem\"")

    on_load(function (target)
        -- 解析 git tag + commit hash
        local raw_version_str = try { function() 
            return os.iorunv("git", {"describe", "--tags", "--always"})
        end } or "v0.0.0-unknown"

        -- 解析版本号组件
        local major, minor, patch = raw_version_str:match("v(%d+)%.(%d+)%.(%d+)")
        major = major or "0"
        minor = minor or "0" 
        patch = patch or "0"

        local version_str = "v" .. major .. "." .. minor .. "." .. patch

        -- 添加版本相关宏定义
        target:add("defines", 
            "LEVITPSYSTEM_VERSION=\"" .. version_str .. "\"",
            "LEVITPSYSTEM_VERSION_MAJOR=" .. major,
            "LEVITPSYSTEM_VERSION_MINOR=" .. minor, 
            "LEVITPSYSTEM_VERSION_PATCH=" .. patch
        )
    end)
    after_build(function (target)
        -- local bindir = path.join(os.projectdir(), "bin")
        -- local outputdir = path.join(bindir, target:name())
        -- -- copy data files
        -- local datadir = path.join(os.projectdir(), "assets", "data")
        -- os.cp(datadir, outputdir)
    end)