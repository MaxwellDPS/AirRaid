# AirRaid
## Raspberry Pi Civil Defense Siren
Proof of Concept Code to Build a Hackable Realistic Civil Defense Siren. To Demonstarate the vulerability in Millions of Real Civil Defense Sirens that work the same across the country 

[![N|Solid](https://static.wixstatic.com/media/632ed1_fb9a3b5268b3477caea3a865bef54a7c.png/v1/fill/w_119,h_67,al_c,usm_0.50_1.20_0.00/632ed1_fb9a3b5268b3477caea3a865bef54a7c.png)](https://maxwelldps.com)

 Raspberry Pi Civil Defense Siren is designed to emulate Federal Signal Systems DTMF ONLY for Now.

### Hardware Needed
  - SDR
  - Adafruit DRV8871 Brushed DC Motor Driver Breakout - https://learn.adafruit.com/adafruit-drv8871-brushed-dc-motor-driver-breakout/overview 
  - Raspberry Pi Model 1, 2, 3
  - AirRaid Siren - https://www.amazon.com/JC-Performance-Powder-Coated-Electric/dp/B01FV5ALI4/ref=sr_1_6?ie=UTF8&qid=1502299372&sr=8-6&keywords=air+raid+siren
### Software Needed
  - RTL_SDR
  - Multimon-ng
  - wiringPi

### Development

Want to contribute? Great!

Decoding Code Needed For:
- Two-tone sequential
- EAS
- POCSAG
- Digital AFSK 

### Tech

This uses a number of open source projects to work properly:

* [RTL_SDR](https://github.com/keenerd/rtl-sdr) - turns your Realtek RTL2832 based DVB dongle into a SDR receiver.
* [MULTIMON-NG](https://github.com/EliasOenal/multimon-ng) - multimon-ng a fork of multimon. It decodes digital transmission modes.
* [WiringPi](https://github.com/WiringPi/WiringPi) - Gordon's Arduino wiring-like WiringPi Library for the Raspberry Pi (Unofficial Mirror for WiringPi bindings).

And of course Raspberry Pi Civil Defense Siren itself is open source with a Public Repo on GitHub.

### Installation of Needed Software

#### Building From Source
```
sudo apt-get update
sudo apt-get upgrade -y
sudo apt-get dist-upgrade -y
sudo apt-get -y install git gcc cmake libusb-1.0-0-dev libpulse-dev libx11-dev screen qt5-default libtool autoconf automake libfftw3-dev

mkdir ~/src

echo "Installing rtl_sdr"
cd ~/src
git clone git://git.osmocom.org/rtl-sdr.git
cd ~/src/rtl-sdr
mkdir build
cd build
cmake ../ -DINSTALL_UDEV_RULES=ON -DDETACH_KERNEL_DRIVER=ON
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

echo "Installing WiringPi"
sudo apt-get purge wiringPi -y
hash -r
cd ~/src
git clone git://git.drogon.net/wiringPi
cd wiringPi
git pull origin
./build

cd ~/src
git clone https://github.com/MaxwellDPS/AirRaid.git
cd AirRaid
gcc -lpthread -lwiringPi -o AirRaid siren.c
sudo mkdir /opt/rpiSiren/
sudo mv AirRaid /opt/rpiSiren/AirRaid
sudo chmod +x /opt/rpiSiren/AirRaid
sudo ln -s /opt/rpiSiren/AirRaid /usr/local/bin
```

#### Running
```
$ sudo AirRaid
```

NOTE: IF program wont run try
```
$ sudo killall -9 rtl_fm
```

#### Using
1. Tune Tranceiver to the Listen Frequency  DEFAULT: 147.48MHZ 
2. Transmit DTMF Tones as set  in siren.c 

```
Default DTMF:
    TONES:     5
    TIMEOUT:   5 SEC
    OFF:       42061
    GROWL:     42062
    ALERT:     42063
    ATTACK:    42064
```

License
----

GNU GPL 3.0


