library('JenkinsPipelineUtils') _

podTemplate(inheritFrom: 'jenkins-agent-large', containers: [
    containerTemplate(name: 'dockcross', image: 'dockcross/linux-arm64', command: 'sleep', args: 'infinity', alwaysPullImage: true, envVars: [
    containerEnvVar(key: 'BUILDER_UID', value: '1000'),
    containerEnvVar(key: 'BUILDER_GID', value: '1000'),
    containerEnvVar(key: 'BUILDER_USER', value: 'jenkins'),
    containerEnvVar(key: 'BUILDER_GROUP', value: 'jenkins'),
  ])
]) {
    node(POD_LABEL) {
        stage('Build paperclock') {
            dir('PaperClock') {
                git branch: 'main',
                    credentialsId: '5f6fbd66-b41c-405f-b107-85ba6fd97f10',
                    url: 'https://github.com/pvginkel/PaperClock.git'
                    
                sh 'git rm tools/windows_simulator'
                sh 'git submodule update --init --recursive --depth 1'
                
                container('dockcross') {
                    sh 'scripts/dockcross/crossbuild.sh bcm'
                }
            }
            
            dir('HelmCharts') {
                git branch: 'main',
                    credentialsId: '5f6fbd66-b41c-405f-b107-85ba6fd97f10',
                    url: 'https://github.com/pvginkel/HelmCharts.git'
            }
        }
        
        stage('Deploy paperclock') {
            dir('PaperClock') {
                helmCharts.ssh('pvginkel@192.168.178.10', 'sudo systemctl stop paperclock')
                helmCharts.scp('bin/.', 'pvginkel@192.168.178.10:/var/local/paperclock/bin')
                helmCharts.ssh('pvginkel@192.168.178.10', 'sudo systemctl start paperclock')
            }
        }
    }
}
