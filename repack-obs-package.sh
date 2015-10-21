#!/usr/bin/env bash

. `dirname "$0"`/common-config.sh

help() {
    echo "Run recursively the update-package script on the obs clone"
    echo
    echo "Using this expects you to have properly configured osc command."
    echo "Parameters:"
    echo "	--nodownload	do not download the new source tarball"
    exit 0
}

# Find and remove the old source tarball and replace it with the new one
replace_source_tarball(){
	find . -regex '\./\(mariadb\|mysql\)-[0-9.]+\.tar\.\(gz\|bz2\)' -delete

    echo "Downloading the new source tarball..."
    if `osc -A https://api.opensuse.org service localrun download_files` ; then
        echo "    The new source tarball was downloaded"
    else
        echo -e "${RED}Download failed! ${NC}"
        exit 1
    fi

    osc -A https://api.opensuse.org addremove
}

# Run the update script in each of the pkg dirs
# param1: package name
# param2: script option
update_package() {
    echo -e "${YELLOW}Working on \"${1}\" package${NC}"
    echo
    pushd "${WORKDIR}/"*"/${1}" > /dev/null
    bash ../../../update-package.sh || exit 1

    if [[ $2 == "--nodownload" ]]; then
        echo "Leaving the original tarball untouched"
    else
        replace_source_tarball
    fi

    popd > /dev/null
    echo
    echo -e "${GREEN}Package \"${1}\" is successfully prepared${NC}"
    echo
    echo "------------------------------------------------------------------------"

}

# Run update for all of the DEVELPKGS
# param1: script option
update_packages() {
    echo -e \
    "${BLUE}\
------------------------------------------------------------------------
Updating all packages...
------------------------------------------------------------------------\
${NC}"

    for i in ${DEVELPKGS[@]}; do
        update_package $i $1
    done
}

if [[ $1 == "--help" || $1 == "-h" ]]; then
    help
fi

update_packages $1
