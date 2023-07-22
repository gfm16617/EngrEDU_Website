import sys
import pyvisa as visa

# GPIB0::9::INSTR

def find(searchString):

    resourceManager = visa.ResourceManager()

    print('Find with search string \'%s\':' % searchString)
    devices = resourceManager.list_resources(searchString)
    if len(devices) > 0:
        for device in devices:
            print('\t%s' % device)
    else:
        print('... didn\'t find anything!')

    resourceManager.close()


if __name__ == '__main__':

    rm = visa.ResourceManager()
    print(rm.list_resources())

    # Finding all devices and interfaces is straightforward
    print('Find all devices and interfaces:\n')
    find('?*')

    # You can specify other device types using different search strings. Here are some common examples:

    # All instruments (no INTFC, BACKPLANE or MEMACC)
    find('?*INSTR')
    # PXI modules
    find('PXI?*INSTR')
    # USB devices
    find('USB?*INSTR')
    # GPIB instruments
    find('GPIB?*')
    # GPIB interfaces
    find('GPIB?*INTFC')
    # GPIB instruments on the GPIB0 interface
    find('GPIB0?*INSTR')
    # LAN instruments
    find('TCPIP?*')
    # SOCKET (::SOCKET) instruments
    find('TCPIP?*SOCKET')
    # VXI-11 (inst) instruments
    find('TCPIP?*inst?*INSTR')
    # HiSLIP (hislip) instruments
    find('TCPIP?*hislip?*INSTR')
    # RS-232 instruments
    find('ASRL?*INSTR')

    print('Done.')
