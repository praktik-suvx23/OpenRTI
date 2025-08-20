
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
        stage ('looking') {
            steps {
                echo 'looking...'
                sh '''
                cd test/input
                ls
                '''
            }
        }
        stage('test') {
            steps {
                echo 'testing...'
                sh '''
                cd build
                chmod +x ../test/jenkinsTest/*.sh ../test/input/*.txt
                ctest
                '''
            }
        }
    }
}

