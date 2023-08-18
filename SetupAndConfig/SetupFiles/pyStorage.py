# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Window.ui'
#
# Created: Fri Jul 29 17:50:53 2016
#      by: PyQt4 UI code generator 4.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui
import sys
import os
from random import randint

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
    
class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(711, 587)
        MainWindow.setMinimumSize(QtCore.QSize(698, 587))
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.centralwidget)
        self.verticalLayout_4.setObjectName(_fromUtf8("verticalLayout_4"))
        self.groupBox = QtGui.QGroupBox(self.centralwidget)
        self.groupBox.setTitle(_fromUtf8(""))
        self.groupBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.groupBox.setFlat(False)
        self.groupBox.setCheckable(False)
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.label = QtGui.QLabel(self.groupBox)
        self.label.setGeometry(QtCore.QRect(10, 140, 131, 16))
        self.label.setObjectName(_fromUtf8("label"))
        self.comboBox = QtGui.QComboBox(self.groupBox)
        self.comboBox.setGeometry(QtCore.QRect(10, 160, 181, 20))
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.comboBox.addItem(_fromUtf8(""))
        self.comboBox.addItem(_fromUtf8(""))
        self.comboBox.addItem(_fromUtf8(""))

        self.label_2 = QtGui.QLabel(self.groupBox)
        self.label_2.setGeometry(QtCore.QRect(10, 210, 131, 16))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.comboBox_2 = QtGui.QComboBox(self.groupBox)
        self.comboBox_2.setGeometry(QtCore.QRect(10, 230, 181, 22))
        self.comboBox_2.setObjectName(_fromUtf8("comboBox_2"))
        self.comboBox_2.addItem(_fromUtf8(""))
        self.comboBox_2.addItem(_fromUtf8(""))
        self.comboBox_2.setCurrentIndex(1)
        self.label_3 = QtGui.QLabel(self.groupBox)
        self.label_3.setGeometry(QtCore.QRect(10, 280, 131, 16))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.comboBox_3 = QtGui.QComboBox(self.groupBox)
        self.comboBox_3.setGeometry(QtCore.QRect(10, 300, 181, 22))
        self.comboBox_3.setObjectName(_fromUtf8("comboBox_3"))
        self.comboBox_3.addItem(_fromUtf8(""))
        self.comboBox_3.addItem(_fromUtf8(""))
        self.comboBox_3.addItem(_fromUtf8(""))
        self.comboBox_3.addItem(_fromUtf8(""))
        self.comboBox_3.addItem(_fromUtf8(""))
        self.comboBox_3.setCurrentIndex(4)
        self.pushButton = QtGui.QPushButton(self.groupBox)
        self.pushButton.setGeometry(QtCore.QRect(10, 370, 181, 61))
        self.pushButton.setAutoFillBackground(False)
        self.pushButton.setAutoDefault(False)
        self.pushButton.setDefault(False)
        self.pushButton.setFlat(False)
        self.pushButton.setObjectName(_fromUtf8("pushButton"))

        QtCore.QObject.connect(self.pushButton, QtCore.SIGNAL("clicked()"), self.b1_clicked)

        self.tabWidget = QtGui.QTabWidget(self.groupBox)
        self.tabWidget.setGeometry(QtCore.QRect(230, 20, 421, 501))
        self.tabWidget.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.tabWidget.setElideMode(QtCore.Qt.ElideLeft)
        self.tabWidget.setObjectName(_fromUtf8("tabWidget"))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.verticalScrollBar = QtGui.QScrollBar(self.tab)
        self.verticalScrollBar.setGeometry(QtCore.QRect(510, 0, 17, 491))
        self.verticalScrollBar.setOrientation(QtCore.Qt.Vertical)
        self.verticalScrollBar.setObjectName(_fromUtf8("verticalScrollBar"))
        self.verticalScrollBar_4 = QtGui.QScrollBar(self.tab)
        self.verticalScrollBar_4.setGeometry(QtCore.QRect(460, 0, 16, 491))
        self.verticalScrollBar_4.setOrientation(QtCore.Qt.Vertical)
        self.verticalScrollBar_4.setObjectName(_fromUtf8("verticalScrollBar_4"))
        self.textEdit_2 = QtGui.QTextEdit(self.tab)
        self.textEdit_2.setGeometry(QtCore.QRect(0, 0, 411, 461))
        self.textEdit_2.setObjectName(_fromUtf8("textEdit_2"))
        self.tabWidget.addTab(self.tab, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.verticalScrollBar_2 = QtGui.QScrollBar(self.tab_2)
        self.verticalScrollBar_2.setGeometry(QtCore.QRect(640, 0, 20, 501))
        self.verticalScrollBar_2.setOrientation(QtCore.Qt.Vertical)
        self.verticalScrollBar_2.setObjectName(_fromUtf8("verticalScrollBar_2"))
        self.verticalScrollBar_3 = QtGui.QScrollBar(self.tab_2)
        self.verticalScrollBar_3.setGeometry(QtCore.QRect(510, 0, 16, 491))
        self.verticalScrollBar_3.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.verticalScrollBar_3.setOrientation(QtCore.Qt.Vertical)
        self.verticalScrollBar_3.setObjectName(_fromUtf8("verticalScrollBar_3"))
        self.verticalScrollBar_5 = QtGui.QScrollBar(self.tab_2)
        self.verticalScrollBar_5.setGeometry(QtCore.QRect(460, 0, 16, 491))
        self.verticalScrollBar_5.setOrientation(QtCore.Qt.Vertical)
        self.verticalScrollBar_5.setObjectName(_fromUtf8("verticalScrollBar_5"))
        self.textEdit = QtGui.QTextEdit(self.tab_2)
        self.textEdit.setGeometry(QtCore.QRect(0, 0, 421, 471))
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.tabWidget.addTab(self.tab_2, _fromUtf8(""))
        self.verticalLayout_4.addWidget(self.groupBox)
        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 711, 27))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        if self.comboBox_3.currentIndex() == 0:
            self.UserSelect()
	    self.comboBox_3.currentIndexChanged()
	'''
        file = '..\Logging\SATA\SATACmd.Log'
        path = os.getcwd()+file


        text=open('..\..\Logging\SATA\SATACmd.Log','r').read()

        path="../../Logging/SATA/SATACmdLog.txt"
        text=open(path).read()
        self.textEdit_2.setPlainText(text)
    '''

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.label.setText(_translate("MainWindow", "Select test suite", None))
        self.comboBox.setItemText(0, _translate("MainWindow", "SATA", None))
        self.comboBox.setItemText(1, _translate("MainWindow", "NVMe", None))
        self.comboBox.setItemText(2, _translate("MainWindow", "SCSi", None))
        self.label_2.setText(_translate("MainWindow", "Test execution method", None))
        self.comboBox_2.setItemText(0, _translate("MainWindow", "CLI", None))
        self.comboBox_2.setItemText(1, _translate("MainWindow", "GUI", None))
        self.label_3.setText(_translate("MainWindow", "Test selection policy", None))
        self.comboBox_3.setItemText(0, _translate("MainWindow", "User selected test cases", None))
        self.comboBox_3.setItemText(1, _translate("MainWindow", "Only Failed test cases", None))
        self.comboBox_3.setItemText(2, _translate("MainWindow", "Only Blocked test cases", None))
        self.comboBox_3.setItemText(3, _translate("MainWindow", "Only Failed or Blocked test cases", None))
        self.comboBox_3.setItemText(4, _translate("MainWindow", "All", None))
        self.pushButton.setText(_translate("MainWindow", "START", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), _translate("MainWindow", "Test logs", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), _translate("MainWindow", "Command output", None))



    def b1_clicked(self):
        self.textEdit_2.setText("")

        print 'Connecting process'
        self.process = QtCore.QProcess(parent=None)


        self.process.readyReadStandardOutput.connect(self.stdoutReady)
        self.process.readyReadStandardError.connect(self.stderrReady)
        self.process.started.connect(lambda: p('Started!'))
        self.process.finished.connect(lambda: p('Finished!'))
        print 'Starting process'
        self.process.start('python', ['main.py', 'tsuite2.json'])


        path = "../../Logging/SATA/SATACmdLog.txt"
        path1 = "../../Logging/SATA/runLog.log"

        text = open(path).read()
        self.textEdit.setText("")
        self.textEdit.setPlainText(text)
        text1 = open(path1).read()

        self.textEdit_2.append(text1)
        if self.comboBox_3.currentIndex() == 0:
            self.UserSelect()



    def UserSelect(self):
        self.model = QtGui.QStandardItemModel()
        '''
        for n in range(5):
        '''

        item = QtGui.QStandardItem('Identify')
        item.setForeground(QtGui.QColor(0, 0, 0))
        check = QtCore.Qt.Checked if randint(0, 1) == 1 else QtCore.Qt.Unchecked
        item.setCheckState(check)
        item.setCheckable(True)
        self.model.appendRow(item)

        item = QtGui.QStandardItem('Read')
        item.setForeground(QtGui.QColor(0, 0, 0))
        check = QtCore.Qt.Checked if randint(0, 1) == 1 else QtCore.Qt.Unchecked
        item.setCheckState(check)
        item.setCheckable(True)
        self.model.appendRow(item)

        item = QtGui.QStandardItem('Write')
        item.setForeground(QtGui.QColor(0, 0, 0))
        check = QtCore.Qt.Checked if randint(0, 1) == 1 else QtCore.Qt.Unchecked
        item.setCheckState(check)
        item.setCheckable(True)
        self.model.appendRow(item)


        item = QtGui.QStandardItem('Write Cache')
        item.setForeground(QtGui.QColor(0, 0, 0))
        check = QtCore.Qt.Checked if randint(0, 1) == 1 else QtCore.Qt.Unchecked
        item.setCheckState(check)
        item.setCheckable(True)
        self.model.appendRow(item)

        print (self.model.rowCount())

        self.view = QtGui.QListView()
        self.view.setModel(self.model)
        self.view.show()


    def append(self, text):
        cursor = self.textEdit_2.textCursor()
        cursor.movePosition(cursor.End)
        cursor.insertText(text)
        #self.output.ensureCursorVisible()

    def stdoutReady(self):
        text = str(self.process.readAllStandardOutput())
        print text.strip()
        self.append(text)

    def stderrReady(self):
        text = str(self.process.readAllStandardError())
        print text.strip()
        self.append(text)


class MyApp(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(MyApp, self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    window = MyApp()
    window.show()
    sys.exit(app.exec_())
