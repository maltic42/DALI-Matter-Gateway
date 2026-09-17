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
