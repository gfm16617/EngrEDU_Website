import nidaqmx
from nidaqmx import constants

import time

device_name = "6001_Dev1/ai0"


def ai_single_demand_v1():
    #  Create Task
    analog_task = nidaqmx.Task()

    #  Create Virtual Channel - Analog Input
    analog_task.ai_channels.add_ai_voltage_chan(physical_channel=device_name,
                                                name_to_assign_to_channel="",
                                                terminal_config=constants.TerminalConfiguration.RSE,
                                                min_val=-10.0,
                                                max_val=10.0,
                                                units=constants.VoltageUnits.VOLTS,
                                                custom_scale_name=None)

    #  Start Task
    analog_task.start()

    #  Acquire Analog Value
    Ts = 1  # Sampling Time
    N = 5
    for k in range(N):
        value = analog_task.read()
        print(value)
        time.sleep(Ts)

    #  Stop and Clear Task
    analog_task.stop()
    analog_task.close()


def ai_single_demand_v2():
    #  Create Task
    with nidaqmx.Task() as analog_task:

        #  Create Virtual Channel - Analog Input
        analog_task.ai_channels.add_ai_voltage_chan(physical_channel=device_name,
                                                    name_to_assign_to_channel="",
                                                    terminal_config=constants.TerminalConfiguration.RSE,
                                                    min_val=-10.0,
                                                    max_val=10.0,
                                                    units=constants.VoltageUnits.VOLTS,
                                                    custom_scale_name=None)

        #  Start Task
        analog_task.start()

        #  Acquire Analog Value
        Ts = 1  # Sampling Time
        N = 5
        for k in range(N):
            value = analog_task.read()
            print(value)
            time.sleep(Ts)

        #  Stop and Clear Task
        # no need to include because "with" takes care of it.


if __name__ == "__main__":
    # ai_single_demand_v1()
    ai_single_demand_v2()
