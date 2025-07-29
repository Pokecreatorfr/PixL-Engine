add_rules("mode.debug", "mode.release")

set_languages("c++23")
set_optimize("fastest")
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


target("shaders")
    set_kind("phony")
    before_build(function ()
        -- Récupérer tous les shaders dans le dossier "shaders"
        local vertShaders = os.files("shaders/*.vert")
        local fragShaders = os.files("shaders/*.frag")
        local shader_files = table.join(vertShaders, fragShaders)

        -- Créer le dossier des shaders compilés
        local compiled_dir = "compiled_shaders"
        os.mkdir(compiled_dir)

        -- Compiler chaque shader en SPIR-V
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

    on_load(function (target)
        if is_plat("linux") and is_arch("x86_64") then
            -- active le code optimisé pour Zen2 (Ryzen 4000) et SIMD
            target:add("cxflags", "-march=znver2", "-mavx2", "-mfma", "-msse4.2")
            -- ou simplement -march=native pour détecter automatiquement
            -- target:add("cxflags", "-march=native")
        end
    end)
    add_includedirs("libs/PixL-Rendering-Engine/include")
    add_files("libs/PixL-Rendering-Engine/src/**.cpp")
    add_includedirs("include")
    add_files("src/**.cpp")
    add_packages("libsdl3", "libsdl3_image", "glm" , "imgui")

    -- Assurer que les shaders sont compilés avant l'application
    add_deps("shaders")

    -- Copier les shaders compilés dans le dossier de l'exécutable après compilation
    after_build(function (target)
        local exec_dir = target:targetdir() -- Récupérer le dossier où est placé l'exécutable
        local compiled_dir = "compiled_shaders"

        -- Créer le dossier de destination si nécessaire
        os.mkdir(path.join(exec_dir, "shaders"))

        -- Copier tous les shaders compilés dans le dossier de l'exécutable
        os.cp(path.join(compiled_dir, "*"), path.join(exec_dir, "shaders"))

        print("Shaders compilés copiés dans : " .. path.join(exec_dir, "shaders"))
    end)
