#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <set>
#include <string>
#include <Windows.h>
#include <portabledeviceapi.h>
#include <atlbase.h>
#include "SetupAPI.h"
#include "cfgmgr32.h"

class DeviceManager
{
public:
    DeviceManager();
    void updateCurrentDevices();
    void setCurrentDevicesAsDefault();
    bool loadDefaultDevices();
    bool hasNewDevices();
private:
    void updatePortableDevices();
    void updateAllDevices();
    void RetrieveSubNodes(DEVINST parent);
private:
    std::set<std::wstring> m_currentDevices;
    std::set<std::wstring> m_defaultDevices;
};

#endif // DEVICEMANAGER_H
