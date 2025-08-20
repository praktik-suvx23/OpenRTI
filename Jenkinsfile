
pipeline {
    agent any 
    stages {
        stage('build') {
            steps {
                echo 'building...'
                sh '''
                mkdir build
                cd build
                cmake ..
                make
                make run_simulation
                '''
            }
        }
    }
}

