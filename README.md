# ESP32-Faikin

Everyone knows Daikin make some of the best air conditioners out there, mechanically speaking. Sadly their WiFi control modules are not so good, especially the latest models which are all cloud based, require an internet connection to even work, and are slow.

[<img src="https://github.com/user-attachments/assets/338f32f4-08dc-4355-88a8-cc47ec79a074" width=33% alt="Faikin">](https://www.youtube.com/watch?v=2telNgGL2u8 "Faikin")
<img src="https://github.com/user-attachments/assets/81cdd6ae-362e-4a48-8efd-c60beae9d125" width=33% align="right" Alt="Optional cable">

This code/module provides local control via web interface, MQTT, and HomeAssistant integration, all with no cloud crap.

<img src=https://github.com/user-attachments/assets/110f49e2-de9f-41c9-82de-0c65e32430b9 width=33% align=right>

* There is also a new [Faikin Remote Control](https://remote.revk.uk/) available, BLE linked to the Faikin, with environmental sensors, available on Tindie now.

PCB designs are included, and also available to buy on [Tindie](https://www.tindie.com/stores/revk/), and on [Amazon UK](https://www.amazon.co.uk/dp/B0C2ZYXNYQ). There are other people making and selling, e.g. on [eBay Australia](https://www.ebay.com.au/itm/186860658654).

I'd like to thank the contributors, but contributions are made and accepted on basis that you issue your contribution under the same GPL licence as the project. Forks are allowed on the basis that your forks are on the same GPL licence as the project.

## Trademark

This is an open source project, but bear in mind you cannot sell boards bearing the Andrews & Arnold Ltd name, the A&A logo, the registered trademark AJK logo, or the GS1 allocated EANs assigned to Andrews & Arnold Ltd.

## Further manuals / links

- [Work in progress / release notes](https://github.com/revk/ESP32-Faikin/wiki/Work-in-progress)
- [Wiring](https://github.com/revk/ESP32-Faikin/wiki/Wiring)
- [Setup](Manuals/Setup.md) Manual
- [Controls](Manuals/Controls.md) Manual
- [Advanced](Manuals/Advanced.md) Manual
- [ESP8266 port](https://github.com/Sonic-Amiga/ESP8266-Faikin) of this code
- [list of supported air-con](https://github.com/revk/ESP32-Faikin/wiki/List-of-confirmed-working-air-con-units) WiKi
- [DoC](Manuals/DoC.md) and Vulnerability disclosure policy
- [S21](Manuals/S21.md) reverse engineered details of `S21` protocol

For support, see issues and discussions.

## PCB

The boards from [Tindie](https://www.tindie.com/stores/revk/), [eBay](https://www.ebay.com.au/itm/186860658654), and [Amazon UK](https://www.amazon.co.uk/dp/B0C2ZYXNYQ) works with Daikin modules via an `S21` connector, or `X50A` and `X35A` connectors, or similar. The design is updated from time to time so may not look exactly like this.

<img src=PCB/Faikin/Faikin.png width=49%><img src=PCB/Faikin/Faikin-bottom.png width=49%>

Supplied in a 70x70 panel as an assembled PCB with snap off parts down to two sizes. 45x36mm or 40x16mm.

<img src=PCB/Faikin/Faikin-panel.png width=49%><img src=PCB/Faikin/Faikin-alt-bottom.png width=49%>

## Why I made this

The history is that, after years of using Daikin air-con in my old home, and using the local http control, in my new house in Wales the WiFi was all cloud based with no local control, and useless, and slow. Just configuring it was a nightmare. I spent all day reverse engineering it and making a new module to provide local control. Pull requests and feature ideas welcome.

This whole project is almost entirely by me, but with some valuable contributions from others (thank you). All of my bits are copyright by me and Andrews & Arnold Ltd who sponsor the whole project, and released under GPL. Whilst not required by the licence, attribution and links would be appreciated if you reuse this.

## How to get one

As mentioned, [Tindie](https://www.tindie.com/stores/revk/), [eBay](https://www.ebay.com.au/itm/186860658654), and [Amazon UK](https://www.amazon.co.uk/dp/B0C2ZYXNYQ).

But also, the PCB designs are published, including production files for [JLCPCB](https://jlcpcb.com). You need to remove trademarks, etc, and you would also need something to program them (TC2030 lead). Obviously you need some experience with PCB ordering and diagnostics if getting your own boards made.

# Set-up

Appears as access point with simple web page to set up on local WiFI. On iPhone the setup page auto-loads.

![WiFi1](Manuals/WiFi1.png)

![WiFi2](Manuals/WiFi2.png)

# Operation

Local interactive web control page using *hostname*.local, no app required, no external internet required.

![WiFi3](Manuals/WiFi3.png)

- [Setup](Manuals/Setup.md) Manual
- [Controls](Manuals/Controls.md) Manual
- [Advanced](Manuals/Advanced.md) Manual

# Design

* KiCad PCB designs included, with JLCPCB production files
* 3D printed case STL files
* Documentation of reverse engineered protocol included

Basically, Daikin have gone all cloudy with the latest WiFi controllers. This module is designed to provide an alternative.

<img src="Manuals/MiSensor.jpg" align=right width="25%">

* Simple local web based control with live websocket status, easy to save as desktop icon on a mobile phone
* MQTT reporting and controls
* Works with Home Assistant over MQTT - note Home Assistant can work with HomeKit
* Includes linux mysql/mariadb based logging and graphing tools
* Works with [EnvMon](https://github.com/revk/ESP32-EnvMon) Environmental Monitor for finer control and status display
* or, works with BlueCoinT and Telink [BLE temperature sensor](Manuals/BLE.md) as a remote reference in an auto mode
* Automatically works out if S21 or X50 protocol (used on bigger/ducted units)
* Backwards compatible direct `/aircon/...` URLs

# Building code yourself

Git clone this `--recursive` to get all the submodules. Build commands such as `make` or `idf.py` should be run inside the `ESP` directory. There are make targets for other variations, but this hardware is the `make pico`, `make s2` (ESP32-S2) or `make s3` (ESP32-S3) version. The `make s3` target now builds for the 4 MB flash *without* PSRAM. If you have a module with PSRAM fitted use `make s3r2`. The `make` command actually runs the normal `idf.py` to build which then uses cmake. `make menuconfig` can be used to fine tune the settings, but the defaults should be mostly sane. `make flash` should work to program. If flashing yourself, you will need a programming lead, e.g. [Tazmotizer](https://github.com/revk/Shelly-Tasmotizer-PCB) or similar, and of course the full ESP IDF environment. The latest boards also have 4 pads for direct USB connection to flash with no adaptor. The modules on Amazon come pre-loaded and can upgrade over the air.

If you forget to clone with `--recursive`, run `python setup_submodules.py` (or
`setup_submodules.bat` on Windows) in the repository root to fetch all the
required libraries.

When building on Windows you also need a POSIX environment like MSYS2 or
Cygwin installed. Ensure the `gcc` compiler from that environment is available
in your `PATH` so `build.bat` can compile the helper tools.  The helper tool
`revk_settings` also depends on the [libpopt](https://github.com/rpm-software-management/popt)
library.  Install the corresponding `popt` development package (e.g.
`pacman -S mingw-w64-x86_64-popt` on MSYS2 or `libpopt-dev` on Debian
derivatives) so `popt.h` is available during compilation.  A minimal
`err.h` shim is bundled under `ESP/include` so building does not require
the BSD err library.

Before invoking `idf.py` directly, run `python generate_settings.py` in the
repository root (or `make settings.h` on platforms with `csh` installed).  This
script fetches all submodules, builds the `revk_settings` helper tool, and
creates the required `main/settings.c` and `main/settings.h` files. Without
these files CMake will fail during `idf.py set-target`.

The code is normally set up to automatically upgrade the software, checking roughtly once a week. You can change this in settings via MQTT.

If you build yourself, you either need no code signing, or your own signing key. This will break auto-updates which try to load my code releases, so you need to adjuist settings `otahost` and `otaauto` accordingly. You can set these in the build config, along with WiFi settings, etc.

If you want to purchase a pre-loaded assembled PCB, see [Tindie](https://www.tindie.com/stores/revk/), and [Amazon UK](https://www.amazon.co.uk/dp/B0C2ZYXNYQ).

## Extended HTTP API

The firmware includes a set of stub handlers matching the Daikin HTTP modules.
These endpoints store the most recent parameters so that third party clients
continue to operate while hardware integration is completed.

```
/common/get_remote_method
/common/set_remote_method
/aircon/get_timer
/aircon/set_timer
/aircon/get_price
/aircon/set_price
/aircon/get_target
/aircon/set_target
/aircon/get_program
/aircon/set_program
/aircon/get_scdltimer
/aircon/set_scdltimer
/common/get_notify
/common/set_notify
/common/set_regioncode
/common/set_led
/common/reboot
/aircon/get_year_power
/aircon/get_week_power
```

Hardware support now covers the majority of these handlers.  Timer,
program and schedule timer requests are forwarded via S21 commands and
the notify, remote method, region and power history APIs query the
unit before replying.  Only the pricing endpoints remain local-only.

## Flashing code

You will need to connect a suitable programming lead. Boards have a header for USB. The very latest design (expected on Amazon around Sep 2024) has a tag-connect compatible header for a [TC2030-USB-NL](https://www.tag-connect.com/product/tc2030-usb-nl) lead.

See [https://github.com/revk/ESP32-RevK](https://github.com/revk/ESP32-RevK) for more details of how to flash the files easily.
