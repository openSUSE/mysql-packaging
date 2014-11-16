#!/usr/bin/env bash

. `dirname "$0"`/common-config.sh

help() {
    echo "Run recursively the update-package script on the obs clone"
    echo
    echo "Using this expects you to have properly configured osc command."
    echo
    exit 0
}

# Run the update script in each of the pkg dirs
# param1: package name
update_package() {
    echo "Working on package \"${1}\""
    pushd "${WORKDIR}/"*"/${1}" > /dev/null
    bash ../../../update-package.sh ${1} || exit 1
    popd > /dev/null
    echo "Updated package \"${1}\""
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
