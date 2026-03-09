add_rules("mode.debug", "mode.release")

set_languages("c17")
set_optimize("fastest")
set_defaultmode("release")

includes("libs/VirtuaPPU")

add_requires("libsdl3")
add_requires("openmp")

target("pixl_engine_core")
    set_kind("static")
    if is_plat("windows") then
        set_plat("mingw")
        set_toolchains("mingw")
    end
    add_includedirs("include", {public = true})
    add_files("src/engine/*.c")
    add_cflags("-include", "include/engine/engine_no_alloc.h", {force = true})

    after_build(function (target)
        import("lib.detect.find_tool")

        local objdump = find_tool("objdump")
        if not objdump then
            raise("objdump introuvable dans le toolchain")
        end

        local STATIC_BUDGET = 3682304
        local total = 0

        for _, sourcefile in ipairs(target:sourcefiles()) do
            local objectfile = target:objectfile(sourcefile)
            if os.isfile(objectfile) then
                local out = os.iorunv(objdump.program, {"-h", objectfile})

                for line in out:gmatch("[^\r\n]+") do
                    local sec, sizehex = line:match("^%s*%d+%s+([%._%$%a%d]+)%s+([0-9A-Fa-f]+)")
                    if sec and sizehex then
                        if sec == ".data" or sec == ".bss" then
                            total = total + tonumber(sizehex, 16)
                        end
                    end
                end
            end
        end

        cprint("${bright cyan}PixL static RAM = %d bytes${clear}/%d (%.2f%%)", total,STATIC_BUDGET, (total / STATIC_BUDGET) * 100)

        if total > STATIC_BUDGET then
            raise("Budget RAM statique depasse : %d > %d bytes", total, STATIC_BUDGET)
        end
    end)

target("PixL-Engine")
    set_kind("binary")
    add_packages("libsdl3", "openmp")
    if is_plat("windows") then
        set_plat("mingw")
        set_toolchains("mingw")
    end
    add_includedirs("include")
    add_files("src/*.c")
    remove_files("src/engine/*.c")
    add_deps("pixl_engine_core", "VirtuaPPU")
    add_cflags("-include", "include/engine/engine_no_alloc.h", {force = true})
    add_ldflags("-Wl,--stack,524288")
