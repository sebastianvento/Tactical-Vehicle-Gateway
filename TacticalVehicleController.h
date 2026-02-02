#ifndef TACTICALVEHICLECONTROLLER_H
#define TACTICALVEHICLECONTROLLER_H

#include <QString>

#include <vector>

// Forward declarations
class TacticalVehicleData;
struct TacticalVehicle;

/**
 * @struct FilterCriteria
 * @brief Aggregates all filter inputs resolved from UI state.
 *
 * Pure data container used to parameterize filtering logic.
 * Contains no behavior or validation.
 */
struct FilterCriteria {
    // --- Capability Flags ---
    bool hasSatCom = false;
    bool isAmphibious = false;
    bool isUnmanned = false;
    bool hasActiveDefense = false;

    // --- Identity Filters ---
    bool callsignActive = false;
    QString callsign;

    bool trackIdActive = false;
    QString trackId;

    // --- Strategic Classification ---
    bool domainActive = false;
    QString domain;

    bool propulsionActive = false;
    QString propulsion;

    bool priorityActive = false;
    QString priority;

    // --- Protection Constraints ---
    bool protectionMinActive = false;
    int protectionMin = 0;

    bool protectionMaxActive = false;
    int protectionMax = 0;

    // --- Telemetry Ranges ---
    int fuelMin = 0;
    int fuelMax = 100;

    // --- Distancy To Target ---
    // NOTE: distanceMax == 10000 is treated as "no upper bound" (UI convention).
    int distanceMin = 0;
    int distanceMax = 10000;

    // --- Affiliation ---
    QString affiliation = "All Types";
};

/**
 * @class TacticalVehicleController
 * @brief Central domain controller for tactical vehicle processing.
 *
 * Evaluates filter criteria against the vehicle dataset, maintains
 * derived filtered views, and advances simulation state.
 *
 * This class is UI-agnostic and operates purely on model data.
 */
class TacticalVehicleController {
public:
    explicit TacticalVehicleController(TacticalVehicleData& data);

    // --- Filtering ---
    void applyFilter(const FilterCriteria& criteria);
    bool matches(const TacticalVehicle& vehicle, const FilterCriteria& criteria) const;
    int countMatches(const FilterCriteria& criteria) const;
    bool isFilterActive() const;

    // --- Simulation ---
    void updateSimulation(double targetX, double targetY);

    // --- Derived Views ---
    std::vector<const TacticalVehicle*> filteredVehicles;

private:
    // --- Data Reference ---
    TacticalVehicleData& data; ///< Vehicle data store
};

#endif // TACTICALVEHICLECONTROLLER_H
