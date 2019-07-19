# Modbus TCP Master Example Cpp

A basic Modbus TCP Master example written in CPP using the [CAS Modbus Stack](https://store.chipkin.com/services/stacks/modbus-stack)

## Example output

```txt
FYI: Modbus Stack version: 2.3.11.0
FYI: Modbus stack init, successfuly
FYI: Attempting to connect to TCP IPAddress=[127.0.0.1], port=[502]...
Connected.
FYI: Modbus read Request. SlaveAddress=[0], function=[3], StartingAddress=[0], modbusQuantity=[5]
FYI: TX: 00 00 00 00 00 06 00 03 00 00 00 05
FYI: RX: 00 00 00 00 00 0D 00 03 0A 04 D2 00 01 00 02 00 03 00 04
Read result:
        Offset[0] Value=[1234]
        Offset[1] Value=[1]
        Offset[2] Value=[2]
        Offset[3] Value=[3]
        Offset[4] Value=[4]

FYI: Modbus write Request. SlaveAddress=[0], function=[6], StartingAddress=[0], modbusQuantity=[1], value=[1234]
FYI: TX: 00 00 00 00 00 06 00 06 00 00 04 D2
FYI: RX: 00 00 00 00 00 06 00 06 00 00 04 D2
FYI: Write successful
```

## Building

1. Copy *CASModbusStack_Win32_Debug.dll*, *CASModbusStack_Win32_Release.dll*, *CASModbusStack_x64_Debug.dll*, and *CASModbusStack_x64_Release.dll* from the [CAS Modbus Stack](https://store.chipkin.com/services/stacks/modbus-stack) project  into the /bin/ folder.
2. Use [Visual Studios 2019](https://visualstudio.microsoft.com/vs/) to build the project. The solution can be found in the */ModbusTCPSlaveExampleCpp/* folder.

Note: The project is automaticly build on every checkin using GitlabCI.
