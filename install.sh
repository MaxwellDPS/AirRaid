#!/bin/bash


if [ "$UID" != "0" ]; then
     echo "You will need to run this script as root."
     exit
fi



echo "Installing AirRaid"
cd src
echo "[*] Installing Wiring pi 2.52 see http://wiringpi.com/wiringpi-updated-to-2-52-for-the-raspberry-pi-4b/"
wget https://project-downloads.drogon.net/wiringpi-latest.deb
dpkg -i wiringpi-latest.deb
echo "[*] Insalling Multimon-ng / rtl-sdr"
apt-get install -qq -y multimon-ng rtl-sdr > /dev/null
mkdir -p /opt/AirRaid
cp run.sh /opt/AirRaid
echo "[*] Placing Symlink for /usr/bin/AirRaid"
ln -s /opt/AirRaid/run.sh /usr/bin/AirRaid
echo "[*] Insalling AirRaid "
gcc airraid.c siren.c netcli.c radio.c -lpthread -lwiringPi -o /opt/AirRaid/AirRaid
echo "[+] Done run with AirRaid"
AirRaid
