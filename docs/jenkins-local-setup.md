# Jenkins local setup

This describes a local, disposable Jenkins-in-Docker sandbox for exercising
`Jenkinsfile` end to end on a workstation. It is **not** the production CI
configuration — it exists to let a developer validate pipeline changes before
they reach a real Jenkins controller/agent setup.

Repo: https://github.com/aquila21/qt-ci-demo (branch: `main`)

## Prerequisites

- Docker installed and running (`docker info` succeeds).
- Port `8080` free.
- `Dockerfile.jenkins` (repo root) available locally.

## 1. Build a Jenkins image with the Docker CLI installed

The stock `jenkins/jenkins:lts` image has no `docker` command. `Dockerfile.jenkins`
adds it, so Jenkins can shell out to the host's Docker engine to build and run
the app image ("Docker-outside-of-Docker").

```
docker build -t jenkins-with-docker -f Dockerfile.jenkins .
```

If the image's final line is `USER jenkins`, the container will fail to reach
`/var/run/docker.sock` with `permission denied`, because the non-root user has
no access to the mounted host socket. For this sandbox, remove that line so
the container runs as root — acceptable for local use; not a pattern to carry
into a production CI setup without a separate risk assessment (rootless
Docker, a dedicated build-agent VM, or a managed CI runner are the usual
production alternatives).

## 2. Run the container

```
docker run -d --name jenkins -p 8080:8080 -v /var/run/docker.sock:/var/run/docker.sock -v jenkins_home:/var/jenkins_home jenkins-with-docker
```

- `/var/run/docker.sock` mount: lets the container's `docker` CLI reach the
  host engine.
- `jenkins_home` named volume: persists setup, plugins, and job config across
  container recreation.
- No workspace bind mount is required — the job pulls the repo from GitHub
  directly.

Verify before continuing:
```
docker exec jenkins docker info
```
This must return full Docker info with no permission error.

## 3. First-time setup

```
docker exec jenkins cat /var/jenkins_home/secrets/initialAdminPassword
```
Open `http://localhost:8080`, enter the password, choose **Install suggested
plugins** (includes the Pipeline plugin the `Jenkinsfile` needs), then create
an admin user.

## 4. Create the pipeline job

| Field | Value |
|---|---|
| Item type | Pipeline |
| Definition | Pipeline script from SCM |
| SCM | Git |
| Repository URL | `https://github.com/aquila21/qt-ci-demo` |
| Branch Specifier | `*/main` |
| Script Path | `Jenkinsfile` |

Public repo — no credentials required.

## 5. Run

**Build Now.** Expected stage order, per `Jenkinsfile`: Checkout → Build image
→ Unit tests → Integration tests, with JUnit results published after each test
stage. A unit-test failure stops the pipeline before integration tests run
(fail-fast by design).

Expected result: 7 unit + 2 integration = 9 tests passing, visible under
**Test Result Trend**.

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `permission denied ... /var/run/docker.sock` | Container runs as non-root `jenkins` user | Remove `USER jenkins` from `Dockerfile.jenkins`, rebuild, recreate container |
| `Conflict. The container name "/jenkins" is already in use` | A container with that name already exists (possibly stopped) | `docker rm -f jenkins`, then re-run step 2 — `jenkins_home` is a named volume and persists independently |
| `fatal: not in a git directory`, referencing `git config remote.origin.url` during Checkout | Known Jenkins Git-plugin issue tied to the lightweight-checkout step used to fetch the `Jenkinsfile` before the main checkout | Job **Configure** → Pipeline → uncheck **Lightweight checkout** |
| `docker: failed to connect to the docker API at npipe://...` | Docker Desktop/engine not running or its backend crashed | Start/restart Docker Desktop, confirm with `docker info` before retrying |

## Security note

Mounting the host Docker socket grants the container effective root on the
host. Acceptable for this local sandbox; requires a separate risk discussion
before use in any shared or production CI environment.
