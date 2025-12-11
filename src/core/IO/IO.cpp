#include <core/Errors.hpp>
#include <core/IO/IO.hpp>

using namespace pixl::core::io;

bool IO::IsInitialized_ = false;

int IO::Init()
{
    if (IsInitialized_)
    {
        return 0;
    }

    if (Pak::Init() != 0)
    {
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UnknownError, "IO", "Failed to initialize Pak subsystem");
    }

    IsInitialized_ = true;
    return 0;
}

int IO::Quit()
{
    if (!IsInitialized_)
    {
        return 0;
    }

    Pak::Quit();
    IsInitialized_ = false;
    return 0;
}

std::vector<char> IO::ReadFileToBuffer(const std::string &filepath)
{
    std::vector<char> buffer;

    if (!IsInitialized_)
    {
        PIXL_LOG_AND_RETURN_ERROR(buffer, pixl::core::Errc::UninitialisedSubsystem, "IO", "IO subsystem is not initialized");
    }

    // if the filename starts with "pak://" we read from a pak file
    if (filepath.rfind("pak://", 0) == 0)
    {
        size_t firstSlash = filepath.find('/', 6);
        if (firstSlash == std::string::npos)
        {
            PIXL_LOG_AND_RETURN_ERROR(buffer, pixl::core::Errc::InvalidParameter, "IO", "Invalid pak file path: %s", filepath.c_str());
        }
        std::string pakFilePath = filepath.substr(6, firstSlash - 6);
        std::string internalFilePath = filepath.substr(firstSlash + 1);
        buffer = Pak::ReadFileFromPak(pakFilePath, internalFilePath);
        if (buffer.empty())
        {
            PIXL_LOG_AND_RETURN_ERROR(buffer, pixl::core::Errc::FileNotFound, "IO", "File not found in pak: %s", filepath.c_str());
        }

        return buffer;
    }

    FILE *file = fopen(filepath.c_str(), "rb");
    if (!file)
    {
        if (Pak::TryRead(filepath, buffer))
        {
            return buffer;
        }
        PIXL_LOG_AND_RETURN_ERROR(buffer, pixl::core::Errc::FileNotFound, "IO", "File not found: %s", filepath.c_str());
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer.resize(fileSize);
    size_t bytesRead = fread(buffer.data(), 1, fileSize, file);
    fclose(file);

    if (bytesRead != fileSize)
    {
        PIXL_LOG_AND_RETURN_ERROR(buffer, pixl::core::Errc::UnknownError, "IO", "Failed to read entire file: %s", filepath.c_str());
    }

    return buffer;
}

bool IO::WriteBufferToFile(const std::string &filepath, const std::vector<char> &buffer)
{
    if (!IsInitialized_)
    {
        PIXL_LOG_AND_RETURN_ERROR(false, pixl::core::Errc::UninitialisedSubsystem, "IO", "IO subsystem is not initialized");
    }

    // if the filename starts with "pak://" we cannot write to a pak file
    if (filepath.rfind("pak://", 0) == 0)
    {
        PIXL_LOG_AND_RETURN_ERROR(false, pixl::core::Errc::InvalidParameter, "IO", "Cannot write to pak file: %s", filepath.c_str());
    }

    FILE *file = fopen(filepath.c_str(), "wb");
    if (!file)
    {
        PIXL_LOG_AND_RETURN_ERROR(false, pixl::core::Errc::UnknownError, "IO", "Failed to open file for writing: %s", filepath.c_str());
    }

    size_t bytesWritten = fwrite(buffer.data(), 1, buffer.size(), file);
    fclose(file);

    return bytesWritten == buffer.size();
}

bool IO::FileExists(const std::string &filepath)
{
    if (!IsInitialized_)
    {
        PIXL_LOG_AND_RETURN_ERROR(false, pixl::core::Errc::UninitialisedSubsystem, "IO", "IO subsystem is not initialized");
    }

    // if the filename starts with "pak://" we check in the pak file
    if (filepath.rfind("pak://", 0) == 0)
    {
        size_t firstSlash = filepath.find('/', 6);
        if (firstSlash == std::string::npos)
        {
            PIXL_LOG_AND_RETURN_ERROR(false, pixl::core::Errc::InvalidParameter, "IO", "Invalid pak file path: %s", filepath.c_str());
        }
        std::string pakFilePath = filepath.substr(6, firstSlash - 6);
        std::string internalFilePath = filepath.substr(firstSlash + 1);
        return Pak::IsPathInPak(pakFilePath, internalFilePath);
    }

    FILE *file = fopen(filepath.c_str(), "rb");
    if (file)
    {
        fclose(file);
        return true;
    }

    return Pak::HasFile(filepath);
}
