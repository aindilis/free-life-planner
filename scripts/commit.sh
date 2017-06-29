#!/bin/bash

cd /var/lib/myfrdcsa/codebases/minor && (git add .; git commit .)
echo "##################################"
cd /var/lib/myfrdcsa/codebases/releases && (git add .; git commit .)
echo "##################################"
cd /var/lib/myfrdcsa/codebases/minor/fcms/FCMS && (git add .; git commit .)
echo "##################################"
cd /var/lib/myfrdcsas/versions/myfrdcsa-1.1 && (git add .; git commit .)
