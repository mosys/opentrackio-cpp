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

#include "opentrackio-cpp/OpenTrackIOProperties.h"
#include <regex>
#include "opentrackio-cpp/OpenTrackIOHelper.h"

namespace opentrackio::opentrackioproperties
{
    std::optional<Camera> Camera::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("static") || !json["static"].contains("camera"))
        {
            return std::nullopt;
        }

        if (!json["static"]["camera"].is_object())
        {
            errors.emplace_back("field: camera isn't of type: object");
            return std::nullopt;
        }
        
        Camera cam{};
        auto& cameraJson = json["static"]["camera"];
        
        if (cameraJson.contains("activeSensorPhysicalDimensions"))
        {
            cam.activeSensorPhysicalDimensions = opentrackiotypes::Dimensions<double>::parse(
                    cameraJson, "activeSensorPhysicalDimensions", errors);
            cameraJson.erase("activeSensorPhysicalDimensions");
        }

        if (cameraJson.contains("activeSensorResolution"))
        {
            cam.activeSensorResolution = opentrackiotypes::Dimensions<uint32_t>::parse(cameraJson,
                   "activeSensorResolution", errors);
            cameraJson.erase("activeSensorResolution");
        }

        if (cameraJson.contains("anamorphicSqueeze"))
        {
            cam.anamorphicSqueeze = opentrackiotypes::Rational::parse(cameraJson, "anamorphicSqueeze", errors);
            cameraJson.erase("anamorphicSqueeze");
        }
        
        OpenTrackIOHelpers::assignField(cameraJson, "firmwareVersion", cam.firmwareVersion, "string", errors);
        OpenTrackIOHelpers::assignField(cameraJson, "label", cam.label, "string", errors);
        OpenTrackIOHelpers::assignField(cameraJson, "make", cam.make, "string", errors);
        OpenTrackIOHelpers::assignField(cameraJson, "model", cam.model, "string", errors);
        OpenTrackIOHelpers::assignField(cameraJson, "serialNumber", cam.serialNumber, "string", errors);

        if (cameraJson.contains("captureFrameRate"))
        {
            cam.captureFrameRate = opentrackiotypes::Rational::parse(cameraJson, "captureFrameRate", errors);
            cameraJson.erase("captureFrameRate");
        }
        
        const std::regex pattern{R"(^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$)"};
        OpenTrackIOHelpers::assignRegexField(cameraJson, "fdlLink", cam.fdlLink, pattern, errors);
        
        OpenTrackIOHelpers::assignField(cameraJson, "isoSpeed", cam.isoSpeed, "integer", errors);
        OpenTrackIOHelpers::assignField(cameraJson, "shutterAngle", cam.shutterAngle, "double", errors);
        
        if (cam.shutterAngle.has_value() && cam.shutterAngle.value() > 360)
        {
            errors.emplace_back("field: shutterAngle is outside the expected range 1 - 360.");
            cam.shutterAngle = std::nullopt;            
        }

        
        OpenTrackIOHelpers::clearFieldIfEmpty(json["static"], "camera");
        return cam;
    }

    std::optional<Duration> Duration::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("static") || !json["static"].contains("duration"))
        {
            return std::nullopt;    
        }

        if (!json["static"]["duration"].is_object())
        {
            errors.emplace_back("field: duration isn't of type: object");
            return std::nullopt;
        }
        
        auto& durationJson = json["static"]["duration"];
        std::optional<uint32_t> numerator = std::nullopt;
        std::optional<uint32_t> denominator = std::nullopt;
        
        OpenTrackIOHelpers::assignField(durationJson, "num", numerator, "uint32", errors);
        OpenTrackIOHelpers::assignField(durationJson, "denom", denominator, "uint32", errors);
        
        if (!numerator.has_value() || !denominator.has_value())
        {
            errors.emplace_back("field: duration is missing required fields");
            return std::nullopt;
        }

        OpenTrackIOHelpers::clearFieldIfEmpty(json["static"], "duration");
        return Duration{{numerator.value(), denominator.value()}};
    }

    std::optional<GlobalStage> GlobalStage::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("globalStage"))
        {
            return std::nullopt;
        }

        if (!json["globalStage"].is_object())
        {
            errors.emplace_back("field: globalStage isn't of type: object");
            return std::nullopt;
        }

        GlobalStage gs{};
        const auto& gsJson = json["globalStage"];
        
        const auto fieldCheckAndAssign = [&](std::string_view fieldStr, double& field) 
        {
            if (!gsJson.contains(fieldStr))
            {
                errors.emplace_back(std::format("field: globalStage is missing require field: {}", fieldStr));
                return false;
            }
            
            if (!OpenTrackIOHelpers::checkTypeAndSetField(gsJson[fieldStr], field))
            {
                errors.emplace_back(std::format("field: globalStage/{} isn't a number", fieldStr));
                return false;
            }
            return true;
        };

        const auto fieldsSet =
                fieldCheckAndAssign("E", gs.e) && fieldCheckAndAssign("N", gs.n) && 
                fieldCheckAndAssign("U", gs.u) && fieldCheckAndAssign("lat0", gs.lat0) && 
                fieldCheckAndAssign("lon0", gs.lon0) && fieldCheckAndAssign("h0", gs.h0);
        
        if (!fieldsSet)
        {
            return std::nullopt;
        }

        json.erase("globalStage");
        return gs;
    }

    std::optional<Lens> Lens::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("lens") && (!json.contains("static") || !json["static"].contains("lens")))
        {
            return std::nullopt;
        }

        Lens lens{};
        
        // ------- Static Fields
        if (json.contains("static") && json["static"].contains("lens"))
        {
            auto& lensJson = json["static"]["lens"];
            OpenTrackIOHelpers::assignField(lensJson, "firmwareVersion", lens.firmwareVersion, "string", errors);
            OpenTrackIOHelpers::assignField(lensJson, "make", lens.make, "string", errors);
            OpenTrackIOHelpers::assignField(lensJson, "model", lens.model, "string", errors);
            OpenTrackIOHelpers::assignField(lensJson, "nominalFocalLength", lens.nominalFocalLength, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "serialNumber", lens.serialNumber, "string", errors);

            OpenTrackIOHelpers::clearFieldIfEmpty(json["static"], "lens");
        }
        
        // ------- Standard Fields
        if (json.contains("lens"))
        {
            auto& lensJson = json["lens"];
            if (lensJson.contains("custom") && lensJson["custom"].is_array())
            {
                if (!OpenTrackIOHelpers::iterateJsonArrayAndPopulateVector(lensJson["custom"], lens.custom))
                {
                    errors.emplace_back("field: lens/custom value isn't of type: double");
                    lens.custom = std::nullopt;
                }
                lensJson.erase("custom");
            }

            if (lensJson.contains("distortion"))
            {
                std::optional<std::vector<double>> radial = std::nullopt;
                std::optional<std::vector<double>> tangential = std::nullopt;

                OpenTrackIOHelpers::assignField(lensJson["distortion"], "radial", radial, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["distortion"], "tangential", tangential, "double", errors);

                if (radial.has_value())
                {
                    lens.distortion = Distortion();
                    lens.distortion->radial = std::move(radial.value());
                    lens.distortion->tangential = std::move(tangential);
                }
                lensJson.erase("distortion");
            }

            OpenTrackIOHelpers::assignField(lensJson, "distortionOverscan", lens.distortionOverscan, "double", errors);

            if (lensJson.contains("distortionShift"))
            {
                std::optional<double> x = std::nullopt;
                std::optional<double> y = std::nullopt;

                OpenTrackIOHelpers::assignField(lensJson["distortionShift"], "x", x, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["distortionShift"], "y", y, "double", errors);

                if (x.has_value() && y.has_value())
                {
                    lens.distortionShift = DistortionShift{x.value(), y.value()};
                }
                lensJson.erase("distortionShift");
            }

            OpenTrackIOHelpers::assignField(lensJson, "encoders", lens.encoders, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "entrancePupilOffset", lens.entrancePupilOffset, "double", errors);

            if (lensJson.contains("exposureFalloff"))
            {
                std::optional<double> a1 = std::nullopt;
                std::optional<double> a2 = std::nullopt;
                std::optional<double> a3 = std::nullopt;

                OpenTrackIOHelpers::assignField(lensJson["exposureFalloff"], "a1", a1, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["exposureFalloff"], "a2", a2, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["exposureFalloff"], "a3", a3, "double", errors);

                if (a1.has_value())
                {
                    lens.exposureFalloff = ExposureFalloff{a1.value(), a2, a3};
                }
                lensJson.erase("exposureFalloff");
            }

            OpenTrackIOHelpers::assignField(lensJson, "fStop", lens.fStop, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "focalLength", lens.focalLength, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "focusDistance", lens.focusDistance, "double", errors);

            if (lensJson.contains("perspectiveShift"))
            {
                std::optional<double> x = std::nullopt;
                std::optional<double> y = std::nullopt;

                OpenTrackIOHelpers::assignField(lensJson["perspectiveShift"], "x", x, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["perspectiveShift"], "y", y, "double", errors);

                if (x.has_value() && y.has_value())
                {
                    lens.perspectiveShift = PerspectiveShift{x.value(), y.value()};
                }
                lensJson.erase("perspectiveShift");
            }

            OpenTrackIOHelpers::assignField(lensJson, "rawEncoders", lens.rawEncoders, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "tStop", lens.tStop, "double", errors);

            if (lensJson.contains("undistortion"))
            {
                std::optional<std::vector<double>> radial = std::nullopt;
                std::optional<std::vector<double>> tangential = std::nullopt;

                OpenTrackIOHelpers::assignField(lensJson["undistortion"], "radial", radial, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["undistortion"], "tangential", tangential, "double", errors);

                if (radial.has_value())
                {
                    lens.undistortion = Undistortion();
                    lens.undistortion->radial = std::move(radial.value());
                    lens.undistortion->tangential = std::move(tangential);
                }
                lensJson.erase("undistortion");
            }

            OpenTrackIOHelpers::clearFieldIfEmpty(json, "lens");
        }
        
        return lens;
    }

    std::optional<Protocol> Protocol::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("protocol"))
        {
            return std::nullopt;
        }
        
        Protocol pro{};
        auto& proJson = json["protocol"];
        if (!OpenTrackIOHelpers::checkTypeAndSetField(proJson["name"], pro.name))
        {
            errors.emplace_back("field: protocol isn't of type: string");
            return std::nullopt;
        }


        std::optional<std::string> versionStr;
        const std::regex pattern{R"(^[0-9]+.[0-9]+.[0-9]+$)"};
        OpenTrackIOHelpers::assignRegexField(proJson, "version", versionStr, pattern, errors);
        
        if (!versionStr.has_value())
        {
            return std::nullopt;
        }
        pro.version = std::move(versionStr.value());

        json.erase("protocol");
        return pro;
    }

    std::optional<RelatedSampleIds> RelatedSampleIds::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("relatedSampleIds"))
        {
            return std::nullopt;
        }

        if (!json["relatedSampleIds"].is_array())
        {
            errors.emplace_back("field: relatedSampleIds isn't of type: array");
            return std::nullopt;
        }

        RelatedSampleIds rs{};
        const auto& rsJson = json["relatedSampleIds"];
        const std::regex pattern{R"(^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$)"};
        
        for (const auto& item : rsJson.items()) 
        {
            std::string str;
            if (!OpenTrackIOHelpers::checkTypeAndSetField(item.value(), str)) 
            {
                errors.emplace_back("field: relatedSampleIds/element isn't of type: string");
                continue;
            }

            // Check the string received to ensure that it matches the pattern described by the spec.
            if (std::smatch res; !std::regex_match(str, res, pattern))
            {
                errors.emplace_back("field: relatedSampleIds/element doesn't match required pattern");
                continue;
            }
            
            rs.samples.emplace_back(std::move(str));
        }

        json.erase("relatedSampleIds");
        return rs;
    }

    std::optional<SampleId> SampleId::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("sampleId"))
        {
            return std::nullopt;
        }

        std::optional<std::string> str;
        const std::regex pattern{R"(^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$)"};
        OpenTrackIOHelpers::assignRegexField(json, "sampleId", str, pattern, errors);
        
        if (!str.has_value())
        {
            return std::nullopt;
        }

        json.erase("sampleId");
        return SampleId{std::move(str.value())};
    }

    std::optional<StreamId> StreamId::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        if (!json.contains("streamId"))
        {
            return std::nullopt;
        }

        std::optional<std::string> str;
        const std::regex pattern{R"(^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$)"};
        OpenTrackIOHelpers::assignRegexField(json, "streamId", str, pattern, errors);

        if (!str.has_value())
        {
            return std::nullopt;
        }

        json.erase("streamId");
        return StreamId{std::move(str.value())};
    }    

    std::optional<Timing> Timing::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("timing"))
        {
            return std::nullopt;
        }

        if (!json["timing"].is_object())
        {
            errors.emplace_back("field: timing isn't of type: object");
            return std::nullopt;
        }

        Timing timing{};
        auto& timingJson = json["timing"];

        if (timingJson.contains("frameRate"))
        {
            timing.frameRate = opentrackiotypes::Rational::parse(timingJson, "frameRate", errors);
            timingJson.erase("frameRate");
        }
        
        std::optional<std::string> str;
        OpenTrackIOHelpers::assignField(timingJson, "mode", str, "string", errors);
        if (str.has_value() && (str == "external" || "internal"))
        {
            timing.mode = str == "external" ? Mode::EXTERNAL : Mode::INTERNAL;
            timingJson.erase("mode");
        }
        else
        {
            errors.emplace_back("field: timing/mode has an invalid string value.");
            timing.mode = std::nullopt;
        }
        
        if (timingJson.contains("recordedTimestamp"))
        {
            timing.recordedTimestamp = opentrackiotypes::Timestamp::parse(timingJson, "recordedTimestamp", errors);
            timingJson.erase("recordedTimestamp");
        }

        if (timingJson.contains("sampleTimestamp"))
        {
            timing.sampleTimestamp = opentrackiotypes::Timestamp::parse(timingJson, "sampleTimestamp", errors);
            timingJson.erase("sampleTimestamp");
        }

        OpenTrackIOHelpers::assignField(timingJson, "sequenceNumber", timing.sequenceNumber, "uint16", errors);
        
        if (timingJson.contains("synchronization"))
        {
            timing.synchronization = parseSynchronization(timingJson["synchronization"], errors);
            OpenTrackIOHelpers::clearFieldIfEmpty(timingJson, "synchronization");
        }
        
        if (timingJson.contains("timecode"))
        {
            timing.timecode = opentrackiotypes::Timecode::parse(timingJson, "timecode", errors);
            timingJson.erase("timecode");
        }

        OpenTrackIOHelpers::clearFieldIfEmpty(json, "timing");
        return timing;
    }

    std::optional<Timing::Synchronization>
    Timing::parseSynchronization(nlohmann::json &json, std::vector<std::string> &errors)
    {
        Timing::Synchronization outSync{};

        // Required Fields -------
        bool hasRequired = json.contains("frequency") && json.contains("locked") && json.contains("source");
        if (!hasRequired)
        {
            errors.emplace_back("field: timing/synchronization is missing required fields");
            return std::nullopt;
        }
        
        std::optional<opentrackiotypes::Rational> freq = opentrackiotypes::Rational::parse(json, "frequency", errors);
        if (!freq.has_value())
        {
            errors.emplace_back("field: timing/synchronization/frequency is missing required fields");
            return std::nullopt;
        }
        outSync.frequency = freq.value();
        json.erase("frequency");
        
        if (!OpenTrackIOHelpers::checkTypeAndSetField(json["locked"], outSync.locked))
        {
            errors.emplace_back("field: timing/synchronization/lock isn't of type: bool");
            return std::nullopt;
        }
        json.erase("locked");

        std::string str;
        if (!OpenTrackIOHelpers::checkTypeAndSetField(json["source"], str))
        {
            errors.emplace_back("field: timing/synchronization/source isn't of type: string");
            return std::nullopt;
        }
        else
        {
            if (str == "genlock")
            {
                outSync.source = Synchronization::SourceType::GEN_LOCK;
            }
            else if (str == "videoIn")
            {
                outSync.source = Synchronization::SourceType::VIDEO_IN;
            }
            else if (str == "ptp")
            {
                outSync.source = Synchronization::SourceType::PTP;
            }
            else if (str == "ntp")
            {
                outSync.source = Synchronization::SourceType::NTP;
            }
            else
            {
                errors.emplace_back("field: timing/synchronization/source isn't a valid enumeration");
                return std::nullopt;
            }
            json.erase("source");
        }

        // Non-Required Fields --------
        if (json.contains("offsets"))
        {
            outSync.offsets = Synchronization::Offsets{};
            OpenTrackIOHelpers::assignField(json["offsets"], "translation", outSync.offsets->translation, "double", errors);
            OpenTrackIOHelpers::assignField(json["offsets"], "rotation", outSync.offsets->rotation, "double", errors);
            OpenTrackIOHelpers::assignField(json["offsets"], "lensEncoders", outSync.offsets->lensEncoders, "double", errors);

            if (!outSync.offsets->translation.has_value() && !outSync.offsets->rotation.has_value() &&
                !outSync.offsets->lensEncoders.has_value())
            {
                outSync.offsets = std::nullopt;
            }
            json.erase("offsets");
        }

        OpenTrackIOHelpers::assignField(json, "present", outSync.present, "bool", errors);
        
        if (json.contains("ptp"))
        {
            outSync.ptp = Synchronization::Ptp{};
            OpenTrackIOHelpers::assignField(json["ptp"], "domain", outSync.ptp->domain, "uint16", errors);
            OpenTrackIOHelpers::assignField(json["ptp"], "offset", outSync.ptp->offset, "double", errors);
            const std::regex pattern{R"(^([A-F0-9]{2}:){5}[A-F0-9]{2}$)"};
            OpenTrackIOHelpers::assignRegexField(json["ptp"], "master", outSync.ptp->master, pattern, errors);

            if (!outSync.ptp->domain.has_value() && !outSync.ptp->offset.has_value() && !outSync.ptp->master.has_value())
            {
                outSync.ptp = std::nullopt;
            }
            json.erase("ptp");
        }
        
        return outSync;
    }

    std::optional<Tracker> Tracker::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("tracker") && (!json.contains("static") || !json["static"].contains("tracker")))
        {
            return std::nullopt;
        }

        Tracker tkr{};

        // ------- Static Fields
        if (json.contains("static") && json["static"].contains("tracker"))
        {
            auto& tkrJson = json["static"]["tracker"];
            OpenTrackIOHelpers::assignField(tkrJson, "firmwareVersion", tkr.firmwareVersion, "string", errors);
            OpenTrackIOHelpers::assignField(tkrJson, "make", tkr.make, "string", errors);
            OpenTrackIOHelpers::assignField(tkrJson, "model", tkr.model, "string", errors);
            OpenTrackIOHelpers::assignField(tkrJson, "serialNumber", tkr.serialNumber, "string", errors);

            OpenTrackIOHelpers::clearFieldIfEmpty(json["static"], "tracker");
        }
        
        // ------- Standard Fields
        if (json.contains("tracker"))
        {
            auto& tkrJson = json["tracker"];
            OpenTrackIOHelpers::assignField(tkrJson, "notes", tkr.notes, "string", errors);
            OpenTrackIOHelpers::assignField(tkrJson, "recording", tkr.recording, "boolean", errors);
            OpenTrackIOHelpers::assignField(tkrJson, "slate", tkr.slate, "string", errors);
            OpenTrackIOHelpers::assignField(tkrJson, "status", tkr.status, "string", errors);

            OpenTrackIOHelpers::clearFieldIfEmpty(json, "tracker");
        }
        
        return tkr;
    }    

    std::optional<Transforms> Transforms::parse(nlohmann::json &json, std::vector<std::string>& errors)
    {
        if (!json.contains("transforms"))
        {
            return std::nullopt;
        }

        if (!json["transforms"].is_array())
        {
            errors.emplace_back("Transforms is not an array.");
            return std::nullopt;
        }

        Transforms tfs{};
        auto& tfsJson = json["transforms"];

        for (auto& item : tfsJson.items())
        {
            auto& transformJson = item.value();
            auto tf = opentrackiotypes::Transform::parse(transformJson, errors);

            if (tf.has_value())
            {
                tfs.transforms.emplace_back(tf.value());
            }
        }
        
        json.erase("transforms");
        return tfs;
    }
} // opentrackioproperties
