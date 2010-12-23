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

if [ -z "$1" ]; then
   echo "Usage:"
   echo "   ./quilt-setup.sh series"
   exit 3
fi

[ "$PATCROOT" ] || PATCHROOT="`echo "$0" | sed 's|quilt-setup.sh|..|'`"

pushd . > /dev/null
cd "$PATCHROOT"
PATCHROOT="`pwd`"
NAME="`ls -1d *-patches`"
PATCHDIR="$PATCHROOT/$NAME"
popd > /dev/null

[ -e patches ] && rm patches
[ -e patches ] && rmdir patches
if [ -e patches ]; then
   echo "ERROR: Can't delete existing patches directory!"
   exit 4
fi

ln -s "$PATCHDIR" patches
echo " * Patches linked."
rm -f series

echo " * Creating series file..."
while read patch; do
   patch="`echo "$patch" | sed -e 's|^[[:blank:]]*||' -e 's|[[:blank:]]*#.*||'`"
   [ -z "$patch" ] && continue
   [ \! -f "$PATCHDIR/$patch" ] && [ -f "$PATCHDIR/$patch.patch" ] && patch="$patch.patch"
   if [ \! -f "$PATCHDIR/$patch" ]; then
      echo " ! Patch \"$patch\" does not exist in \"$PATCHDIR\" !!!"
      exit 1
   fi
   p="`head -n1 "$PATCHDIR/$patch" | sed -n 's/^PATCH-P\([0-9]\+\)-.*/\1/p'`"
   if [ \! "$p" ]; then
      echo " ! Patch \"$patch\" doesn't have a correct tag !!!"
      exit 2
   fi
   echo "$patch" -p$p >> series
   echo "   - Patch "$patch" added to series."
done < "$1"
echo " * Everything ready."
