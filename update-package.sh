#!/bin/sh

 ##############################################################################
 #                                                                            #
 # Copyright (C) 2011 by Michal Hrusecky <Michal@Hrusecky.net>                #
 # All rights reserved.                                                       #
 #                                                                            #
 # Redistribution  and  use  in  source  and  binary  forms,  with or without #
 # modification, are permitted  provided  that  the following  conditions are #
 # met:                                                                       #
 #                                                                            #
 # 1. Redistributions  of source code must retain the above copyright notice, #
 #    this list of conditions and the following disclaimer.                   #
 #                                                                            #
 # 2. Redistributions  in binary  form  must  reproduce the  above  copyright #
 #    notice, this  list of conditions  and the  following  disclaimer in the #
 #    documentation and/or other materials provided with the distribution.    #
 #                                                                            #
 # 3. The name of the author may not be used to endorse  or  promote products #
 #    derived from this software without specific prior written permission.   #
 #                                                                            #
 # THIS  SOFTWARE IS  PROVIDED  BY THE AUTHOR  ``AS IS''  AND ANY  EXPRESS OR #
 # IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  WARRANTIES #
 # OF  MERCHANTABILITY AND FITNESS  FOR A PARTICULAR  PURPOSE ARE DISCLAIMED. #
 # IN  NO  EVENT  SHALL THE  AUTHOR  BE  LIABLE  FOR  ANY  DIRECT,  INDIRECT, #
 # INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLUDING,  BUT #
 # NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE, #
 # DATA, OR  PROFITS; OR  BUSINESS  INTERRUPTION) HOWEVER  CAUSED  AND ON ANY #
 # THEORY  OF  LIABILITY,  WHETHER  IN CONTRACT,  STRICT  LIABILITY,  OR TORT #
 # (INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF #
 # THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          #
 #                                                                            #
 ##############################################################################

OLDROOT="`pwd`"
[ "$REPOROOT" ] || REPOROOT="`dirname "$0"`"
[ "$VARIANT"  ] || VARIANT="`basename "$OLDROOT"`"

if ! [ -d "$REPOROOT"/"$VARIANT" ]; then
   echo "No configuration for this variant found!"
   exit 1
fi

if [ -z "`which mustache`" ]; then
   echo "'mustache' not found!"
   echo
   echo "Mustache is templating software that is needed to create .spec file"
   echo "You can install it by"
   echo "   gem install mustache"
   exit 2
fi

echo "Copying common files..."
if ! cp -v "$REPOROOT"/common/* .; then
   echo "Can't copy files!"
   exit 3
fi

echo "Copying $VARIANT files..."
if ! cp -v "$REPOROOT"/"$VARIANT"/* .; then
   echo "Can't copy files!"
   exit 4
fi

echo "Creating files from templates..."
PKGNAME="`echo '{{pkg-name}}' | mustache ./config.yaml -`"
if [ "$PKGNAME" \!= "$VARIANT" ]; then
   EXTRAPKGNAME="`echo '{{extra_provides}}' | mustache ./config.yaml -`"
   sed -i "s|\(pkg-name:[[:blank:]]\+\)$PKGNAME|\1$EXTRAPKGNAME|" config.yaml
   sed -i "s|\(use_extra_provides:[[:blank:]]\+\)1|\10|" config.yaml
fi

if [ "`head -n 1 config.yaml | sed -n 's|---|yes|p'`" ]; then
   echo -e '1d\nwq\n' | ed config.yaml  > /dev/null 2> /dev/null
fi
if [ "`tail -n 1 default.yaml | sed -n 's|---|yes|p'`" ]; then
   echo -e '$d\nwq\n' | ed default.yaml > /dev/null 2> /dev/null
fi
cat ./default.yaml ./config.yaml > ./whole-config.yaml
sync

SPEC="`ls -1 *.spec | head -n1`"

PKGNAME="`echo '{{pkg-name}}' | mustache ./whole-config.yaml -`"
for i in ./*.in; do
   REALNAME="`echo "$i" | sed -e 's|\.in$||'`"
   echo "Creating \"$REALNAME\"..."
   if !  mustache ./whole-config.yaml "$i" > "$REALNAME"; then
      echo "Creating \"$REALNAME\" failed!"
      exit 4
   fi
done

sync

sleep 1

[ "$SPEC" = "mysql.spec" ] || mv "mysql.spec" "$SPEC"

echo "Packing configuration..."
if ! tar -cjf configuration-tweaks.tar.bz2 *.cnf; then
   echo "Can't create configuration tarball!"
   exit 4
fi

echo "Adding patches..."
if ! "$REPOROOT"/patches/tools/gettar.sh; then
   echo "Getting patches failed!"
   exit 4
fi

echo "Cleaning up..."
rm -f ./*.in ./default.yaml ./config.yaml ./whole-config.yaml ./*.cnf
