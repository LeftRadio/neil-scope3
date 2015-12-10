# What is NeilScope3 ?
NeilScope3 is project of portable digital storage oscilloscope, the basic characteristics:

* Bandwidth analog         -  20 MHz
* ADC                      -  AD9288-100 (2ch, 8 bit, 100Msps)
* Maximum sample rate      -  200Msps(1 channel), 100Msps(2 channel)
* Minimum sensitivity      -  50 V/div
* Maximum sensitivity      -  10 mV/div
* Inputs limiting voltage  -  100V (depend used components)
* Memory per channel       -  256kB
* Logic Analyzer mode      -  Yes
* Logic Analyzer inputs    -  8 bits
* LA data compression      -  None / RLE (in EPM570)
* LCD                      -  HX8352A based, 8/16 bit interface
* Interfaceses             -  USB 2.0 FullSpeed, WiFi(ESP8266)


![Dev](http://hobby-research.at.ua/NS3/photo/razr_res.png)


If you need more information, please follow these links:
* [Home page](http://hobby-research.at.ua/publ/razrabotki/izmerenija/neil_scope_3/4-1-0-42).
* [Forum](http://hobby-research.at.ua/forum/2-1-1).


## Build

1. Install [Python3](https://www.python.org/downloads/)
2. Install [gcc toolchain](https://launchpad.net/gcc-arm-embedded/+download).
3. Download and unpack this repository

4. From unpacked repository put in console:

>> python ns3_build.py - h

5. Run build for you LCD hardware, for example:

>> python ns3_build.py --lcd-bits __LCD_16_BIT__ --lcd-bus __LCD_DIRECT__


*You can also build NeilScope3 with CoIDE, EM::Blocks, Eclipse+ARM toolchain etc.


## Download NeilScope3 Flash



