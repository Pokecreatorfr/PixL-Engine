#pragma once
#include <core/ECS.hpp>
#include <core/Log.hpp>
#include <core/maths/Maths.hpp>
#include <cstdio>

namespace pixl::core
{
    struct CoreInitData
    {
        const char *appName = "PixL Engine";
        const char *ressourcePath = "./resources/";
        const char *cachePath = "./cache/";
        bool enableConsole = true;
        bool logToFile = true;
    };

    struct BuildInfo
    {
        const char *buildDate = __DATE__;
        const char *buildTime = __TIME__;
        const char *compiler = __VERSION__;
        const char *buildType = __BUILD_TYPE__;
    };

    class Core
    {
    public:
        static int Init(const CoreInitData &initData);
        static int Quit();
        static const BuildInfo GetBuildInfo();

        static int runMainLoop();

    private:
        static bool IsInitialized_;
        static const BuildInfo buildInfo;
    };
}