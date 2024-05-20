add_rules("mode.debug", "mode.release")

set_languages("c++17")
set_optimize("fastest")

add_requires("ffmpeg")
add_requires("libsdl")
add_requires("libsdl_image")
add_requires("python")
-- pip install pillow

before_build(function (target)
    os.exec("pip install pillow")
    os.exec("python include_ressources.py")
    os.exec("python convert_fonts.py")
    os.exec("python include_fonts.py")
    os.exec("python include_tilesets.py")
    os.exec("python include_maps.py")
end)


target("PixL-Engine")
    set_kind("binary")
    add_includedirs("include")
    add_files("asm/*.s")
    add_files("src/*.cpp")
    add_packages("ffmpeg","libsdl", "libsdl_image")
