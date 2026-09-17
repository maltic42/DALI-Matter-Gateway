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

#include <http.h>
#include <Matter.h>
#include <utils.h>

static String escapeJson(const String &text) {
  String escaped;
  escaped.reserve(text.length());
  for (uint16_t i = 0; i < text.length(); ++i) {
    const char character = text[i];
    switch (character) {
      case '\\': escaped += "\\\\"; break;
      case '"': escaped += "\\\""; break;
      case '\n': escaped += "\\n"; break;
      case '\r': escaped += "\\r"; break;
      case '\t': escaped += "\\t"; break;
      default: escaped += character; break;
    }
  }
  return escaped;
}

void handleRoot(
  WebServer &server,
  MatterDimmableLight lights[],
  uint8_t lightCount,
  const char *lightNames[],
  const bool lightIsGroup[],
  const uint8_t lightSetIds[],
  const uint8_t lightStatusIds[],
  String &webLog
) {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>DALI-Matter-Gateway</title>";
  html += "<style>";
  html += "*{box-sizing:border-box}body{margin:0;background:#f3f5f6;color:#263238;font-family:Arial,sans-serif;line-height:1.45}";
  html += ".page{max-width:960px;margin:0 auto;padding:32px 20px}header{border-bottom:1px solid #cfd8dc;padding-bottom:18px;margin-bottom:24px}";
  html += "h1{font-size:28px;font-weight:500;margin:0 0 12px}h2{font-size:18px;font-weight:500;margin:0 0 12px}";
  html += ".status{display:inline-block;padding:5px 10px;border-left:4px solid #607d8b;background:#fff;font-size:14px}";
  html += ".status.ok{border-color:#2e7d32;color:#1b5e20}.status.wait{border-color:#ef6c00;color:#bf360c}";
  html += ".memory{margin-top:8px;color:#78909c;font-size:12px}";
  html += ".section{background:#fff;border-top:3px solid #90a4ae;padding:18px 20px;margin-bottom:20px}";
  html += "table{width:100%;border-collapse:collapse}th,td{text-align:left;vertical-align:middle;padding:11px 8px;border-bottom:1px solid #eceff1}th{color:#607d8b;font-size:12px;font-weight:600;text-transform:uppercase}";
  html += ".state{font:inherit;line-height:inherit}.on{color:#2e7d32}.off{color:#c62828}textarea{display:block;width:100%;min-height:260px;resize:vertical;border:1px solid #cfd8dc;background:#263238;color:#eceff1;padding:12px;font:13px monospace;line-height:1.5}";
  html += "button{border:0;background:#455a64;color:#fff;padding:10px 16px;font-size:14px;cursor:pointer}button:hover{background:#263238}.muted{color:#607d8b;font-size:14px}";
  html += "@media(max-width:600px){.page{padding:22px 12px}h1{font-size:24px}.section{padding:14px 12px}th,td{padding:9px 4px}th:nth-child(3),td:nth-child(3){display:none}}";
  html += "</style></head><body><main class='page'>";
  html += "<header><h1>DALI-Matter-Gateway</h1><div id='commission-status' class='status ";
  html += Matter.isDeviceCommissioned() ? "ok'>Commissioned" : "wait'>Waiting for commissioning";
  html += "</div><div id='memory' class='memory'>SRAM: ";
  html += String(ESP.getFreeHeap() / 1024);
  html += " / ";
  html += String(ESP.getHeapSize() / 1024);
  html += " KB free | PSRAM: ";
  html += String(ESP.getFreePsram() / 1024);
  html += " / ";
  html += String(ESP.getPsramSize() / 1024);
  html += " KB free</div></header>";
  if (Matter.isDeviceCommissioned()) {
    html += "<section class='section'><h2>Lights</h2><table><thead><tr><th>Name</th><th>DALI set ID</th><th>DALI status ID</th><th>Group</th><th>State</th><th>Brightness</th></tr></thead><tbody id='lights-body'>";
    for (uint8_t i = 0; i < lightCount; ++i) {
      html += "<tr><td>";
      html += lightNames[i];
      html += "</td><td>";
      html += String(lightSetIds[i]);
      html += "</td><td>";
      html += String(lightStatusIds[i]);
      html += "</td><td>";
      html += lightIsGroup[i] ? "Yes" : "No";
      html += "</td><td id='light-state-";
      html += String(i);
      html += "' class='state ";
      html += lights[i] ? "on'>ON" : "off'>OFF";
      html += "</td><td id='light-brightness-";
      html += String(i);
      html += "'>";
      html += String(lights[i].getBrightness());
      html += "</td></tr>";
    }
    html += "</tbody></table><p id='light-count' class='muted'>";
    html += String(lightCount);
    html += " light(s) configured</p></section>";
    html += "</p><form method='POST' action='/decommission'><button type='submit'>Decommission</button></form></section>";

  } else {
    html += "<section class='section'><h2>Pairing</h2><p class='muted'>Manual pairing code</p><p>";
    html += String(Matter.getManualPairingCode().c_str());
  }
  html += "<section class='section'><h2>Log</h2><textarea id='log' readonly>";
  html += escapeHtml(webLog);
  html += "</textarea><script>";
  html += "const log=document.getElementById('log');";
  html += "const memory=document.getElementById('memory');";
  html += "const commissionStatus=document.getElementById('commission-status');";
  html += "const initialCommissioned=";
  html += Matter.isDeviceCommissioned() ? "true;" : "false;";
  html += "const updateStatus=async()=>{try{const response=await fetch('/status',{cache:'no-store'});if(!response.ok)return;const data=await response.json();if(data.commissioned!==initialCommissioned){window.location.reload();return;}commissionStatus.textContent=data.commissioned?'Commissioned':'Waiting for commissioning';commissionStatus.className='status '+(data.commissioned?'ok':'wait');memory.textContent=`SRAM: ${data.freeHeap} / ${data.heapSize} KB free | PSRAM: ${data.freePsram} / ${data.psramSize} KB free`;data.lights.forEach((light,index)=>{const state=document.getElementById('light-state-'+index);const brightness=document.getElementById('light-brightness-'+index);if(state){state.textContent=light.on?'ON':'OFF';state.className='state '+(light.on?'on':'off');}if(brightness)brightness.textContent=light.brightness;});const wasAtBottom=log.scrollTop+log.clientHeight>=log.scrollHeight-2;const oldTop=log.scrollTop;log.value=data.log;if(wasAtBottom)log.scrollTop=log.scrollHeight;else log.scrollTop=oldTop;}catch(error){}};";
  html += "setInterval(updateStatus,2000);updateStatus();";
  html += "</script></section></main></body></html>";
  server.send(200, "text/html", html);
}

void handleStatus(
  WebServer &server,
  MatterDimmableLight lights[],
  uint8_t lightCount,
  const char *lightNames[],
  const bool lightIsGroup[],
  const uint8_t lightSetIds[],
  const uint8_t lightStatusIds[],
  String &webLog
) {
  String json = "{\"commissioned\":";
  json += Matter.isDeviceCommissioned() ? "true" : "false";
  json += ",\"freeHeap\":";
  json += String(ESP.getFreeHeap() / 1024);
  json += ",\"heapSize\":";
  json += String(ESP.getHeapSize() / 1024);
  json += ",\"freePsram\":";
  json += String(ESP.getFreePsram() / 1024);
  json += ",\"psramSize\":";
  json += String(ESP.getPsramSize() / 1024);
  json += ",\"lights\":[";
  for (uint8_t i = 0; i < lightCount; ++i) {
    if (i > 0) json += ",";
    json += "{\"name\":\"";
    json += escapeJson(lightNames[i]);
    json += "\",\"on\":";
    json += lights[i] ? "true" : "false";
    json += ",\"brightness\":";
    json += String(lights[i].getBrightness());
    json += "}";
  }
  json += "],\"log\":\"";
  json += escapeJson(webLog);
  json += "\"}";
  server.send(200, "application/json", json);
}
