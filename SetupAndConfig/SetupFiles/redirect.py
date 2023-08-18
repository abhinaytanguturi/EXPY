from PyQt4 import QtGui, QtCore, uic
import os

def p(x):
    print x

class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        QtGui.QWidget.__init__(self)


        uic.loadUi('redirect.ui', self)



        print 'Connecting process'

        self.process = QtCore.QProcess(self)

        self.process.readyReadStandardOutput.connect(self.stdoutReady)
        self.process.readyReadStandardError.connect(self.stderrReady)
        self.process.started.connect(lambda: p('Started!'))
        self.process.finished.connect(lambda: p('Finished!'))

        print 'Starting process'
        self.process.start('python', ['main.py', 'tsuite2.json'])



    def append(self, text):
        cursor = self.textEdit.textCursor()
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


def main():
    os.system("python main.py tsuite2.json")

    import sys
    app = QtGui.QApplication(sys.argv)

    win = MainWindow()
    win.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
