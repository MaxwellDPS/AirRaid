#!/bin/bash


if [ "$UID" != "0" ]; then
     echo "You will need to run this script as root."
     exit
fi

echo "Installing AirRaid"
cd src
echo "[*] Installing Wiring pi 2.52 see http://wiringpi.com/wiringpi-updated-to-2-52-for-the-raspberry-pi-4b/"
wget -q https://project-downloads.drogon.net/wiringpi-latest.deb
dpkg -i wiringpi-latest.deb
rm wiringpi-latest.deb
echo "[*] Insalling Multimon-ng / rtl-sdr / json-c"
apt-get install -qq -y multimon-ng rtl-sdr libjson-c-dev > /dev/null
mkdir -p /opt/AirRaid
cp run.sh /opt/AirRaid
cp ../config/config.json /opt/AirRaid
echo "[*] Placing Symlink for /usr/bin/AirRaid"
ln -s /opt/AirRaid/run.sh /usr/bin/AirRaid
echo "[*] Insalling AirRaid "
gcc -Wl,--allow-multiple-definition airraid.c siren.c netcli.c radio.c parse.c exec.c -lpthread -ljson-c -lwiringPi -o /opt/AirRaid/AirRaid
echo "[+] Done run with AirRaid"
echo "[+] Default Config at /opt/AirRaid/config.json"
AirRaid
