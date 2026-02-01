# Tactical Vehicle Gateway (TVG)

**Tactical Vehicle Gateway** is an advanced C++/Qt situational awareness system for monitoring, filtering, sorting, and simulating tactical assets in real time. The application combines robust data persistence, deterministic kinematic simulation, and a modular, architecture-driven UI suitable for operational and analytical contexts.

---

## 🛠 Technical Highlights

* **Custom UI Components**  
  Includes a bespoke `RangeSlider` widget built from first principles by inheriting from `QWidget` and overriding low-level event handlers (`paintEvent`, `mousePressEvent`, `mouseMoveEvent`). This demonstrates control over rendering, interaction, and value–pixel mapping beyond standard Qt widgets.

* **Data Management & Persistence**  
  Uses `QJsonDocument` for deterministic JSON ingestion. The architecture follows a **master–derived view pattern**:
  * Authoritative ownership in `std::deque<TacticalVehicle>`
  * Filtered views represented as `std::vector<const TacticalVehicle*>`  
  This ensures memory safety, pointer stability, cache-friendly iteration, and no duplication of vehicle data.

* **Time-Stepped Simulation Engine**  
  A timed simulation heartbeat (`QTimer`) updates vehicle kinematics and recalculates distances relative to a user-defined mission target. Simulation logic is isolated in the controller layer and uses vector mathematics, trigonometry (`std::cos`, `std::sin`), and Euclidean distance calculations.

* **Algorithmic Efficiency & Sorting**  
  Sorting is implemented using static predicate functions and `std::sort`, supporting both pointer-based filtered views and in-place sorting of the master dataset. Assets can be ordered by:
  * Distance to target
  * Fuel criticality
  * Strategic priority
  * Classification

---

## 📡 Operational Features

### 1. Situational Awareness (SA)

* **Multi-Dimensional Filtering**  
  A single-pass deterministic filter evaluates:
  * Capability flags (SATCOM, Amphibious, Unmanned, Active Defense)
  * Identity constraints (Callsign, Track ID)
  * Strategic classification (Domain, Propulsion, Priority)
  * Protection bounds (STANAG 4569 min/max)
  * Telemetry ranges (Fuel %, Distance to target)
  * Affiliation (Friendly, Hostile, Neutral, Unknown)

* **Outcome-Based Filter Activation**  
  The system defines “filter active” by result-set divergence rather than UI intent. If all vehicles still match the criteria, the system correctly treats filtering as inactive—avoiding misleading UI states.

* **Tactical Visualization**  
  Results are rendered in a monospaced layout and color-coded by affiliation:
  * Friendly → Blue
  * Hostile → Red
  * Neutral / Unknown → White 

* **Standards Awareness**  
  Native support for STANAG 4569 protection levels (1–6) and structural readiness for APP-6 / MIL-STD-2525 symbology integration via NATO icon identifiers.

---

### 2. Interaction, Control & Reliability

* **Explicit Operator Control**  
  Filtering, sorting, and rendering are intentionally decoupled. Results are only rendered when explicitly requested or when live updates are enabled—preventing unintended UI churn and preserving operator intent.

* **Live Simulation Updates**  
  When enabled, both the main list and per-entity dialog views update dynamically as the simulation advances, without duplicating simulation logic or violating data ownership rules.

* **Robust Input Handling**  
  * `QCompleter` enables rapid and error-resistant callsign and track ID selection.
  * `QDoubleValidator` enforces numeric correctness for mission target coordinates.
  * Range sliders and text inputs remain synchronized via signal blocking to prevent feedback loops.

---

## 🧭 Entity Inspection & Detail View

* **Per-Asset Dialogs**  
  Double-clicking an asset opens a dedicated entity dialog displaying:
  * Identity and classification
  * Dynamic telemetry (distance, speed, heading)
  * Capabilities and protection level
  * Fuel and ammunition estimates

* **Live Entity Telemetry**  
  Entity dialogs can subscribe to live simulation updates independently of the main list, reflecting real-time kinematic changes without redundant computation.

---

## 🏗 System Architecture

The project is organized into clearly separated layers of responsibility:

* **`TacticalVehicle`**  
  A lightweight data model encapsulating identity, classification, capabilities, and dynamic telemetry.

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

* **`MainWindow`**  
  The UI orchestration layer handling:
  * Signal/slot wiring
  * UI state resolution
  * Delegation of domain logic to controllers

* **`RangeSlider`**  
  A reusable, standalone dual-handle slider widget for intuitive range-based input.

This structure ensures strong separation of concerns, testability, and safe long-term evolution of both UI and domain logic.

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
