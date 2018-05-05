#include "WrapperWebconfig.h"

void WrapperWebconfig::begin() {
  _server->onNotFound([&](){ WrapperWebconfig::handleNotFound(); });
  _server->on("/", [&](){ WrapperWebconfig::handleRoot(); });
  _server->begin();
}

void WrapperWebconfig::handle(void) {
  _server->handleClient();
}

void WrapperWebconfig::handleNotFound(void) {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += _server->uri();
  message += "\nMethod: ";
  message += (_server->method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += _server->args();
  message += "\n";
  for (uint8_t i=0; i<_server->args(); i++){
    message += " " + _server->argName(i) + ": " + _server->arg(i) + "\n";
  }
  _server->send(404, "text/plain", message);
}

void WrapperWebconfig::handleRoot(void) {
  Log.debug("Webconfig started HEAP=%i", ESP.getFreeHeap());
  initHelperVars();
  Log.debug("Webconfig initialized HEAP=%i", ESP.getFreeHeap());
  if (_server->method() == HTTP_POST) {
    Log.debug("POST HEAP=%i", ESP.getFreeHeap());
    changeConfig();
  }
  clearHelperVars();
  String message = htmlTemplate("ESP8266 LED Configuration", WrapperWebconfig::config());
  Log.debug("Webconfig max HEAP=%i", ESP.getFreeHeap());
  
  //Log.debug("Webconfig cleared HEAP=%i", ESP.getFreeHeap());
  _server->send(200, "text/html", message);
  Log.debug("Webconfig sent HEAP=%i", ESP.getFreeHeap());
}

String WrapperWebconfig::escape(String text) {
  return text;
}
String WrapperWebconfig::escape(char* text) {
  return String(text);
}
String WrapperWebconfig::escape(uint8_t text) {
  char buf[4];
  sprintf(buf, "%u", text);
  return String(buf);  
}
String WrapperWebconfig::escape(uint16_t text) {
  char buf[6];
  sprintf(buf, "%u", text);
  return String(buf);  
}
String WrapperWebconfig::escape(uint32_t text) {
  char buf[10];
  sprintf(buf, "%u", text);
  return String(buf);  
}
String WrapperWebconfig::ipToString(ConfigIP ip) {
  char buf[16];
  if (ip.a == 0)
    return "";
  
  sprintf(buf, "%d.%d.%d.%d", ip.a, ip.b, ip.c, ip.d);
  return String(buf);
}
void WrapperWebconfig::changeConfig(void) {
  ConfigStruct *cfg = Config::getConfig();
  boolean restart = false;
  boolean loadStatic = false;
  
  for (uint8_t i=0; i<_server->args(); i++){
    String argName = _server->argName(i);
    String argValue = _server->arg(i);

    Log.debug("Config: \"%s\":\"%s\"", argName.c_str(), argValue.c_str());
    
    if (argName == "wifi-ssid" && argValue.length() < sizeof(cfg->wifi.ssid)) {
      strcpy(cfg->wifi.ssid, argValue.c_str());
    } else if (argName == "wifi-password" && argValue.length() < sizeof(cfg->wifi.password) && argValue.length() > 0) {
      //only set if empty (Password field is always empty on the webpage)
      strcpy(cfg->wifi.password, argValue.c_str());
    } else if (argName == "wifi-ip") {
      byte ip[4];
      parseBytes(argValue.c_str(), '.', ip, 4, 10);
      cfg->wifi.ip.a = ip[0];
      cfg->wifi.ip.b = ip[1];
      cfg->wifi.ip.c = ip[2];
      cfg->wifi.ip.d = ip[3];
    } else if (argName == "wifi-subnet") {
      byte ip[4];
      parseBytes(argValue.c_str(), '.', ip, 4, 10);
      cfg->wifi.subnet.a = ip[0];
      cfg->wifi.subnet.b = ip[1];
      cfg->wifi.subnet.c = ip[2];
      cfg->wifi.subnet.d = ip[3];
    } else if (argName == "wifi-dns") {
      byte ip[4];
      parseBytes(argValue.c_str(), '.', ip, 4, 10);
      cfg->wifi.dns.a = ip[0];
      cfg->wifi.dns.b = ip[1];
      cfg->wifi.dns.c = ip[2];
      cfg->wifi.dns.d = ip[3];
    } else if (argName == "wifi-hostname" && argValue.length() < sizeof(cfg->wifi.hostname)) {
      strcpy(cfg->wifi.hostname, argValue.c_str());
    } else if (argName == "ports-json") {
      cfg->ports.jsonServer = argValue.toInt();
      if (cfg->ports.jsonServer == 0)
        cfg->ports.jsonServer = 19444;
    } else if (argName == "ports-udp") {
      cfg->ports.udpLed = argValue.toInt();
      if (cfg->ports.udpLed == 0)
        cfg->ports.udpLed = 19446;
    } else if (argName == "led-idleMode") {
      cfg->led.idleMode = getSelectedEntry<uint8_t>(argValue, _idleModes);
    } else if (argName == "led-timeoutMs") {
      if (argValue.equals("0")) {
        cfg->led.timeoutMs = 0;
      } else {
        int val = argValue.toInt();
        if (val > 0) {
          cfg->led.timeoutMs = val;
        } else {
          cfg->led.timeoutMs = 5000;
        }
      }
    } else if (argName == "led-autoswitch") {
      if (argValue.equals("led-autoswitch")) {
        cfg->led.autoswitch = true;
      } else {
        cfg->led.autoswitch = false;
      }
    } else if (argName == "saveRestart") {
      restart = true;
    } else if (argName == "loadStatic") {
      loadStatic = true;
    }
  }
  if (loadStatic)
    Config::loadStaticConfig();
    
  Config::saveConfig();
  
  if (restart)
    ESP.restart();
}

void WrapperWebconfig::parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
    //sscanf Workaround
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}

String WrapperWebconfig::htmlTemplate(String title, String content) {
  String html = "";
  html += "<html>";
  html += "<head>";
  html += "<meta charset=\"utf-8\">";
  html += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  
  html += "<title>" + title + "</title>";

  html += "<script src=\"https://code.jquery.com/jquery-2.2.1.min.js\" integrity=\"sha256-gvQgAFzTH6trSrAWoH1iPo9Xc96QxSZ3feW6kem+O00=\" crossorigin=\"anonymous\"></script>";
  //html += "<script src=\"https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css\"></script>";

  html += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css\" integrity=\"sha384-1q8mTJOASx8j1Au+a5WDVnPi2lkFfwwEAa8hDDdjZlpLegxhjVME1fgjWPGmkzs7\" crossorigin=\"anonymous\">";
  html += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap-theme.min.css\" integrity=\"sha384-fLW2N01lMqjakBkx3l/M9EahuwpSfeNvV63J5ezn3uZzapT0u7EYsXMjQV+0En5r\" crossorigin=\"anonymous\">";
  html += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js\" integrity=\"sha384-0mSbJDEHialfmuBBQP6A4Qrprq5OVfW37PRR3j5ELqxss1yVqOtnepnHVP9aJ7xS\" crossorigin=\"anonymous\"></script>";

  html += "<script>";
  html += "$(document).ready(function() {";
  html += "  $(\"#loadStatic\").click(function(event) {";
  html += "     if( !confirm('Are you sure you want to overwrite your settings with those from the ConfigStatic.h?') ) ";
  html += "          event.preventDefault();";
  html += "  });";
  html += "  $('[data-toggle=\"tooltip\"]').tooltip();";
  html += "});";
  html += "</script>";

  html += "<style>.tooltip-inner { text-align: left; }</style>";
  
  html += "</head>";
  
  html += "<body role=\"document\">";
  html += "<div class=\"container theme-showcase\" role=\"main\">";
  html += content;
  html += "</div>";
  html += "</body>";
  
  html += "</html>";

  return html;
}

String WrapperWebconfig::groupTemplate(String title, String body) {
  String html = "";
  
  html += "<div class=\"panel panel-default\">";
  html +=   "<div class=\"panel-heading\">" + title + "</div>";
  html +=   "<div class=\"panel-body\">";

  html += body;
  
  html +=   "</div>";
  html += "</div>";
  
  return html;
}

String WrapperWebconfig::entryTemplate(String label, String tooltip, String id, String content) {
  String html = "";
  
  html += "<div class=\"form-group\">";
  html +=   "<label class=\"col-md-4 control-label\" for=\"" + id + "\">" + label + "</label>";
  html +=   "<div class=\"col-md-4\">";
  html +=     content;
  html +=   "</div>";

  if (tooltip.length() > 0) {
    html +=   "<div class=\"col-md-4\">";
    html +=     "<span class=\"badge\" data-toggle=\"tooltip\" title=\"" + tooltip + "\">i</span>";
    html +=   "</div>";
  }
  
  html += "</div>";

  return html;
}

String WrapperWebconfig::textTemplate(String label, String tooltip, String id, String text, String placeholder = "", int maxLen = 524288) {
  String html = "";

  html += "<input id=\"" + id + "\" name=\"" + id + "\" type=\"text\" maxlength=\"" + maxLen + "\" placeholder=\"" + placeholder + "\" class=\"form-control input-md\" value=\"" + text + "\"/>";

  return entryTemplate(label, tooltip, id, html);
}

String WrapperWebconfig::checkboxTemplate(String label, String tooltip, String id, boolean isChecked) {
  String html = "";
  String checked = "";
  if (isChecked) {
    checked = " checked=\"checked\"";
  }
  html += "<label class=\"checkbox-inline\">";
  html +=   "<input type=\"hidden\" value=\"\" name=\"" + id + "\">";
  html +=   "<input id=\"" + id + "\" name=\"" + id + "\" type=\"checkbox\" value=\"" + id + "\" class=\"input-md\"" + checked + "/>";
  html += "</label>";

  return entryTemplate(label, tooltip, id, html);
}

String WrapperWebconfig::selectTemplate(String label, String tooltip, String id, LinkedList<SelectEntryBase*>* entries) {
  String html = "";

  html += "<select id=\"" + id + "\" name=\"" + id + "\" class=\"form-control\">";
  for (int i=0; i<entries->size(); i++) {
    SelectEntryBase *entry = entries->get(i);
    html += "<option value=\"" + entry->getSelectedValue() + "\"";
    if (entry->isSelected()) 
      html += " selected=\"selected\"";
    html += ">";
    html += entry->getText();
    html += "</option>";
  }
  html += "</select>";

  return entryTemplate(label, tooltip, id, html);
}

String WrapperWebconfig::config(void) {
  String html = "";
  String groupContent = "";
  boolean wifiReady = false;
  
  ConfigStruct *cfg = Config::getConfig();

  if (cfg->wifi.ssid[0] != 0) {
    //check Wifi
    wifiReady = true;
  }
  
  html += "<form class=\"form-horizontal\" method=\"post\">";
  html += "<fieldset>";

  html += "<legend>ESP8266 LED Coniguration</legend>";

  groupContent = "";
  groupContent += textTemplate("WiFi SSID", "", "wifi-ssid", escape(cfg->wifi.ssid), "", sizeof(cfg->wifi.ssid)-1);

  String passwordPlaceholder = "no password set";
  if (cfg->wifi.password[0] != 0)
    passwordPlaceholder = "password saved";
  groupContent += textTemplate("WiFi Password", "", "wifi-password", "", passwordPlaceholder, sizeof(cfg->wifi.password)-1);

  groupContent += textTemplate("IP","",  "wifi-ip", ipToString(cfg->wifi.ip), "leave empty for dhcp", 15);
  groupContent += textTemplate("Subnet","",  "wifi-subnet", ipToString(cfg->wifi.subnet), "255.255.255.0", 15);
  groupContent += textTemplate("DNS Server","",  "wifi-dns", ipToString(cfg->wifi.dns), "192.168.1.1", 15);
  
  groupContent += textTemplate("Module Hostname","",  "wifi-hostname", escape(cfg->wifi.hostname), "ESP8266", sizeof(cfg->wifi.hostname)-1);
  
  html += groupTemplate("WiFi", groupContent);
  
  if (wifiReady) {
    groupContent = "";
    groupContent += textTemplate("JSON Server Port","",  "ports-json", escape(cfg->ports.jsonServer), "19444", 5);
    groupContent += textTemplate("UDP LED Port","",  "ports-udp", escape(cfg->ports.udpLed), "19446", 5);
    html += groupTemplate("Ports", groupContent);
    groupContent = "";

    _idleModes = new LinkedList<SelectEntryBase*>();
    getIdleModes(cfg->led.idleMode, _idleModes);
    groupContent += selectTemplate("LED Idle Mode", "", "led-idleMode", _idleModes);
    groupContent += checkboxTemplate("Autoswitch to Hyperion_UDP/Idle Mode", "Automatically switch to Hyperion_UDP when UDP Data arriving and switch back to idle mode after timeout", "led-autoswitch", cfg->led.autoswitch);
    groupContent += textTemplate("Timeout Fallback in MS", "Switches back to Idle Mode after x milliseconds when no UDP data is arriving",  "led-timeoutMs", escape(cfg->led.timeoutMs), "5000", 10);
    clearLinkedList(_idleModes);
    
    html += groupTemplate("LEDs", groupContent);
    
  } else {
    html += "<div class=\"form-group\">";
    html += "More settings visible, when wifi-connection is ready (internet needed for design)";
    html += "</div>";
  }

  groupContent = "";
  groupContent += "<div class=\"form-group\">";
  groupContent +=   "<label class=\"col-md-4 control-label\" for=\"save\"></label>";
  groupContent +=   "<div class=\"col-md-4\">";
  groupContent +=     "<button id=\"save\" name=\"save\" class=\"btn btn-primary\">Save Settings</button>";
  groupContent +=   "</div>";
  groupContent += "</div>";
  
  groupContent += "<div class=\"form-group\">";
  groupContent +=   "<label class=\"col-md-4 control-label\" for=\"saveRestart\"></label>";
  groupContent +=   "<div class=\"col-md-4\">";
  groupContent +=     "<button id=\"saveRestart\" name=\"saveRestart\" class=\"btn btn-primary\">Save &amp; restart</button>";
  groupContent +=   "</div>";
  groupContent += "</div>";
  
  groupContent += "<div class=\"form-group\">";
  groupContent +=   "<label class=\"col-md-4 control-label\" for=\"loadStatic\"></label>";
  groupContent +=   "<div class=\"col-md-4\">";
  groupContent +=     "<button id=\"loadStatic\" name=\"loadStatic\" class=\"btn btn-primary\">Load static config</button>";
  groupContent +=   "</div>";
  groupContent += "</div>";
  
  groupContent += "</fieldset>";
  groupContent += "</form>";
  
  html += groupTemplate("Buttons", groupContent);

  return html;
}

void WrapperWebconfig::initHelperVars(void) {
  ConfigStruct *cfg = Config::getConfig();
  
  _idleModes = new LinkedList<SelectEntryBase*>();
  getIdleModes(cfg->led.idleMode, _idleModes);
}
void WrapperWebconfig::clearHelperVars(void) {
  clearLinkedList(_idleModes);
}
void WrapperWebconfig::clearLinkedList(LinkedList<SelectEntryBase*>* target) {
  Log.debug("Clearing LinkedList HEAP=%i", ESP.getFreeHeap());
  for (int i=0; i<target->size(); i++) {
    SelectEntryBase* entry = target->get(i);
    delete(entry);
  }
  target->clear();
  delete(target);
  Log.debug("Cleared LinkedList HEAP=%i", ESP.getFreeHeap());
}

template<typename T>
T WrapperWebconfig::getSelectedEntry(String selectedEntryValue, LinkedList<SelectEntryBase*>* target) {
  T result;
  SelectEntry<T>* entry;
  for (int i=0; i<target->size(); i++) {
    entry = (SelectEntry<T>*)target->get(i);
    if (entry->getSelectedValue() == selectedEntryValue) {
      result = entry->getValue();
      entry->setSelected(true);
    } else {
      entry->setSelected(false);
    }
  }
  return result;
}

void WrapperWebconfig::getIdleModes(uint8_t active, LinkedList<SelectEntryBase*>* target) {
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Off", "Off", active == OFF, OFF));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Hyperion", "Hyperion UDP", active == HYPERION_UDP, HYPERION_UDP));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Static", "Static color", active == STATIC_COLOR, STATIC_COLOR));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Rainbow", "Rainbow", active == RAINBOW, RAINBOW));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Fire2012", "Fire2012", active == FIRE2012, FIRE2012));
}
