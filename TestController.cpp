#include <QtTest>
#include "TacticalVehicleController.h"
#include "TacticalVehicleData.h"

class TestController : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testFilter_affiliation();
    void testFilter_callsign();
    void testFilter_combinedHasSatComHasActiveDefense();
    void testFilter_countMatches();
    void testFilter_datasetSize();
    void testFilter_distance();
    void testFilter_empty();
    void testFilter_fuel();
    void testFilter_fuelExact();
    void testFilter_hasSatCom();
    void testFilter_noFilter();
    void testFilter_protection();
    void testJSON_loading();
    void testSimulation_distance();
    void testSimulation_euclid();
    void testSimulation_fuel();
    void testSimulation_position();
    void testSimulation_speed();
    void testSort_classificationAsc();
    void testSort_distanceAsc();
    void testSort_distanceDesc();
    void testSort_fuelAsc();
    void testSort_priorityAsc();
    void testThreatscore_differences();
    void testThreatscore_values();
};

void TestController::initTestCase() {
}

void TestController::testFilter_affiliation() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.affiliation = "Hostile";
    controller.applyFilter(criteria);

    QVERIFY(!controller.filteredVehicles.empty());

    for (const TacticalVehicle* v : controller.filteredVehicles) {
        QVERIFY(v->affiliation == "Hostile");
    }
}

void TestController::testFilter_callsign() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.callsignActive = true;
    criteria.callsign = "KARHU 11";
    controller.applyFilter(criteria);

    QVERIFY(controller.filteredVehicles.size() == 1);
    QVERIFY(controller.filteredVehicles[0]->callsign == "KARHU 11");
}

void TestController::testFilter_combinedHasSatComHasActiveDefense() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.hasSatCom = true;
    criteria.hasActiveDefense = true;
    controller.applyFilter(criteria);

    QVERIFY(!controller.filteredVehicles.empty());

    for (const TacticalVehicle* v : controller.filteredVehicles) {
        QVERIFY(v->hasSatCom);
        QVERIFY(v->hasActiveDefense);
    }
}

void TestController::testFilter_countMatches() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    controller.applyFilter(criteria);
    unsigned long count = controller.countMatches(criteria);

    QVERIFY(controller.filteredVehicles.size() == count);
}

void TestController::testFilter_datasetSize() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.hasSatCom = true;
    controller.applyFilter(criteria);

    QVERIFY(!controller.filteredVehicles.empty());
    QVERIFY(controller.filteredVehicles.size() < data.vehicles().size());
}

void TestController::testFilter_distance() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    controller.updateSimulation(0, 0);
    criteria.distanceMax = 1000;
    controller.applyFilter(criteria);

    for (const TacticalVehicle* v : controller.filteredVehicles) {
        QVERIFY(v->distanceToTarget <= 1000);
    }
}

void TestController::testFilter_empty() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.callsignActive = true;
    criteria.callsign = "THIS_DOES_NOT_EXIST";
    controller.applyFilter(criteria);

    QVERIFY(controller.filteredVehicles.empty());
}

void TestController::testFilter_fuel() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.fuelMin = 80.0;
    controller.applyFilter(criteria);

    QVERIFY(!controller.filteredVehicles.empty());

    for (const TacticalVehicle* v : controller.filteredVehicles) {
        QVERIFY(v->fuelLevel >= 80.0);
    }
}

void TestController::testFilter_fuelExact() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.fuelMin = 80.0;
    criteria.fuelMax = 80.0;
    controller.applyFilter(criteria);

    for (const TacticalVehicle* v : controller.filteredVehicles) {
        QVERIFY(v->fuelLevel == 80.0);
    }
}

void TestController::testFilter_hasSatCom() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.hasSatCom = true;
    controller.applyFilter(criteria);

    QVERIFY(!controller.filteredVehicles.empty());

    for (const TacticalVehicle* v : controller.filteredVehicles) {
        QVERIFY(v->hasSatCom);
    }
}

void TestController::testFilter_noFilter() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    controller.applyFilter(criteria);

    QVERIFY(!data.vehicles().empty());
    QVERIFY(controller.filteredVehicles.size() == data.vehicles().size());
}

void TestController::testFilter_protection() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    FilterCriteria criteria;
    criteria.protectionMin = 3;
    criteria.protectionMax = 5;
    criteria.protectionMinActive = true;
    criteria.protectionMaxActive = true;
    controller.applyFilter(criteria);

    QVERIFY(!controller.filteredVehicles.empty());

    for (const TacticalVehicle* v : controller.filteredVehicles) {
        QVERIFY(v->protectionLevel >= 3);
        QVERIFY(v->protectionLevel <= 5);
    }
}

void TestController::testJSON_loading() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");

    QVERIFY(!data.vehicles().empty());
}

void TestController::testSimulation_distance() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);

    for (const auto& v : data.vehicles()) {
        QVERIFY(v.distanceToTarget >= 0);
    }
}

void TestController::testSimulation_euclid() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);

    for (const auto& v : data.vehicles()) {
        const double dx = 0 - v.posX;
        const double dy = 0 - v.posY;
        const double expected = std::sqrt(dx * dx + dy * dy);
        QVERIFY(std::abs(v.distanceToTarget - expected) < 1e-6);
    }
}

void TestController::testSimulation_fuel() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);

    for (const auto& v : data.vehicles()) {
        QVERIFY(v.fuelLevel >= 0);
        QVERIFY(v.fuelLevel <= 100);
    }
}

void TestController::testSimulation_position() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    std::deque<TacticalVehicle> testD = data.vehicles();
    controller.updateSimulation(0, 0);

    bool difference = false;

    for (unsigned long i = 0; i < data.vehicles().size(); ++i) {
        if (data.vehicles()[i].posX != testD[i].posX || data.vehicles()[i].posY != testD[i].posY) {
            difference = true;
            break;
        }
    }
    QVERIFY(difference);
}

void TestController::testSimulation_speed() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);

    for (const auto& v : data.vehicles()) {
        QVERIFY(v.speed >= 0);
        QVERIFY(v.speed <= v.maxSpeed);
    }
}

void TestController::testSort_classificationAsc() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");

    std::sort(data.vehiclesMutable().begin(), data.vehiclesMutable().end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByClassificationAsc(&a, &b); });

    for (unsigned long i = 1; i < data.vehiclesMutable().size(); ++i) {
        QVERIFY(data.vehiclesMutable()[i - 1].classification <= data.vehiclesMutable()[i].classification);
    }
}

void TestController::testSort_distanceAsc() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);

    std::sort(data.vehiclesMutable().begin(), data.vehiclesMutable().end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByDistanceAsc(&a, &b); });

    for (unsigned long i = 1; i < data.vehiclesMutable().size(); ++i) {
        QVERIFY(data.vehiclesMutable()[i - 1].distanceToTarget <= data.vehiclesMutable()[i].distanceToTarget);
    }
}

void TestController::testSort_distanceDesc() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);

    std::sort(data.vehiclesMutable().begin(), data.vehiclesMutable().end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByDistanceDesc(&a, &b); });

    for (unsigned long i = 1; i < data.vehiclesMutable().size(); ++i) {
        QVERIFY(data.vehiclesMutable()[i - 1].distanceToTarget >= data.vehiclesMutable()[i].distanceToTarget);
    }
}

void TestController::testSort_fuelAsc() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");

    std::sort(data.vehiclesMutable().begin(), data.vehiclesMutable().end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByFuelAsc(&a, &b); });

    for (unsigned long i = 1; i < data.vehiclesMutable().size(); ++i) {
        QVERIFY(data.vehiclesMutable()[i - 1].fuelLevel <= data.vehiclesMutable()[i].fuelLevel);
    }
}

void TestController::testSort_priorityAsc() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");

    std::sort(data.vehiclesMutable().begin(), data.vehiclesMutable().end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByPriorityAsc(&a, &b); });

    for (unsigned long i = 1; i < data.vehiclesMutable().size(); ++i) {
        QVERIFY(data.vehiclesMutable()[i - 1].priority <= data.vehiclesMutable()[i].priority);
    }
}

void TestController::testThreatscore_differences() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);
    controller.updateThreatScore();

    const auto& vehicles = data.vehicles();
    const double first = vehicles[0].threatScore;
    bool difference = false;

    for (const auto& v : vehicles) {
        if (v.threatScore != first) {
            difference = true;
            break;
        }
    }
    QVERIFY(difference);
}

void TestController::testThreatscore_values() {
    TacticalVehicleData data;
    data.loadVehiclesFromJson(":/data/vehicles.json");
    TacticalVehicleController controller(data);
    controller.updateSimulation(0, 0);
    controller.updateThreatScore();

    for (const auto& v : data.vehicles()) {
        QVERIFY(v.threatScore >= 0);
        QVERIFY(v.threatScore <= 100);
    }
}

QTEST_MAIN(TestController)
#include "TestController.moc"
