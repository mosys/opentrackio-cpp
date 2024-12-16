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
#include <nlohmann/json.hpp>
#include <span>
#include <opentrackio-cpp/OpenTrackIOSample.h>

TEST_CASE("OpenTrackIOSample basic initialisation", "[init]") {
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

TEST_CASE("OpenTrackIOSample example initialisation", "[init]") {
    // TODO curl the example
}
