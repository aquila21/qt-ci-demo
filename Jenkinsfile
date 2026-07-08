pipeline {
    agent any
    options { timeout(time: 30, unit: 'MINUTES'); buildDiscarder(logRotator(numToKeepStr: '30')) }
    environment { IMG = "qt-ci-demo:${env.BUILD_NUMBER}" }

    stages {
        stage('Checkout') { steps { checkout scm } }

        stage('Build image') {
            steps { sh 'docker build -t $IMG .' }
        }

        // Fast gate first: unit tests must pass before we spend time on integration.
        stage('Unit tests') {
            steps {
                sh 'rm -rf results && mkdir -p results'
                sh 'docker run --rm -v $PWD/results:/app/results $IMG sh -c \
                      "ctest --test-dir build -L unit --output-on-failure \
                       --output-junit /app/results/unit.xml"'
            }
            post { always { junit 'results/unit.xml' } }
        }

        stage('Integration tests') {
            steps {
                sh 'docker run --rm -v $PWD/results:/app/results $IMG sh -c \
                      "ctest --test-dir build -L integration --output-on-failure \
                       --output-junit /app/results/integration.xml"'
            }
            post { always { junit 'results/integration.xml' } }
        }
    }
    post { always { sh 'docker rmi $IMG || true' } }
}
