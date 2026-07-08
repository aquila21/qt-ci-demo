# Setting up Jenkins for `aquila21/qt-ci-demo`

This documents how to stand up a local Jenkins instance in Docker and run this
repository's `Jenkinsfile` against it directly from GitHub — no local checkout
or bind mount required, since the repo is public.

Repo: https://github.com/aquila21/qt-ci-demo (branch: `main`)

## Prerequisites

- Docker Desktop installed and running (`docker info` should succeed with no
  errors — see the Troubleshooting section below if it doesn't).
- Port `8080` free on your machine.
- `Dockerfile.jenkins` from this repo, saved locally (needed to build the
  Jenkins image — see Step 1).

## Step 1 — Build a Jenkins image with the Docker CLI installed

The stock `jenkins/jenkins:lts` image has no `docker` command. This repo's
`Dockerfile.jenkins` adds it, so Jenkins can shell out to your host's Docker
engine to build and run the app image (the "Docker-outside-of-Docker" pattern).

Get the file, e.g.:
```
curl -O https://raw.githubusercontent.com/aquila21/qt-ci-demo/main/Dockerfile.jenkins
```
Then build:
```
docker build -t jenkins-with-docker -f Dockerfile.jenkins .
```

**Note on the container's default user:** the version of `Dockerfile.jenkins`
that ends with `USER jenkins` will hit a `permission denied` error when it
tries to reach `/var/run/docker.sock`, because the non-root `jenkins` user
inside the container isn't authorized to use the mounted host socket. If you
hit that error in Step 3, remove the trailing `USER jenkins` line and rebuild
so the container runs as root — a reasonable trade for a local POC, though not
one to carry into a production CI setup without a real risk conversation.

## Step 2 — Run the Jenkins container

```
docker run -d --name jenkins -p 8080:8080 -v /var/run/docker.sock:/var/run/docker.sock -v jenkins_home:/var/jenkins_home jenkins-with-docker
```

- `-v /var/run/docker.sock:/var/run/docker.sock` — lets the container's
  `docker` CLI talk to your host's Docker engine.
- `-v jenkins_home:/var/jenkins_home` — a named volume, so Jenkins' setup,
  plugins, and job configuration survive container restarts/recreations.
- **No workspace bind mount is needed here** — unlike testing against a local
  working copy, Jenkins will clone the repo directly from GitHub.

Sanity-check the socket works before continuing:
```
docker exec jenkins docker info
```
This should return full Docker info with no permission error. If it errors,
see Step 1's note above.

## Step 3 — First-time Jenkins setup

Get the initial admin password:
```
docker exec jenkins cat /var/jenkins_home/secrets/initialAdminPassword
```
Open `http://localhost:8080`, paste the password, choose **"Install suggested
plugins"** (this includes the Pipeline plugin the `Jenkinsfile` needs — no
extra plugin install required), then create an admin user (or continue as
admin).

## Step 4 — Create the Pipeline job

1. **New Item** → name it `qt-ci-demo` → type **Pipeline** → OK.
2. Under **Pipeline**, set **Definition** to **"Pipeline script from SCM"**.
3. **SCM**: Git.
4. **Repository URL**: `https://github.com/aquila21/qt-ci-demo`
   (public repo — no credentials needed).
5. **Branch Specifier**: `*/main`.
6. **Script Path**: `Jenkinsfile` (default — correct, since it's at the repo
   root).
7. Save.

## Step 5 — Run it

Click **Build Now**. Expect these stages in order, matching the `Jenkinsfile`:
`Checkout` → `Build image` → `Unit tests` → `Integration tests`, each
publishing JUnit results as it completes. A unit-test failure stops the
pipeline before the slower integration stage runs — that's the fail-fast
design, not a bug if you see it happen.

Check results via the job's **Test Result Trend** (expect 7 unit + 2
integration = 9 passing) and **Console Output** for the full log.

## Troubleshooting reference

These are real issues encountered setting this up, kept here so you don't have
to rediscover them:

| Symptom | Cause | Fix |
|---|---|---|
| `permission denied ... /var/run/docker.sock` | Container running as non-root `jenkins` user has no access to the mounted host socket | Remove `USER jenkins` from `Dockerfile.jenkins`, rebuild, recreate the container |
| `Conflict. The container name "/jenkins" is already in use` | A container with that name already exists (possibly stopped) | `docker rm jenkins` (or `docker rm -f jenkins`), then re-run Step 2 — the named `jenkins_home` volume persists your setup regardless |
| `fatal: not in a git directory` during Checkout, referencing `git config remote.origin.url` | Known Jenkins Git-plugin quirk tied to the "lightweight checkout" Jenkinsfile-fetch step | In the job's **Configure** page, under Pipeline, uncheck **"Lightweight checkout"** |
| `ssh: Could not resolve hostname c` | A Windows-style path (`C:/...`) was used as the Repository URL; git parses the colon as SCP-style `host:path` syntax | Not applicable here — this only affected local `file://` setups. With the public GitHub URL above, this won't occur. |
| `docker: Error response from daemon: failed to connect to the docker API at npipe://...` | Docker Desktop isn't running, or its engine/WSL2 backend crashed | Ensure Docker Desktop is running (system tray), restart it if needed, confirm with `docker info` before retrying anything else |

## Security note

Mounting the host Docker socket into the Jenkins container (Step 2) gives that
container effective root on the host. That's an acceptable trade for local
experimentation; it is **not** a pattern to carry into a production CI setup
without a real risk conversation (rootless Docker, a dedicated build-agent VM,
or a managed CI runner are the usual alternatives).
