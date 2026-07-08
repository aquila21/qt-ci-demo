# Setting up Jenkins for `aquila21/qt-ci-demo`

This documents how to stand up a local Jenkins instance in Docker and run this
repository's `Jenkinsfile` against it directly from GitHub. The steps below
are ordered to avoid five real issues discovered the hard way the first time
this was set up (Windows host, Docker Desktop, WSL2 backend) — see the
Troubleshooting table if you still hit one, but following Steps 1–5 as written
should avoid all of them.

Repo: https://github.com/aquila21/qt-ci-demo (branch: `main`)

## Prerequisites

- Docker Desktop installed and running (`docker info` succeeds with no
  errors — see Troubleshooting if not).
- Port `8080` free.
- A local folder to hold shared test results, e.g. `C:\dev\qt-ci-demo\jenkins-results`
  — create it now if it doesn't exist.
- `Dockerfile.jenkins` from this repo, saved locally.

## Step 1 — Build a Jenkins image with the Docker CLI installed

The stock `jenkins/jenkins:lts` image has no `docker` command. `Dockerfile.jenkins`
adds it, so Jenkins can shell out to the host's Docker engine to build and run
the app image ("Docker-outside-of-Docker" / DooD).

```
curl.exe -O https://raw.githubusercontent.com/aquila21/qt-ci-demo/main/Dockerfile.jenkins
docker build -t jenkins-with-docker -f Dockerfile.jenkins .
```
(Use `curl.exe`, not bare `curl` — in PowerShell, `curl` is an alias for
`Invoke-WebRequest`, which doesn't support `-O` the way real curl does.)

**The image must run as root.** If `Dockerfile.jenkins` ends with `USER jenkins`,
remove that line before building. The non-root `jenkins` user has no access to
the mounted host Docker socket (Step 2), and will fail with `permission denied`.
Running this container as root is an acceptable trade for a local sandbox; not
a pattern to carry into a shared/production CI setup without a separate risk
assessment.

## Step 2 — Run the Jenkins container

```
docker run -d --name jenkins -p 8080:8080 -v /var/run/docker.sock:/var/run/docker.sock -v jenkins_home:/var/jenkins_home -v C:/dev/qt-ci-demo/jenkins-results:/var/jenkins_home/workspace/qt-ci-demo/results -e HOST_RESULTS_DIR="/run/desktop/mnt/host/c/dev/qt-ci-demo/jenkins-results" jenkins-with-docker
```

Adjust both the `-v` path and the `HOST_RESULTS_DIR` value if your local
folder isn't `C:\dev\qt-ci-demo\jenkins-results` — same folder, two different
representations (see Step 5 and the Troubleshooting table for why both are
needed).

- `/var/run/docker.sock` mount — lets the container's `docker` CLI reach the
  host engine.
- `jenkins_home` named volume — Jenkins setup/plugins/job config persist
  across container recreation.
- The results-folder mount, at the exact path `/var/jenkins_home/workspace/qt-ci-demo/results`
  — pre-places your shared results folder *inside* where Jenkins will create
  the job's workspace, which the `junit` step requires (it doesn't reliably
  find result files outside the workspace).
- `HOST_RESULTS_DIR` — the same physical folder, addressed the way the **host
  Docker engine** needs to see it when a `docker run` command originates from
  inside this container (see Troubleshooting: "No test report files found").

Verify:
```
docker exec jenkins docker info
```
Must return full Docker info, no permission error.

## Step 2.1 — Pre-empt a git ownership-check failure

Do this now, before creating any job — it avoids a confusing `fatal: not in a
git directory` error that otherwise appears intermittently during checkout:
```
docker exec jenkins git config --system --add safe.directory '*'
```
This is a known git behavior (not a bug): git ≥2.35.2 refuses to operate in a
directory it doesn't consider "owned" by the current user, which this
containerized, root-run setup can trip on ownership-wise depending on how the
socket-shared host writes files. `--system` (not `--global`) matters here — it
writes to `/etc/gitconfig`, read by every user regardless of `$HOME`, avoiding
a subtler failure mode where a `--global` write lands in a `$HOME` the actual
Jenkins process doesn't use.

## Step 3 — First-time Jenkins setup

```
docker exec jenkins cat /var/jenkins_home/secrets/initialAdminPassword
```
Open `http://localhost:8080`, paste the password, **Install suggested plugins**
(includes Pipeline — nothing extra needed), create an admin user.

## Step 4 — Create the Pipeline job

1. **New Item** → name `qt-ci-demo` → type **Pipeline** → OK.
2. **Pipeline** → **Definition**: "Pipeline script from SCM".
3. **SCM**: Git.
4. **Repository URL**: `https://github.com/aquila21/qt-ci-demo`.
5. **Branch Specifier**: `*/main`.
6. **Script Path**: `Jenkinsfile`.
7. Save.

## Step 5 — Run it

**Build Now.** Expected stages: `Checkout` → `Build image` → `Unit tests` →
`Integration tests`, each publishing JUnit results. A unit-test failure stops
the pipeline before integration runs — fail-fast by design.

Check **Test Result Trend** (expect 7 unit + 2 integration = 9 passing) and
**Console Output**.

The `Jenkinsfile` at the repo root must contain, for the results handling to
work with Step 2's mount layout:
```groovy
stage('Unit tests') {
    steps {
        sh 'mkdir -p results && rm -rf results/*'
        sh 'docker run --rm -v "$HOST_RESULTS_DIR:/app/results" $IMG sh -c \
            "ctest --test-dir build -L unit --output-on-failure \
            --output-junit /app/results/unit.xml"'
    }
    post { always { junit 'results/unit.xml' } }
}
stage('Integration tests') {
    steps {
        sh 'docker run --rm -v "$HOST_RESULTS_DIR:/app/results" $IMG sh -c \
            "ctest --test-dir build -L integration --output-on-failure \
            --output-junit /app/results/integration.xml"'
    }
    post { always { junit 'results/integration.xml' } }
}
```
Note `junit` uses **workspace-relative** paths (`results/unit.xml`), while the
inner `docker run` uses the **daemon-native** `$HOST_RESULTS_DIR` — these
resolve to the same folder through two different mechanisms; see
Troubleshooting for why both forms are required.

## Troubleshooting reference

| Symptom | Cause | Fix |
|---|---|---|
| `permission denied ... /var/run/docker.sock` | Container running as non-root `jenkins` user | Remove `USER jenkins` from `Dockerfile.jenkins`, rebuild, recreate container |
| `Conflict. The container name "/jenkins" is already in use` | A container with that name already exists (possibly stopped) | `docker rm -f jenkins`, then re-run Step 2 — `jenkins_home` persists regardless |
| `fatal: not in a git directory`, referencing `git config remote.origin.url`, during the Jenkinsfile-fetch step specifically | Git plugin's "lightweight checkout" quirk | Job **Configure** → Pipeline → uncheck **Lightweight checkout**. Should not occur at all if Step 2.5 was done first. |
| `fatal: not in a git directory` during the **main** `Checkout` stage (not the lightweight fetch) | Git ≥2.35.2 ownership check; a prior `--global safe.directory` fix didn't take effect because it was written to the wrong `$HOME` | `docker exec jenkins git config --system --add safe.directory '*'` (Step 2.5) — `--system` avoids the `$HOME` dependency entirely |
| `ssh: Could not resolve hostname c` | A Windows-style path (`C:/...`) used as the Repository URL; git parses the colon as SCP-style `host:path` | Use the GitHub HTTPS URL, never a local Windows path, as the Repository URL |
| `docker: failed to connect to the docker API at npipe://...` | Docker Desktop not running, or its engine/WSL2 backend crashed | Start/restart Docker Desktop, confirm with `docker info` before retrying |
| `.../Jenkinsfile not found` after a successful checkout | The repo's `main` branch genuinely has no `Jenkinsfile` at its root | Add `Jenkinsfile` to the repo root, commit, push |
| `rm: cannot remove '/shared-results': Device or resource busy` | Attempting to `rm -rf` a bind-mount point itself, not its contents | Clear contents only: `rm -rf /shared-results/*` (or, with this doc's layout, `results/*`), never `rm` the mount point |
| Tests pass, but `junit` reports **"No test report files were found. Configuration error?"** | Two possible causes, both addressed by Step 2's layout: (1) the `docker run` mount source was a Windows-style path (`C:/...`), which only the Windows-side Docker client auto-translates — a Linux `docker` CLI calling the same daemon over the socket does **not** get that translation, so the file is silently written to an unrelated auto-created path; (2) even with a correct path, the `junit` step does not reliably find result files located **outside** the job's workspace | (1) Use the daemon-native path form (`/run/desktop/mnt/host/<drive-letter>/...`) for any `docker run -v` issued from inside a socket-mounted container. (2) Mount the shared results folder directly at the job's workspace path (as in Step 2) and reference it with a workspace-relative path in `junit` |

## Security note

Mounting the host Docker socket into the Jenkins container gives that
container effective root on the host. Acceptable for local experimentation;
requires a separate risk conversation before use in any shared or production
CI environment (rootless Docker, a dedicated build-agent VM, or a managed CI
runner are the usual production alternatives).
