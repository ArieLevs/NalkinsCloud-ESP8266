
// ########### ADMIN PAGE START ###########
const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<strong>Administration</strong>
<hr>
<a href="generalSettings" style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="configNetwork" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="info" style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="restart" style="width:250px"  class="btn btn--m btn--blue" onclick="if(!confirm('Are you sure you want to restart the device?')) return false;">Restart Device</a><br>

<script>
window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        // Do something after load...
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";
// ########### ADMIN PAGE END ###########

// ########### GENERAL CONFIG PAGE START ###########
const char PAGE_GeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s">Back</a>&nbsp;&nbsp;<strong>General Settings</strong>

<hr>
<form action="generalSaved" method="post">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr>
  <td align="right">Customer Username</td>
  <td><input type="text" id="username" name="username" value=""></td>
</tr>
<tr>
  <td align="right">Device Password</td>
  <td><input type="text" id="devicepassword" name="devicepassword" value=""></td>
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>

<hr>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<p align="center">Note - Below values cannot be changed</p>
<tr>
  <td align="right">Version</td>
  <td><input type="text" id="version" name="version" value=""></td>
</tr>
<tr>
  <td align="right">Device Model</td>
  <td><input type="text" id="model" name="model" value=""></td>
</tr>
<tr>
  <td align="right">Device Id</td>
  <td><input type="text" id="deviceid" name="deviceid" value=""></td>
</tr>
</table>

<script>
 
window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        setValues("/admin/generalvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>
)=====";

// Called when 'PAGE_GeneralSettings' is built (/)
// Will send all relevant info to be displayed on page
void webServerGetGeneralConfigurationValues()
{
  //configs.clientUsername = readStringFromEEPROM(USERNAMESTARTADDR);
  configs.devicePassword = readStringFromEEPROM(DEVICEPASSSTARTADDR); // Read device password from EEPROM and store to 'configs' struct
  String values ="";
  values += "version|" +  (String)  versionNum +  "|input\n";
  values += "model|" +  (String)  chipType +  "|input\n";
  values += "deviceid|" +  (String)  deviceId +  "|input\n";
  //values += "username|" +  (String)  configs.clientUsername +  "|input\n";
  values += "devicepassword|" +  (String)  configs.devicePassword +  "|input\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
}
// ########### GENERAL CONFIG PAGE END ###########

// ########### GENERAL CONFIG SAVE PAGE START ###########
const char PAGE_GeneralSettingsSaved[] PROGMEM = R"=====(
  <script>
    window.onload = function ()
    {
      alert("Client namd And Device password saved");
      window.location = "/";
    }
  </script>
)=====";
// ########### GENERAL CONFIG SAVE PAGE END ###########

// ########### INFORMATION PAGE START ###########
const char PAGE_Information[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="/"  class="btn btn--s">Back</a>&nbsp;&nbsp;<strong>Network Information</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID :</td><td><span id="x_ssid"></span></td></tr>
<tr><td align="right">IP :</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Netmask :</td><td><span id="x_netmask"></span></td></tr>
<tr><td align="right">Gateway :</td><td><span id="x_gateway"></span></td></tr>
<tr><td align="right">Mac :</td><td><span id="x_mac"></span></td></tr>

<tr><td colspan="2"></span></td></tr>
<strong>WiFi Connection State:</strong><div id="wificonnectionstate">N/A</div>
<tr><td colspan="2" align="center"><a href="javascript:GetNetworkState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
<tr><td colspan="2" align="center"><a href="javascript:ReconnectToWifi()" class="btn btn--m btn--blue">Reconnect</a></td></tr>
</table>

<hr>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Server :</td><td><span id="mqttServer"></span></td></tr>
<tr><td align="right">Port :</td><td><span id="mqttport"></span></td></tr>
<tr><td align="right">ID :</td><td><span id="mqttdeviceid"></span></td></tr>
<tr><td align="right">Pass :</td><td><span id="mqttpassword"></span></td></tr>

<tr><td colspan="2"></span></td></tr>
<strong>MQTT Connection State:</strong><div id="mqttconnectionstate">N/A</div>
</table>

<br>

<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Fingerprint :</td><td><span id="fingerprint"></span></td></tr>

<tr><td colspan="2"></span></td></tr>
<strong>SSL Connection:</strong><div id="sslconnection">N/A</div>
<tr><td colspan="2" align="center"><a href="javascript:GetMQTTState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>

<script>
function GetNetworkState() {
  setValues("/admin/infovalues"); // Call webServerGetNetworkInfo() function
}
function ReconnectToWifi() {
  setValues("/admin/wifireconnect"); // Call reconnect_to_wifi() function
}
function GetMQTTState() {
  setValues("/admin/mqttinfovalues");
}

window.onload = function () {
  GetNetworkState();
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
       GetMQTTState(); // Call webServerGetMqttInfo() function
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
</script>
)=====" ;


// Called when 'PAGE_Information' is built
// Will send all relevant info to be displayed on page
void webServerGetNetworkInfo() {
  connectToWifi(); // Try connecting to wifi
  String state = "N/A";
  if (WiFi.status() == 0) state = "Idle";
  else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
  else if (WiFi.status() == 2) state = "SCAN COMPLETED";
  else if (WiFi.status() == 3) state = "CONNECTED";
  else if (WiFi.status() == 4) state = "CONNECT FAILED";
  else if (WiFi.status() == 5) state = "CONNECTION LOST";
  else if (WiFi.status() == 6) state = "DISCONNECTED";
  
  String values ="";

  values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
  values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
  values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
  values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
  values += "x_mac|" + macToStr(getMacAddress()) +  "|div\n";
  values += "wificonnectionstate|" +  state + "|div\n";
  server.send ( 200, "text/plain", values);
  if (DEBUG)
    Serial.println(__FUNCTION__); 
}

void webServerReconnectToWifi() {
  reconnectToWifi();
  webServerGetNetworkInfo();
}

// Called when 'PAGE_Information' is built
// Will send all relevant info to be displayed on page
void webServerGetMqttInfo() {
  String mqttState = "N/A";
  String sslState = "N/A";
  if (connectToMQTTBroker()) { //Try to connect to MQTT server
    mqttState = "CONNECTED";
    if (checkMQTTSSL()) //Try to verify MQTT server
      sslState = "VERIFIED";
    else
      sslState = "NOT SECURED!!!";
  }
  else
    mqttState = "DISCONNECTED";
  mqttClient.disconnect(); //Disconnect from the MQTT server connected before
  
  String values ="";
  values += "mqttServer|" + (String)configs.mqttServer +  "|div\n";
  values += "mqttport|" +  (String)configs.mqttPort +  "|div\n";
  values += "mqttdeviceid|" +  deviceId +  "|div\n";
  values += "mqttpassword|" +  (String)configs.devicePassword +  "|div\n";
  values += "mqttconnectionstate|" +  mqttState + "|div\n";
  values += "fingerprint|" + (String)fingerprint +  "|div\n";
  values += "sslconnection|" +  sslState + "|div\n";
  server.send ( 200, "text/plain", values);
  if (DEBUG)
    Serial.println(__FUNCTION__); 
}
// ########### INFORMATION PAGE END ###########

// ########### Network Configuration PAGE START ###########
const char PAGE_NetworkConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s">Back</a>&nbsp;&nbsp;<strong>Network Configuration</strong>
<hr>
Connect to Router with these settings:<br>
<form action="configSaved" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID:</td><td><input type="text" id="ssid" name="ssid" value=""></td></tr>
<tr><td align="right">Password:</td><td><input type="text" id="password" name="password" value=""></td></tr>
<tr><td align="right">DHCP: </td><td><input type="checkbox" checked="checked" id="dhcp" name="dhcp"></td></tr>
<tr><td align="right">Optional: </td><td>fill below details if DHCP is NOT selected</td></tr>
<tr><td align="right">IP:     </td><td><input type="text" id="ip_0" name="ip_0" size="3">.<input type="text" id="ip_1" name="ip_1" size="3">.<input type="text" id="ip_2" name="ip_2" size="3">.<input type="text" id="ip_3" name="ip_3" size="3"></td></tr>
<tr><td align="right">Netmask:</td><td><input type="text" id="nm_0" name="nm_0" size="3">.<input type="text" id="nm_1" name="nm_1" size="3">.<input type="text" id="nm_2" name="nm_2" size="3">.<input type="text" id="nm_3" name="nm_3" size="3"></td></tr>
<tr><td align="right">Gateway:</td><td><input type="text" id="gw_0" name="gw_0" size="3">.<input type="text" id="gw_1" name="gw_1" size="3">.<input type="text" id="gw_2" name="gw_2" size="3">.<input type="text" id="gw_3" name="gw_3" size="3"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<hr>
<strong>Connection State:</strong><div id="wificonnectionstate">N/A</div>
<hr>
<strong>Networks:</strong><br>
<table border="0"  cellspacing="3" style="width:310px" >
<tr><td><div id="networks">Scanning...</div></td></tr>
<tr><td align="center"><a href="javascript:GetState()" style="width:150px" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>

<script>

function GetState() { // Function will return avilable networks
  setValues("/admin/getnetworks"); // This page will start the webServerGetAvailableNetworks() function
}
function selssid(value) {
  document.getElementById("ssid").value = value; 
}

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
          setTimeout(GetState,3000);
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
</script>
)=====";

/**
 * Update network states
 */
void webServerGetAvailableNetworks()
{
  String state = "N/A";
  String Networks = "";
  if (WiFi.status() == 0) state = "Idle";
  else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
  else if (WiFi.status() == 2) state = "SCAN COMPLETED";
  else if (WiFi.status() == 3) state = "CONNECTED";
  else if (WiFi.status() == 4) state = "CONNECT FAILED";
  else if (WiFi.status() == 5) state = "CONNECTION LOST";
  else if (WiFi.status() == 6) state = "DISCONNECTED";
  Serial.println("Scanning networks");
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
   Networks = "<font color='#FF0000'>No networks found!</font>";
  } else { 
    Networks = "Found " +String(n) + " Networks<br>";
    Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
    Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
    for (int i = 0; i < n; ++i) {
      if (DEBUG)
        Serial.println("discovered network: " + String(WiFi.SSID(i)));
      int quality=0;
      if(WiFi.RSSI(i) <= -100) {
          quality = 0;
      } else if(WiFi.RSSI(i) >= -50) {
          quality = 100;
      } else {
        quality = 2 * (WiFi.RSSI(i) + 100);
      }
    
      Networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*")  + "</td></tr>";
    }
    Networks += "</table>";
  }
   
  String values ="";
  values += "wificonnectionstate|" +  state + "|div\n";
  values += "networks|" +  Networks + "|div\n";
  server.send (200, "text/plain", values);
  if (DEBUG)
    Serial.println(__FUNCTION__); 
}
// ########### Network Configuration PAGE END ###########

// ########### Network CONF SAVE PAGE START ###########
const char PAGE_NetworkConfigurationSaved[] PROGMEM = R"=====(
  <script>
    window.onload = function ()
    {
      setTimeout(function(){ alert("Configurations saved, Please check connection"); }, 10000);
      window.location = "/info";
    }
  </script>
)=====";
// ########### Network CONF SAVE PAGE END ###########

// ########### RESTART PAGE START ###########
const char PAGE_Restart[] PROGMEM = R"=====(
  <script>
    window.onload = function ()
    {
      //alert("Device will now restart");
      confirm("Are you sure you want to restart the device?");
      window.location = "/";
    }
  </script>
)=====" ;
// ########### RESTART PAGE END ###########

// ########### STYLE PAGE START ###########
const char PAGE_Style_css[] PROGMEM = R"=====(
body { color: #000000; font-family: avenir, helvetica, arial, sans-serif;  letter-spacing: 0.15em;} 
hr {    background-color: #eee;    border: 0 none;   color: #eee;    height: 1px; } 
.btn, .btn:link, .btn:visited {  
  border-radius: 0.3em;  
  border-style: solid;  
  border-width: 1px;  
color: #111;  
display: inline-block;  
  font-family: avenir, helvetica, arial, sans-serif;  
  letter-spacing: 0.15em;  
  margin-bottom: 0.5em;  
padding: 1em 0.75em;  
  text-decoration: none;  
  text-transform: uppercase;  
  -webkit-transition: color 0.4s, background-color 0.4s, border 0.4s;  
transition: color 0.4s, background-color 0.4s, border 0.4s; 
} 
.btn:hover, .btn:focus {
color: #7FDBFF;  
border: 1px solid #7FDBFF;  
  -webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
transition: background-color 0.3s, color 0.3s, border 0.3s; 
}
  .btn:active {  
color: #0074D9;  
border: 1px solid #0074D9;  
    -webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
transition: background-color 0.3s, color 0.3s, border 0.3s; 
  } 
  .btn--s 
  {  
    font-size: 12px; 
  }
  .btn--m { 
    font-size: 14px; 
  }
  .btn--l {  
    font-size: 20px;  border-radius: 0.25em !important; 
  } 
  .btn--full, .btn--full:link {
    border-radius: 0.25em; 
display: block;  
      margin-left: auto; 
      margin-right: auto; 
      text-align: center; 
width: 100%; 
  } 
  .btn--blue:link, .btn--blue:visited {
color: #fff;  
    background-color: #0074D9; 
  }
  .btn--blue:hover, .btn--blue:focus {
color: #fff !important;  
    background-color: #0063aa;  
    border-color: #0063aa; 
  }
  .btn--blue:active {
color: #fff; 
    background-color: #001F3F;  border-color: #001F3F; 
  }
  @media screen and (min-width: 32em) {
    .btn--full {  
      max-width: 16em !important; } 
  }
)=====";
// ########### STYLE PAGE END ###########

// ########### microajax_js PAGE START ###########
const char PAGE_microajax_js[] PROGMEM = R"=====(
function microAjax(B,A){this.bindFunction=function(E,D){return function(){return E.apply(D,[D])}};this.stateChange=function(D){if(this.request.readyState==4){this.callbackFunction(this.request.responseText)}};this.getRequest=function(){if(window.ActiveXObject){return new ActiveXObject("Microsoft.XMLHTTP")}else{if(window.XMLHttpRequest){return new XMLHttpRequest()}}return false};this.postBody=(arguments[2]||"");this.callbackFunction=A;this.url=B;this.request=this.getRequest();if(this.request){var C=this.request;C.onreadystatechange=this.bindFunction(this.stateChange,this);if(this.postBody!==""){C.open("POST",B,true);C.setRequestHeader("X-Requested-With","XMLHttpRequest");C.setRequestHeader("Content-type","application/x-www-form-urlencoded");C.setRequestHeader("Connection","close")}else{C.open("GET",B,true)}C.send(this.postBody)}};

function setValues(url)
{
  microAjax(url, function (res)
  {
    res.split(String.fromCharCode(10)).forEach(function(entry) {
    fields = entry.split("|");
    if(fields[2] == "input")
    {
        document.getElementById(fields[0]).value = fields[1];
    }
    else if(fields[2] == "div")
    {
        document.getElementById(fields[0]).innerHTML  = fields[1];
    }
    else if(fields[2] == "chk")
    {
        document.getElementById(fields[0]).checked  = fields[1];
    }
    });
  });
}
)=====";
// ########### microajax_js PAGE END ###########

