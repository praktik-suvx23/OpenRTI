
pipeline {
    agent { docker { image 'node:22.18.0-alpine3.22' } }
    stages {
        stage('build') {
            steps {
                echo 'building...'
                sh 'cmake ..'
                sh 'make'
            }
        }
    }
}

