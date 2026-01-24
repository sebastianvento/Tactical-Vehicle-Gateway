#include "TacticalVehicleController.h"
#include "TacticalVehicleData.h"

TacticalVehicleController::TacticalVehicleController(TacticalVehicleData& data)
    : data(data)
{
}

void TacticalVehicleController::applyFilter(
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
    QCheckBox* affiliationButtonPlaceholder, // intentionally unused for now
    int fuelMin,
    int fuelMax,
    int distanceMin,
    int distanceMax,
    const QString& affiliationText
    )
{
    Q_UNUSED(affiliationButtonPlaceholder);

    filteredVehicles.clear();

    for (const auto& vehicle : data.allVehicles) {

        bool capabilityMatch = true;
        bool callsignMatch = false;
        bool trackIdMatch = false;
        bool domainMatch = false;
        bool propulsionMatch = false;
        bool priorityMatch = false;
        bool protectionMatchMin = false;
        bool protectionMatchMax = false;
        bool fuelMatchMin = false;
        bool fuelMatchMax = false;
        bool distanceMatchMin = false;
        bool distanceMatchMax = false;
        bool affiliationMatch = false;

        // --- 1. Capabilities (Strict "AND" Logic) ---
        if (cbHasSatCom->isChecked() && !vehicle.hasSatCom) {
            capabilityMatch = false;
        }
        if (cbIsAmphibious->isChecked() && !vehicle.isAmphibious) {
            capabilityMatch = false;
        }
        if (cbIsUnmanned->isChecked() && !vehicle.isUnmanned) {
            capabilityMatch = false;
        }
        if (cbHasActiveDefense->isChecked() && !vehicle.hasActiveDefense) {
            capabilityMatch = false;
        }

        // --- 2. Identity ---
        if (callsignSelectionPressed_Btn->isVisible()) {
            callsignMatch = (vehicle.callsign == callsignSelectionPressed_Btn->text());
        } else {
            callsignMatch = true;
        }

        if (trackIdSelectionPressed_Btn->isVisible()) {
            trackIdMatch = (vehicle.trackId == trackIdSelectionPressed_Btn->text());
        } else {
            trackIdMatch = true;
        }

        // --- 3. Strategic Classifications ---
        if (domainButtonSelectionPressed_Btn->isVisible()) {
            domainMatch = (vehicle.domain == domainButtonSelectionPressed_Btn->text());
        } else {
            domainMatch = true;
        }

        if (propulsionSelectionPressed_Btn->isVisible()) {
            propulsionMatch = (vehicle.propulsion == propulsionSelectionPressed_Btn->text());
        } else {
            propulsionMatch = true;
        }

        if (prioritySelectionPressed_Btn->isVisible()) {
            priorityMatch = (vehicle.priority == prioritySelectionPressed_Btn->text());
        } else {
            priorityMatch = true;
        }

        // --- 4. Protection Levels ---
        if (protectionSelectionMinPressed_Btn->isVisible()) {
            protectionMatchMin = (vehicle.protectionLevel >= protectionSelectionMinPressed_Btn->text().toInt());
        } else {
            protectionMatchMin = true;
        }

        if (protectionSelectionMaxPressed_Btn->isVisible()) {
            protectionMatchMax = (vehicle.protectionLevel <= protectionSelectionMaxPressed_Btn->text().toInt());
        } else {
            protectionMatchMax = true;
        }

        // --- 5. Telemetry Ranges ---
        if (vehicle.fuelLevel >= fuelMin) {
            fuelMatchMin = true;
        }
        if (vehicle.fuelLevel <= fuelMax) {
            fuelMatchMax = true;
        }

        if (vehicle.distanceToTarget >= distanceMin) {
            distanceMatchMin = true;
        }
        if (distanceMax >= 10000) {
            distanceMatchMax = true;
        }
        else if (vehicle.distanceToTarget <= distanceMax) {
            distanceMatchMax = true;
        }

        // --- 6. Affiliation ---
        if (affiliationText == "All Types" ||
            vehicle.affiliation == affiliationText) {
            affiliationMatch = true;
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
