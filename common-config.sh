# No interpreter so people not try to exec it
WORKDIR="$(pwd)/obsclone"                                                      
DEVELPROJECT="server:database"                                                 
DEVELPKGS=(                                                                    
    "mariadb"                                                                  
    "mariadb-100"                                                              
    "mariadb-101"                                                              
    "mysql-community-server"
    "mysql-community-server-56"                                                
    "mysql-community-server-57"                                                
)
# These MUST work
SUPPORTED_PLATFORMS=(                                                          
    "openSUSE_Factory"
    "openSUSE_Leap_42.1"
)
# These might work and it is up to packager
# to decide if something needs fixing
MAYBE_WORKING_PLATFORMS=(
    "SLE_12"
)

# Color definition
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m' # no color
