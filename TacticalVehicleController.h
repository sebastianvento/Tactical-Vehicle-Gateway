#ifndef TACTICALVEHICLECONTROLLER_H
#define TACTICALVEHICLECONTROLLER_H

#include <vector>


class TacticalVehicleData;
struct TacticalVehicle;

/**
 * @class TacticalVehicleController
 * @brief Orchestrates filtering, simulation, and derived tactical views.
 */
class TacticalVehicleController
{
public:
    explicit TacticalVehicleController(TacticalVehicleData& data);
    std::vector<const TacticalVehicle*> filteredVehicles;

private:
    TacticalVehicleData& data;
};

#endif // TACTICALVEHICLECONTROLLER_H
