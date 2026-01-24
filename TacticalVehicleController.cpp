#include "TacticalVehicleController.h"
#include "TacticalVehicleData.h"
#include <cmath>

/**
 * @brief TacticalVehicleController Constructor
 * Binds the controller to the shared TacticalVehicleData store.
 * The controller operates on data but owns no UI state.
 */
TacticalVehicleController::TacticalVehicleController(TacticalVehicleData& data) : data(data) {
}

/**
 * @section FILTERING_LOGIC
 * Core engine for evaluating tactical vehicle data against
 * UI-provided filter criteria.
 *
 * This function is intentionally UI-agnostic: all visual state
 * (visibility, selections, ranges) is resolved by MainWindow
 * before being passed here as primitive values.
 */
void TacticalVehicleController::applyFilter(
    bool hasSatCom,
    bool isAmphibious,
    bool isUnmanned,
    bool hasActiveDefense,

    bool callsignActive,
    const QString& callsign,

    bool trackIdActive,
    const QString& trackId,

    bool domainActive,
    const QString& domain,

    bool propulsionActive,
    const QString& propulsion,

    bool priorityActive,
    const QString& priority,

    bool protectionMinActive,
    int protectionMin,

    bool protectionMaxActive,
    int protectionMax,

    int fuelMin,
    int fuelMax,

    int distanceMin,
    int distanceMax,

    const QString& affiliation
    )
{
    filteredVehicles.clear();

    for (const auto& vehicle : data.vehicles()) {

        // Default to permissive matching; constraints narrow results
        bool capabilityMatch     = true;
        bool callsignMatch       = true;
        bool trackIdMatch        = true;
        bool domainMatch         = true;
        bool propulsionMatch     = true;
        bool priorityMatch       = true;
        bool protectionMatchMin  = true;
        bool protectionMatchMax  = true;
        bool fuelMatchMin        = true;
        bool fuelMatchMax        = true;
        bool distanceMatchMin    = true;
        bool distanceMatchMax    = true;
        bool affiliationMatch    = true;

        // --- 1. Operational Capabilities (Strict AND Logic) ---
        if (hasSatCom && !vehicle.hasSatCom) {
            capabilityMatch = false;
        }
        if (isAmphibious && !vehicle.isAmphibious) {
            capabilityMatch = false;
        }
        if (isUnmanned && !vehicle.isUnmanned) {
            capabilityMatch = false;
        }
        if (hasActiveDefense && !vehicle.hasActiveDefense) {
            capabilityMatch = false;
        }

        // --- 2. Identity Filters ---
        if (callsignActive && vehicle.callsign != callsign) {
            callsignMatch = false;
        }

        if (trackIdActive && vehicle.trackId != trackId) {
            trackIdMatch = false;
        }

        // --- 3. Strategic Classification ---
        if (domainActive && vehicle.domain != domain) {
            domainMatch = false;
        }

        if (propulsionActive && vehicle.propulsion != propulsion) {
            propulsionMatch = false;
        }

        if (priorityActive && vehicle.priority != priority) {
            priorityMatch = false;
        }

        // --- 4. Protection Constraints ---
        if (protectionMinActive && vehicle.protectionLevel < protectionMin) {
            protectionMatchMin = false;
        }

        if (protectionMaxActive && vehicle.protectionLevel > protectionMax) {
            protectionMatchMax = false;
        }

        // --- 5. Telemetry Ranges ---
        if (vehicle.fuelLevel < fuelMin) {
            fuelMatchMin = false;
        }
        if (vehicle.fuelLevel > fuelMax) {
            fuelMatchMax = false;
        }

        if (vehicle.distanceToTarget < distanceMin) {
            distanceMatchMin = false;
        }
        if (distanceMax < 10000 && vehicle.distanceToTarget > distanceMax) {
            distanceMatchMax = false;
        }

        // --- 6. Affiliation ---
        if (affiliation != "All Types" &&
            vehicle.affiliation != affiliation) {
            affiliationMatch = false;
        }

        // --- FINAL EVALUATION ---
        if (capabilityMatch &&
            callsignMatch &&
            trackIdMatch &&
            domainMatch &&
            propulsionMatch &&
            priorityMatch &&
            protectionMatchMin &&
            protectionMatchMax &&
            fuelMatchMin &&
            fuelMatchMax &&
            distanceMatchMin &&
            distanceMatchMax &&
            affiliationMatch) {

            filteredVehicles.push_back(&vehicle);
        }
    }
}

/**
 * @section SIMULATION_LOGIC
 * Advances vehicle positions and recalculates distances
 * relative to the current mission target.
 *
 * This function operates exclusively on model data and is
 * triggered externally by a timed heartbeat (QTimer).
 */
void TacticalVehicleController::updateSimulation(double targetX, double targetY)
{
    constexpr double PI_CONST = 3.14159265358979323846;

    for (auto& v : data.vehiclesMutable()) {

        // Convert heading to radians (UI uses degrees)
        const double rad = (v.heading - 90.0) * (PI_CONST / 180.0);

        // Speed conversion: km/h -> m/s
        const double distPerSecond = v.speed / 3.6;

        // Integrate position
        v.posX += distPerSecond * std::cos(rad);
        v.posY += distPerSecond * std::sin(rad);

        // Update target-relative distance
        const double dx = targetX - v.posX;
        const double dy = targetY - v.posY;
        v.distanceToTarget = std::sqrt(dx * dx + dy * dy);
    }
}
