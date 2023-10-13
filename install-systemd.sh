#!/bin/bash -eu

sudo cp pi-rfid.service /etc/systemd/system/
sudo touch /var/log/pi-rfid.log
sudo touch /var/log/pi-rfid-error.log
sudo chown pi:pi /var/log/pi-rfid*
sudo systemctl daemon-reload
sudo systemctl start pi-rfid.service
sudo systemctl enable pi-rfid.service

echo "Done"