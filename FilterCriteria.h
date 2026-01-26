#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include <QString>

/**
 * @struct FilterCriteria
 * @brief Aggregates all filter inputs resolved from UI state.
 *
 * Pure data container. No logic.
 */
struct FilterCriteria
{
    // --- Operational Capabilities ---
    bool hasSatCom = false;
    bool isAmphibious = false;
    bool isUnmanned = false;
    bool hasActiveDefense = false;

    // --- Identity Filters ---
    bool callsignActive = false;
    QString callsign;

    bool trackIdActive = false;
    QString trackId;

    // --- Strategic Classification ---
    bool domainActive = false;
    QString domain;

    bool propulsionActive = false;
    QString propulsion;

    bool priorityActive = false;
    QString priority;

    // --- Protection Constraints ---
    bool protectionMinActive = false;
    int protectionMin = 0;

    bool protectionMaxActive = false;
    int protectionMax = 0;

    // --- Telemetry Ranges ---
    int fuelMin = 0;
    int fuelMax = 100;

    int distanceMin = 0;
    int distanceMax = 10000;

    // --- Affiliation ---
    QString affiliation = "All Types";
};

#endif // FILTERCRITERIA_H
