#include "MainWindow.h"
#include "TacticalVehicleData.h"
#include "RangeSlider.h"
#include "FilterCriteria.h"
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
#include <vector>
#include <algorithm>

/**
 * @brief MainWindow Constructor
 * Organizes the tactical gateway UI into logical functional blocks.
 */
MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {

    // --- 1. DATA & CORE INITIALIZATION ---
    tacticalVehicleDb = std::make_unique<TacticalVehicleData>();
    controller = std::make_unique<TacticalVehicleController>(*tacticalVehicleDb);
    tacticalVehicleDb->loadVehiclesFromJson(":/data/vehicles.json");
    choiceDeletion = QIcon::fromTheme(QIcon::ThemeIcon::WindowClose);

    // Initial Window Constraints
    setMinimumSize(1000, 720);
    this->resize(1000, 750);

    // --- 2. LAYOUT ARCHITECTURE ---
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *leftPanel = new QVBoxLayout();
    QVBoxLayout *rightPanel = new QVBoxLayout();

    rightPanel->setContentsMargins(0, 15, 0, 0);
    rightPanel->setSpacing(0);

    // --- 3. LEFT PANEL: UI GROUPS ---

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

    idGrid->addWidget(labelTrackId = new QLabel("Track ID:"), 1, 0);
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
    domainMenu->addAction("Cyber/Electronic");
    domainButton->setMenu(domainMenu);
    stratGrid->addWidget(domainButton, 0, 1);
    stratGrid->addWidget(domainButtonSelectionPressed_Btn = new QPushButton(), 0, 2);
    domainButtonSelectionPressed_Btn->setIcon(choiceDeletion);
    domainButtonSelectionPressed_Btn->setVisible(false);

    stratGrid->addWidget(new QLabel("Locomotion Type:"), 1, 0);
    propulsionButton = new QPushButton("Select Type");
    propulsionMenu = new QMenu(this);
    propulsionMenu->addAction("Wheeled (High Mobility)");
    propulsionMenu->addAction("Tracked (Heavy Terrain)");
    propulsionMenu->addAction("Articulated (All-Terrain)");
    propulsionMenu->addAction("Amphibious");
    propulsionMenu->addAction("Rotary-Wing / VTOL");
    propulsionButton->setMenu(propulsionMenu);
    stratGrid->addWidget(propulsionButton, 1, 1);
    stratGrid->addWidget(propulsionSelectionPressed_Btn = new QPushButton(), 1, 2);
    propulsionSelectionPressed_Btn->setIcon(choiceDeletion);
    propulsionSelectionPressed_Btn->setVisible(false);

    stratGrid->addWidget(new QLabel("Strategic Priority:"), 2, 0);
    priorityButton = new QPushButton("Set Priority");
    priorityMenu = new QMenu(this);
    priorityMenu->addAction("Flash (Immediate)");
    priorityMenu->addAction("High (Operational)");
    priorityMenu->addAction("Routine");
    priorityMenu->addAction("Low (Deferred)");
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
    protectionButtonMin = new QPushButton("Min level");
    protectionButtonMax = new QPushButton("Max level");
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
    teleGrid->addWidget(new QLabel("Fuel Level (%):"), 0, 0);
    fuelSlider = new RangeSlider();
    fuelSlider->setRange(0, 100);
    fuelSlider->setValues(0, 100);
    teleGrid->addWidget(fuelSlider, 0, 1, 1, 2);
    teleGrid->addWidget(fuelInputMin = new QLineEdit("0%"), 1, 1);
    teleGrid->addWidget(fuelInputMax = new QLineEdit("100%"), 1, 2);

    teleGrid->addWidget(new QLabel("Distance to Target (m):"), 2, 0);
    distanceSlider = new RangeSlider();
    distanceSlider->setRange(0, 10000);
    distanceSlider->setValues(0, 10000);
    teleGrid->addWidget(distanceSlider, 2, 1, 1, 2);
    teleGrid->addWidget(distanceInputMin = new QLineEdit("0 m"), 3, 1);
    teleGrid->addWidget(distanceInputMax = new QLineEdit("MAX (No Limit)"), 3, 2);

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

    QPushButton* exitButton = new QPushButton("EXIT SYSTEM");
    exitButton->setMinimumHeight(50);
    exitButton->setStyleSheet(
        "QPushButton { background-color: #2a2a2a; color: #cc5555; border: 1px solid #444; } "
        "QPushButton:hover { background-color: #352b2b; color: #ff6666; border: 1px solid #663333; } "
        "QPushButton:pressed { background-color: #1a1a1a; }"
        );

    leftPanel->addWidget(displayButton);
    leftPanel->addSpacing(5);
    leftPanel->addWidget(exitButton);

    // --- 4. RIGHT PANEL: RESULTS & SORTING ---

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
    sortBarLayout->setContentsMargins(0, 10, 0, 0);
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

    resultsList = new QListWidget();
    rightPanel->addWidget(resultsList);

    // --- 5. FINAL LAYOUT ASSEMBLY ---
    mainLayout->addLayout(leftPanel, 1);
    mainLayout->addLayout(rightPanel, 2);
    setLayout(mainLayout);

    // --- 6. SIGNAL & SLOT CONNECTIONS ---

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

    // --- 7. AUTO-COMPLETE & DYNAMIC UPDATES ---

    // Populate Search Data
    for (const auto& v : tacticalVehicleDb->vehicles()) {
        if (!v.callsign.isEmpty() && !callsignList.contains(v.callsign)) callsignList << v.callsign;
        if (!v.trackId.isEmpty() && !trackIdList.contains(v.trackId)) trackIdList << v.trackId;
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

/**
 * @section FILTERING_LOGIC
 * Core engine for processing vehicle data based on UI criteria.
 */
void MainWindow::filterFunction() {
    FilterCriteria criteria;

    criteria.hasSatCom = cbHasSatCom->isChecked();
    criteria.isAmphibious = cbIsAmphibious->isChecked();
    criteria.isUnmanned = cbIsUnmanned->isChecked();
    criteria.hasActiveDefense = cbHasActiveDefense->isChecked();

    criteria.callsignActive = callsignSelectionPressed_Btn->isVisible();
    criteria.callsign = callsignSelectionPressed_Btn->text();

    criteria.trackIdActive = trackIdSelectionPressed_Btn->isVisible();
    criteria.trackId = trackIdSelectionPressed_Btn->text();

    criteria.domainActive = domainButtonSelectionPressed_Btn->isVisible();
    criteria.domain = domainButtonSelectionPressed_Btn->text();

    criteria.propulsionActive = propulsionSelectionPressed_Btn->isVisible();
    criteria.propulsion = propulsionSelectionPressed_Btn->text();

    criteria.priorityActive = prioritySelectionPressed_Btn->isVisible();
    criteria.priority = prioritySelectionPressed_Btn->text();

    criteria.protectionMinActive = protectionSelectionMinPressed_Btn->isVisible();
    criteria.protectionMin = protectionSelectionMinPressed_Btn->text().toInt();

    criteria.protectionMaxActive = protectionSelectionMaxPressed_Btn->isVisible();
    criteria.protectionMax = protectionSelectionMaxPressed_Btn->text().toInt();

    criteria.fuelMin = fuelSlider->lowerValue();
    criteria.fuelMax = fuelSlider->upperValue();

    criteria.distanceMin = distanceSlider->lowerValue();
    criteria.distanceMax = distanceSlider->upperValue();

    criteria.affiliation = affiliationButton->text();

    controller->applyFilter(criteria);

    if (controller->viewMode() == VehicleViewMode::AllVehicles) {
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

/**
 * @section UI_INPUT_HANDLERS
 * Slots for handling user interaction with UI elements.
 */
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
    domainButton->setText("Domain");
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
    propulsionButton->setText("Propulsion");
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
    priorityButton->setText("Priority");
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
        distanceInputMin->setText("0 m");
    } else {
        distanceInputMin->setText(QString::number(x) + " m");
    }
    if (y == 10000) {
        distanceInputMax->setText("10000 m");
    } else {
        distanceInputMax->setText(QString::number(y) + " m");
    }
    if (y >= 10000) {
        distanceInputMax->setText("MAX (No Limit)");
    }
    distanceInputMin->blockSignals(false);
    distanceInputMax->blockSignals(false);
    filterFunction();
}

void MainWindow::distanceInputMinChanged(const QString &distanceString) {
    distanceSlider->blockSignals(true);
    if (distanceString.isEmpty()) {
        distanceSlider->setValues(0, distanceSlider->upperValue());
        distanceInputMin->setPlaceholderText("0 m");
    } else {
        distanceSlider->setValues(distanceString.toInt(), distanceSlider->upperValue());
    }
    distanceSlider->blockSignals(false);
    filterFunction();
}

void MainWindow::distanceInputMaxChanged(const QString &distanceString) {
    distanceSlider->blockSignals(true);
    if (distanceString.isEmpty()) {
        distanceSlider->setValues(distanceSlider->lowerValue(), 10000);
        distanceInputMax->setPlaceholderText("10000 m");
    } else {
        distanceSlider->setValues(distanceSlider->lowerValue(), distanceString.toInt());
    }
    distanceSlider->blockSignals(false);
    filterFunction();
}

void MainWindow::fuelSliderChanged(int x, int y) {
    fuelInputMin->blockSignals(true);
    fuelInputMax->blockSignals(true);
    if (x == 0) {
        fuelInputMin->setText("0%");
    } else {
        fuelInputMin->setText(QString::number(x) + "%");
    }
    if (y == 100) {
        fuelInputMax->setText("100%");
    } else {
        fuelInputMax->setText(QString::number(y) + "%");
    }
    fuelInputMin->blockSignals(false);
    fuelInputMax->blockSignals(false);
    filterFunction();
}

void MainWindow::fuelInputMinChanged(const QString &fuelString) {
    fuelSlider->blockSignals(true);
    if (fuelString.isEmpty()) {
        fuelSlider->setValues(0, fuelSlider->upperValue());
        fuelInputMin->setPlaceholderText("0%");
    } else {
        fuelSlider->setValues(fuelString.toInt(), fuelSlider->upperValue());
    }
    fuelSlider->blockSignals(false);
    filterFunction();
}

void MainWindow::fuelInputMaxChanged(const QString &fuelString) {
    fuelSlider->blockSignals(true);
    if (fuelString.isEmpty()) {
        fuelSlider->setValues(fuelSlider->lowerValue(), 100);
        fuelInputMax->setPlaceholderText("100%");
    } else {
        fuelSlider->setValues(fuelSlider->lowerValue(), fuelString.toInt());
    }
    fuelSlider->blockSignals(false);
    filterFunction();
}

/**
 * @section OPERATIONAL_LOGIC
 * Math and simulation logic for real-time asset tracking.
 */
void MainWindow::onSimulationTick() {
    const double targetX = targetXLine->text().toDouble();
    const double targetY = targetYLine->text().toDouble();
    controller->updateSimulation(targetX, targetY);
}

/**
 * @section SORTING_METHODS
 * Handlers for organizing asset lists by various metrics.
 */
void MainWindow::updateSortStatus() {
    printList();
    QString buttonText = sortButton->text();
    QFontMetrics metrics(sortButton->font());
    QString elidedText = metrics.elidedText(buttonText, Qt::ElideRight, sortButton->width() - 10);
    sortButton->setText(elidedText);
    sortButton->setToolTip(buttonText);
}

void MainWindow::sortByFuelAsc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByFuelAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByFuelAsc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Fuel: Critical first");
    manualUpdateRequested = true;
    updateSortStatus();
}

void MainWindow::sortByFuelDesc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByFuelDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByFuelDesc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Fuel: Full first");
    manualUpdateRequested = true;
    updateSortStatus();
}

void MainWindow::sortByPriorityAsc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByPriorityAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByPriorityAsc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Priority (A-Z)");
    manualUpdateRequested = true;
    updateSortStatus();
}

void MainWindow::sortByPriorityDesc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByPriorityDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByPriorityDesc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Priority (Z-A)");
    manualUpdateRequested = true;
    updateSortStatus();
}

void MainWindow::sortByClassificationAsc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByClassificationAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByClassificationAsc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Classification (A-Z)");
    manualUpdateRequested = true;
    updateSortStatus();
}

void MainWindow::sortByClassificationDesc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByClassificationDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByClassificationDesc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Classification (Z-A)");
    manualUpdateRequested = true;
    updateSortStatus();
}

void MainWindow::sortByDistanceAsc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByDistanceAsc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByDistanceAsc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Distance: Closest first");
    manualUpdateRequested = true;
    updateSortStatus();
}

void MainWindow::sortByDistanceDesc() {
    if (resultsList->count() == 0) return;
    auto& fv = controller->filteredVehicles;
    auto& av = tacticalVehicleDb->vehiclesMutable();

    if (displayButton->text().contains("(" + QString::number(fv.size()) + ")") && fv.size() != av.size()) {
        std::sort(fv.begin(), fv.end(), TacticalVehicleData::sortByDistanceDesc);
    } else {
        std::sort(av.begin(), av.end(), [](const auto& a, const auto& b) { return TacticalVehicleData::sortByDistanceDesc(&a, &b); });
        filterFunction();
    }
    sortButton->setText("Distance: Farthest first");
    manualUpdateRequested = true;
    updateSortStatus();
}

/**
 * @section DISPLAY_AND_OUTPUT
 * Functions responsible for rendering data to the user interface.
 */
void MainWindow::displayButtonClicked() {
    manualUpdateRequested = true;
    filterFunction();
    printList();
    sortByDistanceAsc();
    manualUpdateRequested = false;
    sortButton->setText("Sort");
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
        QString dDist = ("Distance to target: " + QString::number(dist, 'f', 0) + "m").leftJustified(32, ' ');
        QString fFuel = ("Est. fuel level: " + QString::number(fuel, 'f', 1) + "%").leftJustified(24, ' ');
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

    if (controller->viewMode() == VehicleViewMode::FilteredVehicles) {
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

MainWindow::~MainWindow() {
}
