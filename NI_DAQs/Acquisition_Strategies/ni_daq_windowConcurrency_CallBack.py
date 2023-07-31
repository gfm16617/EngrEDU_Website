import sys
import random
from collections import deque

import numpy as np
from PyQt6 import QtCore, QtGui, QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QLabel, QInputDialog, QMessageBox

import nidaqmx
from nidaqmx import constants, system
from nidaqmx.constants import Edge, AcquisitionType
from nidaqmx import stream_readers

from nidaqmx.stream_readers import AnalogSingleChannelReader

from pyqtgraph import PlotWidget
import pyqtgraph as pg


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        uic.loadUi("ni_daq_windowConcurrency.ui", self)

        # Initialize Plot
        # -----------------
        self.widgetPlot.setTitle("Analog Data Test")
        self.widgetPlot.setLabel('left', 'Voltage (V)')
        self.widgetPlot.setLabel('bottom', 'Time (s)')
        self.widgetPlot.setBackground('w')
        self.widgetPlot.setYRange(-6.0, 6.0, padding=0)

        self.dat = deque()
        self.xTime = deque()
        self.xCounter = 0
        self.maxLen = 200
        self.xValues = np.arange(1, 201)  # change the size based on self.samples_per_channel
        self.xCounterArray = 0

        self.curve1 = self.widgetPlot.plot(pen='r')

        # Initialize DAQ
        # ----------------
        self.deviceName = "6002_Dev2/ai0"

        # Create NIDAQmx Task
        self.aiDAQTask = nidaqmx.Task()

        # Configure NIDAQms Task
        self.aiDAQTask.ai_channels.add_ai_voltage_chan(self.deviceName, "",
                                                       nidaqmx.constants.TerminalConfiguration.RSE,
                                                       -5.0, 5.0,
                                                       nidaqmx.constants.VoltageUnits.VOLTS, None)

        # Configure sampling rate and acquisition type (Continuous)
        self.fs = 2000
        self.samplingRate = 0.1  # msecs

        self.samples_per_channel = int(self.fs * self.samplingRate)  # 2000 * 0.1 = 200 samples
        self.num_channels = 1

        self.aiDAQTask.timing.cfg_samp_clk_timing(self.fs, None, nidaqmx.constants.Edge.RISING,
                                                  nidaqmx.constants.AcquisitionType.CONTINUOUS,
                                                  self.samples_per_channel * self.num_channels)

        # Create stream reader object
        self.in_stream_reader = AnalogSingleChannelReader(self.aiDAQTask.in_stream)

        # Initialize data 1D array of input analog samples
        self.in_stream_array = np.zeros(self.samples_per_channel, dtype=np.float64)

        # Initialize NI Callback Function
        # ------------------
        self.aiDAQTask.register_every_n_samples_acquired_into_buffer_event(self.samples_per_channel,
                                                                           self.renderPlotTimer_Task)

        # Start Timer and DAQ
        # --------------------
        self.aiDAQTask.start()

    # -----------------------------------

    def renderPlotTimer_Task(self, task_handle, every_n_samples_event_type, number_of_samples, callback_data):
        if len(self.dat) > self.maxLen:
            self.dat.popleft()  # remove oldest
            self.xTime.popleft()

        # Update time axis
        self.xCounter += self.samplingRate
        self.xTime.append(self.xCounter)

        # Update time axis for array of data
        self.xValues += 200*self.xCounterArray
        self.xCounterArray += 1

        # Read Analog values from NI DAQ
        self.in_stream_reader.read_many_sample(self.in_stream_array,
                                               number_of_samples_per_channel=self.samples_per_channel,
                                               timeout=10.0)

        np_values = np.array(self.in_stream_array)
        #print(len(np_values))
        mean_value = np.mean(np_values)

        # Append value to data
        self.dat.append(mean_value)

        #self.curve1.setData(self.xTime, self.dat)  # plots DC component
        self.curve1.setData(self.xValues, np_values)  # plots AC component

        return 0

    # -------------------------------------------------

    def closeEvent(self, *args, **kwargs):
        self.aiDAQTask.stop()
        self.aiDAQTask.close()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec())
