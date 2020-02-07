# AirRaid
## Raspberry Pi Civil Defense Siren
Proof of Concept Code to Build a Hackable Realistic Civil Defense Siren. To Demonstarate the vulerability in Millions of Real Civil Defense Sirens that work the same across the country 

[![N|Solid](https://static.wixstatic.com/media/632ed1_fb9a3b5268b3477caea3a865bef54a7c.png/v1/fill/w_119,h_67,al_c,usm_0.50_1.20_0.00/632ed1_fb9a3b5268b3477caea3a865bef54a7c.png)](https://maxwelldps.com)

 Raspberry Pi Civil Defense Siren is designed to emulate Federal Signal Systems DTMF ONLY for Now.

### Hardware Needed
  - SDR (If you want to use radio Activaion)
  - Adafruit DRV8871 Brushed DC Motor Driver Breakout (PWM Motor driver) - https://learn.adafruit.com/adafruit-drv8871-brushed-dc-motor-driver-breakout/overview 
  - Raspberry Pi Model 1, 2, 3, 4
  - AirRaid Siren ( or What you want to drive) - https://www.amazon.com/JC-Performance-Powder-Coated-Electric/dp/B01FV5ALI4/ref=sr_1_6?ie=UTF8&qid=1502299372&sr=8-6&keywords=air+raid+siren
### Software Needed
  - RTL-SDR
  - Multimon-ng
  - wiringPi 2.52 Thank you @drogon Wiring Pi Will Be missed

### Development

#### To do
> Replace Wiring Pi for PWM :(

Decoding Code Needed For:
- Two-tone sequential
- EAS
- POCSAG
- Digital AFSK 

#### Want to contribute? Great!
Send a Pull request

### Tech

This uses a number of open source projects to work properly:

> Thank you to all the projects that help make this possable

* [RTL_SDR](https://github.com/keenerd/rtl-sdr) - turns your Realtek RTL2832 based DVB dongle into a SDR receiver.
* [MULTIMON-NG](https://github.com/EliasOenal/multimon-ng) - multimon-ng a fork of multimon. It decodes digital transmission modes.
* [WiringPi](http://wiringpi.com/) - WiringPi Library for the Raspberry Pi.

### Installation of Needed Software

#### Building and installing

This will install RTL-SDR & multimon-ng from apt and install Wiring Pi via deb then build and install AirRaid to /opt/AirRaid
```
git clone https://github.com/MaxwellDPS/AirRaid.git
cd AirRaid
sudo ./install.sh
```

#### Running
```
$ AirRaid
```


#### Using
```
----------------------------------------------------------
AirRaid PWM RPI Hackable Civil Defense Siren Controler
----------------------------------------------------------
-c, --cli for Command line Control
-d, --daemon To daemonize MUST BE USED with -r OR  -n (Defaults to --network)
-n, --network for UDP Control
-r, --radio to use RTL-SDR for DTMF
-f, --freq Set listen Frequency in HZ Default: 147480000 HZ

```

##### Daemon
Dameon logs to file at /var/log/AirRaid.log

To view live:
```
$ tail -f /var/log/AirRaid.log
```

##### Network (UDP)
Send the following numbers in ascii to port 21234 via UDP

```
1 - Growl
2 - ALERT
3 - ATTACK
4 - OFF
99 - EXIT Useful for -d 
```

##### Radio
1. Install RTL-SDR in RPI
2. Tune Tranceiver to the Listen Frequency  DEFAULT: 147.48MHZ 
3. Transmit DTMF Tones as set in siren.h

> Tones can be changed in src/siren.h after install by editing src/siren.h and running src/build.sh from the AirRaid Directory

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
