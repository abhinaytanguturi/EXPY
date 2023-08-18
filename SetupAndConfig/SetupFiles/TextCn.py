# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'TestConsole.ui'
#
# Created by: PyQt4 UI code generator 4.11.4
#
# WARNING! All changes made in this file will be lost!
import sys
from PyQt4 import QtCore, QtGui

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
'''
class MyApp(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(MyApp, self).__init__(parent)
        self.ui = Ui_Dialog()
        self.ui.setupUi(self)

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    exit=app
    window = MyApp()
    window.show()
    sys.exit(app.exec_())
'''
