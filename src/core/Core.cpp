#include <core/Core.hpp>
#include <core/Errors.hpp>
#include <core/IO/IO.hpp>
#include <core/IO/Pak.hpp>
#include <core/Resources.hpp>
#include <filesystem>

using namespace pixl::core;

const BuildInfo Core::buildInfo{};
bool Core::IsInitialized_ = false;

int Core::Init(const CoreInitData &initData)
{
    if (initData.enableConsole)
    {
        const BuildInfo &info = GetBuildInfo();
        printf("Initializing %s\n", initData.appName);
        printf("Build Date: %s\n", info.buildDate);
        printf("Build Time: %s\n", info.buildTime);
        printf("Compiler: %s\n", info.compiler);
        printf("Build Type: %s\n", info.buildType);
    }

    Log::Init(initData.enableConsole ? LogLevel::LOG_LEVEL_DEBUG : LogLevel::LOG_LEVEL_ERROR,
              initData.enableConsole,
              initData.logToFile);

    Errors::Init();

    if (maths::Maths::Init() != 0)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, Errc::UnknownError, "Core", "Failed to initialize Maths module");
    }

    if (io::IO::Init() != 0)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, Errc::UnknownError, "Core", "Failed to initialize IO subsystem");
    }

    if (initData.autoMountResourcePak && initData.ressourcePath)
    {
        std::filesystem::path resPath = std::filesystem::path(initData.ressourcePath);
        std::filesystem::path pakCandidate = resPath / "resources.pak";
        std::error_code ec;
        if (std::filesystem::exists(pakCandidate, ec))
        {
            io::Pak::MountPakFile(pakCandidate.string());
        }
    }

    for (const auto &pak : initData.pakFiles)
    {
        io::Pak::MountPakFile(pak);
    }

    if (initData.manifestFile && std::strlen(initData.manifestFile) > 0)
    {
        std::filesystem::path manifestPath = std::filesystem::path(initData.ressourcePath) / initData.manifestFile;
        std::error_code ec;
        if (std::filesystem::exists(manifestPath, ec))
        {
            Resources::LoadManifest(manifestPath.string());
        }
        else if (std::filesystem::exists(initData.manifestFile, ec))
        {
            Resources::LoadManifest(initData.manifestFile);
        }
    }

    IsInitialized_ = true;

    return 0;
}

int pixl::core::Core::Quit()
{
    if (!IsInitialized_)
    {
        return 0;
    }

    Log::WriteLog(LOG_LEVEL_INFO, "Quitting PixL Engine...");

    if (maths::Maths::Quit() != 0)
    {
        PIXL_LOG_ERROR(Errc::UnknownError, "Core", "Failed to quit Maths module properly");
    }

    io::IO::Quit();
    Log::Quit();
    IsInitialized_ = false;
    return 0;
}
const BuildInfo Core::GetBuildInfo()
{
    return buildInfo;
}

int pixl::core::Core::runMainLoop()
{
    return 0;
}
