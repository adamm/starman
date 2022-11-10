# BUILDING

This doc describes building the source code, not building your own Starman :)
Except for Step 1, the instructions here are not specific to Starman and can be
used for any "esp-idf"-based project.

> While the Starman hardware is Arduino-compabile, the codebase is not written
> using the Arduino toolchain.  If you want to use the hardware as an arbitrary
> Arduino device, feel free, but to access the lights and sound you'll have to
> write your own port :)

## 1. Solder 2x3 headers onto J5

The back module pops off, but it's pretty stiff.  Take care not to damage any
of the components.  This will allow you access to the back to solder on the
headers, if they're not there already.

## 2. Get an [ESP-Prog](https://espressif-docs.readthedocs-hosted.com/projects/espressif-esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html) and cables to connect to J5

Be sure to match up pins with the ESP-Prog's 2x3
[communication interface](https://espressif-docs.readthedocs-hosted.com/projects/espressif-esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html#communication-interface).
A ribbon cable [like this one](https://www.amazon.ca/CablesOnline-12-inch-2-54mm-Pitch-Female-FR-0612/dp/B078LM31J9)
is most convenient.

Alternatively, you can get away with not using an ESP-Prog and instead use any
FTDI USB->Serial adapter, [like this one](https://www.sparkfun.com/products/9717),
just be sure it's 3.3V logic level.  Also, you won't have convenient access to
trigger the boot and reset pins automatically.  You'll have to manually touch
those to ground to reset into bootloader mode and upload new code.

Be sure to only connect your cable to the TX, RX, and Gnd pins on J5.  The
transmit/receive is relative to the Starman MCU, so you may need to
connect the TX pin to your cable's RX port, and vice versa.

## 3. [Download and install the FDTI driver](https://ftdichip.com/drivers/vcp-drivers/)

If you have an ESP-Prog or other FTDI USB->Serial device or cable, download
and install the FTDI drivers [here](https://ftdichip.com/drivers/vcp-drivers/).
If you have a non-FTDI USB->Serial device, you'll need to follow its instructions.

## 4. [Download and install the esp-idf toolchain](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation)

## 5. Clone this repository

The codebase lives in the main/ directory, but be sure to run all the
following commands in the root of the cloned repository.

## 6. Build, Flash, Monitor the project

If you're using an IDE with the [Eclipse plugin](https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md)
or [VSCode extension](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md),
follow those instructions.  If you using the command-line follow these steps:

1. `idf.py set-target esp32s3` to prepare the build environment.  (You only need to do this once.)

2. `idf.py build` to compile the project.

3. `idf.py flash` to actually upload the compiled binary.

To upload your code you need to be in bootloader mode.  The ESP-Prog will
take care of this for you, but if you used your own cable or FTDI device,
you'll have to manually enter bootloader mode.  To do this, connect J5
"Boot" pin to "Gnd", press the Reset button, then disconnect the "Boot" pin.
Run `idf.py flash` normally.  After flashing is complete you need to press
"Reset" again for normal operations.

4. `idf.py monitor` to open up a serial port to monitor the logs of the device.

You can combine all three steps into one command `idf.py build flash monitor`
for convenience.