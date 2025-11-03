#pragma once
#include <core/IO/IO.hpp>
#include <core/maths/Maths.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace pixl::core::serialize
{

    using json = nlohmann::json;

    inline void to_json(json &j, const Vec2f &v) { j = {v.x, v.y}; }
    inline void from_json(const json &j, Vec2f &v) { v = {j.at(0).get<float>(), j.at(1).get<float>()}; }

    inline void to_json(json &j, const Vec3f &v) { j = {v.x, v.y, v.z}; }
    inline void from_json(const json &j, Vec3f &v) { v = {j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>()}; }

    inline void to_json(json &j, const Vec4f &v) { j = {v.x, v.y, v.z, v.w}; }
    inline void from_json(const json &j, Vec4f &v) { v = {j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>(), j.at(3).get<float>()}; }

    inline void to_json(json &j, const Mat3f &m)
    {
        j = json::array();
        const float *p = reinterpret_cast<const float *>(&m);
        for (int i = 0; i < 9; ++i)
            j.push_back(p[i]);
    }
    inline void from_json(const json &j, Mat3f &m)
    {
        float *p = reinterpret_cast<float *>(&m);
        for (int i = 0; i < 9; ++i)
            p[i] = j.at(i).get<float>();
    }

    inline void to_json(json &j, const Mat4f &m)
    {
        j = json::array();
        const float *p = reinterpret_cast<const float *>(&m);
        for (int i = 0; i < 16; ++i)
            j.push_back(p[i]);
    }

    inline void from_json(const json &j, Mat4f &m)
    {
        float *p = reinterpret_cast<float *>(&m);
        for (int i = 0; i < 16; ++i)
            p[i] = j.at(i).get<float>();
    }

    inline void to_json(json &j, const Quatf &q)
    {
        j = {q.x, q.y, q.z, q.w};
    }

    inline void from_json(const json &j, Quatf &q)
    {
        q = {j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>(), j.at(3).get<float>()};
    }

    template <class T>
    concept JsonSerializable = requires(const T &a, json &jw, const json &jr, T &b) {
        { to_json(jw, a) } -> std::same_as<void>;
        { from_json(jr, b) } -> std::same_as<void>;
    };

    template <JsonSerializable T>
    json to_json_value(const T &obj)
    {
        json j;
        to_json(j, obj);
        return j;
    }

    template <JsonSerializable T>
    std::optional<T> from_json_value(const json &j)
    {
        try
        {
            T v{};
            from_json(j, v);
            return v;
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    template <JsonSerializable T>
    std::optional<T> load_from_file(const std::string &filepath)
    {
        try
        {
            json j;

            std::vector<char> buf = core::io::IO::ReadFileToBuffer(filepath);

            T out{};
            from_json(j, out);
            return out;
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

}