#!/bin/bash

/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/auto-commit.sh

cd /var/lib/myfrdcsa/sandbox && echo sandbox && (git add .; git commit . -m "Nice (auto)" .)
cd /var/lib/myfrdcsa/collections && echo collections && (git add .; git commit . -m "Nice (auto)" .)
cd /var/lib/myfrdcsa/capabilities && echo capabilities && (git add .; git commit . -m "Nice (auto)" .)
cd /var/lib/myfrdcsa/private && echo private && (git add .; git commit . -m "Nice (auto)" .)
cd /var/lib/myfrdcsa/codebases/cats && echo cats && (git add .; git commit . -m "Nice (auto)" .)

/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/auto-commit-rest.sh

# # FIXME add
# /var/lib/myfrdcsa/codebases/data once get a origin repo
# /var/lib/myfrdcsa/codebases/external
# /var/lib/myfrdcsa/codebases/minor-data
# /var/lib/myfrdcsa/codebases/non-free
# /var/lib/myfrdcsa/codebases/work
# /var/lib/myfrdcsa/codebases/work-data
