# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MainWindow.ui'
#
# Created: Tue Nov  4 15:50:54 2014
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

import matplotlib
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
from matplotlib.figure import Figure

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(823, 1136)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.graphicsView = QtGui.QGraphicsView(self.centralwidget)
        self.graphicsView.setGeometry(QtCore.QRect(20, 150, 741, 411))
        self.graphicsView.setObjectName(_fromUtf8("graphicsView"))
        self.pushButton_browse = QtGui.QPushButton(self.centralwidget)
        self.pushButton_browse.setGeometry(QtCore.QRect(530, 30, 61, 27))
        self.pushButton_browse.setObjectName(_fromUtf8("pushButton_browse"))
        self.lineEdit = QtGui.QLineEdit(self.centralwidget)
        self.lineEdit.setGeometry(QtCore.QRect(130, 30, 381, 27))
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.horizontalSlider = QtGui.QSlider(self.centralwidget)
        self.horizontalSlider.setGeometry(QtCore.QRect(20, 570, 301, 29))
        self.horizontalSlider.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSlider.setTickPosition(QtGui.QSlider.TicksAbove)
        self.horizontalSlider.setObjectName(_fromUtf8("horizontalSlider"))
        self.verticalSlider = QtGui.QSlider(self.centralwidget)
        self.verticalSlider.setGeometry(QtCore.QRect(770, 150, 29, 121))
        self.verticalSlider.setOrientation(QtCore.Qt.Vertical)
        self.verticalSlider.setTickPosition(QtGui.QSlider.TicksAbove)
        self.verticalSlider.setObjectName(_fromUtf8("verticalSlider"))
        self.comboBox = QtGui.QComboBox(self.centralwidget)
        self.comboBox.setGeometry(QtCore.QRect(130, 80, 461, 27))
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.pushButton_3 = QtGui.QPushButton(self.centralwidget)
        self.pushButton_3.setGeometry(QtCore.QRect(610, 80, 98, 27))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.lineEdit_3 = QtGui.QLineEdit(self.centralwidget)
        self.lineEdit_3.setGeometry(QtCore.QRect(120, 600, 151, 27))
        self.lineEdit_3.setObjectName(_fromUtf8("lineEdit_3"))
        self.lineEdit_4 = QtGui.QLineEdit(self.centralwidget)
        self.lineEdit_4.setGeometry(QtCore.QRect(552, 600, 161, 27))
        self.lineEdit_4.setObjectName(_fromUtf8("lineEdit_4"))
        self.label_3 = QtGui.QLabel(self.centralwidget)
        self.label_3.setGeometry(QtCore.QRect(20, 600, 101, 17))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.label_4 = QtGui.QLabel(self.centralwidget)
        self.label_4.setGeometry(QtCore.QRect(440, 600, 101, 17))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.horizontalSlider_2 = QtGui.QSlider(self.centralwidget)
        self.horizontalSlider_2.setGeometry(QtCore.QRect(460, 570, 301, 29))
        self.horizontalSlider_2.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSlider_2.setTickPosition(QtGui.QSlider.TicksAbove)
        self.horizontalSlider_2.setObjectName(_fromUtf8("horizontalSlider_2"))
        self.verticalSlider_2 = QtGui.QSlider(self.centralwidget)
        self.verticalSlider_2.setGeometry(QtCore.QRect(770, 440, 29, 121))
        self.verticalSlider_2.setOrientation(QtCore.Qt.Vertical)
        self.verticalSlider_2.setTickPosition(QtGui.QSlider.TicksBelow)
        self.verticalSlider_2.setObjectName(_fromUtf8("verticalSlider_2"))
        self.pushButton_refreshWS = QtGui.QPushButton(self.centralwidget)
        self.pushButton_refreshWS.setGeometry(QtCore.QRect(20, 80, 91, 27))
        self.pushButton_refreshWS.setObjectName(_fromUtf8("pushButton_refreshWS"))
        self.pushButton_setT0 = QtGui.QPushButton(self.centralwidget)
        self.pushButton_setT0.setGeometry(QtCore.QRect(280, 600, 41, 27))
        self.pushButton_setT0.setObjectName(_fromUtf8("pushButton_setT0"))
        self.pushButton_setTf = QtGui.QPushButton(self.centralwidget)
        self.pushButton_setTf.setGeometry(QtCore.QRect(720, 600, 41, 27))
        self.pushButton_setTf.setObjectName(_fromUtf8("pushButton_setTf"))
        self.label_lognamevalue = QtGui.QLabel(self.centralwidget)
        self.label_lognamevalue.setGeometry(QtCore.QRect(160, 130, 101, 17))
        self.label_lognamevalue.setObjectName(_fromUtf8("label_lognamevalue"))
        self.label_logname = QtGui.QLabel(self.centralwidget)
        self.label_logname.setGeometry(QtCore.QRect(20, 130, 81, 17))
        self.label_logname.setObjectName(_fromUtf8("label_logname"))
        self.filterTab = QtGui.QTabWidget(self.centralwidget)
        self.filterTab.setGeometry(QtCore.QRect(20, 690, 741, 391))
        self.filterTab.setObjectName(_fromUtf8("filterTab"))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.comboBox_2 = QtGui.QComboBox(self.tab)
        self.comboBox_2.setGeometry(QtCore.QRect(150, 20, 421, 27))
        self.comboBox_2.setObjectName(_fromUtf8("comboBox_2"))
        self.label_7 = QtGui.QLabel(self.tab)
        self.label_7.setGeometry(QtCore.QRect(20, 20, 81, 17))
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.label_8 = QtGui.QLabel(self.tab)
        self.label_8.setGeometry(QtCore.QRect(20, 140, 121, 17))
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.label_9 = QtGui.QLabel(self.tab)
        self.label_9.setGeometry(QtCore.QRect(390, 140, 111, 17))
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.label_10 = QtGui.QLabel(self.tab)
        self.label_10.setGeometry(QtCore.QRect(20, 190, 111, 17))
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.label_11 = QtGui.QLabel(self.tab)
        self.label_11.setGeometry(QtCore.QRect(350, 190, 161, 17))
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.pushButton_4 = QtGui.QPushButton(self.tab)
        self.pushButton_4.setGeometry(QtCore.QRect(600, 20, 98, 27))
        self.pushButton_4.setObjectName(_fromUtf8("pushButton_4"))
        self.label_19 = QtGui.QLabel(self.tab)
        self.label_19.setGeometry(QtCore.QRect(10, 240, 151, 17))
        self.label_19.setObjectName(_fromUtf8("label_19"))
        self.label_20 = QtGui.QLabel(self.tab)
        self.label_20.setGeometry(QtCore.QRect(40, 290, 111, 17))
        self.label_20.setObjectName(_fromUtf8("label_20"))
        self.label_21 = QtGui.QLabel(self.tab)
        self.label_21.setGeometry(QtCore.QRect(420, 240, 101, 17))
        self.label_21.setObjectName(_fromUtf8("label_21"))
        self.lineEdit_5 = QtGui.QLineEdit(self.tab)
        self.lineEdit_5.setGeometry(QtCore.QRect(150, 140, 191, 27))
        self.lineEdit_5.setObjectName(_fromUtf8("lineEdit_5"))
        self.lineEdit_6 = QtGui.QLineEdit(self.tab)
        self.lineEdit_6.setGeometry(QtCore.QRect(530, 140, 181, 27))
        self.lineEdit_6.setObjectName(_fromUtf8("lineEdit_6"))
        self.lineEdit_7 = QtGui.QLineEdit(self.tab)
        self.lineEdit_7.setGeometry(QtCore.QRect(150, 190, 191, 27))
        self.lineEdit_7.setObjectName(_fromUtf8("lineEdit_7"))
        self.lineEdit_8 = QtGui.QLineEdit(self.tab)
        self.lineEdit_8.setGeometry(QtCore.QRect(150, 240, 191, 27))
        self.lineEdit_8.setObjectName(_fromUtf8("lineEdit_8"))
        self.lineEdit_9 = QtGui.QLineEdit(self.tab)
        self.lineEdit_9.setGeometry(QtCore.QRect(150, 290, 191, 27))
        self.lineEdit_9.setObjectName(_fromUtf8("lineEdit_9"))
        self.comboBox_4 = QtGui.QComboBox(self.tab)
        self.comboBox_4.setGeometry(QtCore.QRect(530, 190, 181, 27))
        self.comboBox_4.setObjectName(_fromUtf8("comboBox_4"))
        self.comboBox_5 = QtGui.QComboBox(self.tab)
        self.comboBox_5.setGeometry(QtCore.QRect(530, 240, 181, 27))
        self.comboBox_5.setObjectName(_fromUtf8("comboBox_5"))
        self.pushButton_filterLog = QtGui.QPushButton(self.tab)
        self.pushButton_filterLog.setGeometry(QtCore.QRect(530, 290, 181, 41))
        self.pushButton_filterLog.setObjectName(_fromUtf8("pushButton_filterLog"))
        self.label_mean = QtGui.QLabel(self.tab)
        self.label_mean.setGeometry(QtCore.QRect(30, 70, 66, 17))
        self.label_mean.setObjectName(_fromUtf8("label_mean"))
        self.label_meanvalue = QtGui.QLabel(self.tab)
        self.label_meanvalue.setGeometry(QtCore.QRect(170, 70, 111, 17))
        self.label_meanvalue.setObjectName(_fromUtf8("label_meanvalue"))
        self.label_avg = QtGui.QLabel(self.tab)
        self.label_avg.setGeometry(QtCore.QRect(390, 70, 101, 17))
        self.label_avg.setObjectName(_fromUtf8("label_avg"))
        self.label_timeAvgValue = QtGui.QLabel(self.tab)
        self.label_timeAvgValue.setGeometry(QtCore.QRect(530, 70, 111, 17))
        self.label_timeAvgValue.setObjectName(_fromUtf8("label_timeAvgValue"))
        self.label_freq = QtGui.QLabel(self.tab)
        self.label_freq.setGeometry(QtCore.QRect(30, 100, 81, 17))
        self.label_freq.setObjectName(_fromUtf8("label_freq"))
        self.label_freqValue = QtGui.QLabel(self.tab)
        self.label_freqValue.setGeometry(QtCore.QRect(170, 100, 111, 17))
        self.label_freqValue.setObjectName(_fromUtf8("label_freqValue"))
        self.label_logsize = QtGui.QLabel(self.tab)
        self.label_logsize.setGeometry(QtCore.QRect(390, 100, 101, 17))
        self.label_logsize.setObjectName(_fromUtf8("label_logsize"))
        self.label_logsizevalue = QtGui.QLabel(self.tab)
        self.label_logsizevalue.setGeometry(QtCore.QRect(530, 100, 111, 17))
        self.label_logsizevalue.setObjectName(_fromUtf8("label_logsizevalue"))
        self.filterTab.addTab(self.tab, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.lineEdit_timeInterval = QtGui.QLineEdit(self.tab_2)
        self.lineEdit_timeInterval.setGeometry(QtCore.QRect(250, 100, 181, 27))
        self.lineEdit_timeInterval.setObjectName(_fromUtf8("lineEdit_timeInterval"))
        self.label_6 = QtGui.QLabel(self.tab_2)
        self.label_6.setGeometry(QtCore.QRect(110, 110, 101, 17))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.pushButton_filterTime = QtGui.QPushButton(self.tab_2)
        self.pushButton_filterTime.setGeometry(QtCore.QRect(250, 160, 181, 61))
        self.pushButton_filterTime.setObjectName(_fromUtf8("pushButton_filterTime"))
        self.filterTab.addTab(self.tab_2, _fromUtf8(""))
        self.tab_3 = QtGui.QWidget()
        self.tab_3.setObjectName(_fromUtf8("tab_3"))
        self.checkBox_filterByPulse = QtGui.QCheckBox(self.tab_3)
        self.checkBox_filterByPulse.setGeometry(QtCore.QRect(40, 270, 181, 22))
        self.checkBox_filterByPulse.setObjectName(_fromUtf8("checkBox_filterByPulse"))
        self.checkBox_groupWS = QtGui.QCheckBox(self.tab_3)
        self.checkBox_groupWS.setGeometry(QtCore.QRect(40, 310, 211, 22))
        self.checkBox_groupWS.setObjectName(_fromUtf8("checkBox_groupWS"))
        self.checkBox_from1 = QtGui.QCheckBox(self.tab_3)
        self.checkBox_from1.setGeometry(QtCore.QRect(360, 270, 271, 22))
        self.checkBox_from1.setObjectName(_fromUtf8("checkBox_from1"))
        self.checkBox_splitLog = QtGui.QCheckBox(self.tab_3)
        self.checkBox_splitLog.setGeometry(QtCore.QRect(360, 310, 271, 22))
        self.checkBox_splitLog.setObjectName(_fromUtf8("checkBox_splitLog"))
        self.comboBox_tofCorr = QtGui.QComboBox(self.tab_3)
        self.comboBox_tofCorr.setGeometry(QtCore.QRect(260, 30, 341, 27))
        self.comboBox_tofCorr.setObjectName(_fromUtf8("comboBox_tofCorr"))
        self.comboBox_tofCorr.addItem(_fromUtf8(""))
        self.comboBox_tofCorr.addItem(_fromUtf8(""))
        self.comboBox_tofCorr.addItem(_fromUtf8(""))
        self.comboBox_tofCorr.addItem(_fromUtf8(""))
        self.comboBox_tofCorr.addItem(_fromUtf8(""))
        self.comboBox_skipSpectrum = QtGui.QComboBox(self.tab_3)
        self.comboBox_skipSpectrum.setGeometry(QtCore.QRect(260, 220, 351, 27))
        self.comboBox_skipSpectrum.setObjectName(_fromUtf8("comboBox_skipSpectrum"))
        self.comboBox_skipSpectrum.addItem(_fromUtf8(""))
        self.comboBox_skipSpectrum.addItem(_fromUtf8(""))
        self.label_12 = QtGui.QLabel(self.tab_3)
        self.label_12.setGeometry(QtCore.QRect(40, 40, 181, 20))
        self.label_12.setObjectName(_fromUtf8("label_12"))
        self.label_13 = QtGui.QLabel(self.tab_3)
        self.label_13.setGeometry(QtCore.QRect(40, 230, 191, 17))
        self.label_13.setObjectName(_fromUtf8("label_13"))
        self.label_Ei = QtGui.QLabel(self.tab_3)
        self.label_Ei.setGeometry(QtCore.QRect(110, 80, 121, 21))
        self.label_Ei.setObjectName(_fromUtf8("label_Ei"))
        self.lineEdit_Ei = QtGui.QLineEdit(self.tab_3)
        self.lineEdit_Ei.setGeometry(QtCore.QRect(260, 80, 341, 27))
        self.lineEdit_Ei.setObjectName(_fromUtf8("lineEdit_Ei"))
        self.label_Ei_2 = QtGui.QLabel(self.tab_3)
        self.label_Ei_2.setGeometry(QtCore.QRect(40, 110, 191, 21))
        self.label_Ei_2.setObjectName(_fromUtf8("label_Ei_2"))
        self.comboBox_corrWS = QtGui.QComboBox(self.tab_3)
        self.comboBox_corrWS.setGeometry(QtCore.QRect(260, 110, 341, 27))
        self.comboBox_corrWS.setObjectName(_fromUtf8("comboBox_corrWS"))
        self.pushButton_refreshCorrWSList = QtGui.QPushButton(self.tab_3)
        self.pushButton_refreshCorrWSList.setGeometry(QtCore.QRect(620, 110, 81, 27))
        self.pushButton_refreshCorrWSList.setObjectName(_fromUtf8("pushButton_refreshCorrWSList"))
        self.checkBox_fastLog = QtGui.QCheckBox(self.tab_3)
        self.checkBox_fastLog.setGeometry(QtCore.QRect(90, 170, 97, 22))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        font.setStrikeOut(False)
        self.checkBox_fastLog.setFont(font)
        self.checkBox_fastLog.setObjectName(_fromUtf8("checkBox_fastLog"))
        self.checkBox_doParallel = QtGui.QCheckBox(self.tab_3)
        self.checkBox_doParallel.setGeometry(QtCore.QRect(260, 170, 231, 22))
        font = QtGui.QFont()
        font.setBold(True)
        font.setWeight(75)
        self.checkBox_doParallel.setFont(font)
        self.checkBox_doParallel.setObjectName(_fromUtf8("checkBox_doParallel"))
        self.line = QtGui.QFrame(self.tab_3)
        self.line.setGeometry(QtCore.QRect(0, 140, 741, 21))
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName(_fromUtf8("line"))
        self.line_2 = QtGui.QFrame(self.tab_3)
        self.line_2.setGeometry(QtCore.QRect(0, 200, 741, 16))
        self.line_2.setFrameShape(QtGui.QFrame.HLine)
        self.line_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_2.setObjectName(_fromUtf8("line_2"))
        self.filterTab.addTab(self.tab_3, _fromUtf8(""))
        self.widget = QtGui.QWidget()
        self.widget.setObjectName(_fromUtf8("widget"))
        self.plainTextEdit_ErrorMsg = QtGui.QPlainTextEdit(self.widget)
        self.plainTextEdit_ErrorMsg.setGeometry(QtCore.QRect(30, 20, 681, 281))
        self.plainTextEdit_ErrorMsg.setObjectName(_fromUtf8("plainTextEdit_ErrorMsg"))
        self.pushButton_clearerror = QtGui.QPushButton(self.widget)
        self.pushButton_clearerror.setGeometry(QtCore.QRect(30, 320, 681, 27))
        self.pushButton_clearerror.setObjectName(_fromUtf8("pushButton_clearerror"))
        self.filterTab.addTab(self.widget, _fromUtf8(""))
        self.lineEdit_title = QtGui.QLineEdit(self.centralwidget)
        self.lineEdit_title.setGeometry(QtCore.QRect(430, 650, 331, 27))
        self.lineEdit_title.setObjectName(_fromUtf8("lineEdit_title"))
        self.label_5 = QtGui.QLabel(self.centralwidget)
        self.label_5.setGeometry(QtCore.QRect(340, 650, 91, 20))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.label_outwsname = QtGui.QLabel(self.centralwidget)
        self.label_outwsname.setGeometry(QtCore.QRect(20, 650, 101, 20))
        self.label_outwsname.setObjectName(_fromUtf8("label_outwsname"))
        self.lineEdit_outwsname = QtGui.QLineEdit(self.centralwidget)
        self.lineEdit_outwsname.setGeometry(QtCore.QRect(120, 650, 211, 27))
        self.lineEdit_outwsname.setObjectName(_fromUtf8("lineEdit_outwsname"))
        self.label_error = QtGui.QLabel(self.centralwidget)
        self.label_error.setGeometry(QtCore.QRect(730, 20, 81, 41))
        font = QtGui.QFont()
        font.setPointSize(14)
        font.setBold(True)
        font.setItalic(True)
        font.setWeight(75)
        self.label_error.setFont(font)
        self.label_error.setObjectName(_fromUtf8("label_error"))
        self.label_logname_2 = QtGui.QLabel(self.centralwidget)
        self.label_logname_2.setGeometry(QtCore.QRect(20, 30, 111, 17))
        self.label_logname_2.setObjectName(_fromUtf8("label_logname_2"))
        self.pushButton_load = QtGui.QPushButton(self.centralwidget)
        self.pushButton_load.setGeometry(QtCore.QRect(610, 30, 98, 27))
        self.pushButton_load.setObjectName(_fromUtf8("pushButton_load"))
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 823, 25))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)

        self.figure = Figure((4.0, 3.0), dpi=100)
        self.theplot = self.figure.add_subplot(111)
        self.graphicsView = FigureCanvas(self.figure)
        self.graphicsView.setParent(self.centralwidget)
        self.graphicsView.setGeometry(QtCore.QRect(20, 150, 741, 411))
        self.graphicsView.setObjectName(_fromUtf8("graphicsView"))

        self.retranslateUi(MainWindow)
        self.filterTab.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_browse.setText(QtGui.QApplication.translate("MainWindow", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEdit.setToolTip(QtGui.QApplication.translate("MainWindow", "<html><head/><body><p>The name of the NeXus file or the run number to load. </p><p><br/></p><p>Run number should be InstrumentName_RunNumber (for example, NOM_11788)</p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_3.setText(QtGui.QApplication.translate("MainWindow", "Use", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("MainWindow", "Starting Time", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("MainWindow", "Stopping Time", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_refreshWS.setText(QtGui.QApplication.translate("MainWindow", "Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_setT0.setText(QtGui.QApplication.translate("MainWindow", "Set", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_setTf.setText(QtGui.QApplication.translate("MainWindow", "Set", None, QtGui.QApplication.UnicodeUTF8))
        self.label_lognamevalue.setText(QtGui.QApplication.translate("MainWindow", "?", None, QtGui.QApplication.UnicodeUTF8))
        self.label_logname.setText(QtGui.QApplication.translate("MainWindow", "Log Name", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("MainWindow", "Sample Log", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("MainWindow", "Minimum Value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("MainWindow", "Maximum Value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("MainWindow", "Log Step Size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("MainWindow", "Value Change Direction", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_4.setText(QtGui.QApplication.translate("MainWindow", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.label_19.setText(QtGui.QApplication.translate("MainWindow", "Log Value Tolerance", None, QtGui.QApplication.UnicodeUTF8))
        self.label_20.setText(QtGui.QApplication.translate("MainWindow", "Time Tolerance", None, QtGui.QApplication.UnicodeUTF8))
        self.label_21.setText(QtGui.QApplication.translate("MainWindow", "Log Boundary", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_filterLog.setText(QtGui.QApplication.translate("MainWindow", "Filter", None, QtGui.QApplication.UnicodeUTF8))
        self.label_mean.setText(QtGui.QApplication.translate("MainWindow", "Mean", None, QtGui.QApplication.UnicodeUTF8))
        self.label_meanvalue.setText(QtGui.QApplication.translate("MainWindow", "?", None, QtGui.QApplication.UnicodeUTF8))
        self.label_avg.setText(QtGui.QApplication.translate("MainWindow", "Time Average", None, QtGui.QApplication.UnicodeUTF8))
        self.label_timeAvgValue.setText(QtGui.QApplication.translate("MainWindow", "?", None, QtGui.QApplication.UnicodeUTF8))
        self.label_freq.setText(QtGui.QApplication.translate("MainWindow", "Frequency", None, QtGui.QApplication.UnicodeUTF8))
        self.label_freqValue.setText(QtGui.QApplication.translate("MainWindow", "?", None, QtGui.QApplication.UnicodeUTF8))
        self.label_logsize.setText(QtGui.QApplication.translate("MainWindow", "Log Size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_logsizevalue.setText(QtGui.QApplication.translate("MainWindow", "?", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTab.setTabText(self.filterTab.indexOf(self.tab), QtGui.QApplication.translate("MainWindow", "Filter By Log", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("MainWindow", "Time Interval", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_filterTime.setText(QtGui.QApplication.translate("MainWindow", "Filter", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTab.setTabText(self.filterTab.indexOf(self.tab_2), QtGui.QApplication.translate("MainWindow", "Filter By Time", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_filterByPulse.setText(QtGui.QApplication.translate("MainWindow", "Filter By Pulse Time", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_groupWS.setText(QtGui.QApplication.translate("MainWindow", "Group Output Workspace", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_from1.setText(QtGui.QApplication.translate("MainWindow", "Output Workspace Indexed From 1", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_splitLog.setText(QtGui.QApplication.translate("MainWindow", "Split Sample Logs", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox_tofCorr.setItemText(0, QtGui.QApplication.translate("MainWindow", "None", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox_tofCorr.setItemText(1, QtGui.QApplication.translate("MainWindow", "Elastic", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox_tofCorr.setItemText(2, QtGui.QApplication.translate("MainWindow", "Direct", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox_tofCorr.setItemText(3, QtGui.QApplication.translate("MainWindow", "Indirect", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox_tofCorr.setItemText(4, QtGui.QApplication.translate("MainWindow", "Customized", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox_skipSpectrum.setItemText(0, QtGui.QApplication.translate("MainWindow", "Skip", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox_skipSpectrum.setItemText(1, QtGui.QApplication.translate("MainWindow", "Skip only if TOF correction", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("MainWindow", "TOF Correction To Sample", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("MainWindow", "Spectrum without Detector", None, QtGui.QApplication.UnicodeUTF8))
        self.label_Ei.setText(QtGui.QApplication.translate("MainWindow", "Incident Energy", None, QtGui.QApplication.UnicodeUTF8))
        self.label_Ei_2.setText(QtGui.QApplication.translate("MainWindow", "TOF Correction Workspace", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_refreshCorrWSList.setText(QtGui.QApplication.translate("MainWindow", "Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_fastLog.setText(QtGui.QApplication.translate("MainWindow", "Fast Log", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_doParallel.setText(QtGui.QApplication.translate("MainWindow", "Generate Filter In Parallel", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTab.setTabText(self.filterTab.indexOf(self.tab_3), QtGui.QApplication.translate("MainWindow", "Advanced Setup", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_clearerror.setText(QtGui.QApplication.translate("MainWindow", "CLEAR", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTab.setTabText(self.filterTab.indexOf(self.widget), QtGui.QApplication.translate("MainWindow", "Error Message", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("MainWindow", "Splitter Title", None, QtGui.QApplication.UnicodeUTF8))
        self.label_outwsname.setText(QtGui.QApplication.translate("MainWindow", "Output Name", None, QtGui.QApplication.UnicodeUTF8))
        self.label_error.setText(QtGui.QApplication.translate("MainWindow", "<html><head/><body><p><span style=\" color:#ff0000;\">ERROR!</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.label_logname_2.setToolTip(QtGui.QApplication.translate("MainWindow", "<html><head/><body><p>The name of the NeXus file or the run number to load</p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.label_logname_2.setWhatsThis(QtGui.QApplication.translate("MainWindow", "<html><head/><body><p>Label for file name or run number</p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.label_logname_2.setText(QtGui.QApplication.translate("MainWindow", "File / Run ", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_load.setText(QtGui.QApplication.translate("MainWindow", "Load", None, QtGui.QApplication.UnicodeUTF8))

