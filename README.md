# 2026 Wattown operating software

## Build instructions

Install the
[Pico SDK](https://github.com/raspberrypi/pico-sdk)
and
[picotool](https://github.com/raspberrypi/picotool)
per Raspberry Pi's instructions.


Clone this repository and set it as your working directory.
Make sure PICO_SDK_PATH is set in your environment.

Set up the cmake build environment:

```sh
cmake -S . -B build
```

(Re)build the code:

```sh
cmake --build build --target wattown
```
With the Pico connected over USB, you can automatically reboot the Pico into
BOOTSEL mode, load and run the new code with a single command:

```sh
picotool load build/wattown.uf2 -fvx
```

## Usage instructions

If the Pico is connected over USB, you can connect to it to receive simulation
status messages using a serial console like (tio)[https://github.com/tio/tio].
Note you'll need to determine the correct TTY device to use (although
/dev/ttyACM0 is a good place to start).

```sh
tio /dev/ttyACM0
```
