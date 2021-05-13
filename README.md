## THE GRUB SWITCH ##

#### (c)2019-2021 Ruediger Willenberg

This project enables PC users to select the OS to be booted by *GRUB* through hardware.
It has three independently usable components:

### 1. Configuration scripts ###

A suite of *bash* scripts in **1_config_scripts**, all controlled by one easy-to-use text UI, that provide the following capabilities:
* parse the **grub.cfg** file for all menu entries that represent OS boot options
* enable the user to pick menu entries and order them with numbers (1..15)
* generate *GRUB* script files that make a specific menu entry the default choice; these files can be put on separate USB flash drives; plugging one in will represent that choice of OS to be booted
* enhance the existing **grub.cfg** - in a non-destructive way that survives kernel updates - to look for a boot choice script on a plugged-in drive
* install a list of file hashes in *GRUB* so that only the generated files will be executed

When using several USB flash drives for the boot choices (small, old ones are
fine; only kBytes of capacity are required), no further hardware is necessary.

*GRUB* does not need to be patched, rebuilt or reinstalled. The employed extension of **grub.cfg** is robust against unavailable or malformed data. Any unexpected choices or scripts will just lead to the regular, unchanged *GRUB* boot menu.

### 2. USB device firmware

The **2_usb_device** directory holds firmware source code for *Microchip/AVR ATmega32U4* MCUs that can act as USB devices. It has build support and pre-built firmware images for common maker boards like *Teensy 2.0*, *Arduino Micro*, *Adafruit ItsyBitsy* and *Sparkfun Pro Micro*. Other boards with the ATmega32U4 can be easily added.

A board with the firmware installed will register as a regular USB flash drive when plugged in (albeit one with a tiny 33kByte *FAT12* partition). It will produce a script file SWITCH.GRB that corresponds to one of the
generated script files for the regular flash drives; however, you can control
which boot choice is taken through pin connections on the MCU.

The firmware does NOT need to be rebuilt to change the boot configuration; a new boot configuration can just be written to the USB drive. Through a pin or jumper, *write-protect* can be activated.

Boards can be connected to common *1-of-n-*, *on/off/on-* or *binary-encoding* switches to realize boot choice.

### 3. Custom USB Hardware 

For easier panel and case-internal mounting (and for fun), we designed our own *ATmega32U4* PCB. *EAGLE* and *Gerber* files and all other required information can be found in **3_custom_hardware**. It has the following features:

* Solder pads for common *1-of-12-rotary encoder* and *on/off/on-toggle* switches with front panel mounting threads; no soldering cables required.
* Supports USB connection both by *Mini-B*-plug or directly by internal ribbon cable to motherboard USB pinheaders.
* Firmware easily re-programmable by pushing a button; works with factory *ATmega32U4* chips, no custom bootloader needs to be installed.
* Switch- or jumper-controlled *write-protect*
* Can be used with self-written firmware for any other purposes, like any maker board; provides up to 13 I/O-pins at 5V-level.

Obviously this requires soldering skills and is likely not the cheapest hardware option in comparison to the mass market boards listed above, especially with the required custom PCB order. But fun!

---

#### See [*quick_start.pdf*](./quick_start.pdf) for the fastest way to your custom boot configuration.

The [*documentation*](./documentation/) directory holds more detailed information.

Send any questions or suggestions to [*grubswitch@gmail.com*](mailto:grubswitch@gmail.com). Please be patient. I have a life.
