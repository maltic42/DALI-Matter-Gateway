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

#pragma once

#include <Arduino.h>
#include <WebServer.h>

class MatterDimmableLight;

void handleRoot(
	WebServer &server,
	MatterDimmableLight lights[],
	uint8_t lightCount,
	const char *lightNames[],
	const bool lightIsGroup[],
	const uint8_t lightSetIds[],
	const uint8_t lightStatusIds[],
	String &webLog
);

void handleStatus(
	WebServer &server,
	MatterDimmableLight lights[],
	uint8_t lightCount,
	const char *lightNames[],
	const bool lightIsGroup[],
	const uint8_t lightSetIds[],
	const uint8_t lightStatusIds[],
	String &webLog
);
