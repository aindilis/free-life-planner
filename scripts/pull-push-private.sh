#!/bin/sh

cd /home/andrewdo/.myconfig && echo .myconfig && (git pull && git push)
cd /home/andrewdo/.myconfig-private && echo .myconfig-private && (git pull && git push)
# verify hostname is ai.frdcsa.org
cd /gitroot/frdcsa-private && echo frdcsa-private && (git pull && git push)
