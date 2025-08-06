add_rules("mode.debug", "mode.release")

set_languages("c++17")
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

target("Pak")
    set_kind("phony")
    after_build(function (target)
        local shader_dir = "./assets"
        local compiled_dir = "compiled_shaders"

        -- Créer le dossier de destination si nécessaire
        os.mkdir(path.join(shader_dir, "shaders"))

        -- Si le dossier n'est pas vide, le vider
        if os.isdir(path.join(shader_dir, "shaders")) then
            os.rm(path.join(shader_dir, "shaders", "*"))
        end

        -- Copier tous les shaders compilés dans le dossier de l'exécutable
        os.cp(path.join(compiled_dir, "*"), path.join(shader_dir, "shaders"))

        print("Shaders compilés copiés dans : " .. path.join(shader_dir, "shaders"))

        local pak_file = path.join(target:targetdir(), "assets.pak")


        os.run("./libs/PixL-Pak/build/linux/x86_64/release/PixL-Paker -p ./assets " .. pak_file)
    end)

target("convert_images")
    set_kind("phony")
    before_build(function ()
        import("lib.detect.find_tool")
        local ffmpeg = find_tool("ffmpeg")
        local convert = find_tool("convert")
        assert(ffmpeg or convert, "ERROR: neither ffmpeg nor ImageMagick 'convert' found in PATH")

        local files = os.match("./assets/images/**")
        if #files == 0 then
            print("No image files found in assets/images directory.")
        end
        for _, file in ipairs(files) do
           print("Processing file: " .. file)
            if file:lower():match("%.png$") or file:lower():match("%.jpg$") or file:lower():match("%.jpeg$") then
                local out = file:gsub("%.[^.]+$", ".bmp")
                if ffmpeg then
                    os.exec("%s -y -i %s %s", ffmpeg.program, file, out)
                else
                    os.exec("%s %s %s", convert.program, file, out)
                end
                print(string.format("Converted: %s -> %s", file, out))
            else 
                print(string.format("Skipping unsupported file: %s", file))
            end
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
    add_includedirs("libs/PixL-Pak/include")
    add_includedirs("include")
    add_files("src/**.cpp")
    add_packages("libsdl3", "libsdl3_image", "glm" , "imgui")

    -- Assurer que les shaders sont compilés avant l'application
    add_deps("shaders")
    add_deps("Pak")
    add_deps("convert_images")

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

        after_build(function (target)
        local shader_dir = "./assets"
        local compiled_dir = "compiled_shaders"

        -- Créer le dossier de destination si nécessaire
        os.mkdir(path.join(shader_dir, "shaders"))

        -- Si le dossier n'est pas vide, le vider
        if os.isdir(path.join(shader_dir, "shaders")) then
            os.rm(path.join(shader_dir, "shaders", "*"))
        end

        -- Copier tous les shaders compilés dans le dossier de l'exécutable
        os.cp(path.join(compiled_dir, "*"), path.join(shader_dir, "shaders"))

        print("Shaders compilés copiés dans : " .. path.join(shader_dir, "shaders"))

        local pak_file = path.join(target:targetdir(), "assets.pak")


        os.run("./libs/PixL-Pak/build/linux/x86_64/release/PixL-Paker -p ./assets " .. pak_file)
    end)
