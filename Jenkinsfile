pipeline {
    agent any

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
                sh 'docker build -t qt-ci-demo .'

                echo 'Running unit tests inside Debian container...'
                sh 'docker run --rm qt-ci-demo'
            }
        }

        stage('Cleanup') {
            steps {
                echo 'Removing Docker image...'
                sh 'docker rmi qt-ci-demo || true'
            }
        }
    }

    post {
        success {
            echo 'All tests passed!'
        }
        failure {
            echo 'Build or tests failed!'
        }
    }
}
