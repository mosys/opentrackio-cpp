include(FindPackageHandleStandardArgs)

find_path(NLOHMANN_JSON_INCLUDE_DIR
        NAMES
        nlohmann/json.hpp
        PATHS
        "${CMAKE_CURRENT_SOURCE_DIR}/external"
        "${CMAKE_CURRENT_SOURCE_DIR}/../external")
mark_as_advanced(NLOHMANN_JSON_INCLUDE_DIR)

find_package_handle_standard_args(nlohmann_json
        REQUIRED_VARS NLOHMANN_JSON_INCLUDE_DIR)

if (nlohmann_json_FOUND)
    set(NLOHMANN_JSON_INCLUDE_DIRS ${NLOHMANN_JSON_INCLUDE_DIR})

    if (NOT TARGET nlohmann_json::nlohmann_json)
        add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
        set_target_properties(nlohmann_json::nlohmann_json PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${NLOHMANN_JSON_INCLUDE_DIRS}")
    endif ()
endif ()