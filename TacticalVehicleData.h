#ifndef TACTICALVEHICLEDATA_H
#define TACTICALVEHICLEDATA_H

#include "TacticalVehicle.h"

#include <QString>

#include <deque>

/**
 * @class TacticalVehicleData
 * @brief Data model and persistence layer for tactical vehicle datasets.
 *
 * Owns the authoritative vehicle container, handles JSON-based ingestion,
 * and provides stateless comparison predicates for sorting derived views.
 */
class TacticalVehicleData {
public:
    // --- Construction ---
    TacticalVehicleData();

    // --- Persistence ---
    void loadVehiclesFromJson(const QString &path);

    // --- Data Access ---
    const std::deque<TacticalVehicle>& vehicles() const;
    std::deque<TacticalVehicle>& vehiclesMutable();

    // --- Sorting Predicates ---
    // Stateless comparators intended for std::sort on pointer-based views.

    static bool sortByDistanceAsc(const TacticalVehicle *a, const TacticalVehicle *b);
    static bool sortByDistanceDesc(const TacticalVehicle *a, const TacticalVehicle *b);

    static bool sortByThreatAsc(const TacticalVehicle* a, const TacticalVehicle* b);
    static bool sortByThreatDesc(const TacticalVehicle* a, const TacticalVehicle* b);

    static bool sortByPriorityAsc(const TacticalVehicle *a, const TacticalVehicle *b);
    static bool sortByPriorityDesc(const TacticalVehicle *a, const TacticalVehicle *b);

    static bool sortByFuelAsc(const TacticalVehicle *a, const TacticalVehicle *b);
    static bool sortByFuelDesc(const TacticalVehicle *a, const TacticalVehicle *b);

    static bool sortByClassificationAsc(const TacticalVehicle *a, const TacticalVehicle *b);
    static bool sortByClassificationDesc(const TacticalVehicle *a, const TacticalVehicle *b);

private:
    // --- Data Storage ---
    std::deque<TacticalVehicle> allVehicles; ///< Master container owning all vehicles
};

#endif // TACTICALVEHICLEDATA_H
