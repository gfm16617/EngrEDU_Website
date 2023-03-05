import nidaqmx
from nidaqmx import constants
import numpy as np

device_name = "6001_Dev1/ai0"


def ai_single_finite_v1():
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
                                           sample_mode=nidaqmx.constants.AcquisitionType.FINITE,
                                           samps_per_chan=samples_per_channel)

    #  Start Task
    # -----------
    # no need to start task

    #  Acquire Analog Value
    # ---------------------

    # Initialize data array of input analog samples
    np_values = np.zeros(samples_per_channel, dtype=np.float64)

    # If the task acquires a finite number of samples and you
    # set this input to nidaqmx.constants.READ_ALL_AVAILABLE,
    # the method waits for the task to acquire all requested
    # samples, then reads those samples.
    np_values = analog_task.read(number_of_samples_per_channel=nidaqmx.constants.READ_ALL_AVAILABLE,
                                 timeout=nidaqmx.constants.WAIT_INFINITELY)

    print("Sample Size: " + str(len(np_values)))
    print(np_values)

    #  Stop and Clear Task
    # --------------------
    analog_task.stop()
    analog_task.close()


def ai_single_finite_v2():
    #  Create Task
    # ------------
    with nidaqmx.Task() as analog_task:

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
                                               sample_mode=nidaqmx.constants.AcquisitionType.FINITE,
                                               samps_per_chan=samples_per_channel)

        #  Start Task
        # -----------
        # no need to start task

        #  Acquire Analog Value
        # ---------------------

        # Initialize data array of input analog samples
        np_values = np.zeros(samples_per_channel, dtype=np.float64)

        # If the task acquires a finite number of samples and you
        # set this input to nidaqmx.constants.READ_ALL_AVAILABLE,
        # the method waits for the task to acquire all requested
        # samples, then reads those samples.
        np_values = analog_task.read(number_of_samples_per_channel=nidaqmx.constants.READ_ALL_AVAILABLE,
                                     timeout=nidaqmx.constants.WAIT_INFINITELY)

        print("Sample Size: " + str(len(np_values)))
        print(np_values)

        #  Stop and Clear Task
        # no need to include because "with" takes care of it.


if __name__ == "__main__":
    ai_single_finite_v1()
    # ai_single_finite_v2()
