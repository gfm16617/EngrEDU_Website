import sys
import random
from collections import deque

import numpy as np
from PyQt6 import QtCore, QtGui, QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QLabel, QInputDialog, QMessageBox
from PyQt6.QtCore import QThread, QObject, pyqtSignal, pyqtSlot

import nidaqmx
from nidaqmx import constants, system
from nidaqmx.constants import Edge, AcquisitionType
from nidaqmx import stream_readers

from nidaqmx.stream_readers import AnalogSingleChannelReader

from pyqtgraph import PlotWidget
import pyqtgraph as pg


class ContinuousAcquisitionThread(QThread):
    signal = pyqtSignal(int)

    def __init__(self):
        super(ContinuousAcquisitionThread, self).__init__()
        # Initialize aux variables
        self.endFlag = False

        self.dat = None
        self.maxLen = None
        self.xTime = None
        self.xCounter = 0
        self.curve1 = None
        self.samplingRate = 0
        self.xValues = None
        self.xCounterArray = 0

        self.in_stream_reader = None
        self.in_stream_array = None
        self.samples_per_channel = None

    def setValues(self, dat, maxLen, xTime, xCounter, curve1,
                  samplingRate, in_stream_reader, in_stream_array,
                  samples_per_channel, xValues, xCounterArray):
        self.dat = dat
        self.maxLen = maxLen
        self.xTime = xTime
        self.xCounter = xCounter
        self.curve1 = curve1
        self.samplingRate = samplingRate
        self.in_stream_reader = in_stream_reader
        self.in_stream_array = in_stream_array
        self.samples_per_channel = samples_per_channel
        self.xValues = xValues
        self.xCounterArray = xCounterArray

    def run(self):

        while True:
            if len(self.dat) > self.maxLen:
                self.dat.popleft()  # remove oldest
                self.xTime.popleft()

            # Update time axis
            self.xCounter += self.samplingRate
            self.xTime.append(self.xCounter)

            # Update time axis for array of data
            self.xValues += 200 * self.xCounterArray
            self.xCounterArray += 1

            # Read Analog values from NI DAQ
            self.in_stream_reader.read_many_sample(self.in_stream_array,
                                                   number_of_samples_per_channel=self.samples_per_channel,
                                                   timeout=10.0)

            np_values = np.array(self.in_stream_array)
            #self.signal.emit(len(np_values))
            mean_value = np.mean(np_values)

            # Append value to data
            self.dat.append(mean_value)

            # Plot data
            #self.curve1.setData(self.xTime, self.dat)
            self.curve1.setData(self.xValues, np_values)  # plots AC component

            if self.endFlag:
                print("Thread done.")
                break

    def endThread(self):
        self.endFlag = True


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
        self.maxLen = 100
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

        self.samples_per_channel = int(self.fs * self.samplingRate)
        self.num_channels = 1

        self.aiDAQTask.timing.cfg_samp_clk_timing(self.fs, None, nidaqmx.constants.Edge.RISING,
                                                  nidaqmx.constants.AcquisitionType.CONTINUOUS,
                                                  self.samples_per_channel * self.num_channels)

        # Create stream reader object
        self.in_stream_reader = AnalogSingleChannelReader(self.aiDAQTask.in_stream)

        # Initialize data 1D array of input analog samples
        self.in_stream_array = np.zeros(self.samples_per_channel, dtype=np.float64)

        # Initialize QThread
        # ------------------
        self.worker_ContinuousAcquisitionThread = ContinuousAcquisitionThread()

        self.worker_ContinuousAcquisitionThread.setValues(dat=self.dat,
                                                          maxLen=self.maxLen,
                                                          xTime=self.xTime,
                                                          xCounter=self.xCounter,
                                                          curve1=self.curve1,
                                                          samplingRate=self.samplingRate,
                                                          in_stream_reader=self.in_stream_reader,
                                                          in_stream_array=self.in_stream_array,
                                                          samples_per_channel=self.samples_per_channel,
                                                          xValues=self.xValues,
                                                          xCounterArray=self.xCounterArray)

        self.worker_ContinuousAcquisitionThread.signal.connect(self.consoleOutput)

        # Start QThread
        # --------------------
        self.worker_ContinuousAcquisitionThread.start()

    # -------------------------------------------------

    def consoleOutput(self, msg):
        print(int(msg))

    # -------------------------------------------------

    def closeEvent(self, *args, **kwargs):
        self.worker_ContinuousAcquisitionThread.endThread()

        self.aiDAQTask.stop()
        self.aiDAQTask.close()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec())
