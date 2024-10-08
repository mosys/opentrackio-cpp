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

#include "opentrackio-cpp/OpenTrackIOSample.h"
#include <numbers>
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

    template<>
    void assignJson(nlohmann::json &json, std::string_view field, const std::optional<opentrackiotypes::Dimensions> &value)
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
            json[field]["attoseconds"] = value->attoseconds;
        }
    };
    
    bool OpenTrackIOSample::initialise(const nlohmann::json &json)
    {
        m_json = json;
        camera = opentrackioproperties::Camera::parse(json, m_errorMessages);
        device = opentrackioproperties::Device::parse(json, m_errorMessages);
        duration = opentrackioproperties::Duration::parse(json, m_errorMessages);
        globalStage = opentrackioproperties::GlobalStage::parse(json, m_errorMessages);
        lens = opentrackioproperties::Lens::parse(json, m_errorMessages);
        protocol = opentrackioproperties::Protocol::parse(json, m_errorMessages);
        relatedSamples = opentrackioproperties::RelatedSamples::parse(json, m_errorMessages);
        sampleId = opentrackioproperties::SampleId::parse(json, m_errorMessages);
        timing = opentrackioproperties::Timing::parse(json, m_errorMessages);
        transforms = opentrackioproperties::Transforms::parse(json, m_errorMessages);
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
        
        if (duration.has_value())
        {
            j["duration"]["num"] = duration->rational.numerator;
            j["duration"]["denom"] = duration->rational.denominator;
        }
        
        if (protocol.has_value())
        {
            j["protocol"] = protocol->version;
        }
        
        if (relatedSamples.has_value())
        {
            j["relatedSamples"] = relatedSamples->samples;
        }
        
        if (sampleId.has_value())
        {
            j["sampleId"] = sampleId->id;
        }

        parseCameraToJson(j);
        parseDeviceToJson(j);
        parseLensToJson(j);
        parseGlobalStageToJson(j);
        parseTimingToJson(j);
        parseTransformsToJson(j);
        
        m_json = j;
    }

    void OpenTrackIOSample::parseCameraToJson(nlohmann::json& baseJson)
    {
        if (!camera.has_value())
        {
            return;
        }
        
        auto& cameraJson = baseJson["camera"];
        assignJson(cameraJson, "activeSensorPhysicalDimensions", camera->activeSensorPhysicalDimensions);
        assignJson(cameraJson, "activeSensorResolution", camera->activeSensorResolution);
        assignJson(cameraJson, "captureRate", camera->captureRate);
        assignJson(cameraJson, "anamorphicSqueeze", camera->anamorphicSqueeze);
        assignJson(cameraJson, "firmwareVersion", camera->firmwareVersion);
        assignJson(cameraJson, "id", camera->id);
        assignJson(cameraJson, "make", camera->make);
        assignJson(cameraJson, "model", camera->model);
        assignJson(cameraJson, "serialNumber", camera->serialNumber);
        assignJson(cameraJson, "fdlLink", camera->fdlLink);
        assignJson(cameraJson, "isoSpeed", camera->isoSpeed);
        assignJson(cameraJson, "shutterAngle", camera->shutterAngle);
    }

    void OpenTrackIOSample::parseDeviceToJson(nlohmann::json& baseJson)
    {
        if (!device.has_value())
        {
            return;
        }
        
        assignJson(baseJson["device"], "firmwareVersion", device->firmwareVersion);
        assignJson(baseJson["device"], "make", device->make);
        assignJson(baseJson["device"], "model", device->model);
        assignJson(baseJson["device"], "notes", device->notes);
        assignJson(baseJson["device"], "recording", device->recording);
        assignJson(baseJson["device"], "serialNumber", device->serialNumber);
        assignJson(baseJson["device"], "slate", device->slate);
        assignJson(baseJson["device"], "status", device->status);
    }

    void OpenTrackIOSample::parseLensToJson(nlohmann::json& baseJson)
    {
        if (!lens.has_value())
        {
            return;
        }
        
        if (lens->distortion.has_value())
        {
            baseJson["lens"]["distortion"]["radial"] = lens->distortion->radial;
            assignJson(baseJson["lens"]["distortion"], "tangential", lens->distortion->tangential);
        }

        if (lens->undistortion.has_value())
        {
            baseJson["lens"]["undistortion"]["radial"] = lens->undistortion->radial;
            assignJson(baseJson["lens"]["undistortion"], "tangential", lens->undistortion->tangential);
        }

        if (lens->distortionShift.has_value())
        {
            baseJson["lens"]["distortionShift"]["Cx"] = lens->distortionShift->x;
            baseJson["lens"]["distortionShift"]["Cy"] = lens->distortionShift->y;
        }

        if (lens->perspectiveShift.has_value())
        {
            baseJson["lens"]["perspectiveShift"]["Px"] = lens->perspectiveShift->x;
            baseJson["lens"]["perspectiveShift"]["Py"] = lens->perspectiveShift->y;
        }

        if (lens->entrancePupilDistance.has_value())
        {
            baseJson["lens"]["entrancePupilDistance"]["num"] = lens->entrancePupilDistance->numerator;
            baseJson["lens"]["entrancePupilDistance"]["denom"] = lens->entrancePupilDistance->denominator;
        }

        if (lens->encoders.has_value())
        {
            assignJson(baseJson["lens"]["encoders"], "focus", lens->encoders->focus);
            assignJson(baseJson["lens"]["encoders"], "iris", lens->encoders->iris);
            assignJson(baseJson["lens"]["encoders"], "zoom", lens->encoders->zoom);
        }

        if (lens->rawEncoders.has_value())
        {
            assignJson(baseJson["lens"]["rawEncoders"], "focus", lens->rawEncoders->focus);
            assignJson(baseJson["lens"]["rawEncoders"], "iris", lens->rawEncoders->iris);
            assignJson(baseJson["lens"]["rawEncoders"], "zoom", lens->rawEncoders->zoom);
        }

        if (lens->exposureFalloff.has_value())
        {
            baseJson["lens"]["exposureFalloff"]["a1"] = lens->exposureFalloff->a1;
            assignJson(baseJson["lens"]["exposureFalloff"], "a2", lens->exposureFalloff->a2);
            assignJson(baseJson["lens"]["exposureFalloff"], "a3", lens->exposureFalloff->a3);
        }

        assignJson(baseJson["lens"], "custom", lens->custom);
        assignJson(baseJson["lens"], "distortionOverscan", lens->distortionOverscan);
        assignJson(baseJson["lens"], "distortionScale", lens->distortionScale);
        assignJson(baseJson["lens"], "fStop", lens->fStop);
        assignJson(baseJson["lens"], "tStop", lens->tStop);
        assignJson(baseJson["lens"], "firmwareVersion", lens->firmwareVersion);
        assignJson(baseJson["lens"], "focalLength", lens->focalLength);
        assignJson(baseJson["lens"], "focusDistance", lens->focusDistance);
        assignJson(baseJson["lens"], "make", lens->make);
        assignJson(baseJson["lens"], "model", lens->model);
        assignJson(baseJson["lens"], "nominalFocalLength", lens->nominalFocalLength);
        assignJson(baseJson["lens"], "serialNumber", lens->serialNumber);
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

    void OpenTrackIOSample::parseTimingToJson(nlohmann::json& baseJson)
    {
        if (!timing.has_value())
        {
            return;
        }
        
        assignJson(baseJson["timing"], "frameRate", timing->frameRate);
        if (timing->mode.has_value())
        {
            baseJson["timing"]["mode"] = timing->mode.value() == opentrackioproperties::Timing::Mode::INTERNAL ?
                                  "internal" : "external";
        }
        assignJson(baseJson["timing"], "recordedTimestamp", timing->recordedTimestamp);
        assignJson(baseJson["timing"], "sampleTimestamp", timing->sampleTimestamp);
        assignJson(baseJson["timing"], "sequenceNumber", timing->sequenceNumber);

        if (timing->timecode.has_value())
        {
            baseJson["timing"]["timecode"]["hours"] = timing->timecode->hours;
            baseJson["timing"]["timecode"]["minutes"] = timing->timecode->minutes;
            baseJson["timing"]["timecode"]["seconds"] = timing->timecode->seconds;
            baseJson["timing"]["timecode"]["frames"] = timing->timecode->frames;
            baseJson["timing"]["timecode"]["format"]["dropFrame"] = timing->timecode->dropFrame;
            baseJson["timing"]["timecode"]["format"]["frameRate"]["num"] = timing->timecode->frameRate.numerator;
            baseJson["timing"]["timecode"]["format"]["frameRate"]["denom"] = timing->timecode->frameRate.denominator;
        }

        if (timing->synchronization.has_value())
        {
            auto& syncJson = baseJson["timing"]["synchronization"];
            switch(timing->synchronization->source)
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
            baseJson["timing"]["synchronization"]["frequency"]["num"] = timing->synchronization->frequency.numerator;
            baseJson["timing"]["synchronization"]["frequency"]["denom"] = timing->synchronization->frequency.denominator;
            baseJson["timing"]["synchronization"]["locked"] = timing->synchronization->locked;
            assignJson(baseJson["timing"]["synchronization"], "present", timing->synchronization->present);
            assignJson(baseJson["timing"]["synchronization"], "ptpMaster", timing->synchronization->ptpMaster);
            assignJson(baseJson["timing"]["synchronization"], "ptpOffset", timing->synchronization->ptpOffset);
            assignJson(baseJson["timing"]["synchronization"], "ptpDomain", timing->synchronization->ptpDomain);

            if (timing->synchronization->offsets.has_value())
            {
                assignJson(baseJson["timing"]["synchronization"]["offsets"], "translation",
                           timing->synchronization->offsets->translation);
                assignJson(baseJson["timing"]["synchronization"]["offsets"], "rotation",
                           timing->synchronization->offsets->rotation);
                assignJson(baseJson["timing"]["synchronization"]["offsets"], "encoders",
                           timing->synchronization->offsets->encoders);
            }
        }
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
            assignJson(tfJson, "name", tf.name);
            assignJson(tfJson, "parent", tf.parent);
            if (tf.scale.has_value())
            {
                tfJson["scale"] = {{"x", tf.scale->x}, {"y", tf.scale->y}, {"z", tf.scale->z}};
            }
            
            baseJson["transforms"].push_back(tfJson);
        }
    }
} // namespace opentrackio
