#!/bin/sh

cd "`dirname "$0"`"
git submodule init
git submodule update
cd patches
git submodule init
git submodule update
git checkout master
git pull
