#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <variant>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <asset_loader/asset_loader.hpp>
#include <fastgltf/tools.hpp>

int loadImageFromFile(const char *filename, char *data, size_t dataSize, int &width, int &height)
{
    int n;
    unsigned char *img = stbi_load(filename, &width, &height, &n, 4);
    if (img == nullptr)
    {
        return -1; // Failed to load image
    }

    size_t requiredSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    if (dataSize < requiredSize)
    {
        stbi_image_free(img);
        return -2; // Provided buffer is too small
    }

    memcpy(data, img, requiredSize);
    stbi_image_free(img);
    return 0;
}

namespace
{
    bool extractBufferData(const fastgltf::DataSource &source, const fastgltf::Asset &asset, const std::filesystem::path &base_dir, std::vector<uint8_t> &out)
    {
        using namespace fastgltf;
        using namespace fastgltf::sources;

        out.clear();
        if (std::holds_alternative<std::monostate>(source))
            return false;

        if (const auto *bv = std::get_if<fastgltf::sources::BufferView>(&source))
        {
            if (bv->bufferViewIndex >= asset.bufferViews.size())
                return false;
            const auto &view = asset.bufferViews[bv->bufferViewIndex];
            if (view.bufferIndex >= asset.buffers.size())
                return false;
            const auto &buffer = asset.buffers[view.bufferIndex];

            std::vector<uint8_t> bufferData;
            if (!extractBufferData(buffer.data, asset, base_dir, bufferData))
                return false;
            const size_t start = view.byteOffset;
            const size_t end = view.byteOffset + view.byteLength;
            if (end > bufferData.size())
                return false;
            out.assign(bufferData.begin() + static_cast<std::ptrdiff_t>(start), bufferData.begin() + static_cast<std::ptrdiff_t>(end));
            return true;
        }
        if (const auto *arr = std::get_if<fastgltf::sources::Array>(&source))
        {
            out.reserve(arr->bytes.size());
            for (auto b : arr->bytes)
                out.push_back(static_cast<uint8_t>(b));
            return true;
        }
        if (const auto *vec = std::get_if<fastgltf::sources::Vector>(&source))
        {
            out.reserve(vec->bytes.size());
            for (auto b : vec->bytes)
                out.push_back(static_cast<uint8_t>(b));
            return true;
        }
        if (const auto *view = std::get_if<fastgltf::sources::ByteView>(&source))
        {
            out.reserve(view->bytes.size());
            for (auto b : view->bytes)
                out.push_back(static_cast<uint8_t>(b));
            return true;
        }
        if (const auto *uri = std::get_if<fastgltf::sources::URI>(&source))
        {
            if (uri->uri.isDataUri())
            {
                return false; // Data URIs not handled here
            }
            std::filesystem::path file_path = base_dir / uri->uri.fspath();
            std::ifstream file(file_path, std::ios::binary);
            if (!file.is_open())
                return false;
            file.seekg(0, std::ios::end);
            std::streamsize file_size = file.tellg();
            file.seekg(static_cast<std::streamoff>(uri->fileByteOffset), std::ios::beg);
            if (file_size < static_cast<std::streamsize>(uri->fileByteOffset))
                return false;
            const auto remaining = file_size - static_cast<std::streamsize>(uri->fileByteOffset);
            out.resize(static_cast<size_t>(remaining));
            if (!file.read(reinterpret_cast<char *>(out.data()), remaining))
                return false;
            return true;
        }
        // CustomBuffer or Fallback not supported
        return false;
    }
} // namespace

int loadGltfFromFile(const std::filesystem::path &path, std::vector<LoadedPrimitive> &primitives)
{
    primitives.clear();

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None)
    {
        auto err = data.error();
        std::cerr << "Failed to read glTF buffer at " << path << ": " << fastgltf::getErrorName(err) << " (" << static_cast<std::uint64_t>(err) << ")" << std::endl;
        return -1; // Failed to read file
    }

    fastgltf::Parser parser{};
    constexpr fastgltf::Options options = fastgltf::Options::LoadExternalBuffers |
                                          fastgltf::Options::LoadExternalImages |
                                          fastgltf::Options::GenerateMeshIndices;

    std::filesystem::path base_dir = path.parent_path();
    if (base_dir.empty())
    {
        std::error_code ec;
        base_dir = std::filesystem::current_path(ec);
    }

    auto assetResult = parser.loadGltf(data.get(), base_dir, options);
    if (assetResult.error() != fastgltf::Error::None)
    {
        auto err = assetResult.error();
        std::cerr << "Failed to load glTF (base=" << base_dir << "): " << fastgltf::getErrorName(err) << " (" << static_cast<std::uint64_t>(err) << ")" << std::endl;
        return -2; // Failed to parse glTF
    }

    fastgltf::Asset asset = std::move(assetResult.get());
    if (asset.meshes.empty())
    {
        return -3; // No mesh in file
    }

    struct TexData
    {
        std::vector<uint8_t> pixels;
        int w = 0;
        int h = 0;
    };
    std::unordered_map<size_t, TexData> tex_cache;

    for (const auto &mesh : asset.meshes)
    {
        for (const auto &primitive : mesh.primitives)
        {
            if (primitive.type != fastgltf::PrimitiveType::Triangles)
                continue;

            const auto positionIt = primitive.findAttribute("POSITION");
            if (positionIt == primitive.attributes.end())
                continue; // skip primitives without positions

            const fastgltf::Accessor &positionAccessor = asset.accessors[positionIt->accessorIndex];
            std::vector<fastgltf::math::fvec3> positions(positionAccessor.count);
            fastgltf::copyFromAccessor<fastgltf::math::fvec3>(asset, positionAccessor, positions.data());

            std::vector<fastgltf::math::fvec3> normals;
            const auto normalIt = primitive.findAttribute("NORMAL");
            if (normalIt != primitive.attributes.end())
            {
                const fastgltf::Accessor &normalAccessor = asset.accessors[normalIt->accessorIndex];
                normals.resize(normalAccessor.count);
                fastgltf::copyFromAccessor<fastgltf::math::fvec3>(asset, normalAccessor, normals.data());
            }

            std::vector<fastgltf::math::fvec2> uvs;
            const auto uvIt = primitive.findAttribute("TEXCOORD_0");
            if (uvIt != primitive.attributes.end())
            {
                const fastgltf::Accessor &uvAccessor = asset.accessors[uvIt->accessorIndex];
                uvs.resize(uvAccessor.count);
                fastgltf::copyFromAccessor<fastgltf::math::fvec2>(asset, uvAccessor, uvs.data());
            }

            std::vector<uint32_t> indices;
            if (primitive.indicesAccessor.has_value())
            {
                const fastgltf::Accessor &indexAccessor = asset.accessors[*primitive.indicesAccessor];
                switch (indexAccessor.componentType)
                {
                case fastgltf::ComponentType::UnsignedInt:
                {
                    indices.resize(indexAccessor.count);
                    fastgltf::copyFromAccessor<uint32_t>(asset, indexAccessor, indices.data());
                    break;
                }
                case fastgltf::ComponentType::UnsignedShort:
                {
                    std::vector<uint16_t> temp(indexAccessor.count);
                    fastgltf::copyFromAccessor<uint16_t>(asset, indexAccessor, temp.data());
                    indices.resize(temp.size());
                    std::transform(temp.begin(), temp.end(), indices.begin(), [](uint16_t v)
                                   { return static_cast<uint32_t>(v); });
                    break;
                }
                case fastgltf::ComponentType::UnsignedByte:
                {
                    std::vector<uint8_t> temp(indexAccessor.count);
                    fastgltf::copyFromAccessor<uint8_t>(asset, indexAccessor, temp.data());
                    indices.resize(temp.size());
                    std::transform(temp.begin(), temp.end(), indices.begin(), [](uint8_t v)
                                   { return static_cast<uint32_t>(v); });
                    break;
                }
                default:
                    continue; // unsupported index format
                }
            }
            else
            {
                indices.resize(positions.size());
                std::iota(indices.begin(), indices.end(), 0);
            }

            bool bad_index = false;
            for (uint32_t idx : indices)
            {
                if (idx >= positions.size())
                {
                    bad_index = true;
                    break;
                }
            }
            if (bad_index)
                continue;

            std::optional<size_t> textureIndex;
            if (primitive.materialIndex.has_value())
            {
                const auto &mat = asset.materials[*primitive.materialIndex];
                if (mat.pbrData.baseColorTexture.has_value())
                {
                    textureIndex = mat.pbrData.baseColorTexture->textureIndex;
                }
            }

            LoadedPrimitive part;
            part.vertices.resize(positions.size());
            for (size_t i = 0; i < positions.size(); ++i)
            {
                const auto &p = positions[i];
                const fastgltf::math::fvec3 n = (i < normals.size()) ? normals[i] : fastgltf::math::fvec3{0.0f, 0.0f, 1.0f};
                const fastgltf::math::fvec2 uv = (i < uvs.size()) ? uvs[i] : fastgltf::math::fvec2{0.0f, 0.0f};

                TexturedVertex v{};
                v.x = p[0];
                v.y = p[1];
                v.z = p[2];
                v.u = uv[0];
                v.v = uv[1];
                v.nx = n[0];
                v.ny = n[1];
                v.nz = n[2];
                v.r = 255;
                v.g = 255;
                v.b = 255;
                v.a = 255;
                part.vertices[i] = v;
            }
            part.indices = std::move(indices);
            if (!positions.empty())
            {
                glm::vec3 min_v(positions[0][0], positions[0][1], positions[0][2]);
                glm::vec3 max_v = min_v;
                for (const auto &p : positions)
                {
                    min_v = glm::min(min_v, glm::vec3(p[0], p[1], p[2]));
                    max_v = glm::max(max_v, glm::vec3(p[0], p[1], p[2]));
                }
                part.aabb_min = min_v;
                part.aabb_max = max_v;
            }

            if (textureIndex.has_value() && *textureIndex < asset.textures.size())
            {
                if (tex_cache.find(*textureIndex) == tex_cache.end())
                {
                    const auto &tex = asset.textures[*textureIndex];
                    if (tex.imageIndex.has_value() && *tex.imageIndex < asset.images.size())
                    {
                        const auto &img = asset.images[*tex.imageIndex];
                        std::vector<uint8_t> image_bytes;
                        if (extractBufferData(img.data, asset, base_dir, image_bytes))
                        {
                            int w = 0, h = 0, comp = 0;
                            unsigned char *decoded = stbi_load_from_memory(image_bytes.data(), static_cast<int>(image_bytes.size()), &w, &h, &comp, 4);
                            if (decoded != nullptr)
                            {
                                size_t buf_size = static_cast<size_t>(w) * static_cast<size_t>(h) * 4;
                                TexData t;
                                t.pixels.assign(decoded, decoded + buf_size);
                                t.w = w;
                                t.h = h;
                                tex_cache[*textureIndex] = std::move(t);
                                stbi_image_free(decoded);
                            }
                        }
                    }
                }
                auto it = tex_cache.find(*textureIndex);
                if (it != tex_cache.end())
                {
                    part.texture_pixels = it->second.pixels;
                    part.tex_w = it->second.w;
                    part.tex_h = it->second.h;
                    part.texture_id = 0; // will be uploaded later and assigned an id
                }
            }

            primitives.push_back(std::move(part));
        }
    }

    return primitives.empty() ? -4 : 0;
}
