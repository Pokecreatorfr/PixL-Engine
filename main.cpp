#include <core/Core.hpp>

int main(int argc, char **argv)
{
    pixl::core::Core::Core::Init(pixl::core::CoreInitData{});

    while (pixl::core::Core::runMainLoop())
    {
    }

    pixl::core::Core::Quit();

    return 0;
}