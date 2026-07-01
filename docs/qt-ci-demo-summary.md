# qt-ci-demo — hexagonal architecture & test pipeline (summary)

A compact reference for the `qt-ci-demo` skeleton: what the architecture is, how
the tests are structured, and how the Docker + Jenkins pipeline is wired (with
Xray considered only via the chosen output format, not built).
The arithmetic `Calculator` is a **placeholder** for a real domain core (e.g. a
sensor/detection pipeline) — the value is the structure and test strategy, not the maths.

---

## 1. The hexagon (ports & adapters)

A **Qt-free domain core** in the middle; everything else plugs in through
**ports** (interfaces the core owns). Dependencies point **inward** — the core
depends on nothing outside itself, so it can run with no GUI and no hardware.

- **Core** (`src/core/`) — `Calculator` (pure primitive) + `CalculatorService`
  (the use case). Built as a library that **links no Qt**; independence is
  enforced by the compiler, not by convention.
- **Ports** (`src/ports/`)
  - `ICalculatorService` — *inbound* (the use case driven from outside).
  - `ICalculationLog` — *outbound* (what the core needs from outside).
- **Adapters** (`src/adapters/`)
  - *Driving:* `MainWindow` (Qt GUI); `StubService` in tests.
  - *Driven:* `StdoutCalculationLog` (app), `InMemoryCalculationLog` (tests),
    `MockCalculationLog` (GoogleMock, tests).
- **Composition root** (`src/main.cpp`) — the only place concrete adapters are
  chosen and wired into the core.

Diagram: `docs/architecture.svg`.

### Why this matters
The core being isolated is what makes fast, reliable tests possible: it can be
exercised through its ports with fakes/mocks instead of real GUI or hardware.

---

## 2. Test strategy (unit + integration)

Tests are split by level so the fast ones run first. Labels drive the split:
`ctest -L unit` then `ctest -L integration`.

| Test | Level | Framework | Wires together |
|------|-------|-----------|----------------|
| `test_calculator`          | unit        | GoogleTest              | `Calculator` (pure) |
| `test_calculator_service`  | unit        | GoogleTest + GoogleMock | `CalculatorService` + `MockCalculationLog` |
| `test_service_log`         | integration | GoogleTest              | `CalculatorService` + `InMemoryCalculationLog` |
| `test_gui_service`         | integration | Qt Test                 | `MainWindow` + `StubService` (headless / offscreen) |

**Two frameworks, on purpose.** The core and its tests are kept Qt-free by using
**GoogleTest/GoogleMock**, so they build and run on a bare (or cross-) toolchain.
**Qt Test** is used **only** for `test_gui_service`, which genuinely needs the Qt
event loop, `QTest` input simulation, and the `offscreen` platform.

- *Unit* = a component in isolation (the core, with a mock behind the outbound port).
- *Integration* = real components wired together across a port (service + real
  log adapter; or GUI + inbound port, headless).

**Next tiers (not in the skeleton yet):** port *contract tests* (one suite both a
mock and the real adapter must pass, so fakes can't drift), accuracy-regression on
versioned datasets, and hardware-in-the-loop — run nightly / pre-release.

---

## 3. Build & run

    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ctest --test-dir build                 # all
    ctest --test-dir build -L unit         # fast gate
    ctest --test-dir build -L integration  # wired across ports

Requires CMake ≥ 3.20, a C++17 compiler, Qt 6 (Widgets + Test), and network at
configure time (GoogleTest is fetched via `FetchContent`, pinned to `v1.15.2`
with a SHA256 `URL_HASH`).

---

## 4. The test pipeline (Docker · Jenkins) — with Xray as a considered output target

**What the POC actually builds** is the Docker + Jenkins pipeline. **Xray is not
wired up** in the POC — it is only *considered in the output formats*: the pipeline
emits **JUnit XML**, which is the format Xray can later import. The key idea is that
this one artifact is produced once and can be consumed by more than one system —
Jenkins today (trend/gating), and Xray later (requirement traceability) without
changing how tests run.

Flow (diagram: `cicd-docker-jenkins-xray.svg`):

1. **Developer → Git** — commit & push.
2. **Git → Jenkins** — webhook (or `pollSCM` locally) triggers the pipeline.
3. **Jenkins → Docker** — over the Docker socket: `docker build` produces the
   image (pulling the **pinned base image** from a registry and fetching
   GoogleTest), then `docker run` executes `ctest`.
4. **Docker → Jenkins** — `ctest` emits **JUnit XML** (`unit.xml`,
   `integration.xml`) back to the workspace.
5. **Jenkins publishes** — the `junit` step publishes the XML for build trend &
   gating. *(This is the end of what the POC implements.)*

### Xray — considered, not built
Xray is left as a **downstream option enabled by the output format**, not a
connection in the POC:

- The POC deliberately produces **JUnit XML** because it is an Xray-importable
  format — so adding Xray later needs **no change to how tests run**.
- The actual import (a future step) would be an extra Jenkins stage that pushes
  the same XML to Xray via its REST API (token auth), where it becomes a **Test
  Execution** linked to **Test issues** and **Requirements** for traceability.
- The diagram shows this import as the dashed/optional hand-off, distinct from the
  Docker/Jenkins steps that the POC implements.

### Staging & gating
The Jenkins pipeline runs **fail-fast, fast → slow**: build image → unit tests →
integration tests, publishing JUnit after each stage. Unit failures stop the run
before the slower integration/GUI stage.

### Reproducibility
- GoogleTest pinned (`v1.15.2`, SHA256-checked).
- **Qt version caveat:** Debian Bookworm ships Qt **6.4.2**, not the shipping
  **6.11.x** — pin package versions or use `aqtinstall`, and pin the Docker base
  image by digest.
- For a hermetic/offline build, vendor GoogleTest and point `FetchContent` at a
  local source instead of the URL.

---

## 5. Mapping to the real product

- `Calculator` / `CalculatorService` → the real **detection core**.
- `ICalculatorService` → `IDetectionService` (inbound).
- `ICalculationLog` → split into `ISensorSource` (inbound data) + `IResultSink`
  (outbound results).
- Same two-tier test setup and same pipeline stages carry over unchanged.

---

## Status & caveats

- The **Qt-free core, ports, adapters, and the three GoogleTest/GoogleMock tests**
  were compiled and passed with g++ during development — high confidence.
- The **CMake+Qt build, the Qt Test GUI target, and the Dockerfile/Jenkinsfile**
  were **not** executed end-to-end in that environment — verify them in your CI.
- **Xray is out of scope for the POC** — only the JUnit output format is chosen
  with a future Xray import in mind. When that import is built, note that **Xray
  Cloud vs Server/Data Center use different REST endpoints and auth**, and the
  Xray Jenkins plugin can replace a raw import call — confirm against your
  Jira/Xray edition at that point.
- Architecture/tooling concepts (ports & adapters, the unit/integration split,
  GoogleTest = BSD-3-Clause) are well-established; ownership, naming, timing, and
  the exact pipeline wiring are recommendations to calibrate to your setup.
