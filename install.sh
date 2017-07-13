#!/bin/sh

# install :
# curl -o installer.sh https://raw.githubusercontent.com/MaxwellDPS/AirRaid/master/install.sh && chmod +x installer.sh && sudo ./installer.sh

sudo apt-get update
sudo apt-get upgrade -y
sudo apt-get dist-upgrade -y
sudo apt-get -y install git cmake libusb-1.0-0-dev libpulse-dev libx11-dev screen qt4-qmake libtool autoconf automake libfftw3-dev

mkdir ~/src

echo "Installing rtl_sdr"
cd ~/src
git clone git://git.osmocom.org/rtl-sdr.git
cd ~/src/rtl-sdr
mkdir build
cd build
cmake ../ -DINSTALL_UDEV_RULES=ON
make
sudo make install
sudo ldconfig

echo "Installing multimon-ng"
cd ~/src
git clone https://github.com/EliasOenal/multimonNG.git
cd ~/src/multimonNG
mkdir build
cd build
qmake ../multimon-ng.pro
make
sudo make install

echo "Installing Kalibrate"
cd ~/src
git clone https://github.com/asdil12/kalibrate-rtl.git
cd kalibrate-rtl
git checkout arm_memory
./bootstrap
./configure
make
sudo make install

echo "Installing WiringPi"
sudo apt-get purge wiringPi -y
hash -r
cd ~/src
git clone git://git.drogon.net/wiringPi
cd ~/wiringPi
git pull origin
cd ~/wiringPi
./build

echo "INSTALLING AIRRAID..."
cd ~/src
git clone https://github.com/MaxwellDPS/AirRaid.git
cd AirRaid
gcc -lpthread -lwiringPi -o rpiSiren siren.c
sudo mkdir /opt/rpiSiren/
sudo mv rpiSiren /opt/rpiSiren/rpiSiren
sudo chmod +x /opt/rpiSiren/rpiSiren
sudo ln -s /opt/rpiSiren/rpiSiren /usr/local/bin
