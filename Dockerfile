ARG BASE_IMAGE=debian:bookworm-slim
FROM ${BASE_IMAGE}

# NOTE (unchanged from POC): Bookworm ships Qt 6.4.2, not the shipping 6.11.x.
# Pin package versions or use aqtinstall for a certifiable, reproducible build.
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake build-essential ninja-build ca-certificates \
    qt6-base-dev qt6-base-dev-tools libqt6test6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY CMakeLists.txt .
COPY src/  src/
COPY tests/ tests/

# GoogleTest is fetched + hash-checked here (needs network during build).
# For a fully offline/hermetic build, vendor googletest and switch FetchContent
# to a local SOURCE_DIR instead of the URL.
RUN cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build

# Staged, framework-agnostic reporting: ctest aggregates GoogleTest + QtTest
# results into one JUnit file regardless of framework.
CMD ["sh", "-c", "mkdir -p /app/results && ctest --test-dir build --output-on-failure --output-junit /app/results/tests.xml"]
