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
class QDialog;
class QListWidgetItem;

class RangeSlider;
class TacticalVehicleData;

/**
 * @class MainWindow
 * @brief Primary UI controller for vehicle monitoring and filtering.
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
    void displayButtonClicked();                       ///< Explicit trigger to refresh displayed results
    void filterFunction();                             ///< Resolves UI state into filter criteria
    void filtersCleared();
    void printList();                                  ///< Populates resultsList from current data view
    void listItemDoubleclicked(QListWidgetItem *item); ///< Shows dialog with entity info when QListWidget item clicked.

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
    // --- Sort Modes ---
    enum class SortMode {
        None,
        DistanceAsc,
        DistanceDesc,
        FuelAsc,
        FuelDesc,
        PriorityAsc,
        PriorityDesc,
        ClassificationAsc,
        ClassificationDesc
    };

    SortMode currentSortMode = SortMode::None;

    // --- Backend Data & Controllers ---
    std::unique_ptr<TacticalVehicleData> tacticalVehicleDb;
    std::unique_ptr<TacticalVehicleController> controller;

    QStringList trackIdList;
    QStringList callsignList;

    bool manualUpdateRequested = false; ///< Guards explicit list rendering phases

    bool callsignFilterActive = false;
    bool trackIdFilterActive = false;
    bool domainFilterActive = false;
    bool propulsionFilterActive = false;
    bool priorityFilterActive = false;
    bool protectionMinFilterActive = false;
    bool protectionMaxFilterActive = false;
    QString activeCallsign;
    QString activeTrackId;
    QString activeAffiliation = "All Types";
    QString activeDomain;
    QString activePropulsion;
    QString activePriority;
    int activeProtectionMin;
    int activeProtectionMax;

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

    QPushButton *affiliationButton;
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
    QPushButton *clearButton;
    QCheckBox *liveUpdatesBox;

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
    QListWidget *resultsList;

    // --- Dialogs ---
    QDialog *entityDialog;

    // --- Timing & Helpers ---
    QTimer *simTimer;
};

#endif // MAINWINDOW_H
