#!/bin/sh

echo "127.0.0.1   server" >> /etc/hosts
mosquitto -c /etc/mosquitto/mosquitto.conf -d

python3 /running/script.py &
python3 /running/simulate.py &

tail -f /dev/null
