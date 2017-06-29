#!/bin/bash

source /var/lib/myfrdcsa/codebases/internal/myfrdcsa/frdcsa-helper.sh

echo "Be sure to stop the FLP first"
continue_on

/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/pull-push.sh

cd /var/lib/myfrdcsa/sandbox && echo sandbox && (git pull; git push)
cd /var/lib/myfrdcsa/collections && echo collections && (git pull; git push)
cd /var/lib/myfrdcsa/capabilities && echo capabilities && (git pull; git push)
cd /var/lib/myfrdcsa/private && echo private && (git pull; git push)
cd /var/lib/myfrdcsa/codebases/cats && echo cats && (git pull; git push)

/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/pull-push-rest.sh

echo "Be sure to restart the FLP now"
continue_on
