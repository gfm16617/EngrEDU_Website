import queue
import sys
from PyQt6 import QtCore, QtGui, QtWidgets, uic
from PyQt6.QtWidgets import QApplication
from PyQt6.QtCore import QThread, QObject, pyqtSignal

import pyqtgraph as pg
from pyqtgraph import PlotWidget
from pyqtgraph.Qt import QtGui, QtCore

import nidaqmx
from nidaqmx import constants
from nidaqmx.stream_readers import AnalogSingleChannelReader

from collections import deque

import numpy as np
from time import sleep

device_name = "6001_Dev1/ai0"
analog_task = None
analog_task_flag = False
graphUpdateSpeedMs = 50  # Default

ai_mode = "On Demand"
Fs = 1000.0  # Default
samples_per_channel = 1000  # Default

# Thread Queue Communication
queue_flag = None


class ContinuousAcquisitionThread(QThread):

    def __init__(self):
        super(ContinuousAcquisitionThread, self).__init__()
        # Initialize aux variables
        self.Fs = None
        self.samples_per_channel = None
        self.analog_task = None
        self.graphWidget = None
        self.dat = None
        self.curve1 = None
        self.exit_flag = False

    def setValues(self, Fs, samples_per_channel, analog_task, graphWidget, dat, curve1):
        self.Fs = Fs
        self.samples_per_channel = samples_per_channel
        self.analog_task = analog_task
        self.graphWidget = graphWidget
        self.dat = dat
        self.curve1 = curve1

    def run(self):
        #  Sets the source of the Sample Clock, the rate of the Sample
        #  Clock, and the number of samples to acquire or generate.
        self.analog_task.timing.cfg_samp_clk_timing(rate=self.Fs,
                                                    source=None,
                                                    active_edge=nidaqmx.constants.Edge.RISING,
                                                    sample_mode=nidaqmx.constants.AcquisitionType.CONTINUOUS,
                                                    samps_per_chan=self.samples_per_channel)

        # Create a stream reader object
        in_stream_analogTask = AnalogSingleChannelReader(analog_task.in_stream)

        # Initialize data array of input analog samples
        in_stream_array = np.zeros(self.samples_per_channel, dtype=np.float64)

        while True:
            # Reads samples from an analog input channel in an NI-DAQmx task.
            in_stream_analogTask.read_many_sample(data=in_stream_array,
                                                  number_of_samples_per_channel=self.samples_per_channel,
                                                  timeout=100.0)

            self.dat = in_stream_array

            # Update Graph
            self.curve1.setData(self.dat)

            # Check If Stop Button was Pressed
            global queue_flag
            if not queue_flag.empty():
                self.exit_flag = queue_flag.get()
                queue_flag.queue.clear()  # clear queue

            if self.exit_flag:
                self.exit_flag = False  # reset queue_flag
                break


class FiniteAcquisitionThread(QThread):
    pushButton_stop = pyqtSignal()

    def __init__(self):
        super(FiniteAcquisitionThread, self).__init__()
        # Initialize aux variables
        self.Fs = None
        self.samples_per_channel = None
        self.analog_task = None
        self.graphWidget = None
        self.dat = None
        self.curve1 = None
        self.progressBar = None

    def setValues(self, Fs, samples_per_channel, analog_task, graphWidget, dat, curve1, progressBar):
        self.Fs = Fs
        self.samples_per_channel = samples_per_channel
        self.analog_task = analog_task
        self.graphWidget = graphWidget
        self.dat = dat
        self.curve1 = curve1
        self.progressBar = progressBar

    def run(self):
        #  Sets the source of the Sample Clock, the rate of the Sample
        #  Clock, and the number of samples to acquire or generate.
        self.analog_task.timing.cfg_samp_clk_timing(rate=self.Fs,
                                                    source=None,
                                                    active_edge=nidaqmx.constants.Edge.RISING,
                                                    sample_mode=nidaqmx.constants.AcquisitionType.FINITE,
                                                    samps_per_chan=self.samples_per_channel)

        # Initialize data array of input analog samples
        np_values = np.zeros(self.samples_per_channel, dtype=np.float64)

        # If the task acquires a finite number of samples and you
        # set this input to nidaqmx.constants.READ_ALL_AVAILABLE,
        # the method waits for the task to acquire all requested
        # samples, then reads those samples.
        np_values = self.analog_task.read(number_of_samples_per_channel=nidaqmx.constants.READ_ALL_AVAILABLE,
                                          timeout=nidaqmx.constants.WAIT_INFINITELY)

        self.graphWidget.enableAutoRange(axis='y', enable=True)
        self.dat = np_values
        # Update Graph
        self.curve1.setData(self.dat)

        #  pushButton_stop
        self.pushButton_stop.emit()
        self.progressBar.setVisible(False)


class ProgressBarThread(QThread):
    _signal = pyqtSignal(int)

    def __init__(self):
        super(ProgressBarThread, self).__init__()
        self.counter = 1  # Default 1s

    def setCounter(self, value):
        self.counter = value

    def run(self):
        sleep_time = 0.1
        k = self.counter/sleep_time
        for i in range(int(k)+1):  # +1 to allow the update function to reset to zero
            sleep(sleep_time)
            self._signal.emit(i*(100.0/k))  # Adjust increments to 100%


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Load ui File
        uic.loadUi("ni_max_Continuous.ui", self)

        # Initialize Components on the GUI
        self.comboBox_channelName.addItem(device_name)
        self.comboBox_mode.addItems(["On Demand", "Finite", "Continuous"])
        self.comboBox_inputConfig.addItems(["RSE"])
        self.progressBar_finite.setVisible(False)

        # Callback Functions
        self.pushButton_start.clicked.connect(self.pushButton_start_clicked)
        self.pushButton_stop.clicked.connect(self.pushButton_stop_clicked)
        self.checkBox_scale.stateChanged.connect(self.checkBox_scale_stateChanged)
        self.comboBox_mode.currentIndexChanged.connect(self.comboBox_mode_IndexChanged)

        # Aux ProgressBar Thread
        self.worker_ProgressBarThread = ProgressBarThread()
        self.worker_ProgressBarThread._signal.connect(self.progressBar_Update)

        # Aux Finite Acquisition Thread
        self.worker_FiniteAcquisitionThread = FiniteAcquisitionThread()
        self.worker_FiniteAcquisitionThread.pushButton_stop.connect(self.pushButton_stop_clicked)

        # Aux Continuous Acquisition Thread
        self.worker_ContinuousAcquisitionThread = ContinuousAcquisitionThread()

        global queue_flag
        queue_flag = queue.Queue(maxsize=2)

        # Initialize plot
        self.dat = deque()
        self.maxLen = 100  # max number of data points to show on graph

        self.curve1 = self.graphWidget.plot()

        self.timer = QtCore.QTimer()  # to create a thread that calls a function at intervals
        self.timer.timeout.connect(self.update_onDemand)  # the update function keeps getting called at intervals

    #  --------------------------------
    #  ProgressBar Update Callback Function
    #  --------------------------------
    def progressBar_Update(self, msg):
        self.progressBar_finite.setValue(int(msg))
        if self.progressBar_finite.value() > 99:
            self.progressBar_finite.setValue(0)

    #  --------------------------------
    #  update Callback Function
    #  --------------------------------
    def update_onDemand(self):
        if len(self.dat) > self.maxLen:
            self.dat.popleft()  # remove oldest

        global analog_task
        value = analog_task.read()
        self.dat.append(value)

        # Update Graph
        self.curve1.setData(self.dat)

        # Update textEdit
        self.textEdit_analogValue.setPlainText("{:.2f}".format(value))

    #  --------------------------------
    #  comboBox_mode Callback Function
    #  --------------------------------
    def comboBox_mode_IndexChanged(self):
        global ai_mode
        ai_mode = self.comboBox_mode.currentText()

        if ai_mode == "Finite" or ai_mode == "Continuous":
            #  Unlock Rate and Samples per Channel
            self.label_rate.setEnabled(True)
            self.label_samplesRead.setEnabled(True)
            self.textEdit_rate.setEnabled(True)
            self.textEdit_samplesRead.setEnabled(True)

        elif ai_mode == "On Demand":
            #  Lock Rate and Samples per Channel
            self.label_rate.setEnabled(False)
            self.label_samplesRead.setEnabled(False)
            self.textEdit_rate.setEnabled(False)
            self.textEdit_samplesRead.setEnabled(False)

    #  --------------------------------
    #  checkBox_scale Callback Function
    #  --------------------------------
    def checkBox_scale_stateChanged(self):
        if not self.checkBox_scale.isChecked():
            self.graphWidget.setYRange(float(self.textEdit_minInput.toPlainText()),
                                       float(self.textEdit_maxInput.toPlainText()))
        else:
            self.graphWidget.enableAutoRange(axis='y', enable=True)

    #  --------------------------------
    #  pushButton_start Callback Function
    #  --------------------------------
    def pushButton_start_clicked(self):
        #  Enable/Disable Component
        self.pushButton_stop.setEnabled(True)
        self.pushButton_start.setEnabled(False)
        self.frame_parameters.setEnabled(False)

        #  Create Task
        global analog_task, analog_task_flag

        #  Create Task
        analog_task = nidaqmx.Task()

        #  Create Virtual Channel - Analog Input
        analog_task.ai_channels.add_ai_voltage_chan(physical_channel=device_name,
                                                    name_to_assign_to_channel="",
                                                    terminal_config=constants.TerminalConfiguration.RSE,
                                                    min_val=float(self.textEdit_minInput.toPlainText()),
                                                    max_val=float(self.textEdit_maxInput.toPlainText()),
                                                    units=constants.VoltageUnits.VOLTS,
                                                    custom_scale_name=None)

        #  Check Acquisition Mode
        global ai_mode
        if ai_mode == "On Demand":
            #  Start Task
            analog_task.start()
            analog_task_flag = True

            #  Acquire Analog Value
            global graphUpdateSpeedMs
            self.dat = deque()
            self.timer.start(graphUpdateSpeedMs)

        elif ai_mode == "Finite":

            # Start ProgressBar Thread
            self.progressBar_finite.setVisible(True)
            k = int(self.textEdit_samplesRead.toPlainText()) / float(self.textEdit_rate.toPlainText())
            self.worker_ProgressBarThread.setCounter(int(k))  # seconds
            self.worker_ProgressBarThread.start()

            # Start Finite Acquisition Thread
            self.worker_FiniteAcquisitionThread.setValues(Fs=float(self.textEdit_rate.toPlainText()),
                                                          samples_per_channel=int(self.textEdit_samplesRead.toPlainText()),
                                                          analog_task=analog_task,
                                                          graphWidget=self.graphWidget,
                                                          dat=self.dat,
                                                          curve1=self.curve1,
                                                          progressBar=self.progressBar_finite)

            self.worker_FiniteAcquisitionThread.start()

        elif ai_mode == "Continuous":

            # Start Finite Acquisition Thread
            self.worker_ContinuousAcquisitionThread.setValues(Fs=float(self.textEdit_rate.toPlainText()),
                                                              samples_per_channel=int(self.textEdit_samplesRead.toPlainText()),
                                                              analog_task=analog_task,
                                                              graphWidget=self.graphWidget,
                                                              dat=self.dat,
                                                              curve1=self.curve1)

            self.worker_ContinuousAcquisitionThread.start()

    #  --------------------------------
    #  pushButton_stop Callback Function
    #  --------------------------------
    def pushButton_stop_clicked(self):
        #  Enable/Disable Components
        self.pushButton_stop.setEnabled(False)
        self.pushButton_start.setEnabled(True)
        self.frame_parameters.setEnabled(True)

        #  Check Acquisition Mode
        global ai_mode
        if ai_mode == "On Demand":
            # Stop Graph
            self.timer.stop()

        elif ai_mode == "Finite":
            print("TODO - Change code in QThreads to implement stop button")

        elif ai_mode == "Continuous":
            global queue_flag
            queue_flag.put(True)

        # Stop Task
        global analog_task, analog_task_flag
        analog_task.stop()
        analog_task.close()

        analog_task_flag = False

    #  --------------------------------
    #  closeEvent Callback Function
    #  --------------------------------
    def closeEvent(self, *args, **kwargs):
        global analog_task, analog_task_flag
        if analog_task_flag:
            analog_task.stop()
            analog_task.close()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec())
