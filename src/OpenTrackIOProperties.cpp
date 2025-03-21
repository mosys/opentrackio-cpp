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

#include "opentrackio-cpp/OpenTrackIOProperties.h"
#include "opentrackio-cpp/OpenTrackIOHelper.h"
#include <regex>

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
        
        OpenTrackIOHelpers::assignField(cameraJson, "isoSpeed", cam.isoSpeed, "uint32", errors);
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
        
        OpenTrackIOHelpers::assignField(durationJson, "num", numerator, "uint64", errors);
        OpenTrackIOHelpers::assignField(durationJson, "denom", denominator, "uint64", errors);
        
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
            OpenTrackIOHelpers::assignField(lensJson, "distortionOverscanMax", lens.distortionOverscanMax, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "undistortionOverscanMax", lens.undistortionOverscanMax, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "calibrationHistory", lens.calibrationHistory, "string", errors);

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

            if (lensJson.contains("distortion") && lensJson["distortion"].is_array())
            {
                lens.distortion = std::vector<Distortion>{};
                for (auto& dist : lensJson["distortion"])
                {
                    std::optional<std::vector<double>> radial = std::nullopt;
                    std::optional<std::vector<double>> tangential = std::nullopt;
                    std::optional<double> overscan = std::nullopt;
                    std::optional<std::string> model = std::nullopt;

                    OpenTrackIOHelpers::assignField(dist, "radial", radial, "double", errors);
                    OpenTrackIOHelpers::assignField(dist, "tangential", tangential, "double", errors);
                    OpenTrackIOHelpers::assignField(dist, "model", model, "string", errors);
                    OpenTrackIOHelpers::assignField(dist, "overscan", overscan, "double", errors);

                    if (radial.has_value())
                    {
                        auto d = Distortion();
                        d.radial = std::move(radial.value());
                        d.tangential = std::move(tangential);
                        d.model = std::move(model);
                        d.overscan = std::move(overscan);
                        lens.distortion->emplace_back(d);
                    }
                }
                lensJson.erase("distortion");
            }

            if (lensJson.contains("distortionOffset"))
            {
                std::optional<double> x = std::nullopt;
                std::optional<double> y = std::nullopt;

                OpenTrackIOHelpers::assignField(lensJson["distortionOffset"], "x", x, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["distortionOffset"], "y", y, "double", errors);

                if (x.has_value() && y.has_value())
                {
                    lens.distortionOffset = DistortionOffset{x.value(), y.value()};
                }
                lensJson.erase("distortionOffset");
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
            OpenTrackIOHelpers::assignField(lensJson, "pinholeFocalLength", lens.pinholeFocalLength, "double", errors);
            OpenTrackIOHelpers::assignField(lensJson, "focusDistance", lens.focusDistance, "double", errors);

            if (lensJson.contains("calibrationHistory") && lensJson["calibrationHistory"].is_array())
            {
                if (!OpenTrackIOHelpers::iterateJsonArrayAndPopulateVector(lensJson["calibrationHistory"], lens.calibrationHistory))
                {
                    errors.emplace_back("field: lens/calibrationHistory value isn't of type: string");
                    lens.calibrationHistory = std::nullopt;
                }
                lensJson.erase("calibrationHistory");
            }

            if (lensJson.contains("projectionOffset"))
            {
                std::optional<double> x = std::nullopt;
                std::optional<double> y = std::nullopt;

                OpenTrackIOHelpers::assignField(lensJson["projectionOffset"], "x", x, "double", errors);
                OpenTrackIOHelpers::assignField(lensJson["projectionOffset"], "y", y, "double", errors);

                if (x.has_value() && y.has_value())
                {
                    lens.projectionOffset = ProjectionOffset{x.value(), y.value()};
                }
                lensJson.erase("projectionOffset");
            }

            if (lensJson.contains("rawEncoders"))
            {
                lens.rawEncoders = RawEncoders{};
                OpenTrackIOHelpers::assignField(lensJson["rawEncoders"], "focus", lens.rawEncoders->focus, "unit32", errors);
                OpenTrackIOHelpers::assignField(lensJson["rawEncoders"], "iris", lens.rawEncoders->iris, "uint32", errors);
                OpenTrackIOHelpers::assignField(lensJson["rawEncoders"], "zoom", lens.rawEncoders->zoom, "uint32", errors);
                lensJson.erase("rawEncoders");
            }

            OpenTrackIOHelpers::assignField(lensJson, "tStop", lens.tStop, "double", errors);

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

        if (!proJson["version"].is_array())
        {
            errors.emplace_back("field: protocol version isn't of type: [int, int, int]");
            return std::nullopt;
        }

        if (proJson["version"].size() != 3)
        {
            errors.emplace_back("field: protocol version isn't of size 3: [int, int, int]");
            return std::nullopt;
        }

        if (proJson["version"][0] != OPEN_TRACK_IO_PROTOCOL_MAJOR_VERSION ||
            proJson["version"][1] != OPEN_TRACK_IO_PROTOCOL_MINOR_VERSION || 
            proJson["version"][2] != OPEN_TRACK_IO_PROTOCOL_PATCH)
        {
            errors.emplace_back("version: protocol version mismatch");
            return std::nullopt;
        }
        
        pro.version = {
            OPEN_TRACK_IO_PROTOCOL_MAJOR_VERSION,
            OPEN_TRACK_IO_PROTOCOL_MINOR_VERSION,
            OPEN_TRACK_IO_PROTOCOL_PATCH
        };

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
  
    std::optional<SourceId> SourceId::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        if (!json.contains("sourceId"))
        {
            return std::nullopt;
        }

        std::optional<std::string> str;
        const std::regex pattern{R"(^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$)"};
        OpenTrackIOHelpers::assignRegexField(json, "sourceId", str, pattern, errors);

        if (!str.has_value())
        {
            return std::nullopt;
        }

        json.erase("sourceId");
        return SourceId{std::move(str.value())};
    }

    std::optional<SourceNumber> SourceNumber::parse(nlohmann::json &json, std::vector<std::string> &errors)
    {
        if (!json.contains("sourceNumber"))
        {
            return std::nullopt;
        }

        std::optional<uint32_t> val;
        OpenTrackIOHelpers::assignField(json, "sourceNumber", val, "uint32", errors);

        if (!val.has_value())
        {
            return std::nullopt;
        }

        json.erase("sourceNumber");
        return SourceNumber{val.value()};
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

        if (timingJson.contains("sampleRate"))
        {
            timing.sampleRate = opentrackiotypes::Rational::parse(timingJson, "sampleRate", errors);
            timingJson.erase("sampleRate");
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
            timing.synchronization = parseSynchronization(timingJson, errors);
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
        Synchronization outSync{};
        auto& syncJson = json["synchronization"];

        // Required Fields -------
        const bool hasRequired = syncJson.contains("locked") && syncJson.contains("source");
        if (!hasRequired)
        {
            errors.emplace_back("field: timing/synchronization is missing required fields");
            return std::nullopt;
        }
        
        if (syncJson.contains("frequency"))
        {
            std::optional<opentrackiotypes::Rational> freq = opentrackiotypes::Rational::parse(syncJson, "frequency", errors);
            if (!freq.has_value())
            {
                errors.emplace_back("field: timing/synchronization/frequency is missing required fields");
                return std::nullopt;
            }
            outSync.frequency = freq.value();
            syncJson.erase("frequency");
        }

        if (!OpenTrackIOHelpers::checkTypeAndSetField(syncJson["locked"], outSync.locked))
        {
            errors.emplace_back("field: timing/synchronization/locked isn't of type: bool");
            return std::nullopt;
        }
        syncJson.erase("locked");

        std::string str;
        if (!OpenTrackIOHelpers::checkTypeAndSetField(syncJson["source"], str))
        {
            errors.emplace_back("field: timing/synchronization/source isn't of type: string");
            return std::nullopt;
        }

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
        syncJson.erase("source");

        // Non-Required Fields --------
        if (syncJson.contains("offsets"))
        {
            outSync.offsets = Synchronization::Offsets{};
            OpenTrackIOHelpers::assignField(syncJson["offsets"], "translation", outSync.offsets->translation, "double", errors);
            OpenTrackIOHelpers::assignField(syncJson["offsets"], "rotation", outSync.offsets->rotation, "double", errors);
            OpenTrackIOHelpers::assignField(syncJson["offsets"], "lensEncoders", outSync.offsets->lensEncoders, "double", errors);

            if (!outSync.offsets->translation.has_value() && !outSync.offsets->rotation.has_value() &&
                !outSync.offsets->lensEncoders.has_value())
            {
                outSync.offsets = std::nullopt;
            }
            syncJson.erase("offsets");
        }

        OpenTrackIOHelpers::assignField(syncJson, "present", outSync.present, "bool", errors);

        if (syncJson.contains("ptp"))
        {
            outSync.ptp = parsePtp(syncJson, errors);
            OpenTrackIOHelpers::clearFieldIfEmpty(syncJson, "ptp");
        }

        return outSync;
    }

    std::optional<Timing::Synchronization::Ptp>
    Timing::parsePtp(nlohmann::json& json, std::vector<std::string>& errors)
    {
        Synchronization::Ptp outPtp{};
        auto& ptpJson = json["ptp"];

        std::optional<std::string> profileStr;
        OpenTrackIOHelpers::assignField(ptpJson, "profile", profileStr, "string", errors);
        bool successfullyAssignedProfileField = false;
        if (profileStr.has_value())
        {
            successfullyAssignedProfileField = true;
            if (profileStr == "IEEE Std 1588-2019")
            {
                outPtp.profile = Synchronization::Ptp::ProfileType::IEEE_Std_1588_2019;
            }
            else if (profileStr == "IEEE Std 802.1AS-2020")
            {
                outPtp.profile = Synchronization::Ptp::ProfileType::IEEE_Std_802_1AS_2020;
            }
            else if (profileStr == "SMPTE ST2059-2:2021")
            {
                outPtp.profile = Synchronization::Ptp::ProfileType::SMPTE_ST2059_2_2021;
            }
            else
            {
                successfullyAssignedProfileField = false;
            }
        }
        else
        {
            errors.emplace_back("field: timing/synchronization/ptp/profile is required, however it is missing.");
            return std::nullopt;
        }

        if (!successfullyAssignedProfileField)
        {
            errors.emplace_back("field: profile has an invalid string value.");
            return std::nullopt;
        }
        ptpJson.erase("profile");

        std::optional<uint16_t> domain;
        OpenTrackIOHelpers::assignField(ptpJson, "domain", domain, "uint16", errors);
        if (!domain.has_value())
        {
            errors.emplace_back("field: timing/synchronization/ptp/domain is required, however it is missing.");
            return std::nullopt;
        }
        outPtp.domain = domain.value();

        std::optional<std::string> leaderIdentity;
        const std::regex pattern{R"((?:^[0-9a-f]{2}(?::[0-9a-f]{2}){5}$)|(?:^[0-9a-f]{2}(?:-[0-9a-f]{2}){5}$))"};
        OpenTrackIOHelpers::assignRegexField(ptpJson, "leaderIdentity", leaderIdentity, pattern, errors);

        if (!leaderIdentity.has_value())
        {
            errors.emplace_back("field: timing/synchronization/ptp/leaderIdentity is required, however it is missing.");
            return std::nullopt;
        }
        outPtp.leaderIdentity = leaderIdentity.value();

        std::optional<uint8_t> priority1;
        std::optional<uint8_t> priority2;
        constexpr auto leaderPrioritiesStr = "leaderPriorities";
        OpenTrackIOHelpers::assignField(ptpJson[leaderPrioritiesStr], "priority1", priority1, "uint8", errors);
        OpenTrackIOHelpers::assignField(ptpJson[leaderPrioritiesStr], "priority2", priority2, "uint8", errors);

        if (!priority1.has_value() || !priority2.has_value())
        {
            errors.emplace_back("field: timing/synchronization/ptp/leaderPriorities is required, however it is missing a subfield(s).");
            return std::nullopt;
        }
        OpenTrackIOHelpers::clearFieldIfEmpty(ptpJson, leaderPrioritiesStr);

        outPtp.leaderPriorities = Synchronization::Ptp::LeaderPriorities{
            priority1.value(),
            priority2.value()
        };

        std::optional<double> leaderAccuracy;
        OpenTrackIOHelpers::assignField(ptpJson, "leaderAccuracy", leaderAccuracy, "double", errors);
        if (!leaderAccuracy.has_value())
        {
            errors.emplace_back("field: timing/synchronization/ptp/leaderAccuracy is required, however it is missing.");
            return std::nullopt;
        }
        outPtp.leaderAccuracy = leaderAccuracy.value();

        std::optional<double> meanPathDelay;
        OpenTrackIOHelpers::assignField(ptpJson, "meanPathDelay", meanPathDelay, "double", errors);
        if (!meanPathDelay.has_value())
        {
            errors.emplace_back("field: timing/synchronization/ptp/meanPathDelay is required, however it is missing.");
            return std::nullopt;
        }
        outPtp.meanPathDelay = meanPathDelay.value();

        OpenTrackIOHelpers::assignField(ptpJson, "vlan", outPtp.vlan, "uint32", errors);

        std::optional<std::string> leaderTimeSourceStr;
        OpenTrackIOHelpers::assignField(ptpJson, "leaderTimeSource", leaderTimeSourceStr, "string", errors);
        if (leaderTimeSourceStr.has_value())
        {
            if (leaderTimeSourceStr == "GNSS")
            {
                outPtp.leaderTimeSource = Synchronization::Ptp::LeaderTimeSourceType::GNSS;
            }
            else if (leaderTimeSourceStr == "Atomic clock")
            {
                outPtp.leaderTimeSource = Synchronization::Ptp::LeaderTimeSourceType::Atomic_clock;
            }
            else if (leaderTimeSourceStr == "NTP")
            {
                outPtp.leaderTimeSource = Synchronization::Ptp::LeaderTimeSourceType::NTP;
            }
        }

        return outPtp;
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
