#ifndef TACTICALVEHICLEDATA_H
#define TACTICALVEHICLEDATA_H

#include "TacticalVehicle.h"
#include <deque>
#include <QString>

/**
 * @class TacticalVehicleData
 * @brief Data model and persistence layer for tactical vehicle datasets.
 *
 * Owns the master vehicle container, handles JSON loading, and provides
 * stateless comparison predicates for sorting derived views.
 */
class TacticalVehicleData
{
public:
    // --- Lifecycle ---
    TacticalVehicleData();

    // --- Persistence & I/O ---
    /**
     * @brief Parses vehicle data from a JSON resource.
     * @param path The file path or Qt resource path (e.g., ":/data/vehicles.json").
     */
    void loadVehiclesFromJson(const QString &path);

    // Read-only access (UI-safe)
    const std::deque<TacticalVehicle>& vehicles() const;

    // Controlled mutable access (sorting / simulation)
    std::deque<TacticalVehicle>& vehiclesMutable();

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

private:
    // --- Data Storage ---
    /** @brief Master container owning all vehicle objects. */
    std::deque<TacticalVehicle> allVehicles;

};

#endif // TACTICALVEHICLEDATA_H


