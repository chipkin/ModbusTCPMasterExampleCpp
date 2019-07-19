// ModbusTCPMasterExampleCpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
// CASModbusMasterTCPExample
//
// Last updated: 2017 Dec 12
//

#include <stdio.h>
#include <time.h> // Current time in seconds.

#include "CTCP.h"                   // TCP used by Modbus TCP
#include "CASModbusAdapter.h"		// Loads all the Modbus functions. 
#include "ChipkinEndianness.h"		// Helps with OS depenent Endianness

using namespace ChipkinCommon;

// Settings
// ===========================
const char SETTING_TCP_IP_ADDRESS[] = "127.0.0.1";
// const char SETTING_TCP_IP_ADDRESS[] = "192.168.1.201";
const unsigned short SETTING_TCP_PORT = 502;
const unsigned char SETTING_MODBUS_SERVER_SLAVE_ADDRESS = 0x00;
unsigned int SETTING_MODBUS_TYPE = 2; // MODBUS_TYPE_RTU = 1 || MODBUS_TYPE_TCP = 2

// Globals
// ===========================
// TCP connection
CTCP gTcp;


// API return codes.
// ===========================
static const unsigned int MODBUS_STATUS_ERROR_UNKNOWN = 0;
static const unsigned int MODBUS_STATUS_SUCCESS = 1;

// Modbus function codes
// ===========================
static const unsigned char MODBUS_FUNCTION_01_READ_COIL_STATUS = 1;
static const unsigned char MODBUS_FUNCTION_02_READ_INPUT_STATUS = 2;
static const unsigned char MODBUS_FUNCTION_03_READ_HOLDING_REGISTERS = 3;
static const unsigned char MODBUS_FUNCTION_04_READ_INPUT_REGISTERS = 4;
static const unsigned char MODBUS_FUNCTION_05_FORCE_SINGLE_COIL = 5;
static const unsigned char MODBUS_FUNCTION_06_PRESET_SINGLE_REGISTER = 6;
static const unsigned char MODBUS_FUNCTION_0F_FORCE_MULTIPLE_COILS = 15;
static const unsigned char MODBUS_FUNCTION_10_FORCE_MULTIPLE_REGISTERS = 16;
static const unsigned short MODBUS_FORCE_SINGLE_COIL_ON = 0xFF00;
static const unsigned short MODBUS_FORCE_SINGLE_COIL_OFF = 0x0000;

// Modbus exception codes
// ===========================
static const unsigned int MODBUS_STATUS_EXCEPTION_01_ILLEGAL_FUNCTION = 0x01;
static const unsigned int MODBUS_STATUS_EXCEPTION_02_ILLEGAL_DATA_ADDRESS = 0x02;
static const unsigned int MODBUS_STATUS_EXCEPTION_04_SLAVE_DEVICE_FAILURE = 0x04;

// Test functions
// ===========================
void TestWriteRequest(unsigned short connectionID);
void TestReadRequest(unsigned short connectionID);



// Helper functions
// ===========================

unsigned char SetBit(unsigned char data, unsigned char bit, bool value)
{
	// http://stackoverflow.com/a/47990
	data ^= (-(value ? 1 : 0) ^ data) & (1 << bit);
	return data;
}
bool GetBit(unsigned char data, unsigned char bit)
{
	return ((data >> bit) & 1);
}

// Callback functions
// ===========================
bool sendModbusMessage(const unsigned short connectionId, const unsigned char* payload, const unsigned short payloadSize)
{
	// Debug print sent message.
	printf("FYI: TX: ");
	for (unsigned int offset = 0; offset < payloadSize; offset++)
	{
		printf("%02X ", payload[offset]);
	}
	printf("\n");

	if (!gTcp.SendMessage(connectionId, payload, payloadSize))
	{
		printf("Error: SendMessage failed.\n");
		return false; // Unknonw error
	}
	// Everything looks good.
	return true;
}
unsigned int recvModbusMessage(unsigned short& connectionId, unsigned char* payload, unsigned short maxPayloadSize)
{
	if (maxPayloadSize <= 0 || payload == NULL)
	{
		return 0; // No space to store message.
	}
	int length = gTcp.GetMessage(connectionId, payload, maxPayloadSize);
	if (length <= 0)
	{
		return 0;
	}

	// Debug print recived message.
	printf("FYI: RX: ");
	for (unsigned int offset = 0; offset < length; offset++)
	{
		printf("%02X ", payload[offset]);
	}
	printf("\n");

	// Everything looks good.
	return length;
}
unsigned long currentTime()
{
	return (unsigned long)time(0);
}

int main()
{
	// Load the DLL functions
	// --------------------------------
	if (!LoadModbusFunctions())
	{
		printf("\nError could not load DLL functions\n\n");
		return 1;
	}
	printf("FYI: Modbus Stack version: %d.%d.%d.%d\n", fpModbusStack_GetAPIMajorVersion(), fpModbusStack_GetAPIMinorVersion(), fpModbusStack_GetAPIPatchVersion(), fpModbusStack_GetAPIBuildVersion());

	// Set up the API and callbacks.
	// --------------------------------
	unsigned int returnCode = fpModbusStack_Init(SETTING_MODBUS_TYPE, sendModbusMessage, recvModbusMessage, currentTime);
	if (returnCode != MODBUS_STATUS_SUCCESS)
	{
		printf("\nError: Could not init the Modbus Stack, returnCode=%d\n\n", returnCode);
		return 1;
	}
	printf("FYI: Modbus stack init, successfuly\n");

	// Connect to the Modbus Slave 
	printf("FYI: Attempting to connect to TCP IPAddress=[%s], port=[%d]... \n", SETTING_TCP_IP_ADDRESS, SETTING_TCP_PORT);
	int connectionID = gTcp.Connect(SETTING_TCP_IP_ADDRESS, SETTING_TCP_PORT);
	if (connectionID < 0) {
		printf("\n");
		printf("Error: Can not connect to TCP server. IP Address=[%s], port=[%d], lastError=[%d]\n\n", SETTING_TCP_IP_ADDRESS, SETTING_TCP_PORT, gTcp.GetLastError());
		return 1;
	}
	printf("Connected.\n");

	// Build and send the Modbus messages 
	TestReadRequest(connectionID);
	TestWriteRequest(connectionID);

	return 0;
}


void TestReadRequest(unsigned short connectionID) {
	// Create a read request.
	const unsigned char modbusFunction = MODBUS_FUNCTION_03_READ_HOLDING_REGISTERS;
	const unsigned short modbusStartingAddress = 0;
	const unsigned short modbusQuantity = 5;
	unsigned short data[10];
	unsigned char exceptionCode;

	printf("FYI: Modbus read Request. SlaveAddress=[%d], function=[%d], StartingAddress=[%d], modbusQuantity=[%d]\n", SETTING_MODBUS_SERVER_SLAVE_ADDRESS, modbusFunction, modbusStartingAddress, modbusQuantity);
	unsigned int returnCode = fpModbusStack_ReadRegisters(connectionID, SETTING_MODBUS_SERVER_SLAVE_ADDRESS, modbusFunction, modbusStartingAddress, modbusQuantity, (unsigned char*)data, sizeof(short) * 10, exceptionCode);
	if (returnCode != MODBUS_STATUS_SUCCESS) {
		printf("Error: Could not send Read Register message. returnCode=%d\n", returnCode);
		return;
	}

	// Print the results from the read result.
	printf("Read result: \n");
	for (int offset = 0; offset < modbusQuantity; offset++) {
		unsigned short value = data[offset];
		CEndianness::FromSmallEndian(&value, sizeof(short));
		printf("\tOffset[%d] Value=[%d]\n", offset, value);
	}
	printf("\n");
}

void TestWriteRequest(unsigned short connectionID) {
	// Create a read request.
	const unsigned char modbusFunction = MODBUS_FUNCTION_06_PRESET_SINGLE_REGISTER;
	const unsigned short modbusStartingAddress = 0;
	const unsigned short modbusQuantity = 1;
	unsigned short data[1];
	data[0] = 1234;
	CEndianness::ToSmallEndian(&data[0], sizeof(short));
	unsigned char exceptionCode;

	printf("FYI: Modbus write Request. SlaveAddress=[%d], function=[%d], StartingAddress=[%d], modbusQuantity=[%d], value=[%d]\n", SETTING_MODBUS_SERVER_SLAVE_ADDRESS, modbusFunction, modbusStartingAddress, modbusQuantity, data[0]);
	unsigned int returnCode = fpModbusStack_WriteRegisters(connectionID, SETTING_MODBUS_SERVER_SLAVE_ADDRESS, modbusFunction, modbusStartingAddress, (unsigned char*)data, (unsigned short)modbusQuantity, exceptionCode);
	if (returnCode != MODBUS_STATUS_SUCCESS) {
		printf("Error: Could not send Read Register message. returnCode=%d\n", returnCode);
		return;
	}
	printf("FYI: Write successful\n");
}