import sys
from time import sleep
import numpy as np
from timeit import default_timer as timer

import pyvisa as visa

DAQ_ADDRESS = 'GPIB0::9::INSTR'  # DAQ -> KEYSIGHT 3497A
CHANNEL_LIST = "(@101:106)"

rm = visa.ResourceManager()


def DAQ_Info():
    # System Information
    # ---------------------
    # Open connection
    inst = rm.open_resource(DAQ_ADDRESS)
    print('INSTRUMENT ID string:\n  ', inst.query('*IDN?'), flush=True)
    print("SCPI Version: ", inst.query("SYST:VERS?"), flush=True)
    # Reset Device
    inst.write("*rst; status:preset; *cls")
    # ---------------------

    # Close connection
    inst.close()
    # ---------------------


def DAQ_Manual_Sampling():
    global CHANNEL_LIST

    # Open connection
    # ---------------------
    inst = rm.open_resource(DAQ_ADDRESS)

    # Reset Device
    # ---------------------
    inst.write("*rst; status:preset; *cls")
    sleep(1)

    # Channels Configuration
    # ---------------------
    inst.write("ROUT:OPEN " + CHANNEL_LIST)  # Open channels

    # Configure channel for DC voltage
    print("[Config]: ", inst.query("CONF? {}".format(CHANNEL_LIST)), flush=True)
    inst.write("CONF:VOLT:DC 10,0.001," + CHANNEL_LIST)
    print("[Config]: ", inst.query("CONF? {}".format(CHANNEL_LIST)), flush=True)

    # Enables automatic delay
    print("[Delay]: ", inst.query("ROUT:CHAN:DELAY? {}".format(CHANNEL_LIST)), flush=True)
    inst.write("ROUT:CHAN:DELAY:AUTO ON," + CHANNEL_LIST)
    print("[Delay]: ", inst.query("ROUT:CHAN:DELAY? {}".format(CHANNEL_LIST)), flush=True)

    # Measurement Setup
    # ---------------------

    inst.write("DISP:TEXT 'SCANNING...'")
    print("Scanning...")

    # Enables the internal DMM
    inst.write("INST:DMM ON")

    # Turns display off for slight increase in measurement rates
    inst.write("DISP OFF")

    # This should take 10 msec
    start = timer()  # TIC

    for i in range(10):
        inst.write("MEAS:VOLT:DC? " + CHANNEL_LIST)
        inst.read()
        #print(inst.read())
        sleep(0.001)

    end = timer()  # TOC

    print("[Execution Time]: " + str(end-start) + " secs")  # execution time = TOC - TIC

    print("Done.")

    # Close connection
    # ---------------------
    inst.close()


def DAQ_Timer_Sampling():
    global CHANNEL_LIST

    # Open connection
    # ---------------------
    inst = rm.open_resource(DAQ_ADDRESS)

    # Reset Device
    # ---------------------
    inst.write("*rst; status:preset; *cls")
    sleep(1)

    # Channels Configuration
    # ---------------------
    inst.write("ROUT:OPEN " + CHANNEL_LIST)  # Open channels

    # Configure channel for DC voltage
    print("[Config]: ", inst.query("CONF? {}".format(CHANNEL_LIST)), flush=True)
    inst.write("CONF:VOLT:DC 10,0.001," + CHANNEL_LIST)
    print("[Config]: ", inst.query("CONF? {}".format(CHANNEL_LIST)), flush=True)

    # Enables automatic delay
    print("[Delay]: ", inst.query("ROUT:CHAN:DELAY? {}".format(CHANNEL_LIST)), flush=True)
    inst.write("ROUT:CHAN:DELAY:AUTO ON," + CHANNEL_LIST)
    print("[Delay]: ", inst.query("ROUT:CHAN:DELAY? {}".format(CHANNEL_LIST)), flush=True)

    # Measurement Setup
    # ---------------------

    inst.write("DISP:TEXT 'SCANNING...'")
    print("Scanning...")

    # Enables the internal DMM
    inst.write("INST:DMM ON")

    # Turns display off for slight increase in measurement rates
    inst.write("DISP OFF")

    # Scan List of the desired multiplex channels
    inst.write("ROUT:SCAN " + CHANNEL_LIST)

    # Timer Trigger Example
    # ------------------------
    # Select the interval timer configuration
    inst.write("TRIG:SOURCE TIMER")
    # Set the scan interval to 50 msec
    inst.write("TRIG:TIMER 50E-03")
    # Time in seconds between 400 μs and 1 second, with 4 μs
    inst.write("VOLT:DC:APER 400E-06," + CHANNEL_LIST)
    # Sweep the scan list
    inst.write("TRIG:COUNT 10") # -> TRIG:TIMER / (VOLT:DC:APER * Num_Channels)

    # Initiate the scan when trig condition happens -> stores readings in memory
    inst.write("INITIATE")

    start = timer()  # TIC

    inst.write("FETCH?")
    values = inst.read()
    print("[Data]: " + values)

    end = timer()  # TOC
    print("[Execution Time]: " + str(end - start) + " secs")  # execution time = TOC - TIC

    '''
    # Software Trigger Example
    # ------------------------
    # Select type of trigger
    inst.write("TRIG:SOURCE BUS")
    # Initiate the scan when trig condition happens -> stores readings in memory
    inst.write("INITIATE")
    # Software Trigger
    inst.write("*TRG")
    # Transfers the readings from memory to the instrument's output buffer.
    inst.write("FETCH?")
    print("[Data]: " + inst.read())
    '''

    # Close connection
    # ---------------------
    inst.close()


def Read_Errors():
    global rm

    # Open connection
    inst = rm.open_resource(DAQ_ADDRESS)

    emptyFlag = False
    while not emptyFlag:
        msg = inst.query("SYST:ERR?")
        print("[Error]: ", msg)

        if msg.find('No error') != -1:
            emptyFlag = True

    # Close connection
    inst.close()


if __name__ == '__main__':
    #DAQ_Info()
    Read_Errors()  # Clear error buffer

    # Run this to see the challenge with manual sampling
    #DAQ_Manual_Sampling()

    # Run this to see good sampling
    DAQ_Timer_Sampling()

    Read_Errors()  # Get new errors
