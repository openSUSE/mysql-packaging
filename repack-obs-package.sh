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
    "mariadb-101"
    "mariadb-55"
    "mysql-community-server-55"
    "mysql-community-server-56"
    "mysql-community-server-57"
)

# Run the update script in each of the pkg dirs
# param2: package name
update_package() {
    echo "Working on package \"${i}\""
    pushd "${WORKDIR}/"*"/${i}" > /dev/null
    bash ../../../update-package.sh ${i} || exit 1
    popd > /dev/null
    echo "Updated package \"${i}\""
}

# Run update for all of the DEVELPKGS
update_packages() {
    for i in ${DEVELPKGS[@]}; do
        update_package $i
    done
}

if [[ $1 == "--help" || $1 == "-h" ]]; then
    help
fi

update_packages
