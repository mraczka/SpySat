import time as t
import statistics as st
import numpy as np
import random
import matplotlib.pyplot as plt

import serial
import re

x_data = []
y_data = []

y2_data = []


AverageNumber = 10

lastData = []
sum = 0

fig, ax = plt.subplots()
ax.set_xlim(-1, 40)
ax.set_ylim(-60, 20)


fig.canvas.manager.set_window_title('Sheesh window')

plt.xlabel('X - Axis [sec]')
plt.ylabel('Y - Axis [data]')



#plt.axis([-50,50,0,10000])
plt.ion()
plt.plot(x_data, y_data, color='blue', label = "Data")
plt.plot(x_data, y_data, color='orange', label = "Average")

plt.legend()
plt.show()

def Average():
    if(len(lastData) > AverageNumber):
        global sum
        sum -= lastData[0]
        del lastData[0]
        lastData.append(y_data[-1])
        sum += lastData[-1]
    else:
        lastData.append(y_data[-1])
        sum += lastData[-1]  
    y2_data.append(sum/len(lastData))
    plt.plot(x_data, y2_data, color = "orange", label = "Average")
    
    
time = 1

    
    
###################################################################################################

serialPort = serial.Serial(port = "COM5", baudrate=115200,
                           bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

serialString = ""                           # Used to hold data coming over UART

ReceviedSerialData = ""

print(serialPort.name)
while (True):

    
    if(serialPort.in_waiting > 0):
        
        # Read data out of the buffer until a new line is found
        serialString = serialPort.readline()

        # Print the contents of the serial data
        print(serialString.decode('Ascii')) #Type: string
        
        ReceviedSerialData = serialString.decode('Ascii')
        
        if("RSSI" in ReceviedSerialData):
            ReceviedRSSI = int(re.search(r'\d+', ReceviedSerialData).group())
            
            if('-' in ReceviedSerialData):
                ReceviedRSSI = int(str('-') + str(ReceviedRSSI))
                #print(ReceviedRSSI)
            #else:                          
                #print(ReceviedRSSI)
        
        ##May be useful - serialPort.write(b"Thank you for sending data \r\n")


        
            x_data.append(time)
            y_data.append(ReceviedRSSI) 
            plt.plot(x_data, y_data, color='blue', label = "Data")
            Average()
            plt.draw()
            time += 1
            plt.pause(0.1)   
            ax.cla()
                
