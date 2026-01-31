## What's this?
Really dirty test code for YD26's peristaltic pump. Contains Pico code compiled with the Pico SDK and a client compiled for the host, which will communicate with the Pico over serial.
## Dependencies
* GNU Make
* CMake
* Toolchain for your host machine
* Toolchain for `arm-none-eabi`
* lots of spare time

This goes a lot smoother if you're using Linux or WSL. Cygwin and MinGW work but the ARM toolchain isn't prebuilt, so you'll spend another hour figuring out the right build options to compile it from source through trial and error.
## Building
If you plan on running this, you should probably open a screen or tmux window and run `make pico` like, right now. The Pico SDK will download automatically but is huge. Luckily it only needs to do this once unless you remove the `pico-sdk` directory, so... don't do that.
The root Makefile supports two important targets:
* `pico`: builds code for the Pico. Install it by mounting the Pico as a USB device by resetting it/powering it on while holding BOOTSEL, and moving `pico-build/pico-peristaltic-tester.uf2` there. The Pico will dismount automatically and start running the code.
* `client`: builds the host-side client code. It produces a binary at `client/client`.
