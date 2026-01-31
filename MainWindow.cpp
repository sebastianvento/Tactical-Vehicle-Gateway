#include "MainWindow.h"
#include "TacticalVehicleData.h"
#include "RangeSlider.h"

#include <QApplication>
#include <QObject>
#include <QPushButton>
#include <QListWidget>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QCompleter>
#include <QIcon>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QDialog>
#include <QListWidgetItem>

#include <vector>
#include <algorithm>

/**
 * @brief Constructs and wires the main tactical gateway UI.
 *
 * Responsible for:
 *  - Initializing core data and controller objects
 *  - Building the full UI layout hierarchy
 *  - Connecting UI state to filtering, sorting, and simulation logic
 */
MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    // --- DATA & CORE INITIALIZATION ---
    tacticalVehicleDb = std::make_unique<TacticalVehicleData>();
    controller = std::make_unique<TacticalVehicleController>(*tacticalVehicleDb);
    tacticalVehicleDb->loadVehiclesFromJson(":/data/vehicles.json");
    choiceDeletion = QIcon::fromTheme(QIcon::ThemeIcon::WindowClose);

    setMinimumSize(1000, 720);
    this->resize(1000, 750);

    // --- LAYOUT ARCHITECTURE ---
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *leftPanel = new QVBoxLayout();
    QVBoxLayout *rightPanel = new QVBoxLayout();

    rightPanel->setContentsMargins(0, 15, 0, 0);
    rightPanel->setSpacing(0);

    // --- LEFT PANEL: UI GROUPS ---
    // Section: Operational Capabilities
    QGroupBox *capGroup = new QGroupBox("Operational Capabilities");
    QGridLayout *capGrid = new QGridLayout();
    capGrid->addWidget(new QLabel("SATCOM Link"), 0, 0);
    capGrid->addWidget(cbHasSatCom = new QCheckBox(), 1, 0);
    capGrid->addWidget(new QLabel("Amphibious"), 0, 1);
    capGrid->addWidget(cbIsAmphibious = new QCheckBox(), 1, 1);
    capGrid->addWidget(new QLabel("Unmanned (UxV)"), 0, 2);
    capGrid->addWidget(cbIsUnmanned = new QCheckBox(), 1, 2);
    capGrid->addWidget(new QLabel("Active Defense"), 0, 3);
    capGrid->addWidget(cbHasActiveDefense = new QCheckBox(), 1, 3);
    capGroup->setLayout(capGrid);
    leftPanel->addWidget(capGroup);

    // Section: Asset Identity & Affiliation
    QGroupBox *idGroup = new QGroupBox("Asset Identity & Affiliation");
    QGridLayout *idGrid = new QGridLayout();

    idGrid->addWidget(new QLabel("Callsign:"), 0, 0);
    idGrid->addWidget(callsignLine = new QLineEdit(), 0, 1);
    idGrid->addWidget(callsignSelectionPressed_Btn = new QPushButton(), 0, 2);
    callsignSelectionPressed_Btn->setIcon(choiceDeletion);
    callsignSelectionPressed_Btn->setVisible(false);
    idGrid->addWidget(new QLabel("Affiliation:"), 0, 3);

    idGrid->addWidget(new QLabel("Track ID:"), 1, 0);
    idGrid->addWidget(trackIdLine = new QLineEdit(), 1, 1);
    idGrid->addWidget(trackIdSelectionPressed_Btn = new QPushButton(), 1, 2);
    trackIdSelectionPressed_Btn->setIcon(choiceDeletion);
    trackIdSelectionPressed_Btn->setVisible(false);

    affiliationButton = new QPushButton("All Types");
    affiliationMenu = new QMenu(this);
    affiliationMenu->addAction("All Types");
    affiliationMenu->addAction("Friendly");
    affiliationMenu->addAction("Hostile");
    affiliationMenu->addAction("Neutral");
    affiliationMenu->addAction("Unknown");
    affiliationButton->setMenu(affiliationMenu);
    idGrid->addWidget(affiliationButton, 1, 3);

    idGroup->setLayout(idGrid);
    leftPanel->addWidget(idGroup);

    // Section: Strategic Classification
    QGroupBox *stratGroup = new QGroupBox("Strategic Classification");
    QGridLayout *stratGrid = new QGridLayout();

    stratGrid->addWidget(new QLabel("Operational Domain:"), 0, 0);
    domainButton = new QPushButton("Select Domain");
    domainMenu = new QMenu(this);
    domainMenu->addAction("Land");
    domainMenu->addAction("Air");
    domainMenu->addAction("Maritime");
    domainMenu->addAction("Subsurface");
    domainMenu->addAction("Space");
    domainMenu->addAction("Electronic");
    domainButton->setMenu(domainMenu);
    stratGrid->addWidget(domainButton, 0, 1);
    stratGrid->addWidget(domainButtonSelectionPressed_Btn = new QPushButton(), 0, 2);
    domainButtonSelectionPressed_Btn->setIcon(choiceDeletion);
    domainButtonSelectionPressed_Btn->setVisible(false);

    stratGrid->addWidget(new QLabel("Locomotion Type:"), 1, 0);
    propulsionButton = new QPushButton("Select Type");
    propulsionMenu = new QMenu(this);
    propulsionMenu->addAction("Wheeled");
    propulsionMenu->addAction("Tracked");
    propulsionMenu->addAction("Legged");
    propulsionMenu->addAction("Aerial");
    propulsionMenu->addAction("Maritime");
    propulsionButton->setMenu(propulsionMenu);
    stratGrid->addWidget(propulsionButton, 1, 1);
    stratGrid->addWidget(propulsionSelectionPressed_Btn = new QPushButton(), 1, 2);
    propulsionSelectionPressed_Btn->setIcon(choiceDeletion);
    propulsionSelectionPressed_Btn->setVisible(false);

    stratGrid->addWidget(new QLabel("Strategic Priority:"), 2, 0);
    priorityButton = new QPushButton("Set Priority");
    priorityMenu = new QMenu(this);
    priorityMenu->addAction("Flash");
    priorityMenu->addAction("High");
    priorityMenu->addAction("Routine");
    priorityMenu->addAction("Low");
    priorityButton->setMenu(priorityMenu);
    stratGrid->addWidget(priorityButton, 2, 1);
    stratGrid->addWidget(prioritySelectionPressed_Btn = new QPushButton(), 2, 2);
    prioritySelectionPressed_Btn->setIcon(choiceDeletion);
    prioritySelectionPressed_Btn->setVisible(false);

    stratGroup->setLayout(stratGrid);
    leftPanel->addWidget(stratGroup);

    // Section: Protection Level
    QGroupBox *protGroup = new QGroupBox("Protection Level (STANAG 4569)");
    QHBoxLayout *protLayout = new QHBoxLayout();
    protectionButtonMin = new QPushButton("Min Level");
    protectionButtonMax = new QPushButton("Max Level");
    protectionMenuMin = new QMenu(this);
    protectionMenuMax = new QMenu(this);
    for (int lvl = 1; lvl <= 6; ++lvl) {
        protectionMenuMin->addAction(QString::number(lvl));
        protectionMenuMax->addAction(QString::number(lvl));
    }
    protectionButtonMin->setMenu(protectionMenuMin);
    protectionButtonMax->setMenu(protectionMenuMax);
    protLayout->addWidget(protectionButtonMin);
    protLayout->addWidget(protectionButtonMax);
    protLayout->addWidget(protectionSelectionMinPressed_Btn = new QPushButton());
    protLayout->addWidget(protectionSelectionMaxPressed_Btn = new QPushButton());
    protectionSelectionMinPressed_Btn->setIcon(choiceDeletion);
    protectionSelectionMaxPressed_Btn->setIcon(choiceDeletion);
    protectionSelectionMinPressed_Btn->setVisible(false);
    protectionSelectionMaxPressed_Btn->setVisible(false);
    protGroup->setLayout(protLayout);
    leftPanel->addWidget(protGroup);

    // Section: Telemetry & Target Inputs
    QGroupBox *teleGroup = new QGroupBox("Telemetry & Mission Target");
    QGridLayout *teleGrid = new QGridLayout();
    QIntValidator *fuelValidator = new QIntValidator(0,100, this);
    teleGrid->addWidget(new QLabel("Fuel Level (%):"), 0, 0);
    fuelSlider = new RangeSlider();
    fuelSlider->setRange(0, 100);
    fuelSlider->setValues(0, 100);
    teleGrid->addWidget(fuelSlider, 0, 1, 1, 2);
    teleGrid->addWidget(fuelInputMin = new QLineEdit("0"), 1, 1);
    fuelInputMin->setValidator(fuelValidator);
    teleGrid->addWidget(fuelInputMax = new QLineEdit("100"), 1, 2);
    fuelInputMax->setValidator(fuelValidator);

    QIntValidator *distanceValidator = new QIntValidator(0,10000, this);
    teleGrid->addWidget(new QLabel("Distance to Target (m):"), 2, 0);
    distanceSlider = new RangeSlider();
    distanceSlider->setRange(0, 10000);
    distanceSlider->setValues(0, 10000);
    teleGrid->addWidget(distanceSlider, 2, 1, 1, 2);
    teleGrid->addWidget(distanceInputMin = new QLineEdit("0"), 3, 1);
    distanceInputMin->setValidator(distanceValidator);
    teleGrid->addWidget(distanceInputMax = new QLineEdit("MAX (No Limit)"), 3, 2);
    distanceInputMax->setValidator(distanceValidator);

    teleGrid->addWidget(new QLabel("Target (X, Y):"), 4, 0);
    QHBoxLayout *coordLayout = new QHBoxLayout();
    QDoubleValidator *coordValidator = new QDoubleValidator(-999999.0, 999999.0, 2, this);
    targetXLine = new QLineEdit("0.00");
    targetXLine->setPlaceholderText("X");
    targetXLine->setValidator(coordValidator);
    targetYLine = new QLineEdit("0.00");
    targetYLine->setPlaceholderText("Y");
    targetYLine->setValidator(coordValidator);
    coordLayout->addWidget(targetXLine);
    coordLayout->addWidget(targetYLine);
    teleGrid->addLayout(coordLayout, 4, 1, 1, 2);

    teleGroup->setLayout(teleGrid);
    leftPanel->addWidget(teleGroup);

    // Section: Command Buttons
    leftPanel->addStretch();
    displayButton = new QPushButton("DISPLAY RESULTS (" + QString::number(tacticalVehicleDb->vehicles().size()) + ")");
    displayButton->setMinimumHeight(50);
    displayButton->setStyleSheet(
        "QPushButton { background-color: #1a2a3a; color: white; border: 1px solid #334466; } "
        "QPushButton:hover { background-color: #253b52; border: 1px solid #4466aa; } "
        "QPushButton:pressed { background-color: #111c26; }"
        );

    QPushButton *exitButton = new QPushButton("EXIT SYSTEM");
    exitButton->setMinimumHeight(50);
    exitButton->setStyleSheet(
        "QPushButton { background-color: #2a2a2a; color: #cc5555; border: 1px solid #444; } "
        "QPushButton:hover { background-color: #352b2b; color: #ff6666; border: 1px solid #663333; } "
        "QPushButton:pressed { background-color: #1a1a1a; }"
        );

    leftPanel->addWidget(displayButton);
    leftPanel->addSpacing(5);
    leftPanel->addWidget(exitButton);

    // --- RIGHT PANEL: RESULTS & SORTING ---
    // Header Logo Bar
    QWidget *headerBar = new QWidget();
    headerBar->setObjectName("headerBar");
    headerBar->setStyleSheet("QWidget#headerBar { background-color: #1a2a3a; border-radius: 4px; }");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerBar);
    labelLogo = new QLabel("TACTICAL VEHICLE GATEWAY");
    QFont logoFont("Arial", 14, QFont::Bold);
    logoFont.setLetterSpacing(QFont::PercentageSpacing, 110);
    labelLogo->setFont(logoFont);
    labelLogo->setStyleSheet("color: white; border: none; background: transparent;");
    labelLogo->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(labelLogo);
    headerLayout->setContentsMargins(10, 5, 10, 5);
    rightPanel->addWidget(headerBar);

    // Sort Menu Actions
    QHBoxLayout *sortBarLayout = new QHBoxLayout();
    clearButton = new QPushButton("Clear all filters");
    QLabel *liveUpdateLabel = new QLabel("Live Updates");
    liveUpdateLabel->setContentsMargins(10, 0, 10, 0);
    liveUpdatesBox = new QCheckBox;
    sortBarLayout->setContentsMargins(0, 10, 0, 0);
    sortBarLayout->addWidget(clearButton);
    sortBarLayout->addWidget(liveUpdateLabel);
    sortBarLayout->addWidget(liveUpdatesBox);
    sortBarLayout->addStretch();
    sortButton = new QPushButton("Sort");
    sortMenu = new QMenu(this);
    QAction* actionDistAsc = new QAction("Distance: Closest First", this);
    QAction* actionDistDesc = new QAction("Distance: Farthest First", this);
    QAction* actionFuelAsc = new QAction("Fuel: Critical First", this);
    QAction* actionFuelDesc = new QAction("Fuel: Full First", this);
    QAction* actionPriorityAsc = new QAction("Priority (A-Z)", this);
    QAction* actionPriorityDesc = new QAction("Priority (Z-A)", this);
    QAction* actionClassAsc = new QAction("Classification (A-Z)", this);
    QAction* actionClassDesc = new QAction("Classification (Z-A)", this);
    sortMenu->addActions({actionDistAsc, actionDistDesc, actionFuelAsc, actionFuelDesc, actionPriorityAsc, actionPriorityDesc, actionClassAsc, actionClassDesc});
    sortButton->setMenu(sortMenu);
    sortBarLayout->addWidget(sortButton);
    rightPanel->addLayout(sortBarLayout);

    // Result List
    resultsList = new QListWidget();
    rightPanel->addWidget(resultsList);

    // --- FINAL LAYOUT ASSEMBLY ---
    mainLayout->addLayout(leftPanel, 1);
    mainLayout->addLayout(rightPanel, 2);
    setLayout(mainLayout);

    // --- SIGNAL & SLOT CONNECTIONS ---
    // Capability Toggles
    connect(cbHasSatCom, &QCheckBox::toggled, this, &MainWindow::filterFunction);
    connect(cbIsAmphibious, &QCheckBox::toggled, this, &MainWindow::filterFunction);
    connect(cbIsUnmanned, &QCheckBox::toggled, this, &MainWindow::filterFunction);
    connect(cbHasActiveDefense, &QCheckBox::toggled, this, &MainWindow::filterFunction);

    // Search & Selection
    connect(callsignLine, &QLineEdit::textChanged, this, &MainWindow::callsignChanged);
    connect(callsignLine, &QLineEdit::returnPressed, this, &MainWindow::callsignReturnPressed);
    connect(callsignSelectionPressed_Btn, &QPushButton::clicked, this, &MainWindow::callsignSelectionPressed);
    connect(trackIdLine, &QLineEdit::textChanged, this, &MainWindow::trackIdChanged);
    connect(trackIdLine, &QLineEdit::returnPressed, this, &MainWindow::trackIdReturnPressed);
    connect(trackIdSelectionPressed_Btn, &QPushButton::clicked, this, &MainWindow::trackIdSelectionPressed);

    // Strategic Menus
    connect(affiliationMenu, &QMenu::triggered, this, &MainWindow::affiliationActionClicked);
    connect(domainMenu, &QMenu::triggered, this, &MainWindow::domainActionClicked);
    connect(domainButtonSelectionPressed_Btn, &QPushButton::clicked, this, &MainWindow::domainSelectionPressed);
    connect(propulsionMenu, &QMenu::triggered, this, &MainWindow::propulsionActionClicked);
    connect(propulsionSelectionPressed_Btn, &QPushButton::clicked, this, &MainWindow::propulsionSelectionPressed);
    connect(priorityMenu, &QMenu::triggered, this, &MainWindow::priorityActionClicked);
    connect(prioritySelectionPressed_Btn, &QPushButton::clicked, this, &MainWindow::prioritySelectionPressed);
    connect(protectionMenuMin, &QMenu::triggered, this, &MainWindow::protectionMenuMinClicked);
    connect(protectionMenuMax, &QMenu::triggered, this, &MainWindow::protectionMenuMaxClicked);
    connect(protectionSelectionMinPressed_Btn, &QPushButton::clicked, this, &MainWindow::protectionSelectionMinPressed);
    connect(protectionSelectionMaxPressed_Btn, &QPushButton::clicked, this, &MainWindow::protectionSelectionMaxPressed);

    // Range Sliders & Inputs
    connect(fuelSlider, &RangeSlider::valuesChanged, this, &MainWindow::fuelSliderChanged);
    connect(fuelInputMin, &QLineEdit::textChanged, this, &MainWindow::fuelInputMinChanged);
    connect(fuelInputMax, &QLineEdit::textChanged, this, &MainWindow::fuelInputMaxChanged);
    connect(distanceSlider, &RangeSlider::valuesChanged, this, &MainWindow::distanceSliderChanged);
    connect(distanceInputMin, &QLineEdit::textChanged, this, &MainWindow::distanceInputMinChanged);
    connect(distanceInputMax, &QLineEdit::textChanged, this, &MainWindow::distanceInputMaxChanged);
    connect(targetXLine, &QLineEdit::textChanged, this, &MainWindow::filterFunction);
    connect(targetYLine, &QLineEdit::textChanged, this, &MainWindow::filterFunction);

    // Filter Clearing
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::filtersCleared);

    // Sorting & Application Actions
    connect(displayButton, &QPushButton::clicked, this, &MainWindow::displayButtonClicked);
    connect(actionDistAsc, &QAction::triggered, this, &MainWindow::sortByDistanceAsc);
    connect(actionDistDesc, &QAction::triggered, this, &MainWindow::sortByDistanceDesc);
    connect(actionFuelAsc, &QAction::triggered, this, &MainWindow::sortByFuelAsc);
    connect(actionFuelDesc, &QAction::triggered, this, &MainWindow::sortByFuelDesc);
    connect(actionPriorityAsc, &QAction::triggered, this, &MainWindow::sortByPriorityAsc);
    connect(actionPriorityDesc, &QAction::triggered, this, &MainWindow::sortByPriorityDesc);
    connect(actionClassAsc, &QAction::triggered, this, &MainWindow::sortByClassificationAsc);
    connect(actionClassDesc, &QAction::triggered, this, &MainWindow::sortByClassificationDesc);
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    // Listwidget dialog
    connect(resultsList, &QListWidget::itemDoubleClicked, this, &MainWindow::listItemDoubleclicked);

    // --- AUTO-COMPLETE & DYNAMIC UPDATES ---
    // Populate Search Data
    for (const auto& v : tacticalVehicleDb->vehicles()) {
        if (!v.callsign.isEmpty() && !callsignList.contains(v.callsign)) {
            callsignList << v.callsign;
        }
        if (!v.trackId.isEmpty() && !trackIdList.contains(v.trackId)) {
            trackIdList << v.trackId;
        }
    }

    callsignCompleter = new QCompleter(callsignList, this);
    callsignCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    callsignLine->setCompleter(callsignCompleter);

    trackIdCompleter = new QCompleter(trackIdList, this);
    trackIdCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    trackIdCompleter->setFilterMode(Qt::MatchContains);
    trackIdLine->setCompleter(trackIdCompleter);

    // Simulation Heartbeat
    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &MainWindow::onSimulationTick);
    simTimer->start(1000);
}

// --- Filtering Logic ---
// Resolves UI state into filter criteria and delegates evaluation to the controller.
void MainWindow::filterFunction() {
    FilterCriteria criteria;

    // --- Capability Flags ---
    criteria.hasSatCom = cbHasSatCom->isChecked();
    criteria.isAmphibious = cbIsAmphibious->isChecked();
    criteria.isUnmanned = cbIsUnmanned->isChecked();
    criteria.hasActiveDefense = cbHasActiveDefense->isChecked();

    // --- Identity Filters ---
    criteria.callsignActive = callsignSelectionPressed_Btn->isVisible();
    criteria.callsign = callsignSelectionPressed_Btn->text();
    criteria.trackIdActive = trackIdSelectionPressed_Btn->isVisible();
    criteria.trackId = trackIdSelectionPressed_Btn->text();

    // --- Strategic Classification ---
    criteria.domainActive = domainButtonSelectionPressed_Btn->isVisible();
    criteria.domain = domainButtonSelectionPressed_Btn->text();

    criteria.propulsionActive = propulsionSelectionPressed_Btn->isVisible();
    criteria.propulsion = propulsionSelectionPressed_Btn->text();

    criteria.priorityActive = prioritySelectionPressed_Btn->isVisible();
    criteria.priority = prioritySelectionPressed_Btn->text();

    // --- Protection Constraints ---
    criteria.protectionMinActive = protectionSelectionMinPressed_Btn->isVisible();
    criteria.protectionMin = protectionSelectionMinPressed_Btn->text().toInt();

    criteria.protectionMaxActive = protectionSelectionMaxPressed_Btn->isVisible();
    criteria.protectionMax = protectionSelectionMaxPressed_Btn->text().toInt();

    // --- Telemetry Ranges ---
    criteria.fuelMin = fuelSlider->lowerValue();
    criteria.fuelMax = fuelSlider->upperValue();
    criteria.distanceMin = distanceSlider->lowerValue();
    criteria.distanceMax = distanceSlider->upperValue();

    // --- Affiliation ---
    criteria.affiliation = affiliationButton->text();

    controller->applyFilter(criteria);

    if (!controller->isFilterActive()) {
        displayButton->setText(
            "DISPLAY RESULTS (" +
            QString::number(tacticalVehicleDb->vehicles().size()) + ")"
            );
    } else {
        displayButton->setText(
            "DISPLAY RESULTS (" +
            QString::number(controller->filteredVehicles.size()) + ")"
            );
    }
}

void MainWindow::filtersCleared() {
    // --- Capability Flags ---
    cbHasSatCom->setCheckState(Qt::Unchecked);
    cbIsAmphibious->setCheckState(Qt::Unchecked);
    cbIsUnmanned->setCheckState(Qt::Unchecked);
    cbHasActiveDefense->setCheckState(Qt::Unchecked);

    // --- Identity Filters ---
    callsignSelectionPressed_Btn->setVisible(false);
    callsignLine->setText("");
    trackIdSelectionPressed_Btn->setVisible(false);
    trackIdLine->setText("");

    // --- Strategic Classification ---
    domainButtonSelectionPressed_Btn->setVisible(false);
    domainButtonSelectionPressed_Btn->setText("");
    domainButton->setText("Select Domain");

    propulsionSelectionPressed_Btn->setVisible(false);
    propulsionSelectionPressed_Btn->setText("");
    propulsionButton->setText("Select Type");

    prioritySelectionPressed_Btn->setVisible(false);
    prioritySelectionPressed_Btn->setText("");
    priorityButton->setText("Set Priority");

    // --- Protection Constraints ---
    protectionSelectionMinPressed_Btn->setVisible(false);
    protectionSelectionMinPressed_Btn->setText("");
    protectionButtonMin->setText("Min Level");

    protectionSelectionMaxPressed_Btn->setVisible(false);
    protectionSelectionMaxPressed_Btn->setText("");
    protectionButtonMax->setText("Max Level");

    // --- Telemetry Ranges ---
    fuelInputMin->setText("0");
    fuelInputMax->setText("100");
    fuelSlider->setValues(0, 100);
    distanceInputMin->setText("0");
    distanceInputMax->setText("MAX (No Limit)");
    distanceSlider->setValues(0, 10000);

    // --- Affiliation ---
    affiliationButton->setText("All Types");

    filterFunction();
}

// --- UI Input Logic ---
// Slots responsible for translating direct user interaction into UI state changes.
void MainWindow::callsignChanged(const QString &callsignText) {
    QString callsignFormatted = callsignText;
    if (!callsignText.isEmpty()) {
        callsignFormatted[0] = callsignFormatted[0].toUpper();
        if (callsignList.contains(callsignFormatted, Qt::CaseInsensitive)) {
            callsignSelectionPressed_Btn->setVisible(true);
            callsignSelectionPressed_Btn->setText(callsignFormatted);
            callsignSelectionPressed_Btn->adjustSize();
            filterFunction();
        } else {
            callsignSelectionPressed_Btn->setVisible(false);
            filterFunction();
        }
    } else {
        callsignSelectionPressed_Btn->setVisible(false);
        filterFunction();
    }
}

void MainWindow::callsignReturnPressed() {
    if (!callsignLine->text().isEmpty()) {
        QString completedCallsign = callsignCompleter->currentCompletion();
        callsignLine->setText(completedCallsign);
    }
}

void MainWindow::callsignSelectionPressed() {
    callsignLine->setText("");
}

void MainWindow::trackIdChanged(const QString &trackIdText) {
    QString trackIdFormatted = trackIdText;
    if (!trackIdText.isEmpty()) {
        trackIdFormatted = trackIdFormatted.toUpper();
        if (trackIdList.contains(trackIdFormatted, Qt::CaseInsensitive)) {
            trackIdSelectionPressed_Btn->setVisible(true);
            trackIdSelectionPressed_Btn->setText(trackIdFormatted);
            trackIdSelectionPressed_Btn->adjustSize();
            filterFunction();
        } else {
            trackIdSelectionPressed_Btn->setVisible(false);
            filterFunction();
        }
    } else {
        trackIdSelectionPressed_Btn->setVisible(false);
        filterFunction();
    }
}

void MainWindow::trackIdReturnPressed() {
    if (!trackIdLine->text().isEmpty()) {
        QString completedTrackId = trackIdCompleter->currentCompletion();
        trackIdLine->setText(completedTrackId);
    }
}

void MainWindow::trackIdSelectionPressed() {
    trackIdLine->setText("");
}

void MainWindow::affiliationActionClicked(QAction* action) {
    affiliationButton->setText(action->text());
    filterFunction();
}

void MainWindow::domainActionClicked(QAction* action) {
    domainButton->setText(action->text());
    domainButtonSelectionPressed_Btn->setVisible(true);
    domainButtonSelectionPressed_Btn->setText(action->text());
    domainButtonSelectionPressed_Btn->adjustSize();

    QString buttonText = domainButton->text();
    QFontMetrics metrics(domainButton->font());
    QString shortenedText = metrics.elidedText(buttonText, Qt::ElideRight, domainButton->width() - 10);
    domainButton->setText(shortenedText);
    domainButton->setToolTip(buttonText);
    filterFunction();
}

void MainWindow::domainSelectionPressed() {
    domainButtonSelectionPressed_Btn->setVisible(false);
    domainButton->setText("Select Domain");
    filterFunction();
}

void MainWindow::propulsionActionClicked(QAction* action) {
    propulsionButton->setText(action->text());
    propulsionSelectionPressed_Btn->setVisible(true);
    propulsionSelectionPressed_Btn->setText(action->text());
    propulsionSelectionPressed_Btn->adjustSize();

    QString buttonText = propulsionButton->text();
    QFontMetrics metrics(propulsionButton->font());
    QString shortenedText = metrics.elidedText(buttonText, Qt::ElideRight, propulsionButton->width() - 10);
    propulsionButton->setText(shortenedText);
    propulsionButton->setToolTip(buttonText);
    filterFunction();
}

void MainWindow::propulsionSelectionPressed() {
    propulsionSelectionPressed_Btn->setVisible(false);
    propulsionButton->setText("Select Type");
    filterFunction();
}

void MainWindow::priorityActionClicked(QAction* action){
    priorityButton->setText(action->text());
    prioritySelectionPressed_Btn->setVisible(true);
    prioritySelectionPressed_Btn->setText(action->text());
    prioritySelectionPressed_Btn->adjustSize();

    QString buttonText = priorityButton->text();
    QFontMetrics metrics(priorityButton->font());
    QString shortenedText = metrics.elidedText(buttonText, Qt::ElideRight, priorityButton->width() - 10);
    priorityButton->setText(shortenedText);
    priorityButton->setToolTip(buttonText);
    filterFunction();
}

void MainWindow::prioritySelectionPressed() {
    prioritySelectionPressed_Btn->setVisible(false);
    priorityButton->setText("Set Priority");
    filterFunction();
}

void MainWindow::protectionMenuMinClicked(QAction* action) {
    protectionButtonMin->setText(action->text());
    protectionSelectionMinPressed_Btn->setVisible(true);
    protectionSelectionMinPressed_Btn->setText(action->text());
    protectionSelectionMinPressed_Btn->adjustSize();

    QString buttonText = protectionButtonMin->text();
    QFontMetrics metrics(protectionButtonMin->font());
    QString shortenedText = metrics.elidedText(buttonText, Qt::ElideRight, protectionButtonMin->width() - 10);
    protectionButtonMin->setText(shortenedText);
    protectionButtonMin->setToolTip(buttonText);

    if (protectionSelectionMaxPressed_Btn->isVisible() && protectionButtonMin->text().toInt() > protectionButtonMax->text().toInt()) {
        protectionSelectionMaxPressed();
    }

    for (QAction *act : protectionMenuMax->actions()) {
        act->setVisible(true);
    }
    int minThreshold = protectionButtonMin->text().toInt();
    for (QAction* act : protectionMenuMax->actions()) {
        if (act->text().toInt() < minThreshold) {
            act->setVisible(false);
        }
    }
    filterFunction();
}

void MainWindow::protectionMenuMaxClicked(QAction* action) {
    protectionButtonMax->setText(action->text());
    protectionSelectionMaxPressed_Btn->setVisible(true);
    protectionSelectionMaxPressed_Btn->setText(action->text());
    protectionSelectionMaxPressed_Btn->adjustSize();

    QString buttonText = protectionButtonMax->text();
    QFontMetrics metrics(protectionButtonMax->font());
    QString shortenedText = metrics.elidedText(buttonText, Qt::ElideRight, protectionButtonMax->width() - 10);
    protectionButtonMax->setText(shortenedText);
    protectionButtonMax->setToolTip(buttonText);

    filterFunction();
}

void MainWindow::protectionSelectionMinPressed() {
    protectionSelectionMinPressed_Btn->setVisible(false);
    protectionButtonMin->setText("Min Level");
    for (QAction *action : protectionMenuMax->actions()) {
        action->setVisible(true);
    }
    filterFunction();
}

void MainWindow::protectionSelectionMaxPressed() {
    protectionSelectionMaxPressed_Btn->setVisible(false);
    protectionButtonMax->setText("Max Level");
    filterFunction();
}
void MainWindow::distanceSliderChanged(int x, int y) {
    distanceInputMin->blockSignals(true);
    distanceInputMax->blockSignals(true);
    if (x == 0) {
        distanceInputMin->setText("0");
    } else {
        distanceInputMin->setText(QString::number(x));
    }
    if (y >= 10000) {
        distanceInputMax->setText("MAX (No Limit)");
    } else {
        distanceInputMax->setText(QString::number(y));
    }
    distanceInputMin->blockSignals(false);
    distanceInputMax->blockSignals(false);
    filterFunction();
}

void MainWindow::distanceInputMinChanged(const QString &distanceString) {
    if (distanceInputMin->signalsBlocked()) return;

    distanceSlider->blockSignals(true);
    if (distanceString.isEmpty()) {
        distanceSlider->setValues(0, distanceSlider->upperValue());
        distanceInputMin->setPlaceholderText("0");
    } else {
        bool ok;
        int val = distanceString.toInt(&ok);
        if (ok) distanceSlider->setValues(val, distanceSlider->upperValue());
    }
    distanceSlider->blockSignals(false);
    filterFunction();
}

void MainWindow::distanceInputMaxChanged(const QString &distanceString) {
    if (distanceInputMax->signalsBlocked()) return;

    distanceSlider->blockSignals(true);
    if (distanceString.isEmpty()) {
        distanceSlider->setValues(distanceSlider->lowerValue(), 10000);
        distanceInputMax->setPlaceholderText("MAX (No Limit)");
    }
    else if (distanceString == "MAX (No Limit)" || distanceString.toInt() >= 10000) {
        distanceSlider->setValues(distanceSlider->lowerValue(), 10000);

        distanceInputMax->blockSignals(true);
        distanceInputMax->setText("MAX (No Limit)");
        distanceInputMax->blockSignals(false);
    }
    else {
        bool ok;
        int val = distanceString.toInt(&ok);
        if (ok) {
            distanceSlider->setValues(distanceSlider->lowerValue(), val);
        }
    }
    distanceSlider->blockSignals(false);
    filterFunction();
}

void MainWindow::fuelSliderChanged(int x, int y) {
    fuelInputMin->blockSignals(true);
    fuelInputMax->blockSignals(true);
    if (x == 0) {
        fuelInputMin->setText("0");
    } else {
        fuelInputMin->setText(QString::number(x));
    }
    if (y == 100) {
        fuelInputMax->setText("100");
    } else {
        fuelInputMax->setText(QString::number(y));
    }
    fuelInputMin->blockSignals(false);
    fuelInputMax->blockSignals(false);
    filterFunction();
}

void MainWindow::fuelInputMinChanged(const QString &fuelString) {
    if (fuelInputMin->signalsBlocked()) return;

    fuelSlider->blockSignals(true);
    if (fuelString.isEmpty()) {
        fuelSlider->setValues(0, fuelSlider->upperValue());
        fuelInputMin->setPlaceholderText("0");
    } else {
        bool ok;
        int val = fuelString.toInt(&ok);
        if (ok) fuelSlider->setValues(val, fuelSlider->upperValue());
    }
    fuelSlider->blockSignals(false);
    filterFunction();
}

void MainWindow::fuelInputMaxChanged(const QString &fuelString) {
    if (fuelInputMax->signalsBlocked()) return;

    fuelSlider->blockSignals(true);
    if (fuelString.isEmpty()) {
        fuelSlider->setValues(fuelSlider->lowerValue(), 100);
        fuelInputMax->setPlaceholderText("100");
    } else {
        bool ok;
        int val = fuelString.toInt(&ok);
        if (ok) fuelSlider->setValues(fuelSlider->lowerValue(), val);
    }
    fuelSlider->blockSignals(false);
    filterFunction();
}

// --- Simulation Logic ---
// Triggers controller-side simulation updates based on current UI target state.
void MainWindow::onSimulationTick() {
    const double targetX = targetXLine->text().toDouble();
    const double targetY = targetYLine->text().toDouble();
    controller->updateSimulation(targetX, targetY);
    if (resultsList->count() > 0 && liveUpdatesBox->isChecked()) {
        manualUpdateRequested = true;
        if (sortButton->text() == "Distance: Closest First") {
            sortByDistanceAsc();
        }
        else if (sortButton->text() == "Distance: Farthest First") {
            sortByDistanceDesc();
        } else {
            printList();
        }
        manualUpdateRequested = false;
    }
}

// --- Sorting Logic ---
// UI-driven handlers for ordering asset views by operational metrics.
void MainWindow::sortByFuelAsc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByFuelAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByFuelAsc(&a, &b); });
    }
    sortButton->setText("Fuel: Critical First");
    manualUpdateRequested = true;
    printList();
}

void MainWindow::sortByFuelDesc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByFuelDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByFuelDesc(&a, &b); });
    }
    sortButton->setText("Fuel: Full First");
    manualUpdateRequested = true;
    printList();
}

void MainWindow::sortByPriorityAsc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByPriorityAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByPriorityAsc(&a, &b); });
    }
    sortButton->setText("Priority (A-Z)");
    manualUpdateRequested = true;
    printList();
}

void MainWindow::sortByPriorityDesc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByPriorityDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByPriorityDesc(&a, &b); });
    }
    sortButton->setText("Priority (Z-A)");
    manualUpdateRequested = true;
    printList();
}

void MainWindow::sortByClassificationAsc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByClassificationAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByClassificationAsc(&a, &b); });
    }
    sortButton->setText("Classification (A-Z)");
    manualUpdateRequested = true;
    printList();
}

void MainWindow::sortByClassificationDesc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByClassificationDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByClassificationDesc(&a, &b); });
    }
    sortButton->setText("Classification (Z-A)");
    manualUpdateRequested = true;
    printList();
}

void MainWindow::sortByDistanceAsc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByDistanceAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByDistanceAsc(&a, &b); });
    }
    sortButton->setText("Distance: Closest First");
    manualUpdateRequested = true;
    printList();
}

void MainWindow::sortByDistanceDesc() {
    if (resultsList->count() == 0) return;

    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (controller->isFilterActive()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByDistanceDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByDistanceDesc(&a, &b); });
    }
    sortButton->setText("Distance: Farthest First");
    manualUpdateRequested = true;
    printList();
}

// --- Display Logic  ---
// Functions responsible for rendering data to the user interface.

// Displays results and applies default distance-based ordering.
void MainWindow::displayButtonClicked() {
    manualUpdateRequested = true;
    filterFunction();
    printList();
    sortByDistanceAsc();
    manualUpdateRequested = false;
}

void MainWindow::printList() {
    if (!manualUpdateRequested) return;

    resultsList->clear();

    QFont monoFont("Lucida Console", 12);
    monoFont.setStyleHint(QFont::Monospace);
    resultsList->setFont(monoFont);

    auto populateRow = [&](const QString& callsign, const QString& type, const QString& trackId,
                           double fuel, double dist, int prot, const QString& affiliation) {

        QString cSign = callsign.leftJustified(18, ' ');
        QString vType = type.leftJustified(20, ' ');
        QString tId   = ("ID: " + trackId).leftJustified(18, ' ');
        QString dDist = ("Distance to target: " + QString::number(dist, 'f', 0) + " m").leftJustified(32, ' ');
        QString fFuel = ("Est. fuel level: " + QString::number(fuel, 'f', 1) + " %").leftJustified(24, ' ');
        QString pProt = ("Protection level: " + QString::number(prot)).leftJustified(22, ' ');

        QString row = QString("%1 | %2 | %3 | %4 | %5 | %6")
                          .arg(cSign).arg(vType).arg(tId).arg(dDist).arg(fFuel).arg(pProt);

        QListWidgetItem* item = new QListWidgetItem(row, resultsList);

        if (affiliation.contains("Friendly", Qt::CaseInsensitive)) {
            item->setForeground(QColor(0, 162, 232));
        } else if (affiliation.contains("Hostile", Qt::CaseInsensitive)) {
            item->setForeground(Qt::red);
        } else {
            item->setForeground(Qt::white);
        }
    };

    if (controller->isFilterActive()) {
        for (const auto* vehicle : controller->filteredVehicles) {
            populateRow(vehicle->callsign,
                        vehicle->type,
                        vehicle->trackId,
                        vehicle->fuelLevel,
                        vehicle->distanceToTarget,
                        vehicle->protectionLevel,
                        vehicle->affiliation);
        }
    } else {
        for (const auto& vehicle : tacticalVehicleDb->vehicles()) {
            populateRow(vehicle.callsign,
                        vehicle.type,
                        vehicle.trackId,
                        vehicle.fuelLevel,
                        vehicle.distanceToTarget,
                        vehicle.protectionLevel,
                        vehicle.affiliation);
        }
    }
}

// --- Dialog Logic  ---
// Slot responsible for entity dialog.
void MainWindow::listItemDoubleclicked(QListWidgetItem *item) {
    entityDialog = new QDialog(this);
    entityDialog->setAttribute(Qt::WA_DeleteOnClose);
    entityDialog->show();
    entityDialog->raise();
    entityDialog->activateWindow();
    entityDialog->setSizeGripEnabled(true);
    entityDialog->setBaseSize(375, 375);

    QString extractedCallsign = item->text().split('|').first().trimmed();

    QHBoxLayout *entityTopPanel = new QHBoxLayout();
    QLabel *entityLiveUpdatesLabel = new QLabel("Live Updates");
    QCheckBox *entityLiveUpdatesBox = new QCheckBox();
    entityTopPanel->addWidget(entityLiveUpdatesLabel);
    entityTopPanel->addWidget(entityLiveUpdatesBox);
    entityTopPanel->addStretch();

    QListWidget *entityList = new QListWidget();
    entityList->setSelectionMode(QAbstractItemView::NoSelection);

    QVBoxLayout *entityLayout = new QVBoxLayout();
    entityLayout->addLayout(entityTopPanel);
    entityLayout->addWidget(entityList);
    entityDialog->setLayout(entityLayout);

    QFont entityFont("Lucida Console", 12);
    entityFont.setStyleHint(QFont::Monospace);
    entityList->setFont(entityFont);

    QListWidgetItem *distanceItem = new QListWidgetItem;
    QListWidgetItem *speedItem = new QListWidgetItem;
    QListWidgetItem *headingItem = new QListWidgetItem;
    for (const auto& vehicle : tacticalVehicleDb->vehicles()) {
        if (vehicle.callsign == extractedCallsign) {
            QString dCall =  ("Callsign:           " + extractedCallsign);
            QString dTrack = ("Track ID:           " + vehicle.trackId);
            QString dPrio =  ("Strategic Priority: " + vehicle.priority);
            QString dDom =   ("Domain:             " + vehicle.domain);
            QString dClas =  ("Classification:     " + vehicle.classification);
            QString dTyp =   ("Type:               " + vehicle.type);
            QString dDist =  ("Distance to target: " + QString::number(vehicle.distanceToTarget, 'f', 0) + " m");
            QString dSpe =   ("Speed:              " + QString::number(vehicle.speed, 'f', 0) + " km/h");
            QString dHea =   ("Heading:            " + QString::number(vehicle.heading, 'f', 0) + "°");
            QString dFue =   ("Est. fuel level:    " + QString::number(vehicle.fuelLevel, 'f', 1) + " %");
            QString dAmm =   ("Est. amm. level:    " + QString::number(vehicle.ammunitionLevel, 'f', 1) + " %");
            QString dUnm;
            if (vehicle.isUnmanned) {
                dUnm = "Yes";
            }
            else {
                dUnm = "No";
            }
            dUnm =           ("Unmanned:           " + dUnm);
            QString dSat;
            if (vehicle.hasSatCom) {
                dSat = "Yes";
            }
            else {
                dSat = "No";
            }
            dSat =           ("Has SatCom:         " + dSat);
            QString dAct;
            if (vehicle.hasActiveDefense) {
                dAct = "Yes";
            }
            else {
                dAct = "No";
            }
            dAct =           ("Has Active Defence: " + dAct);
            QString dAmp;
            if (vehicle.isAmphibious) {
                dAmp = "Yes";
            }
            else {
                dAmp = "No";
            }

            dAmp =           ("Is Amphibious:      " + dAmp);
            QString dProt =  ("Protection Level:   " + QString::number(vehicle.protectionLevel, 'f', 0));
            QString dMSpe =  ("Maximum Speed:      " + QString::number(vehicle.maxSpeed, 'f', 0) + " km/h");
            QString dProp =  ("Propulsion:         " + vehicle.propulsion);
            new QListWidgetItem(dCall, entityList);
            new QListWidgetItem(dTrack, entityList);
            new QListWidgetItem(dPrio, entityList);
            new QListWidgetItem(dClas, entityList);
            new QListWidgetItem(dDom, entityList);
            new QListWidgetItem(dTyp, entityList);
            distanceItem->setText(dDist);
            entityList->insertItem(7, distanceItem);
            speedItem->setText(dSpe);
            entityList->insertItem(8, speedItem);
            headingItem->setText(dHea);
            entityList->insertItem(9, headingItem);
            new QListWidgetItem(dFue, entityList);
            new QListWidgetItem(dAmm, entityList);
            new QListWidgetItem(dUnm, entityList);
            new QListWidgetItem(dSat, entityList);
            new QListWidgetItem(dAct, entityList);
            new QListWidgetItem(dAmp, entityList);
            new QListWidgetItem(dProt, entityList);
            new QListWidgetItem(dMSpe, entityList);
            new QListWidgetItem(dProp, entityList);

            if (vehicle.affiliation.contains("Friendly", Qt::CaseInsensitive)) {
                entityList->setStyleSheet("QListWidget { color: rgb(0, 162, 232); }");
            } else if (vehicle.affiliation.contains("Hostile", Qt::CaseInsensitive)) {
                entityList->setStyleSheet("QListWidget { color: red; }");
            } else {
                entityList->setStyleSheet("QListWidget { color: white; }");
            }

        }
    }
    connect(simTimer, &QTimer::timeout, entityDialog, [=]() {
        if (!entityDialog || !entityDialog->isVisible()) return;

        for (const auto &vehicleUpdate : tacticalVehicleDb->vehicles()) {
            if (vehicleUpdate.callsign == extractedCallsign && entityLiveUpdatesBox->isChecked()) {
                distanceItem->setText("Distance to target: " +QString::number(vehicleUpdate.distanceToTarget, 'f', 0) + " m");
                speedItem->setText   ("Speed:              " + QString::number(vehicleUpdate.speed, 'f', 0) + " km/h");
                headingItem->setText ("Heading:            " + QString::number(vehicleUpdate.heading, 'f', 0) + "°");
            }
        }
    });
}

MainWindow::~MainWindow() {
    // Default cleanup handled by Qt parent–child ownership.
}
