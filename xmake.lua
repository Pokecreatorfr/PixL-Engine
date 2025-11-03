add_rules("mode.debug", "mode.release")

set_languages("c++20")
set_optimize("none")
set_defaultmode("debug")

if (is_mode("debug")) then
    add_defines("__BUILD_TYPE__=\"DEBUG\"")
    add_defines("__DEBUG_BUILD__")
else
    add_defines("__BUILD_TYPE__=\"RELEASE\"")
    add_defines("__RELEASE_BUILD__")
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
add_requires("fmt")


target("PixL-Engine")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})

    add_includedirs("include", "libs")
    add_files("src/**.cpp", "main.cpp")
    add_packages( "glm", "openmp", "fmt","nlohmann_json")

target("Test")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})

    add_includedirs("include", "tests" , "libs")
    add_files("tests/**.cpp", "src/**.cpp", "main_tests.cpp")
    add_packages("glm", "openmp" , "fmt" ,  "nlohmann_json")
    add_deps("PixL-Engine")

