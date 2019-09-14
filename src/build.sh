#!/bin/bash
#sudo gcc siren.c -o /opt/AirRaid/AirRaid -lpthread -lwiringPi
cd src
sudo gcc airraid.c netcli.c radio.c siren.c -lwiringPi -lpthread -o /opt/AirRaid/AirRaid
