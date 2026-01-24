#ifndef TACTICALVEHICLECONTROLLER_H
#define TACTICALVEHICLECONTROLLER_H

class TacticalVehicleData;

/**
 * @class TacticalVehicleController
 * @brief Orchestrates filtering, simulation, and derived tactical views.
 */
class TacticalVehicleController
{
public:
    explicit TacticalVehicleController(TacticalVehicleData& data);

private:
    TacticalVehicleData& data;
};

#endif // TACTICALVEHICLECONTROLLER_H
