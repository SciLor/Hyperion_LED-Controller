#include "WrapperWebconfig.h"

void WrapperWebconfig::begin() {
  _server = ESP8266WebServer(_port);
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
  String message = htmlTemplate("ESP8266 LED Configuration", WrapperWebconfig::config());
  _server.send(200, "text/html", message);
}

String WrapperWebconfig::escape(String text) {
  return text;
}
String WrapperWebconfig::escape(char* text) {
  return String(text);
}
String WrapperWebconfig::escape(uint8_t text) {
  char buf[4];
  sprintf (buf, "%u", text);
  return String(buf);  
}
String WrapperWebconfig::escape(uint16_t text) {
  char buf[6];
  sprintf (buf, "%u", text);
  return String(buf);  
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

String WrapperWebconfig::config(void) {
  String html = "";
  boolean wifiReady = true;
  
  html += "<form class=\"form-horizontal\" method=\"post\">";
  html += "<fieldset>";

  html += "<legend>ESP8266 LED Coniguration</legend>";

  html += "<div class=\"panel panel-default\">";
  html += "  <div class=\"panel-heading\">WiFi</div>";
  html += "  <div class=\"panel-body\">";

  html += "<div class=\"form-group\">";
  html += "  <label class=\"col-md-4 control-label\" for=\"wifi-ssid\">WiFi SSID</label>";
  html += "  <div class=\"col-md-4\">";
  html += "  <input id=\"wifi-ssid\" name=\"wifi-ssid\" type=\"text\" placeholder=\"\" class=\"form-control input-md\" value=\"" + WrapperWebconfig::escape(Config::ssid) + "\">";
  html += "  </div>";
  html += "</div>";

  html += "<div class=\"form-group\">";
  html += "  <label class=\"col-md-4 control-label\" for=\"wifi-password\">WiFi Password</label>";
  html += "  <div class=\"col-md-4\">";
  html += "    <input id=\"wifi-password\" name=\"wifi-password\" type=\"password\" placeholder=\"\" class=\"form-control input-md\">";
  html += "  </div>";
  html += "</div>";

  html += "<div class=\"form-group\">";
  html += "  <label class=\"col-md-4 control-label\" for=\"staticIp\">Static IP</label>";
  html += "  <div class=\"col-md-4\">";
  html += "    <div class=\"input-group\">";
  html += "      <span class=\"input-group-addon\">";
  html += "          <input type=\"checkbox\">";
  html += "      </span>";
  html += "      <input id=\"staticIp\" name=\"staticIp\" class=\"form-control\" type=\"text\" placeholder=\"192.168.1.123\">";
  html += "    </div>";
  html += "    <p class=\"help-block\">check for static IP</p>";
  html += "  </div>";
  html += "</div>";
  
  html += "<div class=\"form-group\">";
  html += "  <label class=\"col-md-4 control-label\" for=\"subnet\">Subnet</label>";
  html += "  <div class=\"col-md-4\">";
  html += "  <input id=\"subnet\" name=\"subnet\" type=\"text\" placeholder=\"255.255.255.0\" class=\"form-control input-md\">";
  html += "  </div>";
  html += "</div>";
  
  html += "<div class=\"form-group\">";
  html += "  <label class=\"col-md-4 control-label\" for=\"dns\">DNS Server</label>";
  html += "  <div class=\"col-md-4\">";
  html += "  <input id=\"dns\" name=\"dns\" type=\"text\" placeholder=\"192.168.1.1\" class=\"form-control input-md\">";
  html += "  </div>";
  html += "</div>";

  html += "<div class=\"form-group\">";
  html += "  <label class=\"col-md-4 control-label\" for=\"hostname\">Module Hostname</label>";
  html += "  <div class=\"col-md-4\">";
  html += "  <input id=\"hostname\" name=\"hostname\" type=\"text\" placeholder=\"ESP8266\" class=\"form-control input-md\" value=\"" + WrapperWebconfig::escape(Config::host) + "\">";
  html += "  </div>";
  html += "</div>";
  
  html += "  </div>";
  html += "</div>";

  if (wifiReady) {
    html += "<div class=\"panel panel-default\">";
    html += "  <div class=\"panel-heading\">Ports</div>";
    html += "  <div class=\"panel-body\">";
    
    html += "<div class=\"form-group\">";
    html += "  <label class=\"col-md-4 control-label\" for=\"jsonServerPort\">JSON Server Port</label>";
    html += "  <div class=\"col-md-4\">";
    html += "  <input id=\"jsonServerPort\" name=\"jsonServerPort\" type=\"text\" placeholder=\"19444\" class=\"form-control input-md\" value=\"" + WrapperWebconfig::escape(Config::jsonServerPort) + "\">";
    html += "  </div>";
    html += "</div>";
  
    html += "<div class=\"form-group\">";
    html += "  <label class=\"col-md-4 control-label\" for=\"udpLedPort\">UDP LED Port</label>";
    html += "  <div class=\"col-md-4\">";
    html += "  <input id=\"udpLedPort\" name=\"udpLedPort\" type=\"text\" placeholder=\"19446\" class=\"form-control input-md\" value=\"" + WrapperWebconfig::escape(Config::udpLedPort) + "\">";
    html += "  </div>";
    html += "</div>";

    html += "  </div>";
    html += "</div>";
    
    html += "<div class=\"panel panel-default\">";
    html += "  <div class=\"panel-heading\">LEDs</div>";
    html += "  <div class=\"panel-body\">";
    
    html += "<div class=\"form-group\">";
    html += "  <label class=\"col-md-4 control-label\" for=\"chipset\">LED Chipset</label>";
    html += "  <div class=\"col-md-4\">";
    html += "    <select id=\"chipset\" name=\"chipset\" class=\"form-control\">";
    html += "      <option value=\"spi-LPD8806\">LPD8806</option>";
    html += "      <option value=\"spi-WS2801\">WS2801</option>";
    html += "      <option value=\"spi-WS2803\">WS2803</option>";
    html += "      <option value=\"spi-SM16716\">SM16716</option>";
    html += "      <option value=\"spi-P9813\">P9813</option>";
    html += "      <option value=\"spi-APA102\">APA102</option>";
    html += "      <option value=\"spi-DOTSTAR\">DOTSTAR</option>";
    html += "      <option value=\"clockless-NEOPIXEL\">NEOPIXEL</option>";
    html += "      <option value=\"clockless-TM1829\">TM1829</option>";
    html += "      <option value=\"clockless-TM1812\">TM1812</option>";
    html += "      <option value=\"clockless-TM1809\">TM1809</option>";
    html += "      <option value=\"clockless-TM1804\">TM1804</option>";
    html += "      <option value=\"clockless-TM1803\">TM1803</option>";
    html += "      <option value=\"clockless-UCS1903\">UCS1903</option>";
    html += "      <option value=\"clockless-UCS1903B\">UCS1903B</option>";
    html += "      <option value=\"clockless-UCS1904\">UCS1904</option>";
    html += "      <option value=\"clockless-UCS2903\">UCS2903</option>";
    html += "      <option value=\"clockless-WS2812\">WS2812</option>";
    html += "      <option value=\"clockless-WS2812B\">WS2812B</option>";
    html += "      <option value=\"clockless-SK6812\">SK6812</option>";
    html += "      <option value=\"clockless-SK6822\">SK6822</option>";
    html += "      <option value=\"clockless-PL9823\">PL9823</option>";
    html += "      <option value=\"clockless-WS2811\">WS2811</option>";
    html += "      <option value=\"clockless-APA104\">APA104</option>";
    html += "      <option value=\"clockless-WS2811_400\">WS2811_400</option>";
    html += "      <option value=\"clockless-GW6205\">GW6205</option>";
    html += "      <option value=\"clockless-GW6205_400\">GW6205_400</option>";
    html += "      <option value=\"clockless-LPD1886\">LPD1886</option>";
    html += "    </select>";
    html += "  </div>";
    html += "</div>";
  
    html += "<div class=\"form-group\">";
    html += "  <label class=\"col-md-4 control-label\" for=\"colorOrder\">LED Color Order</label>";
    html += "  <div class=\"col-md-4\"> ";
    html += "    <label class=\"radio-inline\" for=\"colorOrder-rgb\">";
    html += "      <input type=\"radio\" name=\"colorOrder\" id=\"colorOrder-rgb\" value=\"RGB\" checked=\"checked\">";
    html += "      RGB";
    html += "    </label> ";
    html += "    <label class=\"radio-inline\" for=\"colorOrder-rbg\">";
    html += "      <input type=\"radio\" name=\"colorOrder\" id=\"colorOrder-rbg\" value=\"RBG\">";
    html += "      RBG";
    html += "    </label> ";
    html += "    <label class=\"radio-inline\" for=\"colorOrder-gbr\">";
    html += "      <input type=\"radio\" name=\"colorOrder\" id=\"colorOrder-gbr\" value=\"GBR\">";
    html += "      GBR";
    html += "    </label> ";
    html += "    <label class=\"radio-inline\" for=\"colorOrder-grb\">";
    html += "      <input type=\"radio\" name=\"colorOrder\" id=\"colorOrder-grb\" value=\"GRB\">";
    html += "      GRB";
    html += "    </label> ";
    html += "    <label class=\"radio-inline\" for=\"colorOrder-brg\">";
    html += "      <input type=\"radio\" name=\"colorOrder\" id=\"colorOrder-brg\" value=\"BRG\">";
    html += "      BRG";
    html += "    </label> ";
    html += "    <label class=\"radio-inline\" for=\"colorOrder-bgr\">";
    html += "      <input type=\"radio\" name=\"colorOrder\" id=\"colorOrder-bgr\" value=\"BGR\">";
    html += "      BGR";
    html += "    </label>";
    html += "  </div>";
    html += "</div>";
  
    html += "<div class=\"form-group\">";
    html += "  <label class=\"col-md-4 control-label\" for=\"ledCount\">LED Count</label>";
    html += "  <div class=\"col-md-4\">";
    html += "  <input id=\"ledCount\" name=\"ledCount\" type=\"text\" placeholder=\"\" class=\"form-control input-md\" value=\"" + WrapperWebconfig::escape(Config::ledCount) + "\">";
    html += "  </div>";
    html += "</div>";
  
    html += "<div class=\"form-group\">";
    html += "  <label class=\"col-md-4 control-label\" for=\"dataPin\">LED Data Pin</label>";
    html += "  <div class=\"col-md-4\">";
    html += "  <input id=\"dataPin\" name=\"dataPin\" type=\"text\" placeholder=\"\" class=\"form-control input-md\" value=\"" + WrapperWebconfig::escape(Config::dataPin) + "\">";
    html += "  </div>";
    html += "</div>";
  
    html += "<div class=\"form-group\">";
    html += "  <label class=\"col-md-4 control-label\" for=\"clockPin\">LED Clock Pin</label>";
    html += "  <div class=\"col-md-4\">";
    html += "  <input id=\"clockPin\" name=\"clockPin\" type=\"text\" placeholder=\"\" class=\"form-control input-md\" value=\"" + WrapperWebconfig::escape(Config::clockPin) + "\">";
    html += "  </div>";
    html += "</div>";
    
    html += "  </div>";
    html += "</div>";
  } else {
    html += "<div class=\"form-group\">";
    html += "More settings visible, when wifi-connection is ready (internet needed for bootstrap)";
    html += "</div>";
  }
  
  html += "<div class=\"form-group\">";
  html += "  <label class=\"col-md-4 control-label\" for=\"save\"></label>";
  html += "  <div class=\"col-md-4\">";
  html += "    <button id=\"save\" name=\"save\" class=\"btn btn-primary\">Save Settings</button>";
  html += "  </div>";
  html += "</div>";
  
  html += "</fieldset>";
  html += "</form>";

  return html;
}

