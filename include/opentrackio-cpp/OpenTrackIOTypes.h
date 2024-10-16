/**
 * Copyright 2024 Mo-Sys Engineering Ltd
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), 
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace opentrackio::opentrackiotypes
{
    struct Rational
    {
        int64_t numerator = 0;
        int64_t denominator = 0;

        Rational() = default;

        Rational(int64_t n, int64_t d) : numerator{n}, denominator{d}
        {};
        
        static std::optional<Rational> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors);
    };

    struct Vector3
    {
        double x = 0;
        double y = 0;
        double z = 0;

        Vector3() = default;

        Vector3(double x, double y, double z) : x{x}, y{y}, z{z}
        {};

        static std::optional<Vector3> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors);
    };

    struct Rotation
    {
        double pan = 0;
        double tilt = 0;
        double roll = 0;

        Rotation() = default;

        Rotation(double p, double t, double r) : pan{p}, tilt{t}, roll{r}
        {};

        static std::optional<Rotation> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors);
    };

    struct Timecode
    {
        uint8_t hours = 0;
        uint8_t minutes = 0;
        uint8_t seconds = 0;
        uint8_t frames = 0;
        
        struct Format
        {
            Rational frameRate{};
            bool dropFrame = false;
            std::optional<bool> oddField = std::nullopt;            
        };
        Format format{};
        
        Timecode() = default;

        Timecode(uint8_t h, uint8_t m, uint8_t s, uint8_t f, Format fmt)
                : hours{h}, minutes{m}, seconds{s}, frames{f}, format{fmt} {};

        static std::optional<Timecode> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors);
    };

    struct Timestamp
    {
        uint64_t seconds = 0;
        uint32_t nanoseconds = 0;
        uint32_t attoseconds = 0;

        Timestamp() = default;

        Timestamp(uint64_t s, uint32_t n, uint32_t a) : seconds{s}, nanoseconds{n}, attoseconds{a}
        {};

        static std::optional<Timestamp> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors);
    };

    struct Dimensions
    {
        uint32_t width = 0;
        uint32_t height = 0;

        Dimensions() = default;

        Dimensions(uint32_t w, uint32_t h) : width{w}, height{h}
        {};

        static std::optional<Dimensions> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors);
    };

    struct Transform
    {
        Vector3 translation{};
        Rotation rotation{};
        std::optional<Vector3> scale = std::nullopt;
        std::optional<std::string> transformId = std::nullopt;
        std::optional<std::string> parentTransformId = std::nullopt;

        Transform() = default;

        Transform(Vector3 trans, Rotation rot) : translation{trans}, rotation{rot} {};
        
        static std::optional<Transform> parse(nlohmann::json &json, std::vector<std::string> &errors);
    };
} // namespace opentrackio::opentrackiotypes
