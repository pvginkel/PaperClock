library('JenkinsPipelineUtils') _

withCredentials([
    string(credentialsId: 'IOTSUPPORT_CLIENT_ID', variable: 'IOTSUPPORT_CLIENT_ID'),
    string(credentialsId: 'IOTSUPPORT_CLIENT_SECRET', variable: 'IOTSUPPORT_CLIENT_SECRET'),
]) {
    podTemplate(inheritFrom: 'jenkins-agent-large', containers: [
        containerTemplate(name: 'idf', image: 'espressif/idf:v5.5.2', command: 'sleep', args: 'infinity', envVars: [
            containerEnvVar(key: 'IOTSUPPORT_CLIENT_ID', value: '$IOTSUPPORT_CLIENT_ID'),
            containerEnvVar(key: 'IOTSUPPORT_CLIENT_SECRET', value: '$IOTSUPPORT_CLIENT_SECRET'),
        ])
    ]) {
        node(POD_LABEL) {
            stage('Build paper clock') {
                dir('esp-libs') {
                    git branch: 'main',
                        credentialsId: '5f6fbd66-b41c-405f-b107-85ba6fd97f10',
                        url: 'https://github.com/pvginkel/esp-libs.git'
                }

                dir('ThermostatProxy') {
                    git branch: 'main',
                        credentialsId: '5f6fbd66-b41c-405f-b107-85ba6fd97f10',
                        url: 'https://github.com/pvginkel/PaperClock.git'
                        
                    container('idf') {
                        // Necessary because the IDF container doesn't have support
                        // for setting the uid/gid.
                        sh 'git config --global --add safe.directory \'*\''

                        sh '/opt/esp/entrypoint.sh idf.py build'
                    }
                }
            }
            stage('Deploy paper clock') {
                dir('PaperClock') {
                    sh 'chmod +x scripts/upload.sh'
                    sh 'scripts/upload.sh https://iot.ginbov.nl'
                }
            }
        }
    }
}
