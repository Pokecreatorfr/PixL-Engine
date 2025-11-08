#include <gfx/gfx_material.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

namespace pixl::gfx
{
    namespace
    {
        uint64_t hashShaderLayout(const ShaderLayout &layout)
        {
            constexpr uint64_t kMul = 0x9ddfea08eb382d69ull;
            auto hash64 = [&](uint64_t h, uint64_t value)
            {
                uint64_t a = (value ^ h) * kMul;
                a ^= (a >> 47);
                uint64_t b = (h ^ a) * kMul;
                b ^= (b >> 47);
                return b * kMul;
            };

            uint64_t hash = 0;
            for (const SetLayoutInfo &set : layout.sets)
            {
                hash = hash64(hash, set.set);
                hash = hash64(hash, static_cast<uint64_t>(set.bindings.size()));
                for (const BindingInfo &binding : set.bindings)
                {
                    hash = hash64(hash, binding.binding);
                    hash = hash64(hash, static_cast<uint64_t>(binding.count));
                    hash = hash64(hash, static_cast<uint64_t>(binding.stages));
                    hash = hash64(hash, static_cast<uint64_t>(binding.bindless));
                    hash = hash64(hash, static_cast<uint64_t>(binding.type));
                }
            }

            hash = hash64(hash, static_cast<uint64_t>(layout.pushConstants.size()));
            for (const PushConstantRangeInfo &range : layout.pushConstants)
            {
                hash = hash64(hash, static_cast<uint64_t>(range.stages));
                hash = hash64(hash, static_cast<uint64_t>(range.offset));
                hash = hash64(hash, static_cast<uint64_t>(range.size));
            }
            return hash;
        }

        ShaderLayout mergeShaderLayouts(const std::vector<const ShaderLayout *> &layouts)
        {
            ShaderLayout merged{};
            if (layouts.empty())
            {
                return merged;
            }

            std::map<DescSetIndex, std::map<BindingIndex, BindingInfo>> setBindings{};
            std::map<std::pair<uint32_t, uint32_t>, PushConstantRangeInfo> pushMap{};

            for (const ShaderLayout *layout : layouts)
            {
                if (!layout)
                {
                    continue;
                }
                for (const SetLayoutInfo &set : layout->sets)
                {
                    auto &bindings = setBindings[set.set];
                    for (const BindingInfo &binding : set.bindings)
                    {
                        auto [it, inserted] = bindings.try_emplace(binding.binding, binding);
                        if (!inserted)
                        {
                            BindingInfo &dst = it->second;
                            if (dst.type != binding.type || dst.count != binding.count)
                            {
                                throw std::runtime_error("Shader binding mismatch while merging layouts");
                            }
                            dst.stages |= binding.stages;
                            dst.bindless = dst.bindless || binding.bindless;
                        }
                    }
                }

                for (const PushConstantRangeInfo &range : layout->pushConstants)
                {
                    auto key = std::make_pair(range.offset, range.size);
                    auto [it, inserted] = pushMap.try_emplace(key, range);
                    if (!inserted)
                    {
                        it->second.stages |= range.stages;
                    }
                }
            }

            for (const auto &setPair : setBindings)
            {
                SetLayoutInfo setInfo{};
                setInfo.set = setPair.first;
                for (const auto &bindingPair : setPair.second)
                {
                    setInfo.bindings.push_back(bindingPair.second);
                }
                std::sort(setInfo.bindings.begin(), setInfo.bindings.end(),
                          [](const BindingInfo &a, const BindingInfo &b)
                          { return a.binding < b.binding; });
                merged.sets.push_back(std::move(setInfo));
            }

            for (const auto &rangePair : pushMap)
            {
                merged.pushConstants.push_back(rangePair.second);
            }

            merged.layoutHash = hashShaderLayout(merged);
            merged.spirvHash = merged.layoutHash;
            return merged;
        }
    }

    MaterialDescriptorTable::MaterialDescriptorTable(Device &device, PipelineLayout layout, bool transient)
        : m_device(device), m_layout(layout), m_transient(transient)
    {
        for (DescSetIndex set : {0u, 1u, 2u, 3u})
        {
            DescriptorSetLayout layoutHandle = m_device.getDescriptorSetLayout(layout, set);
            if (!layoutHandle.h)
            {
                continue;
            }
            DescriptorSet descriptorSet = m_device.allocateDescriptorSet(layoutHandle, transient);
            switch (set)
            {
            case 0:
                m_sets.global = descriptorSet;
                break;
            case 1:
                m_sets.material = descriptorSet;
                break;
            case 2:
                m_sets.instance = descriptorSet;
                break;
            case 3:
                m_sets.pass = descriptorSet;
                break;
            default:
                break;
            }
        }
    }

    MaterialDescriptorTable::~MaterialDescriptorTable()
    {
        release();
    }

    void MaterialDescriptorTable::release()
    {
        if (m_released)
        {
            return;
        }
        m_released = true;
        auto releaseSet = [&](DescriptorSet &set)
        {
            if (!set.h)
            {
                return;
            }
            m_device.free(set);
            set = {};
        };

        releaseSet(m_sets.global);
        releaseSet(m_sets.material);
        releaseSet(m_sets.instance);
        releaseSet(m_sets.pass);
    }

    void MaterialDescriptorTable::updateGlobal(std::span<const DescriptorWriteOperation> operations)
    {
        updateDescriptorSet(m_sets.global, operations);
    }

    void MaterialDescriptorTable::updateMaterial(std::span<const DescriptorWriteOperation> operations)
    {
        updateDescriptorSet(m_sets.material, operations);
    }

    void MaterialDescriptorTable::updateInstance(std::span<const DescriptorWriteOperation> operations)
    {
        updateDescriptorSet(m_sets.instance, operations);
    }

    void MaterialDescriptorTable::updatePass(std::span<const DescriptorWriteOperation> operations)
    {
        updateDescriptorSet(m_sets.pass, operations);
    }

    void MaterialDescriptorTable::updateDescriptorSet(const DescriptorSet &descriptorSet, std::span<const DescriptorWriteOperation> operations)
    {
        if (!descriptorSet.h || operations.empty())
        {
            return;
        }

        std::vector<DescriptorWriteOperation> filtered;
        filtered.reserve(operations.size());
        for (const auto &op : operations)
        {
            if (op.set.h == 0)
            {
                DescriptorWriteOperation copy = op;
                copy.set = descriptorSet;
                filtered.push_back(copy);
            }
            else
            {
                filtered.push_back(op);
            }
        }

        m_device.updateDescriptors(filtered);
    }

    MaterialPipelineLayoutCache::MaterialPipelineLayoutCache(Device &device)
        : m_device(device)
    {
    }

    MaterialPipelineLayoutCache::~MaterialPipelineLayoutCache()
    {
        clear();
    }

    void MaterialPipelineLayoutCache::clear()
    {
        for (auto &[hash, layout] : m_cache)
        {
            if (layout.h)
            {
                m_device.destroy(layout);
            }
        }
        m_cache.clear();
    }

    PipelineLayout MaterialPipelineLayoutCache::getOrCreate(std::span<const Shader> shaders)
    {
        if (shaders.empty())
        {
            throw std::runtime_error("MaterialPipelineLayoutCache requires at least one shader");
        }

        std::vector<const ShaderLayout *> layouts;
        layouts.reserve(shaders.size());
        for (const Shader &shader : shaders)
        {
            layouts.push_back(&m_device.getShaderLayout(shader));
        }

        ShaderLayout merged = mergeShaderLayouts(layouts);

        auto it = m_cache.find(merged.layoutHash);
        if (it != m_cache.end())
        {
            return it->second;
        }

        PipelineLayout layout = m_device.createPipelineLayout(merged);
        std::cout << "[MaterialPipelineLayoutCache] Created layout hash 0x" << std::hex << merged.layoutHash << std::dec << "\n";
        for (const SetLayoutInfo &set : merged.sets)
        {
            std::cout << "  Set " << set.set << " (" << set.bindings.size() << " bindings)\n";
            for (const BindingInfo &binding : set.bindings)
            {
                std::cout << "    Binding " << binding.binding << " type=" << static_cast<int>(binding.type)
                          << " count=" << binding.count << " stages=0x" << std::hex << binding.stages << std::dec
                          << (binding.bindless ? " bindless" : "") << "\n";
            }
        }
        if (!merged.pushConstants.empty())
        {
            std::cout << "  Push constants:\n";
            for (const PushConstantRangeInfo &range : merged.pushConstants)
            {
                std::cout << "    offset=" << range.offset << " size=" << range.size << " stages=0x" << std::hex << range.stages << std::dec << "\n";
            }
        }

        m_cache.emplace(merged.layoutHash, layout);
        return layout;
    }
}
