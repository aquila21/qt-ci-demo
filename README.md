# Qt CI Demo

Proof of concept for an automated CI/CD toolchain for a Qt 6 application:
**code → build → unit test → machine-readable report → CI**, reproducible across
multiple environments. The POC deliberately validates only the **skeleton** of the
toolchain, not the actual product core.

---

## What the POC demonstrates

- Reproducible build of the same source on Windows/MSVC and Debian/GCC.
- Automated unit tests (Qt Test) with **JUnit XML** output for Jenkins.
- Pipeline-as-code (Jenkinsfile), end to end from checkout to report.
- Pinned, reproducible build environment (Docker) as the basis for later evidence.

Clean layering: the business logic (`Calculator`) is independent of the GUI
(`MainWindow`) and therefore testable in isolation.

---

## Project structure

```
qt-ci-demo/
├── CMakeLists.txt          # Build: targets QtCiDemo (app) and UnitTests (test runner)
├── Dockerfile              # Reproducible Debian build/test environment
├── Jenkinsfile             # CI pipeline (build & test in Docker, JUnit report)
├── .dockerignore           # Keeps .git/build/results out of the build context
├── src/
│   ├── main.cpp            # Entry point
│   ├── MainWindow.h/.cpp   # GUI layer (Qt Widgets)
│   └── Calculator.h/.cpp   # Business logic (UI-independent)
└── tests/
    └── test_calculator.cpp # Qt Test unit tests (incl. divide-by-zero)
```

---

## Prerequisites

- CMake ≥ 3.20, Ninja, a C++17 compiler
- Qt 6 (components `Widgets` and `Test`)
- For the container path: Docker

> **Version note:** Debian/Ubuntu `apt` installs Qt **6.4.2** — this is **not** the
> shipping version (6.11.x). For production-like builds, align the Qt version
> deliberately (see [Limitations & outlook](#limitations--outlook)).

---

## Build & test locally

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run tests, write JUnit XML, and also log to stdout.
# QT_QPA_PLATFORM=offscreen allows running without a display.
mkdir -p results
QT_QPA_PLATFORM=offscreen ./build/UnitTests -o results/unit.xml,junitxml -o -,txt
```

Alternatively via CTest:

```bash
ctest --test-dir build --output-on-failure
```

Expected result: `Totals: 7 passed, 0 failed`.

---

## Build & test with Docker

```bash
# Build the image
docker build -t qt-ci-demo .

# Run the tests and extract the results
docker run --name qt-ci qt-ci-demo || true
docker cp qt-ci:/app/results ./results
docker rm -f qt-ci
```

For a **reproducible** build, pin the base image by digest
(recommended for certification-grade evidence):

```bash
docker pull debian:bookworm-slim
docker inspect --format='{{index .RepoDigests 0}}' debian:bookworm-slim
docker build --build-arg BASE_IMAGE=debian:bookworm-slim@sha256:<DIGEST> -t qt-ci-demo .
```

---

## CI: Jenkins pipeline

The `Jenkinsfile` defines:

1. **Checkout** – source from SCM.
2. **Build & Test in Docker** – build the image, run the tests in the container,
   extract the JUnit XML via `docker cp`.
3. **post.always** – publish results with the `junit` step (trend/history) and
   clean up container/image.

Properties:

- Unique image tag per build (`qt-ci-demo:${BUILD_NUMBER}`) → no collisions on parallel runs.
- `timeout` and `buildDiscarder` guard against hanging builds and unbounded history.
- Optional local auto-trigger via `pollSCM` (commented out) — a GitHub webhook belongs
  to the centrally hosted production environment.

> **Build status:** if tests fail, the `junit` step marks the build **UNSTABLE (yellow)**
> by default, not FAILED (red). A compile/Docker error results in FAILED.

---

## Test-level coverage

| Test level | POC status |
|---|---|
| Unit (C++) | ✅ Qt Test, 7 tests incl. divide-by-zero |
| Component/integration | ⚠️ build integration only |
| GUI | ❌ GUI exists but is untested |
| Algorithm/AI detection | ❌ `Calculator` is a placeholder |
| System/E2E, hardware-in-the-loop | ❌ |
| Non-functional (performance/security) | ❌ |

---

## Limitations & outlook

The POC proves the automation **skeleton** holds. For a production-ready, certifiable
toolchain, the following must be added:

- **Align the build to the shipping Qt version** (e.g. via `aqtinstall`) and fully pin
  package versions / base image.
- **Requirements traceability** (e.g. Xray in Jira): requirement ↔ test ↔ execution ↔ defect.
- **Real GUI regression** (e.g. Squish) on an agent with a display/`xvfb`.
- **Code coverage** (gcov/lcov or Coco) and **static analysis/security** (clang-tidy, SAST/DAST, SBOM).
- **Central operation**: hosted Jenkins (server/VM), configuration as code, backups,
  dedicated/rootless build agents — the mounted host `docker.sock` is acceptable only
  for the local POC, **not production-ready**.
- **Actual product core**: an algorithm/AI detection test harness with versioned,
  frozen datasets.

---

## Status

POC — runs locally, not intended for production.
