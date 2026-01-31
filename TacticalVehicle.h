#ifndef TACTICALVEHICLE_H
#define TACTICALVEHICLE_H

#include <QString>

/**
 * @struct TacticalVehicle
 * @brief Represents a single tactical asset within the system.
 *
 * Holds static identity and classification data, technical characteristics,
 * and dynamic telemetry used for simulation, filtering, and presentation.
 */
struct TacticalVehicle {
    // --- Identity & Classification ---
    QString callsign;        ///< Unique radio callsign (e.g. "Sabre 1-1")
    QString trackId;         ///< Unique system track identifier (e.g. "T-7201")
    QString type;            ///< Specific vehicle model or designation
    QString classification;  ///< Broad asset category
    QString affiliation;     ///< Hostility status (Friendly, Hostile, Neutral, Unknown)
    QString priority;        ///< Strategic priority level (Flash, High, Routine, Low)
    QString domain;          ///< Operational environment (Land, Sea, Air, Subsurface, Space, Electronic)
    QString propulsion;      ///< Locomotion method (Wheeled, Tracked, Legged, Aerial, Maritime)

    // --- Technical Specifications ---
    int protectionLevel = 0; ///< STANAG 4569 ballistic protection level (1–6)
    double maxSpeed = 0.0;   ///< Maximum rated speed (km/h)
    double targetSpeed = 0.0;///< Target speed for operative function (km/h)
    QString natoIcon;        ///< APP-6 / MIL-STD-2525 symbol identifier

    // --- Operational Capabilities ---
    bool hasSatCom = false;        ///< Satellite communication capability
    bool isAmphibious = false;     ///< Capable of water-based operations
    bool isUnmanned = false;       ///< Unmanned / autonomous operation
    bool hasActiveDefense = false; ///< Active hard/soft-kill defense systems

    // --- Dynamic Telemetry ---
    double posX = 0.0;             ///< Cartesian X coordinate (meters)
    double posY = 0.0;             ///< Cartesian Y coordinate (meters)
    double heading = 0.0;          ///< Navigational heading (degrees, 0–359)
    double speed = 0.0;            ///< Current speed (km/h)
    double fuelLevel = 100.0;      ///< Remaining fuel percentage (0–100)
    double ammunitionLevel = 100.0;///< Remaining ammunition percentage (0–100)
    double distanceToTarget = 0.0; ///< Euclidean distance to mission target (meters)
};

#endif // TACTICALVEHICLE_H
