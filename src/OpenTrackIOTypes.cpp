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

namespace opentrackio::opentrackiotypes
{
    std::optional<Rational> Rational::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        uint32_t num;
        uint32_t denom;
        if (!json.contains("num") || !json.contains("denom"))
        {
            errors.emplace_back("numerator or denominator field is missing.");
            return std::nullopt;
        }

        if (!OpenTrackIOHelpers::checkTypeAndSetField(json["num"], num) ||
            !OpenTrackIOHelpers::checkTypeAndSetField(json["denom"], denom))
        {
            errors.emplace_back("numerator or denominator fields aren't the correct type.");
            return std::nullopt;
        }

        return Rational(num, denom);
    }

    std::optional<Vector3> Vector3::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        Vector3 vec{};
        if (!json.contains("x") || !json.contains("y") || !json.contains("z"))
        {
            errors.emplace_back("field: Vector3 is missing required fields");
            return std::nullopt;
        }

        if (!OpenTrackIOHelpers::checkTypeAndSetField(json["x"], vec.x) ||
            !OpenTrackIOHelpers::checkTypeAndSetField(json["y"], vec.y) ||
            !OpenTrackIOHelpers::checkTypeAndSetField(json["z"], vec.z))
        {
            errors.emplace_back("field: Vector3 fields aren't of type double");
            return std::nullopt;
        }

        return vec;
    }

    std::optional<Rotation> Rotation::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        Rotation rot{};
        if (!json.contains("pan") || !json.contains("tilt") || !json.contains("roll"))
        {
            errors.emplace_back("field: Rotation is missing required fields");
            return std::nullopt;
        }

        if (!OpenTrackIOHelpers::checkTypeAndSetField(json["tilt"], rot.tilt) ||
            !OpenTrackIOHelpers::checkTypeAndSetField(json["pan"], rot.pan) ||
            !OpenTrackIOHelpers::checkTypeAndSetField(json["roll"], rot.roll))
        {
            errors.emplace_back("field: Rotation fields aren't of type double");
            return std::nullopt;
        }

        return rot;
    }

    std::optional<Timecode> Timecode::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        std::optional<uint8_t> hours = std::nullopt;
        std::optional<uint8_t> minutes = std::nullopt;
        std::optional<uint8_t> seconds = std::nullopt;
        std::optional<uint8_t> frames = std::nullopt;

        OpenTrackIOHelpers::assignField(json, "hours", hours, "uint8", errors);
        OpenTrackIOHelpers::assignField(json, "minutes", minutes, "uint8", errors);
        OpenTrackIOHelpers::assignField(json, "seconds", seconds, "uint8", errors);
        OpenTrackIOHelpers::assignField(json, "frames", frames, "uint8", errors);

        if (!hours.has_value() || !minutes.has_value() || !seconds.has_value() || !frames.has_value())
        {
            errors.emplace_back("field: timing/timecode is missing required fields");
            return std::nullopt;
        }

        const bool formatFieldValid =
                json["format"].contains("frameRate") &&
                json["format"].contains("dropFrame") &&
                json["format"]["frameRate"].contains("num") &&
                json["format"]["frameRate"].contains("denom");

        if (!formatFieldValid)
        {
            errors.emplace_back("field: timing/timecode/format is missing required fields");
            return std::nullopt;
        }
        
        auto fr = Rational::parse(json["format"]["frameRate"], errors);
        bool drop;
        std::optional<bool> odd;

        if (!OpenTrackIOHelpers::checkTypeAndSetField(json["format"]["dropFrame"], drop))
        {
            errors.emplace_back("field: timing/timecode/format/dropFrame isn't of type: bool");
            return std::nullopt;
        }

        if (!fr.has_value())
        {
            return std::nullopt;
        }
        
        OpenTrackIOHelpers::assignField(json["format"], "oddField", odd, "bool", errors);

        return Timecode{hours.value(), minutes.value(), seconds.value(), frames.value(), Format{fr.value(), drop, odd}};
    }

    std::optional<Timestamp> Timestamp::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        std::optional<uint64_t> seconds = std::nullopt;
        std::optional<uint32_t> nanoseconds = std::nullopt;
        std::optional<uint32_t> attoseconds = std::nullopt;

        OpenTrackIOHelpers::assignField(json, "seconds", seconds, "uint64", errors);
        OpenTrackIOHelpers::assignField(json, "nanoseconds", nanoseconds, "uint32_t", errors);
        OpenTrackIOHelpers::assignField(json, "attoseconds", attoseconds, "uint32_t", errors);

        if (!seconds.has_value() || !nanoseconds.has_value())
        {
            errors.emplace_back("field: timestamp is missing required fields");
            return std::nullopt;
        }

        return Timestamp(seconds.value(), nanoseconds.value(), attoseconds.value_or(0));
    }

    std::optional<Dimensions> Dimensions::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        std::optional<uint32_t> width = std::nullopt;
        std::optional<uint32_t> height = std::nullopt;

        OpenTrackIOHelpers::assignField(json, "width", width, "uint32_t", errors);
        OpenTrackIOHelpers::assignField(json, "height", height, "uint32_t", errors);

        if (!width.has_value() || !height.has_value())
        {
            errors.emplace_back("field: dimensions is missing required fields");
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
