import sys
import os
from PyQt4 import QtCore, QtGui
from cStringIO import StringIO

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName( "MainWindow" )
        MainWindow.resize(500, 400)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName( "centralwidget" )
        self.verticalLayout = QtGui.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName( "verticalLayout" )
        self.mdiArea = QtGui.QMdiArea(self.centralwidget)
        self.mdiArea.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.mdiArea.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.mdiArea.setActivationOrder(QtGui.QMdiArea.CreationOrder)
        self.mdiArea.setViewMode(QtGui.QMdiArea.TabbedView)
        self.mdiArea.setTabsClosable(True)
        self.mdiArea.setTabsMovable(True)
        self.mdiArea.setObjectName( "mdiArea" )
        self.verticalLayout.addWidget(self.mdiArea)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 508, 21))
        self.menubar.setObjectName( "menubar" )
        self.menuAdd = QtGui.QMenu(self.menubar)
        self.menuAdd.setObjectName( "menuAdd" )
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName( "statusbar" )
        MainWindow.setStatusBar(self.statusbar)
        self.menubar.addAction(self.menuAdd.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(   "MainWindow" )
        self.menuAdd.setTitle( "&Add Form" )

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName( ("Form"))
        Form.resize(400, 800)
        self.gridLayout = QtGui.QGridLayout(Form)
        self.gridLayout.setObjectName( ("gridLayout"))
        self.plainTextEdit = QtGui.QPlainTextEdit(Form)
        self.plainTextEdit.setMinimumSize(QtCore.QSize(0, 731))
        self.plainTextEdit.setObjectName( ("plainTextEdit"))
        self.gridLayout.addWidget(self.plainTextEdit, 0, 0, 1, 1)
        self.buttonBox = QtGui.QDialogButtonBox(Form)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName( ("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 1, 0, 1, 1)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle( "Lengthy subwindow" )
        sys.stdout = StringIO()
        out = sys.stdout.getvalue()
        self.plainTextEdit.setPlainText(out)


class MyApp(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(MyApp, self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

    def Add_Subwindow(self):
        widget = QtGui.QWidget()
        self.subwin_abq = Ui_Form()
        self.subwin_abq.setupUi(widget)
        self.subwindow = QtGui.QMdiSubWindow(self.ui.mdiArea)

        widget.setParent(self.subwindow)

        self.b1 = QtGui.QPushButton("Sata",self)
        self.b1.setParent(self.subwindow)

        self.b1.move(350,20)
        self.b1.clicked.connect(b1_clicked)

        self.b2 = QtGui.QPushButton("Nvme",self)


        self.b2.move(50,50)
        ''' QObject.connect(b2,SIGNAL("clicked()"),b2_clicked) '''

        self.subwindow.setWidget(widget)
        self.subwindow.setWindowTitle("testing")
        self.ui.mdiArea.addSubWindow(self.subwindow)
        widget.show()
        self.subwindow.show()
        self.subwindow.widget().show()
def b1_clicked(self):
  os.system("python main.py tsuite2.json");
  os.system("python main.py tsuite2.json > ../../Logging/SATA/SataCmd.log")

def b2_clicked(self):
   os.system("python main.py tsuite3.json");


if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    window = MyApp()
    window.show()
    window.Add_Subwindow()
    sys.exit(app.exec_())
