import sys,os,signal
from PyQt4 import QtCore,QtGui

class One(QtCore.QObject):
    def __init__(self):
    #Layout
        QtGui.QWidget.__init__(self)
        self.textBrowser = QtGui.QTextBrowser(self)
        self.lineEdit = QtGui.QLineEdit(self)
        self.startButton = QtGui.QPushButton("Start", self)
        self.stopButton = QtGui.QPushButton("Stop", self)
        self.stopButton.setEnabled(False)
        layout = QtGui.QGridLayout(self)
        layout.setSpacing(8)
        layout.addWidget(self.textBrowser, 0, 0)
        layout.addWidget(self.lineEdit, 1, 0)
        layout.addWidget(self.startButton, 1, 1)
        layout.addWidget(self.stopButton, 1, 2)
        #Connection
        self.connect(self.lineEdit, QtCore.SIGNAL("returnPressed()"), self.startCommand)
        self.connect(self.startButton, QtCore.SIGNAL("clicked()"), self.startCommand)
        self.connect(self.stopButton, QtCore.SIGNAL("clicked()"), self.stopCommand)
        #Process
        self.process = QtCore.QProcess()
        self.connect(self.process, QtCore.SIGNAL("readyReadStdout()"), self.readOutput)
        self.connect(self.process, QtCore.SIGNAL("readyReadStderr()"), self.readErrors)
        self.connect(self.process, QtCore.SIGNAL("processExited()"), self.resetButtons)

    def startCommand(self):
        self.process.start("python main.py tsuite2.json", QtCore.QStringList(["my_file.tex"]))
        StartString="Process started with pid: "+str(self.process.pid())
        print StartString
        self.textBrowser.append(StartString)
        print self.process.ProcessState()
    #self.process.close()
        self.startButton.setEnabled(False)
        self.stopButton.setEnabled(True)
    #self.textBrowser.clear()

    #~ if not self.process.start():
    #~ self.textBrowser.setText(
    #~ QtGui.QString("*** Failed to run %1 ***").arg(self.lineEdit.text())
    #~ )
    #~ self.resetButtons()
    #~ return
    def stopCommand(self):
        self.resetButtons()
        print "Stop process, PID was: ", self.process.pid()
        os.kill(self.process.pid(), signal.SIGKILL)
    #QtCore.QTimer.singleShot(5000, self.process, SLOT("kill()"))

    def readOutput(self):
        print self.process.readStdout()
        self.textBrowser.append(QtGui.QString(self.process .readStdout()))
        if self.process.isRunning()==False:
            self.textBrowser.append("\n Completed Successfully")
    def readErrors(self):
        self.textBrowser.append("error: " + QtGui.QString(self.process.readLineStderr()))
    def resetButtons(self):
        self.startButton.setEnabled(True)
        self.stopButton.setEnabled(False)

    if __name__ == "__main__":
        app = QtGui.QApplication(sys.argv)
        window = One()
        window.show()
        sys.exit(app.exec_())
