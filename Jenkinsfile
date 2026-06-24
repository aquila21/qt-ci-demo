pipeline {
    agent any

    options {
        // Fix: Pipeline nicht ewig hängen lassen, Build-Historie begrenzen
        timeout(time: 30, unit: 'MINUTES')
        buildDiscarder(logRotator(numToKeepStr: '30'))
    }

    // Optional (lokaler POC): Auto-Trigger ohne von außen erreichbares Jenkins.
    // Ein GitHub-Webhook gehört zur zentral gehosteten Produktionsumgebung.
    // triggers { pollSCM('H/5 * * * *') }

    environment {
        // Fix: eindeutiger Image-Tag pro Build -> keine Kollision bei parallelen Läufen
        IMG = "qt-ci-demo:${env.BUILD_NUMBER}"
        CONTAINER = "qt-ci-${env.BUILD_NUMBER}"
    }

    stages {

        stage('Checkout') {
            steps {
                echo 'Checking out source code...'
                checkout scm
            }
        }

        stage('Build & Test in Docker') {
            steps {
                echo 'Building Docker image...'
                sh 'docker build -t $IMG .'

                echo 'Running unit tests inside Debian container...'
                // '|| true': Test-Fehlschlag darf die Pipeline nicht abbrechen,
                // bevor die JUnit-Ergebnisse gesichert sind. Den Build-Status
                // setzt anschliessend der junit-Step (post.always).
                sh 'docker rm -f $CONTAINER || true'
                sh 'docker run --name $CONTAINER $IMG || true'

                echo 'Copying JUnit results out of the container...'
                sh 'rm -rf results && docker cp $CONTAINER:/app/results ./results'
            }
        }
    }

    post {
        always {
            // P2: maschinenlesbare Ergebnisse veröffentlichen (Trend/Historie).
            // Standardverhalten: fehlgeschlagene Tests -> Build UNSTABLE (gelb).
            junit 'results/*.xml'

            // Fix: Cleanup IMMER, auch bei Fehlern
            sh 'docker rm -f $CONTAINER || true'
            sh 'docker rmi $IMG || true'
        }
        success { echo 'All tests passed!' }
        unstable { echo 'Tests failed (build marked UNSTABLE).' }
        failure { echo 'Build error (compile/Docker failed).' }
    }
}
