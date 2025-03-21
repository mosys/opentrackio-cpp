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
#include <vector>
#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "OpenTrackIOTypes.h"

#define OPEN_TRACK_IO_PROTOCOL_NAME "OpenTrackIO"
#define OPEN_TRACK_IO_PROTOCOL_MAJOR_VERSION 1
#define OPEN_TRACK_IO_PROTOCOL_MINOR_VERSION 0
#define OPEN_TRACK_IO_PROTOCOL_PATCH         0

namespace opentrackio::opentrackioproperties
{
    /** Duration of the clip.
    * Unit: Seconds */
    struct Duration
    {
        opentrackiotypes::Rational rational{};

        static std::optional<Duration> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct Camera
    {
        /**
        * Height and width of the active area of the camera sensor in millimeters.
        * Units: Millimeters */
        std::optional<opentrackiotypes::Dimensions<double>> activeSensorPhysicalDimensions = std::nullopt;

        /**
        * Photosite resolution of the active area of the camera sensor in pixels.
        * Units: Pixels */
        std::optional<opentrackiotypes::Dimensions<uint32_t>> activeSensorResolution = std::nullopt;

        /**
        * Nominal ratio of height to width of the image of an axis-aligned square captured by the
        * camera sensor. It can be used to de-squeeze images but is not however an exact number over
        * the entire captured area due to a lens' intrinsic analog nature. */
        std::optional<opentrackiotypes::Rational> anamorphicSqueeze = std::nullopt;

        /**
        * Non-blank string identifying camera firmware version. */
        std::optional<std::string> firmwareVersion = std::nullopt;

        /**
        * Non-blank string containing user-determined camera identifier. */
        std::optional<std::string> label = std::nullopt;

        /**
        * Non-blank string naming camera manufacturer. */
        std::optional<std::string> make = std::nullopt;

        /**
        * Non-blank string identifying camera model. */
        std::optional<std::string> model = std::nullopt;

        /**
        * Non-blank string uniquely identifying the camera.*/
        std::optional<std::string> serialNumber = std::nullopt;

        /**
        * Capture frame rate of the camera
        * Units: Hertz */
        std::optional<opentrackiotypes::Rational> captureFrameRate = std::nullopt;

        /**
        * URN identifying the ASC Framing Decision List used by the camera.
        * Pattern: ^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$ */
        std::optional<std::string> fdlLink = std::nullopt;

        /**
        * Arithmetic ISO scale as defined in ISO 12232 */
        std::optional<uint32_t> isoSpeed = std::nullopt;

        /**
        * Shutter speed as a fraction of the capture frame rate. The shutter speed (in units of 1/s)
        * is equal to the value of the parameter divided by 360 times the capture frame rate.
        * Units: Degree */
        std::optional<double> shutterAngle = std::nullopt;

        static std::optional<Camera> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };

    /**
    * Position of the stage origin in global ENU and geodetic coordinates
    * (E, N, U, lat0, lon0, h0). Note this may be dynamic e.g. if the stage is inside
    * a moving vehicle.
    * Units: Meters */
    struct GlobalStage
    {
        double e;
        double n;
        double u;
        double lat0;
        double lon0;
        double h0;

        static std::optional<GlobalStage> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct Lens
    {
        /**
        * This list provides optional custom additional coefficients for a
        * particular lens model. The meaning of which would require negotiation
        * between a particular producer and consumer. */
        std::optional<std::vector<double>> custom = std::nullopt;

        /**
        * Coefficients for calculating the distortion characteristics of a lens
        * comprising radial distortion coefficients of the spherical distortion (k1-N)
        * and the tangential distortion (p1-N). */
        struct Distortion
        {
            std::vector<double> radial{};
            std::optional<std::vector<double>> tangential = std::nullopt;
            std::optional<std::string> model = std::nullopt;
            std::optional<double> overscan = std::nullopt;
        };
        std::optional<std::vector<Distortion>> distortion = std::nullopt;

        /**
        * Static maximum overscan factor on lens distortion. This is an alternative to providing dynamic
        * overscan values each frame. Note it should be the maximum of both projection-matrix-based and
        * field-of-view-based rendering as per the OpenLensIO documentation. */
        std::optional<double> distortionOverscanMax = std::nullopt;

        /**
        * Static maximum overscan factor on lens undistortion. This is an alternative to providing dynamic
        * overscan values each frame. Note it should be the maximum of both projection-matrix-based and
        * field-of-view-based rendering as per the OpenLensIO documentation. */
        std::optional<double> undistortionOverscanMax = std::nullopt;

        /**
        * Shift in X and Y of the centre of distortion of the virtual camera
        * Units: Millimeters */
        struct DistortionOffset
        {
            double x;
            double y;
        };
        std::optional<DistortionOffset> distortionOffset = std::nullopt;

        /**
        * Normalised real numbers (0-1) for focus, iris and zoom. Encoders are represented in this way
        * (as opposed to raw integer values) to ensure values remain independent of encoder resolution,
        * minimum and maximum (at an acceptable loss of precision). These values are only relevant in lenses with
        * end-stops that demarcate the 0 and 1 range. Value should be provided in the following
        * directions (if known):
        * Focus: 0=infinite, 1=closest
        * Iris:  0=open, 1=closed
        * Zoom:  0=wide-angle, 1=telephoto */
        struct Encoders
        {
            std::optional<double> focus = std::nullopt;
            std::optional<double> iris = std::nullopt;
            std::optional<double> zoom = std::nullopt;            
        };
        std::optional<Encoders> encoders = std::nullopt;
        
        /**
        * Offset of the entrance pupil relative to the nominal imaging plane (positive if the entrance pupil
        * is located on the side of the nominal imaging plane that is towards the object, and negative otherwise).
        * Measured in meters as in a render engine it is often applied in the virtual camera's transform chain.
        * Units: Meters */
        std::optional<double> entrancePupilOffset = std::nullopt;

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
        std::optional<double> fStop = std::nullopt;

        /**
        * Distance between the pinhole and the image plane in the simple CGI pinhole camera model.
        * Units: Millimeters */
        std::optional<double> pinholeFocalLength = std::nullopt;
     
        /**
        * Non-blank string identifying lens firmware version. */
        std::optional<std::string> firmwareVersion = std::nullopt;

        /**
        * Focus distance/position of the lens.
        * Units: Meters */
        std::optional<double> focusDistance = std::nullopt;

        /**
        * Non-blank string naming lens manufacturer. */
        std::optional<std::string> make = std::nullopt;

        /**
        * Non-blank string identifying lens model. */
        std::optional<std::string> model = std::nullopt;

        /**
        * Nominal focal length of the lens.
        * The number printed on the side of a prime lens, e.g. 50 mm,
        * and undefined in the case of a zoom lens.
        * Units: Millimeters */
        std::optional<double> nominalFocalLength = std::nullopt;

        /** List of free strings that describe the history of calibrations of the lens */
        std::optional<std::vector<std::string>> calibrationHistory = std::nullopt;

        /**
        * Offset in X and Y of the centre of perspective projection of the virtual camera
        * Units: Millimeters */
        struct ProjectionOffset
        {
            double x;
            double y;
        };
        std::optional<ProjectionOffset> projectionOffset = std::nullopt;

        /**
        * Raw encoder values for focus, iris and zoom. These values are dependent on encoder resolution and
        * before any homing/ranging has taken place. */
        struct RawEncoders
        {
            std::optional<uint32_t> focus = std::nullopt;
            std::optional<uint32_t> iris = std::nullopt;
            std::optional<uint32_t> zoom = std::nullopt;
        };
        std::optional<RawEncoders> rawEncoders = std::nullopt;

        /**
        * Non-blank string uniquely identifying the lens.*/
        std::optional<std::string> serialNumber = std::nullopt;        
        
        /**
        * The linear t-number of the lens, equal to the F-number of the lens divided by the square root of the
        * transmittance of the lens. */
        std::optional<double> tStop = std::nullopt;

        static std::optional<Lens> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };
    
    struct Protocol
    {
        /**
        * Name of the protocol in which the sample is being employed, and version of that protocol. */
        std::string name;

        /**
        * Version as integers e.g. 1.0.0 */
        std::vector<uint16_t> version;

        static std::optional<Protocol> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct RelatedSampleIds
    {
        /**
        * List of sampleId properties of samples related to this sample.
        * The existence of a sample with a given sampleId is not guaranteed
        * Pattern: ^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$ */
        std::vector<std::string> samples;

        static std::optional<RelatedSampleIds> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct SampleId
    {
        /**
        * URN serving as unique identifier of the sample in which data is being transported.
        * Pattern: ^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$ */
        std::string id;

        static std::optional<SampleId> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };
    
    struct SourceId
    {
        /**
        * URN serving as unique identifier of the source from which data is being transported
        * pattern: ^urn:uuid:[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$ */
        std::string id;

        static std::optional<SourceId> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct SourceNumber
    {
        /**
	    * Number that identifies the index of the stream from a source from which data is being transported.
	    * This is most important in the case where a source is producing multiple streams of samples. */
        uint32_t value;

        static std::optional<SourceNumber> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct Timing
    {
        /**
        * Sample frame rate as a rational number. Drop frame rates such as 29.97 should be
        * represented as e.g. 30000/1001. In a variable rate system this should
        * be estimated from the last sample delta time. */
        std::optional<opentrackiotypes::Rational> sampleRate = std::nullopt;
         
        /**
        * Enumerated value indicating whether the sample transport mechanism provides inherent ('external')
        * timing, or whether the transport mechanism lacks inherent timing and so the sample must contain a
        * PTP timestamp itself ('internal') to carry timing information. */
        enum class Mode
        {
            EXTERNAL,
            INTERNAL
        };
        std::optional<Mode> mode = std::nullopt;

        /**
        * PTP timestamp of the data recording instant, provided for convenience
        * during playback of e.g. pre-recorded tracking data. The timestamp comprises a 48-bit unsigned
        * integer (seconds), a 32-bit unsigned integer (nanoseconds) */
        std::optional<opentrackiotypes::Timestamp> recordedTimestamp = std::nullopt;
        
        /**
        * PTP timestamp of the data capture instant. Note this may differ from the packet's transmission
        * PTP timestamp. The timestamp comprises a 48-bit unsigned integer (seconds), a 32-bit unsigned
        * integer (nanoseconds).
        */
        std::optional<opentrackiotypes::Timestamp> sampleTimestamp = std::nullopt;

        /**
        * Integer incrementing with each sample. */
        std::optional<uint32_t> sequenceNumber = std::nullopt;

        /**
        * Object describing how the tracking device is synchronized for this sample. */
        struct Synchronization
        {
            /**
            * The frequency of a synchronization signal.This may differ from the sample
            * frame rate for example in a gen-locked tracking device.
            * This is not required if the synchronization source is PTP or NTP.*/
            std::optional<opentrackiotypes::Rational> frequency = std::nullopt;

            /**
            * Is the tracking device locked to the synchronization source */
            bool locked;

            /**
            * Offsets in seconds between sync and sample. Critical for e.g. frame remapping, or when using
            * different data sources for position/rotation and lens encoding.
            */
            struct Offsets
            {
                std::optional<double> translation = std::nullopt;
                std::optional<double> rotation = std::nullopt;
                std::optional<double> lensEncoders = std::nullopt;
            };
            std::optional<Offsets> offsets = std::nullopt;

            /**
            * Is the synchronization source present (a synchronization source can be present but
            * not locked if frame rates differ for example). */
            std::optional<bool> present = std::nullopt;

            /**
            * If the synchronization source is a PTP leader. */
            struct Ptp
            {
                /**
                * Specifies the PTP profile in use. This defines the operational rules and parameters
                * for synchronization. For example "SMPTE ST2059-2:2021" for SMPTE 2110 based systems,
                * or "IEEE Std 1588-2019" or "IEEE Std 802.1AS-2020" for industrial applications. */
                enum class ProfileType
                {
                    IEEE_Std_1588_2019,    // "IEEE Std 1588-2019"
                    IEEE_Std_802_1AS_2020, // "IEEE Std 802.1AS-2020"
                    SMPTE_ST2059_2_2021    // "SMPTE ST2059-2:2021"
                };
                ProfileType profile = ProfileType::SMPTE_ST2059_2_2021;

                /**
                * Identifies the PTP domain the device belongs to.
                * Devices in the same domain can synchronize with each other. */
                uint16_t domain = 0;

                /**
                * The unique identifier (usually MAC address) of the current PTP leader (grandmaster). */
                std::string leaderIdentity;

                /**
                * The priority values of the leader used in the Best Master Clock Algorithm (BMCA).
                * Lower values indicate higher priority. */
                struct LeaderPriorities
                {
                    /**
                    * Static priority set by the administrator. */
                    uint8_t priority1 = 0;

                    /**
                    * Dynamic priority based on the leader's role or clock quality. */
                    uint8_t priority2 = 0;
                };
                LeaderPriorities leaderPriorities;

                /**
                * The timing offset in seconds from the sample timestamp to the PTP timestamp. */
                double leaderAccuracy = 0.0;

                /**
                * The average round-trip delay between the device and the PTP leader.
                * Units: Seconds */
                double meanPathDelay = 0.0;

                /**
                * Integer representing the VLAN ID for PTP traffic (e.g., 100 for VLAN 100).  */
                std::optional<int32_t> vlan = std::nullopt;

                /**
                * Indicates the leader's source of time, such as GNSS, atomic clock, or NTP. */
                enum class LeaderTimeSourceType
                {
                    GNSS,         // "GNSS",
                    Atomic_clock, // "Atomic clock",
                    NTP           // "NTP"
                };
                std::optional<LeaderTimeSourceType> leaderTimeSource = std::nullopt;
            };
            std::optional<Ptp> ptp = std::nullopt;

            /**
            * The source of synchronization.
            * genlock: The tracking device has an external black/burst or tri-level analog sync signal
            *          that is triggering the capture of tracking samples.
            * videoIn: The tracking device has an external video signal that is triggering the
            *          capture of tracking samples.
            * ptp:     The tracking device is locked to a PTP leader.
            * ntp:     The tracking device is locked to an NTP server.
             */
            enum class SourceType
            {
                GEN_LOCK, // "genlock"
                VIDEO_IN, // "videoIn"
                PTP,      // "ptp"
                NTP       // "ntp"
            };
            SourceType source;
        };
        std::optional<Synchronization> synchronization = std::nullopt;
        
        /**
        * SMPTE timecode of the sample. Timecode is a standard for labeling individual frames of data in media
        * systems and is useful for inter-frame synchronization. Frame rate is a rational number,
        * allowing drop frame rates such as that colloquially called 29.97 to be represented exactly, as
        * 30000/1001. The timecode frame rate may differ from the sample frequency. The zero-based sub-frame
        * field allows for finer division of the frame, e.g. interlaced frames have two sub-frames, one per field. */
        std::optional<opentrackiotypes::Timecode> timecode = std::nullopt;

        static std::optional<Timing> parse(nlohmann::json& json, std::vector<std::string>& errors);
        
    private:
        static std::optional<Synchronization> parseSynchronization(nlohmann::json& json, std::vector<std::string>& errors);
        static std::optional<Synchronization::Ptp> parsePtp(nlohmann::json& json, std::vector<std::string>& errors);
    };

    struct Tracker
    {
        /**
         * 	Non-blank string identifying tracking device firmware version. */
        std::optional<std::string> firmwareVersion = std::nullopt;

        /**
        * Non-blank string naming tracking device manufacturer. */
        std::optional<std::string> make = std::nullopt;

        /**
        * Non-blank string identifying tracking device model. */
        std::optional<std::string> model = std::nullopt;

        /**
        * Non-blank string containing notes about tracking system. */
        std::optional<std::string> notes = std::nullopt;

        /**
        * Boolean indicating whether tracking system is recording data. */
        std::optional<bool> recording = std::nullopt;

        /**
        * Non-blank string uniquely identifying the tracking device.*/
        std::optional<std::string> serialNumber = std::nullopt;

        /**
        * Non-blank string describing the recording slate. */
        std::optional<std::string> slate = std::nullopt;

        /**
        * Non-blank string describing status of tracking system. */
        std::optional<std::string> status = std::nullopt;

        static std::optional<Tracker> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };    

    /**
    * A list of transforms.
    * Transforms are composed in order with the last in the list representing the X,Y,Z in meters of camera
    * sensor relative to stage origin. The Z axis points upwards and the coordinate system is right-handed.
    * Y points in the forward camera direction (when pan, tilt and roll are zero). For example in an LED volume
    * Y would point towards the centre of the LED wall and so X would point to camera-right. Rotation expressed as
    * euler angles in degrees of the camera sensor relative to stage origin Rotations are intrinsic and are measured
    * around the axes ZXY, commonly referred to as [pan, tilt, roll].
    * Notes on Euler angles: Euler angles are human-readable and unlike quaternions, provide the ability for cycles
    * (with angles >360 or <0 degrees). Where a tracking system is providing the pose of a virtual camera, gimbal lock
    * does not present the physical challenges of a robotic system. Conversion to and from quaternions is trivial
    * with an acceptable loss of precision.
    * Units: Meters/Degrees
     */
    struct Transforms
    {
        std::vector<opentrackiotypes::Transform> transforms{};

        static std::optional<Transforms> parse(nlohmann::json& json, std::vector<std::string>& errors);
    };
} // namespace opentrackio::opentrackioproperties
