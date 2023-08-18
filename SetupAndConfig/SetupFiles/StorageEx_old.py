# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'StorageEx.ui'
#
# Created by: PyQt4 UI code generator 4.11.4
#
# WARNING! All changes made in this file will be lost!
import sys
from PyQt4 import QtCore, QtGui
import globals
import TextCn
import ctypes

sys.path.insert(0, '../../TestEngine/TestInterface/SATA')

from SATA_wrapper import *
from loggin import LogginClass
from FromJsonFiles import *
import globals

#<chandu> all the reference to other files should be moved under one class so that, creating multiple objects is reduced


b = LogginClass()
device_node = JsonFile.read_json('details',0,'drive_name' )
device_node = ctypes.c_char_p(device_node)

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

def p(x):
    print x

class Ui_Dialog(object):
    def setupUi(self, myDialog):
        myDialog.setObjectName(_fromUtf8("Dialog"))

        myDialog.resize(1337, 923)
        self.pushButton = QtGui.QPushButton(myDialog)
        self.pushButton.setGeometry(QtCore.QRect(360, 50, 211, 81))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.pushButton_2 = QtGui.QPushButton(myDialog)
        self.pushButton_2.setGeometry(QtCore.QRect(630, 50, 211, 81))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.pushButton_3 = QtGui.QPushButton(myDialog)
        self.pushButton_3.setGeometry(QtCore.QRect(900, 50, 211, 81))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.label = QtGui.QLabel(myDialog)
        self.label.setGeometry(QtCore.QRect(90, 260, 141, 41))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(myDialog)
        self.label_2.setGeometry(QtCore.QRect(90, 320, 141, 41))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.label_3 = QtGui.QLabel(myDialog)
        self.label_3.setGeometry(QtCore.QRect(90, 380, 141, 41))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.checkBox = QtGui.QCheckBox(myDialog)
        self.checkBox.setGeometry(QtCore.QRect(200, 270, 21, 21))
        self.checkBox.setIconSize(QtCore.QSize(40, 40))
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.checkBox_2 = QtGui.QCheckBox(myDialog)
        self.checkBox_2.setGeometry(QtCore.QRect(200, 330, 21, 21))
        self.checkBox_2.setObjectName(_fromUtf8("checkBox_2"))
        self.checkBox_3 = QtGui.QCheckBox(myDialog)
        self.checkBox_3.setGeometry(QtCore.QRect(200, 390, 21, 21))
        self.checkBox_3.setObjectName(_fromUtf8("checkBox_3"))
        self.label_4 = QtGui.QLabel(myDialog)
        self.label_4.setGeometry(QtCore.QRect(260, 260, 141, 41))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.label_5 = QtGui.QLabel(myDialog)
        self.label_5.setGeometry(QtCore.QRect(260, 320, 141, 41))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.label_6 = QtGui.QLabel(myDialog)
        self.label_6.setGeometry(QtCore.QRect(260, 380, 141, 41))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.textEdit = QtGui.QTextEdit(myDialog)
        self.textEdit.setGeometry(QtCore.QRect(370, 260, 191, 41))
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.textEdit_2 = QtGui.QTextEdit(myDialog)
        self.textEdit_2.setGeometry(QtCore.QRect(370, 320, 191, 41))
        self.textEdit_2.setObjectName(_fromUtf8("textEdit_2"))
        self.textEdit_3 = QtGui.QTextEdit(myDialog)
        self.textEdit_3.setGeometry(QtCore.QRect(370, 380, 191, 41))
        self.textEdit_3.setObjectName(_fromUtf8("textEdit_3"))
        self.textEdit_4 = QtGui.QTextEdit(myDialog)
        self.textEdit_4.setGeometry(QtCore.QRect(620, 320, 191, 41))
        self.textEdit_4.setObjectName(_fromUtf8("textEdit_4"))
        self.textEdit_5 = QtGui.QTextEdit(myDialog)
        self.textEdit_5.setGeometry(QtCore.QRect(620, 380, 191, 41))
        self.textEdit_5.setObjectName(_fromUtf8("textEdit_5"))
        self.textEdit_6 = QtGui.QTextEdit(myDialog)
        self.textEdit_6.setGeometry(QtCore.QRect(620, 260, 191, 41))
        self.textEdit_6.setObjectName(_fromUtf8("textEdit_6"))
        self.textEdit_7 = QtGui.QTextEdit(myDialog)
        self.textEdit_7.setGeometry(QtCore.QRect(850, 380, 191, 41))
        self.textEdit_7.setObjectName(_fromUtf8("textEdit_7"))
        self.textEdit_8 = QtGui.QTextEdit(myDialog)
        self.textEdit_8.setGeometry(QtCore.QRect(850, 320, 191, 41))
        self.textEdit_8.setObjectName(_fromUtf8("textEdit_8"))
        self.textEdit_9 = QtGui.QTextEdit(myDialog)
        self.textEdit_9.setGeometry(QtCore.QRect(850, 260, 191, 41))
        self.textEdit_9.setObjectName(_fromUtf8("textEdit_9"))
        self.textEdit_10 = QtGui.QTextEdit(myDialog)
        self.textEdit_10.setGeometry(QtCore.QRect(1070, 320, 191, 41))
        self.textEdit_10.setObjectName(_fromUtf8("textEdit_10"))
        self.textEdit_11 = QtGui.QTextEdit(myDialog)
        self.textEdit_11.setGeometry(QtCore.QRect(1070, 380, 191, 41))
        self.textEdit_11.setObjectName(_fromUtf8("textEdit_11"))
        self.textEdit_12 = QtGui.QTextEdit(myDialog)
        self.textEdit_12.setGeometry(QtCore.QRect(1070, 260, 191, 41))
        self.textEdit_12.setObjectName(_fromUtf8("textEdit_12"))
        self.label_7 = QtGui.QLabel(myDialog)
        self.label_7.setGeometry(QtCore.QRect(370, 220, 141, 41))
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.label_8 = QtGui.QLabel(myDialog)
        self.label_8.setGeometry(QtCore.QRect(620, 220, 141, 41))
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.label_9 = QtGui.QLabel(myDialog)
        self.label_9.setGeometry(QtCore.QRect(850, 220, 141, 41))
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.label_10 = QtGui.QLabel(myDialog)
        self.label_10.setGeometry(QtCore.QRect(1070, 220, 141, 41))
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.pushButton_4 = QtGui.QPushButton(myDialog)
        self.pushButton_4.setGeometry(QtCore.QRect(860, 540, 141, 51))
        self.pushButton_4.setObjectName(_fromUtf8("pushButton_4"))
        self.pushButton_5 = QtGui.QPushButton(myDialog)

        QtCore.QObject.connect(self.pushButton_5, QtCore.SIGNAL("clicked()"), self.c1_clicked)

        self.pushButton_5.setGeometry(QtCore.QRect(1020, 540, 141, 51))
        self.pushButton_5.setObjectName(_fromUtf8("pushButton_5"))
        self.progressBar = QtGui.QProgressBar(myDialog)
        self.progressBar.setGeometry(QtCore.QRect(750, 620, 411, 21))
        self.progressBar.setProperty("value", 24)
        self.progressBar.setObjectName(_fromUtf8("progressBar"))
        self.label_11 = QtGui.QLabel(myDialog)
        self.label_11.setGeometry(QtCore.QRect(615, 620, 101, 21))
        self.label_11.setObjectName(_fromUtf8("label_11"))

        self.retranslateUi(myDialog)
        QtCore.QMetaObject.connectSlotsByName(myDialog)

    def retranslateUi(self, myDialog):
        myDialog.setWindowTitle(_translate("Dialog", "Dialog", None))
        self.pushButton.setText(_translate("Dialog", "Home", None))
        self.pushButton_2.setText(_translate("Dialog", "Settings", None))
        self.pushButton_3.setText(_translate("Dialog", "EXIT", None))
        self.label.setText(_translate("Dialog", "Read", None))
        self.label_2.setText(_translate("Dialog", "Write", None))
        self.label_3.setText(_translate("Dialog", "Identify", None))
        self.checkBox.setText(_translate("Dialog", "CheckBox", None))
        self.checkBox_2.setText(_translate("Dialog", "CheckBox", None))
        self.checkBox_3.setText(_translate("Dialog", "CheckBox", None))
        self.label_4.setText(_translate("Dialog", "Drive Read", None))
        self.label_5.setText(_translate("Dialog", "Drive Write", None))
        self.label_6.setText(_translate("Dialog", "Drive Inquiry", None))
        self.label_7.setText(_translate("Dialog", "Param1", None))
        self.label_8.setText(_translate("Dialog", "Param2", None))
        self.label_9.setText(_translate("Dialog", "Param3", None))
        self.label_10.setText(_translate("Dialog", "Param4", None))
        self.pushButton_4.setText(_translate("Dialog", "Select All", None))
        self.pushButton_5.setText(_translate("Dialog", "Deselect All", None))
        self.label_11.setText(_translate("Dialog", "Test Progress", None))

    def c1_clicked(self):
        r = Storage(device_node.value).ReadSector(20000)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(1101, 614)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.tabWidget = QtGui.QTabWidget(self.centralwidget)
        self.tabWidget.setGeometry(QtCore.QRect(550, 10, 661, 541))
        self.tabWidget.setObjectName(_fromUtf8("tabWidget"))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.textEdit = QtGui.QTextEdit(self.tab)
        self.textEdit.setGeometry(QtCore.QRect(0, 0, 721, 581))
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.tabWidget.addTab(self.tab, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.textEdit_2 = QtGui.QTextEdit(self.tab_2)
        self.textEdit_2.setGeometry(QtCore.QRect(0, 0, 661, 521))
        self.textEdit_2.setObjectName(_fromUtf8("textEdit_2"))
        self.tabWidget.addTab(self.tab_2, _fromUtf8(""))
        self.tabWidget_2 = QtGui.QTabWidget(self.centralwidget)
        self.tabWidget_2.setGeometry(QtCore.QRect(180, 40, 341, 341))
        self.tabWidget_2.setObjectName(_fromUtf8("tabWidget_2"))
        self.tab_3 = QtGui.QWidget()
        self.tab_3.setObjectName(_fromUtf8("tab_3"))
        self.frame = QtGui.QFrame(self.tab_3)
        self.frame.setGeometry(QtCore.QRect(10, 10, 311, 291))
        self.frame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName(_fromUtf8("frame"))
        self.label_7 = QtGui.QLabel(self.frame)
        self.label_7.setGeometry(QtCore.QRect(10, 10, 71, 31))
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.label_8 = QtGui.QLabel(self.frame)
        self.label_8.setGeometry(QtCore.QRect(140, 10, 121, 31))
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.label_4 = QtGui.QLabel(self.frame)
        self.label_4.setGeometry(QtCore.QRect(10, 30, 61, 31))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.checkBox = QtGui.QCheckBox(self.frame)
        self.checkBox.setGeometry(QtCore.QRect(80, 40, 16, 16))
        self.checkBox.setText(_fromUtf8(""))
        self.checkBox.setObjectName(_fromUtf8("Read"))


        self.checkBox.setStyleSheet("QCheckBox:checked {color:green;}")

        self.label_10 = QtGui.QLabel(self.frame)
        self.label_10.setGeometry(QtCore.QRect(140, 40, 161, 21))
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.checkBox_2 = QtGui.QCheckBox(self.frame)
        self.checkBox_2.setGeometry(QtCore.QRect(80, 70, 16, 16))
        self.checkBox_2.setText(_fromUtf8(""))
        self.checkBox_2.setObjectName(_fromUtf8("Write"))



        self.label_5 = QtGui.QLabel(self.frame)
        self.label_5.setGeometry(QtCore.QRect(10, 60, 61, 41))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.label_11 = QtGui.QLabel(self.frame)
        self.label_11.setGeometry(QtCore.QRect(140, 70, 161, 21))
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.label_12 = QtGui.QLabel(self.frame)
        self.label_12.setGeometry(QtCore.QRect(140, 100, 161, 21))
        self.label_12.setObjectName(_fromUtf8("label_12"))
        self.checkBox_3 = QtGui.QCheckBox(self.frame)
        self.checkBox_3.setGeometry(QtCore.QRect(80, 100, 16, 16))
        self.checkBox_3.setText(_fromUtf8(""))
        self.checkBox_3.setObjectName(_fromUtf8("Identify"))


        '''
        self.palette= QtGui.QPalette(self.checkBox_3.palette())
        self.palette.setColor(QtGui.QPalette.Active,QtGui.QPalette.WindowText,QtCore.Qt.darkGreen)
        self.brush=QtGui.QBrush(QtGui.QColor(0,255,0))
        self.palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Background,self.brush)
        self.checkBox_3.setPalette(self.palette)
        '''

        self.label_6 = QtGui.QLabel(self.frame)
        self.label_6.setGeometry(QtCore.QRect(10, 90, 61, 41))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.pushButton_5 = QtGui.QPushButton(self.frame)
        self.pushButton_5.setGeometry(QtCore.QRect(90, 150, 80, 31))
        self.pushButton_5.setObjectName(_fromUtf8("pushButton_5"))
        self.pushButton_6 = QtGui.QPushButton(self.frame)
        self.pushButton_6.setGeometry(QtCore.QRect(170, 150, 90, 31))
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))
        self.progressBar = QtGui.QProgressBar(self.frame)
        self.progressBar.setGeometry(QtCore.QRect(70, 220, 171, 16))
        self.progressBar.setProperty("value", 24)
        self.progressBar.setObjectName(_fromUtf8("progressBar"))
        self.label_9 = QtGui.QLabel(self.frame)
        self.label_9.setGeometry(QtCore.QRect(90, 250, 81, 31))
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.tabWidget_2.addTab(self.tab_3, _fromUtf8(""))
        self.tab_4 = QtGui.QWidget()
        self.tab_4.setObjectName(_fromUtf8("tab_4"))
        self.frame_4 = QtGui.QFrame(self.tab_4)
        self.frame_4.setGeometry(QtCore.QRect(0, 0, 311, 291))
        self.frame_4.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_4.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_4.setObjectName(_fromUtf8("frame_4"))
        self.label_22 = QtGui.QLabel(self.frame_4)
        self.label_22.setGeometry(QtCore.QRect(10, 10, 71, 31))
        self.label_22.setObjectName(_fromUtf8("label_22"))
        self.label_23 = QtGui.QLabel(self.frame_4)
        self.label_23.setGeometry(QtCore.QRect(140, 10, 121, 31))
        self.label_23.setObjectName(_fromUtf8("label_23"))
        self.label_24 = QtGui.QLabel(self.frame_4)
        self.label_24.setGeometry(QtCore.QRect(10, 30, 61, 31))
        self.label_24.setObjectName(_fromUtf8("label_24"))
        self.checkBox_7 = QtGui.QCheckBox(self.frame_4)
        self.checkBox_7.setGeometry(QtCore.QRect(80, 40, 16, 16))
        self.checkBox_7.setText(_fromUtf8(""))
        self.checkBox_7.setObjectName(_fromUtf8("checkBox_7"))
        self.label_25 = QtGui.QLabel(self.frame_4)
        self.label_25.setGeometry(QtCore.QRect(140, 40, 161, 21))
        self.label_25.setObjectName(_fromUtf8("label_25"))
        self.checkBox_8 = QtGui.QCheckBox(self.frame_4)
        self.checkBox_8.setGeometry(QtCore.QRect(80, 70, 16, 16))
        self.checkBox_8.setText(_fromUtf8(""))
        self.checkBox_8.setObjectName(_fromUtf8("checkBox_8"))
        self.label_26 = QtGui.QLabel(self.frame_4)
        self.label_26.setGeometry(QtCore.QRect(10, 60, 61, 41))
        self.label_26.setObjectName(_fromUtf8("label_26"))
        self.label_27 = QtGui.QLabel(self.frame_4)
        self.label_27.setGeometry(QtCore.QRect(140, 70, 161, 21))
        self.label_27.setObjectName(_fromUtf8("label_27"))
        self.label_28 = QtGui.QLabel(self.frame_4)
        self.label_28.setGeometry(QtCore.QRect(140, 100, 161, 21))
        self.label_28.setObjectName(_fromUtf8("label_28"))
        self.checkBox_9 = QtGui.QCheckBox(self.frame_4)
        self.checkBox_9.setGeometry(QtCore.QRect(80, 100, 16, 16))
        self.checkBox_9.setText(_fromUtf8(""))
        self.checkBox_9.setCheckable(True)
        self.checkBox_9.setChecked(False)
        self.checkBox_9.setObjectName(_fromUtf8("checkBox_9"))
        self.label_29 = QtGui.QLabel(self.frame_4)
        self.label_29.setGeometry(QtCore.QRect(10, 90, 61, 41))
        self.label_29.setObjectName(_fromUtf8("label_29"))
        self.pushButton_9 = QtGui.QPushButton(self.frame_4)
        self.pushButton_9.setGeometry(QtCore.QRect(90, 150, 80, 31))
        self.pushButton_9.setObjectName(_fromUtf8("pushButton_9"))
        self.pushButton_10 = QtGui.QPushButton(self.frame_4)
        self.pushButton_10.setGeometry(QtCore.QRect(170, 150, 90, 31))
        self.pushButton_10.setObjectName(_fromUtf8("pushButton_10"))
        self.progressBar_3 = QtGui.QProgressBar(self.frame_4)
        self.progressBar_3.setGeometry(QtCore.QRect(70, 220, 171, 16))
        self.progressBar_3.setProperty("value", 24)
        self.progressBar_3.setObjectName(_fromUtf8("progressBar_3"))
        self.label_30 = QtGui.QLabel(self.frame_4)
        self.label_30.setGeometry(QtCore.QRect(90, 250, 81, 31))
        self.label_30.setObjectName(_fromUtf8("label_30"))
        self.tabWidget_2.addTab(self.tab_4, _fromUtf8(""))
        self.executionpanel = QtGui.QFrame(self.centralwidget)
        self.executionpanel.setGeometry(QtCore.QRect(20, 140, 131, 341))
        self.executionpanel.setFrameShape(QtGui.QFrame.StyledPanel)
        self.executionpanel.setFrameShadow(QtGui.QFrame.Raised)
        self.executionpanel.setObjectName(_fromUtf8("executionpanel"))
        self.label = QtGui.QLabel(self.executionpanel)
        self.label.setGeometry(QtCore.QRect(10, 10, 211, 21))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(self.executionpanel)
        self.label_2.setGeometry(QtCore.QRect(10, 80, 121, 16))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.label_3 = QtGui.QLabel(self.executionpanel)
        self.label_3.setGeometry(QtCore.QRect(10, 150, 151, 21))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.pushButton = QtGui.QPushButton(self.executionpanel)
        self.pushButton.setGeometry(QtCore.QRect(10, 220, 95, 21))
        self.pushButton.setCheckable(True)
        self.pushButton.setFlat(False)
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        QtCore.QObject.connect(self.pushButton, QtCore.SIGNAL("clicked()"), self.b1_clicked)

        self.pushButton.setDisabled(1)
        QtCore.QObject.connect(self.pushButton_9, QtCore.SIGNAL("clicked()"), self.b2_clicked)

        QtCore.QObject.connect(self.pushButton_5, QtCore.SIGNAL("clicked()"), self.b2_clicked)

        QtCore.QObject.connect(self.pushButton_6, QtCore.SIGNAL("clicked()"), self.b3_clicked)

        QtCore.QObject.connect(self.pushButton_10, QtCore.SIGNAL("clicked()"), self.b3_clicked)



        self.comboBox_4 = QtGui.QComboBox(self.executionpanel)
        self.comboBox_4.setGeometry(QtCore.QRect(10, 170, 111, 21))
        self.comboBox_4.setObjectName(_fromUtf8("comboBox_4"))
        self.comboBox_4.addItem(_fromUtf8(""))
        self.comboBox_4.addItem(_fromUtf8(""))
        self.comboBox_4.addItem(_fromUtf8(""))
        self.comboBox_4.addItem(_fromUtf8(""))
        self.comboBox_4.addItem(_fromUtf8(""))
        self.comboBox_3 = QtGui.QComboBox(self.executionpanel)
        self.comboBox_3.setGeometry(QtCore.QRect(10, 100, 111, 21))
        self.comboBox_3.setObjectName(_fromUtf8("comboBox_3"))
        self.comboBox_3.addItem(_fromUtf8(""))
        self.comboBox_3.addItem(_fromUtf8(""))
        self.comboBox = QtGui.QComboBox(self.executionpanel)
        self.comboBox.setGeometry(QtCore.QRect(10, 30, 111, 21))
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.comboBox.addItem(_fromUtf8(""))
        self.comboBox.addItem(_fromUtf8(""))
        self.comboBox.addItem(_fromUtf8(""))
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1101, 21))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)
        self.toolBar = QtGui.QToolBar(MainWindow)
        self.toolBar.setIconSize(QtCore.QSize(100, 40))
        self.toolBar.setObjectName(_fromUtf8("toolBar"))
        MainWindow.addToolBar(QtCore.Qt.TopToolBarArea, self.toolBar)
        self.actionLogo = QtGui.QAction(MainWindow)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(_fromUtf8(":/images/images/Elogo.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionLogo.setIcon(icon)
        self.actionLogo.setObjectName(_fromUtf8("actionLogo"))
        self.actionSettings = QtGui.QAction(MainWindow)
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap(_fromUtf8("images/settings.jpg")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionSettings.setIcon(icon1)
        self.actionSettings.setObjectName(_fromUtf8("actionSettings"))
        self.actionExit = QtGui.QAction(MainWindow)
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap(_fromUtf8(":/images/images/Exit.jpg")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionExit.setIcon(icon2)
        self.actionExit.setObjectName(_fromUtf8("actionExit"))
        self.actionTextConsole = QtGui.QAction(MainWindow)
        self.actionTextConsole.setCheckable(True)

        QtCore.QObject.connect(self.actionTextConsole, QtCore.SIGNAL("triggered()"), self.TextConsoleClick)

        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap(_fromUtf8(":/images/images/TextConsole.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionTextConsole.setIcon(icon3)
        self.actionTextConsole.setObjectName(_fromUtf8("actionTextConsole"))
        self.actionExit_2 = QtGui.QAction(MainWindow)
        self.actionExit_2.setIcon(icon2)
        self.actionExit_2.setObjectName(_fromUtf8("actionExit_2"))
        self.toolBar.addAction(self.actionLogo)
        self.toolBar.addAction(self.actionTextConsole)
        self.toolBar.addAction(self.actionSettings)
        self.toolBar.addAction(self.actionExit_2)

        QtCore.QObject.connect(self.actionExit_2, QtCore.SIGNAL("triggered()"), self.ExitClick)

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(1)
        self.tabWidget_2.setCurrentIndex(0)
        QtCore.QObject.connect(self.pushButton_9, QtCore.SIGNAL(_fromUtf8("clicked()")), self.checkBox_9.toggle)
        QtCore.QObject.connect(self.pushButton_9, QtCore.SIGNAL(_fromUtf8("clicked()")), self.checkBox_8.toggle)
        QtCore.QObject.connect(self.pushButton_9, QtCore.SIGNAL(_fromUtf8("clicked()")), self.checkBox_7.toggle)
        QtCore.QObject.connect(self.pushButton_10, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.checkBox_9.setChecked)
        QtCore.QObject.connect(self.pushButton_10, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.checkBox_8.setChecked)
        QtCore.QObject.connect(self.pushButton_10, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.checkBox_7.setChecked)

        QtCore.QObject.connect(self.pushButton_5, QtCore.SIGNAL(_fromUtf8("clicked()")), self.checkBox_3.toggle)
        QtCore.QObject.connect(self.pushButton_5, QtCore.SIGNAL(_fromUtf8("clicked()")), self.checkBox_2.toggle)
        QtCore.QObject.connect(self.pushButton_5, QtCore.SIGNAL(_fromUtf8("clicked()")), self.checkBox.toggle)

        QtCore.QObject.connect(self.pushButton_6, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.checkBox_3.setChecked)
        QtCore.QObject.connect(self.pushButton_6, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.checkBox_2.setChecked)
        QtCore.QObject.connect(self.pushButton_6, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.checkBox.setChecked)

        QtCore.QObject.connect(self.checkBox, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.c1_clicked)
        QtCore.QObject.connect(self.checkBox_2, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.c1_clicked)
        QtCore.QObject.connect(self.checkBox_3, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.c1_clicked)

        QtCore.QObject.connect(self.pushButton_9, QtCore.SIGNAL("clicked()"), self.c1_clicked)


        QtCore.QObject.connect(self.progressBar, QtCore.SIGNAL(_fromUtf8("valueChanged(int)")), self.label_6.setNum)
        QtCore.QObject.connect(self.progressBar, QtCore.SIGNAL(_fromUtf8("valueChanged(int)")), self.label_4.setNum)
        QtCore.QObject.connect(self.progressBar, QtCore.SIGNAL(_fromUtf8("valueChanged(int)")), self.label_5.setNum)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), _translate("MainWindow", "TestLog", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), _translate("MainWindow", "CommandLog", None))
        self.label_7.setText(_translate("MainWindow", "Test ID\'s", None))
        self.label_8.setText(_translate("MainWindow", "Description", None))
        self.label_4.setText(_translate("MainWindow", "Read", None))
        self.label_10.setText(_translate("MainWindow", "Drive Read Command", None))
        self.label_5.setText(_translate("MainWindow", "Write", None))
        self.label_11.setText(_translate("MainWindow", "Drive Write Command", None))
        self.label_12.setText(_translate("MainWindow", "Drive Inquiry Data", None))
        self.label_6.setText(_translate("MainWindow", "Identify", None))
        self.pushButton_5.setText(_translate("MainWindow", "Select All", None))
        self.pushButton_6.setText(_translate("MainWindow", "Deselect All", None))
        self.label_9.setText(_translate("MainWindow", "Test progress", None))
        self.tabWidget_2.setTabText(self.tabWidget_2.indexOf(self.tab_3), _translate("MainWindow", "SATA", None))
        self.label_22.setText(_translate("MainWindow", "Test Type", None))
        self.label_23.setText(_translate("MainWindow", "Description", None))
        self.label_24.setText(_translate("MainWindow", "Read", None))
        self.label_25.setText(_translate("MainWindow", "Drive Read Command", None))
        self.label_26.setText(_translate("MainWindow", "Write", None))
        self.label_27.setText(_translate("MainWindow", "Drive Write Command", None))
        self.label_28.setText(_translate("MainWindow", "Drive Inquiry Data", None))
        self.label_29.setText(_translate("MainWindow", "Identify", None))
        self.pushButton_9.setText(_translate("MainWindow", "Select All", None))
        self.pushButton_10.setText(_translate("MainWindow", "Deselect All", None))
        self.label_30.setText(_translate("MainWindow", "Test progress", None))
        self.tabWidget_2.setTabText(self.tabWidget_2.indexOf(self.tab_4), _translate("MainWindow", "NVME", None))
        self.label.setText(_translate("MainWindow", "Select Test Suite", None))
        self.label_2.setText(_translate("MainWindow", "Test Execution Method", None))
        self.label_3.setText(_translate("MainWindow", "Select test cases to run", None))
        self.pushButton.setText(_translate("MainWindow", "Run", None))
        self.comboBox_4.setItemText(0, _translate("MainWindow", "User selected test cases", None))
        self.comboBox_4.setItemText(1, _translate("MainWindow", "Only Failed test cases", None))
        self.comboBox_4.setItemText(2, _translate("MainWindow", "Only Blocked test cases", None))
        self.comboBox_4.setItemText(3, _translate("MainWindow", "Only Failed or Blocked test cases", None))
        self.comboBox_4.setItemText(4, _translate("MainWindow", "All", None))
        self.comboBox_3.setItemText(0, _translate("MainWindow", "CLI", None))
        self.comboBox_3.setItemText(1, _translate("MainWindow", "GUI", None))
        self.comboBox.setItemText(0, _translate("MainWindow", "SATA", None))
        self.comboBox.setItemText(1, _translate("MainWindow", "NVMe", None))
        self.comboBox.setItemText(2, _translate("MainWindow", "SCSi", None))
        self.toolBar.setWindowTitle(_translate("MainWindow", "toolBar", None))
        self.actionLogo.setText(_translate("MainWindow", "Logo", None))
        self.actionLogo.setToolTip(_translate("MainWindow", "Company", None))
        self.actionSettings.setText(_translate("MainWindow", "Settings", None))
        self.actionSettings.setToolTip(_translate("MainWindow", "Settings", None))
        self.actionExit.setText(_translate("MainWindow", "Exit", None))
        self.actionExit.setToolTip(_translate("MainWindow", "Exit the App", None))
        self.actionTextConsole.setText(_translate("MainWindow", "TextConsole", None))
        self.actionTextConsole.setToolTip(_translate("MainWindow", "Tests Individual Command", None))
        self.actionExit_2.setText(_translate("MainWindow", "Exit", None))
        self.actionExit_2.setToolTip(_translate("MainWindow", "Exit", None))

        self.progressBar.setValue(0)
        self.progressBar_3.setValue(0)


    def a1_clicked(self):
        sys.exit(0)

    def c1_clicked(self, val):
        self.pushButton.setDisabled(not val)

    def b2_clicked(self):
        self.pushButton.setDisabled(0)


    def b3_clicked(self):
        self.pushButton.setDisabled(1)

        self.progressBar.setValue(0)
        self.checkBox_3.setStyleSheet("QCheckBox:checked {color:red;background-color:white}")
        self.checkBox_2.setStyleSheet("QCheckBox:checked {color:white;background-color:white}")
        self.checkBox.setStyleSheet("QCheckBox:checked {color:white;background-color:white}")

        self.label_4.setText(_translate("MainWindow", "Read", None))

        self.label_5.setText(_translate("MainWindow", "Write", None))
        self.label_6.setText(_translate("MainWindow", "Identify", None))

    def ExitClick(self):
        sys.exit()

    def TextConsoleClick(self):

        self.Dialog = QtGui.QDialog()
        self.ui = Ui_Dialog()
        self.ui.setupUi(self.Dialog)
        self.Dialog.show()



    def b1_clicked(self):
        self.textEdit_2.setText("")
        self.textEdit.setText("")

        globals.retval = 0
        print 'Connecting process'
        self.process = QtCore.QProcess(parent=None)

        self.process.readyReadStandardOutput.connect(self.stdoutReady)
        self.process.readyReadStandardError.connect(self.stderrReady)
        self.process.started.connect(lambda: p('Started!'))
        self.process.finished.connect(lambda: p('Finished!'))
        print 'Starting process'

        self.process.start('python', ['main.py', 'tsuite2.json'])

        '''
        self.process.start('python', ['main.py', 'tsuite2.json', '> ../../Logging/SATA/One.txt'])
        '''
        path = "../../Logging/SATA/SATACmdLog.txt"



        path1 = "../../Logging/SATA/runLog.log"

        if globals.retval != 0:
            self.checkBox_3.setStyleSheet("QCheckBox:checked {color:red;background-color:red}")
        else:
            self.checkBox_3.setStyleSheet("QCheckBox:checked {color:red;background-color:green}")

        if globals.retvalCache != 0:
            self.checkBox_2.setStyleSheet("QCheckBox:checked {color:white;background-color:red}")
        else:
            self.checkBox_2.setStyleSheet("QCheckBox:checked {color:white;background-color:green}")

        if globals.retvalCacheE != 0:
            self.checkBox.setStyleSheet("QCheckBox:checked {color:white;background-color:red}")
        else:
            self.checkBox.setStyleSheet("QCheckBox:checked {color:white;background-color:green}")

        text = open(path).read()
        self.textEdit_2.setText("")
        self.textEdit_2.setPlainText(text)
        text1 = open(path1).read()

        self.progressBar.setValue(100)

        self.label_4.setText(_translate("MainWindow", "Read", None))

        self.label_5.setText(_translate("MainWindow", "Write", None))
        self.label_6.setText(_translate("MainWindow", "Identify", None))

    def append(self, text):
        cursor = self.textEdit.textCursor()
        cursor.movePosition(cursor.End)
        cursor.insertText(text)
        # self.output.ensureCursorVisible()


    def stdoutReady(self):
        text = str(self.process.readAllStandardOutput())
        print text.strip()
        self.append(text)


    def stderrReady(self):
        text = str(self.process.readAllStandardError())
        print text.strip()
        self.append(text)


import Storage_rc
import sys

class MyApp(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(MyApp, self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    exit=app
    window = MyApp()
    window.show()
    sys.exit(app.exec_())