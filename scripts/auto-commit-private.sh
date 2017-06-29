#!/bin/sh

cd /home/andrewdo/.myconfig && echo .myconfig && (git add . && git commit . -m "Nice (auto)")
cd /home/andrewdo/.myconfig-private && echo .myconfig-private && (git add . && git commit . -m "Nice (auto)")
cd /home/andrewdo/.config/frdcsa-private && echo frdcsa-private && (git add . && git commit . -m "Nice (auto)")
