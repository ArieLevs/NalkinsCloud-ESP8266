//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_HTTP_SERVER_CONTENT_H
#define NALKINSCLOUD_ESP8266_HTTP_SERVER_CONTENT_H


const char PAGE_AdminMainPage[] = R"=====(
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


const char PAGE_GeneralSettings[] PROGMEM = R"=====(
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

const char PAGE_GeneralSettingsSaved[] = R"=====(
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
const char PAGE_Information[] = R"=====(
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

const char PAGE_NetworkConfiguration[] = R"=====(
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

const char PAGE_NetworkConfigurationSaved[] = R"=====(
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
const char PAGE_Restart[] = R"=====(
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
const char PAGE_Style_css[] = R"=====(
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
const char PAGE_microajax_js[] = R"=====(
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

//const char PAGE_AdminMainPage[] = "";
//const char PAGE_GeneralSettings[];
//const char PAGE_GeneralSettingsSaved[];
//const char PAGE_Information[];
//const char PAGE_NetworkConfiguration[];
//const char PAGE_NetworkConfigurationSaved[];
//const char PAGE_Restart[];
//const char PAGE_Style_css[];
//const char PAGE_microajax_js[];

void webServerGetAvailableNetworks();
void webServerGetGeneralConfigurationValues();
void webServerGetNetworkInfo();
void webServerReconnectToWifi();
void webServerGetMqttInfo();

#endif //NALKINSCLOUD_ESP8266_HTTP_SERVER_CONTENT_H
