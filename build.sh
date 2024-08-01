#!/bin/sh

set -e

./scripts/dockcross/build.sh bcm
scp -r -i ../HelmCharts/assets/kubernetes-pipeline-key bin/. pvginkel@192.168.178.10:/var/local/paperclock/bin
