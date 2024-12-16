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
#include <span>
#include <nlohmann/json.hpp>
#include "OpenTrackIOProperties.h"

namespace opentrackio
{
    #define OPEN_TRACK_IO_PROTOCOL_NAME "OpenTrackIO"
    #define OPEN_TRACK_IO_PROTOCOL_VERSION "0.9.0"
    struct OpenTrackIOSample
    {
        std::optional<opentrackioproperties::Camera> camera = std::nullopt;
        std::optional<opentrackioproperties::Duration> duration = std::nullopt;
        std::optional<opentrackioproperties::GlobalStage> globalStage = std::nullopt;
        std::optional<opentrackioproperties::Lens> lens = std::nullopt;
        std::optional<opentrackioproperties::Protocol> protocol = std::nullopt;
        std::optional<opentrackioproperties::RelatedSampleIds> relatedSampleIds = std::nullopt;
        std::optional<opentrackioproperties::SampleId> sampleId = std::nullopt;
        std::optional<opentrackioproperties::SourceId> sourceId = std::nullopt;
        std::optional<opentrackioproperties::SourceNumber> sourceNumber = std::nullopt;
        std::optional<opentrackioproperties::Timing> timing = std::nullopt;
        std::optional<opentrackioproperties::Tracker> tracker = std::nullopt;
        std::optional<opentrackioproperties::Transforms> transforms = std::nullopt;

        OpenTrackIOSample() = default;
        bool initialise(const nlohmann::json& json);
        bool initialise(const std::string_view jsonString);
        bool initialise(std::span<const uint8_t> cbor);
        const std::vector<std::string>& getErrors() { return m_errorMessages; };
        const std::vector<std::string>& getWarnings() { return m_warningMessages; };
        const nlohmann::json& getJson();
        
    private:
        void generateJson();
        void parseCameraToJson(nlohmann::json& baseJson);
        void parseDurationToJson(nlohmann::json& baseJson);
        void parseGlobalStageToJson(nlohmann::json& baseJson);
        void parseLensToJson(nlohmann::json& baseJson);
        void parseProtocolToJson(nlohmann::json& baseJson);
        void parseRelatedSampleIdsToJson(nlohmann::json& baseJson);
        void parseSampleIdToJson(nlohmann::json& baseJson);
        void parseSourceIdToJson(nlohmann::json& baseJson);
        void parseSourceNumberToJson(nlohmann::json& baseJson);
        void parseTimingToJson(nlohmann::json& baseJson);
        void parseTrackerToJson(nlohmann::json& baseJson);
        void parseTransformsToJson(nlohmann::json& baseJson);
        
        void warnForRemainingFields(const nlohmann::json& json);
        
        std::optional<nlohmann::json> m_json = std::nullopt;
        std::vector<std::string> m_errorMessages{};
        std::vector<std::string> m_warningMessages{};
    };
} // namespace opentrackio
