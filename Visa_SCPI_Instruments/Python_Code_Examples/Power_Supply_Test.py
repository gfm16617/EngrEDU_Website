import sys
from time import sleep
import numpy as np

import pyvisa as visa

DAQ_ADDRESS = 'GPIB0::9::INSTR'  # DAQ -> KEYSIGHT 3497A
PS_ADDRESS = 'GPIB0::5::INSTR'  # Power Supply -> KEYSIGHT E3631A

rm = visa.ResourceManager()


def List_Available_Devices():
    global rm
    print('\nVISA library version:\n  ', rm)
    print('\nPython version:\n  ', sys.version)

    print('\nList of instruments discovered by Keysight Connection Expert:')
    i = 1
    for key, value in rm.list_resources_info().items():
        print('\nInstrument ', str(i), ': ', key)
        print('  Interface type: ', value.interface_type)
        print('  Interface board number: ', value.interface_board_number)
        print('  Resource class: ', value.resource_class)
        print('  Resource name: ', value.resource_name)
        print('  Resource alias: ', value.alias)
        i += 1


def Power_Supply_Example():
    # https://github.com/MarkDing/GPIB-pyvisa

    # Open connection
    inst = rm.open_resource(PS_ADDRESS)

    print('INSTRUMENT ID string:\n  ', inst.query('*IDN?'), flush=True)

    # Reset Device
    inst.write("*rst; status:preset; *cls")
    sleep(1)

    inst.write("INST P6V")  # Select +6V output
    inst.write("VOLT 2.0")  # Set output voltage to 3.0 V
    #inst.write("CURR 1.0")  # Set output current to 1.0 A
    # or
    #inst.write("APPL P6V, 3.0, 1.0")

    #inst.write("OUTP OFF")
    inst.write("OUTP ON")

    inst.write("INST P25V")  # Select +6V output
    inst.write("VOLT 10.0")  # Set output voltage to 3.0 V
    # inst.write("OUTP OFF")
    inst.write("OUTP ON")

    inst.write("INST N25V")  # Select +6V output
    inst.write("VOLT -7.0")  # Set output voltage to 3.0 V
    # inst.write("OUTP OFF")
    inst.write("OUTP ON")

    # Close connection
    inst.close()


if __name__ == '__main__':
    List_Available_Devices()
    Power_Supply_Example()
