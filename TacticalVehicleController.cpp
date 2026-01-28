#include "TacticalVehicleController.h"
#include "TacticalVehicleData.h"

#include <cmath>

/**
 * @brief Binds the controller to the shared TacticalVehicleData store.
 *
 * The controller operates purely on model data and owns no UI state.
 */
TacticalVehicleController::TacticalVehicleController(TacticalVehicleData& data) : data(data) {
}

// --- Filtering Logic ---
// Core engine for evaluating tactical vehicle data against
// UI-provided filter criteria.

// This function is intentionally UI-agnostic: all visual state
// (visibility, selections, ranges) is resolved by MainWindow
// before being passed here as primitive values.
void TacticalVehicleController::applyFilter(const FilterCriteria& criteria) {
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

        // --- Capability Flags ---
        if (criteria.hasSatCom && !vehicle.hasSatCom) {
            capabilityMatch = false;
        }
        if (criteria.isAmphibious && !vehicle.isAmphibious) {
            capabilityMatch = false;
        }
        if (criteria.isUnmanned && !vehicle.isUnmanned) {
            capabilityMatch = false;
        }
        if (criteria.hasActiveDefense && !vehicle.hasActiveDefense) {
            capabilityMatch = false;
        }

        // --- Identity Filters ---
        if (criteria.callsignActive && vehicle.callsign != criteria.callsign) {
            callsignMatch = false;
        }

        if (criteria.trackIdActive && vehicle.trackId != criteria.trackId) {
            trackIdMatch = false;
        }

        // --- Strategic Classification ---
        if (criteria.domainActive && vehicle.domain != criteria.domain) {
            domainMatch = false;
        }

        if (criteria.propulsionActive && vehicle.propulsion != criteria.propulsion) {
            propulsionMatch = false;
        }

        if (criteria.priorityActive && vehicle.priority != criteria.priority) {
            priorityMatch = false;
        }

        // --- Protection Constraints ---
        if (criteria.protectionMinActive && vehicle.protectionLevel < criteria.protectionMin) {
            protectionMatchMin = false;
        }

        if (criteria.protectionMaxActive && vehicle.protectionLevel > criteria.protectionMax) {
            protectionMatchMax = false;
        }

        // --- Telemetry Ranges ---
        if (vehicle.fuelLevel < criteria.fuelMin) {
            fuelMatchMin = false;
        }
        if (vehicle.fuelLevel > criteria.fuelMax) {
            fuelMatchMax = false;
        }

        if (vehicle.distanceToTarget < criteria.distanceMin) {
            distanceMatchMin = false;
        }
        if (criteria.distanceMax < 10000 && vehicle.distanceToTarget > criteria.distanceMax) {
            distanceMatchMax = false;
        }

        // --- Affiliation ---
        if (criteria.affiliation != "All Types" &&
            vehicle.affiliation != criteria.affiliation) {
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
 * @brief Indicates whether filtering currently affects the result set by comparing size.
 *
 * This function defines "filter active" in an outcome-based sense:
 * it returns true when the filtered view differs from the full vehicle dataset.
 *
 * Note:
 * - This does not represent user filter intent.
 * - If filter criteria are applied but happen to match all vehicles,
 *   this function will return false.
 *
 * This definition is intentional and matches the current UI behavior,
 * where view selection depends on whether filtering narrows results.
 */
bool TacticalVehicleController::isFilterActive() const {
    return filteredVehicles.size() != data.vehicles().size();
}

// --- Simulation Logic ---
// Advances vehicle positions and recalculates distances
// relative to the current mission target.

// This function operates exclusively on model data and is
// triggered externally by a timed heartbeat (QTimer).
void TacticalVehicleController::updateSimulation(double targetX, double targetY) {
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
