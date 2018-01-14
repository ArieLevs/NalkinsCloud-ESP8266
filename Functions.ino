
#include <ArduinoJson.h>

/**
 * Starts HTTP server pages with relevant page content
 * Function provides an HTML content menu, and acts as an API to handle requests
 */
void startHTTPServer() {
  IPAddress mdnsIP = WiFi.localIP();
  
  //Start mDNS service with the relevant IP
  if (mdns.begin(DNSName.c_str(), mdnsIP)) {
    if (DEBUG) {
      Serial.println("MDNS started successfully");
      Serial.println("Please use this URL to connect:");
      Serial.print("http://");
      Serial.print(DNSName.c_str());
      Serial.println("/");
    }
  }
  else
    if (DEBUG)
      Serial.println("MDNS failed to start");

  /*
   * Once http://DEVICEIP/autoconfig is accessed it will try to parse a json syntax request
   * The request should contain the wifi SSID and password, device password for connecting to MQTT broker, brokers IP/Domain address and port
   * If all conditions are met, store received data to EEPROM storage
   */
  server.on("/autoconfig", HTTP_POST, [](){
    if (DEBUG)
       Serial.println("/autoconfig was accessed");
    StaticJsonBuffer<1024> newBuffer;
    // Get the request JSON body
    JsonObject& jsonmsg = newBuffer.parseObject(server.arg("plain"));
    jsonmsg.prettyPrintTo(wifiClientSecure);
    if (DEBUG) {
      Serial.println("Message received:");
      Serial.println(server.arg("plain"));
    }
    // If message parse failed
    if (!jsonmsg.success()) {
      if (DEBUG)
        Serial.println("Parsing failed");
      server.send(200, "text/plain", "{\"status\":\"failed\",\"message\":\"message contains error(s), check json syntax\"}");
    } else {
      // Get JSON to global variables
      configs.wifiSsid = (const char*)jsonmsg["ssid"]; // Get SSID from JSON
      configs.wifiPassword = (const char*)jsonmsg["wifi_pass"];
      //configs.clientUsername = (const char*)jsonmsg["device_user"];
      configs.devicePassword = (const char*)jsonmsg["device_pass"];
      configs.mqttServer = (const char*)jsonmsg["mqtt_server"];
      configs.mqttPort = (const char*)jsonmsg["mqtt_port"];
      //configs.mqttFingerprint = (const char*)jsonmsg["server_fingerprint"];
      
      if (DEBUG) {
        Serial.print("wifiSsid set: ");
        Serial.println(configs.wifiSsid);
        Serial.print("wifiPassword set: ");
        Serial.println(configs.wifiPassword);
        Serial.print("MQTT Server set: ");
        Serial.println(configs.mqttServer);
        Serial.print("MQTT Port set: ");
        Serial.println(configs.mqttPort);
        //Serial.print("MQTT Fingerprint set: ");
        //Serial.println(configs.mqttFingerprint);
        //Serial.print("clientUsername set: ");
        //Serial.println(configs.clientUsername);
        Serial.print("devicePassword set: ");
        Serial.println(configs.devicePassword);
        Serial.println("Runnig connection check from autoconfig");
      }
      // Connect to received wifi SSID and password 
      if (connectToWifi()) {
        if (DEBUG)
          Serial.println("Wifi connection successful, Writing wifi configs to EEPROM");
        // If successful store wifi credentials
        writeStringToEEPROM(SSIDSTARTSTARTADDR, configs.wifiSsid);
        writeStringToEEPROM(WIFIPASSSTARTADDR, configs.wifiPassword);
        
        if (DEBUG)
          Serial.println("Writing MQTT config ro EEPROM");
        // Store MQTT server info and credentials
        writeStringToEEPROM(MQTTSERVERSTARTADDR, configs.mqttServer);
        writeStringToEEPROM(MQTTPORTSTARTADDR, configs.mqttPort);
        // writeStringToEEPROM(USERNAMESTARTADDR, configs.clientUsername); //Save customers user name to flash memory
        writeStringToEEPROM(DEVICEPASSSTARTADDR, configs.devicePassword); //Save the password to flash memory
  
        server.send(200, "text/plain", "{\"status\":\"success\",\"message\":\"successfully connected to wifi\"}");
        if (DEBUG)
          Serial.println("Autoconfig connection was successful, Restaring device");
        ESP.restart();
      }
      server.send(200, "text/plain", "{\"status\":\"failed\",\"message\":\"could not connect to wifi\"}");
      if (DEBUG)
        Serial.println("Autoconfig connection failed on WiFi connection");
    }
  });

  
  /*
   * Once http://DEVICEIP/returnid is accessed it will return devices ID and TYPE
   */
  server.on("/returnid", HTTP_POST, [](){
    if (DEBUG)
       Serial.println("JSON returnid initiated");
    StaticJsonBuffer<256> newBuffer;
    // Get the request JSON body
    JsonObject& jsonmsg = newBuffer.parseObject(server.arg("plain"));
    jsonmsg.prettyPrintTo(wifiClientSecure);
    if (DEBUG) {
      Serial.println("Message received:");
      Serial.println(server.arg("plain"));
    }
    // If message parse failed
    if (!jsonmsg.success()) {
      if (DEBUG)
        Serial.println("Parsing failed");
      server.send(200, "text/plain", "{\"status\":\"failed\",\"message\":\"message contains error(s), check json syntax\"}");
      if (DEBUG)
        Serial.println("/returnid returned error");
    } else {
      server.send(200, "text/plain", "{\"status\":\"success\", \"device_id\":\"" + deviceId + "\", \"device_type\":\"" + deviceType + "\"}");
      if (DEBUG)
        Serial.println("/returnid returned success");
    }
    delay(500);
  });
  
  /*
   * Once http://DEVICEIP/ is accessed it will return HTML code, with main menu
   */
  server.on ( "/", []() { //Main menu page
    if (DEBUG)
      Serial.println("Return admin.html page"); 
    server.send( 200, "text/html", PAGE_AdminMainPage );
  });
  
  server.on( "/generalSettings", []() {  //General configs, will show device ID, version etc and allow storing new device password
    if (DEBUG)
      Serial.println("Return generalSettings.html page"); 
    server.send ( 200, "text/html", PAGE_GeneralSettings );   
  });
  
  server.on( "/generalSaved", []() { // Small redirect page that indicates that general configs been saved
    if (DEBUG)
      Serial.println("Running generalSaved.html page");
    if ((server.arg("devicepassword").length() == 8) && (server.arg("username").length() <= 16)) { // If Password is in the length of 8 chars
      writeStringToEEPROM(USERNAMESTARTADDR, server.arg("username")); // Save customers user name to EEPROM flash memory
      writeStringToEEPROM(DEVICEPASSSTARTADDR, server.arg("devicepassword")); //Save the password to EEPROM flash memory
      if (DEBUG)
        Serial.println("User name and Device Password written to flash memory");
      server.send( 200, "text/html", PAGE_GeneralSettingsSaved ); //And display small success redirect page
    }
    else // If password length not 8 chars just go back to main menu
      if (DEBUG)
        Serial.println("User name or Device Password are invalid - return to main menu page");
      server.send( 200, "text/html", PAGE_AdminMainPage ); // TODO ############# CONSIDER WRITING GENERAL ERROR PAGE OR DO JAVA SCRIPT VALIDATION #############
  });
  
  server.on( "/configNetwork", []() {
    if (DEBUG)
      Serial.println("Running configNetwork.html page"); 
    server.send( 200, "text/html", PAGE_NetworkConfiguration );   
  });
  
  server.on( "/configSaved", []() { 
    if (DEBUG)
      Serial.println("Running configSaved.html page");
    for ( uint8_t i = 0; i < server.args(); i++ ) { // For each value from network configs page, run on each one and save to 'configs' struct
      if (server.argName(i) == "ssid") configs.wifiSsid = server.arg(i);
      if (server.argName(i) == "password") configs.wifiPassword = server.arg(i); 
      if (server.argName(i) == "ip_0") if (checkRange(server.arg(i)))   configs.IP[0] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_1") if (checkRange(server.arg(i)))   configs.IP[1] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_2") if (checkRange(server.arg(i)))   configs.IP[2] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_3") if (checkRange(server.arg(i)))   configs.IP[3] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_0") if (checkRange(server.arg(i)))   configs.Netmask[0] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_1") if (checkRange(server.arg(i)))   configs.Netmask[1] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_2") if (checkRange(server.arg(i)))   configs.Netmask[2] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_3") if (checkRange(server.arg(i)))   configs.Netmask[3] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_0") if (checkRange(server.arg(i)))   configs.Gateway[0] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_1") if (checkRange(server.arg(i)))   configs.Gateway[1] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_2") if (checkRange(server.arg(i)))   configs.Gateway[2] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_3") if (checkRange(server.arg(i)))   configs.Gateway[3] =  server.arg(i).toInt();
      
      if (server.argName(i) == "dhcp") {
        if (server.arg(i))
          configs.dhcp = true;
        else
          configs.dhcp = false;
      }
    }
    writeNetworkConfigs(); // Write network configurations to EEPROM flash memory
    server.send( 200, "text/html", PAGE_NetworkConfigurationSaved ); // Display small success redirect page
    WiFi.disconnect();
    if (connectToWifi()) // While still inside configuration menu, try to reconnect to wifi with new parameters
      if (DEBUG)
        Serial.println("configSaved page connected to wifi");
  });

  /**
   * Once http://DEVICEIP/info is accessed it will return information page containing
   * Wifi connection status, SSID, IP, Netmask, Gateway and mac address
   */
  server.on( "/info", []() { 
    Serial.println("info.html"); 
    server.send ( 200, "text/html", PAGE_Information );   
  });

  /**
   * Once http://DEVICEIP/restart is accessed it will restart the device
   */
  server.on( "/restart", []() { 
    Serial.println("restart.html"); 
    server.send ( 200, "text/html", PAGE_Restart );
    ESP.restart();
  });
  
  server.on( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );

  /**
   * Bellow url calls are made from HTTP pages 
   * When client is to execute a function that should be run from the HTTP server (browser)
   * The page is being redirected to the below URLs, each url will run the relevant function
   */
  server.on( "/admin/getnetworks", webServerGetAvailableNetworks );
  server.on( "/admin/infovalues", webServerGetNetworkInfo );
  server.on( "/admin/wifireconnect", webServerReconnectToWifi );
  server.on( "/admin/mqttinfovalues", webServerGetMqttInfo );
  server.on( "/admin/generalvalues", webServerGetGeneralConfigurationValues);
  
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
  
  server.begin(); // Start the HTTP server
  
  if (DEBUG)
    Serial.println("HTTP Server Started");
}


/**
 * Convert mac address uint8_t to String
 * 
 * @param mac MAC address
 * 
 * @return String the string value of the MAC address
 */
String macToStr(uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


/*
 * Return devices mac address 
 */
uint8_t* getMacAddress() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  return mac;
}


/**
 * Compare two strings (char arrays)
 * 
 * @param s1 string
 * @param s2 string
 *  
 * @return boolean  true - both strings are equal
 *                  false - string not equal
 */
boolean areEqual(const char *s1, const char *s2) {
  if (sizeof(s1) != sizeof(s2)) // If strings size
    return 0; // Strings must be different
  for (unsigned int i = 0; i < sizeof(s1); i++) {
    if (s1[i] != s2[i])
      return false;  // Strings are different
  }
  return true;  // Strings must be the same
}


// Set time to "delay" configuration button press
unsigned long previousConfButton = 0;
const long ConfButtonInterval = 1000; // interval at which to send message (milliseconds)
int confButtonCountDown = 5; // 5 seconds

//
/**
 * Check if user is pressing the configuration button for more then 5 seconds
 * In case (CONFIGURATIONMODEBUTTON) pressed more then 5 seconds, restart the device
 */
void checkConfigurationButton() {
  // Take current timestamp
  unsigned long currentMillis = millis();
  if(currentMillis - previousConfButton >= ConfButtonInterval) {
    previousConfButton = currentMillis;
    
    int configuration_button = digitalRead(CONFIGURATIONMODEBUTTON); // Read button state
    if(configuration_button == HIGH) { // If button is pressed then
      confButtonCountDown--; 
      if (DEBUG) {
        Serial.print("configuration_button counter: ");
        Serial.println(confButtonCountDown);
      }
      if(confButtonCountDown <= 0) { // If button was pressed for 5 seconds in a row
        setConfigurationStatusFlag(1); // Set configuration mode to 1 (true)
        ESP.restart(); // And restart the device
      }
    } else {
      confButtonCountDown = 5; // If button released before 5 seconds passed, reset the counter
    }
  }
}



