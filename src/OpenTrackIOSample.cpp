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

#include "opentrackio-cpp/OpenTrackIOSample.h"
#include <format>

namespace opentrackio 
{
    template<typename T>
    void assignJson(nlohmann::json &json, std::string_view field, const std::optional<T> &value)
    {
        if (value.has_value())
        {
            json[field] = value.value();
        }
    };

    template<typename T>
    void assignJson(nlohmann::json &json, std::string_view field, const std::optional<opentrackiotypes::Dimensions<T>> &value)
    {
        if (value.has_value())
        {
            json[field]["height"] = value->height;
            json[field]["width"] = value->width;
        }
    }

    template<>
    void assignJson(nlohmann::json &json, std::string_view field, const std::optional<opentrackiotypes::Rational> &value)
    {
        if (value.has_value())
        {
            json[field]["num"] = value->numerator;
            json[field]["denom"] = value->denominator;
        }
    };

    template<>
    void assignJson(nlohmann::json &json, std::string_view field, const std::optional<opentrackiotypes::Timestamp> &value)
    {
        if (value.has_value())
        {
            json[field]["seconds"] = value->seconds;
            json[field]["nanoseconds"] = value->nanoseconds;
        }
    };
    
    bool OpenTrackIOSample::initialise(const nlohmann::json &json)
    {
        // Take a copy of the full JSON for referencing later if needed.
        m_json = json;
        
        /** 
         * Take a second copy of the original which will have all processed fields removed (this is to check for
         * leftover fields. */
        
        nlohmann::json jsonCopy = json;
        camera = opentrackioproperties::Camera::parse(jsonCopy, m_errorMessages);
        duration = opentrackioproperties::Duration::parse(jsonCopy, m_errorMessages);
        globalStage = opentrackioproperties::GlobalStage::parse(jsonCopy, m_errorMessages);
        lens = opentrackioproperties::Lens::parse(jsonCopy, m_errorMessages);
        protocol = opentrackioproperties::Protocol::parse(jsonCopy, m_errorMessages);
        relatedSampleIds = opentrackioproperties::RelatedSampleIds::parse(jsonCopy, m_errorMessages);
        sampleId = opentrackioproperties::SampleId::parse(jsonCopy, m_errorMessages);
        sourceId = opentrackioproperties::SourceId::parse(jsonCopy, m_errorMessages);
        sourceNumber = opentrackioproperties::SourceNumber::parse(jsonCopy, m_errorMessages);
        timing = opentrackioproperties::Timing::parse(jsonCopy, m_errorMessages);
        tracker = opentrackioproperties::Tracker::parse(jsonCopy, m_errorMessages);
        transforms = opentrackioproperties::Transforms::parse(jsonCopy, m_errorMessages);
        
        // Check the copy to see if it has remaining fields and if so bubble up warnings.
        warnForRemainingFields(jsonCopy);
        
        return true;
    }

    bool OpenTrackIOSample::initialise(const std::string_view jsonString)
    {
        nlohmann::json from_string = nlohmann::json::parse(jsonString);
        return initialise(from_string);
    }
    
    bool OpenTrackIOSample::initialise(std::span<const uint8_t> cbor)
    {
        nlohmann::json from_cbor = nlohmann::json::from_cbor(cbor);
        return initialise(from_cbor);
    }

    const nlohmann::json &OpenTrackIOSample::getJson()
    {
        if (!m_json.has_value())
        {
            generateJson();
        }        
        
        return m_json.value();
    }

    void OpenTrackIOSample::generateJson()
    {
        namespace props = opentrackioproperties;
        
        nlohmann::json j;

        parseCameraToJson(j);
        parseDurationToJson(j);
        parseGlobalStageToJson(j);
        parseLensToJson(j);
        parseProtocolToJson(j);
        parseRelatedSampleIdsToJson(j);
        parseSampleIdToJson(j);
        parseSourceIdToJson(j);
        parseSourceNumberToJson(j);
        parseTimingToJson(j);
        parseTrackerToJson(j);
        parseTransformsToJson(j);
        
        m_json = j;
    }

    void OpenTrackIOSample::parseCameraToJson(nlohmann::json& baseJson)
    {
        if (!camera.has_value())
        {
            return;
        }
        
        auto& cameraJson = baseJson["static"]["camera"];
        assignJson(cameraJson, "activeSensorPhysicalDimensions", camera->activeSensorPhysicalDimensions);
        assignJson(cameraJson, "activeSensorResolution", camera->activeSensorResolution);
        assignJson(cameraJson, "anamorphicSqueeze", camera->anamorphicSqueeze);
        assignJson(cameraJson, "firmwareVersion", camera->firmwareVersion);
        assignJson(cameraJson, "label", camera->label);
        assignJson(cameraJson, "make", camera->make);
        assignJson(cameraJson, "model", camera->model);
        assignJson(cameraJson, "serialNumber", camera->serialNumber);
        assignJson(cameraJson, "captureFrameRate", camera->captureFrameRate);
        assignJson(cameraJson, "fdlLink", camera->fdlLink);
        assignJson(cameraJson, "isoSpeed", camera->isoSpeed);
        assignJson(cameraJson, "shutterAngle", camera->shutterAngle);
    }

    void OpenTrackIOSample::parseDurationToJson(nlohmann::json &baseJson)
    {
        if (!duration.has_value())
        {
            return;
        }

        baseJson["static"]["duration"]["num"] = duration->rational.numerator;
        baseJson["static"]["duration"]["denom"] = duration->rational.denominator;
    }

    void OpenTrackIOSample::parseGlobalStageToJson(nlohmann::json &baseJson)
    {
        if (!globalStage.has_value())
        {
            return;
        }

        baseJson["globalStage"]["E"] = globalStage->e;
        baseJson["globalStage"]["N"] = globalStage->n;
        baseJson["globalStage"]["U"] = globalStage->u;
        baseJson["globalStage"]["lat0"] = globalStage->lat0;
        baseJson["globalStage"]["lon0"] = globalStage->lon0;
        baseJson["globalStage"]["h0"] = globalStage->h0;
    }    

    void OpenTrackIOSample::parseLensToJson(nlohmann::json& baseJson)
    {
        if (!lens.has_value())
        {
            return;
        }
        
        // ------- Static Fields
        assignJson(baseJson["static"]["lens"], "firmwareVersion", lens->firmwareVersion);
        assignJson(baseJson["static"]["lens"], "make", lens->make);
        assignJson(baseJson["static"]["lens"], "model", lens->model);
        assignJson(baseJson["static"]["lens"], "nominalFocalLength", lens->nominalFocalLength);
        assignJson(baseJson["static"]["lens"], "serialNumber", lens->serialNumber);
        assignJson(baseJson["static"]["lens"], "distortionOverscanMax", lens->distortionOverscanMax);
        assignJson(baseJson["static"]["lens"], "undistortionOverscanMax", lens->undistortionOverscanMax);

        // ------- Standard Fields
        assignJson(baseJson["lens"], "custom", lens->custom);
        
        baseJson["lens"]["distortion"] = nlohmann::json::array();
        for (const auto& dist : lens->distortion.value())
        {
            nlohmann::json distJson{};
            distJson["radial"] = dist.radial;
            if (dist.tangential.has_value())
            {
                distJson["tangential"] = dist.tangential.value();
            }
            baseJson["lens"]["distortion"].push_back(distJson);
        }

        if (lens->distortionOffset.has_value())
        {
            baseJson["lens"]["distortionOffset"]["x"] = lens->distortionOffset->x;
            baseJson["lens"]["distortionOffset"]["y"] = lens->distortionOffset->y;
        }

        if (lens->encoders.has_value())
        {
            assignJson(baseJson["lens"]["encoders"], "focus", lens->encoders->focus);
            assignJson(baseJson["lens"]["encoders"], "iris", lens->encoders->iris);
            assignJson(baseJson["lens"]["encoders"], "zoom", lens->encoders->zoom);
        }

        assignJson(baseJson["lens"], "entrancePupilOffset", lens->entrancePupilOffset);
        
        if (lens->exposureFalloff.has_value())
        {
            baseJson["lens"]["exposureFalloff"]["a1"] = lens->exposureFalloff->a1;
            assignJson(baseJson["lens"]["exposureFalloff"], "a2", lens->exposureFalloff->a2);
            assignJson(baseJson["lens"]["exposureFalloff"], "a3", lens->exposureFalloff->a3);
        }

        assignJson(baseJson["lens"], "fStop", lens->fStop);
        assignJson(baseJson["lens"], "focusDistance", lens->focusDistance);

        if (lens->projectionOffset.has_value())
        {
            baseJson["lens"]["projectionOffset"]["x"] = lens->projectionOffset->x;
            baseJson["lens"]["projectionOffset"]["y"] = lens->projectionOffset->y;
        }

        if (lens->rawEncoders.has_value())
        {
            assignJson(baseJson["lens"]["rawEncoders"], "focus", lens->rawEncoders->focus);
            assignJson(baseJson["lens"]["rawEncoders"], "iris", lens->rawEncoders->iris);
            assignJson(baseJson["lens"]["rawEncoders"], "zoom", lens->rawEncoders->zoom);
        }
        
        assignJson(baseJson["lens"], "tStop", lens->tStop);
    }

    void OpenTrackIOSample::parseProtocolToJson(nlohmann::json &baseJson)
    {
        if (!protocol.has_value())
        {
            return;
        }
        
        baseJson["protocol"]["name"] = protocol->name;
        baseJson["protocol"]["version"] = protocol->version;
    }

    void OpenTrackIOSample::parseRelatedSampleIdsToJson(nlohmann::json &baseJson)
    {
        if (!relatedSampleIds.has_value())
        {
            return;
        }

        baseJson["relatedSampleIds"] = relatedSampleIds->samples;
    }

    void OpenTrackIOSample::parseSampleIdToJson(nlohmann::json &baseJson)
    {
        if (!sampleId.has_value())
        {
            return;
        }

        baseJson["sampleId"] = sampleId->id;
    }

    void OpenTrackIOSample::parseSourceIdToJson(nlohmann::json &baseJson)
    {
        if (!sourceId.has_value())
        {
            return;
        }

        baseJson["sourceId"] = sourceId->id;
    }

    void OpenTrackIOSample::parseSourceNumberToJson(nlohmann::json &baseJson)
    {
        if (!sourceNumber.has_value())
        {
            return;
        }

        baseJson["sourceNumber"] = sourceNumber->value;
    }    

    void OpenTrackIOSample::parseTimingToJson(nlohmann::json& baseJson)
    {
        if (!timing.has_value())
        {
            return;
        }
        
        assignJson(baseJson["timing"], "sampleRate", timing->sampleRate);
        if (timing->mode.has_value())
        {
            baseJson["timing"]["mode"] = timing->mode.value() == opentrackioproperties::Timing::Mode::INTERNAL ?
                                  "internal" : "external";
        }
        assignJson(baseJson["timing"], "recordedTimestamp", timing->recordedTimestamp);
        assignJson(baseJson["timing"], "sampleTimestamp", timing->sampleTimestamp);
        assignJson(baseJson["timing"], "sequenceNumber", timing->sequenceNumber);

        if (timing->synchronization.has_value())
        {
            if (timing->synchronization->frequency.has_value())
            {
                baseJson["timing"]["synchronization"]["frequency"]["num"] = timing->synchronization->frequency->numerator;
                baseJson["timing"]["synchronization"]["frequency"]["denom"] = timing->synchronization->frequency->denominator;
            }
            baseJson["timing"]["synchronization"]["locked"] = timing->synchronization->locked;
            switch (timing->synchronization->source)
            {
                case opentrackioproperties::Timing::Synchronization::SourceType::GEN_LOCK:
                    baseJson["timing"]["synchronization"]["source"] = "genlock";
                    break;
                case opentrackioproperties::Timing::Synchronization::SourceType::VIDEO_IN:
                    baseJson["timing"]["synchronization"]["source"] = "videoIn";
                    break;
                case opentrackioproperties::Timing::Synchronization::SourceType::PTP:
                    baseJson["timing"]["synchronization"]["source"] = "ptp";
                    break;
                case opentrackioproperties::Timing::Synchronization::SourceType::NTP:
                    baseJson["timing"]["synchronization"]["source"] = "ntp";
                    break;
            }

            if (timing->synchronization->offsets.has_value())
            {
                const auto& offsets = timing->synchronization->offsets.value();
                assignJson(baseJson["timing"]["synchronization"]["offsets"], "translation", offsets.translation);
                assignJson(baseJson["timing"]["synchronization"]["offsets"], "rotation", offsets.rotation);
                assignJson(baseJson["timing"]["synchronization"]["offsets"], "lensEncoders", offsets.lensEncoders);
            }            

            assignJson(baseJson["timing"]["synchronization"], "present", timing->synchronization->present);

            auto ptpJson = baseJson["timing"]["synchronization"]["ptp"];
            if (timing->synchronization->ptp.has_value())
            {
                const auto& ptp = timing->synchronization->ptp.value();
                switch (ptp.profile)
                {
                    case opentrackioproperties::Timing::Synchronization::Ptp::ProfileType::IEEE_Std_1588_2019:
                        ptpJson["profile"] = "IEEE Std 1588-2019";
                        break;
                    case opentrackioproperties::Timing::Synchronization::Ptp::ProfileType::IEEE_Std_802_1AS_2020:
                        ptpJson["profile"] = "IEEE Std 802.1AS-2020";
                        break;
                    case opentrackioproperties::Timing::Synchronization::Ptp::ProfileType::SMPTE_ST2059_2_2021:
                        ptpJson["profile"] = "SMPTE ST2059-2:2021";
                }

                ptpJson["domain"] = ptp.domain;
                ptpJson["leaderIdentity"] = ptp.leaderIdentity;

                const auto& [priority1, priority2] = ptp.leaderPriorities;
                ptpJson["leaderPriorities"]["priority1"] = priority1;
                ptpJson["leaderPriorities"]["priority2"] = priority2;

                ptpJson["leaderAccuracy"] = ptp.leaderAccuracy;
                ptpJson["meanPathDelay"] = ptp.meanPathDelay;

                assignJson(ptpJson, "vlan", ptp.vlan);

                if (ptp.leaderTimeSource.has_value())
                {
                    switch (ptp.leaderTimeSource.value())
                    {
                        case opentrackioproperties::Timing::Synchronization::Ptp::LeaderTimeSourceType::GNSS:
                            ptpJson["leaderTimeSource"] = "GNSS";
                        break;
                        case opentrackioproperties::Timing::Synchronization::Ptp::LeaderTimeSourceType::Atomic_clock:
                            ptpJson["leaderTimeSource"] = "Atomic clock";
                        break;
                        case opentrackioproperties::Timing::Synchronization::Ptp::LeaderTimeSourceType::NTP:
                            ptpJson["leaderTimeSource"] = "NTP";
                    }
                }
            }
        }

        if (timing->timecode.has_value())
        {
            baseJson["timing"]["timecode"]["hours"] = timing->timecode->hours;
            baseJson["timing"]["timecode"]["minutes"] = timing->timecode->minutes;
            baseJson["timing"]["timecode"]["seconds"] = timing->timecode->seconds;
            baseJson["timing"]["timecode"]["frames"] = timing->timecode->frames;
            baseJson["timing"]["timecode"]["frameRate"]["num"] = timing->timecode->frameRate.numerator;
            baseJson["timing"]["timecode"]["frameRate"]["denom"] = timing->timecode->frameRate.denominator;
            assignJson(baseJson["timing"]["timecode"], "subFrame", timing->timecode->subFrame);
        }
    }

    void OpenTrackIOSample::parseTrackerToJson(nlohmann::json& baseJson)
    {
        if (!tracker.has_value())
        {
            return;
        }

        // ------- Static Fields
        assignJson(baseJson["static"]["tracker"], "firmwareVersion", tracker->firmwareVersion);
        assignJson(baseJson["static"]["tracker"], "make", tracker->make);
        assignJson(baseJson["static"]["tracker"], "model", tracker->model);
        assignJson(baseJson["static"]["tracker"], "serialNumber", tracker->serialNumber);

        // ------- Standard Fields
        assignJson(baseJson["tracker"], "notes", tracker->notes);
        assignJson(baseJson["tracker"], "recording", tracker->recording);
        assignJson(baseJson["tracker"], "slate", tracker->slate);
        assignJson(baseJson["tracker"], "status", tracker->status);
    }    
    
    void OpenTrackIOSample::parseTransformsToJson(nlohmann::json& baseJson)
    {
        if (!transforms.has_value())
        {
            return;
        }

        baseJson["transforms"] = nlohmann::json::array();
        for (const auto& tf : transforms->transforms)
        {
            nlohmann::json tfJson{};
            tfJson["translation"] = {{"x", tf.translation.x}, {"y", tf.translation.y}, {"z", tf.translation.z}};
            tfJson["rotation"] = {{"pan", tf.rotation.pan}, {"tilt", tf.rotation.tilt}, {"roll", tf.rotation.roll}};
            assignJson(tfJson, "id", tf.id);
            if (tf.scale.has_value())
            {
                tfJson["scale"] = {{"x", tf.scale->x}, {"y", tf.scale->y}, {"z", tf.scale->z}};
            }
            
            baseJson["transforms"].push_back(tfJson);
        }
    }

    void OpenTrackIOSample::warnForRemainingFields(const nlohmann::json &json)
    {
        const std::function<void(const nlohmann::json&)> iterateItemsAndWarn = [&](const nlohmann::json& currentRoot){
            if (!currentRoot.is_object() || std::distance(currentRoot.items().begin(), currentRoot.items().end()) == 0)
            {
                return;
            }
            
            for (const auto& [key, val] : currentRoot.items())
            {
                if (key != "static")
                {
                    m_warningMessages.push_back(std::format("Key: {} was still remaining after parsing.", key));    
                }
                iterateItemsAndWarn(val);
            }
        };
        
        iterateItemsAndWarn(json);
    }
} // namespace opentrackio
