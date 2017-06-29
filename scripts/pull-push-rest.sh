#!/bin/bash

source /var/lib/myfrdcsa/codebases/internal/myfrdcsa/frdcsa-helper.sh

echo "Be sure to stop the FLP first"
continue_on

# # FIXME add
# /var/lib/myfrdcsa/codebases/data once get a origin repo

# cd /var/lib/myfrdcsa/codebases/data && echo data && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/external && echo external && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/minor-data && echo minor-data && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/non-free && echo non-free && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/work && echo work && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/work-data && echo work-data && (git pull; git push)

echo "Be sure to restart the FLP now"
continue_on
