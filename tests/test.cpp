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

#include <catch2/catch_test_macros.hpp>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <opentrackio-cpp/OpenTrackIOSample.h>
#include <span>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

const std::string OPENTRACKIO_ROOT_URL = "https://www.opentrackio.org/";

TEST_CASE("OpenTrackIOSample basic initialisation", "[init]")
{
    opentrackio::OpenTrackIOSample sample;
    REQUIRE_THROWS(sample.initialise(std::string_view("")));
    std::span<const uint8_t> cbor;
    REQUIRE_THROWS(sample.initialise(cbor));
    json j;
    REQUIRE(sample.initialise(j));
    REQUIRE(sample.getErrors().size() == 0);
    REQUIRE(sample.getWarnings().size() == 0);
    REQUIRE(sample.getJson() == j);
}

//Convert curl out to string
size_t curlToString(char* ptr, size_t size, size_t nmemb, void* data)
{
    std::string* str = (std::string*)data;
    for (size_t x = 0; x < size * nmemb; ++x)
    {
        (*str) += ptr[x];
    }
    return size * nmemb;
}

// Fetch string from the OpenTrackIO HTTPS endpoint
bool getString(const std::string url, std::string& response)
{
    if (CURL* curl = curl_easy_init())
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
        curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
        response = "";
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            return false;
        }
        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}

// Fetch schema from the OpenTrackIO HTTPS endpoint
bool getStringSchema(std::string& response)
{
    std::string url = OPENTRACKIO_ROOT_URL + "schema.json";
    return getString(url, response);
}

// Fetch example from the OpenTrackIO HTTPS endpoint
bool getStringExample(const std::string name, std::string& response)
{
    std::string url = OPENTRACKIO_ROOT_URL + "examples/" + name + ".json";
    return getString(url, response);
}

void testSampleParse(const std::string response, opentrackio::OpenTrackIOSample& sample)
{
    opentrackio::OpenTrackIOSample stringSample;
    REQUIRE(stringSample.initialise(std::string_view(response)));
    REQUIRE(stringSample.protocol->name == OPEN_TRACK_IO_PROTOCOL_NAME);
    REQUIRE(stringSample.protocol->version == OPEN_TRACK_IO_PROTOCOL_VERSION);

    json example = json::parse(response);
    REQUIRE(sample.initialise(example));
    REQUIRE(sample.getJson() == stringSample.getJson());
    REQUIRE(sample.getJson() == example);
}

void testRecommendedDynamic(const std::string response)
{
    opentrackio::OpenTrackIOSample sample;
    testSampleParse(response, sample);

    REQUIRE(sample.lens->distortion->radial == std::vector<double>{1.0, 2.0, 3.0});
    REQUIRE(sample.lens->distortion->tangential == std::vector<double>{1.0, 2.0});
    REQUIRE(sample.lens->encoders->focus == 0.1);
    REQUIRE(sample.lens->encoders->iris == 0.2);
    REQUIRE(sample.lens->encoders->zoom == 0.3);
    REQUIRE(sample.lens->entrancePupilOffset == 0.123);
    REQUIRE(sample.lens->fStop == 4.0);
    REQUIRE(sample.lens->focalLength == 24.305);
    REQUIRE(sample.lens->focusDistance == 1000);
    REQUIRE(sample.lens->perspectiveShift->x == 0.1);
    REQUIRE(sample.lens->perspectiveShift->y == 0.2);

    REQUIRE(sample.protocol->name == OPEN_TRACK_IO_PROTOCOL_NAME);
    REQUIRE(sample.protocol->version == OPEN_TRACK_IO_PROTOCOL_VERSION);

    REQUIRE(sample.sampleId->id.substr(0, 9) == "urn:uuid:");
    REQUIRE(sample.sourceId->id.substr(0, 9) == "urn:uuid:");
    REQUIRE(sample.sourceNumber->value == 1);

    REQUIRE(sample.timing->frameRate->numerator == 24000);
    REQUIRE(sample.timing->frameRate->denominator == 1001);
    REQUIRE(sample.timing->mode == opentrackio::opentrackioproperties::Timing::Mode::EXTERNAL);
    REQUIRE(sample.timing->timecode->hours == 1);
    REQUIRE(sample.timing->timecode->minutes == 2);
    REQUIRE(sample.timing->timecode->seconds == 3);
    REQUIRE(sample.timing->timecode->frames == 4);
    REQUIRE(sample.timing->timecode->format.frameRate.numerator == 24000);
    REQUIRE(sample.timing->timecode->format.frameRate.denominator == 1001);
    REQUIRE(sample.timing->timecode->format.dropFrame);
    REQUIRE(sample.timing->timecode->format.oddField);

    REQUIRE(sample.tracker->notes == "Example generated sample.");
    REQUIRE(sample.tracker->recording == false);
    REQUIRE(sample.tracker->slate == "A101_A_4");
    REQUIRE(sample.tracker->status == "Optical Good");

    REQUIRE(sample.transforms->transforms.size() == 1);
    REQUIRE(sample.transforms->transforms[0].translation.x == 1.0);
    REQUIRE(sample.transforms->transforms[0].translation.y == 2.0);
    REQUIRE(sample.transforms->transforms[0].translation.z == 3.0);
    REQUIRE(sample.transforms->transforms[0].rotation.pan == 180.0);
    REQUIRE(sample.transforms->transforms[0].rotation.tilt == 90.0);
    REQUIRE(sample.transforms->transforms[0].rotation.roll == 45.0);
    REQUIRE(sample.transforms->transforms[0].transformId == "Camera");
}

void testRecommendedStatic(const std::string response)
{
    opentrackio::OpenTrackIOSample sample;
    testSampleParse(response, sample);

    REQUIRE(sample.camera->activeSensorPhysicalDimensions->height == 24.0);
    REQUIRE(sample.camera->activeSensorPhysicalDimensions->width == 36.0);
    REQUIRE(sample.camera->label == "A");
    REQUIRE(sample.lens->make == "LensMaker");
    REQUIRE(sample.lens->model == "Model15");
}

void testCompleteDynamic(const std::string response)
{
    opentrackio::OpenTrackIOSample sample;
    testSampleParse(response, sample);

    REQUIRE(sample.globalStage->e == 100.0);
    REQUIRE(sample.globalStage->n == 200.0);
    REQUIRE(sample.globalStage->u == 300.0);
    REQUIRE(sample.globalStage->lat0 == 100.0);
    REQUIRE(sample.globalStage->lon0 == 200.0);
    REQUIRE(sample.globalStage->h0 == 300.0);

    REQUIRE(sample.lens->custom->size() == 2);
    REQUIRE(sample.lens->custom == std::vector<double>{1.0, 2.0});
    REQUIRE(sample.lens->distortion->radial == std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
    REQUIRE(sample.lens->distortion->tangential == std::vector<double>{1.0, 2.0});
    REQUIRE(sample.lens->distortionOverscan == 1.0);
    REQUIRE(sample.lens->distortionShift->x == 1.0);
    REQUIRE(sample.lens->distortionShift->y == 2.0);
    REQUIRE(sample.lens->encoders->focus == 0.1);
    REQUIRE(sample.lens->encoders->iris == 0.2);
    REQUIRE(sample.lens->encoders->zoom == 0.3);
    REQUIRE(sample.lens->entrancePupilOffset == 0.123);
    REQUIRE(sample.lens->exposureFalloff->a1 == 1.0);
    REQUIRE(sample.lens->exposureFalloff->a2 == 2.0);
    REQUIRE(sample.lens->exposureFalloff->a3 == 3.0);
    REQUIRE(sample.lens->fStop == 4.0);
    REQUIRE(sample.lens->focalLength == 24.305);
    REQUIRE(sample.lens->focusDistance == 10.0);
    REQUIRE(sample.lens->perspectiveShift->x == 0.1);
    REQUIRE(sample.lens->perspectiveShift->y == 0.2);
    REQUIRE(sample.lens->rawEncoders->focus == 1000);
    REQUIRE(sample.lens->rawEncoders->iris == 2000);
    REQUIRE(sample.lens->rawEncoders->zoom == 3000);
    REQUIRE(sample.lens->tStop == 4.1);
    REQUIRE(sample.lens->undistortion->radial == std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
    REQUIRE(sample.lens->undistortion->tangential == std::vector<double>{1.0, 2.0});

    REQUIRE(sample.protocol->name == OPEN_TRACK_IO_PROTOCOL_NAME);
    REQUIRE(sample.protocol->version == OPEN_TRACK_IO_PROTOCOL_VERSION);
    REQUIRE(sample.relatedSampleIds->samples.size() == 2);
    REQUIRE(sample.relatedSampleIds->samples[0].substr(0, 9) == "urn:uuid:");
    REQUIRE(sample.relatedSampleIds->samples[1].substr(0, 9) == "urn:uuid:");
    REQUIRE(sample.sampleId->id.substr(0, 9) == "urn:uuid:");
    REQUIRE(sample.sourceId->id.substr(0, 9) == "urn:uuid:");
    REQUIRE(sample.sourceNumber->value == 1);

    REQUIRE(sample.timing->frameRate->numerator == 24000);
    REQUIRE(sample.timing->frameRate->denominator == 1001);
    REQUIRE(sample.timing->mode == opentrackio::opentrackioproperties::Timing::Mode::INTERNAL);
    REQUIRE(sample.timing->sampleTimestamp->seconds == 1718806554);
    REQUIRE(sample.timing->sampleTimestamp->nanoseconds == 500000000);
    REQUIRE(sample.timing->sampleTimestamp->attoseconds == 0);
    REQUIRE(sample.timing->sequenceNumber == 0);
    REQUIRE(sample.timing->synchronization->frequency.numerator == 24000);
    REQUIRE(sample.timing->synchronization->frequency.denominator == 1001);
    REQUIRE(sample.timing->synchronization->locked);
    REQUIRE(sample.timing->synchronization->source == opentrackio::opentrackioproperties::Timing::Synchronization::SourceType::PTP);
    REQUIRE(sample.timing->synchronization->offsets->translation == 1.0);
    REQUIRE(sample.timing->synchronization->offsets->rotation == 2.0);
    REQUIRE(sample.timing->synchronization->offsets->lensEncoders == 3.0);
    REQUIRE(sample.timing->synchronization->present);
    REQUIRE(sample.timing->synchronization->ptp->domain == 1);
    REQUIRE(sample.timing->synchronization->ptp->master == "00:11:22:33:44:55");
    REQUIRE(sample.timing->synchronization->ptp->offset == 0.0);
    REQUIRE(sample.timing->timecode->hours == 1);
    REQUIRE(sample.timing->timecode->minutes == 2);
    REQUIRE(sample.timing->timecode->seconds == 3);
    REQUIRE(sample.timing->timecode->frames == 4);
    REQUIRE(sample.timing->timecode->format.frameRate.numerator == 24000);
    REQUIRE(sample.timing->timecode->format.frameRate.denominator == 1001);
    REQUIRE(sample.timing->timecode->format.dropFrame);
    REQUIRE(sample.timing->timecode->format.oddField);

    REQUIRE(sample.tracker->notes == "Example generated sample.");
    REQUIRE(sample.tracker->recording == false);
    REQUIRE(sample.tracker->slate == "A101_A_4");
    REQUIRE(sample.tracker->status == "Optical Good");

    REQUIRE(sample.transforms->transforms.size() == 3);
    REQUIRE(sample.transforms->transforms[0].translation.x == 1.0);
    REQUIRE(sample.transforms->transforms[0].translation.y == 2.0);
    REQUIRE(sample.transforms->transforms[0].translation.z == 3.0);
    REQUIRE(sample.transforms->transforms[0].rotation.pan == 180.0);
    REQUIRE(sample.transforms->transforms[0].rotation.tilt == 90.0);
    REQUIRE(sample.transforms->transforms[0].rotation.roll == 45.0);
    REQUIRE(sample.transforms->transforms[0].transformId == "Dolly");
    REQUIRE(sample.transforms->transforms[1].translation.x == 1.0);
    REQUIRE(sample.transforms->transforms[1].translation.y == 2.0);
    REQUIRE(sample.transforms->transforms[1].translation.z == 3.0);
    REQUIRE(sample.transforms->transforms[1].rotation.pan == 180.0);
    REQUIRE(sample.transforms->transforms[1].rotation.tilt == 90.0);
    REQUIRE(sample.transforms->transforms[1].rotation.roll == 45.0);
    REQUIRE(sample.transforms->transforms[1].scale->x == 1.0);
    REQUIRE(sample.transforms->transforms[1].scale->y == 2.0);
    REQUIRE(sample.transforms->transforms[1].scale->z == 3.0);
    REQUIRE(sample.transforms->transforms[1].transformId == "Crane Arm");
    REQUIRE(sample.transforms->transforms[1].parentTransformId == "Dolly");
    REQUIRE(sample.transforms->transforms[2].translation.x == 1.0);
    REQUIRE(sample.transforms->transforms[2].translation.y == 2.0);
    REQUIRE(sample.transforms->transforms[2].translation.z == 3.0);
    REQUIRE(sample.transforms->transforms[2].rotation.pan == 180.0);
    REQUIRE(sample.transforms->transforms[2].rotation.tilt == 90.0);
    REQUIRE(sample.transforms->transforms[2].rotation.roll == 45.0);
    REQUIRE(sample.transforms->transforms[2].scale->x == 1.0);
    REQUIRE(sample.transforms->transforms[2].scale->y == 2.0);
    REQUIRE(sample.transforms->transforms[2].scale->z == 3.0);
    REQUIRE(sample.transforms->transforms[2].transformId == "Camera");
    REQUIRE(sample.transforms->transforms[2].parentTransformId == "Crane Arm");
}

void testCompleteStatic(const std::string response)
{
    opentrackio::OpenTrackIOSample sample;
    testSampleParse(response, sample);

    REQUIRE(sample.camera->activeSensorResolution->height == 2160);
    REQUIRE(sample.camera->activeSensorResolution->width == 3840);
    REQUIRE(sample.camera->anamorphicSqueeze->numerator == 1);
    REQUIRE(sample.camera->anamorphicSqueeze->denominator == 1);
    REQUIRE(sample.camera->firmwareVersion == "1.2.3");
    REQUIRE(sample.camera->make == "CameraMaker");
    REQUIRE(sample.camera->model == "Model20");
    REQUIRE(sample.camera->serialNumber == "1234567890A");
    REQUIRE(sample.camera->captureFrameRate->numerator == 24000);
    REQUIRE(sample.camera->captureFrameRate->denominator == 1001);
    REQUIRE(sample.camera->fdlLink->substr(0, 9) == "urn:uuid:");
    REQUIRE(sample.camera->isoSpeed == 4000);
    REQUIRE(sample.camera->shutterAngle == 45.0);

    REQUIRE(sample.duration->rational.numerator == 1);
    REQUIRE(sample.duration->rational.denominator == 25);

    REQUIRE(sample.lens->distortionOverscanMax == 1.2);
    REQUIRE(sample.lens->nominalFocalLength == 14);
    REQUIRE(sample.lens->serialNumber == "1234567890A");

    REQUIRE(sample.tracker->firmwareVersion == "1.2.3");
    REQUIRE(sample.tracker->make == "TrackerMaker");
    REQUIRE(sample.tracker->model == "Tracker");
    REQUIRE(sample.tracker->serialNumber == "1234567890A");
}

TEST_CASE("OpenTrackIOSample example initialisation", "[init]")
{
    std::string response;
    REQUIRE(getStringExample("recommended_dynamic_example", response));
    testRecommendedDynamic(response);

    REQUIRE(getStringExample("recommended_static_example", response));
    testRecommendedDynamic(response);
    testRecommendedStatic(response);

    REQUIRE(getStringExample("complete_dynamic_example", response));
    testCompleteDynamic(response);

    REQUIRE(getStringExample("complete_static_example", response));
    testCompleteDynamic(response);
    testRecommendedStatic(response);
    testCompleteStatic(response);
}

TEST_CASE("OpenTrackIOSamples validate against the published schema", "[validate]")
{
    std::string response;
    REQUIRE(getStringSchema(response));
    json schema = json::parse(response);
    json_validator validator;
    REQUIRE_NOTHROW(validator.set_root_schema(schema));

    for (auto name : {
        "recommended_dynamic_example",
        "recommended_static_example",
        "complete_dynamic_example",
        "complete_static_example",
        })
    {
        REQUIRE(getStringExample(name, response));
        json example = json::parse(response);
        REQUIRE_NOTHROW(validator.validate(example));

        opentrackio::OpenTrackIOSample sample;
        sample.initialise(example);
        json output = sample.getJson();
        REQUIRE_NOTHROW(validator.validate(output));
    }
}
