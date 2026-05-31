# Tactical Vehicle Gateway (TVG)

**Tactical Vehicle Gateway** is a C++/Qt application for monitoring, filtering, sorting, and simulating tactical vehicles in real time. The application combines data persistence, kinematic simulation, and a modular UI suitable for operational and analytical use.

---

## 🛠 Technical Implementation

* **Custom UI Components**  
  Includes a custom `RangeSlider` widget implemented by inheriting from `QWidget` and overriding low-level event handlers (`paintEvent`, `mousePressEvent`, `mouseMoveEvent`) providing range-based input functionality not available in the standard Qt widget library.

* **Data Management & Persistence**  
  Uses `QJsonDocument` for JSON ingestion. The architecture follows a **master–derived view pattern**:
  * Primary data ownership in `std::deque<TacticalVehicle>`
  * Filtered views represented as `std::vector<const TacticalVehicle*>`  
  This ensures pointer stability, cache-friendly iteration, and avoids duplication of vehicle data.

* **Time-Stepped Simulation Engine**  
  A timed simulation heartbeat (`QTimer`) updates vehicle kinematics and recalculates distances relative to a user-defined mission target. Simulation logic is isolated in the controller layer and uses vector mathematics, trigonometry (`std::cos`, `std::sin`), and Euclidean distance calculations. Vehicle telemetry is updated during the simulation cycle, after which a derived threat score is recalculated.

* **Algorithmic Efficiency & Sorting**  
  Sorting is implemented using static predicate functions and `std::sort`, supporting both pointer-based filtered views and in-place sorting of the master dataset. Assets can be ordered by:
  * Distance to target
  * Threat score
  * Strategic priority
  * Fuel criticality
  * Classification

---

## 📡 Functional Capabilities

### 1. Vehicle Monitoring & Filtering

* **Multi-Dimensional Filtering**  
  A single-pass filter evaluates:
  * Capabilities (SATCOM Link, Amphibious, Unmanned (UxV), Active Defense)
  * Identity (Callsign, Track ID)
  * Affiliation (Friendly, Hostile, Neutral, Unknown)
  * Strategic classification (Operational Domain, Propulsion Type, Strategic Priority)
  * Protection level (STANAG 4569 min/max)
  * Telemetry (Estimated Fuel Level, Distance To Target)
  
* **Divergence-Based Filter Logic**  
  The system evaluates filter necessity by comparing the current result-set against the total dataset. If all vehicles match the criteria, the filtering state is treated as inactive to maintain UI consistency.

* **Affiliation-Based Rendering**  
  Results are rendered in a monospaced layout and color-coded by affiliation:
  * Friendly → Blue
  * Hostile → Red
  * Neutral / Unknown → Default text color

* **Domain Standards Support**  
  Includes support for STANAG 4569 protection levels (1–6) and stores MIL-STD-2525 / APP-6 compatible symbol identifiers.

---

### 2. Control Logic & Input Validation

* **Decoupled Logic & Rendering**  
  Filtering, sorting, and UI rendering are decoupled to optimize performance. UI updates are triggered via explicit user requests or toggled live updates, preventing redundant re-renders.

* **Live Simulation Updates**  
  When enabled, both the main list and per-entity dialog views update dynamically as the simulation advances, without duplicating simulation logic or violating data ownership rules.

* **Input Validation and Constraints**  
  * `QCompleter` enables rapid and error-resistant callsign and track ID selection.
  * `QDoubleValidator` enforces numeric correctness for mission target coordinates.
  * Range sliders and text inputs remain synchronized via signal blocking to prevent feedback loops.

---

## 🧭 Entity Data Visualization

* **Per-Vehicle Dialogs**  
  Double-clicking an asset opens a dedicated entity dialog displaying:
  * Identity
  * Strategic classification
  * Capabilities
  * Protection level
  * Fuel and ammunition estimates
  * Dynamic telemetry
  * Threat score 

* **Live Entity Telemetry**  
  Vehicle dialogs can subscribe to live simulation updates independently of the main list, reflecting real-time kinematic changes and threat calculation without redundant computation.

---

## 🏗 System Architecture

The project is organized into decoupled layers of responsibility:

* **`TacticalVehicle`**  
  A lightweight data model encapsulating entity data.

* **`TacticalVehicleData`**  
  The authoritative data store responsible for:
  * JSON ingestion
  * Owning all vehicle instances
  * Providing stateless sorting predicates

* **`TacticalVehicleController`**  
  A UI-agnostic domain controller that:
  * Applies filter criteria
  * Maintains derived filtered views
  * Advances simulation state
  * Computes threat score

* **`MainWindow`**  
  The UI orchestration layer handling:
  * Signal/slot wiring
  * UI state resolution
  * Delegation of domain logic to controllers

* **`RangeSlider`**  
  A reusable, standalone dual-handle slider widget for intuitive range-based input.

This structure ensures separation of concerns, facilitates unit testing, and maintains the extensibility of both UI and domain logic.

---

## 🚀 Getting Started

### Prerequisites
* Qt 6.x (or higher)
* C++17 compatible compiler (MSVC, GCC, or Clang)

1. **Clone the repository**
   ```bash
   git clone https://github.com/sebastianvento/Tactical-Vehicle-Gateway.git
   ```
2. Open `TacticalVehicleGateway.pro` in **Qt Creator**.
3. The system automatically loads vehicle data from the internal Qt resource path (`:/data/vehicles.json`).
4. **Build and run**
   ```bash
   qmake && make
   ```

### Build Environment
* **Framework:** Qt 6.x (recommended)
* **OS:** macOS / Linux / Windows
* **Compiler:** Apple Clang / MSVC / GCC (C++17)
* **Build Tool:** qmake / make
