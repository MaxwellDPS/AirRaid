#!/bin/bash

cd src
sudo gcc airraid.c netcli.c radio.c siren.c parse.c exec.c -ljson-c -lwiringPi -lpthread -o /opt/AirRaid/AirRaid
