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
#include <nlohmann/json.hpp>

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
    
    class OpenTrackIOHelpers
    {
    public:
        static inline void clearFieldIfEmpty(nlohmann::json &json, std::string_view fieldStr)
        {
            if (json[fieldStr].is_object() && std::distance(json[fieldStr].items().begin(), json[fieldStr].items().end()) == 0)
            {
                json.erase(fieldStr);
            }
        }
        
        template<typename T>
        static inline bool checkTypeAndSetField(const nlohmann::json &jsonVal, T &field)
        {
            try
            {
                field = jsonVal.get<T>();
                return true;
            }
            catch (...)
            {
                // Reset field to default if failed.
                field = T{};
                return false;
            }
        }

        template<typename T>
        static inline bool checkTypeAndSetField(const nlohmann::json &jsonVal, std::optional<T> &field)
        {
            try
            {
                field = jsonVal.get<T>();
                return true;
            }
            catch (...)
            {
                // Reset field to default if failed.
                field = std::nullopt;
                return false;
            }
        }

        template<typename T>
        static inline bool iterateJsonArrayAndPopulateVector(const nlohmann::json &jsonVal, std::vector<T> &vec)
        {
            for (const auto &item: jsonVal.items())
            {
                T val;
                if (!checkTypeAndSetField(item.value(), val))
                {
                    return false;
                }
                vec.emplace_back(val);
            }

            return true;
        }

        template<typename T>
        static inline bool iterateJsonArrayAndPopulateVector(const nlohmann::json &jsonVal, std::optional<std::vector<T>> &vec)
        {
            std::vector<T> out;
            for (const auto &item: jsonVal.items())
            {
                T val;
                if (!checkTypeAndSetField(item.value(), val))
                {
                    return false;
                }
                out.emplace_back(val);
            }

            vec = std::move(out);
            return true;
        }

        template<typename T>
        static inline void assignField(nlohmann::json &json, std::string_view fieldStr, std::optional<T> &field,
                         std::string_view typeStr, std::vector<std::string> &errors)
        {
            if (json.contains(fieldStr))
            {
                if (!checkTypeAndSetField(json[fieldStr], field))
                {
                    errors.emplace_back(std::format("field: {} isn't of type: {}", fieldStr, typeStr));
                    field = std::nullopt;
                    return;
                }
                json.erase(fieldStr);
            }
        }
        
        template<Encoder T>
        static inline void assignField(nlohmann::json &json, std::string_view fieldStr, std::optional<T> &field,
                         std::string_view typeStr, std::vector<std::string> &errors)
        {
            if (!json.contains(fieldStr))
            {
                field = std::nullopt;
                return;
            }

            field = T{};
            auto &encoderJson = json[fieldStr];
            assignField(encoderJson, "focus", field->focus, typeStr, errors);
            assignField(encoderJson, "iris", field->iris, typeStr, errors);
            assignField(encoderJson, "zoom", field->zoom, typeStr, errors);

            if (!(field->focus.has_value() && field->iris.has_value() && field->zoom.has_value()))
            {
                field = std::nullopt;
                return;
            }

            json.erase(fieldStr);
        }

        static inline void assignRegexField(nlohmann::json &json, std::string_view fieldStr, std::optional<std::string> &field,
                              const std::regex &pattern, std::vector<std::string> &errors)
        {
            if (json.contains(fieldStr))
            {
                if (!checkTypeAndSetField(json[fieldStr], field))
                {
                    errors.emplace_back(std::format("field: {} isn't of type: string", fieldStr));
                    field = std::nullopt;
                    return;
                }
                if (std::smatch res; !std::regex_match(field.value(), res, pattern))
                {
                    errors.emplace_back(std::format("field: {} doesn't match the required pattern", fieldStr));
                    field = std::nullopt;
                    return;
                }
                json.erase(fieldStr);
            }
        }  
    };

    template<>
    inline void OpenTrackIOHelpers::assignField<std::vector<std::string>>(nlohmann::json &json, std::string_view fieldStr,
                                                 std::optional<std::vector<std::string>> &field,
                                                 std::string_view typeStr, std::vector<std::string> &errors)
    {
        if (!json.contains(fieldStr) || !json[fieldStr].is_array())
        {
            field = std::nullopt;
            return;
        }

        std::vector<std::string> vec{};
        if (!iterateJsonArrayAndPopulateVector(json[fieldStr], vec))
        {
            errors.emplace_back("field: {} had elements not of type: string");
            field = std::nullopt;
            return;
        }

        field = std::move(vec);
        json.erase(fieldStr);
    }
} // namespace opentrackio
