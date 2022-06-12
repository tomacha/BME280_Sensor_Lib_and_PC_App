##
# @author: Krzysztof Półchłopek
# @date: 22.03.2022
#

from re import ASCII
import serial
from PyQt5 import QtCore

## Constants for selecting receiver operating mode
NORMAL_MODE, COMMAND_MODE = 0, 1

## Class for comunication through UART
class UARTThread(QtCore.QThread):
    
    ## class variable for signal emiting
    data_rec = QtCore.pyqtSignal(object)
    
    ## The Constructor
    # @param com_nbr number of COM port (e.g. "COM10")
    # @param baudrate specifies the baudarte of COM port
    # @param op_mode specifies receiving mode: 0 for normal, 1 for command mode (waiting for \n when receiving data)
    def __init__(self, com_nbr, baudrate, op_mode= 0):
        QtCore.QThread.__init__(self)
        self.ser = serial.Serial(com_nbr, baudrate, timeout = 1000, parity = serial.PARITY_NONE, rtscts  =0)
        self.op_mode = op_mode
        if self.ser.is_open:
            self.started = True
        else:
            self.started = False

    ## Method for checking status of serial port
    def is_opened(self):
        return self.started

    ## Method for receiving data form COM port
    def run(self):
        while self.started:
            if self.ser.in_waiting > 0:
                if self.op_mode == NORMAL_MODE:
                    ret = str(self.ser.read_all())[2:-1]
                elif self.op_mode == COMMAND_MODE:
                    ret = str(self.ser.readline())[2:-1]
                self.data_rec.emit(ret)

    ## Method for changing receiver operating mode
    # @param op_mode specifies receiving mode
    def change_receiver_mode(self,op_mode):
        self.op_mode = op_mode

    ## Method for sending data through COM port
    # @param msg strong message to send   
    def send(self, msg):
        if self.started == True:
            self.ser.write(str.encode(msg, encoding="ASCII"))
            self.ser.flush()


    ## Method for closing serial port and terminating thread
    def closePort(self):
        if self.started == True:
            self.started = False
            self.ser.close()
        self.terminate()
            