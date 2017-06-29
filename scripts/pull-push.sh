#!/bin/bash

source /var/lib/myfrdcsa/codebases/internal/myfrdcsa/frdcsa-helper.sh

echo "Be sure to stop the FLP first"
continue_on

cd /var/lib/myfrdcsa/codebases/minor && echo minor && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/releases && echo releases && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/minor/fcms/FCMS && echo FCMS && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/internal/freekbs2/data-git/MyKBs && echo MyKBs && (git pull; git push)
# cd /var/lib/myfrdcsas/versions/myfrdcsa-1.1 && echo myfrdcsa-1.1 && (git pull; git push)

if [ "$1" = "extra" ]; then
    cd /$HOME/.myconfig-private && (git pull; git push)
fi

echo "Be sure to restart the FLP now"
continue_on
