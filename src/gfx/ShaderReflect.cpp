#include <gfx/gfx_reflect.hpp>

#include <algorithm>
#include <array>
#include <functional>
#include <spirv_reflect.h>
#include <stdexcept>
#include <vector>

namespace pixl::gfx
{
    namespace
    {
        ShaderStageMask toStageMask(SpvReflectShaderStageFlagBits flags)
        {
            ShaderStageMask mask = 0;
            auto accumulate = [&](SpvReflectShaderStageFlagBits bit, ShaderStage stage)
            {
                if (flags & bit)
                {
                    mask |= to_u(stage);
                }
            };

            accumulate(SPV_REFLECT_SHADER_STAGE_VERTEX_BIT, ShaderStage::VERTEX);
            accumulate(SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT, ShaderStage::FRAGMENT);
            accumulate(SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT, ShaderStage::COMPUTE);
            accumulate(SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT, ShaderStage::GEOMETRY);
            accumulate(SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT, ShaderStage::TESS_CONTROL);
            accumulate(SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, ShaderStage::TESS_EVAL);
            accumulate(SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV, ShaderStage::TASK);
            accumulate(SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV, ShaderStage::MESH);
            accumulate(SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR, ShaderStage::RAYGEN);
            accumulate(SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR, ShaderStage::MISS);
            accumulate(SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, ShaderStage::CLOSEST_HIT);
            accumulate(SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR, ShaderStage::ANY_HIT);
            accumulate(SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR, ShaderStage::INTERSECTION);
            accumulate(SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR, ShaderStage::CALLABLE);
            return mask;
        }

        DescType toDescType(SpvReflectDescriptorType type)
        {
            switch (type)
            {
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                return DescType::Sampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                return DescType::CombinedImageSampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                return DescType::SampledImage;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                return DescType::StorageImage;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                return DescType::TexelBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                return DescType::StorageTexelBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                return DescType::UniformBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                return DescType::StorageBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
                return DescType::AccelStructureKHR;
            default:
                throw std::runtime_error("Unsupported descriptor type in reflection");
            }
        }

        uint64_t hash64(uint64_t h, uint64_t value)
        {
            constexpr uint64_t kMul = 0x9ddfea08eb382d69ull;
            uint64_t a = (value ^ h) * kMul;
            a ^= (a >> 47);
            uint64_t b = (h ^ a) * kMul;
            b ^= (b >> 47);
            return b * kMul;
        }

        uint64_t computeLayoutHash(const ShaderLayout &layout)
        {
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

        uint32_t descriptorCount(const SpvReflectDescriptorBinding &binding)
        {
            if (binding.count > 0)
            {
                return binding.count;
            }
            uint32_t total = 1;
            for (uint32_t i = 0; i < binding.array.dims_count; ++i)
            {
                total *= binding.array.dims[i] == 0 ? 1 : binding.array.dims[i];
            }
            return total;
        }
        uint64_t computeSpirvHash(std::span<const uint32_t> words)
        {
            uint64_t hash = 0xcbf29ce484222325ull;
            constexpr uint64_t prime = 0x100000001b3ull;
            const uint8_t *bytes = reinterpret_cast<const uint8_t *>(words.data());
            size_t byteCount = words.size_bytes();
            for (size_t i = 0; i < byteCount; ++i)
            {
                hash ^= bytes[i];
                hash *= prime;
            }
            return hash;
        }
    }

    ShaderLayout reflectShaderLayout(std::span<const uint32_t> spirvWords)
    {
        if (spirvWords.empty())
        {
            throw std::invalid_argument("reflectShaderLayout: SPIR-V blob is empty");
        }

        SpvReflectShaderModule module{};
        SpvReflectResult result = spvReflectCreateShaderModule(spirvWords.size_bytes(), spirvWords.data(), &module);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            throw std::runtime_error("Failed to create SPIRV-Reflect module");
        }

        ShaderLayout layout{};

        uint32_t setCount = 0;
        result = spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            spvReflectDestroyShaderModule(&module);
            throw std::runtime_error("Failed to enumerate descriptor sets");
        }

        std::vector<SpvReflectDescriptorSet *> sets(setCount);
        spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data());

        layout.sets.reserve(setCount);
        for (SpvReflectDescriptorSet *set : sets)
        {
            if (!set)
            {
                continue;
            }
            SetLayoutInfo setInfo{};
            setInfo.set = set->set;
            setInfo.bindings.reserve(set->binding_count);

            for (uint32_t i = 0; i < set->binding_count; ++i)
            {
                const SpvReflectDescriptorBinding *binding = set->bindings[i];
                if (!binding)
                {
                    continue;
                }

                BindingInfo bindingInfo{};
                bindingInfo.binding = binding->binding;
                bindingInfo.type = toDescType(binding->descriptor_type);
                bindingInfo.count = descriptorCount(*binding);
                bindingInfo.stages = toStageMask(module.shader_stage);
                bindingInfo.bindless = binding->count == 0;

                setInfo.bindings.push_back(bindingInfo);
            }

            std::sort(setInfo.bindings.begin(), setInfo.bindings.end(),
                      [](const BindingInfo &a, const BindingInfo &b)
                      { return a.binding < b.binding; });

            layout.sets.push_back(std::move(setInfo));
        }

        std::sort(layout.sets.begin(), layout.sets.end(),
                  [](const SetLayoutInfo &a, const SetLayoutInfo &b)
                  { return a.set < b.set; });

        uint32_t pushCount = 0;
        result = spvReflectEnumeratePushConstantBlocks(&module, &pushCount, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            spvReflectDestroyShaderModule(&module);
            throw std::runtime_error("Failed to enumerate push constants");
        }

        std::vector<SpvReflectBlockVariable *> pushConstants(pushCount);
        spvReflectEnumeratePushConstantBlocks(&module, &pushCount, pushConstants.data());
        layout.pushConstants.reserve(pushCount);

        for (SpvReflectBlockVariable *block : pushConstants)
        {
            if (!block)
            {
                continue;
            }
            PushConstantRangeInfo range{};
            range.offset = block->offset;
            range.size = block->size;
            range.stages = toStageMask(module.shader_stage);
            layout.pushConstants.push_back(range);
        }

        std::sort(layout.pushConstants.begin(), layout.pushConstants.end(),
                  [](const PushConstantRangeInfo &a, const PushConstantRangeInfo &b)
                  { return (a.offset == b.offset) ? (a.size < b.size) : (a.offset < b.offset); });

        layout.layoutHash = computeLayoutHash(layout);
        layout.spirvHash = computeSpirvHash(spirvWords);

        spvReflectDestroyShaderModule(&module);
        return layout;
    }
}
