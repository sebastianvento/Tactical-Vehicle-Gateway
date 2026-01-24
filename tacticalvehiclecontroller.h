#ifndef TACTICALVEHICLECONTROLLER_H
#define TACTICALVEHICLECONTROLLER_H

#include <vector>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QString>


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
    void applyFilter(
        QCheckBox* cbHasSatCom,
        QCheckBox* cbIsAmphibious,
        QCheckBox* cbIsUnmanned,
        QCheckBox* cbHasActiveDefense,
        QPushButton* callsignSelectionPressed_Btn,
        QPushButton* trackIdSelectionPressed_Btn,
        QPushButton* domainButtonSelectionPressed_Btn,
        QPushButton* propulsionSelectionPressed_Btn,
        QPushButton* prioritySelectionPressed_Btn,
        QPushButton* protectionSelectionMinPressed_Btn,
        QPushButton* protectionSelectionMaxPressed_Btn,
        QCheckBox* affiliationButtonPlaceholder, // will be removed later
        int fuelMin,
        int fuelMax,
        int distanceMin,
        int distanceMax,
        const QString& affiliationText
        );

private:
    TacticalVehicleData& data;
};

#endif // TACTICALVEHICLECONTROLLER_H
