
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
 * @param lengthStartAddress the index value of the address that stores the length of 'data'
 * @param startAddress the index value of the address start location
 * @param data the data to write to memory
 */
void writeStringToEEPROM(int lengthStartAddress, int startAddress, String data) {
	unsigned int dateLength = data.length();
	if (dateLength > 32) {
		if (DEBUG) {
			Serial.println("ERROR - data length " + String(dateLength) + " is bigger then 32, max allowed");
			Serial.println("ERROR - CANNOT WRITE DATA TO EEPROM ");
		}
		return;
	}

	if (DEBUG) {
		Serial.println("storing to flash memory: '" + data + "' at address " + String(startAddress));
		Serial.println("data is " + String(dateLength) + " chars long");
	}

	int sizeofDataLength = sizeof(data.length());
	// make sure sizeof(data.length()) is not bigger then 4
	if (sizeofDataLength > 4) {
		if (DEBUG) {
			Serial.println("ERROR - data length is " + String(data.length()) + " and takes " + String(sizeofDataLength) + " bits, max allowed is 4 bits");
			Serial.println("ERROR - CANNOT WRITE DATA TO EEPROM ");
		}
		return;
	}

	EEPROM.write(lengthStartAddress, data.length());
	for (int i = 0; i < data.length(); i++) {
		EEPROM.write(startAddress + i, data[i]);
	}
	EEPROM.commit();
}

/**
 * Read string beginning at a given address
 * Max string size in EEPROM is 32
 *
 * @param beginAddress the index value of the address start location
 *
 * @return String the value stored starting at beginAddress
 */
String readStringFromEEPROM(int lengthStartAddress, int startAddress) {
	int dataLength = EEPROM.read(lengthStartAddress);

	String retString = "";
	// get each bit of values from the EEPROM
	for (int i = 0; i < dataLength; i++) {
		retString += char(EEPROM.read(startAddress + i));
	}
	return retString;
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

	writeStringToEEPROM(SSID_LENGTH_START_ADDR, SSID_START_ADDR, configs.wifiSsid);
	writeStringToEEPROM(WIFI_PASS_LENGTH_START_ADDR, WIFI_PASS_START_ADDR, configs.wifiPassword);

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
	configs.wifiSsid = readStringFromEEPROM(SSID_LENGTH_START_ADDR, SSID_START_ADDR);
	configs.wifiPassword = readStringFromEEPROM(WIFI_PASS_LENGTH_START_ADDR, WIFI_PASS_START_ADDR);

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
		Serial.println("Network configurations: ");
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

void writeIntToEEPROM(int address, int value) {
	if (DEBUG) {
		Serial.print("storing to flash memory: '" + String(value) + "' at address " + String(address));
		Serial.print(". data size is ");
		Serial.println(sizeof(value));
	}
	byte* p = (byte*) &value;
	EEPROM.write(address, *p);
	EEPROM.write(address + 1, *(p + 1));
	EEPROM.write(address + 2, *(p + 2));
	EEPROM.write(address + 3, *(p + 3));
	EEPROM.commit();
}

int readIntFromEEPROM(int address) {
	int value;
	byte* p = (byte*) &value;
	*p        = EEPROM.read(address);
	*(p + 1)  = EEPROM.read(address + 1);
	*(p + 2)  = EEPROM.read(address + 2);
	*(p + 3)  = EEPROM.read(address + 3);
	if (DEBUG) {
		Serial.print("\naddress: ");
		Serial.print(address);
		Serial.print(" | value is: ");
		Serial.println(value);
	}
	return value;
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
