#include "TacticalVehicleController.h"
#include "TacticalVehicleData.h"

#include <cmath>
#include <QRandomGenerator>

/**
 * @brief Binds the controller to the shared TacticalVehicleData store.
 *
 * The controller operates purely on model data and owns no UI state.
 */
TacticalVehicleController::TacticalVehicleController(TacticalVehicleData& data) : data(data) {
}

// --- Filtering Logic ---
// Evaluates TacticalVehicleData against resolved, UI-agnostic filter criteria.
void TacticalVehicleController::applyFilter(const FilterCriteria& criteria) {
    filteredVehicles.clear();

    for (const auto& v : data.vehicles()) {
        if (matches(v, criteria)) {
            filteredVehicles.push_back(&v);
        }
    }
}

bool TacticalVehicleController::matches( const TacticalVehicle& vehicle, const FilterCriteria& criteria) const {
    // Capability Flags
    if (criteria.hasSatCom && !vehicle.hasSatCom) return false;
    if (criteria.isAmphibious && !vehicle.isAmphibious) return false;
    if (criteria.isUnmanned && !vehicle.isUnmanned) return false;
    if (criteria.hasActiveDefense && !vehicle.hasActiveDefense) return false;

    // Identity Filters
    if (criteria.callsignActive && vehicle.callsign != criteria.callsign) return false;
    if (criteria.trackIdActive && vehicle.trackId != criteria.trackId) return false;

    // Strategic Classification
    if (criteria.domainActive && vehicle.domain != criteria.domain) return false;
    if (criteria.propulsionActive && vehicle.propulsion != criteria.propulsion) return false;
    if (criteria.priorityActive && vehicle.priority != criteria.priority) return false;

    // Protection Constraints
    if (criteria.protectionMinActive && vehicle.protectionLevel < criteria.protectionMin) return false;
    if (criteria.protectionMaxActive && vehicle.protectionLevel > criteria.protectionMax) return false;

    // Telemetry Ranges
    if (vehicle.fuelLevel < criteria.fuelMin) return false;
    if (vehicle.fuelLevel > criteria.fuelMax) return false;
    if (vehicle.distanceToTarget < criteria.distanceMin) return false;
    if (criteria.distanceMax < 10000 && vehicle.distanceToTarget > criteria.distanceMax) return false;

    // Affiliation
    if (criteria.affiliation != "All Types" &&
        vehicle.affiliation != criteria.affiliation) return false;

    return true;
}

int TacticalVehicleController::countMatches(const FilterCriteria& criteria) const {
    int count = 0;
    for (const auto& v : data.vehicles()) {
        if (matches(v, criteria)) {
            ++count;
        }
    }
    return count;
}

/**
 * @brief Indicates whether filtering affects the result set.
 *
 * Returns true when the filtered view size differs from the full dataset size.
 * This reflects result-based filtering, not user intent.
 */
bool TacticalVehicleController::isFilterActive() const {
    return filteredVehicles.size() != data.vehicles().size();
}

// --- Simulation Logic ---
// Advances vehicle positions and recalculates distances relative to the current mission target.

// This function operates exclusively on model data and is
// triggered externally by a timed heartbeat (QTimer).
void TacticalVehicleController::updateSimulation(double targetX, double targetY) {
    constexpr double PI_CONST = 3.14159265358979323846;

    for (auto& v : data.vehiclesMutable()) {

        const double rad = (v.heading - 90.0) * (PI_CONST / 180.0);

        // Apply small randomized variation around target speed
        quint32 variedSpeed = 0;
        double upper = 0.0;
        double lower = 0.0;
        if (v.speed > 0) {
            if(v.speed < 100) {
                lower = v.targetSpeed-v.targetSpeed*0.03;
                upper = v.targetSpeed+v.targetSpeed*0.03;
            } else if (v.speed > 100 && v.speed < 300) {
                lower = v.targetSpeed-v.targetSpeed*0.02;
                upper = v.targetSpeed+v.targetSpeed*0.02;
            } else {
                lower = v.targetSpeed-v.targetSpeed*0.01;
                upper = v.targetSpeed+v.targetSpeed*0.01;
            }
            quint32 lowerLimit = static_cast<qint32>(lower);
            quint32 upperLimit = std::max(lowerLimit + 1, static_cast<quint32>(upper));
            variedSpeed = QRandomGenerator::global()->bounded(lowerLimit, upperLimit);
            v.speed = static_cast<double>(variedSpeed);

        // Apply small randomized variation to heading
            quint32 variedHeading = 0;
            if (v.heading > 0) {
                double headup = v.heading+1.0;
                double headlow = v.heading-1.0;
                quint32 headingUpper = static_cast<qint32>(headup);
                quint32 headingLower = static_cast<qint32>(headlow);
                variedHeading = QRandomGenerator::global()->bounded(headingLower, headingUpper);
            } else {
                variedHeading = QRandomGenerator::global()->bounded(0, 1);
            }
            v.heading = static_cast<double>(variedHeading);
        }

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
