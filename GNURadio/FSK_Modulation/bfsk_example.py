#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: BFSK Example
# GNU Radio version: 3.10.3.0

from packaging.version import Version as StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from gnuradio import analog
from gnuradio import blocks
from gnuradio import filter
from gnuradio import gr
from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio.qtgui import Range, RangeWidget
from PyQt5 import QtCore



from gnuradio import qtgui

class bfsk_example(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "BFSK Example", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("BFSK Example")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "bfsk_example")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 32e3
        self.fsk_deviation = fsk_deviation = 500
        self.center_freq = center_freq = 1.5e3

        ##################################################
        # Blocks
        ##################################################
        self._fsk_deviation_range = Range(100, 10e3, 500, 500, 200)
        self._fsk_deviation_win = RangeWidget(self._fsk_deviation_range, self.set_fsk_deviation, "FSK Deviation", "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._fsk_deviation_win)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_f(
            4096, #size
            window.WIN_BLACKMAN_hARRIS, #wintype
            center_freq, #fc
            samp_rate, #bw
            "", #name
            1,
            None # parent
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.010)
        self.qtgui_freq_sink_x_0.set_y_axis((-140), 10)
        self.qtgui_freq_sink_x_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)
        self.qtgui_freq_sink_x_0.set_fft_window_normalized(False)


        self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)

        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_win)
        self.qtgui_const_sink_x_0 = qtgui.const_sink_c(
            1024, #size
            "", #name
            1, #number of inputs
            None # parent
        )
        self.qtgui_const_sink_x_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0.set_y_axis((-2), 2)
        self.qtgui_const_sink_x_0.set_x_axis((-2), 2)
        self.qtgui_const_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, "")
        self.qtgui_const_sink_x_0.enable_autoscale(False)
        self.qtgui_const_sink_x_0.enable_grid(False)
        self.qtgui_const_sink_x_0.enable_axis_labels(True)


        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "red", "red", "red",
            "red", "red", "red", "red", "red"]
        styles = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_const_sink_x_0_win)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(1, firdes.low_pass(1.0,samp_rate,900,300), center_freq, samp_rate)
        self.blocks_vector_source_x_0 = blocks.vector_source_f((1, -1, 1, 1, 1, -1), True, 1, [])
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_repeat_0 = blocks.repeat(gr.sizeof_float*1, 800)
        self.blocks_multiply_xx_1 = blocks.multiply_vff(1)
        self.blocks_multiply_xx_0_0_0 = blocks.multiply_vff(1)
        self.blocks_multiply_xx_0_0 = blocks.multiply_vff(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vff(1)
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)
        self.blocks_add_xx_0_0_0 = blocks.add_vff(1)
        self.blocks_add_xx_0_0 = blocks.add_vff(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.analog_sig_source_x_0_0 = analog.sig_source_f(samp_rate, analog.GR_COS_WAVE, (center_freq-fsk_deviation), 1, 0, 0)
        self.analog_sig_source_x_0 = analog.sig_source_f(samp_rate, analog.GR_COS_WAVE, (center_freq+fsk_deviation), 1, 0, 0)
        self.analog_frequency_modulator_fc_0 = analog.frequency_modulator_fc(0.039)
        self.analog_const_source_x_0_0_0_1 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0.5)
        self.analog_const_source_x_0_0_0_0_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, (-1))
        self.analog_const_source_x_0_0_0_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 1)
        self.analog_const_source_x_0_0_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0.5)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_const_source_x_0_0_0, 0), (self.blocks_multiply_xx_0_0, 0))
        self.connect((self.analog_const_source_x_0_0_0_0, 0), (self.blocks_add_xx_0_0, 0))
        self.connect((self.analog_const_source_x_0_0_0_0_0, 0), (self.blocks_add_xx_0_0_0, 1))
        self.connect((self.analog_const_source_x_0_0_0_1, 0), (self.blocks_multiply_xx_0_0_0, 1))
        self.connect((self.analog_frequency_modulator_fc_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.analog_sig_source_x_0_0, 0), (self.blocks_multiply_xx_1, 1))
        self.connect((self.blocks_add_xx_0, 0), (self.analog_frequency_modulator_fc_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_add_xx_0_0, 0), (self.blocks_multiply_xx_0_0, 1))
        self.connect((self.blocks_add_xx_0_0_0, 0), (self.blocks_multiply_xx_0_0_0, 0))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.qtgui_freq_sink_x_0, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self.qtgui_const_sink_x_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_multiply_xx_0_0_0, 0), (self.blocks_multiply_xx_1, 0))
        self.connect((self.blocks_multiply_xx_1, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_repeat_0, 0), (self.blocks_add_xx_0_0, 1))
        self.connect((self.blocks_repeat_0, 0), (self.blocks_add_xx_0_0_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.blocks_vector_source_x_0, 0), (self.blocks_repeat_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.blocks_throttle_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "bfsk_example")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.analog_sig_source_x_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_0_0.set_sampling_freq(self.samp_rate)
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.freq_xlating_fir_filter_xxx_0.set_taps(firdes.low_pass(1.0,self.samp_rate,900,300))
        self.qtgui_freq_sink_x_0.set_frequency_range(self.center_freq, self.samp_rate)

    def get_fsk_deviation(self):
        return self.fsk_deviation

    def set_fsk_deviation(self, fsk_deviation):
        self.fsk_deviation = fsk_deviation
        self.analog_sig_source_x_0.set_frequency((self.center_freq+self.fsk_deviation))
        self.analog_sig_source_x_0_0.set_frequency((self.center_freq-self.fsk_deviation))

    def get_center_freq(self):
        return self.center_freq

    def set_center_freq(self, center_freq):
        self.center_freq = center_freq
        self.analog_sig_source_x_0.set_frequency((self.center_freq+self.fsk_deviation))
        self.analog_sig_source_x_0_0.set_frequency((self.center_freq-self.fsk_deviation))
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.center_freq)
        self.qtgui_freq_sink_x_0.set_frequency_range(self.center_freq, self.samp_rate)




def main(top_block_cls=bfsk_example, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
