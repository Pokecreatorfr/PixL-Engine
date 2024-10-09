add_rules("mode.debug", "mode.release")

set_languages("c++17")
set_optimize("fastest")

add_requires("libsdl")
add_requires("libsdl_image")
add_requires("boost")
add_requires("nlohmann_json")


target("PixL-Engine")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/**.cpp")
    add_packages("libsdl", "libsdl_image","boost","nlohmann_json")

    -- rules
    before_build(function(target)
        os.rm(target:targetdir() .. "/data")
    end)
    
    after_build(function(target)
        os.cp("data", target:targetdir())
    end)
    
