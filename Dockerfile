# ── Base image: Debian Bookworm (stable) ─────────────────────────
# P1: In Produktion per Digest pinnen (reproduzierbarer Build, Pflicht für Zertifizierungs-Evidenz):
#   docker build --build-arg BASE_IMAGE=debian:bookworm-slim@sha256:<DIGEST> .
# Digest ermitteln:
#   docker pull debian:bookworm-slim
#   docker inspect --format='{{index .RepoDigests 0}}' debian:bookworm-slim
# Default bleibt der bewegliche Tag, damit der POC out-of-the-box baut.
ARG BASE_IMAGE=debian:bookworm-slim
FROM ${BASE_IMAGE}

# ── Install build dependencies ────────────────────────────────────
# Hinweis (P1): Debian Bookworm liefert Qt 6.4.2 — NICHT die Auslieferungsversion.
# Für volle Reproduzierbarkeit Paketversionen pinnen (z. B. qt6-base-dev=<version>,
# exakte Strings via `apt-cache policy <pkg>`) oder snapshot.debian.org verwenden.
# Gegen die Auslieferungs-Qt-Version (6.11.x) stattdessen aqtinstall in einer
# eigenen Build-Stage nutzen statt qt6-base-dev aus apt.
RUN apt-get update && apt-get install -y --no-install-recommends \
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
# P2: JUnit-XML in /app/results (maschinenlesbar für Jenkins) UND Text nach stdout.
# Die '-o'-Option ist wiederholbar; nur eine Ausgabe darf nach stdout (-) gehen.
CMD ["sh", "-c", "mkdir -p /app/results && ./build/UnitTests -o /app/results/unit.xml,junitxml -o -,txt"]
