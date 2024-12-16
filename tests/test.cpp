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
#include <opentrackio-cpp/OpenTrackIOSample.h>
#include <span>


TEST_CASE("OpenTrackIOSample basic initialisation", "[init]")
{
    opentrackio::OpenTrackIOSample sample;
    REQUIRE_THROWS(sample.initialise(std::string_view("")));
    std::span<const uint8_t> cbor;
    REQUIRE_THROWS(sample.initialise(cbor));
    nlohmann::json j;
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
bool getStringExample(const std::string name, std::string& response)
{
    if (CURL* curl = curl_easy_init())
    {
        std::string url = "https://www.opentrackio.org/examples/" + name + ".json";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
        curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
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

// Fetch json from the OpenTrackIO HTTPS endpoint
bool getJsonExample(const std::string name, nlohmann::json& example)
{
    std::string response;
    if (!getStringExample(name, response))
    {
        return false;
    }
    
    return true;
}


TEST_CASE("OpenTrackIOSample example initialisation", "[init]")
{
    std::string response;
    REQUIRE(getStringExample("recommended_dynamic_example", response));
    opentrackio::OpenTrackIOSample stringSample;
    REQUIRE(stringSample.initialise(std::string_view(response)));
    REQUIRE(stringSample.protocol->name == OPEN_TRACK_IO_PROTOCOL_NAME);
    REQUIRE(stringSample.protocol->version == OPEN_TRACK_IO_PROTOCOL_VERSION);

    nlohmann::json example = nlohmann::json::parse(response);
    opentrackio::OpenTrackIOSample sample;
    REQUIRE(sample.initialise(example));
    REQUIRE(sample.getJson() == stringSample.getJson());
    REQUIRE(sample.getJson() == example);
    
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
    REQUIRE(sample.timing->timecode->format.dropFrame == true);
    REQUIRE(sample.timing->timecode->format.oddField == true);

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
