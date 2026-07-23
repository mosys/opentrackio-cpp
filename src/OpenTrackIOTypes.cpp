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

#include "opentrackio-cpp/OpenTrackIOHelper.h"
#include "opentrackio-cpp/OpenTrackIOTypes.h"

namespace opentrackio::opentrackiotypes 
{
    std::optional<Rational> Rational::parse(nlohmann::json& json,
                                             std::string_view fieldStr,
                                             std::vector<std::string>& errors)
    {
        const auto& rationalJson = json[fieldStr];

        uint32_t num;
        uint32_t denom;
        if (!rationalJson.contains("num") || !rationalJson.contains("denom"))
        {
            errors.emplace_back(std::format("Key: {} is missing numerator or denominator field.", fieldStr));
            return std::nullopt;
        }

        if (!rationalJson.at("num").is_number_unsigned() || !rationalJson.at("denom").is_number_unsigned())
        {
            errors.emplace_back(std::format("Key: {} numerator or denominator field isn't of type: unsigned integer", fieldStr));
            return std::nullopt;
        }

        OpenTrackIOHelpers::getFieldFromJson(rationalJson["num"], num);
        OpenTrackIOHelpers::getFieldFromJson(rationalJson["denom"], denom);

        return Rational(num, denom);
    }

    std::optional<Vector3> Vector3::parse(nlohmann::json& json,
                                        std::string_view fieldStr,
                                        std::vector<std::string>& errors)
    {
        const auto& vecJson = json[fieldStr];

        Vector3 vec{};
        if (!vecJson.contains("x") || !vecJson.contains("y") || !vecJson.contains("z"))
        {
            errors.emplace_back(std::format("Key: {} Vector3 is missing required fields", fieldStr));
            return std::nullopt;
        }

        if (!vecJson.at("x").is_number() ||
            !vecJson.at("y").is_number() ||
            !vecJson.at("z").is_number())
        {
            errors.emplace_back(std::format("Key: {} Vector3 fields aren't of type: double", fieldStr));
            return std::nullopt;
        }

        OpenTrackIOHelpers::getFieldFromJson(vecJson["x"],vec.x);
        OpenTrackIOHelpers::getFieldFromJson(vecJson["y"], vec.y);
        OpenTrackIOHelpers::getFieldFromJson(vecJson["z"], vec.z);

        return vec;
    }

    std::optional<Rotation> Rotation::parse(nlohmann::json& json,
                                            std::string_view fieldStr,
                                            std::vector<std::string>& errors)
    {
        const auto& rotJson = json[fieldStr];

        std::optional<Rotation> rot = Rotation{};
        if (!rotJson.contains("pan") || !rotJson.contains("tilt") || !rotJson.contains("roll"))
        {
            errors.emplace_back(std::format("Key: {} Rotation is missing required fields", fieldStr));
            return std::nullopt;
        }

        if (!rotJson.at("pan").is_number() ||
            !rotJson.at("tilt").is_number() ||
            !rotJson.at("roll").is_number())
        {
            errors.emplace_back(std::format("Key: {} Rotation fields aren't of type: double", fieldStr));
            return std::nullopt;
        }

        OpenTrackIOHelpers::getFieldFromJson(rotJson["tilt"], rot->tilt);
        OpenTrackIOHelpers::getFieldFromJson(rotJson["pan"], rot->pan);
        OpenTrackIOHelpers::getFieldFromJson(rotJson["roll"], rot->roll);

        return rot;
    }

    std::optional<Timecode> Timecode::parse(nlohmann::json& json,
                                            std::string_view fieldStr,
                                            std::vector<std::string>& errors)
    {
        auto& tcJson = json[fieldStr];

        std::optional<uint8_t> hours = std::nullopt;
        std::optional<uint8_t> minutes = std::nullopt;
        std::optional<uint8_t> seconds = std::nullopt;
        std::optional<uint8_t> frames = std::nullopt;
        const std::optional<Rational> frameRate = Rational::parse(tcJson, "frameRate", errors);

        OpenTrackIOHelpers::assignField(tcJson, "hours", hours,  errors);
        OpenTrackIOHelpers::assignField(tcJson, "minutes", minutes,  errors);
        OpenTrackIOHelpers::assignField(tcJson, "seconds", seconds,  errors);
        OpenTrackIOHelpers::assignField(tcJson, "frames", frames,  errors);

        if (!hours.has_value() || !minutes.has_value() || !seconds.has_value() || !frames.has_value() || !frameRate.
            has_value())
        {
            errors.emplace_back("field: timing/timecode is missing required fields");
            return std::nullopt;
        }

        std::optional<uint32_t> subFrame;
        OpenTrackIOHelpers::assignField(tcJson, "subFrame", subFrame,  errors);

        std::optional<bool> dropFrame;
        OpenTrackIOHelpers::assignField(tcJson, "dropFrame", dropFrame,  errors);

        return Timecode{
            hours.value(),
            minutes.value(),
            seconds.value(),
            frames.value(),
            frameRate.value(),
            subFrame,
            dropFrame
        };
    }

    std::optional<Timestamp> Timestamp::parse(nlohmann::json& json,
                                            std::string_view fieldStr,
                                            std::vector<std::string>& errors)
    {
        auto& tsJson = json[fieldStr];

        std::optional<uint64_t> seconds = std::nullopt;
        std::optional<uint32_t> nanoseconds = std::nullopt;

        OpenTrackIOHelpers::assignField(tsJson, "seconds", seconds, errors);
        OpenTrackIOHelpers::assignField(tsJson, "nanoseconds", nanoseconds, errors);

        if (!seconds.has_value() || !nanoseconds.has_value())
        {
            errors.emplace_back("field: timestamp is missing required fields");
            return std::nullopt;
        }

        return Timestamp(seconds.value(), nanoseconds.value());
    }

    template<>
    std::optional<Dimensions<unsigned int> > Dimensions<unsigned int>::parse(nlohmann::json& json,
        std::string_view fieldStr,
        std::vector<std::string>& errors)
    {
        auto& dimJson = json[fieldStr];

        std::optional<unsigned int> width = std::nullopt;
        std::optional<unsigned int> height = std::nullopt;

        OpenTrackIOHelpers::assignField(dimJson, "width", width,  errors);
        OpenTrackIOHelpers::assignField(dimJson, "height", height,  errors);

        if (!width.has_value() || !height.has_value())
        {
            errors.emplace_back(std::format("Key: {} dimensions is missing required fields", fieldStr));
            return std::nullopt;
        }

        return Dimensions<unsigned int>(width.value(), height.value());
    }

    template<>
    std::optional<Dimensions<double> > Dimensions<double>::parse(nlohmann::json& json,
        std::string_view fieldStr,
        std::vector<std::string>& errors)
    {
        auto& dimJson = json[fieldStr];

        std::optional<double> width = std::nullopt;
        std::optional<double> height = std::nullopt;

        OpenTrackIOHelpers::assignField(dimJson, "width", width, errors);
        OpenTrackIOHelpers::assignField(dimJson, "height", height, errors);

        if (!width.has_value() || !height.has_value())
        {
            errors.emplace_back(std::format("Key: {} dimensions is missing required fields", fieldStr));
            return std::nullopt;
        }

        return Dimensions<double>(width.value(), height.value());
    }

    std::optional<Transform> Transform::parse(nlohmann::json& json, std::vector<std::string>& errors)
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

        OpenTrackIOHelpers::assignField(json, "id", tf.id, errors);

        return tf;
    }
}
