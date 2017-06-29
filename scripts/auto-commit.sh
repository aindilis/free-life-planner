#!/bin/bash

cd /var/lib/myfrdcsa/codebases/minor && echo minor && (git add .; git commit . -m "Nice (auto)" .)
cd /var/lib/myfrdcsa/codebases/releases && echo releases && (git add .; git commit . -m "Nice (auto)" .)
cd /var/lib/myfrdcsa/codebases/minor/fcms/FCMS && echo FCMS && (git add .; git commit . -m "Nice (auto)" .)
cd /var/lib/myfrdcsa/codebases/internal/freekbs2/data-git/MyKBs && echo MyKBs && (git add .; git commit . -m "Nice (auto)" .)
# cd /var/lib/myfrdcsas/versions/myfrdcsa-1.1 && echo myfrdcsa-1.1 && (git add .; git commit . -m "Nice (auto)" .)

if [ "$1" = "extra" ]; then
    cd /$HOME/.myconfig-private && (git add .; git commit . -m "Nice (auto)" .)
fi
