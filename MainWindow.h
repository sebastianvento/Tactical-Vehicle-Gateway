#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TacticalVehicle.h"
#include "TacticalVehicleController.h"
#include <QWidget>
#include <vector>

// Forward declarations to optimize compilation times
class QAction;
class QCheckBox;
class QCompleter;
class QLabel;
class QLineEdit;
class QListWidget;
class QMenu;
class QPushButton;
class QTimer;
class RangeSlider;
class TacticalVehicleData;

/**
 * @class MainWindow
 * @brief Main Tactical UI Controller for vehicle monitoring and filtering.
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // --- Core Tactical Logic ---
    void displayButtonClicked();    ///< Manual trigger to refresh tactical display
    void filterFunction();          ///< Primary data filtration engine
    void printList();               ///< Handles UI population of resultsList

    // --- Identity & Search Management ---
    void callsignChanged(const QString& text);
    void callsignReturnPressed();
    void callsignSelectionPressed();
    void trackIdChanged(const QString& text);
    void trackIdReturnPressed();
    void trackIdSelectionPressed();

    // --- Strategic Classification Menus ---
    void affiliationActionClicked(QAction* action);
    void domainActionClicked(QAction* action);
    void domainSelectionPressed();
    void priorityActionClicked(QAction* action);
    void prioritySelectionPressed();
    void propulsionActionClicked(QAction* action);
    void propulsionSelectionPressed();
    void protectionMenuMaxClicked(QAction* action);
    void protectionMenuMinClicked(QAction* action);
    void protectionSelectionMaxPressed();
    void protectionSelectionMinPressed();

    // --- Telemetry & Sensor Input Control ---
    void distanceInputMaxChanged(const QString& text);
    void distanceInputMinChanged(const QString& text);
    void distanceSliderChanged(int min, int max);
    void fuelInputMaxChanged(const QString& text);
    void fuelInputMinChanged(const QString& text);
    void fuelSliderChanged(int min, int max);

    // --- Tactical Sorting Engine ---
    void sortByClassificationAsc();
    void sortByClassificationDesc();
    void sortByDistanceAsc();
    void sortByDistanceDesc();
    void sortByFuelAsc();
    void sortByFuelDesc();
    void sortByPriorityAsc();
    void sortByPriorityDesc();

    // --- Simulation & Background Tasks ---
    void updateSimulation();        ///< Periodic update for dynamic asset data

private:
    // --- Backend Data Controllers ---
    std::unique_ptr<TacticalVehicleData> tacticalVehicleDb;
    std::unique_ptr<TacticalVehicleController> controller;
    QStringList trackIdList;
    QStringList callsignList;
    bool manualUpdateRequested = false;

    // --- Capability Filtration ---
    QCheckBox *cbHasActiveDefense;
    QCheckBox *cbHasSatCom;
    QCheckBox *cbIsAmphibious;
    QCheckBox *cbIsUnmanned;

    // --- Search & Auto-complete Components ---
    QLineEdit *callsignLine;
    QCompleter *callsignCompleter;
    QLineEdit *trackIdLine;
    QCompleter *trackIdCompleter;

    // --- UI Navigation & Menu Structures ---
    QPushButton *domainButton;
    QMenu *domainMenu;
    QPushButton *propulsionButton;
    QMenu *propulsionMenu;
    QPushButton *priorityButton;
    QMenu *priorityMenu;
    QPushButton *protectionButtonMax;
    QPushButton *protectionButtonMin;
    QMenu *protectionMenuMax;
    QMenu *protectionMenuMin;
    QMenu *affiliationMenu;
    QMenu *sortMenu;

    // --- Telemetry Input Widgets ---
    RangeSlider *distanceSlider;
    QLineEdit *distanceInputMax;
    QLineEdit *distanceInputMin;
    RangeSlider *fuelSlider;
    QLineEdit *fuelInputMax;
    QLineEdit *fuelInputMin;
    QLineEdit *targetXLine;
    QLineEdit *targetYLine;

    // --- Command & Feedback Elements ---
    QPushButton *displayButton;
    QPushButton *sortButton;
    QPushButton *affiliationButton;
    QIcon choiceDeletion;

    // --- Tactical Tag/Selection Indicators ---
    QPushButton *callsignSelectionPressed_Btn;
    QPushButton *domainButtonSelectionPressed_Btn;
    QPushButton *prioritySelectionPressed_Btn;
    QPushButton *propulsionSelectionPressed_Btn;
    QPushButton *protectionSelectionMaxPressed_Btn;
    QPushButton *protectionSelectionMinPressed_Btn;
    QPushButton *trackIdSelectionPressed_Btn;

    // --- Information Display & Labels ---
    QLabel *labelActiveDefense;
    QLabel *labelAmphibious;
    QLabel *labelCapabilities;
    QLabel *labelDistance;
    QLabel *labelDomain;
    QLabel *labelFuel;
    QLabel *labelIdentity;
    QLabel *labelLogo;
    QLabel *labelPriority;
    QLabel *labelProtection;
    QLabel *labelSatCom;
    QLabel *labelTrackId;
    QLabel *labelUnmanned;
    QListWidget *resultsList;

    // --- Internal Mathematical Helpers ---
    QTimer *simTimer;
    void updateSortStatus();
    double calculateDistance(double vehX, double vehY) const;
};

#endif // MAINWINDOW_H
