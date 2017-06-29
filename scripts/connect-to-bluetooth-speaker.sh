#!/bin/sh

bt-device -l

(blueman-assistant &)
sleep 4
for it in `seq 1 3`; do
    xdotool windowfocus 60817416
    xdotool key Return
    xdotool key Return
done

echo "scan on\npair $1\nconnect $1" | bluetoothctl
