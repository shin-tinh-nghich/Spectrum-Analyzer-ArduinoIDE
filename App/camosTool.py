import time
import sys, os
import serial
import serial.tools.list_ports

from PyQt5 import QtWidgets, QtGui
from PyQt5.QtWidgets import QMessageBox, QApplication
from camosToolGui import Ui_Dialog

class Camos_App(QtWidgets.QWidget, Ui_Dialog):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.serial_port = None

        self.init_ui()
        self.setWindowTitle("camos Tool")

    def init_ui(self):
        self.timebutton.clicked.connect(self.set_time)
        self.combutton.clicked.connect(self.refresh_ports)
        self.bandbutton.clicked.connect(self.change_band)
        self.modebutton.clicked.connect(self.change_mode)
        self.textbutton.clicked.connect(self.change_text)
        self.refresh_ports()

    def refresh_ports(self):
        self.comlist.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.comlist.addItem(port.device)  

    def set_time(self):
        selected_port = self.comlist.currentText()
        if not selected_port:
            return

        try:
            self.serial_port = serial.Serial(
                port=selected_port,
                baudrate=115200,
                timeout=0.1
            )
            self.send_time()
        except Exception as e:
            self.show_error_dialog(f"Failed to connect to {selected_port}: {e}")
    def send_time(self):
        if self.serial_port:
            named_tuple = time.localtime()
            named_tuple = time.localtime(time.mktime(named_tuple) + 1)
            get_time = time.strftime("time %b %d %Y-%H:%M:%S-\n", named_tuple)
            self.serial_port.write(bytearray(get_time.encode()))
            self.serial_port.close()
            # Hiển thị thông báo thành công
            QMessageBox.information(self, 'Success', 'Time change successful!')

    def change_band(self):
        selected_port = self.comlist.currentText()
        if not selected_port:
            return

        try:
            self.serial_port = serial.Serial(
                port=selected_port,
                baudrate=115200,
                timeout=0.1
            )
            self.send_band()
        except Exception as e:
            self.show_error_dialog(f"Failed to connect to {selected_port}: {e}")
    def send_band(self):
        if self.serial_port:
            text = 'mode band\n'
            self.serial_port.write(bytearray(text.encode()))
            self.serial_port.close()
            # Hiển thị thông báo thành công
            #QMessageBox.information(self, 'Success', 'Time change successful!')

    def change_mode(self):
        selected_port = self.comlist.currentText()
        if not selected_port:
            return

        try:
            self.serial_port = serial.Serial(
                port=selected_port,
                baudrate=115200,
                timeout=0.1
            )
            self.send_mode()
        except Exception as e:
            self.show_error_dialog(f"Failed to connect to {selected_port}: {e}")
    def send_mode(self):
        if self.serial_port:
            text = 'mode 12345\n'
            self.serial_port.write(bytearray(text.encode()))
            self.serial_port.close()
            # Hiển thị thông báo thành công
            #QMessageBox.information(self, 'Success', 'Time change successful!')

    def change_text(self):
        selected_port = self.comlist.currentText()
        if not selected_port:
            return

        try:
            self.serial_port = serial.Serial(
                port=selected_port,
                baudrate=115200,
                timeout=0.1
            )
            self.send_text()
        except Exception as e:
            self.show_error_dialog(f"Failed to connect to {selected_port}: {e}")
            self.serial_port.close()
    def send_text(self):
        if self.serial_port:
            _text = self.textedit.text()
            if 0 < len(_text) < 50:
                text = 'text ' + _text + '      \n'
                self.serial_port.write(bytearray(text.encode()))
                self.serial_port.close()
                # Hiển thị thông báo thành công
                #QMessageBox.information(self, 'Success', 'Time change successful!')
            else: 
                QMessageBox.information(self, 'Empty string or more than 50 characters')
                self.serial_port.close()

    def show_error_dialog(self, message):
        QMessageBox.critical(self, 'Error', message)

basedir = os.path.dirname(__file__)

try:
    from ctypes import windll  # Only exists on Windows.
    myappid = 'mycompany.myproduct.subproduct.version com.camos'
    windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)
except ImportError:
    pass

if __name__ == "__main__":
    app = QApplication(sys.argv)
    #app = QtWidgets.QApplication(sys.argv)
    app.setWindowIcon(QtGui.QIcon(os.path.join(basedir, 'Kirby.ico')))
    myshow = Camos_App()
    myshow.show()
    sys.exit(app.exec_())