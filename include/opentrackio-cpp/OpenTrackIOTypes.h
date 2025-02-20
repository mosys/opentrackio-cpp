/**
 * Copyright 2025 Mo-Sys Engineering Ltd
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
#include <format>
#include <nlohmann/json.hpp>
#include "opentrackio-cpp/OpenTrackIOHelper.h"

namespace opentrackio::opentrackiotypes
{
    struct Rational
    {
        uint32_t numerator = 0;
        uint32_t denominator = 0;

        Rational() = default;

        Rational(uint32_t n, uint32_t d) : numerator{n}, denominator{d}
        {};
        
        static std::optional<Rational> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
        {
            const auto& rationalJson = json[fieldStr];

            uint32_t num;
            uint32_t denom;
            if (!rationalJson.contains("num") || !rationalJson.contains("denom"))
            {
                errors.emplace_back(std::format("Key: {} is missing numerator or denominator field.", fieldStr));
                return std::nullopt;
            }

            if (!OpenTrackIOHelpers::checkTypeAndSetField(rationalJson["num"], num) ||
                !OpenTrackIOHelpers::checkTypeAndSetField(rationalJson["denom"], denom))
            {
                errors.emplace_back(std::format("Key: {} numerator or denominator field types are incorrect.", fieldStr));
                return std::nullopt;
            }

            return Rational(num, denom);
        }
    };

    struct Vector3
    {
        double x = 0;
        double y = 0;
        double z = 0;

        Vector3() = default;

        Vector3(double x, double y, double z) : x{x}, y{y}, z{z}
        {};

        static std::optional<Vector3> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
        {
            const auto& vecJson = json[fieldStr];

            Vector3 vec{};
            if (!vecJson.contains("x") || !vecJson.contains("y") || !vecJson.contains("z"))
            {
                errors.emplace_back(std::format("Key: {} Vector3 is missing required fields", fieldStr));
                return std::nullopt;
            }

            if (!OpenTrackIOHelpers::checkTypeAndSetField(vecJson["x"], vec.x) ||
                !OpenTrackIOHelpers::checkTypeAndSetField(vecJson["y"], vec.y) ||
                !OpenTrackIOHelpers::checkTypeAndSetField(vecJson["z"], vec.z))
            {
                errors.emplace_back(std::format("Key: {} Vector3 fields aren't of type double", fieldStr));
                return std::nullopt;
            }

            return vec;
        }
    };

    struct Rotation
    {
        double pan = 0;
        double tilt = 0;
        double roll = 0;

        Rotation() = default;

        Rotation(double p, double t, double r) : pan{p}, tilt{t}, roll{r}
        {};

        static std::optional<Rotation> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
        {
            const auto& rotJson = json[fieldStr];

            Rotation rot{};
            if (!rotJson.contains("pan") || !rotJson.contains("tilt") || !rotJson.contains("roll"))
            {
                errors.emplace_back(std::format("Key: {} Rotation is missing required fields", fieldStr));
                return std::nullopt;
            }

            if (!OpenTrackIOHelpers::checkTypeAndSetField(rotJson["tilt"], rot.tilt) ||
                !OpenTrackIOHelpers::checkTypeAndSetField(rotJson["pan"], rot.pan) ||
                !OpenTrackIOHelpers::checkTypeAndSetField(rotJson["roll"], rot.roll))
            {
                errors.emplace_back(std::format("Key: {} Rotation fields aren't of type double", fieldStr));
                return std::nullopt;
            }

            return rot;
        }
    };

    struct Timecode
    {
        uint8_t hours = 0;
        uint8_t minutes = 0;
        uint8_t seconds = 0;
        uint8_t frames = 0;
        Rational frameRate{};
        std::optional<uint32_t> subFrame = std::nullopt;
        
        Timecode() = default;
        Timecode(uint8_t h, uint8_t m, uint8_t s, uint8_t f, Rational fr, std::optional<uint32_t> sf = std::nullopt)
                : hours{h}, minutes{m}, seconds{s}, frames{f}, frameRate{fr}, subFrame{sf}
        {};

        static std::optional<Timecode> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
        {
            auto& tcJson = json[fieldStr];

            std::optional<uint8_t> hours = std::nullopt;
            std::optional<uint8_t> minutes = std::nullopt;
            std::optional<uint8_t> seconds = std::nullopt;
            std::optional<uint8_t> frames = std::nullopt;
            const std::optional<Rational> frameRate = Rational::parse(tcJson, "frameRate", errors);

            OpenTrackIOHelpers::assignField(tcJson, "hours", hours, "uint8", errors);
            OpenTrackIOHelpers::assignField(tcJson, "minutes", minutes, "uint8", errors);
            OpenTrackIOHelpers::assignField(tcJson, "seconds", seconds, "uint8", errors);
            OpenTrackIOHelpers::assignField(tcJson, "frames", frames, "uint8", errors);

            if (!hours.has_value() || !minutes.has_value() || !seconds.has_value() || !frames.has_value() || !frameRate.has_value())
            {
                errors.emplace_back("field: timing/timecode is missing required fields");
                return std::nullopt;
            }

            std::optional<int> subFrame;
            OpenTrackIOHelpers::assignField(tcJson["format"], "subFrame", subFrame, "uint32", errors);
            return Timecode{hours.value(), minutes.value(), seconds.value(), frames.value(), frameRate.value(), subFrame};
        }
    };

    struct Timestamp
    {
        uint64_t seconds = 0;
        uint32_t nanoseconds = 0;

        Timestamp() = default;

        Timestamp(uint64_t s, uint32_t n) : seconds{s}, nanoseconds{n} {};

        static std::optional<Timestamp> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
        {
            auto& tsJson = json[fieldStr];

            std::optional<uint64_t> seconds = std::nullopt;
            std::optional<uint32_t> nanoseconds = std::nullopt;

            OpenTrackIOHelpers::assignField(tsJson, "seconds", seconds, "uint64", errors);
            OpenTrackIOHelpers::assignField(tsJson, "nanoseconds", nanoseconds, "uint32", errors);

            if (!seconds.has_value() || !nanoseconds.has_value())
            {
                errors.emplace_back("field: timestamp is missing required fields");
                return std::nullopt;
            }

            return Timestamp(seconds.value(), nanoseconds.value());
        }
    };

    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    struct Dimensions
    {
        T width = 0;
        T height = 0;

        Dimensions() = default;

        Dimensions(T w, T h) : width{w}, height{h}
        {};

        static std::optional<Dimensions<T>> parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
        {
            auto& dimJson = json[fieldStr];

            std::optional<T> width = std::nullopt;
            std::optional<T> height = std::nullopt;

            OpenTrackIOHelpers::assignField(dimJson, "width", width, "double", errors);
            OpenTrackIOHelpers::assignField(dimJson, "height", height, "double", errors);

            if (!width.has_value() || !height.has_value())
            {
                errors.emplace_back(std::format("Key: {} dimensions is missing required fields", fieldStr));
                return std::nullopt;
            }

            return Dimensions(width.value(), height.value());
        }
    };

    struct Transform
    {
        Vector3 translation{};
        Rotation rotation{};
        std::optional<Vector3> scale = std::nullopt;
        std::optional<std::string> id = std::nullopt;

        Transform() = default;

        Transform(Vector3 trans, Rotation rot) : translation{trans}, rotation{rot} {};
        
        static std::optional<Transform> parse(nlohmann::json &json, std::vector<std::string> &errors)
        {
            Transform tf{};

            // Required Fields --------
            std::optional<Vector3> translation = std::nullopt;
            std::optional<Rotation> rotation = std::nullopt;

            if (!json.contains("translation") || !json.contains("rotation"))
            {
                return std::nullopt;
            }

            translation = Vector3::parse(json, "translation", errors);
            json.erase("translation");

            rotation = Rotation::parse(json, "rotation", errors);
            json.erase("rotation");
            
            if (!translation.has_value() || !rotation.has_value())
            {
                return std::nullopt;
            }
            
            tf.translation = translation.value();
            tf.rotation = rotation.value();

            // Non-required fields ------
            if (json.contains("scale"))
            {
                tf.scale = Vector3::parse(json, "scale", errors);
                json.erase("scale");
            }
            
            OpenTrackIOHelpers::assignField(json, "id", tf.id, "string", errors);

            return tf;
        }
    };
} // namespace opentrackio::opentrackiotypes
