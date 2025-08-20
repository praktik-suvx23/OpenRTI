
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
                chmod +x ../test/admin_federate_test.sh ../test/launch_test.sh
                ctest
                '''
            }
        }
    }
}

