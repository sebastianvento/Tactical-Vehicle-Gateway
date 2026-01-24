#ifndef TACTICALVEHICLEDATA_H
#define TACTICALVEHICLEDATA_H

#include "TacticalVehicle.h"
#include <vector>
#include <deque>
#include <QString>

/**
 * @class TacticalVehicleData
 * @brief Controller class for managing tactical vehicle datasets.
 * * This class handles the persistence (JSON loading) and provides static
 * comparison logic for sorting both the master and filtered vehicle collections.
 */
class TacticalVehicleData {
public:
    // --- Lifecycle ---
    TacticalVehicleData();

    // --- Data Storage ---
    /** @brief Master container owning all vehicle objects. */
    std::deque<TacticalVehicle> allVehicles;

    // --- Persistence & I/O ---
    /** * @brief Parses vehicle data from a JSON resource.
     * @param path The file path or Qt resource path (e.g., ":/data/vehicles.json").
     */
    void loadVehiclesFromJson(const QString &path);

    // --- Static Sort Predicates ---
    // These functions are used as comparators for std::sort.

    // Distance Based
    static bool sortByDistanceAsc(const TacticalVehicle* a, const TacticalVehicle* b);
    static bool sortByDistanceDesc(const TacticalVehicle* a, const TacticalVehicle* b);

    // Fuel Based
    static bool sortByFuelAsc(const TacticalVehicle* a, const TacticalVehicle* b);
    static bool sortByFuelDesc(const TacticalVehicle* a, const TacticalVehicle* b);

    // Strategic Priority / Priority Based
    static bool sortByPriorityAsc(const TacticalVehicle* a, const TacticalVehicle* b);
    static bool sortByPriorityDesc(const TacticalVehicle* a, const TacticalVehicle* b);

    // Asset Classification Based
    static bool sortByClassificationAsc(const TacticalVehicle* a, const TacticalVehicle* b);
    static bool sortByClassificationDesc(const TacticalVehicle* a, const TacticalVehicle* b);
};

#endif // TACTICALVEHICLEDATA_H


