#include "TacticalVehicleData.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

// --- TacticalVehicleData ---
// Owns the persistent tactical dataset and provides JSON ingestion,
// controlled container access, and stateless sorting predicates.

// --- Construction ---
TacticalVehicleData::TacticalVehicleData() {
    // Intentionally minimal.
    // Data ingestion is explicitly triggered via loadVehiclesFromJson().
}

// --- Data Ingestion ---
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

        // Capability
        v.hasSatCom        = obj["hasSatCom"].toBool();
        v.isAmphibious     = obj["isAmphibious"].toBool();
        v.isUnmanned       = obj["isUnmanned"].toBool();
        v.hasActiveDefense = obj["hasActiveDefense"].toBool();

        // Identity
        v.callsign       = obj["callsign"].toString();
        v.trackId        = obj["trackId"].toString();
        v.type           = obj["type"].toString();

        // Affiliation
        v.affiliation    = obj["affiliation"].toString();

        // Strategic Classification
        v.classification = obj["classification"].toString();
        v.domain         = obj["domain"].toString();
        v.propulsion     = obj["propulsion"].toString();
        v.priority       = obj["priority"].toString();
        v.natoIcon       = obj["natoIcon"].toString();

        // Protection
        v.protectionLevel  = obj["protectionLevel"].toInt();

        // Telemetry & Performance Characteristics
        v.maxSpeed         = obj["maxSpeed"].toDouble();
        v.speed            = obj["speed"].toDouble();
        v.targetSpeed      = obj["targetSpeed"].toDouble();
        v.heading          = obj["heading"].toDouble();
        v.posX             = obj["posX"].toDouble();
        v.posY             = obj["posY"].toDouble();
        v.fuelLevel        = obj["fuelLevel"].toDouble();
        v.ammunitionLevel  = obj["ammunitionLevel"].toDouble();

        // Distance To Target (dynamically updated by the simulation engine)
        v.distanceToTarget = 0.0;

        allVehicles.push_back(v);
    }

    qDebug() << "Loaded" << allVehicles.size() << "tactical vehicles.";
}

// --- Container Accessors ---
const std::deque<TacticalVehicle>& TacticalVehicleData::vehicles() const {
    return allVehicles;
}

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

bool TacticalVehicleData::sortByThreatAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->threatScore < b->threatScore;
}

bool TacticalVehicleData::sortByThreatDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->threatScore > b->threatScore;
}

bool TacticalVehicleData::sortByPriorityAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->priority < b->priority;
}

bool TacticalVehicleData::sortByPriorityDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->priority > b->priority;
}

bool TacticalVehicleData::sortByFuelAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->fuelLevel < b->fuelLevel;
}

bool TacticalVehicleData::sortByFuelDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->fuelLevel > b->fuelLevel;
}

bool TacticalVehicleData::sortByClassificationAsc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->classification < b->classification;
}

bool TacticalVehicleData::sortByClassificationDesc(const TacticalVehicle* a, const TacticalVehicle* b) {
    return a->classification > b->classification;
}
