#include <core/Core.hpp>

using namespace pixl::core;

const BuildInfo Core::buildInfo{};
bool Core::IsInitialized_ = false;

int Core::Init(const CoreInitData &initData)
{
    if (initData.enableConsole)
    {
        // write build info to console
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

    if (maths::Maths::Init() != 0)
    {
        Log::WriteLog(LOG_LEVEL_FATAL, "Failed to initialize Maths module.");
        return -1;
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
        Log::WriteLog(LOG_LEVEL_WARNING, "Failed to quit Maths module properly.");
    }

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