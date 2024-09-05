add_rules("mode.debug", "mode.release")

set_languages("c++17")
set_optimize("fastest")

add_requires("libsdl")
add_requires("libsdl_image")


target("PixL-Engine")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/*.cpp")
    add_packages("libsdl", "libsdl_image")
    -- if mode is debug
    if is_mode("debug") then
        add_defines("DEBUG")
    end
