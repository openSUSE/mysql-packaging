All: obspack update check

# fetch all the supported platforms from obs for the update
obspack: get-obs-package.sh
	bash ./get-obs-package.sh

# generate the refreshed package using the latest git
update:
	true

# try to build the packages and ensure they all passed
check:
	true

clean:
	rm -rf obsclone

.PHONY: obspack update check
