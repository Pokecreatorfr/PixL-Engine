#pragma once

#include <gfx/gfx_api.hpp>
#include <span>
#include <unordered_map>
#include <vector>

namespace pixl::gfx
{
    struct MaterialDescriptorSets
    {
        DescriptorSet global{};
        DescriptorSet material{};
        DescriptorSet instance{};
        DescriptorSet pass{};
    };

    class MaterialDescriptorTable
    {
    public:
        MaterialDescriptorTable(Device &device, PipelineLayout layout, bool transient = true);
        ~MaterialDescriptorTable();

        const MaterialDescriptorSets &sets() const { return m_sets; }
        void release();
        PipelineLayout pipelineLayout() const { return m_layout; }

        void updateGlobal(std::span<const DescriptorWriteOperation> operations);
        void updateMaterial(std::span<const DescriptorWriteOperation> operations);
        void updateInstance(std::span<const DescriptorWriteOperation> operations);
        void updatePass(std::span<const DescriptorWriteOperation> operations);

    private:
        Device &m_device;
        PipelineLayout m_layout{};
        bool m_transient{true};
        MaterialDescriptorSets m_sets{};
        bool m_released{false};

        void updateDescriptorSet(const DescriptorSet &descriptorSet, std::span<const DescriptorWriteOperation> operations);
    };

    class MaterialPipelineLayoutCache
    {
    public:
        explicit MaterialPipelineLayoutCache(Device &device);
        ~MaterialPipelineLayoutCache();

        PipelineLayout getOrCreate(std::span<const Shader> shaders);
        void clear();

    private:
        Device &m_device;
        std::unordered_map<uint64_t, PipelineLayout> m_cache;
    };
}
