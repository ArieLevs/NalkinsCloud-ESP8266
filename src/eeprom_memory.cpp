
#include <HardwareSerial.h>
#include "EEPROM.h"
#include "eeprom_memory.h"
#include "configs.h"


void initEEPROM() {
	EEPROM.begin(512); // Initiate flash rom with 512 bytes
}

/**
 * Write to EEPROM a given string, starting from a given EEPROM address
 * The given addresses MUST be consistent with other parts in this code
 *
 * @param startAddress the index value of the address start location
 * @param value the value to write to memory
 */
void writeStringToEEPROM(int startAddress, String value) {
	char chBuffer[value.length() + 1]; // Declare char buffer
	//value.toCharArray(chBuffer, value.length()+1); // Convert string to char array
	value.c_str(); // Convert string to char array
	for (unsigned int i = 0; i < sizeof(chBuffer); i++)
		EEPROM.write(startAddress + i, chBuffer[i]);
	EEPROM.commit();
}

/**
 * Write wifi SSID and password to EEPROM flash memory
 */
void writeNetworkConfigs() {
	if (DEBUG) {
		Serial.println("Writing network configurations to flash memory");
		Serial.print("Inserting SSID: ");
		Serial.print(configs.wifiSsid.c_str());
		Serial.print(", Password: ");
		Serial.println(configs.wifiPassword.c_str());
	}

	writeStringToEEPROM(SSID_START_ADDR, configs.wifiSsid);
	writeStringToEEPROM(WIFI_PASS_START_ADDR, configs.wifiPassword);

	EEPROM.write(DHCP_FLAG_ADDR, 1);

	EEPROM.write(400, configs.IP[0]);
	EEPROM.write(401, configs.IP[1]);
	EEPROM.write(402, configs.IP[2]);
	EEPROM.write(403, configs.IP[3]);

	EEPROM.write(404, configs.Netmask[0]);
	EEPROM.write(405, configs.Netmask[1]);
	EEPROM.write(406, configs.Netmask[2]);
	EEPROM.write(407, configs.Netmask[3]);

	EEPROM.write(408, configs.Gateway[0]);
	EEPROM.write(409, configs.Gateway[1]);
	EEPROM.write(410, configs.Gateway[2]);
	EEPROM.write(411, configs.Gateway[3]);

	EEPROM.commit();
}

/**
 * Get EEPROM network configs to global struct
 */
void readNetworkConfigs() {
	configs.wifiSsid = readStringFromEEPROM(0);
	configs.wifiPassword = readStringFromEEPROM(32);

	configs.dhcp = EEPROM.read(DHCP_FLAG_ADDR);

	configs.IP[0] = EEPROM.read(400);
	configs.IP[1] = EEPROM.read(401);
	configs.IP[2] = EEPROM.read(402);
	configs.IP[3] = EEPROM.read(403);
	configs.Netmask[0] = EEPROM.read(404);
	configs.Netmask[1] = EEPROM.read(405);
	configs.Netmask[2] = EEPROM.read(406);
	configs.Netmask[3] = EEPROM.read(407);
	configs.Gateway[0] = EEPROM.read(408);
	configs.Gateway[1] = EEPROM.read(409);
	configs.Gateway[2] = EEPROM.read(410);
	configs.Gateway[3] = EEPROM.read(411);

	if (DEBUG) {
		Serial.println("Current configurations: ");
		Serial.print("SSID: ");
		Serial.println(configs.wifiSsid.c_str());
		Serial.print("Wifi Password: ");
		Serial.println(configs.wifiPassword.c_str());

		if (configs.dhcp) {
			Serial.println("Using DHCP: True");
		} else {
			Serial.println("Using DHCP: False");
			Serial.print("IP: ");
			Serial.print(configs.IP[0]);
			Serial.print(".");
			Serial.print(configs.IP[1]);
			Serial.print(".");
			Serial.print(configs.IP[2]);
			Serial.print(".");
			Serial.println(configs.IP[3]);

			Serial.print("Subnet mask: ");
			Serial.print(configs.Netmask[0]);
			Serial.print(".");
			Serial.print(configs.Netmask[1]);
			Serial.print(".");
			Serial.print(configs.Netmask[2]);
			Serial.print(".");
			Serial.println(configs.Netmask[3]);

			Serial.print("Gateway: ");
			Serial.print(configs.Gateway[0]);
			Serial.print(".");
			Serial.print(configs.Gateway[1]);
			Serial.print(".");
			Serial.print(configs.Gateway[2]);
			Serial.print(".");
			Serial.println(configs.Gateway[3]);
		}
	}
}

void writeIntToEEPROM(int address, uint8_t value) {
	EEPROM.write(address, value);
	if (DEBUG) {
		Serial.print("\naddress: ");
		Serial.print(address);
		Serial.print(" | set to: ");
		Serial.println(value);
	}
	EEPROM.commit();
}

uint8_t readIntFromEEPROM(int address) {
	return EEPROM.read(address);
}

/**
 * Read string beginning at a given address
 * Max string size in EEPROM is 32
 * 
 * @param beginAddress the index value of the address start location
 * 
 * @return String the value stored starting at beginAddress
 */
String readStringFromEEPROM(int beginAddress) {
	int index = 0;
	char ch;
	String retString;
	while (true) {
		ch = EEPROM.read(beginAddress + index);
		if (ch == 0)
			break;
		if (index > 31)
			break;
		index++;
		retString += ch;
	}
	return retString;
}

/**
 * Check that Value is between 0-255
 * 
 * @param value a string to validate
 * 
 * @return boolean  true - value is in the range of 0 - 255
 *                  false - value is not in 0 - 255
 */
bool checkRange(String value) {
	return (value.toInt() < 0 || value.toInt() > 255);
}

/**
 * Erase the EEPROM flash memory
 */
void clearEEPROM() {
	for (unsigned int i = 0; i < 512; ++i)
		EEPROM.write(i, 0);
	EEPROM.commit();
	if (DEBUG)
		Serial.println("Entire flash memory deleted");
}

/**
 * Write to EEPROM the configuration status mode of the device
 * EEPROM configuration mode address is 494
 * 
 * @param configurationStatus integer number (should be 0 or 1) to be writen to EEPROM
 */
void setConfigurationStatusFlag(uint8_t configurationStatus) {
	// Write given value to CONFIGURATION_MODE_ADDR (494)
	writeIntToEEPROM(CONFIGURATION_MODE_ADDR, configurationStatus);
	if (DEBUG) {
		Serial.print("ConfigurationStatus set to: ");
		Serial.println(configurationStatus);
	}
}

/**
 * get from EEPROM the configuration status mode of the device
 * 
 * @return boolean  true - value of address at CONFIGURATIONMODEADDR (494) is 1
 *                  false - value in different from 1
 */
bool getConfigurationStatusFlag() {
	int value = readIntFromEEPROM(CONFIGURATION_MODE_ADDR);
	if (DEBUG) {
		Serial.print("configuration status is: ");
		Serial.println(value);
	}
	return (value == 1);
}

/**
 * Write to EEPROM the chosen service mode of the devise
 * EEPROM service mode address is 500
 * 
 * @param serviceMode integer number (should be 0 or 1) to be writen to EEPROM
 */
void setServiceMode(int serviceMode) {
	if (serviceMode == 0) {
		writeIntToEEPROM(SERVICE_MODE_FLAG_ADDR, 0);
		if (DEBUG)
			Serial.println("Service mode set to standalone (0)");
	} else {
		writeIntToEEPROM(SERVICE_MODE_FLAG_ADDR, 1);
		if (DEBUG)
			Serial.println("Service mode set to local-server (1)");
	}
}

/**
 * get from EEPROM the service status mode of the device
 * 
 * @return boolean  true - value of address at SERVICEMODEFLAG (500) is 1
 *                  false - value in different from 1
 */
int getServiceMode() {
	int answer = readIntFromEEPROM(SERVICE_MODE_FLAG_ADDR);
	if (DEBUG) {
		Serial.print("Service mode is: ");
		Serial.println(answer);
	}
	return answer;
}

/**
 * Erase Wifi SSID from EEPROM flash memory
 * Bits 0-31 belong to SSID info
 */
void clearSSIDFromEEPROM() {
	for (unsigned int i = 0; i < 32; ++i)
		EEPROM.write(i, 0);
	EEPROM.commit();
	if (DEBUG)
		Serial.println("SSID deleted from flash memory");
}

/**
 * Erase Wifi pass from EEPROM flash memory
 * Bits 32-63 belong to wifi password info
 */
void clearPasswordFromEEPROM() {
	for (unsigned int i = 0; i < 512; ++i)
		EEPROM.write(32 + i, 0);
	EEPROM.commit();
	if (DEBUG)
		Serial.println("Password deleted from flash memory");
}
