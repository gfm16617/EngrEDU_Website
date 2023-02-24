import nidaqmx
from nidaqmx import constants

import pyqtgraph as pg
from pyqtgraph.Qt import QtGui, QtCore

import sys
from collections import deque

device_name = "6001_Dev1/ai0"
analog_task = None


class Graph:
    def __init__(self, ):
        self.dat = deque()
        self.maxLen = 50  # max number of data points to show on graph
        self.app = QtGui.QApplication([])
        self.win = pg.GraphicsLayoutWidget()
        self.win.show()

        self.p1 = self.win.addPlot(colspan=2, title='Analog Voltage')
        self.p1.setYRange(-5.0, 5.0)
        self.curve1 = self.p1.plot()

        graphUpdateSpeedMs = 50
        timer = QtCore.QTimer()  # to create a thread that calls a function at intervals
        timer.timeout.connect(self.update)  # the update function keeps getting called at intervals
        timer.start(graphUpdateSpeedMs)
        sys.exit(self.app.exec())

    def update(self):
        if len(self.dat) > self.maxLen:
            self.dat.popleft()  # remove oldest

        global analog_task
        self.dat.append(analog_task.read())

        self.curve1.setData(self.dat)
        self.app.processEvents()


def ai_single_demand_pyqtgraph():
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
        q = Graph()

        #  Stop and Clear Task
        # no need to include because "with" takes care of it.


if __name__ == "__main__":
    ai_single_demand_pyqtgraph()
