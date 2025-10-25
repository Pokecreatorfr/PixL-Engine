#include <maths/Maths.hpp>

using namespace pixl::maths;

bool Maths::IsInitialized_ = false;

int Maths::Init()
{
    if (IsInitialized_)
    {
        return 0;
    }
    IsInitialized_ = true;
    return 0;
}

int pixl::maths::Maths::Quit()
{
    if (!IsInitialized_)
    {
        return 0;
    }
    IsInitialized_ = false;
    return 0;
}