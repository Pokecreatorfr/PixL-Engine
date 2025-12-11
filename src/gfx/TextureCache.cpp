#include <core/Errors.hpp>
#include <core/IO/IO.hpp>
#include <gfx/TextureCache.hpp>

using namespace pixl::gfx;

TextureCache::TextureCache(Gpu *gpu)
    : state_(std::make_shared<State>(gpu))
{
}

TextureHandle TextureCache::CreateWhite()
{
    if (!state_ || !state_->gpu)
    {
        return InvalidHandle;
    }

    unsigned char pixel[4] = {255, 255, 255, 255};
    TextureDesc desc{};
    desc.width = 1;
    desc.height = 1;
    desc.format = PixelFormat::RGBA8;
    desc.renderTarget = false;
    return state_->gpu->CreateTexture(desc, pixel, sizeof(pixel));
}

TexturePtr TextureCache::MakeManagedWhite(TextureHandle handle)
{
    if (!state_ || handle == InvalidHandle)
    {
        return nullptr;
    }

    auto state = state_;
    TexturePtr ptr(new TextureRef(handle), [state](TextureRef *ref)
                   {
        if (state->gpu && ref->handle != InvalidHandle)
        {
            state->gpu->DestroyTexture(ref->handle);
        }

        if (state->white.lock().get() == ref)
        {
            state->white.reset();
        }

        delete ref; });

    state_->white = ptr;
    return ptr;
}

TexturePtr TextureCache::MakeManagedTexture(TextureHandle handle, const std::string &path)
{
    if (!state_ || handle == InvalidHandle)
    {
        return nullptr;
    }

    auto state = state_;
    TexturePtr ptr(new TextureRef(handle), [state, path](TextureRef *ref)
                   {
        if (state->gpu && ref->handle != InvalidHandle)
        {
            state->gpu->DestroyTexture(ref->handle);
        }

        auto it = state->cache.find(path);
        if (it != state->cache.end())
        {
            auto locked = it->second.lock();
            if (!locked || locked.get() == ref)
            {
                state->cache.erase(it);
            }
        }

        delete ref; });

    state_->cache[path] = ptr;
    return ptr;
}

TexturePtr TextureCache::GetWhiteTexture()
{
    if (!state_)
    {
        return nullptr;
    }

    if (auto existing = state_->white.lock())
    {
        return existing;
    }

    TextureHandle handle = CreateWhite();
    if (handle == InvalidHandle)
    {
        return nullptr;
    }

    return MakeManagedWhite(handle);
}

TexturePtr TextureCache::GetTexture(const std::string &path)
{
    if (!state_)
    {
        return nullptr;
    }

    if (path.empty())
    {
        return GetWhiteTexture();
    }

    auto it = state_->cache.find(path);
    if (it != state_->cache.end())
    {
        if (auto existing = it->second.lock())
        {
            return existing;
        }
    }

    std::vector<char> data = pixl::core::io::IO::ReadFileToBuffer(path);
    if (data.empty())
    {
        PIXL_LOG_ERROR(pixl::core::Errc::FileNotFound, "TextureCache", "Texture not found: {}", path);
        return GetWhiteTexture();
    }

    LoadedImage img{};
    if (!ImageLoader::LoadFromMemory(data, img, true))
    {
        PIXL_LOG_ERROR(pixl::core::Errc::UnknownError, "TextureCache", "Failed to decode image: {}", path);
        return GetWhiteTexture();
    }

    TextureDesc desc{};
    desc.width = static_cast<uint32_t>(img.width);
    desc.height = static_cast<uint32_t>(img.height);
    desc.format = PixelFormat::RGBA8;
    desc.renderTarget = false;
    desc.minFilter = TextureFilter::Linear;
    desc.magFilter = TextureFilter::Linear;
    desc.wrapU = TextureWrap::ClampToEdge;
    desc.wrapV = TextureWrap::ClampToEdge;

    TextureHandle handle = state_->gpu ? state_->gpu->CreateTexture(desc, img.pixels.data(), img.pixels.size()) : InvalidHandle;
    if (handle == InvalidHandle)
    {
        return GetWhiteTexture();
    }

    return MakeManagedTexture(handle, path);
}

void TextureCache::Clear()
{
    if (!state_)
    {
        return;
    }

    std::vector<TexturePtr> liveTextures;
    liveTextures.reserve(state_->cache.size() + 1);

    for (auto &kv : state_->cache)
    {
        if (auto tex = kv.second.lock())
        {
            liveTextures.push_back(std::move(tex));
        }
    }

    if (auto white = state_->white.lock())
    {
        liveTextures.push_back(std::move(white));
    }

    state_->cache.clear();
    state_->white.reset();
}
