# Set of tools to work with mysql/mariadb updates in openSUSE Leap

**Important**:
Please note that openSUSE:Factory mariadb package uses a classic
OBS process, not this repository. This repository is used only
for the purpose of updating openSUSE:Leap mariadb/mysql-community-server
packages.

This repository contains tools which allows one to maintain and develop
various MariaDB/MySQL versions that are used in openSUSE Leap and
supported by MariaDB or MySQL respectively.

## Developement

There are multiple folders with the git repository:

 * common: contains common content shared among all versions/types
 * patches: directory containing various patches for all supported platforms
 * {mysql,mariadb}-`<version`>: specific overrides for the common settings defined in the above dir

In short you just create changes you desire to do and run make.
The packages will be checked out from OBS, refreshed and in the end
also build. If all this pass you should commit to git and to all
desired OBS projects.

## Maintenance updates

openSUSE released versions are to be kept updated to latest minor
branch of the series.
In example when doing update for mariadb-10.0.31 you should check what
distros have 10.0.something in repository and resubmit to all of those.
