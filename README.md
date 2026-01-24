# Tactical Vehicle Gateway (TVG)

Tactical Vehicle Gateway is an advanced C++/Qt situational awareness system designed for monitoring, filtering, and telemetry analysis of tactical assets. The application integrates robust data persistence, deterministic kinematic simulation, and a modular, architecture-driven UI design.

## 🛠 Technical Highlights

* **Custom UI Components:** Features a bespoke `RangeSlider` widget developed from the ground up. By inheriting from `QWidget` and overriding low-level handlers (`paintEvent`, `mouseMoveEvent`), the project demonstrates a deep mastery of the Qt framework beyond standard library widgets.
* **Data Management & Persistence:** Implements `QJsonDocument` parsing for dynamic asset ingestion. The architecture utilizes a master–derived-view pattern—owning authoritative data in a `std::deque<TacticalVehicle>` while managing filtered views via `std::vector<const TacticalVehicle*>`, ensuring memory safety, cache stability, and high-performance iteration.
* **Kinematic Simulation Engine:** Includes a periodic simulation heartbeat that updates vehicle positions and distances to mission targets. Simulation logic is encapsulated within a dedicated controller layer and employs vector mathematics, trigonometry (`std::cos`, `std::sin`), and Euclidean distance calculations.
* **Algorithmic Efficiency:** Employs static predicate functions and STL `std::sort`, combined with lambda expressions, to provide instantaneous sorting of assets based on fuel criticality, proximity, classification, or strategic priority.

## 📡 Operational Features

### 1. Situational Awareness (SA)
* **Multi-Dimensional Filtering:** A unified filtering engine that evaluates boolean capabilities (SATCOM, Active Defense), string-based identity (Callsign / Track ID), and numerical telemetry ranges in a single deterministic pass.
* **Tactical Visualization:** Results are color-coded by affiliation (Friendly, Hostile, Neutral) and rendered in a monospaced format to ensure maximum readability in high-stress operational environments.
* **Standards Awareness:** Built-in support for STANAG 4569 protection levels (1–6) and structured to support future integration with APP-6 / MIL-STD-2525 symbology.

### 2. Usability & Reliability
* **Deterministic UI Updates:** User interactions immediately recompute tactical views, while result rendering and sorting are explicitly controlled to preserve operator intent and prevent unintended state churn.
* **Input Validation:** Robust error handling using `QCompleter` for rapid searching and `QValidator` for coordinate inputs ensures data integrity during mission planning and execution.

## 🏗 System Architecture

The project is modularized into clearly defined layers of responsibility:

* **TacticalVehicle:** A lightweight, POD-style data model representing individual assets, combining static identity, capabilities, and dynamic telemetry.
* **TacticalVehicleData:** The authoritative data store responsible for JSON ingestion and static sorting predicates. Owns all vehicle instances.
* **TacticalVehicleController:** A domain controller encapsulating tactical logic, including filtering, simulation updates, and construction of derived views without direct UI dependencies.
* **MainWindow:** The UI orchestration layer responsible for signal/slot wiring, user interaction handling, and delegating domain logic to backend controllers.
* **RangeSlider:** A reusable, standalone UI component for intuitive range-based data entry.

This separation ensures internal coherence, testability, and safe evolution of both UI and domain logic.

## 🚀 Getting Started

### Prerequisites
* Qt 6.x (or higher)
* C++17 compatible compiler (MSVC, GCC, or Clang)

### Build & Run
1. **Clone the repository:**  
   `git clone https://github.com/sebastianvento/Tactical-Vehicle-Gateway.git`
2. Open `TacticalVehicleGateway.pro` in Qt Creator.
3. Ensure `vehicles.json` is located within the Qt resource path (`:/data/`).
4. Build and run using:  
   `qmake && make`

### Build Environment
* **Framework:** Qt 6.10.1 (recommended)
* **OS:** macOS
* **Compiler:** Apple Clang (C++17)
* **Build Tool:** qmake / make
