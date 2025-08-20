
pipeline {
    agent any 
    stages {
        stage('build') {
            steps {
                echo 'building...'
                sh '''
                cd build
                cmake ..
                make
                '''
            }
        }
        stage('test') {
            steps {
                echo 'testing...'
                sh '''
                cd build
                ctest
                '''
            }
        }
    }
}

