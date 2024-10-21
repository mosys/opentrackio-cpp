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

#include "opentrackio-cpp/OpenTrackIOTypes.h"
#include "opentrackio-cpp/OpenTrackIOHelper.h"
#include <format>

namespace opentrackio::opentrackiotypes
{
    std::optional<Rational> Rational::parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
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

    std::optional<Vector3> Vector3::parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
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

    std::optional<Rotation> Rotation::parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
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

    std::optional<Timecode> Timecode::parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
    {
        auto& tcJson = json[fieldStr];
        
        std::optional<uint8_t> hours = std::nullopt;
        std::optional<uint8_t> minutes = std::nullopt;
        std::optional<uint8_t> seconds = std::nullopt;
        std::optional<uint8_t> frames = std::nullopt;

        OpenTrackIOHelpers::assignField(tcJson, "hours", hours, "uint8", errors);
        OpenTrackIOHelpers::assignField(tcJson, "minutes", minutes, "uint8", errors);
        OpenTrackIOHelpers::assignField(tcJson, "seconds", seconds, "uint8", errors);
        OpenTrackIOHelpers::assignField(tcJson, "frames", frames, "uint8", errors);

        if (!hours.has_value() || !minutes.has_value() || !seconds.has_value() || !frames.has_value())
        {
            errors.emplace_back("field: timing/timecode is missing required fields");
            return std::nullopt;
        }

        const bool formatFieldValid =
                tcJson["format"].contains("frameRate") &&
                tcJson["format"].contains("dropFrame") &&
                tcJson["format"]["frameRate"].contains("num") &&
                tcJson["format"]["frameRate"].contains("denom");

        if (!formatFieldValid)
        {
            errors.emplace_back("field: timing/timecode/format is missing required fields");
            return std::nullopt;
        }
        
        auto fr = Rational::parse(tcJson["format"], "frameRate", errors);
        bool drop;
        std::optional<bool> odd;

        if (!OpenTrackIOHelpers::checkTypeAndSetField(tcJson["format"]["dropFrame"], drop))
        {
            errors.emplace_back("field: timing/timecode/format/dropFrame isn't of type: bool");
            return std::nullopt;
        }

        if (!fr.has_value())
        {
            return std::nullopt;
        }
        
        OpenTrackIOHelpers::assignField(tcJson["format"], "oddField", odd, "bool", errors);

        return Timecode{hours.value(), minutes.value(), seconds.value(), frames.value(), Format{fr.value(), drop, odd}};
    }

    std::optional<Timestamp> Timestamp::parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
    {
        auto& tsJson = json[fieldStr];
        
        std::optional<uint64_t> seconds = std::nullopt;
        std::optional<uint32_t> nanoseconds = std::nullopt;
        std::optional<uint32_t> attoseconds = std::nullopt;

        OpenTrackIOHelpers::assignField(tsJson, "seconds", seconds, "uint64", errors);
        OpenTrackIOHelpers::assignField(tsJson, "nanoseconds", nanoseconds, "uint32_t", errors);
        OpenTrackIOHelpers::assignField(tsJson, "attoseconds", attoseconds, "uint32_t", errors);

        if (!seconds.has_value() || !nanoseconds.has_value())
        {
            errors.emplace_back("field: timestamp is missing required fields");
            return std::nullopt;
        }

        return Timestamp(seconds.value(), nanoseconds.value(), attoseconds.value_or(0));
    }

    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    std::optional<Dimensions<T>> Dimensions<T>::parse(nlohmann::json &json, std::string_view fieldStr, std::vector<std::string> &errors)
    {
        auto& dimJson = json[fieldStr];
        
        std::optional<T> width = std::nullopt;
        std::optional<T> height = std::nullopt;

        OpenTrackIOHelpers::assignField(dimJson, "width", width, "number", errors);
        OpenTrackIOHelpers::assignField(dimJson, "height", height, "number", errors);

        if (!width.has_value() || !height.has_value())
        {
            errors.emplace_back(std::format("Key: {} dimensions is missing required fields", fieldStr));
            return std::nullopt;
        }

        return Dimensions(width.value(), height.value());
    }

    std::optional<Transform> Transform::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        Transform tf{};

        // Required Fields --------
        std::optional<Vector3> translation = std::nullopt;
        std::optional<Rotation> rotation = std::nullopt;
        
        OpenTrackIOHelpers::assignField(json, "translation", translation, "float", errors);
        OpenTrackIOHelpers::assignField(json, "rotation", rotation, "float", errors);

        if (!translation.has_value() || !rotation.has_value())
        {
            return std::nullopt;
        }

        tf.translation = translation.value();
        tf.rotation = rotation.value();

        // Non-required fields ------
        OpenTrackIOHelpers::assignField(json, "scale", tf.scale, "float", errors);
        OpenTrackIOHelpers::assignField(json, "transformId", tf.transformId, "string", errors);
        OpenTrackIOHelpers::assignField(json, "parentTransformId", tf.parentTransformId, "string", errors);

        return tf;
    }
} // namespace opentrackio::opentrackiotypes
