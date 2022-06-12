##
# @author: Krzysztof Półchłopek
# @date: 11.06.2022
#

from email.policy import default
from symtable import Symbol
from PyQt5 import QtWidgets, QtGui, uic
from numpy import arange, power
import pyqtgraph as pg
import serial
import sys
import glob
import time

from UartThread import *
from BME_Commands import BME280

## Class for gui display and funcions
#
#   This class supports gui display and configuration of functions which are execuded
#   when user performs specific actions in gui
class MainGUI(QtWidgets.QMainWindow):
    
    ## Class variable storing port status
    __portOpened = False

    ## Class variable for accessing commands, stored configuration and data
    __bme = BME280()

    ## Class variable storing UART
    __UARTPort = None

    ## Class variables for storing and ploting temperature and humidity
    __tempPlotX = list()
    __tempPlotY = list()
    __humPlotX = list()
    __humPlotY = list()
    __pressPlotX = list()
    __pressPlotY = list()

    ## The consrtuctor
    def __init__(self):
        super(MainGUI, self).__init__()
        uic.loadUi('mainWindow.ui', self) # Load the .ui file
        self.show() # Show the GUI

        # update port select box 
        for port in self.list_ports():
            self.portSelectBox.addItem(port, port)

        # attach functions to connection part
        self.portRefreshButton.clicked.connect(self.refresh_portSelectBox_onClick)
        self.connectButton.clicked.connect(self.connect_onClick)
        
        # attach functions to controlling sensor
        self.sendButton.clicked.connect(self.transmitData_onClick)
        self.clearButton.clicked.connect(self.clear_onClick)
        self.InitButton.clicked.connect(self.transmitInit_onClick)
        self.DeinitButton.clicked.connect(self.transmitDeinit_onClick)
        self.ReadMeasurementsButton.clicked.connect(self.transmitGetMeasurements_onClick)

        # init plots
        self.tempGraphWidget.setBackground('w')
        self.humGraphWidget.setBackground('w')
        self.pressGraphWidget.setBackground('w')
        self.tempGraphWidget.showGrid(x=True, y=True)
        self.humGraphWidget.showGrid(x=True, y=True)
        self.pressGraphWidget.showGrid(x=True, y=True)
        styles = {"color": "black", "font-size": "15px", "font-weight":"bold"}
        self.tempGraphWidget.setLabel("left", "Temperature [°C]", **styles)
        self.tempGraphWidget.setLabel("bottom", "Sample number", **styles)
        self.humGraphWidget.setLabel("left", "Humidity [% RH]", **styles)
        self.humGraphWidget.setLabel("bottom", "Sample number", **styles)
        self.pressGraphWidget.setLabel("left", "Pressure [hPa]", **styles)
        self.pressGraphWidget.setLabel("bottom", "Sample number", **styles)
        self.tempPen = pg.mkPen(color=(255, 0, 0))
        self.humPen = pg.mkPen(color=(0, 0, 255))
        self.pressPen = pg.mkPen(color=(0, 255, 0))
        self.tempDataLine =  self.tempGraphWidget.plot(self.__tempPlotX, self.__tempPlotY, pen=self.tempPen)
        self.humDataLine =  self.humGraphWidget.plot(self.__tempPlotX, self.__tempPlotY, pen=self.humPen)
        self.pressDataLine =  self.pressGraphWidget.plot(self.__tempPlotX, self.__tempPlotY, pen=self.pressPen)

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for serial port support ##

    ## Function for listing all avaliable ports 
    def list_ports(self):
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            ports = glob.glob('/dev/ttyS[0-90-9]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')
        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result
    
    ## Function for updating portSelectBox
    def refresh_portSelectBox_onClick(self):
        self.portSelectBox.clear()
        for port in self.list_ports():
            self.portSelectBox.addItem(port, port)

    ## Function for opening/closing the serial port
    def connect_onClick(self):
        if self.__portOpened == False:
            self.__UARTPort = UARTThread(com_nbr= str(self.portSelectBox.currentData()),baudrate= int(self.baudrateLineEdit.text()), op_mode= COMMAND_MODE)
            self.__portOpened = self.__UARTPort.is_opened()
            if self.__portOpened == False:
                print("PORT ERROR")
                return
            else:
                self.__UARTPort.data_rec.connect(self.decode_recv_data)
                self.__UARTPort.start()
                self.connectButton.setText("Disconnect")
        else:
            self.__UARTPort.closePort()
            self.__portOpened = False
            self.connectButton.setText("Connect")

    ## Function for interprating input data
    def decode_recv_data(self, data):
        input = data.split(' ')
        match input[0]:
            case "TEMP":
                self.show_recv_data("Temperature " + input[1][:-2])
                self.temp_lcdNumber.display(float(input[1][:-2]))
                self.plotTemp(len(self.__tempPlotX) + 1,float(input[1][:-2]))
                
            case "HUM":
                self.show_recv_data("Humidity " + input[1][:-2])
                self.hum_lcdNumber.display(float(input[1][:-2]))
                self.plotHum(len(self.__humPlotX) + 1,float(input[1][:-2]))
                
            case "PRESS":
                self.show_recv_data("Pressure " + input[1][:-2])
                self.press_lcdNumber.display(float(input[1][:-2]))
                self.plotPress(len(self.__humPlotX) + 1,float(input[1][:-2]))
            
            case "CLI":
                self.show_recv_data("Status: " + input[1][:-2])
            
            case _:
               self.show_recv_data(data)

         
    ## Function for displaying received data through serial port
    def show_recv_data(self, data):
        input = "uC -> PC: " + data
        self.globalResponse.append(input)
        self.globalResponse.moveCursor(QtGui.QTextCursor.MoveOperation.End)

    ## Function for displaying transmitted data through serial port
    def show_send_data(self, data):
        input = "PC -> uC: " + data
        self.globalResponse.append(input)
        self.globalResponse.moveCursor(QtGui.QTextCursor.MoveOperation.End)

    ## Function for sending data to sensor
    def transmit(self, data):
        if self.__portOpened:
            self.__UARTPort.send(data)

    ## Function for sending and printing to GUI sent data
    def transmit_and_show(self, data):
        if self.__portOpened:
            self.__UARTPort.send(data)
            self.show_send_data(data[:-1])

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for sensor buttons handling ##

    ## Function for transmitting data through serial port
    def transmitData_onClick(self):
        if self.__portOpened:
            data = self.send_lineEdit.text() + '\n'
            self.transmit_and_show(data)

    ## Function for clearing communication display
    def clear_onClick(self):
        self.globalResponse.clear()

    ## Function for sending "Initialize" message 
    def transmitInit_onClick(self):
        if self.__portOpened:
            data = self.getConfig()
            self.transmit_and_show(self.__bme.init(data))
            
    ## Function for sending deinitialization message 
    def transmitDeinit_onClick(self):
        if self.__portOpened:
            self.transmit_and_show(self.__bme.deinit())
            
    ## Function for sending force measurement message
    def transmitForceMeas_onClick(self):
        if self.__portOpened:
            self.transmit_and_show(self.__bme.force_measurement())
            
    ## Function for sending get measurements message 
    def transmitGetMeasurements_onClick(self):
        if self.__portOpened:
            self.transmit_and_show(self.__bme.recv_mesurements())

    ## ---------------------------------------------------------------------------------------------------- ##
    ## Functions for displaying and ploting meaurements##
    
    ## Function to update temperature display
    def updateTempLCD(self, temp):
        self.temp_lcdNumber.display(temp)

    ## Function to update humidity display
    def updateHumLCD(self, hum):
        self.hum_lcdNumber.display(hum)
        
    ## Function to update humidity display
    def updatePressLCD(self, press):
        self.press_lcdNumber.display(press)
    
    ## Function for ploting temperature
    def plotTemp(self, time, temperature):
        self.__tempPlotX.append(time)
        self.__tempPlotY.append(temperature)
        
        self.tempDataLine.setData(self.__tempPlotX[0-self.tempPlotLen.value():], self.__tempPlotY[0-self.tempPlotLen.value():])
        
    ## Function for ploting humidity
    def plotHum(self, time, humidity):
        self.__humPlotX.append(time)
        self.__humPlotY.append(humidity)
        
        self.humDataLine.setData(self.__humPlotX[0-self.humPlotLen.value():], self.__humPlotY[0-self.humPlotLen.value():])
        
    ## Function for ploting pressure
    def plotPress(self, time, pressure):
        self.__pressPlotX.append(time)
        self.__pressPlotY.append(pressure)
        
        self.pressDataLine.setData(self.__humPlotX[0-self.pressPlotLen.value():], self.__pressPlotY[0-self.pressPlotLen.value():])
        
    ## ---------------------------------------------------------------------------------------------------- ##
    ## Helper functions ##
    
    ## Function for reading data form UI
    def getConfig(self):
        data = [self.mode_sel.currentIndex(), self.s_time_sel.currentIndex(),self.filter_sel.currentIndex(),
                self.temp_meas_sel.currentIndex(), self.hum_meas_sel.currentIndex(), self.press_meas_sel.currentIndex()]
        return data
