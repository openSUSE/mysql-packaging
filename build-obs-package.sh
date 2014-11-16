#!/usr/bin/env bash

. `dirname "$0"`/common-config.sh

help() {
    echo "Automatic builder of all products and packages of mysql/mariadb"
    echo
    echo "Using this expects you to have properly configured osc command."
    echo
    exit 0
}

FAILED=0

# Run the build for each package and target
# param1: package name
# param2: target
# param3: nonfatal - do not die if the build failed
build_package() {
    echo -n "Test build ${2}/${1}: "
    pushd "${WORKDIR}/"*"/${1}" > /dev/null
    osc build --ccache --cpio-bulk-download --download-api-only ${2} &> /dev/null || {
        echo "FAILED"
        if [[ -z $3 ]]; then
            FAILED=1
        fi ;
        return
    }
    echo "PASSED"
    popd > /dev/null
}

# Run update for all of the DEVELPKGS
build_packages() {
    # must work
    for i in ${DEVELPKGS[@]}; do
        for j in ${SUPPORTED_PLATFORMS[@]}; do
            build_package $i $j
        done
    done
    # might work
    for i in ${MAYBE_WORKING_PLATFORMS[@]}; do
        for j in ${DEVELPKGS[@]}; do
            build_package $j $i 1
        done
    done
}

if [[ $1 == "--help" || $1 == "-h" ]]; then
    help
fi

build_packages

if [[ ${FAILED} == 1 ]]; then
   echo "Some important platforms failed to build!"
   exit 1
fi
