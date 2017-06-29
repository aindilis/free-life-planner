#!/bin/sh

for it in `seq 1 10`; do
    mplayer -quiet -ao pulse::bluez_sink.<BLUETOOTHADDRESS> /var/lib/myfrdcsa/codebases/minor/alexa-integration/data-git/system-sounds/alexa-alarm-delayed.mp3
done
