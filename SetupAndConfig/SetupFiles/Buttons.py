import os
import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from cStringIO import StringIO

def window():
   app = QApplication(sys.argv)
   win = QDialog()



   b1 = QPushButton(win)
   b1.setText("SATA")
   b1.move(50,20)
   b1.clicked.connect(b1_clicked)

   b2 = QPushButton(win)
   b2.setText("Nvme")
   b2.move(50,50)
   QObject.connect(b2,SIGNAL("clicked()"),b3_clicked)



   win.setGeometry(100,100,200,100)
   win.setWindowTitle("PyQt")
   win.show()
   sys.exit(app.exec_())

def b1_clicked():
  os.system("python main.py tsuite2.json");
  os.system("python main.py tsuite2.json > ../../Logging/SATA/SATACmdLog.txt");



def b3_clicked():
   os.system("python main.py tsuite3.json");

if __name__ == '__main__':
   window()
