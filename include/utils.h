/*
 * DALI-Matter-Gateway
 *
 * Copyright (C) 2026 Malte Rudolf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#include <Arduino.h>

String escapeHtml(const String &text);
String byteToHex(uint8_t value);
String daliRegularCommandToString(uint8_t command);
String daliSpecialCommandToString(uint8_t command, uint8_t parameter);
String daliAddressToString(uint8_t firstOctet, uint8_t secondOctet);
String addLogLine(String &webLog, const String &message);