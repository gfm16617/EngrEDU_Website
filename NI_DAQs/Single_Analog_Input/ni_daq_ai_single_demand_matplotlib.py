import nidaqmx
from nidaqmx import constants

import time

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

device_name = "6001_Dev1/ai0"
analog_task = None

#  Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = []
ys = []
sample_index = 1


# This function is called periodically from FuncAnimation
def animate(i, ax, xs, ys):

    #  Acquire Analog Value
    global analog_task
    value = analog_task.read()

    # Add x and y to lists
    global sample_index
    sample_index = sample_index + 1
    xs.append(sample_index)
    ys.append(value)

    # Limit x and y lists to 20 items
    xs = xs[-20:]
    ys = ys[-20:]

    # Draw x and y lists
    ax.clear()
    ax.plot(xs, ys)

    # Format plot
    plt.ylim([-10.0, 10.0])
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.title('Analog Voltage')
    plt.ylabel('Volts')


def ai_single_demand_matplotlib():
    #  Create Task
    global analog_task
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
        # Set up plot to call animate() function periodically
        global ax, xs, ys, fig
        ani = animation.FuncAnimation(fig, animate, fargs=(ax, xs, ys), interval=100)
        plt.show()

        #  Stop and Clear Task
        # no need to include because "with" takes care of it.


if __name__ == "__main__":
    ai_single_demand_matplotlib()
