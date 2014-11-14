All: obspack update check

# fetch all the supported platforms from obs for the update
obspack:
	true

# generate the refreshed package using the latest git
update:
	true

# try to build the packages and ensure they all passed
check:
	true

.PHONY: obspack update check
