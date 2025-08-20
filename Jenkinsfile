
pipeline {
    agent any 
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

