# DALI-Matter-Gateway

Copyright (C) 2026 Malte Rudolf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

## About

This project controls groups of DALI luminaires or individual DALI luminaires
through Matter using a Waveshare ESP32-S3-Pico and the Waveshare DALI2 Expansion
Module. This makes the DALI installation available to Matter platforms such as
Google Home, Amazon Alexa, and Apple HomeKit.

The project was created because existing Matter solutions commonly exposed only
one DALI group, one luminaire, or all luminaires through a single Matter device.
This gateway is designed to expose multiple groups and individual luminaires
independently.

## Hardware

- [Waveshare ESP32-S3-Pico](https://www.waveshare.com/esp32-s3-pico.htm)
- [Waveshare Pico-DALI2 Expansion Module](https://www.waveshare.com/pico-dali2.htm)

## Software

- Visual Studio Code
- pioarduino IDE
- Espressif32 Platform for PlatformIO (pioarduino fork)

## Features

- Control DALI lights through Matter
- Support for multiple Matter dimmable lights
- DALI device and group control
- Reading DALI light levels
- Logging of DALI traffic and gateway events in the web interface
- Web interface for status and control
- OTA firmware updates

## Tested

- Controlling groups of DALI luminaires through Google Home

## Configuration

The following files contain project-specific configuration and should not be committed to Git:

- `src/secrets.cpp` — Wi-Fi credentials
- `src/light_definitions.cpp` — local light configuration
- `include/timezone.h` — timezone for log timestamps

These files are listed in `.gitignore`.

### Timezone

The timezone is configured in `include/timezone.h` using a POSIX timezone string.
The default setting is Central European Time with automatic daylight saving time.
Available timezone names and POSIX string examples can be found in the
[IANA Time Zone Database](https://www.iana.org/time-zones) and the
[ESP-IDF time documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html#daylight-saving-time).

### Light definitions

The DALI lights are configured in `src/light_definitions.cpp`. The four arrays use
the same index, so `lightNames[i]`, `lightIsGroup[i]`, `lightSetIds[i]`, and
`lightStatusIds[i]` always describe the same light in the web interface.

- `lightNames` defines the name shown in the web interface.
- `lightIsGroup` selects whether the DALI target is a group (`true`) or a device
	(`false`).
- `lightSetIds` contains the DALI group ID when `lightIsGroup` is `true`. For a
	device it contains the DALI short address of that device.
- `lightStatusIds` contains the DALI short address of a device whose status is read.
	For a group, enter the short address of any device belonging to that group. The
	status is always read from a device, never from the group itself.

Example:

```cpp
const char *lightNames[] = {
	"Living room group",
	"Hallway device"
};

const bool lightIsGroup[] = {
	true,
	false
};

const uint8_t lightSetIds[] = {
	1,  // DALI group 1
	4   // DALI device short address 4
};

const uint8_t lightStatusIds[] = {
	17, // device short address in group 1
	4   // device short address 4
};
```

## Building

Open the project in PlatformIO and build the project using the configured environment.

The firmware can be uploaded via USB or OTA.

### Versioning

The current product version is stored in `include/version.h` and follows semantic versioning.
The version and copyright notice are shown in the web interface and in the `/status`
JSON response.

Development work should be done on a `development` branch and merged into `main` when
it is ready. To publish a version, update `APP_VERSION` in `include/version.h`, merge it into `main`, and create
and push a matching tag:

```bash
git add include/version.h
git commit -m "Prepare release v0.1.0"
git push
git tag v0.1.0
git push origin v0.1.0
```

The tag starts a GitHub Actions workflow that creates a GitHub Release containing only
the source archive. No PlatformIO build runs on GitHub.

## Open Issues

The Matter QR code is  still generated from the example configuration.

## Screenshots

![web interface](docs/images/webinterface.png)

## License

This project is licensed under the GNU General Public License version 3 or any later version (GPL-3.0-or-later).

See the `LICENSE` file for the complete license terms.

The software may be used, modified and distributed for both private and commercial purposes, subject to the conditions of the GNU General Public License.

## Third-Party Components

### DALI library

The project contains DALI library code originally developed by **qqqlab**.

Copyright © qqqlab

The original copyright and GPLv3+ license notices in the DALI library files have been retained.

The DALI library was obtained from the Waveshare DALI2 hardware/software package.

The applicable copyright and license conditions of the third-party software remain in effect.

## Disclaimer

This software is provided "AS IS", without warranty of any kind, express or implied.

The author provides no guarantee that the software is free of defects or suitable for any particular purpose.

To the extent permitted by applicable law, the author shall not be liable for any damage, data loss, hardware damage, financial loss, or other consequences resulting from the use of this software.

## Support and Maintenance

This project is provided without any obligation to provide support, maintenance, updates, bug fixes, or other services.

There is no guarantee that issues reported through GitHub Issues, Discussions, or other communication channels will be addressed.
