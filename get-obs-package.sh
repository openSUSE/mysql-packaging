#!/usr/bin/env bash

. `dirname "$0"`/common-config.sh

# Fetch OBS packages for all currently supported platforms

help() {
    echo "Automatic downloader for all supported mariadb/mysql packages from buildservice."
    echo
    echo "Using this expects you to have properly configured osc command."
    echo
    echo "Parameters:"
    echo "	--refresh	force refresh of the repositories from scratch"
    echo "			otherwise presumed they are already in correct state"
    exit 0
}

# Create new branch of the package in your home and copy it on the disk
# param1: source prj
# param2: package name
checkout_package() {
    local _branchstate=$(mktemp)
    local _prjname=""

    osc -A https://api.opensuse.org branch $1/$2 &> $_branchstate

    # if the branch already exist, recurse and quit
    if grep -q '^branch target package already exists:' $_branchstate ; then
        _prjname=`cat $_branchstate | grep "branch target package already exists" | sed 's/^branch target package already exists: //'`
        osc -A https://api.opensuse.org rdelete -f $_prjname -m "Replacing with new checkout" &> /dev/null
        checkout_package $1 $2
        return
    elif grep -q 'A working copy of the branched package can be checked out with' $_branchstate ; then
        _prjname="`cat $_branchstate | tail -n1 | sed 's/osc co //'`"
    else
	cat $_branchstate
        echo "ERROR: branching of package $1/$2 failed"
        exit 1
    fi
    pushd "${WORKDIR}" > /dev/null
    osc -A https://api.opensuse.org co $_prjname &> /dev/null || {
        echo "ERROR: checkout of project $_prjname failed";
        exit 1;
    }
    popd > /dev/null
    echo "Checked out package \"${WORKDIR}/$_prjname\""
}

# Prepare list of all packages we need to checkout
checkout_packages() {
    for i in ${DEVELPKGS[@]}; do
        echo "Checking out package..."
        checkout_package ${DEVELPROJECT} $i
    done
}

if [[ $1 == "--help" || $1 == "-h" ]]; then
    help
fi

# first some sanity checks
if [[ -e "${WORKDIR}" ]]; then
    if [[ $1 == "--refresh" ]]; then
        rm -rf "${WORKDIR}"
    else
        echo "There already is present working directory and refresh was not given, skipping."
    fi
    exit 0
else
    mkdir -p "${WORKDIR}"
fi

checkout_packages
