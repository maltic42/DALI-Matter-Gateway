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
#include <Matter.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <time.h>
#include <DALI_lib.h>
#include <secrets.h>
#include <light_definitions.h>
#include <utils.h>
#include <http.h>

// Matter variables
MatterDimmableLight *lights;
uint32_t lastMatterMessageMillis = 0;
bool MatterCommissionedMessageShown = false;
bool updateDaliValues = false;

// WIFI variables
WebServer server(80);

String webLog;

// DALI variables
#define TX_PIN 17
#define RX_PIN 14
extern uint8_t DALI_Addr[64];
extern uint8_t DALI_NUM;
Dali dali;
hw_timer_t *timer = NULL;
bool dataReceived=false;
bool updateCloudValues=false;
uint8_t firstOctet,secondOctet;
unsigned long previousMillis = 0;

// DALI code

uint8_t bus_is_high() {
  return digitalRead(RX_PIN); 
}

void bus_set_low() {
  digitalWrite(TX_PIN,LOW); 
}

void bus_set_high() {
  digitalWrite(TX_PIN,HIGH); 
}

void ARDUINO_ISR_ATTR onTimer() {
  dali.timer();
}

void init_dali() {
  addLogLine(webLog, "Setting up DALI ...");

  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  
  timer = timerBegin(9600000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000, true, 0);

  dali.begin(bus_is_high, bus_set_high, bus_set_low);
  addLogLine(webLog, "DALI setup completed.");
}

// Set the Light based on the current state of the Dimmable Light
bool setLightState(bool state, uint8_t brightness) {
  return true;
}

// Handle the decommissioning of the Matter devices
void handleDecommission() {
  for (uint8_t i = 0; i < lightCount; ++i) {
    lights[i] = false;
  }
  Matter.decommission();
  server.send(200, "text/plain", "Decommissioning...\n");
}

void init_wifi() {
  addLogLine(webLog, "Setting up WIFI ...");
  addLogLine(webLog, String("Connecting to ")+wifiSsid);
  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  addLogLine(webLog, "WIFI connected.");
  addLogLine(webLog, String("IP address: ")+WiFi.localIP().toString());
  addLogLine(webLog, "WIFI setup completed.");
}

void init_ota() {
  addLogLine(webLog, "Setting up OTA ...");
  ArduinoOTA.setHostname("esp32-matter");

  ArduinoOTA.onStart([]() {
    addLogLine(webLog, "OTA update started.");
  });

  ArduinoOTA.onEnd([]() {
      addLogLine(webLog, "OTA update finished.");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      addLogLine(webLog, String("Progress: ")+String((progress * 100) / total)+String("%"));
  });

  ArduinoOTA.onError([](ota_error_t error) {
      addLogLine(webLog, String("OTA error [")+String(error)+String("]"));
  });

  ArduinoOTA.begin();
  addLogLine(webLog, "OTA setup completed.");
}

void init_webserver() {
  addLogLine(webLog, "Setting up web server ...");
  server.on("/", []() {
    handleRoot(server, lights, lightCount, lightNames, lightIsGroup, lightSetIds, lightStatusIds, webLog);
  });
  server.on("/status", []() {
    handleStatus(server, lights, lightCount, lightNames, lightIsGroup, lightSetIds, lightStatusIds, webLog);
  });
  server.on("/decommission", HTTP_POST, handleDecommission);
  server.begin();
  addLogLine(webLog, "Web server setup completed.");
}

void init_matter() {
  int16_t level;

  addLogLine(webLog, "Setting up Matter ...");
  for (uint8_t i = 0; i < lightCount; ++i) {

    lights[i].begin();

    // set the callback function to handle the Light state change
    lights[i].onChange(setLightState);
  
    // lambda functions are used to set the attribute change callbacks
    lights[i].onChangeOnOff([i](bool state) {
      addLogLine(webLog, String("Received OnOff change for light ")+String(i + 1)+String(" changed to ")+String(state ? "ON" : "OFF"));
      if ((!state) && (updateDaliValues)) {
        if (lightIsGroup[i]) {
          addLogLine(webLog, String("Setting DALI group ")+String(lightSetIds[i])+String(" to brightness 0"));
          dali.set_level(0, 64+lightSetIds[i]);
        } else {
          addLogLine(webLog, String("Setting DALI device ")+String(lightSetIds[i])+String(" to brightness 0"));
          dali.set_level(0, lightSetIds[i]);
        }  
      }
      return true;
    });
    lights[i].onChangeBrightness([i](uint8_t level) {
      addLogLine(webLog, String("Received brightness change for light ")+String(i + 1)+String(" changed to ")+String(level));
      if ((lights[i].getOnOff()) && (updateDaliValues)) {
        if (lightIsGroup[i]) {
          addLogLine(webLog, String("Setting DALI group ")+String(lightSetIds[i])+String(" to brightness ")+String(level));
          dali.set_level(level, 64+lightSetIds[i]);
        } else {
          addLogLine(webLog, String("Setting DALI device ")+String(lightSetIds[i])+String(" to brightness ")+String(level));
          dali.set_level(level, lightSetIds[i]);
        }
     }
      return true;
    });
  } 

  Matter.setVendorName("toad.de");
  Matter.setDeviceName("DALI-Matter-Bridge");
  Matter.setProductName("DALI-Matter-Bridge");
  Matter.setHardwareVersion(1);
  Matter.setHardwareVersionString("1.0");
  Matter.setSerialNumber("00000001");

  // Matter beginning - Last step, after all EndPoints are initialized
  Matter.begin();
  // This may be a restart of a already commissioned Matter accessory
  if (Matter.isDeviceCommissioned()) {
    addLogLine(webLog, "Matter Node is commissioned and connected to the network. Ready for use.");
  } 

  addLogLine(webLog, "Updating Matter Identity...");

  // Attribute IDs for Basic Information Cluster (0x0028)
  uint32_t cluster_id = chip::app::Clusters::BasicInformation::Id;
  cluster_t *cluster = cluster::get((uint16_t)2, cluster_id);

  auto update_attr = [&](uint32_t endpoint_id, uint32_t attr_id, const char* value) {
    esp_matter_attr_val_t val = esp_matter_char_str((char*)value, strlen(value));
    esp_err_t err = attribute::update(endpoint_id, cluster_id, attr_id, &val);
    addLogLine(webLog, String("Updating attr ")+String(attr_id)+String(" to ")+String(value));
    if (err != ESP_OK) {
      addLogLine(webLog, String("Failed to update attr ")+String(attr_id)+String(" error ") + String(err));
    }
  };

  // Update Node Label (The friendly name)
  update_attr(0, chip::app::Clusters::BasicInformation::Attributes::NodeLabel::Id, "DALI-Matter-Bridge");
  
 // attribute::create(cluster, chip::app::Clusters::BasicInformation::Attributes::NodeLabel::Id, ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_char_str("DALI-Matter-Bridge x", strlen("DALI-Matter-Bridge")));
  
  // addLogLine(webLog, String("Endpoint ID for light 1: ")+String(lights[1].getEndPointId()));

  addLogLine(webLog, "Matter setup completed.");
}

void setup() {
  Serial.begin(115200);

  lights = new MatterDimmableLight[lightCount];

  addLogLine(webLog, "Starting up ...");

  init_wifi();
  init_ota();
  init_webserver();
  init_dali();
  init_matter();
  updateCloudValues=true;
  previousMillis = millis();
}

void loop() {
  int16_t level;

  server.handleClient();
  ArduinoOTA.handle();

// DALI loop

  uint8_t data[100];
  uint8_t bitcnt = dali.rx(data);

  if(bitcnt>=8) {
    updateCloudValues=false;
    for(uint8_t i=0;i<=(bitcnt-1)>>3;i+=2) {
      firstOctet = data[i];
      i++;
      if (i>(bitcnt-1)>>3) {
        break;
      }
      secondOctet = data[i];  
      addLogLine(webLog, String("DALI data received: ")+byteToHex(firstOctet)+String(" ")+byteToHex(secondOctet));
      addLogLine(webLog, String("DALI command: ") + daliAddressToString(firstOctet, secondOctet));
      if ((secondOctet==0) || (secondOctet==5) || (secondOctet==2) || (secondOctet==1)) {
        for (uint8_t i = 0; i < lightCount; ++i) {
          addLogLine(webLog, "Queuing light update ...");
          updateCloudValues=true;
          previousMillis = millis();
        }
      }
    }
  }

  if (updateCloudValues && (millis() - previousMillis >= 5000)) {
    updateCloudValues=false;
    updateDaliValues=false;
    for (uint8_t i = 0; i < lightCount; ++i) {
      level = dali.cmd(DALI_QUERY_ACTUAL_LEVEL, lightStatusIds[i]);
      addLogLine(webLog, String("Current status of DALI light ")+String(i + 1)+String(": ")+lightNames[i]+String(" | DALI address: ")+String(lightStatusIds[i])+String(" | brightness: ")+String(level));
      if (level == 0) {
        addLogLine(webLog, String("Setting Matter light ")+String(i + 1)+String(" to OFF"));
        lights[i].setOnOff(false);
        addLogLine(webLog, String("Setting Matter light ")+String(i + 1)+String(" brightness to 254"));
        lights[i].setBrightness(254);
       } else {
        addLogLine(webLog, String("Setting Matter light ")+String(i + 1)+String(" to ON"));
        lights[i].setOnOff(true);
        addLogLine(webLog, String("Setting Matter light ")+String(i + 1)+String(" brightness to ")+String(level));
        lights[i].setBrightness(level);
      }
   //   lights[i].updateAccessory();
      updateDaliValues=true;
    }
  }

  if (!Matter.isDeviceCommissioned()) {
    if (millis() - lastMatterMessageMillis > 10000) {
      lastMatterMessageMillis = millis();
      addLogLine(webLog, "Matter Node is not commissioned yet. Waiting for commissioning.");
      addLogLine(webLog, "");
      addLogLine(webLog, "Matter Node is not commissioned yet.");
      addLogLine(webLog, "Initiate the device discovery in your Matter environment.");
      addLogLine(webLog, "Commission it to your Matter hub with the manual pairing code or QR code");
      addLogLine(webLog, String("Manual pairing code: ")+String(Matter.getManualPairingCode().c_str()));
      addLogLine(webLog, String("QR code URL: ")+String(Matter.getOnboardingQRCodeUrl().c_str()));
    }
  } else {
    if (MatterCommissionedMessageShown == false) {
      addLogLine(webLog, "Matter Node is commissioned and connected to the network. Ready for use.");
      for (uint8_t i = 0; i < lightCount; ++i) {
        addLogLine(webLog, String("Initial state ")+String(i + 1)+String(": ")+String(lights[i] ? "ON" : "OFF")+String(" | brightness: ")+String(lights[i].getBrightness()));
        lights[i].updateAccessory();
      }
      MatterCommissionedMessageShown = true;
    }
  }
}
