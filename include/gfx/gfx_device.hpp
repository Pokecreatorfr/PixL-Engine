#pragma once
#include <cstdint>
#include <gfx/gfx_caps.hpp>
#include <gfx/gfx_features.hpp>
#include <gfx/gfx_types.hpp>

namespace pixl::gfx::device
{

  enum class API : uint8_t
  {
    VULKAN,
    DIRECTX12,
    METAL,
    OPENGL,
    OPENGL_ES
  };
  enum class DeviceType : uint8_t
  {
    UNKNOWN,
    INTEGRATED_GPU,
    DISCRETE_GPU,
    VIRTUAL_GPU,
    CPU
  };

  struct DeviceInfo
  {
    API api{API::VULKAN};
    DeviceType type{DeviceType::UNKNOWN};
    uint32_t apiVersionMajor{0}, apiVersionMinor{0};
    uint32_t vendorId{0}, deviceId{0};
    char name[256]{};
    uint64_t vramBytes{0};
    char driver[128]{};

    Features supportedFeatures{};
    Limits limits{};
    QueueCaps queues{};
    PresentCaps present{};
    TextureFormat preferredColorFormat{TextureFormat::B8G8R8A8_UNORM_SRGB};
    TextureFormat preferredDepthFormat{TextureFormat::D32_FLOAT};
  };

  struct RequestQueues
  {
    uint32_t graphics{1};
    uint32_t compute{0};
    uint32_t transfer{0};
  };

  struct DeviceCreateInfo
  {
    void *windowHandle{nullptr};
    RequestQueues queues{};
    Features requestedFeatures{};
    TextureFormat preferredColorFormat{TextureFormat::B8G8R8A8_UNORM_SRGB};
    TextureFormat preferredDepthFormat{TextureFormat::D32_FLOAT};
    bool enableValidation{true};
    bool headless{false};
  };

}
