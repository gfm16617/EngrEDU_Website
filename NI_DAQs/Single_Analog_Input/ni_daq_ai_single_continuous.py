import nidaqmx
from nidaqmx import constants
from nidaqmx.stream_readers import AnalogSingleChannelReader

import numpy as np

device_name = "6001_Dev1/ai0"

analog_task = None


def my_callback(task_handle, every_n_samples_event_type, number_of_samples, callback_data):
    global analog_task

    np_values = analog_task.read(number_of_samples_per_channel=nidaqmx.constants.READ_ALL_AVAILABLE,
                                 timeout=nidaqmx.constants.WAIT_INFINITELY)

    print("Sample Size: " + str(len(np_values)))
    print(np_values)

    return 0


def ai_single_continuous():
    global analog_task

    #  Create Task
    # --------------
    analog_task = nidaqmx.Task()

    #  Create Virtual Channel - Analog Input
    # --------------------------------------
    analog_task.ai_channels.add_ai_voltage_chan(physical_channel=device_name,
                                                name_to_assign_to_channel="",
                                                terminal_config=constants.TerminalConfiguration.RSE,
                                                min_val=-10.0,
                                                max_val=10.0,
                                                units=constants.VoltageUnits.VOLTS,
                                                custom_scale_name=None)

    #  Sets the source of the Sample Clock, the rate of the Sample
    #  Clock, and the number of samples to acquire or generate.
    Fs = 1000.0  # Hz
    samples_per_channel = 1000
    analog_task.timing.cfg_samp_clk_timing(rate=Fs,
                                           source=None,
                                           active_edge=nidaqmx.constants.Edge.RISING,
                                           sample_mode=nidaqmx.constants.AcquisitionType.CONTINUOUS,
                                           samps_per_chan=samples_per_channel)

    # Register my_callback function
    analog_task.register_every_n_samples_acquired_into_buffer_event(samples_per_channel, my_callback)

    #  Start Task
    # -----------
    analog_task.start()

    #  Acquire Analog Value
    # ---------------------
    # my_callback function takes care of the analog acquisition

    input("")

    #  Stop and Clear Task
    # --------------------
    analog_task.stop()
    analog_task.close()


if __name__ == "__main__":
    ai_single_continuous()
