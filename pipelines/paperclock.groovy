library('JenkinsPipelineUtils') _

podTemplate(inheritFrom: 'jenkins-agent-large', containers: [
    containerTemplates.dockbuild('dockcross', 'dockcross/linux-arm64'),
]) {
    node(POD_LABEL) {
        stage('Build paperclock') {
            dir('PaperClock') {
                git branch: 'main',
                    credentialsId: '5f6fbd66-b41c-405f-b107-85ba6fd97f10',
                    url: 'https://github.com/pvginkel/PaperClock.git'
                    
                cache(defaultBranch: 'main', caches: [
                    arbitraryFileCache(path: 'build/lib/libbacktrace', cacheValidityDecidingFile: gitUtils.getTreeHashFile('lib/libbacktrace')),
                    arbitraryFileCache(path: 'build/lib/lvgl', cacheValidityDecidingFile: gitUtils.getTreeHashFile('lib/lvgl')),
                    arbitraryFileCache(path: 'build/lib/lg', cacheValidityDecidingFile: gitUtils.getTreeHashFile('lib/lg')),
                    arbitraryFileCache(path: 'build/lib/freetype', cacheValidityDecidingFile: gitUtils.getTreeHashFile('lib/freetype')),
                ]) {
                    container('dockcross') {
                        sh 'gosu 1000:1000 scripts/dockcross/crossbuild.sh bcm'
                    }
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
