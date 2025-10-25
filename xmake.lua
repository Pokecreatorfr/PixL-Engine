add_rules("mode.debug", "mode.release")

set_languages("c++17")
set_optimize("none")
set_defaultmode("debug")

if (is_mode("debug")) then
    add_defines("__BUILD_TYPE__=\"DEBUG\"")
else
    add_defines("__BUILD_TYPE__=\"RELEASE\"")
end


if(is_host("linux")) then
    add_requires("libsdl3", { system = true })
else
    add_requires("libsdl3", { system = false })
end
add_requires("libsdl3_image")
add_requires("glm")
add_requires("openmp")
add_requires("nlohmann_json")
add_requires("gtest")


target("PixL-Engine")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})

    add_includedirs("include")
    add_files("src/**.cpp", "main.cpp")
    add_packages( "glm", "openmp")

target("Test")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})

    add_includedirs("include", "tests")
    add_files("tests/**.cpp", "src/**.cpp", "main_tests.cpp")
    add_packages("glm", "openmp")
    add_deps("PixL-Engine")

