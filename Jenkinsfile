
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
                make run_simulation
                '''
            }
        }
    }
}

