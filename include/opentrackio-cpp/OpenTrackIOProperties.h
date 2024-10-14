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
#include <vector>
#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "OpenTrackIOTypes.h"

namespace opentrackio::opentrackioproperties
{
    struct Camera
    {
        /**
         *  Active area of the camera sensor.
         *  Units: Microns */
        std::optional<opentrackiotypes::Dimensions> activeSensorPhysicalDimensions = std::nullopt;

        /**
         * Photosite resolution of the active area of the camera sensor.
         *  Units: Pixels */
        std::optional<opentrackiotypes::Dimensions> activeSensorResolution = std::nullopt;

        /**
         * Nominal ratio of height to width of the image of an axis-aligned square
         * captured by the camera sensor. It can be used to de-squeeze images but is
         * not however an exact number over the entire captured area due to a lens'
         * intrinsic analog nature.
         * Units: 0.01 unit */
        std::optional<uint32_t> anamorphicSqueeze = std::nullopt;

        /**
         * Version identifier for the firmware of the camera. */
        std::optional<std::string> firmwareVersion = std::nullopt;

        /**
         * Free string that identifies the camera. */
        std::optional<std::string> id = std::nullopt;

        /**
         * Make of the camera. */
        std::optional<std::string> make = std::nullopt;

        /**
         * Model of the camera. */
        std::optional<std::string> model = std::nullopt;

        /**
         * Unique identifier of the camera.*/
        std::optional<std::string> serialNumber = std::nullopt;

        /**
         * Capture frame rate of the camera
         *  Units: Hertz */
        std::optional<opentrackiotypes::Rational> captureRate = std::nullopt;

        /**
         * Unique identifier of the FDL used by the camera
         * Pattern: ^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$ */
        std::optional<std::string> fdlLink = std::nullopt;

        /**
         * Arithmetic ISO scale as defined in ISO 12232 */
        std::optional<uint32_t> isoSpeed = std::nullopt;

        /**
         * Shutter speed as a fraction of the captured frame rate. The shutter speed (in units of 1/s)
         * is equal to the value of the parameter divided by 360 times the capture frame rate. */
        std::optional<uint32_t> shutterAngle = std::nullopt;

        static std::optional<Camera> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct Device
    {
        /**
         * Version identifier for the firmware of the device producing the data */
        std::optional<std::string> firmwareVersion = std::nullopt;

        /**
        * Make of the device producing data. */
        std::optional<std::string> make = std::nullopt;

        /**
         * Model of the device producing data. */
        std::optional<std::string> model = std::nullopt;

        /**
         * Free string for notes about tracking */
        std::optional<std::string> notes = std::nullopt;

        /**
         * True if the system is recording data - e.g. tracking data */
        std::optional<bool> recording = std::nullopt;

        /**
         * Unique identifier of the device producing data.*/
        std::optional<std::string> serialNumber = std::nullopt;

        /**
         * Free string that describes the recording slate - e.g. 'A101_A_4' */
        std::optional<std::string> slate = std::nullopt;

        /**
         * Free string that describes the status of the system
         * e.g. 'Optical Good' in a tracking system. */
        std::optional<std::string> status = std::nullopt;

        static std::optional<Device> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    /** Duration of the clip.
     * Unit: Seconds */
    struct Duration
    {
        opentrackiotypes::Rational rational{};
        
        static std::optional<Duration> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    /**
     * Position of the stage origin in global ENU and geodetic coordinates
     * (E, N, U, lat0, lon0, h0). Note this may be dynamic e.g. if the stage is inside
     * a moving vehicle. */
    struct GlobalStage
    {
        double e;
        double n;
        double u;
        double lat0;
        double lon0;
        double h0;

        static std::optional<GlobalStage> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct Lens
    {
        /**
         * TODO: Until the OpenLensIO model is finalised, this list provides custom
         * coefficients for a particular lens model e.g. undistortion, anamorphic etc. */
        std::optional<std::vector<double>> custom = std::nullopt;

        /**
         * Coefficients for calculating the distortion characteristics of a lens
         * comprising radial distortion coefficients of the spherical distortion (k1-N)
         * and the tangential distortion (p1-N). */
        struct Distortion
        {
            std::vector<double> radial{};
            std::optional<std::vector<double>> tangential = std::nullopt;            
        };
        std::optional<Distortion> distortion = std::nullopt;

        /**
         * Coefficients for calculating the undistortion characteristics of a lens comprising radial distortion
         * coefficients of the spherical distortion (k1-N) and the tangential distortion (p1-N) */
        struct Undistortion
        {
            std::vector<double> radial{};
            std::optional<std::vector<double>> tangential = std::nullopt;
        };
        std::optional<Undistortion> undistortion = std::nullopt;       

        /**
         * Overscan factor on lens distortion */
        std::optional<double> distortionOverscan = std::nullopt;

        /**
         * Scaling factor on field-of-view for tweaking lens calibrations */
        std::optional<double> distortionScale = std::nullopt;

        /**
         * Shift in X and Y of the centre of distortion of the virtual camera
         * Units: millimeters */
        struct DistortionShift
        {
            double x;
            double y;
        };
        std::optional<DistortionShift> distortionShift = std::nullopt;

        /**
         * Normalised real numbers (0-1) for focus, iris and zoom.
         * Encoders are represented in this way (as opposed to raw integer values) to ensure values remain
         * independent of encoder resolution, minimum and maximum (at an acceptable loss of precision).
         * These values are only relevant in lenses with end-stops that demark the 0 and 1 range. */
        struct Encoders
        {
            std::optional<double> focus = std::nullopt;
            std::optional<double> iris = std::nullopt;
            std::optional<double> zoom = std::nullopt;            
        };
        std::optional<Encoders> encoders = std::nullopt;

        /**
         * Raw encoder values for focus, iris and zoom. These values are dependent on encoder resolution and
         * before any homing / ranging has taken place. */
        struct RawEncoders
        {
            std::optional<uint16_t> focus = std::nullopt;
            std::optional<uint16_t> iris = std::nullopt;
            std::optional<uint16_t> zoom = std::nullopt;            
        };
        std::optional<RawEncoders> rawEncoders = std::nullopt;
        
        /**
         * Position of the entrance pupil relative to the nominal imaging plane
         * (positive if the entrance pupil is located on the side of the nominal imaging plane
         * that is towards the object, and negative otherwise) */
        std::optional<opentrackiotypes::Rational> entrancePupilDistance = std::nullopt;

        /**
         * Coefficients for calculating the exposure fall-off (vignetting) of a lens. */
        struct ExposureFalloff
        {
            double a1;
            std::optional<double> a2 = std::nullopt;
            std::optional<double> a3 = std::nullopt;            
        };
        std::optional<ExposureFalloff> exposureFalloff = std::nullopt;

        /**
         * The linear f-number of the lens, equal to the focal length divided by the diameter of the entrance pupil. */
        std::optional<uint32_t> fStop = std::nullopt;
     
        /**
         * Version identifier for the firmware of the lens */
        std::optional<std::string> firmwareVersion = std::nullopt;

        /**
         * Focal length of the lens
         * Units: millimeter */
        std::optional<double> focalLength = std::nullopt;

        /**
         * Focus distance / position of the lens
         * Units: millimeter */
        std::optional<uint32_t> focusDistance = std::nullopt;

        /**
        * Make of the lens. */
        std::optional<std::string> make = std::nullopt;

        /**
         * Model of the lens. */
        std::optional<std::string> model = std::nullopt;

        /**
         * Unique identifier of the lens.*/
        std::optional<std::string> serialNumber = std::nullopt;        

        /**
         * Nominal focal length of the lens.
         * The number printed on the side of a prime lens, e.g. 50 mm, and undefined in the case of a zoom lens. */
        std::optional<uint32_t> nominalFocalLength = std::nullopt;
     
        /**
         * Shift in X and Y of the centre of perspective projection of the virtual camera
         * Units: millimeters */
         struct PerspectiveShift
         {
             double x;
             double y;
         };
        std::optional<PerspectiveShift> perspectiveShift = std::nullopt;

        /**
         * The linear t-number of the lens, equal to the F-number of the lens divided by the square root of the
         * transmittance of the lens.
         * Units: 0.001 unit */
        std::optional<uint32_t> tStop = std::nullopt;

        static std::optional<Lens> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };
    
    struct Protocol
    {
        /**
         * Free string that describes the version of the protocol that this sample employs. */
        std::string version;

        static std::optional<Protocol> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct RelatedSamples
    {
        /**
         * List of sample unique IDs that are related to this sample.
         * E.g. a related performance capture sample or a sample of static data from the same device.
         * The existence of the related sample should not be relied upon.
         * Pattern: ^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$ */
        std::vector<std::string> samples;

        static std::optional<RelatedSamples> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct SampleId
    {
        /**
         * Unique identifier of the sample in which data is being transported.
         * Pattern: ^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$ */
        std::string id;

        static std::optional<SampleId> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct Timing
    {
        /**
         * TODO: docs are todo on the schema */
        std::optional<opentrackiotypes::Rational> frameRate = std::nullopt;
         
        /**
         * 'external' timing mode describes the case where the transport packet has inherent timing,
         * so no explicit timing data is required in the data).
         * 'internal' mode indicates the transport packet does not have inherent timing,
         * so a PTP timestamp must be provided. */
        enum class Mode
        {
            EXTERNAL,
            INTERNAL
        };
        std::optional<Mode> mode = std::nullopt;

        /**
         * PTP timestamp at which the data was recorded. Provided for conveience during playback of e.g. pre-recorded
         * tracking data. */
        std::optional<opentrackiotypes::Timestamp> recordedTimestamp = std::nullopt;
        
        /**
         * PTP timestamp of the data capture instant. Note this may differ from the packet's transmission PTP timestamp. */
        std::optional<opentrackiotypes::Timestamp> sampleTimestamp = std::nullopt;
        
        /**
         * TODO: Schema doesnt have description */
        std::optional<uint16_t> sequenceNumber = std::nullopt;

        // /**
        //  * TODO: schema doesnt have description */
        struct Synchronization 
        {
            enum class SourceType
            {
                GEN_LOCK,
                VIDEO_IN,
                PTP,
                NTP
            };
            SourceType source;
            opentrackiotypes::Rational frequency;
            bool locked;
            std::optional<bool> present = std::nullopt;
            
            struct Offsets
            {
                std::optional<double> translation = std::nullopt;
                std::optional<double> rotation = std::nullopt;
                std::optional<double> encoders = std::nullopt;                
            };
            std::optional<Offsets> offsets = std::nullopt;

            std::optional<std::string> ptpMaster = std::nullopt;
            std::optional<double> ptpOffset = std::nullopt;
            std::optional<uint16_t> ptpDomain = std::nullopt;
        };
        std::optional<Synchronization> synchronization = std::nullopt;
        
        std::optional<opentrackiotypes::Timecode> timecode = std::nullopt;

        static std::optional<Timing> parse(const nlohmann::json& json, std::vector<std::string>& errors);
        
    private:
        static std::optional<Synchronization> parseSynchronization(const nlohmann::json& json, std::vector<std::string>& errors);
    };

    /**
     * A list of transforms. Transforms can have a name and parent that can be used to compose a transform hierarchy.
     * In the case of multiple children their transforms should be processed in their order in the array.
     * X,Y,Z in metres of camera sensor relative to stage origin.
     * The Z axis points upwards and the coordinate system is right-handed.
     * Y points in the forward camera direction (when pan, tilt and roll are zero).
     * For example in an LED volume Y would point towards the centre of the LED wall and so X would point to camera-right.
     * Rotation expressed as euler angles in degrees of the camera sensor relative to stage origin
     * Rotations are intrinsic and are measured around the axes ZXY, commonly referred to as [pan, tilt, roll]
     * Notes on Euler angles: Euler angles are human-readable and unlike quarternions,
     * provide the ability for cycles (with angles >360 or <0 degrees).
     * Where a tracking system is providing the pose of a virtual camera,
     * gimbal lock does not present the physical challenges of a robotic system.
     * Conversion to and from quarternions is trivial with an acceptable loss of precision */
    struct Transforms
    {
        std::vector<opentrackiotypes::Transform> transforms{};

        static std::optional<Transforms> parse(const nlohmann::json& json, std::vector<std::string>& errors);
    };
} // namespace opentrackio::opentrackioproperties
