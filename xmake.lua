add_rules("mode.debug", "mode.release")

set_languages("c++20")
set_optimize("none")
set_defaultmode("debug")


if(is_host("linux")) then
    add_requires("libsdl3", { system = true })
else
    add_requires("libsdl3", { system = false })
end
add_requires("libsdl3_image")
add_requires("glm")
add_requires("imgui" , { version = "v1.92.0-docking" ,configs = { sdl3_gpu = true , sdl3 = true} })
add_requires("openmp")
add_requires("nlohmann_json")
add_requires("stb")
add_requires("fastgltf")



target("shaders")
    set_kind("phony")
    before_build(function ()
        local vertShaders = os.files("shaders/*.vert")
        local fragShaders = os.files("shaders/*.frag")
        local shader_files = table.join(vertShaders, fragShaders)

        local compiled_dir = "build/shaders"
        os.mkdir(compiled_dir)

        for _, shader in ipairs(shader_files) do
            local output = path.join(compiled_dir, path.filename(shader) .. ".spv")
            print("Compilation de " .. shader .. " -> " .. output)
            os.run("glslc %s -o %s", shader, output)
        end
    end)



target("PixL-Engine")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})


    add_includedirs("include")
    add_files("src/**.cpp")
    add_packages("libsdl3", "glm" , "imgui", "stb" , "fastgltf")

    add_deps("shaders")
    
target("PixL-Engine-example")
    set_kind("binary")

    add_cflags("-fopenmp", {force = true})
    add_cxxflags("-fopenmp", {force = true})
    add_ldflags("-fopenmp", {force = true})


    add_includedirs("include", "example_ressources/ressource_code/include")
    add_files("src/**.cpp" , "example_ressources/ressource_code/src/**.cpp")
    add_packages("libsdl3", "glm" , "imgui", "stb" , "fastgltf")

    add_deps("shaders")