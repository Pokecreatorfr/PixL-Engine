add_rules("mode.debug", "mode.release")

set_languages("c++20")
set_optimize("fastest")
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
add_requires("glm")
add_requires("openmp")
add_requires("nlohmann_json")
add_requires("gtest")
add_requires("fmt")
add_requires("shaderc")
add_requires("imgui" , { version = "v1.92.0-docking" ,configs = { sdl3_gpu = true , sdl3 = true} })
add_requires("sol2")
add_requires("stb")


rule("compile.shaders")
    set_extensions(".vert.glsl", ".frag.glsl")
    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        local outdir = path.join(target:targetdir(), "shaders")
        batchcmds:mkdir(outdir)
        local basename = path.basename(sourcefile):gsub("%.glsl$", "")
        local outfile = path.join(outdir, basename .. ".spv")
        local glslc = os.getenv("GLSLC") or "glslc"
        local stage = "vert"
        if sourcefile:match("%.frag%.glsl$") then
            stage = "frag"
        end
        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.shader %s", sourcefile)
        batchcmds:vrunv(glslc, {"-std=450", "-O", "-fshader-stage=" .. stage, "-o", outfile, sourcefile})
    end)

target("PixL-Engine")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})

    add_includedirs("include", "libs")
    add_files("src/**.cpp", "main.cpp")
    add_files("resources/shaders/*.vert.glsl|*.frag.glsl", {rule = "compile.shaders"})
    add_files("resources/shaders/*.frag.glsl|*.vert.glsl", {rule = "compile.shaders"})
    add_packages( "glm", "openmp", "fmt","nlohmann_json", "libsdl3", "stb" )


target("Test")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})

    add_includedirs("include", "tests" , "libs")
    add_files("tests/**.cpp", "src/**.cpp", "main_tests.cpp")
    add_packages("gtest")
    add_packages( "glm", "openmp", "fmt","nlohmann_json", "libsdl3", "stb" )
    add_deps("PixL-Engine")
