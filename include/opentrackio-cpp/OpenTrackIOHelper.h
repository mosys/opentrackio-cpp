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

#pragma once
#include <format>
#include <regex>
#include <string>
#include <type_traits>
#include <nlohmann/json.hpp>

#define TYPE_COVERSION_ASSERT(JsonConcept, CppType) \
    static_assert(JsonConcept<CppType>, "Type is not compatible!");

#define TYPE_NON_COVERSION_ASSERT(JsonConcept, CppType) \
    static_assert(!JsonConcept<CppType>, "Type is unexpectedly compatible!");

namespace opentrackio
{
    template<typename T>
    concept Encoder =
    requires(T t)
    {
        { t.focus } -> std::convertible_to<std::optional<double>>;
        { t.iris } -> std::convertible_to<std::optional<double>>;
        { t.zoom } -> std::convertible_to<std::optional<double>>;
    } ||
    requires(T t)
    {
        { t.focus } -> std::convertible_to<std::optional<uint16_t>>;
        { t.iris } -> std::convertible_to<std::optional<uint16_t>>;
        { t.zoom } -> std::convertible_to<std::optional<uint16_t>>;
    };

    template<typename T>
    concept JsonBool = std::is_same_v<T, bool>;
    template<typename T>
    concept JsonString = std::is_same_v<T, std::string>;
    template<typename T>
    concept JsonNumber = std::is_integral_v<T> && !JsonBool<T>;
    template<typename T>
    concept JsonFloatDouble = std::is_floating_point_v<T> && !JsonNumber<T>;

    // Basic type conversion checks to ensure compatability with json library.
    TYPE_COVERSION_ASSERT(JsonBool, bool);
    TYPE_NON_COVERSION_ASSERT(JsonBool, int);
    TYPE_NON_COVERSION_ASSERT(JsonBool, short);
    TYPE_NON_COVERSION_ASSERT(JsonBool, long);

    TYPE_COVERSION_ASSERT(JsonString, std::string);
    TYPE_NON_COVERSION_ASSERT(JsonString, const char*);

    TYPE_COVERSION_ASSERT(JsonNumber, int);
    TYPE_COVERSION_ASSERT(JsonNumber, long);
    TYPE_COVERSION_ASSERT(JsonNumber, short);
    TYPE_NON_COVERSION_ASSERT(JsonNumber, float);
    TYPE_NON_COVERSION_ASSERT(JsonNumber, double);

    TYPE_COVERSION_ASSERT(JsonFloatDouble, float);
    TYPE_COVERSION_ASSERT(JsonFloatDouble, double);
    TYPE_NON_COVERSION_ASSERT(JsonFloatDouble, int);
    TYPE_NON_COVERSION_ASSERT(JsonFloatDouble, long);
    TYPE_NON_COVERSION_ASSERT(JsonFloatDouble, short);
    
    class OpenTrackIOHelpers
    {
    public:
        static void clearFieldIfEmpty(nlohmann::json &json, std::string_view fieldStr)
        {
            if (json[fieldStr].is_object() && std::distance(json[fieldStr].items().begin(), json[fieldStr].items().end()) == 0)
            {
                json.erase(fieldStr);
            }
        }
        
        template<typename T>
        static void getFieldFromJson(const nlohmann::json &jsonVal, T &outField) noexcept
        {
                outField = jsonVal.get<T>();
        }

        template<typename T>
        static void getFieldFromJson(const nlohmann::json &jsonVal, std::optional<T> &field) noexcept
        {
                field = jsonVal.get<T>();
        }

        template<typename T>
        static void iterateJsonArrayAndPopulateVector(const nlohmann::json &jsonVal, std::vector<T> &vec)
        {
            for (const auto &item: jsonVal.items())
            {
                T val;
                getFieldFromJson(item.value(), val);
                vec.emplace_back(val);
            }
        }

        template<typename T>
        static void iterateJsonArrayAndPopulateVector(const nlohmann::json &jsonVal, std::optional<std::vector<T>> &vec)
        {
            std::vector<T> out;
            for (const auto &item: jsonVal.items())
            {
                T val;
                getFieldFromJson(item.value(), val);
                out.emplace_back(val);
            }

            vec = std::move(out);
        }

        template<typename T>
        static void assignField(nlohmann::json &json, std::string_view fieldStr, std::optional<T> &field,
                         std::vector<std::string> &errors)
        {
            if (json.contains(fieldStr))
            {
                if (!checkJsonTypeMatch<T>(json[fieldStr], fieldStr, errors))
                    return;

                getFieldFromJson(json[fieldStr], field);
                json.erase(fieldStr);
            }
        }

        template<typename T>
        static void assignFieldArray(nlohmann::json& json, std::string_view fieldStr, std::optional<std::vector<T>>& field,
            std::vector<std::string>& errors)
        {
            if (json.contains(fieldStr))
            {
                if (!json[fieldStr].is_array())
                {
                    errors.emplace_back(std::format("field: {0} isn't an array:", fieldStr));
                    return;
                }

                for (const auto& jsonValue : json[fieldStr])
                {
                    if (!checkJsonTypeMatch<T>(jsonValue, fieldStr, errors))
                        return;
                }

                getFieldFromJson(json[fieldStr], field);
                json.erase(fieldStr);
            }
        }
        
        template<Encoder T>
        static void assignField(nlohmann::json &json, std::string_view fieldStr, std::optional<T> &field,
                         std::vector<std::string> &errors)
        {
            if (!json.contains(fieldStr))
            {
                field = std::nullopt;
                return;
            }

            field = T{};
            auto &encoderJson = json[fieldStr];
            assignField(encoderJson, "focus", field->focus, errors);
            assignField(encoderJson, "iris", field->iris, errors);
            assignField(encoderJson, "zoom", field->zoom, errors);

            if (!(field->focus.has_value() && field->iris.has_value() && field->zoom.has_value()))
            {
                field = std::nullopt;
                return;
            }

            json.erase(fieldStr);
        }

        static void assignRegexField(nlohmann::json &json, std::string_view fieldStr, std::optional<std::string> &field,
                              const std::regex &pattern, std::vector<std::string> &errors)
        {
            if (json.contains(fieldStr))
            {
                if (!json[fieldStr].is_string())
                {
                    errors.emplace_back(std::format("field: {} isn't of type: string", fieldStr));
                    field = std::nullopt;
                    return;
                }

                getFieldFromJson(json[fieldStr], field);

                if (std::smatch res; !std::regex_match(field.value(), res, pattern))
                {
                    errors.emplace_back(std::format("field: {} doesn't match the required pattern", fieldStr));
                    field = std::nullopt;
                    return;
                }
                json.erase(fieldStr);
            }
        }  

        static inline void assignStringArray(nlohmann::json& json, std::string_view fieldStr,
            std::optional<std::vector<std::string>>& field,
            std::vector<std::string>& errors)
        {
            if (!json.contains(fieldStr) || !json[fieldStr].is_array())
            {
                field = std::nullopt;
                return;
            }

            std::vector<std::string> vec{};
            iterateJsonArrayAndPopulateVector<std::string>(json[fieldStr], vec);

            field = std::move(vec);
            json.erase(fieldStr);
        }

        private:
            template<typename FieldT>
            static inline constexpr nlohmann::json::value_t getJsonTypeValue() 
                requires JsonBool<FieldT> || JsonFloatDouble<FieldT> || JsonNumber<FieldT> || JsonString<FieldT>
            {
                if constexpr (JsonString<FieldT>) { return nlohmann::json::value_t::string; }
                else if constexpr (JsonBool<FieldT>) { return nlohmann::json::value_t::boolean; }
                else if constexpr (JsonFloatDouble<FieldT>) { return nlohmann::json::value_t::number_float; }
                else if constexpr (JsonNumber<FieldT>) { return nlohmann::json::value_t::number_unsigned; }
            }

            template<typename FieldT>
            static inline bool constexpr checkJsonTypeMatch(const nlohmann::json& json, const std::string_view& fieldStr, std::vector<std::string>& errors)
            {
                nlohmann::json::value_t cppToJsonType = getJsonTypeValue<FieldT>();
                if (json.type() != cppToJsonType)
                {
                    errors.emplace_back(std::format("field: {0} of type {1} isn't compatible with {2}.", fieldStr, json.type_name(), typeid(FieldT).name()));
                    return false;
                }

                return true;
            }

    };
} // namespace opentrackio
