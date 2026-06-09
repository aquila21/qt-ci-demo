# ── Base image: Debian Bookworm (stable) ─────────────────────────
FROM debian:bookworm-slim

# ── Install build dependencies ────────────────────────────────────
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    ninja-build \
    qt6-base-dev \
    qt6-base-dev-tools \
    libqt6test6 \
    && rm -rf /var/lib/apt/lists/*

# ── Set working directory ─────────────────────────────────────────
WORKDIR /app

# ── Copy project source ───────────────────────────────────────────
COPY CMakeLists.txt .
COPY src/ src/
COPY tests/ tests/

# ── Build ─────────────────────────────────────────────────────────
RUN cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build

# ── Run unit tests ────────────────────────────────────────────────
CMD ["./build/UnitTests"]
