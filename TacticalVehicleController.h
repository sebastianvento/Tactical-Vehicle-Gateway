#ifndef TACTICALVEHICLECONTROLLER_H
#define TACTICALVEHICLECONTROLLER_H

#include "FilterCriteria.h"
#include <vector>
#include <QString>


class TacticalVehicleData;
struct TacticalVehicle;

/**
 * @class TacticalVehicleController
 * @brief Central logic controller for tactical vehicle processing.
 *
 * Responsibilities:
 *  - Evaluate filtering criteria against the vehicle database
 *  - Maintain derived filtered views for presentation
 *  - Advance simulation state and compute derived telemetry
 *
 * This class is intentionally UI-agnostic and operates purely
 * on data and primitive inputs resolved by MainWindow.
 */
class TacticalVehicleController
{
public:
    explicit TacticalVehicleController(TacticalVehicleData& data);

    /**
     * @section FILTERING_LOGIC
     * Computes filteredVehicles based on externally provided criteria.
     * Does NOT perform any UI updates or own presentation state.
     */
    void applyFilter(const FilterCriteria& criteria);
    bool isFilterActive() const;

    /**
     * @section SIMULATION_LOGIC
     * Advances vehicle positions and updates target-relative telemetry.
     */
    void updateSimulation(double targetX, double targetY);

    /**
     * @brief Derived view of vehicles matching current filter criteria.
     * Owned and maintained by the controller.
     */
    std::vector<const TacticalVehicle*> filteredVehicles;

private:
    /** Reference to the authoritative vehicle data store. */
    TacticalVehicleData& data;
};

#endif // TACTICALVEHICLECONTROLLER_H
