THE GRUB SWITCH
(c)2019 Ruediger Willenberg

This project enables PC users to select the OS to be booted by GRUB through hardware.
It has two major components:

1) grub-switch-configure (not added yet)

grub-switch-configure is a command-line tool with text UI for Linux that
* parses the GRUB.CFG file for menu entries that represent OS boot options
* allows the user to associate menu entries with a single digit number (1..0xF)
* generates GRUB script files that make a specific menu entry the default choice;
  these files can be put on separate USB flash drives; plugging one in will
  represent that choice of OS to be booted
* enhance the existing GRUB.CFG in a non-destructive way that survives kernel
  updates to look for a boot choice script on a plugged-in drive

When using several USB flash drives for the boot choices (small, old ones are
fine; only kBytes of capacity required), no further hardware is necessary.

2) usb_device (fully functional for the Teensy 2.0 board)

usb_device is a firmware for Microchip/AVR ATmega MCUs that can act as USB
devices (currently ATmega32U4 and AT90USB128x are supported).

A supported board with the firmware installed will register as a regular USB
flash drive when plugged in (albeit one with a tiny 33kByte FAT12 partition).

It will produce a script file SWITCH.GRB that corresponds to one of the
generated script files for the regular flash drives; however, you can control
which script (and therefore which boot choice) through pin connections on the
MCU. In practice, a 1-of-n switch or a binary encoder switch can be used to
pick the OS you want to boot.

Instructions will be added here on how to use the board/firmware without the
command-line tool.
