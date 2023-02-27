import sys
from PyQt6 import QtCore, QtGui, QtWidgets, uic
from PyQt6.QtWidgets import QApplication

import pyqtgraph as pg
from pyqtgraph import PlotWidget
from pyqtgraph.Qt import QtGui, QtCore

import nidaqmx
from nidaqmx import constants

from collections import deque

device_name = "6001_Dev1/ai0"
analog_task = None
analog_task_flag = False
graphUpdateSpeedMs = 50  # Default


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Load ui File
        uic.loadUi("ni_max_onDemand.ui", self)

        # Initialize Components on the GUI
        self.comboBox_channelName.addItem(device_name)
        self.comboBox_mode.addItem("On Demand")
        self.comboBox_inputConfig.addItem("RSE")

        # Callback Functions
        self.pushButton_start.clicked.connect(self.pushButton_start_clicked)
        self.pushButton_stop.clicked.connect(self.pushButton_stop_clicked)
        self.checkBox_scale.stateChanged.connect(self.checkBox_scale_stateChanged)

        # Initialize plot
        self.dat = deque()
        self.maxLen = 100  # max number of data points to show on graph

        self.curve1 = self.graphWidget.plot()

        self.timer = QtCore.QTimer()  # to create a thread that calls a function at intervals
        self.timer.timeout.connect(self.update)  # the update function keeps getting called at intervals

    #  --------------------------------
    #  update Callback Function
    #  --------------------------------
    def update(self):
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

        #  Start Task
        analog_task.start()
        analog_task_flag = True

        #  Acquire Analog Value
        global graphUpdateSpeedMs
        self.timer.start(graphUpdateSpeedMs)

    #  --------------------------------
    #  pushButton_stop Callback Function
    #  --------------------------------
    def pushButton_stop_clicked(self):
        #  Enable/Disable Components
        self.pushButton_stop.setEnabled(False)
        self.pushButton_start.setEnabled(True)
        self.frame_parameters.setEnabled(True)

        # Stop Graph
        self.timer.stop()

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
