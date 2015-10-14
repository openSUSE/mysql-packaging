#!/bin/sh

 ##############################################################################
 #                                                                            #
 # Copyright (C) 2010 by Michal Hrusecky <Michal@Hrusecky.net>                #
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

series="`ls -1 *.series series 2> /dev/null`"
[ -z "$1" ] || series="$1"
if [ -z "$series" ]; then
   echo "ERROR: No series file!"
   echo
   echo "Usage:"
   echo "   $0 [series]"
   exit 3
fi

[ "$PATCHROOT" ] || PATCHROOT="`echo "$0" | sed 's|gettar.sh|..|'`"

pushd . > /dev/null
cd "$PATCHROOT"
PATCHROOT="`pwd`"
NAME="`ls -1d *-patches`"
PATCHDIR="$PATCHROOT/$NAME"
popd > /dev/null

TO_PACK="./tools ./README ./MAINTAINERS"

echo "Will create tarball from the following patches:"
while read patch; do
   patch="`echo "$patch" | sed -e 's|^[[:blank:]]*||' -e 's|[[:blank:]]*#.*||'`"
   [ -z "$patch" ] && continue
   [ \! -f "$PATCHDIR/$patch" ] && [ -f "$PATCHDIR/$patch.patch" ] && patch="$patch.patch"
   if [ \! -f "$PATCHDIR/$patch" ]; then
      echo " ! Patch \"$patch\" does not exist in \"$PATCHDIR\" !!!"
      exit 1
   fi
   TO_PACK="$TO_PACK ./$NAME/$patch"
   echo "    $patch"
done << EOF
`echo "$series" | while read serie; do
   cat "$serie"
done`
EOF
echo

rm -f "$NAME.tar"
tar -cf "$NAME.tar" --exclude '.git' --transform "s|^\./|$NAME/|" -C "$PATCHROOT" $TO_PACK || exit 1
rm -f "$NAME.tar.bz2"
bzip2 -9 "$NAME.tar" || exit 1
echo "Tarball ready!"
echo
