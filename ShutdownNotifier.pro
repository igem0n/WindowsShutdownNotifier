TEMPLATE = app
CONFIG += c++11
CONFIG -= qt

LIBS+= -lUser32 -lPortableDeviceGuids -lCfgmgr32

SOURCES += \
        devicemanager.cpp \
        main.cpp

HEADERS += \
    devicemanager.h
