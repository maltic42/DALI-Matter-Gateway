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

Second version of the DALI-Matter-Gateway.

Controls DALI lights via Matter.

## Hardware

- Waveshare ESP32-S3-Pico
- Waveshare DALI2 Expansion Module

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

## Configuration

The following files contain project-specific configuration and should not be committed to Git:

- `src/secrets.cpp` — Wi-Fi credentials
- `src/light_definitions.cpp` — local light configuration

These files are listed in `.gitignore`.

## Building

Open the project in PlatformIO and build the project using the configured environment.

The firmware can be uploaded via USB or OTA.

### Versioning and builds

The current product version is stored in `VERSION` and follows semantic versioning.
The build number is stored in `BUILD` and is automatically incremented before each
local PlatformIO build or upload. The version, build number, and copyright notice are
shown in the web interface and in the `/status` JSON response.

Development work should be done on a `development` branch and merged into `main` when
it is ready. To publish a version, update `VERSION`, merge it into `main`, and create
and push a matching tag. The build number is incremented automatically by PlatformIO:

```bash
printf "0.1.0\n" > VERSION
platformio run
git add BUILD VERSION
git commit -m "Prepare release v0.1.0"
git push
git tag v0.1.0
git push origin v0.1.0
```

The tag starts a GitHub Actions workflow that creates a GitHub Release containing only
the source archive. The release title and description contain the version and build
number. No PlatformIO build runs on GitHub.

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
