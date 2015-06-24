#!/usr/bin/env bash

OLDROOT="`pwd`"
[ -z "$1" ]             || VARIANT="$1"
[ "$REPOROOT" ]         || REPOROOT="`dirname "$0"`"
[ "$VARIANT"  ]         || VARIANT="`basename "$OLDROOT"`"

if [[ ! -d "$REPOROOT"/"$VARIANT" ]]; then
   echo "No configuration for this variant found!"
   exit 1
fi

if [[ -z "`which mustache`" ]]; then
   echo "'mustache' not found!"
   echo
   echo "Mustache is templating software that is needed to create .spec file"
   exit 2
fi

rm -f *.diff

echo "Copying common files..."
if ! cp -Lv "$REPOROOT"/common/* .; then
   echo "Unable to copy files!"
   exit 3
fi

echo "Copying $VARIANT files..."
if ! cp -Lv "$REPOROOT"/"$VARIANT"/* .; then
   echo "Unable to copy files!"
   exit 4
fi

echo "Checking for additional patches to the project..."
for diff in [0-9]_*.diff [0-9][0-9]_*.diff; do
   if [ -f "$diff" ]; then
      echo " * using $diff..."
      if patch < "$diff"; then
         rm "$diff"
      else
         exit 5
      fi
   fi
done
rm *.orig &> /dev/null
echo "Additional patching done."

if [[ -f to_delete ]]; then
   cat to_delete | while read f; do
      echo "Deleting $f..."
      rm -f "$f"
   done
fi
rm -f to_delete

echo "Creating files from templates..."
if [ "`head -n 1 config.yaml | sed -n 's|---|yes|p'`" ]; then
   echo -e '1d\nwq\n' | ed config.yaml  > /dev/null 2> /dev/null
fi
if [ "`tail -n 1 default.yaml | sed -n 's|---|yes|p'`" ]; then
   echo -e '$d\nwq\n' | ed default.yaml > /dev/null 2> /dev/null
fi
cat ./default.yaml ./config.yaml > ./whole-config.yaml
sync

# If it is preferred variant set correct library name
[ "`echo '0{{preferred}}' | mustache ./whole-config.yaml -`" -le 0 ] || sed -i 's|^lib-name:.*|lib-name:\ mysqlclient|' ./whole-config.yaml

SPEC="`ls -1 *.spec | head -n1`"

PKGNAME="`echo '{{pkg-name}}' | mustache ./whole-config.yaml -`"
for i in ./*.in; do
   REALNAME="`echo "$i" | sed -e 's|\.in$||'`"
   echo "Creating \"$REALNAME\"..."
   if !  mustache ./whole-config.yaml "$i" > "$REALNAME"; then
      echo "Creating \"$REALNAME\" failed!"
      exit 6
   fi
done

sync

sleep 1

[ "$SPEC" = "mysql.spec" ] || mv "mysql.spec" "$SPEC"

echo "Packing configuration..."
if ! tar -cjf configuration-tweaks.tar.bz2 *.cnf; then
   echo "Unable to create configuration tarball!"
   exit 4
fi

echo "Adding patches..."
if ! "$REPOROOT"/patches/tools/gettar.sh; then
   echo "Getting patches failed!"
   exit 4
fi

echo "Cleaning up..."
rm -f ./*.in ./default.yaml ./config.yaml ./whole-config.yaml ./*.cnf
if [[ -x ./last_run.sh ]]; then
   ./last_run.sh && rm ./last_run.sh
fi
