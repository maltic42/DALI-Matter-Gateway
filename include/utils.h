#include <Arduino.h>

String escapeHtml(const String &text);
String byteToHex(uint8_t value);
String daliRegularCommandToString(uint8_t command);
String daliSpecialCommandToString(uint8_t command, uint8_t parameter);
String daliAddressToString(uint8_t firstOctet, uint8_t secondOctet);
String addLogLine(String &webLog, const String &message);