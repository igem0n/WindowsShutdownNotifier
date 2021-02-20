#include "devicemanager.h"
#include <vector>
#include <fstream>
#include <ios>
#include <iostream>

#define CONFIG_NAME "ShutdownNotifier.conf"

DeviceManager::DeviceManager()
{

}

void DeviceManager::updateCurrentDevices()
{
    updateAllDevices();
}

void DeviceManager::setCurrentDevicesAsDefault()
{
    m_defaultDevices = m_currentDevices;
    std::wofstream config(CONFIG_NAME);
    for(auto &&device : m_defaultDevices)
    {
        config << device << std::endl;
    }
}

bool DeviceManager::loadDefaultDevices()
{
    m_defaultDevices.clear();
    std::wifstream config(CONFIG_NAME);
    std::wstring line;
    while(config.is_open() && !config.eof())
    {
        config >> line;
        if(!line.empty())
        {
            m_defaultDevices.insert(line);
        }
    }
    return config.is_open();
}

bool DeviceManager::hasNewDevices()
{
    for(auto &&device : m_currentDevices)
    {
        if(m_defaultDevices.find(device) == m_defaultDevices.end())
        {
            return true;
        }
    }

    return false;
}

void DeviceManager::updatePortableDevices()
{
    m_currentDevices.clear();

    CComPtr<IPortableDeviceManager> pPortableDeviceManager;
    HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&pPortableDeviceManager));
    if (FAILED(hr))
    {
        return;
    }

    DWORD cPnPDeviceIDs = 0;
    hr = pPortableDeviceManager->GetDevices(NULL, &cPnPDeviceIDs);
    if (FAILED(hr))
    {
        return;
    }

    std::vector<PWSTR> pPnpDeviceIDs(cPnPDeviceIDs, 0);
    hr = pPortableDeviceManager->GetDevices(pPnpDeviceIDs.data(), &cPnPDeviceIDs);
    if (SUCCEEDED(hr))
    {
        pPnpDeviceIDs.resize(cPnPDeviceIDs);
        m_currentDevices.insert(pPnpDeviceIDs.begin(), pPnpDeviceIDs.end());

        for (DWORD dwIndex = 0; dwIndex < cPnPDeviceIDs; dwIndex++)
        {
            CoTaskMemFree(pPnpDeviceIDs[dwIndex]);
            pPnpDeviceIDs[dwIndex] = NULL;
        }
    }
}

void DeviceManager::updateAllDevices()
{
    m_currentDevices.clear();

    DEVNODE dnRoot;
    CM_Locate_DevNode(&dnRoot, NULL, 0);

    RetrieveSubNodes(dnRoot);
}

void DeviceManager::RetrieveSubNodes(DEVINST parent)
{
    DEVNODE dn = NULL;

    CONFIGRET cr = CM_Get_Child(&dn, parent, 0);
    while(cr == CR_SUCCESS && dn != NULL)
    {
        WCHAR IdString[MAX_DEVICE_ID_LEN];
        ULONG Size = sizeof(IdString);
        memset(IdString, 0, Size);
        cr = CM_Get_DevNode_Registry_Property(dn, CM_DRP_HARDWAREID, NULL, IdString, &Size, 0);
        if (cr == CR_SUCCESS)
        {
            if(wcslen(IdString))
            {
                m_currentDevices.insert(IdString);
            }
        }

        RetrieveSubNodes(dn);

        cr = CM_Get_Sibling(&dn, dn, 0);
    }
}
