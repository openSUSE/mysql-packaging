#!/usr/bin/env bash

# Fetch OBS packages for all currently supported platforms

help() {
    echo "Automatic updater syncing git state to OBS packages."
    echo
    echo "Using this expects you to have properly configured osc command."
    echo
    exit 0
}

WORKDIR="$(pwd)/obsclone"
DEVELPKGS=(
    "mariadb"
    "mariadb-100"
    "mariadb-55"
    "mysql-community-server-55"
    "mysql-community-server-56"
    "mysql-community-server-57"
)
SUPPORTED_PLATFORMS=(
    "openSUSE_12.3"
    "openSUSE_13.1"
    "openSUSE_13.2"
    "openSUSE_Factory"
    "SLE_12"
)

# Run the build for each package and target
# param1: package name
# param2: target
build_package() {
    echo "Going to build \"${1}\" on platform ${2}"
    pushd "${WORKDIR}/"*"/${1}" > /dev/null
    osc build --ccache --cpio-bulk-download --download-api-only ${2}
    popd > /dev/null
    echo "Success building package \"${1}\" on platform ${2}"
}

# Run update for all of the DEVELPKGS
build_packages() {
    for i in ${DEVELPKGS[@]}; do
        for j in ${SUPPORTED_PLATFORMS[@]}; do
            build_package $i $j
        done
    done
}

if [[ $1 == "--help" || $1 == "-h" ]]; then
    help
fi

build_packages
