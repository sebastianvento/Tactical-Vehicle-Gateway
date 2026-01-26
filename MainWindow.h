#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TacticalVehicleController.h"

#include <QWidget>

// Forward declarations (compile-time optimization)
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
 * @brief Primary tactical UI controller for vehicle monitoring and filtering.
 *
 * Owns all UI state and presentation logic, delegates domain logic
 * (filtering, simulation) to TacticalVehicleController.
 */
class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // --- Core Logic ---
    void displayButtonClicked();    ///< Explicit trigger to refresh displayed results
    void filterFunction();          ///< Resolves UI state into filter criteria
    void printList();               ///< Populates resultsList from current data view

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

    // --- Telemetry & Input Control ---
    void distanceInputMaxChanged(const QString& text);
    void distanceInputMinChanged(const QString& text);
    void distanceSliderChanged(int min, int max);

    void fuelInputMaxChanged(const QString& text);
    void fuelInputMinChanged(const QString& text);
    void fuelSliderChanged(int min, int max);

    // --- Sorting ---
    void sortByClassificationAsc();
    void sortByClassificationDesc();

    void sortByDistanceAsc();
    void sortByDistanceDesc();

    void sortByFuelAsc();
    void sortByFuelDesc();

    void sortByPriorityAsc();
    void sortByPriorityDesc();

    // --- Simulation & Background Tasks ---
    void onSimulationTick();        ///< Periodic update for dynamic asset data

private:
    // --- Backend Data & Controllers ---
    std::unique_ptr<TacticalVehicleData> tacticalVehicleDb;
    std::unique_ptr<TacticalVehicleController> controller;

    QStringList trackIdList;
    QStringList callsignList;

    bool manualUpdateRequested = false; ///< Guards programmatic vs UI-triggered refresh

    // --- Capability Flags ---
    QCheckBox *cbHasActiveDefense;
    QCheckBox *cbHasSatCom;
    QCheckBox *cbIsAmphibious;
    QCheckBox *cbIsUnmanned;

    // --- Search & Auto-complete ---
    QLineEdit *callsignLine;
    QCompleter *callsignCompleter;

    QLineEdit *trackIdLine;
    QCompleter *trackIdCompleter;

    // --- Navigation & Menu Structures ---
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

    // --- Telemetry & Target Inputs ---
    RangeSlider *distanceSlider;
    QLineEdit *distanceInputMax;
    QLineEdit *distanceInputMin;

    RangeSlider *fuelSlider;
    QLineEdit *fuelInputMax;
    QLineEdit *fuelInputMin;

    QLineEdit *targetXLine;
    QLineEdit *targetYLine;

    // --- Command & Feedback Controls ---
    QPushButton *displayButton;
    QPushButton *sortButton;
    QPushButton *affiliationButton;

    QIcon choiceDeletion;

    // --- Selection / Tag Indicators ---
    QPushButton *callsignSelectionPressed_Btn;
    QPushButton *domainButtonSelectionPressed_Btn;
    QPushButton *prioritySelectionPressed_Btn;
    QPushButton *propulsionSelectionPressed_Btn;
    QPushButton *protectionSelectionMaxPressed_Btn;
    QPushButton *protectionSelectionMinPressed_Btn;
    QPushButton *trackIdSelectionPressed_Btn;

    // --- Information Display ---
    QLabel *labelLogo;
    QLabel *labelTrackId;
    QListWidget *resultsList;

    // --- Timing & Helpers ---
    QTimer *simTimer;

    void updateSortStatus();
};

#endif // MAINWINDOW_H

