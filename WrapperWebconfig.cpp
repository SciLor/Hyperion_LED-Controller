#include "WrapperWebconfig.h"

#ifdef CONFIG_TYPE_WEBCONFIG
void WrapperWebconfig::begin() {
  _server.onNotFound([&](){ WrapperWebconfig::handleNotFound(); });
  _server.on("/", [&](){ WrapperWebconfig::handleRoot(); });
  _server.begin();
}

void WrapperWebconfig::handle(void) {
  _server.handleClient();
}

void WrapperWebconfig::handleNotFound(void) {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += _server.uri();
  message += "\nMethod: ";
  message += (_server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += _server.args();
  message += "\n";
  for (uint8_t i=0; i<_server.args(); i++){
    message += " " + _server.argName(i) + ": " + _server.arg(i) + "\n";
  }
  _server.send(404, "text/plain", message);
}

void WrapperWebconfig::handleRoot(void) {
  Log.debug("Webconfig started HEAP=%i", ESP.getFreeHeap());
  initHelperVars();
  Log.debug("Webconfig initialized HEAP=%i", ESP.getFreeHeap());
  if (_server.method() == HTTP_POST) {
    Log.debug("POST HEAP=%i", ESP.getFreeHeap());
    changeConfig();
  }
  clearHelperVars();
  String message = htmlTemplate("ESP8266 LED Configuration", WrapperWebconfig::config());
  Log.debug("Webconfig max HEAP=%i", ESP.getFreeHeap());
  
  //Log.debug("Webconfig cleared HEAP=%i", ESP.getFreeHeap());
  _server.send(200, "text/html", message);
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
String WrapperWebconfig::ipToString(ConfigIP ip) {
  char buf[16];
  if (ip.a == 0)
    return "";
  
  sprintf(buf, "%d.%d.%d.%d", ip.a, ip.b, ip.c, ip.d);
  return String(buf);
}
void WrapperWebconfig::changeConfig(void) {
  ConfigStruct cfg = Config::getConfig();
  boolean restart = false;
  for (uint8_t i=0; i<_server.args(); i++){
    String argName = _server.argName(i);
    String argValue = _server.arg(i);

    Log.debug("Config: \"%s\":\"%s\"", argName.c_str(), argValue.c_str());
    
    if (argName == "wifi-ssid" && argValue.length() < sizeof(cfg.wifi.ssid)) {
      strcpy(cfg.wifi.ssid, argValue.c_str());
    } else if (argName == "wifi-password" && argValue.length() < sizeof(cfg.wifi.password) && argValue.length() > 0) {
      //only set if empty (Password field is always empty on the webpage)
      strcpy(cfg.wifi.password, argValue.c_str());
    } else if (argName == "wifi-ip") {
      byte ip[4];
      parseBytes(argValue.c_str(), '.', ip, 4, 10);
      cfg.wifi.ip.a = ip[0];
      cfg.wifi.ip.b = ip[1];
      cfg.wifi.ip.c = ip[2];
      cfg.wifi.ip.d = ip[3];
      //sscanf(argValue.c_str(), "%d.%d.%d.%d", &cfg.wifi.ip.a, &cfg.wifi.ip.b, &cfg.wifi.ip.c, &cfg.wifi.ip.d);
    } else if (argName == "wifi-subnet") {
      byte ip[4];
      parseBytes(argValue.c_str(), '.', ip, 4, 10);
      cfg.wifi.subnet.a = ip[0];
      cfg.wifi.subnet.b = ip[1];
      cfg.wifi.subnet.c = ip[2];
      cfg.wifi.subnet.d = ip[3];
      //sscanf(argValue.c_str(), "%d.%d.%d.%d", &cfg.wifi.subnet.a, &cfg.wifi.subnet.b, &cfg.wifi.subnet.c, &cfg.wifi.subnet.d);
    } else if (argName == "wifi-dns") {
      byte ip[4];
      parseBytes(argValue.c_str(), '.', ip, 4, 10);
      cfg.wifi.dns.a = ip[0];
      cfg.wifi.dns.b = ip[1];
      cfg.wifi.dns.c = ip[2];
      cfg.wifi.dns.d = ip[3];
      //sscanf(argValue.c_str(), "%d.%d.%d.%d", &cfg.wifi.dns.a, &cfg.wifi.dns.b, &cfg.wifi.dns.c, &cfg.wifi.dns.d);
    } else if (argName == "wifi-hostname" && argValue.length() < sizeof(cfg.wifi.hostname)) {
      strcpy(cfg.wifi.hostname, argValue.c_str());
    } else if (argName == "ports-json") {
      cfg.ports.jsonServer = argValue.toInt();
      if (cfg.ports.jsonServer == 0)
        cfg.ports.jsonServer = 19444;
    } else if (argName == "ports-udp") {
      cfg.ports.udpLed = argValue.toInt();
      if (cfg.ports.udpLed == 0)
        cfg.ports.udpLed = 19446;
    } else if (argName == "led-chipset") {
      cfg.led.chipset = getSelectedEntry<uint8_t>(argValue, _chipsets);
    } else if (argName == "led-colorOrder") {
      cfg.led.colorOrder = getSelectedEntry<uint8_t>(argValue, _rgbOrder);
    } else if (argName == "led-dataPin") {
      cfg.led.dataPin = getSelectedEntry<uint8_t>(argValue, _dataPins);
    } else if (argName == "led-clockPin") {
      cfg.led.clockPin = getSelectedEntry<uint8_t>(argValue, _clockPins);
    } else if (argName == "led-count") {
      cfg.led.count = argValue.toInt();
      if (cfg.led.count == 0)
        cfg.led.count = 1;
    } else if (argName == "led-idleMode") {
      cfg.led.idleMode = getSelectedEntry<uint8_t>(argValue, _idleModes);
    } else if (argName == "saveRestart") {
      restart = true;
    }
  }
  Config::saveConfig(cfg);
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

  //html += "<script src=\"https://code.jquery.com/jquery-2.2.1.min.js\" integrity=\"sha256-gvQgAFzTH6trSrAWoH1iPo9Xc96QxSZ3feW6kem+O00=\" crossorigin=\"anonymous\"></script>";
  //html += "<script src=\"https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css\"></script>"

  html += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css\" integrity=\"sha384-1q8mTJOASx8j1Au+a5WDVnPi2lkFfwwEAa8hDDdjZlpLegxhjVME1fgjWPGmkzs7\" crossorigin=\"anonymous\">";
  html += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap-theme.min.css\" integrity=\"sha384-fLW2N01lMqjakBkx3l/M9EahuwpSfeNvV63J5ezn3uZzapT0u7EYsXMjQV+0En5r\" crossorigin=\"anonymous\">";
  html += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js\" integrity=\"sha384-0mSbJDEHialfmuBBQP6A4Qrprq5OVfW37PRR3j5ELqxss1yVqOtnepnHVP9aJ7xS\" crossorigin=\"anonymous\"></script>";
  
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

String WrapperWebconfig::entryTemplate(String title, String id, String content) {
  String html = "";
  
  html += "<div class=\"form-group\">";
  html +=   "<label class=\"col-md-4 control-label\" for=\"" + id + "\">" + title + "</label>";
  html +=   "<div class=\"col-md-4\">";
  html += content;
  html +=   "</div>";
  html += "</div>";

  return html;
}

String WrapperWebconfig::textTemplate(String title, String id, String text, String placeholder = "", int maxLen = 524288) {
  String html = "";

  html += "<input id=\"" + id + "\" name=\"" + id + "\" type=\"text\" maxlength=\"" + maxLen + "\" placeholder=\"" + placeholder + "\" class=\"form-control input-md\" value=\"" + text + "\">";

  return entryTemplate(title, id, html);
}

String WrapperWebconfig::selectTemplate(String title, String id, LinkedList<SelectEntryBase*>* entries) {
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

  return entryTemplate(title, id, html);
}

String WrapperWebconfig::config(void) {
  String html = "";
  String groupContent = "";
  boolean wifiReady = false;
  
  ConfigStruct cfg = Config::getConfig();

  if (cfg.wifi.ssid[0] != 0) {
    //check Wifi
    wifiReady = true;
  }
  
  html += "<form class=\"form-horizontal\" method=\"post\">";
  html += "<fieldset>";

  html += "<legend>ESP8266 LED Coniguration</legend>";

  groupContent = "";
  groupContent += textTemplate("WiFi SSID", "wifi-ssid", escape(cfg.wifi.ssid), "", sizeof(cfg.wifi.ssid)-1);

  String passwordPlaceholder = "no password set";
  if (cfg.wifi.password[0] != 0)
    passwordPlaceholder = "password saved";
  groupContent += textTemplate("WiFi Password", "wifi-password", "", passwordPlaceholder, sizeof(cfg.wifi.password)-1);

  groupContent += textTemplate("IP", "wifi-ip", ipToString(cfg.wifi.ip), "leave empty for dhcp", 15);
  groupContent += textTemplate("Subnet", "wifi-subnet", ipToString(cfg.wifi.subnet), "255.255.255.0", 15);
  groupContent += textTemplate("DNS Server", "wifi-dns", ipToString(cfg.wifi.dns), "192.168.1.1", 15);
  
  groupContent += textTemplate("Module Hostname", "wifi-hostname", escape(cfg.wifi.hostname), "ESP8266", sizeof(cfg.wifi.hostname)-1);
  
  html += groupTemplate("WiFi", groupContent);
  
  if (wifiReady) {
    groupContent = "";
    groupContent += textTemplate("JSON Server Port", "ports-json", escape(cfg.ports.jsonServer), "19444", 5);
    groupContent += textTemplate("UDP LED Port", "ports-udp", escape(cfg.ports.udpLed), "19446", 5);
    html += groupTemplate("Ports", groupContent);
    groupContent = "";
    
    _chipsets = new LinkedList<SelectEntryBase*>();
    getChipsets(cfg.led.chipset, _chipsets);
    groupContent += selectTemplate("LED Chipset", "led-chipset", _chipsets);
    clearLinkedList(_chipsets);

    _rgbOrder = new LinkedList<SelectEntryBase*>();
    getRgbOrder(cfg.led.colorOrder, _rgbOrder);
    groupContent += selectTemplate("LED Color Order", "led-colorOrder", _rgbOrder);
    clearLinkedList(_rgbOrder);

    _dataPins = new LinkedList<SelectEntryBase*>();
    getAllPins(cfg.led.dataPin, _dataPins);
    groupContent += selectTemplate("LED Data Pin", "led-dataPin", _dataPins);
    clearLinkedList(_dataPins);

    _clockPins = new LinkedList<SelectEntryBase*>();
    getAllPins(cfg.led.clockPin, _clockPins);
    groupContent += selectTemplate("LED Clock Pin", "led-clockPin", _clockPins);
    clearLinkedList(_clockPins);
    
    groupContent += textTemplate("LED Count", "led-count", escape(cfg.led.count), "", 5);

    _idleModes = new LinkedList<SelectEntryBase*>();
    getIdleModes(cfg.led.idleMode, _idleModes);
    groupContent += selectTemplate("LED Idle Mode", "led-idleMode", _idleModes);
    clearLinkedList(_idleModes);
    
    html += groupTemplate("LEDs", groupContent);
    
  } else {
    html += "<div class=\"form-group\">";
    html += "More settings visible, when wifi-connection is ready (internet needed for design)";
    html += "</div>";
  }
  
  html += "<div class=\"form-group\">";
  html +=   "<label class=\"col-md-4 control-label\" for=\"save\"></label>";
  html +=   "<div class=\"col-md-4\">";
  html +=     "<button id=\"save\" name=\"save\" class=\"btn btn-primary\">Save Settings</button>";
  html +=   "</div>";
  html += "</div>";
  
  html += "<div class=\"form-group\">";
  html +=   "<label class=\"col-md-4 control-label\" for=\"saveRestart\"></label>";
  html +=   "<div class=\"col-md-4\">";
  html +=     "<button id=\"saveRestart\" name=\"saveRestart\" class=\"btn btn-primary\">Save &amp; restart</button>";
  html +=   "</div>";
  html += "</div>";
  
  html += "</fieldset>";
  html += "</form>";

  return html;
}

void WrapperWebconfig::initHelperVars(void) {
  ConfigStruct cfg = Config::getConfig();

  _chipsets = new LinkedList<SelectEntryBase*>();
  _rgbOrder = new LinkedList<SelectEntryBase*>();
  _dataPins = new LinkedList<SelectEntryBase*>();
  _clockPins = new LinkedList<SelectEntryBase*>();
  _idleModes = new LinkedList<SelectEntryBase*>();
  
  getChipsets(cfg.led.chipset, _chipsets);
  getRgbOrder(cfg.led.colorOrder, _rgbOrder);
  getAllPins(cfg.led.dataPin, _dataPins);
  getAllPins(cfg.led.clockPin, _clockPins);
  getIdleModes(cfg.led.idleMode, _idleModes);
}
void WrapperWebconfig::clearHelperVars(void) {
  clearLinkedList(_chipsets);
  clearLinkedList(_rgbOrder);
  clearLinkedList(_dataPins);
  clearLinkedList(_clockPins);
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

void WrapperWebconfig::getChipsets(uint8_t active, LinkedList<SelectEntryBase*>* target) {
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("spi-LPD8806", "LPD8806", active == SPI_LPD8806, SPI_LPD8806));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("spi-WS2801", "WS2801", active == SPI_WS2801, SPI_WS2801));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("spi-WS2803", "WS2803", active == SPI_WS2803, SPI_WS2803));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("spi-SM16716", "SM16716", active == SPI_SM16716, SPI_SM16716));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("spi-P9813", "P9813", active == SPI_P9813, SPI_P9813));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("spi-APA102", "APA102", active == SPI_APA102, SPI_APA102));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("spi-DOTSTAR", "DOTSTAR", active == SPI_DOTSTAR, SPI_DOTSTAR));
  /*
  target->add((SelectEntryBase*) new SelectEntry<NEOPIXEL>("clockless-NEOPIXEL","NEOPIXEL", false, NEOPIXEL));
  target->add((SelectEntryBase*) new SelectEntry<TM1829>("clockless-TM1829","TM1829", false, TM1829));
  target->add((SelectEntryBase*) new SelectEntry<TM1812>("clockless-TM1812","TM1812", false, TM1812));
  target->add((SelectEntryBase*) new SelectEntry<TM1809>("clockless-TM1809","TM1809", false, TM1809));
  target->add((SelectEntryBase*) new SelectEntry<TM1804>("clockless-TM1804","TM1804", false, TM1804));
  target->add((SelectEntryBase*) new SelectEntry<TM1803>("clockless-TM1803","TM1803", false, TM1803));
  target->add((SelectEntryBase*) new SelectEntry<UCS1903>("clockless-UCS1903","UCS1903", false, UCS1903));
  target->add((SelectEntryBase*) new SelectEntry<UCS1903B>("clockless-UCS1903B","UCS1903B", false, UCS1903B));
  target->add((SelectEntryBase*) new SelectEntry<UCS1904>("clockless-UCS1904","UCS1904", false, UCS1904));
  target->add((SelectEntryBase*) new SelectEntry<UCS2903>("clockless-UCS2903","UCS2903", false, UCS2903));
  target->add((SelectEntryBase*) new SelectEntry<WS2812>("clockless-WS2812","WS2812", false, WS2812));
  target->add((SelectEntryBase*) new SelectEntry<WS2812B>("clockless-WS2812B","WS2812B", false, WS2812B));
  target->add((SelectEntryBase*) new SelectEntry<SK6812>("clockless-SK6812","SK6812", false, SK6812));
  target->add((SelectEntryBase*) new SelectEntry<SK6822>("clockless-SK6822","SK6822", false, SK6822));
  target->add((SelectEntryBase*) new SelectEntry<PL9823>("clockless-PL9823","PL9823", false, PL9823));
  target->add((SelectEntryBase*) new SelectEntry<WS2811>("clockless-WS2811","WS2811", false, WS2811));
  target->add((SelectEntryBase*) new SelectEntry<APA104>("clockless-APA104","APA104", false, APA104));
  target->add((SelectEntryBase*) new SelectEntry<WS2811_400>("clockless-WS2811_400","WS2811_400", false, WS2811_400));
  target->add((SelectEntryBase*) new SelectEntry<GW6205>("clockless-GW6205","GW6205", false, GW6205));
  target->add((SelectEntryBase*) new SelectEntry<GW6205_400>("clockless-GW6205_400","GW6205_400", false, GW6205_400));
  target->add((SelectEntryBase*) new SelectEntry<LPD1886>("clockless-LPD1886","LPD1886", false, LPD1886));*/
}

void WrapperWebconfig::getAllPins(uint8_t active, LinkedList<SelectEntryBase*>* target) {
  //target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D0", "D0 (LED)", active == D0, D0));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D1", "D1", active == D1, D1));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D2", "D2", active == D2, D2));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D3", "D3", active == D3, D3));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D4", "D4", active == D4, D4));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D5", "D5", active == D5, D5));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D6", "D6", active == D6, D6));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D7", "D7", active == D7, D7));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D8", "D8", active == D8, D8));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D9", "D9 (RX)", active == D9, D9));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("D10", "D10 (TX)", active == D10, D10));
}

void WrapperWebconfig::getRgbOrder(uint8_t active, LinkedList<SelectEntryBase*>* target) {
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("RGB", "RGB", active == RGB, RGB));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("RBG", "RBG", active == RBG, RBG));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("GBR", "GBR", active == GBR, GBR));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("GRB", "GRB", active == GRB, GRB));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("BRG", "BRG", active == BRG, BRG));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("BGR", "BGR", active == BGR, BGR));
}
void WrapperWebconfig::getIdleModes(uint8_t active, LinkedList<SelectEntryBase*>* target) {
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Rainbow", "Rainbow", active == RAINBOW, RAINBOW));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Static", "Static color", active == STATIC_COLOR, STATIC_COLOR));
  target->add((SelectEntryBase*) new SelectEntry<uint8_t>("Off", "Off", active == OFF, OFF));
}
#endif
