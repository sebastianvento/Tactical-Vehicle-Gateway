#include "TacticalVehicleData.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

// --- Tactical Vehicle Data Model ---
// Owns the persistent tactical dataset and provides JSON ingestion,
// controlled container access, and stateless sorting predicates.

// --- Construction ---
TacticalVehicleData::TacticalVehicleData() {
    // Intentionally minimal.
    // Data ingestion is explicitly triggered via loadVehiclesFromJson().
}

// --- Data Ingestion ---
/**
 * @brief Parses data from a JSON file and initializes internal containers.
 *
 * This function fully resets the internal dataset before loading to ensure
 * no stale or partially-loaded data remains.
 *
 * @param path The file system path or Qt resource path to the source JSON file.
 */
void TacticalVehicleData::loadVehiclesFromJson(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Data Error: Unable to open JSON file at" << path;
        return;
    }

    // Reset database to ensure a clean, deterministic state
    allVehicles.clear();

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    // Validation of JSON syntax and high-level structure
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Parse Error at offset" << parseError.offset << ":" << parseError.errorString();
        return;
    }
    if (!doc.isArray()) {
        qWarning() << "Structural Error: JSON root must be an array.";
        return;
    }

    QJsonArray array = doc.array();

    // Map JSON attributes to TacticalVehicle member variables
    for (const QJsonValue &value : std::as_const(array)) {
        QJsonObject obj = value.toObject();
        TacticalVehicle v;

        // Identity & Classification
        v.callsign       = obj["callsign"].toString();
        v.trackId        = obj["trackId"].toString();
        v.type           = obj["type"].toString();
        v.classification = obj["classification"].toString();
        v.affiliation    = obj["affiliation"].toString();
        v.priority       = obj["priority"].toString();
        v.domain         = obj["domain"].toString();
        v.propulsion     = obj["propulsion"].toString();
        v.natoIcon       = obj["natoIcon"].toString();

        // Capability Flags
        v.hasSatCom        = obj["hasSatCom"].toBool();
        v.isAmphibious     = obj["isAmphibious"].toBool();
        v.isUnmanned       = obj["isUnmanned"].toBool();
        v.hasActiveDefense = obj["hasActiveDefense"].toBool();

        // Telemetry & Performance Characteristics
        v.protectionLevel  = obj["protectionLevel"].toInt();
        v.speed            = obj["speed"].toDouble();
        v.maxSpeed         = obj["maxSpeed"].toDouble();
        v.targetSpeed      = obj["targetSpeed"].toDouble();
        v.fuelLevel        = obj["fuelLevel"].toDouble();
        v.ammunitionLevel  = obj["ammunitionLevel"].toDouble();
        v.posX             = obj["posX"].toDouble();
        v.posY             = obj["posY"].toDouble();
        v.heading          = obj["heading"].toDouble();

        // Distance is dynamically updated by the simulation engine
        v.distanceToTarget = 0.0;

        allVehicles.push_back(v);
    }

    qDebug() << "Loaded" << allVehicles.size() << "tactical vehicles.";
}

// --- Container Accessors ---
/**
 * @brief Read-only access to the master vehicle container.
 *
 * Intended for UI rendering and non-mutating inspection.
 */
const std::deque<TacticalVehicle>& TacticalVehicleData::vehicles() const {
    return allVehicles;
}

/**
 * @brief Mutable access to the vehicle container.
 *
 * Intended for controlled use cases such as:
 *  - Simulation updates
 *  - In-place sorting of the master dataset
 */
std::deque<TacticalVehicle>& TacticalVehicleData::vehiclesMutable() {
    return allVehicles;
}

// --- Sorting Predicates ---
bool TacticalVehicleData::sortByDistanceAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->distanceToTarget < b->distanceToTarget;
}

bool TacticalVehicleData::sortByDistanceDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->distanceToTarget > b->distanceToTarget;
}

bool TacticalVehicleData::sortByFuelAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->fuelLevel < b->fuelLevel;
}

bool TacticalVehicleData::sortByFuelDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->fuelLevel > b->fuelLevel;
}

bool TacticalVehicleData::sortByPriorityAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->priority < b->priority;
}

bool TacticalVehicleData::sortByPriorityDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->priority > b->priority;
}

bool TacticalVehicleData::sortByClassificationAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->classification < b->classification;
}

bool TacticalVehicleData::sortByClassificationDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->classification > b->classification;
}
