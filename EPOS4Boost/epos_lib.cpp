#include <iostream>
#include "Definitions.h"
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>


typedef void* HANDLE;
typedef int BOOL;

enum EAppMode
{
    AM_UNKNOWN,
    AM_DEMO,
    AM_INTERFACE_LIST,
    AM_PROTOCOL_LIST,
    AM_VERSION_INFO
};

using namespace std;

void* g_pKeyHandle = 0;
unsigned short g_usNodeId = 1;
string g_deviceName;
string g_protocolStackName;
string g_interfaceName;
string g_portName;
int g_baudrate = 0;
EAppMode g_eAppMode = AM_DEMO;

const string g_programName = "EPOS4Boost";

#ifndef MMC_SUCCESS
    #define MMC_SUCCESS 0
#endif

#ifndef MMC_FAILED
    #define MMC_FAILED 1
#endif

#ifndef MMC_MAX_LOG_MSG_SIZE
    #define MMC_MAX_LOG_MSG_SIZE 512
#endif


// void PrintUsage()
// {
//     cout << "Usage: HelloEposCmd" << endl;
//     cout << "\t-h : this help" << endl;
//     cout << "\t-n : node id (default 1)" << endl;
//     cout << "\t-d   : device name (EPOS2, EPOS4, default - EPOS4)"  << endl;
//     cout << "\t-s   : protocol stack name (MAXON_RS232, CANopen, MAXON SERIAL V2, default - MAXON SERIAL V2)"  << endl;
//     cout << "\t-i   : interface name (RS232, USB, CAN_ixx_usb 0, CAN_kvaser_usb 0,... default - USB)"  << endl;
//     cout << "\t-p   : port name (COM1, USB0, CAN0,... default - USB0)" << endl;
//     cout << "\t-b   : baudrate (115200, 1000000,... default - 1000000)" << endl;
//     cout << "\t-l   : list available interfaces (valid device name and protocol stack required)" << endl;
//     cout << "\t-r   : list supported protocols (valid device name required)" << endl;
//     cout << "\t-v   : display device version" << endl;
// }

void LogError(string functionName, int p_lResult, unsigned int p_ulErrorCode)
{
    cerr << g_programName << ": " << functionName << " failed (result=" << p_lResult << ", errorCode=0x" << std::hex << p_ulErrorCode << ")"<< endl;
}

void LogInfo(string message)
{
    cout << message << endl;
}

void SeparatorLine()
{
    const int lineLength = 65;
    for(int i=0; i<lineLength; i++)
    {
        cout << "-";
    }
    cout << endl;
}

void PrintHeader()
{
    SeparatorLine();
    LogInfo("EPOS4 Boost interface");
    SeparatorLine();
}

struct EPOS4Boost
{
    void* g_pKeyHandle = 0;
    unsigned short g_usNodeId = 1;
    string g_deviceName = "EPOS4";
    string g_protocolStackName = "MAXON SERIAL V2";
    string g_interfaceName = "USB";
    string g_portName = "USB0"; 
    int g_baudrate = 1000000;
    EAppMode g_eAppMode = AM_DEMO;
    unsigned int p_pErrorCode = 0;

    void PrintSettings()
    {
        stringstream msg;
        msg << "default settings:" << endl;
        msg << "node id             = " << g_usNodeId << endl;
        msg << "device name         = '" << g_deviceName << "'" << endl;
        msg << "protocal stack name = '" << g_protocolStackName << "'" << endl;
        msg << "interface name      = '" << g_interfaceName << "'" << endl;
        msg << "port name           = '" << g_portName << "'"<< endl;
        msg << "baudrate            = " << g_baudrate;
        LogInfo(msg.str());
        SeparatorLine();
    }


    int PrintAvailablePorts(char* p_pInterfaceNameSel)
    {
        int lResult = MMC_FAILED;
        int lStartOfSelection = 1;
        int lMaxStrSize = 255;
        char* pPortNameSel = new char[lMaxStrSize];
        int lEndOfSelection = 0;
        unsigned int ulErrorCode = 0;
        do
        {
            if(!VCS_GetPortNameSelection((char*)g_deviceName.c_str(), (char*)g_protocolStackName.c_str(), p_pInterfaceNameSel, lStartOfSelection, pPortNameSel, lMaxStrSize, &lEndOfSelection, &ulErrorCode))
            {
                lResult = MMC_FAILED;
                LogError("GetPortNameSelection", lResult, ulErrorCode);
                break;
            }
            else
            {
                lResult = MMC_SUCCESS;
                printf("            port = %s\n", pPortNameSel);
            }

            lStartOfSelection = 0;
        }
        while(lEndOfSelection == 0);
        return lResult;
    }

    int PrintAvailableInterfaces()
    {
        int lResult = MMC_FAILED;
        int lStartOfSelection = 1;
        int lMaxStrSize = 255;
        char* pInterfaceNameSel = new char[lMaxStrSize];
        int lEndOfSelection = 0;
        unsigned int ulErrorCode = 0;
        do
        {
            if(!VCS_GetInterfaceNameSelection((char*)g_deviceName.c_str(), (char*)g_protocolStackName.c_str(), lStartOfSelection, pInterfaceNameSel, lMaxStrSize, &lEndOfSelection, &ulErrorCode))
            {
                lResult = MMC_FAILED;
                LogError("GetInterfaceNameSelection", lResult, ulErrorCode);
                break;
            }
            else
            {
                lResult = MMC_SUCCESS;

                printf("interface = %s\n", pInterfaceNameSel);

                PrintAvailablePorts(pInterfaceNameSel);
            }

            lStartOfSelection = 0;
        }
        while(lEndOfSelection == 0);
        SeparatorLine();
        delete[] pInterfaceNameSel;
        return lResult;
    }

    int PrintDeviceVersion()
    {
        int lResult = MMC_FAILED;
        unsigned short usHardwareVersion = 0;
        unsigned short usSoftwareVersion = 0;
        unsigned short usApplicationNumber = 0;
        unsigned short usApplicationVersion = 0;
        unsigned int ulErrorCode = 0;
        if(VCS_GetVersion(g_pKeyHandle, g_usNodeId, &usHardwareVersion, &usSoftwareVersion, &usApplicationNumber, &usApplicationVersion, &ulErrorCode))
        {
            printf("%s Hardware Version    = 0x%04x\n      Software Version    = 0x%04x\n      Application Number  = 0x%04x\n      Application Version = 0x%04x\n",
                    g_deviceName.c_str(), usHardwareVersion, usSoftwareVersion, usApplicationNumber, usApplicationVersion);
            lResult = MMC_SUCCESS;
        }

        return lResult;
    }

    int PrintAvailableProtocols()
    {
        int lResult = MMC_FAILED;
        int lStartOfSelection = 1;
        int lMaxStrSize = 255;
        char* pProtocolNameSel = new char[lMaxStrSize];
        int lEndOfSelection = 0;
        unsigned int ulErrorCode = 0;
        do
        {
            if(!VCS_GetProtocolStackNameSelection((char*)g_deviceName.c_str(), lStartOfSelection, pProtocolNameSel, lMaxStrSize, &lEndOfSelection, &ulErrorCode))
            {
                lResult = MMC_FAILED;
                LogError("GetProtocolStackNameSelection", lResult, ulErrorCode);
                break;
            }
            else
            {
                lResult = MMC_SUCCESS;

                printf("protocol stack name = %s\n", pProtocolNameSel);
            }

            lStartOfSelection = 0;
        }
        while(lEndOfSelection == 0);
        SeparatorLine();
        delete[] pProtocolNameSel;
        return lResult;
    }


    int OpenDevice()
    {
        int lResult = MMC_FAILED;
        char* pDeviceName = new char[255];
        char* pProtocolStackName = new char[255];
        char* pInterfaceName = new char[255];
        char* pPortName = new char[255];
        strcpy(pDeviceName, g_deviceName.c_str());
        strcpy(pProtocolStackName, g_protocolStackName.c_str());
        strcpy(pInterfaceName, g_interfaceName.c_str());
        strcpy(pPortName, g_portName.c_str());
        LogInfo("Opening device...");
        g_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, &p_pErrorCode);
        if(g_pKeyHandle!=0 && p_pErrorCode == 0)
        {
            unsigned int lBaudrate = 0;
            unsigned int lTimeout = 0;
            if(VCS_GetProtocolStackSettings(g_pKeyHandle, &lBaudrate, &lTimeout, &p_pErrorCode)!=0)
            {
                if(VCS_SetProtocolStackSettings(g_pKeyHandle, g_baudrate, lTimeout, &p_pErrorCode)!=0)
                {
                    if(VCS_GetProtocolStackSettings(g_pKeyHandle, &lBaudrate, &lTimeout, &p_pErrorCode)!=0)
                    {
                        if(g_baudrate==(int)lBaudrate)
                        {
                            LogInfo("Opened device...");
                            lResult = MMC_SUCCESS;
                        }
                    }
                }
            }
        }
        else
        {
            g_pKeyHandle = 0;
        }
        delete []pDeviceName;
        delete []pProtocolStackName;
        delete []pInterfaceName;
        delete []pPortName;
        return lResult;
    }

    int CloseDevice()
    {
        int lResult = MMC_FAILED;
        if(VCS_CloseDevice(g_pKeyHandle, &p_pErrorCode)!=0 && p_pErrorCode == 0)
        {
            lResult = MMC_SUCCESS;
            LogInfo("Closed device");
        }
        return lResult;
    }

    void enableState()
    {
        if (VCS_SetState(g_pKeyHandle, g_usNodeId, 1, &p_pErrorCode) == 0)
        {
            LogInfo("Failed  to enable state");
        }
        else
        {
            LogInfo("Enabled state");
        }
    }

    void disableState()
    {
        if (VCS_SetState(g_pKeyHandle, g_usNodeId, 0, &p_pErrorCode) == 0)
        {
            LogInfo("Failed  to disable state");
        }
        else
        {
            LogInfo("Disabled state");
        }
    }

    void setMinPosition(int limit)
    {
       unsigned int* num_bytes_read = 0;
       int read_data = 0;
       disableState();
       if(VCS_SetObject(g_pKeyHandle, g_usNodeId, 0x607D, 0x01, 
                        static_cast<int*>(&limit), 4, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to write data rate");
        } 
        enableState();
        if(VCS_GetObject(g_pKeyHandle, g_usNodeId, 0x607D, 0x01, 
                        static_cast<int*>(&read_data), 4, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to read");
        }
        std::cout << "Read value for position limit\t" << read_data << "\n";
    }

    void setupRLSEncoder()
    {
       short int write_data_rate = 1999;
       //5 multi turn bits, 15 single turn bits 8 trailing bits
       unsigned int bit_pattern = 0x00050F08;
       unsigned int position_bits = 0x0000B9CB;
       unsigned int* num_bytes_read = 0;
       disableState();
       if(VCS_SetObject(g_pKeyHandle, g_usNodeId, 0x3012, 0x01, 
                        static_cast<short int*>(&write_data_rate), 2, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to write data rate");
        }
        if(VCS_SetObject(g_pKeyHandle, g_usNodeId, 0x3012, 0x02, 
                        static_cast<unsigned int*>(&bit_pattern), 4, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to write configure bit pattern");
        }
        if(VCS_SetObject(g_pKeyHandle, g_usNodeId, 0x3012, 0x0B, 
                        static_cast<unsigned int*>(&position_bits), 4, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to write position bit pattern");
        }
        enableState();
    }

    void checkRLSEncoderSettings()
    {
        short int read_data_rate = 0;
        unsigned int bit_pattern = 0;
        unsigned int* num_bytes_read = 0;
        unsigned int position_bits = 0;
        unsigned int program_version = 0;
        if(VCS_GetObject(g_pKeyHandle, g_usNodeId, 0x1018, 0x03, 
                        static_cast<unsigned int*>(&program_version), 4, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to read program version");
        }
        if(VCS_GetObject(g_pKeyHandle, g_usNodeId, 0x3012, 0x01, 
                        static_cast<short int*>(&read_data_rate), 2, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to read data rate");
        }
        if(VCS_GetObject(g_pKeyHandle, g_usNodeId, 0x3012, 0x02, 
                        static_cast<unsigned int*>(&bit_pattern), 4, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to read configure bit pattern");
        }
        if(VCS_GetObject(g_pKeyHandle, g_usNodeId, 0x3012, 0x0B, 
                        static_cast<unsigned int*>(&position_bits), 4, num_bytes_read, &p_pErrorCode) == 0)
        {
            LogInfo("Failed to read position bit pattern");
        }
        std::cout << "Read value for program version \t" << program_version << "\n";
        std::cout << "Read value for data rate \t" << read_data_rate << "\n";
        std::cout << "Read value for configure bit pattern \t" << bit_pattern << "\n";
        std::cout << "Read value for position bit pattern \t" << position_bits << "\n";
    }

    int PrepareMode()
    {
        int lResult = MMC_SUCCESS;
        BOOL oIsFault = 0;
        if(VCS_GetFaultState(g_pKeyHandle, g_usNodeId, &oIsFault, &p_pErrorCode ) == 0)
        {
            LogError("VCS_GetFaultState", lResult, p_pErrorCode);
            lResult = MMC_FAILED;
        }

        if(lResult==0)
        {
            if(oIsFault)
            {
                stringstream msg;
                msg << "clear fault, node = '" << g_usNodeId << "'";
                LogInfo(msg.str());

                if(VCS_ClearFault(g_pKeyHandle, g_usNodeId, &p_pErrorCode) == 0)
                {
                    LogError("VCS_ClearFault", lResult, p_pErrorCode);
                    lResult = MMC_FAILED;
                }
            }

            if(lResult==0)
            {
                BOOL oIsEnabled = 0;

                if(VCS_GetEnableState(g_pKeyHandle, g_usNodeId, &oIsEnabled, &p_pErrorCode) == 0)
                {
                    LogError("VCS_GetEnableState", lResult, p_pErrorCode);
                    lResult = MMC_FAILED;
                }

                if(lResult==0)
                {
                    if(!oIsEnabled)
                    {
                        if(VCS_SetEnableState(g_pKeyHandle, g_usNodeId, &p_pErrorCode) == 0)
                        {
                            LogError("VCS_SetEnableState", lResult, p_pErrorCode);
                            lResult = MMC_FAILED;
                        }
                    }
                }
            }
        }
        return lResult;
    }


   int PositionProfileMode(long targetPosition)
    {
        int lResult = MMC_SUCCESS;
        stringstream msg;
        msg << "Set profile position mode, node = " << g_usNodeId;
        LogInfo(msg.str());
        msg << "Move to position = " << targetPosition << ", node = " << g_usNodeId;
        LogInfo(msg.str());
        PrepareMode();
        
        if(VCS_ActivateProfilePositionMode(g_pKeyHandle, g_usNodeId, &p_pErrorCode) == 0)
        {
            LogError("VCS_ActivateProfilePositionMode", lResult, p_pErrorCode);
            lResult = MMC_FAILED;
        }
        else
        {
            if(VCS_MoveToPosition(g_pKeyHandle, g_usNodeId, targetPosition, 0, 1, &p_pErrorCode) == 0)
            {
                LogError("VCS_MoveToPosition", lResult, p_pErrorCode);
                lResult = MMC_FAILED;
            }
            sleep(1);
            if(lResult == MMC_SUCCESS)
            {
                LogInfo("halt position movement");
                if(VCS_HaltPositionMovement(g_pKeyHandle, g_usNodeId, &p_pErrorCode) == 0)
                {
                    LogError("VCS_HaltPositionMovement", lResult, p_pErrorCode);
                    lResult = MMC_FAILED;
                }
            }
        }
        return lResult;
    }

    bool VelocityProfileMode(long targetvelocity)
    {
        int lResult = MMC_SUCCESS;
        stringstream msg;
        msg << "set profile velocity mode, node = " << g_usNodeId;
        LogInfo(msg.str());
        msg << "Move with target velocity = " << targetvelocity << " rpm, node = " << g_usNodeId;
        LogInfo(msg.str());
        PrepareMode();

        if(VCS_ActivateProfileVelocityMode(g_pKeyHandle, g_usNodeId, &p_pErrorCode) == 0)
        {
            LogError("VCS_ActivateProfileVelocityMode", lResult, p_pErrorCode);
            lResult = MMC_FAILED;
        }
        else
        {
            if(VCS_MoveWithVelocity(g_pKeyHandle, g_usNodeId, targetvelocity, &p_pErrorCode) == 0)
            {
                lResult = MMC_FAILED;
                LogError("VCS_MoveWithVelocity", lResult, p_pErrorCode);
            }
            sleep(1);
            if(lResult == MMC_SUCCESS)
            {
                LogInfo("halt velocity movement");
                if(VCS_HaltVelocityMovement(g_pKeyHandle, g_usNodeId, &p_pErrorCode) == 0)
                {
                    lResult = MMC_FAILED;
                    LogError("VCS_HaltVelocityMovement", lResult, p_pErrorCode);
                }
            }
        }
        return lResult;
    }
};



