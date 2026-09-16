#pragma once

#include <Arduino.h>
#include <WebServer.h>

class MatterDimmableLight;

void handleRoot(
	WebServer &server,
	MatterDimmableLight lights[],
	uint8_t lightCount,
	const char *lightNames[],
	String &webLog
);

void handleStatus(
	WebServer &server,
	MatterDimmableLight lights[],
	uint8_t lightCount,
	const char *lightNames[],
	String &webLog
);
