import sys

import numpy as np
from PyQt6 import QtCore, QtGui, QtWidgets, uic
from PyQt6.QtWidgets import QApplication


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        #uic.loadUi("ni_daq_windowConcurrency.ui", self)

        self.timer = QtCore.QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.timer_task)
        self.timer.start()

    @staticmethod
    def timer_task():
        print("Working...")

    def closeEvent(self, *args, **kwargs):
        print("App Closed")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = MainWindow()
    #win.show()
    sys.exit(app.exec())