/*

Copyright (C) 2026 Malte Rudolf

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0).

You are free to:
use the software for private or commercial purposes study and modify the source code redistribute the software distribute modified versions of the software
Any redistribution of this software or modified versions must comply with the terms of the GNU General Public License v3.0.

Third-Party Components

This project uses third-party software and libraries that are distributed under their respective licenses.
The applicable licenses and copyright notices of third-party components remain in effect. Please refer to the respective source code and documentation for details.

Disclaimer

This software is provided "AS IS", without warranty of any kind, express or implied.
The author provides no guarantee that the software is free of defects or suitable for any particular purpose. The author shall not be responsible for any damage, data loss, hardware damage, financial loss, or other consequences resulting from the use of this software, to the extent permitted by applicable law.

Support and Maintenance

This project is provided without any obligation to provide support, maintenance, updates, bug fixes, or other services.
There is no guarantee that issues reported through GitHub Issues, Discussions, or other communication channels will be addressed.
For the complete terms and conditions, please refer to the LICENSE file containing the GNU General Public License v3.0.

*/

#include <Arduino.h>
#include <DALI_lib.h>
#include <utils.h>

constexpr size_t maxLogLength = 20000;

String escapeHtml(const String &text) {
  String escaped = text;
  escaped.replace("&", "&amp;");
  escaped.replace("<", "&lt;");
  escaped.replace(">", "&gt;");
  escaped.replace("\"", "&quot;");
  return escaped;
}

String byteToHex(uint8_t value) { 
  String str = String(value, HEX); 
  if (str.length() < 2) { 
    str = "0" + str; 
  } 
  str.toUpperCase(); 
  return str; 
}

String daliRegularCommandToString(uint8_t command) {
  switch (command) {
    case 0: return "OFF";
    case 1: return "UP";
    case 2: return "DOWN";
    case 3: return "STEP UP";
    case 4: return "STEP DOWN";
    case 5: return "RECALL MAX LEVEL";
    case 6: return "RECALL MIN LEVEL";
    case 7: return "STEP DOWN AND OFF";
    case 8: return "ON AND STEP UP";
    case 9: return "ENABLE DAPC SEQUENCE";
    case 10: return "GO TO LAST ACTIVE LEVEL";
    case 11 ... 15: return "RESERVED";
    case 16 ... 31: return "GO TO SCENE " + String(command - 16);
    case 32: return "RESET";
    case 33: return "STORE ACTUAL LEVEL IN DTR0";
    case 34: return "SAVE PERSISTENT VARIABLES";
    case 35: return "SET OPERATING MODE";
    case 36: return "RESET MEMORY BANK";
    case 37: return "IDENTIFY DEVICE";
    case 42: return "SET MAX LEVEL";
    case 43: return "SET MIN LEVEL";
    case 44: return "SET SYSTEM FAILURE LEVEL";
    case 45: return "SET POWER ON LEVEL";
    case 46: return "SET FADE TIME";
    case 47: return "SET FADE RATE";
    case 48: return "SET EXTENDED FADE TIME";
    case 64 ... 79: return "SET SCENE " + String(command - 64);
    case 80 ... 95: return "REMOVE FROM SCENE " + String(command - 80);
    case 96 ... 111: return "ADD TO GROUP " + String(command - 96);
    case 112 ... 127: return "REMOVE FROM GROUP " + String(command - 112);
    case 128: return "SET SHORT ADDRESS";
    case 129: return "ENABLE WRITE MEMORY";
    case 144: return "QUERY STATUS";
    case 145: return "QUERY CONTROL GEAR PRESENT";
    case 146: return "QUERY LAMP FAILURE";
    case 147: return "QUERY LAMP POWER ON";
    case 148: return "QUERY LIMIT ERROR";
    case 149: return "QUERY RESET STATE";
    case 150: return "QUERY MISSING SHORT ADDRESS";
    case 151: return "QUERY VERSION NUMBER";
    case 152: return "QUERY CONTENT DTR0";
    case 153: return "QUERY DEVICE TYPE";
    case 154: return "QUERY PHYSICAL MINIMUM LEVEL";
    case 155: return "QUERY POWER FAILURE";
    case 156: return "QUERY CONTENT DTR1";
    case 157: return "QUERY CONTENT DTR2";
    case 158: return "QUERY OPERATING MODE DALI-2";
    case 159: return "QUERY LIGHT SOURCE TYPE";
    case 160: return "QUERY ACTUAL LEVEL";
    case 161: return "QUERY MAX LEVEL";
    case 162: return "QUERY MIN LEVEL";
    case 163: return "QUERY POWER ON LEVEL";
    case 164: return "QUERY SYSTEM FAILURE LEVEL";
    case 165: return "QUERY FADE TIME AND FADE RATE";
    case 166: return "QUERY MANUFACTURER SPECIFIC MODE";
    case 167: return "QUERY NEXT DEVICE TYPE";
    case 168: return "QUERY EXTENDED FADE TIME";
    case 169: return "QUERY CONTROL GEAR FAILURE";
    case 176 ... 191: return "QUERY SCENE " + String(command - 176) + " LEVEL";
    case 192: return "QUERY GROUPS 0-7";
    case 193: return "QUERY GROUPS 8-15";
    case 194: return "QUERY RANDOM ADDRESS H";
    case 195: return "QUERY RANDOM ADDRESS M";
    case 196: return "QUERY RANDOM ADDRESS L";
    case 197: return "READ MEMORY LOCATION";
    case 224: return "REFERENCE SYSTEM POWER";
    case 225: return "ENABLE CURRENT PROTECTOR";
    case 226: return "DISABLE CURRENT PROTECTOR";
    case 227: return "SELECT DIMMING CURVE";
    case 228: return "STORE DTR AS FAST FADE TIME";
    case 237: return "QUERY GEAR TYPE";
    case 238: return "QUERY DIMMING CURVE";
    case 239: return "QUERY POSSIBLE OPERATING MODE";
    case 240: return "QUERY FEATURES";
    case 241: return "QUERY FAILURE STATUS";
    case 242: return "QUERY SHORT CIRCUIT";
    case 243: return "QUERY OPEN CIRCUIT";
    case 244: return "QUERY LOAD DECREASE";
    case 245: return "QUERY LOAD INCREASE";
    case 246: return "QUERY CURRENT PROTECTOR ACTIVE";
    case 247: return "QUERY THERMAL SHUTDOWN";
    case 248: return "QUERY THERMAL OVERLOAD";
    case 249: return "QUERY REFERENCE RUNNING";
    case 250: return "QUERY REFERENCE MEASUREMENT FAILED";
    case 251: return "QUERY CURRENT PROTECTOR ENABLE";
    case 252: return "QUERY OPERATING MODE";
    case 253: return "QUERY FAST FADE TIME";
    case 254: return "QUERY MIN FAST FADE TIME";
    case 255: return "QUERY EXTENDED VERSION NUMBER";
    default: return "RESERVED";
  }
}

String daliSpecialCommandToString(uint8_t command, uint8_t parameter) {
  String name;
  switch (command) {
    case 0xA1: name = "TERMINATE"; break;
    case 0xA3: name = "DTR0"; break;
    case 0xA5: name = "INITIALISE"; break;
    case 0xA7: name = "RANDOMISE"; break;
    case 0xA9: name = "COMPARE"; break;
    case 0xAB: name = "WITHDRAW"; break;
    case 0xAF: name = "PING"; break;
    case 0xB1: name = "SEARCHADDRH"; break;
    case 0xB3: name = "SEARCHADDRM"; break;
    case 0xB5: name = "SEARCHADDRL"; break;
    case 0xB7: name = "PROGRAM SHORT ADDRESS"; break;
    case 0xB9: name = "VERIFY SHORT ADDRESS"; break;
    case 0xBB: name = "QUERY SHORT ADDRESS"; break;
    case 0xBD: name = "PHYSICAL SELECTION"; break;
    case 0xC1: name = "ENABLE DEVICE TYPE X"; break;
    case 0xC3: name = "DTR1"; break;
    case 0xC5: name = "DTR2"; break;
    case 0xC7: name = "WRITE MEMORY LOCATION"; break;
    case 0xC9: name = "WRITE MEMORY LOCATION NO REPLY"; break;
    default: return "SPECIAL 0x" + byteToHex(command) + " 0x" + byteToHex(parameter);
  }
  return name + " 0x" + byteToHex(parameter);
}

String daliAddressToString(uint8_t firstOctet, uint8_t secondOctet) {
  if (firstOctet >= 0xA1 && firstOctet <= 0xC9 && (firstOctet & 0x01)) {
    return daliSpecialCommandToString(firstOctet, secondOctet);
  }

  if ((firstOctet & 0x01) == 0) {
    String target;
    if (firstOctet == 0xFE) {
      target = "Broadcast";
    } else if ((firstOctet & 0xE0) == 0x80) {
      target = "Group " + String((firstOctet >> 1) & 0x0F);
    } else {
      target = "Device " + String((firstOctet >> 1) & 0x3F);
    }
    return target + " - DAPC " + String(secondOctet);
  }

  String target;
  if (firstOctet == 0xFF) {
    target = "Broadcast";
  } else if ((firstOctet & 0xE0) == 0x80) {
    target = "Group " + String((firstOctet >> 1) & 0x0F);
  } else {
    target = "Device " + String((firstOctet >> 1) & 0x3F);
  }
  return target + " - " + daliRegularCommandToString(secondOctet);
}

String addLogLine(String &webLog, const String &message) {
  struct tm timeInfo;
  String timestamp;

  if (getLocalTime(&timeInfo, 100)) {
    char timeBuffer[24];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    timestamp = String(timeBuffer);
  } else {
    timestamp = "uptime " + String(millis() / 1000) + "s";
  }

  String line = timestamp + " - " + message + "\n";
  webLog = line + webLog;
  while (webLog.length() > maxLogLength) {
    int newlinePosition = webLog.lastIndexOf('\n', webLog.length() - 2);
    if (newlinePosition < 0) {
      webLog = webLog.substring(0, maxLogLength);
      break;
    }
    webLog.remove(newlinePosition + 1);
  }
  Serial.print(line);
  return webLog;
}